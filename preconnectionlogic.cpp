//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//

#ifndef		__INCLUDED_PRECONNECTIONLOGIC_HPP__
#include	"PreConnectionLogic.hpp"
#endif		//__INCLUDED_PRECONNECTIONLOGIC_HPP__

//from rdpencsplprot.h

#ifndef _WS2TCPIP_H_
#include	<ws2tcpip.h>
#endif //_WS2TCPIP_H_


typedef int (*PFNWSASTARTUP)(WORD, LPWSADATA);
typedef int (*PNFWSACLEANUP)(void);
typedef int (*PNFSEND)(SOCKET, const char FAR*, int, int);
typedef int (*PNFCLOSESOCKET)(SOCKET);
typedef int (*PFNCONNECT)(SOCKET, const struct sockaddr FAR*, int);
typedef int (*PFNGETADDRINFO)(const char FAR*, const char FAR*, const struct addrinfo FAR*, struct addrinfo FAR* FAR*);
typedef SOCKET (*PFNSOCKET)(int, int, int);
typedef int (*PFNWSAGETLASTERROR)(void);

PFNWSASTARTUP       g_pfnWSAStartUp;
PNFWSACLEANUP       g_pfnWSACleanUp;
PNFSEND             g_pfnSend;	
PNFCLOSESOCKET      g_pfnCloseSocket;
PFNCONNECT          g_pfnConnect;
PFNGETADDRINFO      g_pfnGetAddrInfo;
PFNSOCKET           g_pfnSocket;
PFNWSAGETLASTERROR  g_pfnWSAGetLastError;


struct	PreConnectionPacket_t
{
	int               cbSize;    
	int                Flags;     
	int                Version;
	int                Id;
};

static	const	int	 s_WSA_MAX_MAJOR = 0x02;
static	const	int	 s_WSA_MAX_MINOR = 0x00;
const	WCHAR*		PreConnectionLogic_t::s_PreconnectionFileName = L"\\windows\\preconnection.txt";



class	AddrInfoHint_t
{
private:
	ADDRINFO    m_Hints;

public:
	AddrInfoHint_t(
		)
		{
		m_Hints.ai_addr = NULL;
		m_Hints.ai_addrlen = 0;
		m_Hints.ai_canonname = NULL;
		m_Hints.ai_next     = NULL;
		m_Hints.ai_family   = (int)AF_UNSPEC;
		m_Hints.ai_flags    = 0;
		m_Hints.ai_protocol = IPPROTO_TCP;
		m_Hints.ai_socktype = SOCK_STREAM;
		}

	bool
	IsValidAddress(
		const	char*	pAddressBuffer,
		const	char*	pPortBuffer,
		ADDRINFO**		ppAddressInfo
		)
		{
		return( g_pfnGetAddrInfo(pAddressBuffer, pPortBuffer, &m_Hints, ppAddressInfo) == 0 ); //successfull
		}
};




PreConnectionLogic_t::
PreConnectionLogic_t(
	):m_PreConnectionSocket(INVALID_SOCKET),
	m_IsPictorConnection(false),
	m_CollabConnectFailedEvent( NULL ),
	m_IsValid(false)
{

	m_CollabConnectFailedEvent = OpenEvent( EVENT_ALL_ACCESS, FALSE, L"CollabConnectFailedEvent" );

	WSADATA	WSAdata;
	WORD		VersionRequested = MAKEWORD(s_WSA_MAX_MAJOR, s_WSA_MAX_MINOR);


	m_hWS2Lib = LoadLibrary(L"ws2.dll");
	if (m_hWS2Lib == NULL)
		{
		return;
		}

	g_pfnWSAStartUp = (PFNWSASTARTUP)GetProcAddress(m_hWS2Lib, L"WSAStartup");
	g_pfnWSACleanUp = (PNFWSACLEANUP)GetProcAddress(m_hWS2Lib, L"WSACleanup");
	g_pfnSend = (PNFSEND)GetProcAddress(m_hWS2Lib, L"send");
	g_pfnCloseSocket = (PNFCLOSESOCKET)GetProcAddress(m_hWS2Lib, L"closesocket");
	g_pfnConnect = (PFNCONNECT)GetProcAddress(m_hWS2Lib, L"connect");
	g_pfnSocket = (PFNSOCKET)GetProcAddress(m_hWS2Lib, L"socket");
	g_pfnGetAddrInfo = (PFNGETADDRINFO)GetProcAddress(m_hWS2Lib, L"getaddrinfo");
	g_pfnWSAGetLastError = (PFNWSAGETLASTERROR)(m_hWS2Lib, L"WSAGetLastError");

	if (!g_pfnWSAStartUp || !g_pfnWSACleanUp || !g_pfnSend || !g_pfnCloseSocket ||
		!g_pfnConnect || !g_pfnSocket || !g_pfnGetAddrInfo || !g_pfnWSAGetLastError)
		{
		FreeLibrary(m_hWS2Lib);
		m_hWS2Lib = NULL;
		return;
		}

	// WinSock version negotiation. Use 2.0
	if(g_pfnWSAStartUp(VersionRequested, &WSAdata) == 0 )
		{
		//Read preconnection file
		if( ReadPreconnectionFile() )
			{
			m_IsPictorConnection = true;
			if( CreateSocket() )
				{
				m_IsValid = SendPreConnectionPacket();
				}
			}
		}
}

PreConnectionLogic_t::
~PreConnectionLogic_t(
	)
{
	if (m_hWS2Lib)
		{
		g_pfnWSACleanUp();
		FreeLibrary(m_hWS2Lib);
		}
}


bool
PreConnectionLogic_t::
IsValid(
	)
{
	return m_IsValid;
}


SOCKET
PreConnectionLogic_t::
GetSocket(
	)
{
	return m_PreConnectionSocket;
}

bool
PreConnectionLogic_t::
IsPictorConnection(
	)
{
	return	m_IsPictorConnection;
}



bool
PreConnectionLogic_t::
SendPreConnectionPacket(
	)
{
	uint		SessionID = s_RDPENCSPL_SESSION_ID_INVALID;
	PreConnectionPacket_t		ConnectionBlob;
	
	swscanf( m_SessionID.get_buffer(), L"%d", &SessionID );
	if (SessionID != s_RDPENCSPL_SESSION_ID_INVALID ) 
		{
		ConnectionBlob.cbSize  = sizeof(ConnectionBlob);
		ConnectionBlob.Flags   = 0;
		ConnectionBlob.Version = s_RDPENCSPL_CONNECTION_BLOB_V1;
		ConnectionBlob.Id      = SessionID;

		//todo - check the return value and print output accordingly
		int	BytesSent = g_pfnSend( m_PreConnectionSocket, 
				(char*)&ConnectionBlob, 
				sizeof(ConnectionBlob), 
				0 );

		if(SOCKET_ERROR == BytesSent)
			{
			RETAILMSG(1, (L"Error sending preconnection packet. Last Error %d", g_pfnWSAGetLastError()));
			return	false;
			}

		if(sizeof(ConnectionBlob) != BytesSent)
			{
			RETAILMSG(1, (L"Error sending preconnection packet. Sent % bytes rather than %d", BytesSent, sizeof(ConnectionBlob)));
			return	false;
			}
		}

	return true;
}

bool
PreConnectionLogic_t::
CreateSocket(
	)
{
	//find connection info for server
	AddressPortMap_t::iterator	CurrentPair = m_AddressPortMap.begin();

	ADDRINFO*		pAddressInfo = NULL;

	while( CurrentPair != m_AddressPortMap.end() )
		{
		int		AddressBufferLen = (CurrentPair->first.length() + 1)* (sizeof(WCHAR) );
		char*	pAddressBuffer = new char [AddressBufferLen];

		if( WideCharToMultiByte(CP_ACP,
					0,
					(WCHAR*)CurrentPair->first.get_buffer(),
					-1,
					pAddressBuffer,
					AddressBufferLen,
					NULL,
					NULL
					) )
			{
			int		PortBufferLen = (CurrentPair->second.length() + 1)* (sizeof(WCHAR) );
			char*	pPortBuffer = new char [PortBufferLen];

			//todo - check return value of wcstombs
			if( WideCharToMultiByte(CP_ACP,
						0,
						(WCHAR*)CurrentPair->second.get_buffer(),
						-1,
						pPortBuffer,
						PortBufferLen,
						NULL,
						NULL
						) )
				{
				AddrInfoHint_t		AddrInfoHint;
				
				if( AddrInfoHint.IsValidAddress(pAddressBuffer, pPortBuffer, &pAddressInfo) ) //successfull
					{
					SOCKET	PreConnectionSocket = g_pfnSocket(pAddressInfo->ai_family,
						pAddressInfo->ai_socktype,
						pAddressInfo->ai_protocol);

					//if successfully retrieved info, create a socket using the same ai_family, socket type and protocol
					if( g_pfnConnect(
							PreConnectionSocket, 
							pAddressInfo->ai_addr, 
							(int) pAddressInfo->ai_addrlen
							) == 0)
						{
						m_PreConnectionSocket = PreConnectionSocket;
						return true;
						}
					else
						{
						g_pfnCloseSocket( PreConnectionSocket );
						}
					}
				}
			}
		CurrentPair++;
		}

	return false;
}


bool
PreConnectionLogic_t::
ReadPreconnectionFile(
	)
{
	WCHAR		Buffer1[s_MAX_BUFFERLEN];
	WCHAR		Buffer2[s_MAX_BUFFERLEN];
	FILE*	pPreconnectionFile = NULL;
	bool	ReadSuccessfully = false;

	Buffer1[0] = 0;
	Buffer2[0] = 0;
	
	pPreconnectionFile = _wfopen(s_PreconnectionFileName, L"rb");

	if( pPreconnectionFile )
		{
		//Read SID
		if( fgetws( Buffer1, s_MAX_BUFFERLEN, pPreconnectionFile ) )
			{
				m_SessionID = Buffer1;
			}

		//If able to read SID - it is a valid preconnection file
		if( !m_SessionID.empty() )
			{
			while( fgetws( Buffer1, s_MAX_BUFFERLEN, pPreconnectionFile ) )
				{
				if( fgetws( Buffer2, s_MAX_BUFFERLEN, pPreconnectionFile ))
					{
					m_AddressPortMap.insert(ce::wstring(Buffer1), ce::wstring(Buffer2));
					}
				}
			if( m_AddressPortMap.size() )
				{
				ReadSuccessfully = true;
				}
			}
		fclose(pPreconnectionFile);
		DeleteFile(s_PreconnectionFileName);
		}
	
	return ReadSuccessfully;
}


void
PreConnectionLogic_t::
SingalConnectFailed(
	)
{
	SetEvent( m_CollabConnectFailedEvent );
	return;
}



