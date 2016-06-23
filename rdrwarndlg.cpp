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
#include "rdrwarndlg.h"

CRdrWarnDlg::CRdrWarnDlg(HWND hwndParent, HINSTANCE hInst, ISettings *pSettings) : 
					CDlgBase(hwndParent, hInst, IDD_RDRWARNDLG)
{
	ASSERT(pSettings);
	_pSettings = pSettings;
	_szServer[0] = L'\0';
	_dwNewFlags = 0;
	_dwOldFlags = 0;
}

CRdrWarnDlg::~CRdrWarnDlg()
{

}

BOOL CRdrWarnDlg::PromptRequired()
{
	HKEY hk = NULL;
	WCHAR szKey[MAX_PATH];

	DWORD dwType, dwSize;
	TscPropType eType;
	int nCurVal = 0;

	dwSize = sizeof(_szServer);
	if ( (!_pSettings->Read(PROP_SERVER, &eType, (PBYTE)(L""), (PBYTE )_szServer, &dwSize)) || 
		(_szServer[0] == L'\0') )
	{
		return TRUE;
	}

	dwSize = sizeof(int);
	int nValDef = 0, nVal = 0;
	if ( (_pSettings->Read(PROP_REDIRECTPORTS, &eType, (PBYTE)&nValDef, (PBYTE)&nVal, &dwSize)) &&
		 (dwSize == sizeof(int)) && (nVal == 1)  )
	{
		nCurVal |= PROMPT_DEVICES_PORTS;
	}

	nValDef = 0;
	dwSize = sizeof(int);
	if ( (_pSettings->Read(PROP_REDIRECTDRIVES, &eType, (PBYTE)&nValDef, (PBYTE)&nVal, &dwSize)) &&
		 (dwSize == sizeof(int)) && (nVal == 1)  )
	{
		nCurVal |= PROMPT_DEVICES_DRIVES;
	}

	if (nCurVal == 0)	//no redirection requested
		return FALSE;

	_dwNewFlags = nCurVal;

	if (S_OK != StringCchCopy(szKey, MAX_PATH, TSC_SETTINGS_REG_ROOT))
		return TRUE;

	if (S_OK != StringCchCat(szKey, MAX_PATH, DEVICES_ROOT))
		return TRUE;

	DWORD dwDisp;
	if ((ERROR_SUCCESS != RegCreateKeyEx(HKEY_CURRENT_USER, szKey, 0, NULL, 0, 0, NULL, &hk, &dwDisp)) || (hk == NULL))
		return TRUE;

	dwSize = sizeof(DWORD);
	BOOL bPrompt = ( (ERROR_SUCCESS != RegQueryValueEx(hk, _szServer, NULL, &dwType, (LPBYTE)&nVal, &dwSize)) ||
		 (dwType != REG_DWORD) || (dwSize > sizeof(DWORD)) || (nVal < 0) || (nVal > PROMPT_DEVICES_MAX) );

	RegCloseKey(hk);

	if (bPrompt)
		return TRUE;

	_dwOldFlags = nVal;

	if (~_dwOldFlags & _dwNewFlags)	//Check if the user has selected anything other than what has already been approved
	{
		_dwNewFlags = 0;
		if ((nCurVal & PROMPT_DEVICES_PORTS) && (!(nVal & PROMPT_DEVICES_PORTS)) )
			_dwNewFlags |= PROMPT_DEVICES_PORTS;

		if ((nCurVal & PROMPT_DEVICES_DRIVES) && (!(nVal & PROMPT_DEVICES_DRIVES)) )
			_dwNewFlags |= PROMPT_DEVICES_DRIVES;

		ASSERT(_dwNewFlags);

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

VOID CRdrWarnDlg::UpdatePromptFlags()
{
	HKEY hk = NULL;
	WCHAR szKey[MAX_PATH];

	if (SendDlgItemMessage(_hwndDlg, IDC_CHK_NOPROMPT, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
		return;

	if (S_OK != StringCchCopy(szKey, MAX_PATH, TSC_SETTINGS_REG_ROOT))
		return;

	if (S_OK != StringCchCat(szKey, MAX_PATH, DEVICES_ROOT))
		return;

	DWORD dwDisp;
	if ((ERROR_SUCCESS != RegCreateKeyEx(HKEY_CURRENT_USER, szKey, 0, NULL, 0, 0, NULL, &hk, &dwDisp)) || (hk == NULL))
		return;

	DWORD dwVal = _dwOldFlags | _dwNewFlags;
	
	RegSetValueEx(hk, _szServer, NULL, REG_DWORD, (PBYTE)&dwVal, sizeof(DWORD));
	
	RegCloseKey(hk);
}

int CRdrWarnDlg::DialogProc(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam)
{
	int nRet = FALSE;
	if (uMsg == WM_INITDIALOG)
	{
		WCHAR szPromptString[MAX_PATH];
		szPromptString[0] = L'\0';

		LPWSTR pszPorts = (LPWSTR )LoadString(_hInst, IDS_PROMPTPORTS, NULL, 0);
		LPWSTR pszDrives = (LPWSTR )LoadString(_hInst, IDS_PROMPTDRIVES, NULL, 0);

		if ((_dwNewFlags & PROMPT_DEVICES_PORTS) && (pszPorts))
			StringCchCat(szPromptString, MAX_PATH, pszPorts);

		if ((_dwNewFlags & PROMPT_DEVICES_DRIVES) && (pszDrives))
			StringCchCat(szPromptString, MAX_PATH, pszDrives);

		SetWindowText(GetDlgItem(hwndDlg, IDC_STC_DEVICES), szPromptString);

		nRet = TRUE;
	}
	else if ( ((uMsg == WM_COMMAND) && (LOWORD(wParam) == IDCANCEL)) || 
				(uMsg == WM_CLOSE) )
	{
		nRet = TRUE;
	}
	else if ((uMsg == WM_COMMAND) && (LOWORD(wParam) == IDOK))
	{
		UpdatePromptFlags();
		nRet = TRUE;
	}

	return (CDlgBase::DialogProc(hwndDlg, uMsg, wParam, lParam) || nRet);
}
