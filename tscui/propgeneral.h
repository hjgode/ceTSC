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
#ifndef _PROPGENERAL_H_
#define _PROPGENERAL_H_

class CPropGeneral : public CDlgBase
{
public:
	CPropGeneral(HWND hwndParent, HINSTANCE hInst, ISettings *pSettings)
		: CDlgBase(hwndParent, hInst, IDD_PROPPAGE_GENERAL), _pSettings(pSettings)
	{
		_fDisableFileAccess = FALSE;
		ASSERT(_pSettings);
		StringCchCopy(_szRdpFile, MAX_PATH, DEFAULT_RDP_FILE_NAME);
	}
	~CPropGeneral()	{}

	virtual int DialogProc(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam);

	BOOL	LoadFromSettings();
	BOOL	SaveToSettings();

protected:
	BOOL	LoadRdpFile();
	BOOL	SaveRdpFile();

protected:
	WCHAR		_szRdpFile[MAX_PATH];
	ISettings	*_pSettings;
	BOOL		_fDisableFileAccess;
};

#endif