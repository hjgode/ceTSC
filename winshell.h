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
#ifndef _WINSHELL_H_
#define _WINSHELL_H_

typedef BOOL (*PFN_INITIALIZETSC)(HINSTANCE);
typedef VOID (*PFN_UNINITIALIZETSC)();

class CWinShell : public TscCore
{
public:
	CWinShell();
	~CWinShell();

public:
	//TscCore methods
	virtual HRESULT CreateCoreWindow(HINSTANCE hInst, HWND hWndParent, HWND *phwnd);
	virtual HRESULT Destroy();
	virtual HRESULT RegisterEventSink(TscCoreEvents *pEvents);

	virtual HRESULT Connect();
	virtual HRESULT Disconnect();
	virtual HRESULT RequestClose();

	virtual HRESULT GoFullscreen(BOOL fFullscreen);

	virtual HRESULT GetProperty(PWSTR pszPropName, TscPropType eType, PDWORD pdwSize, PBYTE pbVal);
	virtual HRESULT SetProperty(PWSTR pszPropName, TscPropType eType, DWORD dwSize, PBYTE pbVal);
	virtual HRESULT SetProperty(Property *pProp);

	static  LRESULT CALLBACK StaticWinShellWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT                  WinShellWndProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	BOOL	RegisterShellWindow();

protected:
	HINSTANCE				_hInst;
	HWND					_hWndParent;
	HWND					_hWndWinShell;
	HWND					_hWndControl;
	TscCoreEvents			*_pEventSink;
	HINSTANCE				_hLibTscClient;
	PFN_INITIALIZETSC		_pfnInitializeTsc;
	PFN_UNINITIALIZETSC		_pfnUninitializeTsc;
};


#endif
