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
#include "common.h"

static DWORD gadwDisableCtls[] = {	IDC_CHK_START_PROGRAM, 
									IDC_STC_STARTPROGRAM,
									IDC_EDT_STARTPROGRAM,
									IDC_STC_WORKDIR,
									IDC_EDT_WORKDIR
								};

int CPropRun::DialogProc(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam)
{
	int nRet = FALSE;
	switch(uMsg)
	{
	case WM_INITDIALOG:
	{
		nRet = CDlgBase::DialogProc(hwndDlg, uMsg, wParam, lParam);
		if (!nRet)
			return FALSE;
		return LoadFromSettings();
	}

	case WM_COMMAND:
	{
		if ( (LOWORD(wParam) == IDC_CHK_START_PROGRAM) && (HIWORD(wParam) == BN_CLICKED) )
		{
			BOOL bChecked = (BST_CHECKED == SendMessage((HWND)lParam, BM_GETCHECK, 0, 0));
			EnableControls(_hwndDlg, &gadwDisableCtls[1], (sizeof(gadwDisableCtls)/sizeof(gadwDisableCtls[0])) - 1, bChecked);
		}
		break;
	}

	case MSG_TSC_CONNECT:
	{
		SaveToSettings();
		EnableControls(_hwndDlg, gadwDisableCtls, sizeof(gadwDisableCtls)/sizeof(gadwDisableCtls[0]), FALSE);
		return TRUE;
	}

	case MSG_TSC_DISCONNECTED:
	{
		HWND hwndChk = GetDlgItem(_hwndDlg, IDC_CHK_START_PROGRAM);
		ASSERT(hwndChk);
		
		EnableWindow(hwndChk, TRUE);
		BOOL bChecked = (BST_CHECKED == SendMessage(hwndChk, BM_GETCHECK, 0, 0));

		EnableControls(_hwndDlg, &gadwDisableCtls[1], (sizeof(gadwDisableCtls)/sizeof(gadwDisableCtls[0])) - 1, bChecked);
		return TRUE;
	}

	case WM_DESTROY:
	{
		nRet = SaveToSettings();
		break;
	}

	default:
		break;
	}
	return CDlgBase::DialogProc(hwndDlg, uMsg, wParam, lParam) || nRet;
}



BOOL CPropRun::LoadFromSettings()
{
	WCHAR szProp[MAX_PATH] = {0};
	WCHAR pszEmpty[] = L"";
	szProp[0] = L'\0';

	TscPropType eType = TscTypeSTRING;
	DWORD dwSize = sizeof(szProp);

	ASSERT(_pSettings);
	_pSettings->Read(PROP_STARTPROGRAM, &eType, (PBYTE)pszEmpty, (PBYTE)szProp, &dwSize);

	BOOL bStart = (szProp[0] != L'\0');
	SendDlgItemMessage(_hwndDlg, IDC_CHK_START_PROGRAM, BM_SETCHECK, bStart ? BST_CHECKED : BST_UNCHECKED, 0);

	if (!bStart)
	{
		EnableControls(_hwndDlg, &gadwDisableCtls[1], (sizeof(gadwDisableCtls)/sizeof(gadwDisableCtls[0])) - 1, FALSE);
		return TRUE;
	}

	SetWindowText(GetDlgItem(_hwndDlg, IDC_EDT_STARTPROGRAM), szProp);

	_pSettings->Read(PROP_WORKDIR, &eType, (PBYTE)pszEmpty, (PBYTE)szProp, &dwSize);
	SetWindowText(GetDlgItem(_hwndDlg, IDC_EDT_WORKDIR), szProp);

	return TRUE;
}

BOOL CPropRun::SaveToSettings()
{
	WCHAR szProp[MAX_PATH];
	WCHAR *pszEmpty = L"";

	ASSERT(_pSettings);

	BOOL bStart = (SendDlgItemMessage(_hwndDlg, IDC_CHK_START_PROGRAM, BM_GETCHECK, 0, 0) == BST_CHECKED);
	WCHAR *pszVal = bStart ? szProp : pszEmpty;

	int nLen = 0;
	if (bStart)
	{
		nLen = GetWindowText(GetDlgItem(_hwndDlg, IDC_EDT_STARTPROGRAM), szProp, MAX_PATH);
	}

	if (!_pSettings->Write(PROP_STARTPROGRAM, TscTypeSTRING, (PBYTE)pszVal, (nLen + 1)*sizeof(WCHAR) ) )
	{
		return FALSE;
	}

	if (bStart)
	{
		nLen = GetWindowText(GetDlgItem(_hwndDlg, IDC_EDT_WORKDIR), szProp, MAX_PATH);
	}

	if (!_pSettings->Write(PROP_WORKDIR, TscTypeSTRING, (PBYTE)pszVal, (nLen + 1)*sizeof(WCHAR) ) )
	{
		return FALSE;
	}

	return TRUE;
}

