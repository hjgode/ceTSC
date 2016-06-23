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
#include "common.h"
#include <commdlg.h>
#include <wincrypt.h>

static DWORD gadwDisableCtls[] = {	IDC_CBO_GENERAL_SERVERS,
									IDC_EDT_GENERAL_USERNAME,
									IDC_EDT_GENERAL_PASSWORD,
									IDC_EDT_GENERAL_DOMAIN,
									IDC_CHK_SAVE_PASSWORD,
								};

static DWORD gadwFileButtons[] = {
									IDC_BTN_SAVE,
									IDC_BTN_OPEN
								};


int CPropGeneral::DialogProc(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam)
{
	int nRet = FALSE;
	switch(uMsg)
	{
	case WM_INITDIALOG:
	{
		nRet = CDlgBase::DialogProc(hwndDlg, uMsg, wParam, lParam);
		if (!nRet)
			return FALSE;

		SendDlgItemMessage(hwndDlg, IDC_CBO_GENERAL_SERVERS, CB_LIMITTEXT, MAX_SERVERNAMELEN, 0);
		SendDlgItemMessage(hwndDlg, IDC_EDT_GENERAL_USERNAME, EM_LIMITTEXT, MAX_USERNAMELEN, 0);
		SendDlgItemMessage(hwndDlg, IDC_EDT_GENERAL_PASSWORD, EM_LIMITTEXT, MAX_PASSWORDLEN, 0);
		SendDlgItemMessage(hwndDlg, IDC_EDT_GENERAL_DOMAIN, EM_LIMITTEXT, MAX_DOMAINLEN, 0);

		return LoadFromSettings();
	}

	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_BTN_SAVE)
		{
			SaveRdpFile();
			return TRUE;
		}
		else if (LOWORD(wParam) == IDC_BTN_OPEN)
		{
			LoadRdpFile();
			return TRUE;
		}
		break;

	case MSG_TSC_CONNECT:
	{
		SaveToSettings();
		EnableControls(_hwndDlg, gadwDisableCtls, sizeof(gadwDisableCtls)/sizeof(gadwDisableCtls[0]), FALSE);
		if (!_fDisableFileAccess)
			EnableControls(_hwndDlg, gadwFileButtons, sizeof(gadwFileButtons)/sizeof(gadwFileButtons[0]), FALSE);
		return TRUE;
	}

	case MSG_TSC_CONNECTED:
	{
		UpdateServersCombo(_pSettings,GetDlgItem(_hwndDlg, IDC_CBO_GENERAL_SERVERS));
		return TRUE;
	}

	case MSG_TSC_DISCONNECTED:
	{
		EnableControls(_hwndDlg, gadwDisableCtls, sizeof(gadwDisableCtls)/sizeof(gadwDisableCtls[0]), TRUE);
		if (!_fDisableFileAccess)
			EnableControls(_hwndDlg, gadwFileButtons, sizeof(gadwFileButtons)/sizeof(gadwFileButtons[0]), TRUE);
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

BOOL CPropGeneral::LoadFromSettings()
{
	WCHAR szProp[MAX_PATH*2];
	WCHAR pszEmpty[] = L"";
	szProp[0] = L'\0';

	TscPropType eType = TscTypeSTRING;

	if (!InitServersCombo(_pSettings, GetDlgItem(_hwndDlg, IDC_CBO_GENERAL_SERVERS)) )
	{
		DEBUGMSG(ZONE_ERROR, (L"Failed to initialize servers combo\n"));
		return FALSE;
	}

	ASSERT(_pSettings);
	DWORD dwSize = sizeof(szProp);
	if ( (_pSettings->Read(PROP_USERNAME, &eType, (PBYTE)pszEmpty, (PBYTE)szProp, &dwSize)) && 
		 (eType == TscTypeSTRING) || (dwSize < sizeof(szProp)) )
	{
		SetWindowText(GetDlgItem(_hwndDlg, IDC_EDT_GENERAL_USERNAME), szProp);
	}

	dwSize = sizeof(szProp);
	if ( (_pSettings->Read(PROP_DOMAIN, &eType, (PBYTE)pszEmpty, (PBYTE)szProp, &dwSize)) && 
		 (eType == TscTypeSTRING) || (dwSize < sizeof(szProp)) )
	{
		SetWindowText(GetDlgItem(_hwndDlg, IDC_EDT_GENERAL_DOMAIN), szProp);
	}

	//This is to disable access to the file system from the WBT shell
	//If you want to give access to the file system, either remove the DisableFileAccess 
	//registry entry under HKCU\Software\Microsoft\Terminal Server Client\Default or comment out the code below
	DWORD dwVal = 0;
	dwSize = sizeof(dwVal);
	_fDisableFileAccess = ( (_pSettings->Read(PROP_DISABLEFILEACCESS, &eType, NULL, (PBYTE)&dwVal, &dwSize)) && (dwVal == 1));
	if (_fDisableFileAccess)
	{
		EnableWindow(GetDlgItem(_hwndDlg, IDC_BTN_SAVE), FALSE);
		EnableWindow(GetDlgItem(_hwndDlg, IDC_BTN_OPEN), FALSE);
		_fDisableFileAccess = TRUE;
	}
		 

	dwSize = 0;
	if ( (!_pSettings->Read(PROP_CLEARTEXTPASSWORD, &eType, (PBYTE)pszEmpty, NULL, &dwSize)) || (dwSize == 0) || (eType != TscTypeBINARY))
		return TRUE;

	PBYTE pbPassword = NULL;
	__try
	{
		pbPassword = (PBYTE )_alloca(dwSize);
	}
	__except(1)
	{
		DEBUGMSG(ZONE_ERROR, (L"Out of memory!\n"));
		return FALSE;
	}

	DWORD dwSize1 = dwSize;
	if ( (!_pSettings->Read(PROP_CLEARTEXTPASSWORD, &eType, (PBYTE)pszEmpty, pbPassword, &dwSize)) || (dwSize == 0) || (eType != TscTypeBINARY) )
		return FALSE;


	DATA_BLOB blobIn, blobOut;
	blobIn.cbData = dwSize;
	blobIn.pbData = pbPassword;
	blobOut.cbData = 0;
	blobOut.pbData = NULL;
//	LPWSTR pszDescr = L"rdp";
	if (!CryptUnprotectData(&blobIn, NULL, NULL, NULL, NULL, CRYPTPROTECT_UI_FORBIDDEN, &blobOut))
	{
		DEBUGMSG(ZONE_ERROR, (L"Password decryption failed\n"));
		return FALSE;
	}

	ASSERT(blobOut.pbData);

	SetWindowText(GetDlgItem(_hwndDlg, IDC_EDT_GENERAL_PASSWORD), (LPWSTR )blobOut.pbData);

	LocalFree(blobOut.pbData);

	return TRUE;
}


BOOL CPropGeneral::SaveToSettings()
{
	WCHAR szProp[MAX_PATH*2];
	int nLen, cch = MAX_PATH*2;

	ASSERT(_pSettings);

	if (!SaveServersCombo(_pSettings, GetDlgItem(_hwndDlg, IDC_CBO_GENERAL_SERVERS)) )
	{
		DEBUGMSG(ZONE_ERROR, (L"Failed to save servers combo\n"));
		return FALSE;
	}

	nLen = GetWindowText(GetDlgItem(_hwndDlg, IDC_EDT_GENERAL_USERNAME), szProp, cch);

	//TODO: Trim spaces
	if (!_pSettings->Write(PROP_USERNAME, TscTypeSTRING, (PBYTE)szProp, (nLen + 1)*sizeof(WCHAR) ) )
	{
		return FALSE;
	}

	nLen = GetWindowText(GetDlgItem(_hwndDlg, IDC_EDT_GENERAL_DOMAIN), szProp, cch);

	if (!_pSettings->Write(PROP_DOMAIN, TscTypeSTRING, (PBYTE)szProp, (nLen + 1)*sizeof(WCHAR) ) )
	{
		return FALSE;
	}


	BOOL bRet = TRUE;

	nLen = GetWindowText(GetDlgItem(_hwndDlg, IDC_EDT_GENERAL_PASSWORD), szProp, cch);

	DATA_BLOB blobIn, blobOut;
	blobIn.cbData = (nLen+1)*sizeof(WCHAR);
	blobIn.pbData = (PBYTE )szProp;
	blobOut.cbData = 0;
	blobOut.pbData = NULL;
	if (!CryptProtectData(&blobIn, L"rdp", NULL, NULL, NULL, CRYPTPROTECT_UI_FORBIDDEN, &blobOut))
	{
		DEBUGMSG(ZONE_ERROR, (L"Password encryption failed\n"));
		return FALSE;
	}

	ASSERT(blobOut.pbData);

	bRet = _pSettings->Write(PROP_CLEARTEXTPASSWORD, TscTypeBINARY, blobOut.pbData, blobOut.cbData);
	
	LocalFree(blobOut.pbData);

#ifdef DEBUG
	DWORD dwVal = (SendDlgItemMessage(_hwndDlg, IDC_CHK_SAVE_PASSWORD, BM_GETCHECK, 0, 0) & BST_CHECKED) ? 1 : 0;
	if (!_pSettings->Write(PROP_SAVEPASSWORD, TscTypeULONG, (PBYTE )&dwVal, sizeof(DWORD)))
	{
		return FALSE;
	}
#endif

	return bRet;
}

BOOL CPropGeneral::LoadRdpFile()
{
	CFileSettings rdpFile;
	WCHAR szRdpFile[MAX_PATH];

	szRdpFile[0] = L'\0';

	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = _hwndDlg;
	ofn.hInstance = _hInst;
	ofn.lpstrFilter = (WCHAR *)LoadString(_hInst, IDS_FILTERSTRING, NULL, 0);
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = szRdpFile;
	ofn.nMaxFile = sizeof(szRdpFile)/sizeof(WCHAR);
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrDefExt = (WCHAR *)LoadString(_hInst, IDS_DEFEXT, NULL, 0);
	ofn.Flags = OFN_PATHMUSTEXIST;

	if (!GetOpenFileName(&ofn))
		return FALSE;

	if (!InitializeDefaultSettings(&rdpFile))
	{
		goto Cleanup;
	}

	if(ERROR_SUCCESS != rdpFile.Open(szRdpFile, TRUE))
	{
		goto Cleanup;
	}

	if (!rdpFile.Copy(_pSettings, PROP_MRU, TRUE))
	{
		goto Cleanup;
	}

	_pSettings->Close();
	
	if (!_pSettings->Copy(&rdpFile, NULL, TRUE))
	{
		goto Cleanup;
	}

	StringCchCopy(_szRdpFile, MAX_PATH, szRdpFile);
	
	if (!LoadFromSettings())
	{
		goto Cleanup;
	}
	
	return TRUE;

Cleanup:
	WCHAR szMsg[2*MAX_PATH];
	LPWSTR pszTitle = (LPWSTR )LoadString(_hInst, IDS_RDP, NULL, 0);
	LPWSTR pszText = (LPWSTR )LoadString(_hInst, IDS_ERR_LOAD, NULL, 0);
	if ((pszText) && (pszTitle) && (S_OK == StringCchPrintf(szMsg, 2*MAX_PATH, pszText, szRdpFile) ) )
	{
		MessageBox(_hwndOwner, szMsg, pszTitle, MB_OK|MB_SETFOREGROUND);
	}
	PostMessage(_hwndOwner, WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED), (LPARAM)GetDlgItem(GetParent(_hwndOwner), IDCANCEL) ); 
	return FALSE;
}

BOOL CPropGeneral::SaveRdpFile()
{
	CFileSettings rdpFile;
	WCHAR szRdpFile[MAX_PATH];
	HWND hwndPwd;

	if (!SaveToSettings())
	{
		return FALSE;
	}

	StringCchCopy(szRdpFile, MAX_PATH, _szRdpFile);

	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = _hwndDlg;
	ofn.hInstance = _hInst;
	ofn.lpstrFilter = (WCHAR *)LoadString(_hInst, IDS_FILTERSTRING, NULL, 0);
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = szRdpFile;
	ofn.nMaxFile = sizeof(szRdpFile)/sizeof(WCHAR);
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrDefExt = (WCHAR *)LoadString(_hInst, IDS_DEFEXT, NULL, 0);
	ofn.Flags = OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT;

	if (!GetSaveFileName(&ofn))
		return FALSE;


	LONG lRet = rdpFile.Open(szRdpFile, FALSE);
	if ((lRet != ERROR_SUCCESS) && (lRet != ERROR_FILE_NOT_FOUND))
	{
		goto Cleanup;
	}

	if (!rdpFile.Copy(_pSettings, NULL, FALSE))
	{
		goto Cleanup;
	}

	hwndPwd = GetDlgItem(_hwndDlg,IDC_CHK_SAVE_PASSWORD);
	if (!IsWindow(hwndPwd) || (!IsWindowVisible(hwndPwd)) || (SendMessage(hwndPwd, BM_GETCHECK, 0, 0) == BST_UNCHECKED))
	{
		rdpFile.Delete(PROP_SAVEPASSWORD);
		rdpFile.Delete(PROP_CLEARTEXTPASSWORD);
	}

	DeleteFixedupSettings(&rdpFile);

	if (!rdpFile.Commit())
	{
		goto Cleanup;
	}
	
	StringCchCopy(_szRdpFile, MAX_PATH, szRdpFile);

	return TRUE;

Cleanup:
	WCHAR szMsg[2*MAX_PATH];
	LPWSTR pszTitle = (LPWSTR )LoadString(_hInst, IDS_RDP, NULL, 0);
	LPWSTR pszText = (LPWSTR )LoadString(_hInst, IDS_ERR_SAVE, NULL, 0);
	if ((pszText) && (pszTitle) && (S_OK == StringCchPrintf(szMsg, 2*MAX_PATH, pszText, szRdpFile) ) )
	{
		MessageBox(_hwndOwner, szMsg, pszTitle, MB_OK|MB_SETFOREGROUND);
	}
	return FALSE;
}
