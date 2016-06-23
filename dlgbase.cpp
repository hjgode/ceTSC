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
#include "dlgbase.h"
#include "mstsc.h"

CDlgBase::CDlgBase(HWND hwndOwner, HINSTANCE hInst, DWORD dwResId)
			: _hwndOwner(hwndOwner), _hInst(hInst), _dwResId(dwResId)
{

}

CDlgBase::~CDlgBase()
{

}

int CALLBACK CDlgBase::StaticDialogProc(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam)
{
	CDlgBase *pDlgBase = NULL;
	if ((uMsg == WM_INITDIALOG) || (uMsg == WM_CREATE))
	{
		pDlgBase = (CDlgBase *)lParam;
		SetWindowLong(hwndDlg, GWL_USERDATA, lParam);
	}
	else
	{
		pDlgBase = (CDlgBase *)GetWindowLong(hwndDlg, GWL_USERDATA);
	}

	if (pDlgBase)
	{
		return pDlgBase->DialogProc(hwndDlg, uMsg, wParam, lParam);
	}
	else
	{
		DEBUGMSG(ZONE_VERBOSE, (L"Dialog pointer NULL for hwndDlg=0x%08x uMsg=0x%x, wp=0x%x, lp=0x%x\n", hwndDlg, uMsg, wParam, lParam));
		return FALSE;
	}
}

int CDlgBase::DialogProc(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		_hwndDlg = hwndDlg;
		CenterWindow();
		return TRUE;

	case WM_COMMAND:
	{
		switch(LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hwndDlg, IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hwndDlg, IDCANCEL);
			return TRUE;
	
		default:
			break;
		}
		break;
	}
	
	case WM_CLOSE:
		EndDialog(hwndDlg, IDCANCEL);
		return TRUE;
	}
	
	return FALSE;
}

void CDlgBase::CenterWindow()
{
	int nX = GetSystemMetrics(SM_CXSCREEN);
	int nY = GetSystemMetrics(SM_CYSCREEN);

	RECT rc;
	int nWidth, nHeight;
	GetWindowRect(_hwndDlg, &rc);
	nWidth = rc.right - rc.left;
	nHeight = rc.bottom - rc.top;
	MoveWindow(_hwndDlg, (nX - nWidth)/2, (nY - nHeight)/2, nWidth, nHeight, TRUE);
}
