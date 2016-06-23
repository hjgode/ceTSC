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
#include <windows.h>
#include "mstsax.h" //HGO
#include "baseui.h"
#include "utils.h"
#include "mstsc.h"
#include "propnames.h"

struct ErrInfo
{
	DWORD	dwErrCode;
	DWORD	dwErrStrId;
};

static ErrInfo gErrMain[] = 
{
	{	disconnectReasonNoInfo,							IDS_INTERNAL_ERROR						},	
//	{	disconnectReasonLocalNotError,						
//	{	disconnectReasonRemoteByUser,					
	{	disconnectReasonByServer,						IDS_DISCONNECT_REMOTE_BY_SERVER			},	
	{	disconnectReasonDNSLookupFailed,				IDS_BAD_SERVER_NAME						},
	{	disconnectReasonOutOfMemory,					IDS_LOW_MEMORY							},
	{	disconnectReasonConnectionTimedOut,				IDS_ERR_CONNECTCALLFAILED				},
	{	disconnectReasonSocketConnectFailed,			IDS_NETWORK_ERROR						},			
	{	disconnectReasonOutOfMemory2,					IDS_LOW_MEMORY							},
	{	disconnectReasonHostNotFound,					IDS_BAD_SERVER_NAME						},	
	{	disconnectReasonWinsockSendFailed,				IDS_NETWORK_ERROR						},		
	{	disconnectReasonOutOfMemory3,					IDS_LOW_MEMORY							},
	{	disconnectReasonInvalidIPAddr,					IDS_BAD_SERVER_NAME						},	
	{	disconnectReasonSocketRecvFailed,				IDS_NETWORK_ERROR						},
	{	disconnectReasonInvalidSecurityData,			IDS_SECURITY_ERROR						},
	{	disconnectReasonInternalError,					IDS_INTERNAL_ERROR						},
	{	disconnectReasonInvalidEncryption,				IDS_ENCRYPTION_ERROR					},
	{	disconnectReasonDNSLookupFailed2,				IDS_BAD_SERVER_NAME						},		
	{	disconnectReasonGetHostByNameFailed,			IDS_BAD_SERVER_NAME						},
	{	disconnectReasonInvalidServerSecurityInfo,		IDS_SECURITY_ERROR						},
	{	disconnectReasonTimerError,						IDS_LOW_MEMORY							},
	{	disconnectReasonTimeoutOccurred,				IDS_CONNECTION_TIMEOUT					},
	{	disconnectReasonServerCertificateUnpackErr,		IDS_SECURITY_ERROR						},				
	{	disconnectReasonInvalidIP,						IDS_BAD_SERVER_NAME						},
	{	disconnectReasonLicensingFailed,				IDS_LICENSING_NEGOT_FAILED				},		
	{	disconnectReasonInternalSecurityError,			IDS_SECURITY_ERROR						},						
	{	disconnectReasonAtClientWinsockFDCLOSE,			IDS_NETWORK_ERROR						},
	{	disconnectReasonLicensingTimeout,				IDS_LICENSING_TIMEDOUT					},				
	{	disconnectReasonInternalSecurityError2,			IDS_SECURITY_ERROR						},
	{	disconnectReasonEncryptionError,				IDS_ENCRYPTION_ERROR					},
	{	disconnectReasonDecryptionError,				IDS_ENCRYPTION_ERROR					},
	{	disconnectReasonClientDecompressionError,		IDS_ENCRYPTION_ERROR					}
};

#define NUM_MAIN_ERRORS		(sizeof(gErrMain)/sizeof(ErrInfo))

static ErrInfo gErrExt[] = 
{
	{	exDiscReasonAPIInitiatedDisconnect,				IDS_DISCONNECT_REMOTE_BY_SERVER_TOOL	},
	{	exDiscReasonAPIInitiatedLogoff,					IDS_LOGOFF_REMOTE_BY_SERVER				},		
	{	exDiscReasonServerIdleTimeout,					IDS_DISCONNECT_IDLE_TIMEOUT				},
	{	exDiscReasonServerLogonTimeout,					IDS_DISCONNECT_LOGON_TIMEOUT			},
	{	exDiscReasonReplacedByOtherConnection,			IDS_DISCONNECT_BYOTHERCONNECTION		},
	{	exDiscReasonOutOfMemory,						IDS_SERVER_OUT_OF_MEMORY				},
	{	exDiscReasonServerDeniedConnection,				IDS_SERVER_DENIED_CONNECTION			},
	{	exDiscReasonLicenseInternal,					IDS_LICENSE_INTERNAL					},
	{	exDiscReasonLicenseNoLicenseServer,				IDS_LICENSE_NO_LICENSE_SERVER			},
	{	exDiscReasonLicenseNoLicense,					IDS_LICENSE_NO_LICENSE					},
	{	exDiscReasonLicenseErrClientMsg,				IDS_LICENSE_BAD_CLIENT_MSG				},
	{	exDiscReasonLicenseHwidDoesntMatchLicense,		IDS_LICENSE_HWID_DOESNT_MATCH_LICENSE	},
	{	exDiscReasonLicenseErrClientLicense,			IDS_LICENSE_BAD_CLIENT_LICENSE			},
	{	exDiscReasonLicenseCantFinishProtocol,			IDS_LICENSE_CANT_FINISH_PROTOCOL		},
	{	exDiscReasonLicenseClientEndedProtocol,			IDS_LICENSE_CLIENT_ENDED_PROTOCOL		},
	{	exDiscReasonLicenseErrClientEncryption,			IDS_LICENSE_BAD_CLIENT_ENCRYPTION		},
	{	exDiscReasonLicenseCantUpgradeLicense,			IDS_LICENSE_CANT_UPGRADE_LICENSE		},
	{	exDiscReasonLicenseNoRemoteConnections,			IDS_LICENSE_NO_REMOTE_CONNECTIONS		}
};

#define NUM_EXT_ERRORS		(sizeof(gErrExt)/sizeof(ErrInfo))


DWORD GetErrorStringId(DWORD dwMainErr, DWORD dwExtErr)
{
	for (int i=0; i<NUM_EXT_ERRORS; i++)
	{
		if (dwExtErr == gErrExt[i].dwErrCode)
			return gErrExt[i].dwErrStrId;
	}

	for (int i=0; i<NUM_MAIN_ERRORS; i++)
	{
		if (dwMainErr == gErrMain[i].dwErrCode)
			return gErrMain[i].dwErrStrId;
	}
	return IDS_INTERNAL_ERROR;
}

BOOL ValidateServerName(LPWSTR pszServerName)
{
	WCHAR *pszOrig = pszServerName;
	unsigned int nOrigLen = wcslen(pszOrig)+1;

	//skip leading spaces
	while ((*pszServerName == L' ') || (*pszServerName == L'\t'))
	{
		pszServerName++;
	}
	
	unsigned int nLen = wcslen(pszServerName);

	//remove trailing spaces
	while ( (nLen > 0) && ((pszServerName[nLen - 1] == L' ') || (pszServerName[nLen - 1] == L'\t')) )
	{
		pszServerName[nLen - 1] = L'\0';
		nLen --;
	}
	
	if (*pszServerName == L'\0')
	{
		return FALSE;
	}

	//check for invalid chars
	if (wcscspn(pszServerName, INVALID_CHARS) < nLen)
	{
		return FALSE;
	}


	if (S_OK != StringCchCopy(pszOrig, nOrigLen, pszServerName))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL InitializeDefaultSettings(ISettings *pSettings)
{
    BOOL bRet = TRUE;
    DWORD dwVal = 1;
    bRet = bRet && pSettings->Write(PROP_BBARENABLED,                   TscTypeBOOL,        (PBYTE )&dwVal, sizeof(DWORD));
    bRet = bRet && pSettings->Write(PROP_BBARPINNED,                    TscTypeBOOL,        (PBYTE )&dwVal, sizeof(DWORD));
    bRet = bRet && pSettings->Write(PROP_REDIRECTPRINTERS,              TscTypeBOOL,        (PBYTE )&dwVal, sizeof(DWORD));
    bRet = bRet && pSettings->Write(PROP_REDIRECTSMARTCARDS,            TscTypeBOOL,        (PBYTE )&dwVal, sizeof(DWORD));
    bRet = bRet && pSettings->Write(PROP_ENABLEAUTORECONNECT,           TscTypeBOOL,        (PBYTE )&dwVal, sizeof(DWORD));
    
    dwVal = 0;
    bRet = bRet && pSettings->Write(PROP_REDIRECTDRIVES,                TscTypeBOOL,        (PBYTE )&dwVal, sizeof(DWORD));
    bRet = bRet && pSettings->Write(PROP_REDIRECTPORTS,                 TscTypeBOOL,        (PBYTE )&dwVal, sizeof(DWORD));
    bRet = bRet && pSettings->Write(PROP_AUDIOREDIRECTIONMODE,          TscTypeULONG,       (PBYTE )&dwVal, sizeof(DWORD));
    bRet = bRet && pSettings->Write(PROP_FULLSCREENALWAYS,              TscTypeBOOL,        (PBYTE )&dwVal, sizeof(DWORD));
    bRet = bRet && pSettings->Write(PROP_BITMAPPERSISTENCE,             TscTypeBOOL,        (PBYTE )&dwVal, sizeof(DWORD));
    bRet = bRet && pSettings->Write(PROP_BBARSHOWPINBTN,                TscTypeBOOL,        (PBYTE )&dwVal, sizeof(DWORD));

    dwVal = 1;
    bRet = bRet && pSettings->Write(PROP_COMPRESS,                      TscTypeBOOL,        (PBYTE )&dwVal, sizeof(DWORD));
    bRet = bRet && pSettings->Write(PROP_STARTFULLSCREEN,               TscTypeBOOL,        (PBYTE )&dwVal, sizeof(DWORD));
    bRet = bRet && pSettings->Write(PROP_KEYBOARDHOOKMODE,              TscTypeULONG,       (PBYTE )&dwVal, sizeof(DWORD));

    dwVal = 20;
    bRet = bRet && pSettings->Write(PROP_MAXRECONNECTATTEMPTS,          TscTypeULONG,       (PBYTE )&dwVal, sizeof(DWORD));


    dwVal = 1;
    bRet = bRet && pSettings->Write(PROP_DISABLEWALLPAPER,              TscTypeBOOL,        (PBYTE )&dwVal, sizeof(DWORD));
    bRet = bRet && pSettings->Write(PROP_DISABLEFULLWINDOWDRAG,         TscTypeBOOL,        (PBYTE )&dwVal, sizeof(DWORD));
    bRet = bRet && pSettings->Write(PROP_DISABLEMENUANIMS,              TscTypeBOOL,        (PBYTE )&dwVal, sizeof(DWORD));

    dwVal = 0;
    bRet = bRet && pSettings->Write(PROP_DISABLETHEMES,                 TscTypeBOOL,        (PBYTE )&dwVal, sizeof(DWORD));
    bRet = bRet && pSettings->Write(PROP_AUTOLOGON,                     TscTypeBOOL,        (PBYTE )&dwVal, sizeof(DWORD));


    dwVal = GetSystemMetrics(SM_CXSCREEN);
    bRet = bRet && pSettings->Write(PROP_DESKTOPWIDTH,                  TscTypeULONG,       (PBYTE )&dwVal, sizeof(DWORD));
    
    dwVal = GetSystemMetrics(SM_CYSCREEN);
    bRet = bRet && pSettings->Write(PROP_DESKTOPHEIGHT,                 TscTypeULONG,       (PBYTE )&dwVal, sizeof(DWORD));

    HDC hdc = GetDC(NULL);
    if (!hdc)
        return FALSE;

    dwVal = GetDeviceCaps(hdc, BITSPIXEL);
    ReleaseDC(NULL, hdc);
    dwVal = min(4, (dwVal / 4) - 1); //New ColorDepthIDs : 4bpp=0, 8bpp=1, 15bpp=2, 16bpp=3, 24bpp=4
    bRet = bRet && pSettings->Write(PROP_COLORDEPTH,                    TscTypeULONG,       (PBYTE )&dwVal, sizeof(DWORD));

    return bRet;
}

//Delete any fixed up properties and properties that shouldnt be in an rdp file
void DeleteFixedupSettings(ISettings *pSettings)
{
	WCHAR *pszPropDelete[] = 
	{
		PROP_MRU,
		PROP_BBARPINNED,
		PROP_FULLSCREENALWAYS,
		PROP_PERFORMANCEFLAGS,
		PROP_BITMAPPERSISTCACHELOCATION,
		PROP_AUTOLOGON
	};

	//Delete any fixed up properties and properties that shouldnt be in an rdp file
	for (int i=0; i<sizeof(pszPropDelete)/sizeof(pszPropDelete[0]); i++)
	{
		pSettings->Delete(pszPropDelete[i]);
	}
}

void TrimSpaces(WCHAR *pszStr)
{
	WCHAR *pszStart = pszStr;
	if (!pszStart)
		return;

	while(iswspace(*pszStart))
		pszStart++;

	WCHAR *pszEnd = pszStart + wcslen(pszStart);
	if (pszEnd > pszStart)
	{
		pszEnd--;	//Move pszEnd to point to the character before the NULL
		while (iswspace(*pszEnd))
			*pszEnd-- = L'\0';
		pszEnd++;	//Move it back to point to the NULL
	}

	memmove(pszStr, pszStart, (pszEnd - pszStart + 1)*sizeof(WCHAR));
}
