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
#include <mstsax.h>
#include "mstsc.h"
#include "MainWnd.h"
#include "baseui.h"
#include "utils.h"

HRESULT	CMainWnd::OnConnected()
{
	SetState(Connected);

	UINT	uMsg = MSG_TSC_CONNECTED;
	WPARAM	wParam = 0;

	LPWSTR pszTitle	= (LPWSTR )LoadString(ghInstance, (_fDefaultProfile) ? IDS_TITLE_CONNECTED_DEFAULT:	IDS_TITLE_CONNECTED	, NULL,	0);
	if (pszTitle)
	{
		WCHAR szTitle[MAX_PATH + MAX_SERVERNAMELEN];
		HRESULT	hr;
		if (_fDefaultProfile)
			hr = StringCchPrintf(szTitle, MAX_PATH + MAX_SERVERNAMELEN,	pszTitle, _szServer);
		else
			hr = StringCchPrintf(szTitle, MAX_PATH + MAX_SERVERNAMELEN,	pszTitle, _szProfile, _szServer);

		pszTitle = (hr == S_OK)	? szTitle :	_szServer;
		SetWindowText(_hWnd, pszTitle);
	}

	StopAutoReconnect();

	if (_fIsFullscreen)
		AutoHideTaskbar(TRUE);
	SetWindowPos(_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);

	if (!_fDlgUI)
	{
		uMsg = WM_COMMAND;
		wParam = MAKEWPARAM(IDOK, 0);
	}

	if (_hwndDlg)
	{
		ASSERT(::IsWindow(_hwndDlg));
		PostMessage(_hwndDlg, uMsg,	wParam,	0);
	}

	UpdateServersMRU();

	SetFocus(_hWndTscAx);

	return S_OK;
}

HRESULT	CMainWnd::OnDisconnected(LONG lReason, ExtendedDisconnectReasonCode	extReason)
{
	if (_state == Disconnected)
		return S_OK;

	SetState(Disconnected);

	if(_fIsWirelessProjector )
		{
		PostMessage(_hWnd, WM_CLOSE, 0,	0L);
		return	S_OK;
		}
	SetWindowText(_hWnd, (LPWSTR)LoadString(ghInstance,	IDS_RDP, NULL, 0));

	StopAutoReconnect();

	BOOL fShowError	= (	((lReason != disconnectReasonLocalNotError)	&& (lReason	!= disconnectReasonRemoteByUser)) ||
				   (extReason == exDiscReasonReplacedByOtherConnection)	);


	BOOL fExit = (fShowError) ?	FALSE :	_fLoggedIn;

	if (_fDlgUI)
	{
		ASSERT((_hwndDlg) && ::IsWindow(_hwndDlg));
		if ((_fCloseRecvd) || (fExit))
			PostMessage(_hWnd, WM_CLOSE, 0,	0L);
		else
		{
			//Use SendMessage and then hide	the	main window
			//else you may run into	focus issues because hiding	the	main window	will post an
			//activate message to any app behind the main window which may get it after	the	
			//Disconnect message has been processed
			SendMessage(_hwndDlg, MSG_TSC_DISCONNECTED,	0, 0L);
			ShowWindow(_hWnd, SW_HIDE);
		}
	}
	else
	{
		ShowWindow(_hWnd, SW_HIDE);
	}

	if (fShowError && _fPromptUser)
	{
		//Extract the error code and post a message back to the main window to 
		//to show the error message. Putting a message box here in the OnDisconnect event handler
		//can lead to reentrancy issues in the core. 
		DWORD dwErrCode	= GetErrorStringId(lReason,	extReason);
		PostMessage(_hWnd, MSG_TSC_ERROR, dwErrCode, 0);
	}

	if ((!fShowError) && (!_fDlgUI))
	{
		if(_hwndDlg)
		{
			EndDialog(_hwndDlg,	IDOK);
			_hwndDlg = NULL;
		}

		PostMessage(_hWnd, WM_CLOSE, 0,	0L);
	}

	return S_OK;
}

HRESULT	CMainWnd::OnLoginComplete()
{
	SetState(LoggedIn);
	_fLoggedIn = TRUE;
	return S_OK;
}

HRESULT	CMainWnd::OnFatalError(LONG	lCode)
{
	if(_fIsWirelessProjector )
		{
		PostMessage(_hWnd, WM_CLOSE, 0,	0L);
		return	S_OK;
		}


	WCHAR szTitle[MAX_PATH];
	LPWSTR pszTitle	= (LPWSTR )LoadString(ghInstance, IDS_FATALERROR_TITLE,	NULL, 0);
	LPWSTR pszMsg =	(LPWSTR	)LoadString(ghInstance,	IDS_FATALERROR_MSG,	NULL, 0);

	int	nRet = IDABORT;
#ifdef DEBUG
	DWORD dwType = MB_ABORTRETRYIGNORE | MB_ICONSTOP | MB_APPLMODAL	| MB_SETFOREGROUND | MB_TOPMOST;
#else
	DWORD dwType = MB_OK | MB_ICONSTOP | MB_APPLMODAL |	MB_SETFOREGROUND | MB_TOPMOST;
#endif
	
	if ( (_fPromptUser) && (pszTitle) && (pszMsg) && 
		(S_OK == StringCchPrintf(szTitle, MAX_PATH, pszTitle, lCode)) )
	{
		MessageBox(_hWnd, pszMsg, szTitle, dwType);
	}

	if ((nRet == IDOK) || (nRet	== IDABORT))
	{
		TerminateProcess(GetCurrentProcess(), 0);
	}
#ifdef DEBUG
	else if	(nRet == IDRETRY)
	{
		DebugBreak();
	}
#endif

	return S_OK;
}

HRESULT	CMainWnd::OnWarning(LONG lCode)
{
/*
	if (lCode != WARN_BITMAPCACHE_CORRUPTED)
		return S_OK;

	LPWSTR pszTitle	= (LPWSTR )LoadString(ghInstance, IDS_BMPCACHEFAIL_TITLE, NULL,	0);
	LPWSTR pszMsg =	(LPWSTR	)LoadString(ghInstance,	IDS_BMPCACHEFAIL_MSG, NULL,	0);

	if (pszCaption && pszMsg)
	{
		MessageBox(_hWnd, pszMsg, pszTitle,	MB_OK);
	}
*/
	return S_OK;
}

HRESULT	CMainWnd::OnRequestContainerMinimize()
{
	AutoHideTaskbar(FALSE);
	ShowWindow(_hWnd, SW_MINIMIZE);
	return S_OK;
}

HRESULT	CMainWnd::OnRequestFullScreen(BOOL fFullscreen)
{
	_fIsFullscreen = fFullscreen;

	DWORD dwStyle = GetWindowLong(_hWnd, GWL_STYLE);
	SetWindowLong(_hWnd, GWL_STYLE, (fFullscreen) ? (dwStyle & (~WS_OVERLAPPEDWINDOW)) : (dwStyle | WS_OVERLAPPEDWINDOW) );

	if (fFullscreen)
	{
		SetWindowPos(_hWnd, HWND_TOP, 0, 0, _dwScreenWidth, _dwScreenHeight, SWP_SHOWWINDOW);
	}
	else
	{
		RECT rcWnd;
		IntersectRect(&rcWnd, &_rcMainWnd, &_rcWorkArea);
		SetWindowPos(_hWnd, HWND_TOP, rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, SWP_NOZORDER);
		ShowWindow(_hWnd, SW_RESTORE);
	}
	
	AutoHideTaskbar(fFullscreen);
	return S_OK;
}

HRESULT	CMainWnd::OnConfirmClose(BOOL *pfConfirm)
{
	ASSERT(pfConfirm);

	*pfConfirm = TRUE;

	if ((_fCloseRecvd) || (!_fPromptUser))
		return S_OK;

	LPWSTR pszTitle	= (LPWSTR )LoadString(ghInstance, IDS_CONFIRMCLOSE_TITLE, NULL,	0);
	LPWSTR pszMsg =	(LPWSTR	)LoadString(ghInstance,	IDS_CONFIRMCLOSE_MSG, NULL,	0);

	if (pszTitle &&	pszMsg)
	{
		if (IDOK ==	MessageBox(_hWnd, pszMsg, pszTitle,	MB_OKCANCEL	| MB_ICONQUESTION | MB_SETFOREGROUND))	
		{
			_fCloseRecvd = TRUE;
			DEBUGMSG(ZONE_VERBOSE, (L"OnConfirmClose...Received close\n"));
			*pfConfirm = TRUE;
		}
		else
		{
			*pfConfirm = FALSE;
		}
	}
	return S_OK;
}

HRESULT CMainWnd::OnShutdown()
{
	return OnDisconnected((LONG)disconnectReasonLocalNotError, exDiscReasonNoInfo);
}

HRESULT	CMainWnd::OnAutoReconnecting(long lAttempt,	BOOL *pfManual)
{
	ASSERT(pfManual);

	*pfManual =	TRUE;

	if (lAttempt ==	1)
	{
		ASSERT(!_pArcDlg);

		_pArcDlg = new CArcDlg(_hWnd, ghInstance);
		if (_pArcDlg) 
		{
			_pArcDlg->SetMaxAttempts(_dwMaxArcAttempts);
			if (!_fBbarEnabled)
				ShowWindow(_hWnd, SW_MINIMIZE);
			if (_pArcDlg->DoModeless())	
				_pArcDlg->ShowTopMost();

			_fAutoReconnecting = TRUE;
		}

	}
	
	BOOL fContinueArc =	FALSE;
	if (_pArcDlg) 
	{
		_pArcDlg->OnNotifyAutoReconnecting(lAttempt, &fContinueArc);

		if (fContinueArc) 
			SetTimer(_hWnd,	(UINT_PTR)(this), ARC_CONNECTION_DELAY,	StaticArcTimerCallback);
		else
		{
			PostMessage(_hWnd, WM_CLOSE, 0,	0L);
		}
	}

	return S_OK;
}

void CALLBACK
CMainWnd::StaticArcTimerCallback(
	HWND hwnd,
	UINT uMsg,
	UINT_PTR idEvent,
	DWORD dwTime
	)
{
	CMainWnd *pMainWnd = (CMainWnd *)idEvent;

	ASSERT(pMainWnd);

	pMainWnd->ArcTimerCallback(hwnd, uMsg, idEvent,	dwTime);

}

void 
CMainWnd::ArcTimerCallback(
	HWND hwnd,
	UINT uMsg,
	UINT_PTR idEvent,
	DWORD dwTime)
{
	BOOL bRet =	KillTimer(hwnd,	idEvent);
	ASSERT(bRet);

	//initiate a disconnect	if we failed to	autoreconnect
	if (FAILED(_pTscCore->Connect())) 
	{
		PostMessage(_hWnd, WM_CLOSE, 0,	0L);
	}
}
