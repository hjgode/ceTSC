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