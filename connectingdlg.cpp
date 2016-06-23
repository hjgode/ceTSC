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
#include "connectingdlg.h"

CConnectingDlg::CConnectingDlg(HWND hwndParent, HINSTANCE hInst, CMainWnd *pMainWnd, LPWSTR szServer) : 
					_pMainWnd(pMainWnd), CDlgBase(hwndParent, hInst, IDD_CONNECTING)
{
	ASSERT(pMainWnd);
	StringCchCopy(_szServer, MAX_SERVERNAMELEN, szServer);
}

CConnectingDlg::~CConnectingDlg()
{

}

int CConnectingDlg::DialogProc(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam)
{
	int nRet = FALSE;
	if (uMsg == WM_INITDIALOG)
	{
		LPWSTR pszConn = (LPWSTR )LoadString(_hInst, IDS_CONNECTING_TO, NULL, 0);
		if (pszConn)
		{
			WCHAR szTitle[MAX_PATH + MAX_SERVERNAMELEN];
			// PREFIX #49975
			_snwprintf(szTitle, MAX_PATH + MAX_SERVERNAMELEN - 1, pszConn, _szServer);
			szTitle[MAX_PATH + MAX_SERVERNAMELEN - 1] = L'\0';
			SetDlgItemText(hwndDlg, IDC_STC_CONNECTINGTO, szTitle);
		}
		_pMainWnd->SetDlgHandle(hwndDlg, FALSE);
		nRet = TRUE;
	}
	else if ( ((uMsg == WM_COMMAND) && (LOWORD(wParam) == IDCANCEL)) || 
				(uMsg == WM_CLOSE) )
	{
		_pMainWnd->Disconnect();
		_pMainWnd->SetDlgHandle(NULL);
		nRet = TRUE;
	}
	else if ((uMsg == WM_COMMAND) && (LOWORD(wParam) == IDOK))
	{
		_pMainWnd->SetDlgHandle(NULL);
		nRet = TRUE;
	}

	return (CDlgBase::DialogProc(hwndDlg, uMsg, wParam, lParam) || nRet);
}
