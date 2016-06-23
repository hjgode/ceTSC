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

Module Name:  mainwnd.h

Abstract:  Implements the main window, the container for the webbrowser

Functions:

Notes: This class implements the container and its interaction with the webbrowser control,
        commandbar, statusbar etc.


--*/

#ifndef _MAINWND_H_
#define _MAINWND_H_

#include <windows.h>
#include <shlobj.h>

#include "shobjidl_cetsc.h" //HGO

#include "settings.h"
#include "arcdlg.h"
#include "mstsc.h"
#include "tsccore.h"
#include "svsutil.h"

#ifdef DEBUG
extern WCHAR *pszState[];
#endif

#ifndef WS_OVERLAPPEDWINDOW
#define WS_OVERLAPPEDWINDOW (WS_OVERLAPPED     | \
                             WS_CAPTION        | \
                             WS_SYSMENU        | \
                             WS_THICKFRAME     | \
                             WS_MINIMIZEBOX    | \
                             WS_MAXIMIZEBOX)
#endif

//HGO
#ifndef KL_NAMELENGTH
#define KL_NAMELENGTH 256
#endif

class CMainWnd : public TscCoreEvents
{

public:
    CMainWnd();
    ~CMainWnd();

public:
	BOOL					Connect();
	BOOL					Disconnect();
	State					GetState();
	
public:
//    BOOL                    PreTranslateMessage(LPMSG pMsg);
    HWND					Create();
	
	LRESULT					OnSize(DWORD dwType, DWORD dwCx, DWORD dwCy);
	BOOL					OnClose();
	VOID					OnDestroy();
	VOID					OnError(DWORD dwErrCode);
    
	HWND                    GetMainWindow()		{   return _hWnd;				}
	HWND					GetDlgHandle()		{	return _hwndDlg;			}
	BOOL					IsDefaultProfile()	{	return _fDefaultProfile;	}
	BOOL					IsCmdLineServer()	{	return _fCmdLineServer;		}

	void					UpdateSettings();
	BOOL					ParseCmdLine(LPWSTR pszCmdLine);
	CSettings *				GetSettings()		{	return _pSettings;	}
	VOID					FixupSettings();				
	BOOL					ApplySettings();
	void					AutoHideTaskbar(BOOL fHide);


    static LRESULT  CALLBACK StaticMainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT					MainWndProc(UINT message, WPARAM wParam, LPARAM lParam);

	static void CALLBACK	StaticArcTimerCallback(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);
	void					ArcTimerCallback(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);


	const LPWSTR			GetServer();

	VOID  SetDlgHandle(HWND hwndDlg, BOOL fDlgUI=FALSE)	
	{	
		_hwndDlg = hwndDlg;	
		_fDlgUI = (hwndDlg) ? fDlgUI : FALSE;	
	}

	BOOL		IsWirelessProjector();
protected:

    BOOL                    RegisterMainWnd();
	HWND					CreateCore();
	
	void					SetState(State state);
	void					StopAutoReconnect();	
	
	BOOL					UpdateServersMRU();
	BOOL					CommitGlobalSettings();

	void					ApplyPassword();
	void					ApplyPluginDlls();
	void					InitSupportedRedirectors();
	BOOL					IsPropertyExcluded(PWSTR pszPropName);	//should the property be applied to the control?



protected:		//event handlers
	virtual HRESULT OnConnected();
	virtual HRESULT OnDisconnected(LONG lReason, ExtendedDisconnectReasonCode	extReason);
	virtual HRESULT OnLoginComplete();
	virtual HRESULT OnFatalError(LONG lCode);
	virtual HRESULT OnWarning(LONG lCode);
	virtual HRESULT OnRequestContainerMinimize();
	virtual HRESULT OnConfirmClose(BOOL* pfConfirm);
	virtual HRESULT OnRequestFullScreen(BOOL fFullscreen);
	virtual HRESULT OnAutoReconnecting(long attemptCount, BOOL *pfManual);
	virtual HRESULT OnShutdown();

protected:	//helper functions
	PWSTR	SkipWhiteSpace(PWSTR psz);

protected:
	HICON					_hicoBig;
	HICON					_hicoSmall;
    HACCEL                  _hAccelTbl;				//a handle to the accelerator table
    HWND                    _hWnd;					//the main window
	HWND					_hwndDlg;				//handle to the main/connecting dialog
	BOOL					_fDlgUI;				//if the app was started from a dialog UI
	BOOL					_fDefaultProfile;		//if the app was started with default.rdp
	BOOL					_fCmdLineServer;		//if a server to connect to was specified on the command line
	BOOL					_fCmdLineConsole;		//if a console session was specified on the command line
	BOOL					_fPromptUser;			//if the user should be prompted with message boxes
	BOOL					_fStartFullscreen;		//if the client should start in fullscreen mode
	BOOL					_fIsFullscreen;			//if the client is currently in fullscreen mode

	WCHAR					_szOrigKbdLayout[KL_NAMELENGTH];

	ITaskbarList2			*_pTaskbarList;			//To handle fullscreen transitions

	WCHAR					_szServer[MAX_SERVERNAMELEN];
	WCHAR					_szProfile[MAX_PATH];
#ifndef PICTOR
	BSTR					_bstrClxCmdLine;
#else
	WCHAR					szClxCmdLine[MAX_PATH];
#endif // PICTOR
	CSettings				*_pSettings;
	State					_state;
	BOOL					_fCloseRecvd;
	BOOL					_fInCloseHandler;
	BOOL					_fLoggedIn;
	DWORD					_dwSupportedRedirectors;

	CArcDlg					*_pArcDlg;
	DWORD					_dwMaxArcAttempts;
	BOOL					_fAutoReconnecting;

    HWND                    _hWndTscAx;       //handle to the browser window

	RECT					_rcWorkArea;		//screen work area
	RECT					_rcMainWnd;			//window rectangle of the mainwindow
	DWORD					_dwWinStyle;		//creation styles for the main window
	DWORD                   _dwWidth;
    DWORD                   _dwHeight;
	DWORD					_dwScreenWidth;
	DWORD					_dwScreenHeight;
	BOOL					_fBbarEnabled;

	TscCore					*_pTscCore;
	BOOL					_fIsWirelessProjector;	
};

extern HINSTANCE			ghInstance;

#endif	//_MAINWND_H_
