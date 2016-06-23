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
#ifndef _PROPLOCALRES_H_
#define _PROPLOCALRES_H_

class CPropLocalRes : public CDlgBase
{
public:
	CPropLocalRes(HWND hwndParent, HINSTANCE hInst, ISettings *pSettings)
		: CDlgBase(hwndParent, hInst, IDD_PROPPAGE_LOCALRESOURCES), _pSettings(pSettings)
	{
		ASSERT(_pSettings);
	}
	~CPropLocalRes()	{}

	virtual int DialogProc(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam);

protected:

	void	InitSCardCheckBox();
	void	InitPrinterInfo();

	BOOL	LoadFromSettings();
	BOOL	SaveToSettings();

	BOOL	SetupPrinter();
	void	UpdateRedirectorControls();

protected:
	ISettings	*_pSettings;
	DWORD		_dwSupportedRedirectors;
};

#endif
