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
#ifndef _TSCCORE_H_
#define _TSCCORE_H_

#include "mstsax.h"
#include "settings.h"

class TscCoreEvents
{
public:
	virtual HRESULT OnConnected() = 0;
	virtual HRESULT OnDisconnected(LONG lReason, ExtendedDisconnectReasonCode	extReason) = 0;
	virtual HRESULT OnLoginComplete() = 0;
	virtual HRESULT OnFatalError(LONG lCode) = 0;
	virtual HRESULT OnWarning(LONG lCode) = 0;
	virtual HRESULT OnRequestContainerMinimize() = 0;
	virtual HRESULT OnConfirmClose(BOOL* pvbConfirm) = 0;
	virtual HRESULT OnRequestFullScreen(BOOL fFullScreen) = 0;
	virtual HRESULT OnAutoReconnecting(long attemptCount, BOOL *pfManual) = 0;
	virtual HRESULT OnShutdown() = 0;
};

class TscCore
{
public:
	virtual HRESULT CreateCoreWindow(HINSTANCE hInst, HWND hWndParent, HWND *phwnd) = 0;
	virtual HRESULT Destroy() = 0;
	virtual HRESULT RegisterEventSink(TscCoreEvents *pEvents) = 0;

	virtual HRESULT Connect() = 0;
	virtual HRESULT Disconnect() = 0;
	virtual HRESULT RequestClose() = 0;

	virtual HRESULT GoFullscreen(BOOL fFullscreen) = 0;

	virtual HRESULT GetProperty(PWSTR pszPropName, TscPropType eType, PDWORD pdwSize, PBYTE pbVal) = 0;
	virtual HRESULT SetProperty(PWSTR pszPropName, TscPropType eType, DWORD dwSize, PBYTE pbVal) = 0;
	virtual HRESULT SetProperty(Property *pProp) = 0;

};

extern "C" TscCore* CreateTscCore();
extern "C" void     DeleteTscCore(TscCore *pTsc);

#endif

