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
/*++
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Module Name:  mainwnd.cpp

Abstract:  Implements the main window, the container for the webbrowser

Functions:

Notes: Most of the code resides here. The container and its interaction with the webbrowser control,
        commandbar, statusbar etc.


--*/

#include <windows.h>
#include "mstsax.h"
#include "MainWnd.h"
#include "utils.h"
#include "baseui.h"
#include "connectingdlg.h"
#include "rdrwarndlg.h"
#include "tsperf.h"
#include <wincrypt.h>
#include <creg.hxx>
#include "pchannel.h"

#ifndef		__INCLUDED_PRECONNECTIONLOGIC_HPP__
#include	"PreConnectionLogic.hpp"
#endif		//__INCLUDED_PRECONNECTIONLOGIC_HPP__

#ifndef		__INCLUDED_PROJECTORSTATUS_HPP__
#include	"ProjectorStatus.hpp"
#endif		//__INCLUDED_PROJECTORSTATUS_HPP__

#ifndef		__INCLUDED_CETSCKEEPALIVE_HPP__
#include	"CetscKeepAlive.hpp"
#endif		//__INCLUDED_CETSCKEEPALIVE_HPP__


#ifndef		__INCLUDED_PICTORAPPMESSAGES_HPP__
#include	"PictorAppMessages.hpp"
#endif		//__INCLUDED_PICTORAPPMESSAGES_HPP__


#ifdef DEBUG
WCHAR *pszState[] = 
{
	L"Disconnected",
	L"Connecting",
	L"Connected",
	L"LoggedIn",
	L"Disconnecting"
};

DBGPARAM dpCurSettings = {
    TEXT("CETSC"), {
    TEXT("Error"),TEXT("Main"),TEXT("UI"),TEXT("Settings"),
    TEXT(""),TEXT(""),TEXT(""),TEXT(""),
    TEXT(""),TEXT(""),TEXT(""),TEXT(""),
    TEXT(""),TEXT(""),TEXT(""),TEXT("Verbose") },
    0x1
}; 

#endif

#define CONSOLECMDLINE		L"console"
#define CLXCMDLINE			L"CLXCMDLINE"
#define MAX_PLUGINS			CHANNEL_MAX_COUNT
HINSTANCE			ghInstance;
extern BOOL			gfPictorInKernel;

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPTSTR lpCmdLine, int nCmdShow)
{
    RETAILMSG(1, (L"cetsc.exe started. Cmdline : '%s'\n",lpCmdLine ? lpCmdLine : L"<NULL>"));

	CMainWnd wndMain;
	HWND hwndMain = NULL, hwndDlg = NULL;
	LPWSTR pszServer = NULL;
	BOOL bUsage = TRUE;
	
	ProjectorStatus_t*		pWiFiProjectorStatus = NULL;
	CetscKeepAlive_t*		pCetscKeepAlive = NULL;
	BOOL	IsConnectOkay = FALSE;
	
    ghInstance = hInst;

#ifndef PICTOR
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
		return 0;
#endif // PICTOR

	svsutil_Initialize();
	gpfmdPropSet = svsutil_AllocFixedMemDescr(sizeof(Property), 10);
	if (!gpfmdPropSet)
		goto Cleanup;

	HACCEL hAccel = NULL;
#ifndef PICTOR
	hAccel = LoadAccelerators(ghInstance, MAKEINTRESOURCE(IDR_ACCELERATORS));
	if (!hAccel){
		DEBUGMSG(1, (L"LoadAccelerators failed 0x%08x", GetLastError()));
		goto Cleanup;
	}
#endif // PICTOR

	if (!wndMain.ParseCmdLine(lpCmdLine))
		goto Cleanup;

	hwndMain = wndMain.Create();
	if (!hwndMain)
		goto Cleanup;

	pszServer = wndMain.GetServer();
	if ( (wndMain.IsCmdLineServer()) ||  ( (!wndMain.IsDefaultProfile()) && (pszServer[0] != L'\0') ) )
	{
		bUsage = FALSE;

		hwndDlg = NULL;
		IsConnectOkay = wndMain.Connect();
		if(IsConnectOkay == FALSE)
		{
			PostMessage(wndMain.GetMainWindow(), WM_CLOSE, 0, 0);
		}
	}
#ifndef PICTOR
	else
	{
		IsConnectOkay = true;
		hwndDlg = StartTscUI(hwndMain, ghInstance, wndMain.GetSettings());
		if (hwndDlg)
		{
			bUsage = FALSE;
			wndMain.SetDlgHandle(hwndDlg, TRUE);
		}
		else
		{
			PostMessage(wndMain.GetMainWindow(), WM_CLOSE, 0, 0);
		}
	}
#endif // PICTOR

   	if(wndMain.IsWirelessProjector())
	{
		pWiFiProjectorStatus = new	ProjectorStatus_t();
		pCetscKeepAlive = new	CetscKeepAlive_t(&wndMain);
		if(!pWiFiProjectorStatus || !pCetscKeepAlive )
		{
			DEBUGMSG(ZONE_ERROR, (L"Could not allocate memory for pWiFiProjectorStatus/pCetscKeepAlive "));
		}
	}

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0)
	{
		if ( (!hwndDlg) || 
			( (!TranslateAccelerator(hwndDlg, hAccel, &msg)) && 
			(!IsDialogMessage(hwndDlg, &msg)) )  )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	   
Cleanup: 
	delete	pWiFiProjectorStatus;
	delete	pCetscKeepAlive;
	PictorAppMessages_t::PostClearStatus();
	if (bUsage)
	{
		LPWSTR pszUsage = (LPWSTR )LoadString(ghInstance, IDS_USAGE, NULL, 0);
		LPWSTR pszTitle = (LPWSTR )LoadString(ghInstance, IDS_RDP, NULL, 0);
		if (pszUsage && pszTitle)
		{
			MessageBox(NULL, pszUsage, pszTitle, MB_OK|MB_SETFOREGROUND);
		}
	}

	if (gpfmdPropSet)
		svsutil_ReleaseFixedNonEmpty(gpfmdPropSet);
	svsutil_DeInitialize();

#ifndef PICTOR
	CoUninitialize();
#else
	UnregisterClass(TSC_AX_HOSTWND_CLS, ghInstance);
#endif // PICTOR

    return 0;
}


CMainWnd::CMainWnd()
{
    _hWnd = NULL;
	_hwndDlg = NULL;
	_hWndTscAx = NULL;
	_state = Disconnected;
	_fLoggedIn = FALSE;
	_fDlgUI = FALSE;
	_fDefaultProfile = FALSE;
	_fCmdLineServer = FALSE;
	_pSettings = NULL;
	_pArcDlg  = NULL;
	_dwMaxArcAttempts = MAX_ARC_ATTEMPTS;
	_fAutoReconnecting = FALSE;
	_szServer[0] = L'\0';
	_fCloseRecvd = FALSE;
#ifndef PICTOR
	_bstrClxCmdLine = NULL;
#else
	*szClxCmdLine = L'\0';
#endif // PICTOR
	_fCmdLineConsole = FALSE;
	_fInCloseHandler = FALSE;
	_dwWinStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW;
	_dwScreenWidth = _dwWidth = GetSystemMetrics(SM_CXSCREEN);
	_dwScreenHeight = _dwHeight = GetSystemMetrics(SM_CYSCREEN);
	SystemParametersInfo(SPI_GETWORKAREA, 0, &_rcWorkArea, 0);
	SetRect(&_rcMainWnd, 0, 0, _dwWidth, _dwHeight);
	_pTscCore = NULL;
	_fBbarEnabled = TRUE;
	_fPromptUser = TRUE;
	_fStartFullscreen = TRUE;
	_fIsFullscreen = FALSE;
	_pTaskbarList = NULL;
	StringCchCopy(_szOrigKbdLayout, KL_NAMELENGTH, L"00000409");
	_fIsWirelessProjector = FALSE;
}

CMainWnd::~CMainWnd()
{
}   

PWSTR CMainWnd::SkipWhiteSpace(PWSTR psz)
{
	ASSERT(psz);
	while(iswspace(*psz))
		psz++;
	return (*psz) ? psz : NULL;
}

BOOL CMainWnd::ParseCmdLine(LPWSTR pszCmdLine)
{
	WCHAR szCmdLine[2*MAX_PATH];
	WCHAR szServer[MAX_SERVERNAMELEN] = {0};
	WCHAR szRegSession[MAX_PATH] = {0};
	WCHAR szRdpFile[MAX_PATH] = {0};

	Profile prof[2] = {profileNone, profileNone};
	WCHAR *pszProfile[2] = {0};
	WCHAR *pszCurProfile = NULL;
	
	int nProfIndex = 0;

	if (S_OK != StringCchCopy(szCmdLine, 2*MAX_PATH, pszCmdLine))
		return FALSE;

	WCHAR *pTok = SkipWhiteSpace(szCmdLine);
	while(pTok)
	{
		BOOL bAllowSpace = TRUE;
		if (*pTok == L'/')	//it is a cmd line option
		{
			if (*(++pTok) == L'\0')
				return FALSE;

			BOOL bNewTok = FALSE;

			if (0 == wcsnicmp(pTok, CONSOLECMDLINE, sizeof(CONSOLECMDLINE)/sizeof(WCHAR) - 1))
			{
				_fCmdLineConsole = TRUE;
				bNewTok = TRUE;
				pTok += sizeof(CONSOLECMDLINE)/sizeof(WCHAR) - 1;
			}
			else if (0 == wcsnicmp(pTok, CLXCMDLINE, sizeof(CLXCMDLINE)/sizeof(WCHAR) - 1))
			{
				pTok += (sizeof(CLXCMDLINE)/sizeof(WCHAR) - 1);
				if ( ((*pTok++) != L'=') || (*pTok == L'\0') )
					return FALSE;

				WCHAR *pEnd = pTok;
				while((*pEnd) && (!iswspace(*pEnd)))
					pEnd++;
				*pEnd = L'\0';

#ifndef PICTOR
				_bstrClxCmdLine = SysAllocString(pTok);
				if (!_bstrClxCmdLine)
					return FALSE;
#else
				StringCchCopy(szClxCmdLine, MAX_PATH, pTok);
#endif // PICTOR

				pTok = pEnd+1;

				bNewTok = TRUE;
			}

			if (bNewTok)
			{
				pTok = SkipWhiteSpace(pTok);
				continue;
			}

			switch(towlower(*pTok))
			{
			case L's':
				pszCurProfile = szRegSession;
				pszProfile[nProfIndex] = pszCurProfile;
				prof[nProfIndex++] = profileRegSession;
				break;
			case L'v':
				_fCmdLineServer = TRUE;
				pszCurProfile = szServer;
				bAllowSpace = FALSE;
				break;
			default:
				//TODO show usage
				return FALSE;
			}

			pTok++;
			if ((*pTok == L':') || (*pTok == L'='))
			{
				pTok++;
			}
			else if (iswspace(*pTok))
			{
				pTok = SkipWhiteSpace(pTok);
			}
			else
			{
				return FALSE;
			}
			
			if ((!pTok) || (*pTok == L'\0'))
				return FALSE;
		}
		else	//it is an rdp file
		{
			pszCurProfile = szRdpFile;
			pszProfile[nProfIndex] = pszCurProfile;
			prof[nProfIndex++] = profileRDPFile;
		}

		ASSERT(pszCurProfile);
		
		pszCurProfile[0] = L'\0';
		WCHAR *pEnd = pTok;
		WCHAR *pNext;
		if (*pTok == L'"')
		{
			pTok++;
			if ((pEnd = wcschr(pTok, L'"')) == NULL)
				return FALSE;
			*pEnd = L'\0';

			if (!bAllowSpace)
			{
				WCHAR *pStr = pTok;
				while(*pStr)
				{
					if (iswspace(*pStr++))
						return FALSE;
				}
			}
			pNext = pEnd + 1;	//we changed the '"' above to a NULL and there should be a valid char beyond that.
		}
		else
		{
			while((*pEnd) && (!iswspace(*pEnd)))
				pEnd++;
			pNext = (*pEnd) ? (pEnd + 1) : NULL;
			*pEnd = L'\0';
		}
		if (S_OK != StringCchCat(pszCurProfile, (_fCmdLineServer) ? MAX_SERVERNAMELEN : MAX_PATH, pTok))
			return FALSE;

		pTok = (pNext) ? SkipWhiteSpace(pNext) : NULL;
	}


	//first initialize default settings
	CSettings defSettings;
	if (!InitializeDefaultSettings(&defSettings))
		return FALSE;

	//pick up global settings from registry
	CRegSettings regGlobal;
	regGlobal.Open(NULL, TRUE);

	WCHAR *pszDefault = DEFAULT_RDP_FILE;
	CSettings *pSettings;

	ASSERT(_pSettings == NULL);

	for (int i=0; i<2; i++)
	{
		pSettings = NULL;
		switch(prof[i])
		{	
		case profileRDPFile:
			pSettings = new CFileSettings();
			pszCurProfile = szRdpFile;
			if (0 == wcsicmp(szRdpFile, pszDefault))
				_fDefaultProfile = TRUE;
			break;
		case profileRegSession:
			pSettings = new CRegSettings();
			pszCurProfile = szRegSession;
			if (0 == wcsicmp(szRegSession, DEFAULT_REG_SESSION))
				_fDefaultProfile = TRUE;
			break;
		default:
			if (i == 0)
			{
				pSettings = new CFileSettings();
				pszCurProfile = pszDefault;
				_fDefaultProfile = TRUE;
			}
			break;
		}

		if (!pSettings)
		{
			if (_pSettings)
				break;
			else
				return FALSE;
		}

		LONG lRet = pSettings->Open(pszCurProfile, TRUE);
		if ((lRet != ERROR_SUCCESS) && (!_fDefaultProfile))
		{
			delete pSettings;
			return FALSE;
		}

		VERIFY(S_OK == StringCchCopy(_szProfile, MAX_PATH, pszCurProfile));

		if ((_pSettings) && (!_pSettings->Copy(pSettings, NULL, TRUE)))
		{
			delete pSettings;
			return FALSE;
		}
		else
		{
			_pSettings = pSettings;
		}
	}

	//merge global settings without overwriting existing entries
	if (!_pSettings->Copy(&regGlobal, NULL, FALSE))
		return FALSE;

	//merge default settings without overwriting
	if (!_pSettings->Copy(&defSettings, NULL, FALSE))
		return FALSE;


	//command line settings override everything else
	if (_fCmdLineConsole)
	{
		DWORD dwVal = 1;
		if (!_pSettings->Write(PROP_CONNECTTOSERVERCONSOLE, TscTypeBOOL, (PBYTE )&dwVal, sizeof(DWORD)))
			return FALSE;
	}

	if (szServer[0] != L'\0')
	{
		if (!_pSettings->Write(PROP_SERVER, TscTypeSTRING, (PBYTE )szServer, (wcslen(szServer)+1)*sizeof(WCHAR)))
			return FALSE;
	}

	DWORD dwSize = sizeof(_szServer);
	TscPropType eType;
	_pSettings->Read(PROP_SERVER, &eType, (PBYTE)(L""), (PBYTE )_szServer, &dwSize);
	TrimSpaces(_szServer);
	if (!_pSettings->Write(PROP_SERVER, TscTypeSTRING, (PBYTE )_szServer, (wcslen(_szServer)+1)*sizeof(WCHAR)))
		return FALSE;

	dwSize = sizeof(_fPromptUser);
	_pSettings->Read(PROP_PROMPTUSER, &eType, NULL, (PBYTE)&_fPromptUser, &dwSize);

	CReg regTSC;
	if (regTSC.Open(HKEY_LOCAL_MACHINE, TSC_SETTINGS_REG_ROOT))
	{
		DWORD dwVal = regTSC.ValueDW(PROP_FULLSCREENALWAYS, 0);
		if (!_pSettings->Write(PROP_FULLSCREENALWAYS, TscTypeULONG, (PBYTE )&dwVal, sizeof(DWORD)))
			return FALSE;

		dwVal = regTSC.ValueDW(PROP_DISABLEFILEACCESS, 0);
		if (!_pSettings->Write(PROP_DISABLEFILEACCESS, TscTypeULONG, (PBYTE )&dwVal, sizeof(DWORD)))
			return FALSE;
	}
	return TRUE;
}


VOID CMainWnd::ApplyPassword()
{
	DWORD dwSize = 0;
	TscPropType eType;
	WCHAR pszEmpty[] = L"";

	if ( (!_pSettings->Read(PROP_CLEARTEXTPASSWORD, &eType, (PBYTE)pszEmpty, NULL, &dwSize)) || (dwSize == 0) || (eType != TscTypeBINARY))
		return ;

	PBYTE pbPassword = NULL;
	__try
	{
		pbPassword = (PBYTE )_alloca(dwSize);
	}
	__except(1)
	{
		DEBUGMSG(ZONE_ERROR, (L"Out of memory!\n"));
		return;
	}

	DWORD dwSize1 = dwSize;
	if ( (!_pSettings->Read(PROP_CLEARTEXTPASSWORD, &eType, (PBYTE)pszEmpty, pbPassword, &dwSize)) || (dwSize == 0) || (eType != TscTypeBINARY) )
		return;


	DATA_BLOB blobIn, blobOut;
	blobIn.cbData = dwSize;
	blobIn.pbData = pbPassword;
	blobOut.cbData = 0;
	blobOut.pbData = NULL;
	if (!CryptUnprotectData(&blobIn, NULL, NULL, NULL, NULL, CRYPTPROTECT_UI_FORBIDDEN, &blobOut))
	{
		DEBUGMSG(ZONE_ERROR, (L"Password decryption failed\n"));
		return;
	}

	BOOL fAutoLogon = 0;
	if ((blobOut.cbData > 0) && (((WCHAR *)blobOut.pbData)[0] != L'\0')  )
	{
		fAutoLogon = 1;
		_pTscCore->SetProperty(PROP_CLEARTEXTPASSWORD, TscTypeSTRING, blobOut.cbData, blobOut.pbData);
	}
	_pTscCore->SetProperty(PROP_AUTOLOGON, TscTypeBOOL, sizeof(fAutoLogon), (PBYTE)&fAutoLogon);

	LocalFree(blobOut.pbData);

}


//Fix up settings that dont have a one-to-one mapping for property names
VOID CMainWnd::FixupSettings()
{
	DWORD dwVal, dwDefVal;
	TscPropType eType = TscTypeULONG;
	DWORD dwSize = sizeof(DWORD);

	DWORD dwPerfFlags = 0;

	//Performance Flags
	dwDefVal = 1;
	if ( (_pSettings->Read(PROP_DISABLEWALLPAPER, &eType, (PBYTE)&dwDefVal, (PBYTE)&dwVal, &dwSize)) &&
		 (dwSize == sizeof(DWORD)) && (dwVal == 1)  )
	{
		dwPerfFlags |= TS_PERF_DISABLE_WALLPAPER;
	}

	if ( (_pSettings->Read(PROP_DISABLEFULLWINDOWDRAG, &eType, (PBYTE)&dwDefVal, (PBYTE)&dwVal, &dwSize)) &&
		 (dwSize == sizeof(DWORD)) && (dwVal == 1)  )
	{
		dwPerfFlags |= TS_PERF_DISABLE_FULLWINDOWDRAG;
	}
	
	if ( (_pSettings->Read(PROP_DISABLEMENUANIMS, &eType, (PBYTE)&dwDefVal, (PBYTE)&dwVal, &dwSize)) &&
		 (dwSize == sizeof(DWORD)) && (dwVal == 1)  )
	{
		dwPerfFlags |= TS_PERF_DISABLE_MENUANIMATIONS;
	}

	if ( (_pSettings->Read(PROP_DISABLETHEMES, &eType, (PBYTE)&dwDefVal, (PBYTE)&dwVal, &dwSize)) &&
		 (dwSize == sizeof(DWORD)) && (dwVal == 1)  )
	{
		dwPerfFlags |= TS_PERF_DISABLE_THEMING;
	}

	_pSettings->Write(PROP_PERFORMANCEFLAGS, TscTypeULONG, (PBYTE )&dwPerfFlags, sizeof(DWORD));

	//Remove any MRUx settings in case this rdp file is from the desktop
	for (int i=0; i<10; i++)
	{
		WCHAR szMRU[MAX_PROPNAME_LEN];
		StringCchPrintf(szMRU, MAX_PROPNAME_LEN, L"%s%d", PROP_MRU, i);
		if (!_pSettings->Delete(szMRU))
			break;
	}

	//separate out the servername and port if it is of the form servername:port
	dwSize = sizeof(_szServer);
	DWORD dwPort = DEFAULT_RDP_PORT;
	if ( (_pSettings->Read(PROP_SERVER, &eType, (PBYTE)(L""), (PBYTE)_szServer, &dwSize)) && 
		 (eType == TscTypeSTRING) && (dwSize < sizeof(_szServer)) )
	{
		WCHAR *pszColon = wcschr(_szServer, L':');
		if (pszColon)
		{
			*pszColon++ = L'\0';
			_pSettings->Write(PROP_SERVER, TscTypeSTRING, (PBYTE )_szServer, (wcslen(_szServer)+1)*sizeof(WCHAR));

			dwPort = (DWORD)_wtoi(pszColon);
		}
		else
		{
			dwSize = sizeof(DWORD);
			if (!_pSettings->Read(PROP_RDPPORT, &eType, NULL, (PBYTE)&dwPort, &dwSize))
				dwPort = DEFAULT_RDP_PORT;
		}
	}
	_pSettings->Write(PROP_RDPPORT, TscTypeULONG, (PBYTE )(&dwPort), sizeof(DWORD));	//always write out the port in case a previous session used a different port

	dwSize = sizeof(DWORD);
	if (!_pSettings->Read(PROP_MAXRECONNECTATTEMPTS, &eType, NULL, (PBYTE )&_dwMaxArcAttempts, &dwSize))
		_dwMaxArcAttempts = MAX_ARC_ATTEMPTS;

	//Get the current keyboard layout and then load	the	English	keyboard layout	always,	
	//because we want keyboard layout and key translations to happen on the server side.
	//This is done specifically to fix some Alt+Gr key combinations on hebrew and other languages
	if (GetKeyboardLayoutName(_szOrigKbdLayout))
	{
		WCHAR szLayoutStr[16];
		StringCchPrintf(szLayoutStr, 16, L"0x%s", _szOrigKbdLayout);
		_pSettings->Write(PROP_KEYBOARDLAYOUT, TscTypeSTRING, (PBYTE)szLayoutStr, (wcslen(szLayoutStr)+1)*sizeof(WCHAR));
		
		dwVal =	GetKeyboardType(0);
		_pSettings->Write(PROP_KEYBOARDTYPE, TscTypeULONG, (PBYTE)&dwVal, sizeof(dwVal));
		dwVal =	GetKeyboardType(1);
		_pSettings->Write(PROP_KEYBOARDSUBTYPE,	TscTypeULONG, (PBYTE)&dwVal, sizeof(dwVal));
		dwVal =	GetKeyboardType(2);
		_pSettings->Write(PROP_KEYBOARDFUNCTIONKEY,	TscTypeULONG, (PBYTE)&dwVal, sizeof(dwVal));
	}


	//if the container is going to be in fullscreen mode always, disable the restore button on the bbar
	//and override StartFullScreen
	dwSize = sizeof(DWORD);
	dwDefVal = 0;
	_pSettings->Read(PROP_FULLSCREENALWAYS, &eType, (PBYTE)&dwDefVal, (PBYTE)&dwVal, &dwSize);
	if (dwVal == 1)
	{
		dwVal = 0;
		_pSettings->Write(PROP_BBARSHOWRESTOREBTN, TscTypeBOOL, (PBYTE)&dwVal, dwSize);
		dwVal = 1;
		_pSettings->Write(PROP_STARTFULLSCREEN, TscTypeBOOL, (PBYTE)&dwVal, dwSize);
	}
}

void CMainWnd::ApplyPluginDlls()
{
	WCHAR szReg[MAX_PATH];
	CReg reg;
	DWORD dwCount, i;
	
	WCHAR *pszPluginList = (WCHAR *)LocalAlloc(0, sizeof(WCHAR)*MAX_PATH*MAX_PLUGINS);
	if (!pszPluginList)
		return;

	pszPluginList[0] = L'\0';

	if (S_OK != StringCchPrintf(szReg, MAX_PATH, L"%s%s\\Addins",  TSC_SETTINGS_REG_ROOT, DEFAULT_REG_SESSION))
		goto Cleanup;

	if ( (!reg.Open(HKEY_LOCAL_MACHINE, szReg)) || 
		 (0 == (dwCount = reg.NumSubkeys()) ) )
	{
		DEBUGMSG(ZONE_ERROR, (L"No plugins to load\n"));
		goto Cleanup;
	}

	for (int i=0; i<dwCount; i++)
	{
		if (!reg.EnumKey(szReg, MAX_PATH))
			goto Cleanup;
		
		CReg reg1;
		if (!reg1.Open((HKEY)reg, szReg))
			continue;

		if (!reg1.ValueSZ(L"Name", szReg, MAX_PATH))
			continue;

		if (S_OK != StringCchCat(pszPluginList, MAX_PATH*MAX_PLUGINS, szReg))
			continue;

		if (S_OK != StringCchCat(pszPluginList, MAX_PATH*MAX_PLUGINS, L","))
			continue;
	}

	_pTscCore->SetProperty(PROP_PLUGINDLLS, TscTypeSTRING, (wcslen(pszPluginList)+1)*sizeof(WCHAR), (PBYTE)pszPluginList);
Cleanup:
	LocalFree(pszPluginList);
}

BOOL CMainWnd::ApplySettings()
{
	Property *pProp = NULL;
	BOOL bRet;

	ASSERT(_pSettings);

	ApplyPassword();

	ApplyPluginDlls();

	FixupSettings();

#ifndef PICTOR
	if (_bstrClxCmdLine)
	{
		_pTscCore->SetProperty(PROP_CLXCMDLINE, TscTypeSTRING, (SysStringLen(_bstrClxCmdLine) + 1)*sizeof(WCHAR), (PBYTE)_bstrClxCmdLine);
	}
#else	
	if (szClxCmdLine)
	{
		_pTscCore->SetProperty(PROP_CLXCMDLINE, TscTypeSTRING, (lstrlen(szClxCmdLine) + 1)*sizeof(WCHAR), (PBYTE)szClxCmdLine);
	}
#endif // PICTOR

	bRet = _pSettings->GetPropertySet(&pProp);
	ASSERT(bRet);

	while(pProp)
	{
		if (IsPropertyExcluded(pProp->szName))
		{
			pProp = pProp->pNext;
			continue;
		}
			bRet = SUCCEEDED(_pTscCore->SetProperty(pProp));

		if ( (bRet) && (0 == wcsicmp(pProp->szName, PROP_SERVER)) )
			StringCchCopy(_szServer, MAX_SERVERNAMELEN, (LPWSTR)pProp->uVal.pbVal);

		if ( (bRet) && (0 == wcsicmp(pProp->szName, PROP_BBARENABLED)) )
			_fBbarEnabled = pProp->uVal.dwVal;

		pProp = pProp->pNext;
	}
	
	return TRUE;
}

BOOL CMainWnd::RegisterMainWnd()
{
    WNDCLASS        wc;
    wc.style         = 0;
    wc.lpfnWndProc   = (WNDPROC)CMainWnd::StaticMainWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = ghInstance;
    wc.hIcon         = NULL;	//LoadIcon(ghInstance, MAKEINTRESOURCE(IDI_IE));
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetSysColorBrush(COLOR_WINDOW);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = TSC_AX_HOSTWND_CLS;

#ifdef PICTOR
    RegisterClass(&wc);
#else
    if (!(RegisterClass(&wc)))
        return FALSE;
#endif // PICTOR

    return TRUE;
}


HWND CMainWnd::Create()
{
   if (!RegisterMainWnd())
		return NULL;

    _hWnd = ::CreateWindowEx(0,
							TSC_AX_HOSTWND_CLS,
                            (LPWSTR)LoadString(ghInstance, IDS_RDP, NULL, 0),
							_dwWinStyle,
                            0,
                            0,
                            _dwWidth,
                            _dwHeight,
                            NULL, NULL, ghInstance, 0);
    if (!_hWnd)
        return NULL;

    SetWindowLong(_hWnd, GWL_USERDATA, (DWORD)this);

    if (!(_hWndTscAx = CreateCore()))
        return NULL;

    _hicoSmall = (HICON)LoadImage(ghInstance, MAKEINTRESOURCE(IDI_MSTSC), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
    _hicoBig   = LoadIcon(ghInstance, MAKEINTRESOURCE(IDI_MSTSC));

#ifndef PICTOR
	ITaskbarList *pTaskbar = NULL;
	HRESULT hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList, (void**) &pTaskbar);
	if(SUCCEEDED(hr))
	{
		ASSERT(pTaskbar);
		hr = pTaskbar->QueryInterface( __uuidof(ITaskbarList2), (void**) &_pTaskbarList);
		pTaskbar->Release();
	}
#endif // PICTOR

	return _hWnd;
}


HWND CMainWnd::CreateCore()
{
	HWND hwndCore = NULL;

	_pTscCore = CreateTscCore();
	if (!_pTscCore)
		return NULL;

	HRESULT hr = _pTscCore->CreateCoreWindow(ghInstance, _hWnd, &hwndCore);
	if (SUCCEEDED(hr) && (hwndCore != NULL))
	{
		_pTscCore->RegisterEventSink(this);
		InitSupportedRedirectors();
	}
	return hwndCore;
}


LRESULT CALLBACK CMainWnd::StaticMainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CMainWnd *pMainWnd = (CMainWnd*)GetWindowLong(hwnd, GWL_USERDATA);
	if (pMainWnd)
		return pMainWnd->MainWndProc(message, wParam, lParam);
	else
		return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CMainWnd::MainWndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case MSG_TSC_CLOSE:
    case WM_CLOSE:
		OnClose();
		return 0;

	case MSG_TSC_CONNECT:
		return Connect();
		break;

	case MSG_TSC_DISCONNECT:
		Disconnect();
		break;
	case MSG_TSC_GETRDRSUPPORT:
		return _dwSupportedRedirectors;

	case MSG_TSC_ERROR:
		OnError(wParam);
		return 0;

    case WM_DESTROY:
		OnDestroy();
        return 0;

	case WM_QUERYNEWPALETTE:
	case WM_SETTINGCHANGE:
	case WM_PALETTECHANGED:
	case WM_SYSCOLORCHANGE:
		SendMessage(_hWndTscAx, message, wParam, lParam);
        break;

    case WM_SETFOCUS:
	case WM_EXITMENULOOP:
		::SetFocus(_hWndTscAx);
		break;

	case WM_ENABLE:
		if ((wParam) && (_state >= Connected))
			SetFocus(_hWndTscAx);
		break;

	case WM_ACTIVATE:
		if (LOWORD(wParam) != WA_INACTIVE)
		{
			if (_state >= Connected)
			{
				if (_fIsFullscreen)
					AutoHideTaskbar(TRUE);

				//In case we are coming here after a minimizing the window, this will ensure that
				//_hWndTscAx is reset to its original value
				RECT rcClient;
				GetClientRect(_hWnd, &rcClient);
				OnSize(SIZE_RESTORED, rcClient.right, rcClient.bottom);
			}
			::SetFocus(_hWndTscAx);
		}
		return 0;

	case WM_GETICON:
		return (LRESULT)(wParam == ICON_BIG ? _hicoBig: _hicoSmall);

	case WM_SIZE:
		return OnSize(wParam, LOWORD(lParam), HIWORD(lParam));
		break;

	default:
        break;
    }

    return DefWindowProc(_hWnd, message, wParam, lParam);
}

LRESULT CMainWnd::OnSize(DWORD dwType, DWORD dwCx, DWORD dwCy)
{
	if ((dwType == SIZE_MAXIMIZED) && (_dwWidth == _dwScreenWidth) && (_dwHeight == _dwScreenHeight))
	{
		_pTscCore->GoFullscreen(TRUE);	
		return 0;
	}

	if (_hWndTscAx)
	{
		RECT rcClient;
		if (dwType == SIZE_MINIMIZED)
			SetRect(&rcClient, 0, 0, 0, 0);
		else
			GetClientRect(_hWnd, &rcClient);
		SetWindowPos(_hWndTscAx, HWND_TOP, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
	}

	if (!_fIsFullscreen)
	{
		DWORD cx, cy;
		cx = min(dwCx, (DWORD)_rcMainWnd.right);
		cy = min(dwCy, (DWORD)_rcMainWnd.bottom);
		if ((dwCx != cx) || (dwCy != cy))
			SetWindowPos(_hWnd, HWND_TOP, 0, 0, cx, cy, SWP_NOMOVE|SWP_NOZORDER);
	}

	return 0;
}


BOOL CMainWnd::OnClose()
{
	if (_fInCloseHandler)
		return TRUE;

	BOOL bAllowClose = (_state == Disconnected);

	if (_state == LoggedIn)
	{
		if (!_fAutoReconnecting)
		{
			OnConfirmClose(&bAllowClose);
			if (!bAllowClose)
				return FALSE;
		}
		else if (_fPromptUser)
		{
			WCHAR *pszErrMsg = (WCHAR *)LoadString(ghInstance, IDS_NETWORK_ERROR, NULL,	0);
			WCHAR *pszTitle	= (WCHAR *)LoadString(ghInstance, IDS_DISCONNECTED_CAPTION,	NULL, 0);
			if (pszErrMsg && pszTitle)
			{
				MessageBox(_hwndDlg, pszErrMsg,	pszTitle, MB_OK|MB_ICONINFORMATION|MB_SETFOREGROUND);
			}
		}
	}

	_fInCloseHandler = TRUE;	//Set this to TRUE before calling any of the TscCore methods


	if ((_state == Connected) && (TRUE == Disconnect()))
	{
		_fInCloseHandler = FALSE;
		return TRUE;
	}

	//Set this to TRUE before calling RequestClose because this variable is checked 
	//in OnConfirmClose when called from RequestClose
	_fCloseRecvd = TRUE;
	DEBUGMSG(ZONE_VERBOSE, (L"OnClose...Received close\n"));


	if ((!bAllowClose) && (_state > Disconnecting))
	{
		HRESULT hr = _pTscCore->RequestClose();
		bAllowClose = (SUCCEEDED(hr));
	}

	if (!bAllowClose)
	{
		_fInCloseHandler = FALSE;
		return FALSE;
	}


	StopAutoReconnect();	//In case the close button on the bbar was hit without cancelling the arc dialog
	AutoHideTaskbar(FALSE);

	if (_fDlgUI)
	{
		CloseTscUI();
		_hwndDlg = NULL;
	}

	if (_pSettings)
	{
		CommitGlobalSettings();
		if (_fLoggedIn)
		{
			if (_fDefaultProfile)
			{	
				//Dont save the servername or the port if starting from the default profile
				_pSettings->Delete(PROP_SERVER);
				_pSettings->Delete(PROP_RDPPORT);
			}

			if (_fCmdLineConsole)
			{	//if a console session was specified on the command line, dont save it to settings
				_pSettings->Delete(PROP_CONNECTTOSERVERCONSOLE);
			}
			
			if (IsCmdLineServer())
				_pSettings->Delete(PROP_RDPPORT);	//written in the MRU along with the server name

			//Delete the password if it is not supposed to be saved
			DWORD dwVal, dwDefVal=0, dwSize = sizeof(DWORD);
			TscPropType eType;
			if ( (!_pSettings->Read(PROP_SAVEPASSWORD, &eType, (PBYTE)&dwDefVal, (PBYTE)&dwVal, &dwSize)) ||
				(dwSize != sizeof(DWORD)) || (dwVal != 1)  )
			{
				_pSettings->Delete(PROP_CLEARTEXTPASSWORD);
				_pSettings->Delete(PROP_SAVEPASSWORD);
			}

			DeleteFixedupSettings(_pSettings);

			_pSettings->Commit();
		}

		_pSettings->Close();
		delete _pSettings;
		_pSettings = NULL;
	}

	_pTscCore->Destroy();
	DeleteTscCore(_pTscCore);
	_pTscCore = NULL;

	if (_hWnd)
	{
		DestroyWindow(_hWnd);
		_hWnd = NULL;
	}

#ifndef PICTOR
	if (_bstrClxCmdLine)
	{
		SysFreeString(_bstrClxCmdLine);
		_bstrClxCmdLine = NULL;
	}
#endif // PICTOR

	_fInCloseHandler = FALSE;

	return TRUE;
}

VOID CMainWnd::OnError(DWORD dwErrCode)
{
	WCHAR *pszErrMsg = (WCHAR *)LoadString(ghInstance, dwErrCode, NULL,	0);
	WCHAR *pszTitle	= (WCHAR *)LoadString(ghInstance, IDS_DISCONNECTED_CAPTION,	NULL, 0);
	if (pszErrMsg && pszTitle)
	{
		MessageBox(NULL, pszErrMsg,	pszTitle, MB_OK|MB_ICONINFORMATION|MB_SETFOREGROUND|MB_TOPMOST);
	}

	if (!_fDlgUI)
	{
		if(_hwndDlg)
		{
			EndDialog(_hwndDlg,	IDOK);
			_hwndDlg = NULL;
		}

		PostMessage(_hWnd, WM_CLOSE, 0,	0L);
	}
}

VOID CMainWnd::OnDestroy()
{
	DestroyIcon (_hicoBig);
	DestroyIcon (_hicoSmall);
	if (_pTaskbarList)
	{
		_pTaskbarList->DeleteTab(_hWnd);
		_pTaskbarList->Release();
		_pTaskbarList = NULL;
	}
	PostQuitMessage(0);
}

BOOL CMainWnd::Connect()
{
	BOOL		ReturnValue = FALSE;
	CRdrWarnDlg rdrWarn(_hWnd, ghInstance, _pSettings);
	PreConnectionLogic_t*		pPreconnection = NULL;
	if (_state != Disconnected)
	{
		DEBUGMSG(ZONE_ERROR, (L"Control not yet disconnected. Current state=%d\n", _state));
		goto Cleanup;
	}

	if (!ApplySettings())
	{
		DEBUGMSG(ZONE_ERROR, (L"ApplySettings failed\n"));
		goto Cleanup;
	}

	pPreconnection = new	PreConnectionLogic_t();

	if( pPreconnection && pPreconnection->IsPictorConnection())
		{
		_fIsWirelessProjector = TRUE;
		if(pPreconnection->IsValid())
			{
			PictorAppMessages_t::PostStatusConnecting();
			//Set the prefix reverse property
			_pTscCore->SetProperty(L"ConnectModeString", TscTypeSTRING, 22, (PBYTE)L"TCPREVERSE");
			//set connected socket handle
			_pTscCore->SetProperty(L"ConnectedSocketHandle", TscTypePtrULONG, sizeof(ULONG_PTR), (PBYTE)pPreconnection->GetSocket());
			//Check for alive every 15 second - same as collab
			DWORD		KeepAliveInterval = 15;
			_pTscCore->SetProperty(L"KeepAliveInterval", TscTypeULONG, sizeof(DWORD), (PBYTE)&KeepAliveInterval);
			}
		else
			{
			RETAILMSG(1,(L"ERROR in Preconnection phase."));
			
			//Send message to pictorapp to display appropriate message
			PictorAppMessages_t::PostConnectionFailed();
			goto Cleanup;
			}
		}

	if ( (_dwSupportedRedirectors & (RDRSUPPORT_DRIVE|RDRSUPPORT_PORT)) && (rdrWarn.PromptRequired()) && (rdrWarn.DoModal() == IDCANCEL) )
	{
		goto Cleanup;
	}

	if (_fCloseRecvd)
	{
		//If somebody sent us a WM_CLOSE when the modal dialog was up, that message will have been processed
		//in the message loop of the modal dialog. So dont continue any further if that happened.
		ASSERT(!_pTscCore);
		goto Cleanup;
	}

	_fLoggedIn = FALSE;	//reset _fLoggedIn in case we are coming here after an auto-reconnect failure

	//not part of settings, but always written
	BOOL fVal;
	fVal = TRUE;
	_pTscCore->SetProperty(PROP_CONTAINERHANDLESFULLSCREEN, TscTypeBOOL, sizeof(DWORD), (PBYTE)&fVal);

	TscPropType eType;
	DWORD dwSize;
	dwSize = sizeof(DWORD);
	_pSettings->Read(PROP_STARTFULLSCREEN, &eType, (PBYTE)&fVal, (PBYTE)&_fStartFullscreen, &dwSize);

	if (_fStartFullscreen)
	{
		_dwWidth = _dwScreenWidth;
		_dwHeight = _dwScreenHeight;
	}
	else
	{
		RECT rc;
		GetClientRect(_hWnd, &rc);
		dwSize = sizeof(DWORD);
		_pSettings->Read(PROP_DESKTOPWIDTH, &eType, (PBYTE)&(rc.right), (PBYTE)&_dwWidth, &dwSize);

		dwSize = sizeof(DWORD);
		_pSettings->Read(PROP_DESKTOPHEIGHT, &eType, (PBYTE)&(rc.bottom), (PBYTE)&_dwHeight, &dwSize);
	}

	SetRect(&_rcMainWnd, 0, 0, _dwWidth, _dwHeight);

	if (!_fStartFullscreen)
	{
		AdjustWindowRectEx(&_rcMainWnd, _dwWinStyle, FALSE, 0);
		OffsetRect(&_rcMainWnd, -_rcMainWnd.left, -_rcMainWnd.top);

		IntersectRect(&_rcMainWnd, &_rcMainWnd, &_rcWorkArea);
	}

	SetWindowPos(_hWnd, HWND_TOP, 0, 0, _rcMainWnd.right, _rcMainWnd.bottom, SWP_NOMOVE);

	HRESULT hr;
	hr = _pTscCore->Connect();
	if (SUCCEEDED(hr))
	{
		DEBUGMSG(ZONE_MAIN, (L"Connection initiated\n"));
		SetState(Connecting);
		
		if (!_fDlgUI && !_fIsWirelessProjector)
		{
			CConnectingDlg dlg(_hWnd, ghInstance, this, _szServer);
			dlg.DoModal();
		}

		ReturnValue = TRUE;
		goto Cleanup;
	}

	if (_fPromptUser)
	{
		LPWSTR pszMessg, pszTitle;
		pszMessg = (LPWSTR )LoadString(ghInstance, IDS_ERR_CONNECTCALLFAILED, NULL, 0);
		pszTitle = (LPWSTR )LoadString(ghInstance, IDS_RDP, NULL, 0);
		ASSERT(pszMessg && pszTitle);
		MessageBox(NULL, pszMessg, pszTitle, MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
	}

Cleanup:
	delete	pPreconnection;
	pPreconnection = NULL;
	if (_fDlgUI && (FALSE == ReturnValue))
	{
		ASSERT((_hwndDlg) && ::IsWindow(_hwndDlg));
		PostMessage(_hwndDlg, MSG_TSC_DISCONNECTED, 0, 0);
	}
	return ReturnValue;
}

BOOL CMainWnd::Disconnect()
{
	if (_state < Connecting)
	{
		DEBUGMSG(ZONE_ERROR, (L"Control not connected. Current state=%d\n", _state));
		return TRUE;
	}

	SetState(Disconnecting);

	//If this call succeeds, it can potentially call OnDisconnected before it returns
	//OnDisconnected sets the value of _state to Disconnected.

	HRESULT hr = _pTscCore->Disconnect();

	if (S_OK != hr)
	{
		DEBUGMSG(ZONE_ERROR, (L"Call to disconnect failed. _state reset to 'Disconnected'\n"));
		return FALSE;
	}
	return TRUE;
}

State
CMainWnd::
GetState(
	)
{
	return	_state;
}

VOID CMainWnd::SetState(State state)
{
	DEBUGMSG(ZONE_MAIN, (L"Changing state from %s to %s\n", pszState[_state], pszState[state]) );
	_state = state;
}	

void CMainWnd::StopAutoReconnect()
{
	if (!_pArcDlg)
		return;

	KillTimer(_hWnd, (UINT_PTR)(this));
	_pArcDlg->StopAutoReconnect();
	delete _pArcDlg;
	_pArcDlg = NULL;
	_fAutoReconnecting = FALSE;
}

void CMainWnd::UpdateSettings()
{
	DWORD vtBool;
	DWORD dwSize = sizeof(vtBool);
	WCHAR szBuf[1024];

	if (SUCCEEDED(_pTscCore->GetProperty(PROP_BBARPINNED, TscTypeBOOL, &dwSize, (PBYTE)&vtBool)) )
	{
		DWORD dwVal = (vtBool != VARIANT_FALSE);
		_pSettings->Write(PROP_BBARPINNED, TscTypeBOOL, (PBYTE )&dwVal, sizeof(DWORD));
	}

	dwSize = sizeof(szBuf);
	if (SUCCEEDED(_pTscCore->GetProperty(PROP_USERNAME, TscTypeSTRING, &dwSize, (PBYTE)szBuf)) )
	{
		_pSettings->Write(PROP_USERNAME, TscTypeSTRING, (PBYTE)szBuf, (wcslen(szBuf) + 1)*sizeof(WCHAR));
	}

	dwSize = sizeof(szBuf);
	if (SUCCEEDED(_pTscCore->GetProperty(PROP_DOMAIN, TscTypeSTRING, &dwSize, (PBYTE)szBuf)) )
	{
		_pSettings->Write(PROP_DOMAIN, TscTypeSTRING, (PBYTE)szBuf, (wcslen(szBuf) + 1)*sizeof(WCHAR));
	}
}


const LPWSTR CMainWnd::GetServer()
{
	return _szServer;
}

BOOL
CMainWnd::
IsWirelessProjector(
	)
{
	return	_fIsWirelessProjector;
}


BOOL CMainWnd::UpdateServersMRU()
{
	ASSERT(_pSettings);

	WCHAR *pszServers = NULL;
	WCHAR pszEmpty[] = L"";
	DWORD dwSize = sizeof(DWORD);
	TscPropType eType = TscTypeSTRING;
	DWORD dwTotalSize = 0;
	DWORD dwPort = DEFAULT_RDP_PORT;
	WCHAR szServer[MAX_SERVERNAMELEN];
	WCHAR szPort[16];

	ASSERT(_pSettings);

	_pSettings->Read(PROP_RDPPORT, &eType, NULL, (PBYTE)&dwPort, &dwSize);
	_itow(dwPort, szPort, 10);

	dwSize = 0;
	_pSettings->Read(PROP_MRU, &eType, (PBYTE)pszEmpty, NULL, &dwSize);

	if (S_OK != StringCchPrintf(szServer, MAX_SERVERNAMELEN, L"%s%s%s", _szServer, (dwPort == DEFAULT_RDP_PORT) ? L"" : L":", (dwPort == DEFAULT_RDP_PORT) ? L"" : szPort) )
		return FALSE;

	int nLen = wcslen(szServer);	
	DEBUGMSG(ZONE_MAIN, (L"Server : %s\n", szServer));

	__try
	{
		pszServers = (WCHAR *)_alloca(dwSize + ((nLen + 2)*sizeof(WCHAR)) )  ;	//+2 for the trailing semi-colon and trailing NULL
	}
	__except(1)
	{
		DEBUGMSG(ZONE_ERROR, (L"Size of servers list too long(%d)\n", dwSize));
		return FALSE;
	}


	memcpy(pszServers, szServer, nLen*sizeof(WCHAR));
#ifdef PICTOR
    if (dwSize)
#endif PICTOR
    {
	pszServers[nLen] = L';';
	pszServers[nLen + 1] = L'\0';
    }

	dwTotalSize = (nLen+1)*sizeof(WCHAR);

	DWORD dwSize1 = dwSize;
	WCHAR *pszOldMru = pszServers + nLen + 1;

	_pSettings->Read(PROP_MRU, &eType, (PBYTE)pszEmpty, (PBYTE)pszOldMru, &dwSize1);
	DEBUGMSG(ZONE_MAIN, (L"Old MRU list: %s\n", pszOldMru));

	if (dwSize1 != dwSize)
	{
		return FALSE;
	}

	if (dwSize != 0)
	{
		ASSERT(dwSize >= 3*sizeof(WCHAR));	//The smallest server name will be stored as "a;"

		ASSERT(pszOldMru[(dwSize/2) - 1] == L'\0');
		ASSERT(pszOldMru[(dwSize/2) - 2] == L';');

		dwTotalSize += dwSize; 

		WCHAR *pszFound = NULL;
		WCHAR *pszStart = pszOldMru;
		WCHAR *pszEnd = pszStart + wcslen(pszStart);
		while ((pszStart < pszEnd) && ((pszFound = wcsstr(pszStart, szServer)) != NULL))
		{
			if (pszFound + nLen >= pszEnd)
				return FALSE;

			if (pszFound[nLen] == L';')
				break;

			WCHAR *pszSemiColon = wcschr(pszFound, L';');
			ASSERT(pszSemiColon);
			pszStart = pszSemiColon + 1;
			pszFound = NULL;
		}

		if (pszFound)
		{
			ASSERT(pszFound[nLen] == L';');
			memcpy(pszFound, pszFound + nLen + 1, dwSize - ( ((pszFound - pszOldMru) + nLen + 1) * sizeof(WCHAR) )  );
			dwTotalSize -= (nLen+1)*sizeof(WCHAR); 
		}
		else
		{
			//this is a new server we are adding to the MRU list. Make sure the list isnt longer than MAX_MRUCOUNT
			PWSTR pwszSep = pszServers;
			PWSTR pwszEnd = pszServers + wcslen(pszServers);
			int nMruCount = 1;
			while ((pwszSep < pszEnd) && (NULL != (pwszSep = wcschr(pwszSep, L';'))))
			{
				if ((nMruCount++ >= MAX_MRUCOUNT) && (pwszSep + 1 < pszEnd))
				{
					*(pwszSep+1) = L'\0';
					dwTotalSize = (pwszSep - pszServers + 1)*sizeof(WCHAR);
					break;
				}
				pwszSep++;
			}
		}
	}
	else
	{
		dwTotalSize += sizeof(WCHAR);
	}

	DEBUGMSG(ZONE_MAIN, (L"Writing new MRU list: %s\n", pszServers));
#ifdef PICTOR
    if (dwSize)
#endif // PICTOR
    if (!_pSettings->Write(PROP_MRU, TscTypeSTRING, (PBYTE)pszServers, dwTotalSize ) )
	{
		return FALSE;
	}

	return TRUE;	
}

BOOL CMainWnd::CommitGlobalSettings()
{
	ASSERT(_pSettings);

	UpdateSettings();

	CRegSettings regGlobal;
	if (!regGlobal.Copy(_pSettings, PROP_MRU, TRUE))
		return FALSE;
	
	if (!regGlobal.Copy(_pSettings, PROP_BBARPINNED, TRUE))
		return FALSE;

	regGlobal.Commit();

	return TRUE;
}

void CMainWnd::InitSupportedRedirectors()
{
	ASSERT(_pTscCore);
	
	DWORD vtBool = VARIANT_TRUE;

	_dwSupportedRedirectors = 0;
	_dwSupportedRedirectors |= (SUCCEEDED(_pTscCore->SetProperty(PROP_REDIRECTSMARTCARDS, TscTypeBOOL, sizeof(DWORD), (PBYTE)&vtBool)))		? RDRSUPPORT_SCARD		: 0;
	_dwSupportedRedirectors |= (SUCCEEDED(_pTscCore->SetProperty(PROP_REDIRECTDRIVES, TscTypeBOOL, sizeof(DWORD), (PBYTE)&vtBool)))			? RDRSUPPORT_DRIVE		: 0;
	_dwSupportedRedirectors |= (SUCCEEDED(_pTscCore->SetProperty(PROP_REDIRECTPRINTERS, TscTypeBOOL, sizeof(DWORD), (PBYTE)&vtBool)))		? RDRSUPPORT_PRINTER	: 0;
	_dwSupportedRedirectors |= (SUCCEEDED(_pTscCore->SetProperty(PROP_REDIRECTPORTS, TscTypeBOOL, sizeof(DWORD), (PBYTE)&vtBool)))			? RDRSUPPORT_PORT		: 0;

	vtBool = 0;
	_dwSupportedRedirectors |= (SUCCEEDED(_pTscCore->SetProperty(PROP_AUDIOREDIRECTIONMODE, TscTypeULONG, sizeof(DWORD), (PBYTE)&vtBool)))	? RDRSUPPORT_AUDIO		: 0;
	_dwSupportedRedirectors |= (IsClipbdRedirectorPresent()) ? RDRSUPPORT_CLIPBD : 0;
}

BOOL CMainWnd::IsPropertyExcluded(PWSTR pszPropName)
{
	//Properties exluded during the call to ApplySettings
	WCHAR *pszExcluded[] = {
		PROP_DISABLEWALLPAPER,				//The "perf" settings are merged into one single "PerformanceFlags" property and applied
		PROP_DISABLEFULLWINDOWDRAG,
		PROP_DISABLEMENUANIMS,
		PROP_DISABLETHEMES,
		PROP_CLEARTEXTPASSWORD,				//the password is applied separately and not as a part of ApplySettings
		PROP_CONTAINERHANDLESFULLSCREEN,	//container handles fullscreen
		PROP_FULLSCREENALWAYS,				//if the container always remains in fullscreen mode
		PROP_MRU,							//Only used by the container
		PROP_SAVEPASSWORD,					//Only used by the container
		PROP_MAXRECONNECTATTEMPTS,			//Autoreconnect is driven by the container, and so this property is relevant only to the container
		PROP_BITMAPPERSISTCACHELOCATION,	//This is read directly from the registry
		PROP_DISABLEFILEACCESS,				//Whether to disable the Open and Save  
		PROP_AUTOLOGON,						//Dont apply the AutoLogon value. It is implicit with the presence of a password
		PROP_PROMPTUSER						//Whether the user should be prompted with message boxes
	};

	DWORD dwCount = sizeof(pszExcluded)/sizeof(pszExcluded[0]);
	for (DWORD i=0; i<dwCount; i++)
	{
		if (0 == wcsicmp(pszExcluded[i], pszPropName))
			return TRUE;
	}

	return FALSE;
}

void CMainWnd::AutoHideTaskbar(BOOL fHide)
{
	if (_pTaskbarList)
		_pTaskbarList->MarkFullscreenWindow(_hWnd, fHide);
}

#ifdef PICTOR
BOOL WINAPI IOControl(
	  DWORD dwInst,
	  DWORD dwIoControlCode,
	  LPVOID lpInBuf,
	  DWORD nInBufSize,
	  LPVOID lpOutBuf,
	  DWORD nOutBufSize,
	  LPDWORD lpBytesReturned
);

DWORD LaunchCetsc(LPVOID lpParameter);

BOOL WINAPI DllMain(HANDLE hInstDll, DWORD fdwReason, LPVOID pReserved)
{
    return TRUE;
}

BOOL WINAPI IOControl(
	  DWORD dwInst,
	  DWORD dwIoControlCode,
	  LPVOID lpInBuf,
	  DWORD nInBufSize,
	  LPVOID lpOutBuf,
	  DWORD nOutBufSize,
	  LPDWORD lpBytesReturned
)
{
	if (dwIoControlCode == 0x2004EE05)
	{
		DWORD dwThreadId;
		if (::CreateThread(NULL, 0, LaunchCetsc, lpInBuf, 0, &dwThreadId))
			return TRUE;
	}

	return FALSE;
}

DWORD LaunchCetsc(LPVOID lpParameter)
{
	gfPictorInKernel = TRUE;
	WinMain(GetModuleHandle(NULL), NULL, _T("/v \\windows\\default.rdp"), 0);
	return 0;
}


#endif // PICTOR
