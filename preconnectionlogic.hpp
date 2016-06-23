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
#define		__INCLUDED_PRECONNECTIONLOGIC_HPP__

#include	<windows.h>
#include	<winsock2.h>

#ifndef __STRING__
#include	"string.hxx" //HGO
#endif //__STRING__

#ifndef __HASH_MAP__
#include	"hash_map.hxx"
#endif //__HASH_MAP__

typedef		ce::hash_map<ce::wstring, ce::wstring>		AddressPortMap_t;

class	PreConnectionLogic_t
{

public:
	static	const	int		s_RDPENCSPL_CONNECTION_BLOB_V1 = 0x01;
	static	const	int		s_RDPENCSPL_SESSION_ID_INVALID = 0;
	static	const	WCHAR*		s_PreconnectionFileName;
private:
	static	const	int		s_MAX_BUFFERLEN = 200;
	static	const	WCHAR*		s_pPictorAppWndClassName;
	
	SOCKET		m_PreConnectionSocket;
	ce::wstring	m_SessionID;
	AddressPortMap_t		m_AddressPortMap;
	
	bool		m_IsPictorConnection;
	bool			m_IsValid;
	HINSTANCE	m_hWS2Lib;
	HANDLE		m_CollabConnectFailedEvent;
public:
	PreConnectionLogic_t(
		);

	~PreConnectionLogic_t(
		);

	bool
	IsValid(
		);

	SOCKET
	GetSocket(
		);

	bool
	IsPictorConnection(
		);

	void
	SingalConnectFailed(
		);
private:
	bool
	SendPreConnectionPacket(
		);

	bool
	CreateSocket(
		);
	
	bool
	ReadPreconnectionFile(
		);
};

#endif	//__INCLUDED_PRECONNECTIONLOGIC_HPP__

