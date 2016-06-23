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