//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//
#include <windows.h>
#include "tsccore.h"
#include "tscctrl.h"
#include "winshell.h"

#define	WINSHELL_CLASS	L"WinShell"
#define	TSCWINCTRL_LIB	L"mstscax.dll"

TscCore* CreateTscCore()
{
	return new CWinShell();
}

void DeleteTscCore(TscCore *pTsc)
{
	CWinShell *pWinShell = (CWinShell *)pTsc;
	delete pWinShell;
}


CWinShell::CWinShell()
{
	_hInst = NULL;
	_hWndControl = NULL;
	_hWndWinShell =	NULL;
	_hWndParent	= NULL;
	_pEventSink	= NULL;
	_hLibTscClient = NULL;
	_pfnInitializeTsc = NULL;
	_pfnUninitializeTsc = NULL;
}

CWinShell::~CWinShell()
{
	
}

BOOL CWinShell::RegisterShellWindow()
{
	WNDCLASS wc;

	wc.lpfnWndProc		= CWinShell::StaticWinShellWndProc;
	wc.style			= 0;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= _hInst;
	wc.hIcon			= NULL;
	wc.hCursor			= NULL;
	wc.hbrBackground	= (HBRUSH )GetStockObject(NULL_BRUSH);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= WINSHELL_CLASS;

	return RegisterClass(&wc);
}

HRESULT	CWinShell::CreateCoreWindow(HINSTANCE hInst, HWND hWndParent, HWND *phwnd)
{
	ASSERT(phwnd);
	ASSERT(hInst);

	HRESULT	hr = E_FAIL;
	_hInst = hInst;
	_hWndParent	= hWndParent;


	if (!RegisterShellWindow())
		return E_FAIL;


	//If we	didnt find this	class, Gwes	hasnt registered the class and the TSC client will not run in gwe.
	//So initialize	it for this	process
	WNDCLASS wc;
	if (!GetClassInfo(_hInst, TSCCONTROL_CLASS,	&wc)) 
	{
		_hLibTscClient = LoadLibrary(TSCWINCTRL_LIB);
		if (!_hLibTscClient)
			return E_FAIL;	
		
		_pfnInitializeTsc =	(PFN_INITIALIZETSC )GetProcAddress(_hLibTscClient, L"InitializeTSC");
		_pfnUninitializeTsc	= (PFN_UNINITIALIZETSC )GetProcAddress(_hLibTscClient, L"UninitializeTSC");

		if ((NULL == _pfnInitializeTsc)	|| (NULL ==	_pfnUninitializeTsc))
		{
			FreeLibrary(_hLibTscClient);
			_hLibTscClient = NULL;
			return E_FAIL;	
		}

		if (!_pfnInitializeTsc(_hInst))
			return E_FAIL;
	}

	RECT rcParent;
	GetClientRect(_hWndParent, &rcParent);

	_hWndWinShell =	CreateWindow(WINSHELL_CLASS, WINSHELL_CLASS, WS_CHILD|WS_VISIBLE, rcParent.left, rcParent.top,
		rcParent.right - rcParent.left,	rcParent.bottom-rcParent.top, _hWndParent, NULL, _hInst, NULL);

	if (!_hWndWinShell)
		return E_FAIL;

	SetWindowLong(_hWndWinShell, GWL_USERDATA, (LONG)this);

	_hWndControl = CreateWindow(TSCCONTROL_CLASS, TSCCONTROL_CLASS,	WS_CHILD|WS_VISIBLE, rcParent.left,	rcParent.top,
		rcParent.right - rcParent.left,	rcParent.bottom-rcParent.top, _hWndWinShell, NULL, _hInst, NULL);

	if (!_hWndControl)
		return E_FAIL;

	*phwnd = _hWndWinShell;
	return S_OK;
}

HRESULT	CWinShell::Destroy()
{
	if (_hWndWinShell)
	{
		DestroyWindow(_hWndWinShell);
		_hWndWinShell =	NULL;
	}

	if (_hWndControl)
	{
		if (IsWindow(_hWndControl))
			DestroyWindow(_hWndControl);
		_hWndControl = NULL;
	}

	if (_pfnUninitializeTsc)
	{
		_pfnUninitializeTsc();
		
		ASSERT(_hLibTscClient);
		FreeLibrary(_hLibTscClient);
	}

	return S_OK;
}

HRESULT	CWinShell::RegisterEventSink(TscCoreEvents *pEventSink)
{
	ASSERT(pEventSink);
	_pEventSink	= pEventSink;
	return S_OK;
}

HRESULT	CWinShell::Connect()
{
	ASSERT(_hWndControl);
	return SendMessage(_hWndControl, WM_TSCMSG_CONNECT,	0, 0);
}

HRESULT	CWinShell::Disconnect()
{
	ASSERT(_hWndControl);
	return SendMessage(_hWndControl, WM_TSCMSG_DISCONNECT, 0, 0);
}

HRESULT	CWinShell::RequestClose()
{
	ASSERT(_hWndControl);
	return SendMessage(_hWndControl, WM_TSCMSG_REQUESTCLOSE, 0,	0);
}

HRESULT CWinShell::GoFullscreen(BOOL fFullscreen)
{
	ASSERT(_hWndControl);
	return SendMessage(_hWndControl, WM_TSCMSG_PUTFULLSCREEN, (WPARAM)fFullscreen, 0);
}

HRESULT	CWinShell::GetProperty(PWSTR pszPropName, TscPropType eType, PDWORD	pdwSize, PBYTE pbVal)
{
	ASSERT(pdwSize);

	TSCPROPERTY	tscProp;
	tscProp.pszPropName	= pszPropName;
	tscProp.dwPropSize = *pdwSize;
	tscProp.ePropType =	eType;

	if (eType >	TscTypeBOOL)
		tscProp.propVal.pszStrVal =	(PWSTR )pbVal;

	HRESULT	hr = SendMessage(_hWndControl, WM_TSCMSG_GETPROPERTY, 0, (LPARAM)&tscProp);
	if (FAILED(hr))
		return hr;

	if (eType <= TscTypeBOOL)
		*((PDWORD)pbVal) = tscProp.propVal.lLongVal;

	return hr;
}

HRESULT	CWinShell::SetProperty(PWSTR pszPropName, TscPropType eType, DWORD dwSize, PBYTE pbVal)
{
	TSCPROPERTY	tscProp;
	tscProp.pszPropName	= pszPropName;
	tscProp.ePropType =	eType;
	tscProp.dwPropSize = dwSize;
	if (eType <= TscTypeBOOL)
		tscProp.propVal.lLongVal = *((PDWORD)pbVal);
	else
		tscProp.propVal.pszStrVal =	(PWSTR)pbVal;

	return SendMessage(_hWndControl, WM_TSCMSG_SETPROPERTY,	0, (LPARAM)&tscProp);
		
}

HRESULT	CWinShell::SetProperty(Property	*pProp)
{
	return SetProperty(pProp->szName, pProp->eType,	pProp->dwSize, (pProp->eType <=	TscTypeBOOL) ? (PBYTE)&pProp->uVal.pbVal : pProp->uVal.pbVal);
}

LRESULT	CALLBACK CWinShell::StaticWinShellWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CWinShell *pWinShell = (CWinShell *)GetWindowLong(hwnd,	GWL_USERDATA);
	if (pWinShell)
		return pWinShell->WinShellWndProc(message, wParam, lParam);
	else
		return DefWindowProc(hwnd, message,	wParam,	lParam);
}

LRESULT	CWinShell::WinShellWndProc(UINT	message, WPARAM	wParam,	LPARAM lParam)
{
	HRESULT	hr = E_NOTIMPL;

	if (!_pEventSink)
		return S_OK;

	switch (message)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
			break;	//else fallthrough
	case WM_SETFOCUS:
	case WM_EXITMENULOOP:
		::SetFocus(_hWndControl);
		break;

	case WM_SIZE:
		if (_hWndControl)
			MoveWindow(_hWndControl, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;

	case WM_WINDOWPOSCHANGED:
	{
		LPWINDOWPOS lpWPos = (LPWINDOWPOS )lParam;
		if (_hWndControl)
			MoveWindow(_hWndControl, lpWPos->x, lpWPos->y, lpWPos->cx, lpWPos->cy, TRUE);
		break;
	}

	case WM_QUERYNEWPALETTE:
	case WM_SETTINGCHANGE:
	case WM_PALETTECHANGED:
	case WM_SYSCOLORCHANGE:
		SendMessage(_hWndControl, message, wParam, lParam);
		break;

	case WM_TSCEVT_CONNECTED:
		hr = _pEventSink->OnConnected();
		break;
	case WM_TSCEVT_TERMTSC:
		hr = _pEventSink->OnShutdown();
		break;
	
	case WM_TSCEVT_DISCONNECTED:
		hr = _pEventSink->OnDisconnected(wParam, (ExtendedDisconnectReasonCode	)lParam);
		return (SUCCEEDED(hr)) ? TRUE :	FALSE;

	case WM_TSCEVT_LOGINCOMPLETE:
		hr = _pEventSink->OnLoginComplete();
		break;
	case WM_TSCEVT_FATALERROR:
		hr = _pEventSink->OnFatalError(wParam);
		break;
	case WM_TSCEVT_WARNING:
		hr = _pEventSink->OnWarning(wParam);
		break;
	case WM_TSCEVT_REQUESTMINIMIZE:
		hr = _pEventSink->OnRequestContainerMinimize();
		break;
	case WM_TSCEVT_REQUESTFULLSCREEN:
		hr = _pEventSink->OnRequestFullScreen((BOOL)wParam);
		break;
	case WM_TSCEVT_ASKCONFIRMCLOSE:
		hr = _pEventSink->OnConfirmClose((BOOL *)wParam);
		break;
	case WM_TSCEVT_AUTORECONNECTING:
		hr = _pEventSink->OnAutoReconnecting(wParam, (BOOL *)lParam);
		break;

	default:
		hr = DefWindowProc(_hWndWinShell, message, wParam, lParam);
		break;
	}
	return hr;
}
