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
#ifndef _PROPDISPLAY_H_
#define _PROPDISPLAY_H_


class CPropDisplay : public CDlgBase
{
public:
	CPropDisplay(HWND hwndParent, HINSTANCE hInst, ISettings *pSettings)
		: CDlgBase(hwndParent, hInst, IDD_PROPPAGE_DISPLAY), _pSettings(pSettings)
	{
		ASSERT(_pSettings);
		_nMaxSliderPos = 0;
	}
	~CPropDisplay()	{}

	virtual int DialogProc(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam);

	void	UpdateColorBox();
	BOOL	InitColorCombo();

	int     IndexFromSize(DWORD dwCx, DWORD dwCy);
	BOOL	InitDesktopSizeSlider();
	void	UpdateDesktopSizeSlider();
	void	RecalcLayout();

	BOOL	LoadFromSettings();
	BOOL	SaveToSettings();

protected:
	ISettings   *_pSettings;
	int         _nMaxSliderPos;
	HWND        _hwndSlider;
};

#endif