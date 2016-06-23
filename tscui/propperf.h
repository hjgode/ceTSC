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
#ifndef _PROPPERF_H_
#define _PROPPERF_H_

class CPropPerf : public CDlgBase
{
public:
	CPropPerf(HWND hwndParent, HINSTANCE hInst, ISettings *pSettings)
		: CDlgBase(hwndParent, hInst, IDD_PROPPAGE_PERF), _pSettings(pSettings)
	{
		ASSERT(_pSettings);
	}
	~CPropPerf()	{}

	virtual int DialogProc(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam);

	void	UpdatePerfComboBox();

	BOOL	LoadFromSettings();
	BOOL	SaveToSettings();

protected:
	ISettings	*_pSettings;
};

#endif
