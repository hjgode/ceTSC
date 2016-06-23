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
#include <commctrl.h>


static ComboItem gaColorDepth[] = {	{0,	 1,	IDS_COLOR_8BPP	},
								{1,	 3,	IDS_COLOR_16BPP	},
								{2,	 4,	IDS_COLOR_24BPP	}
							};

static DWORD gadwDisableCtls[] = {	IDC_STC_COLORPREVIEW,
									IDC_CBO_COLOR_DEPTH,
									IDC_CHK_DISPLAY_BBAR,
									IDC_SLD_DESKTOP_SIZE
								};

static DWORD gadwDesktopSize[] = {	MAKELPARAM(640, 480),
									MAKELPARAM(800, 600),
									MAKELPARAM(1024, 768),
									MAKELPARAM(1152, 864),
									MAKELPARAM(1280, 1024),
									MAKELPARAM(1600, 1200)
								};

static DWORD gadwDesktopSizeIds[] = {	IDS_640_480,
										IDS_800_600,
										IDS_1024_768,
										IDS_1152_864,
										IDS_1280_1024,
										IDS_1600_1200
									};

								

#define NUM_BIT_DEPTHS	(sizeof(gaColorDepth)/sizeof(gaColorDepth[0]))
#define NUM_DESKTOP_SIZES (sizeof(gadwDesktopSizeIds)/sizeof(gadwDesktopSizeIds[0]))

int CPropDisplay::DialogProc(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam)
{
	int nRet = FALSE;
	switch(uMsg)
	{
	case WM_INITDIALOG:
	{
		nRet = CDlgBase::DialogProc(hwndDlg, uMsg, wParam, lParam);
		if (!nRet)
			return FALSE;

		if (!InitColorCombo())
			return FALSE;

		if (!InitDesktopSizeSlider())
			return FALSE;

		return LoadFromSettings();
	}

	case WM_COMMAND:
	{	
		if ((LOWORD(wParam) == IDC_CBO_COLOR_DEPTH) && (HIWORD(wParam) == CBN_SELCHANGE) )
		{
			UpdateColorBox();
			nRet = TRUE;
		}
		
		break;
	}

	case WM_HSCROLL:
		UpdateDesktopSizeSlider();
		break;

	case MSG_TSC_CONNECT:
	{
		SaveToSettings();
		EnableControls(_hwndDlg, gadwDisableCtls, sizeof(gadwDisableCtls)/sizeof(gadwDisableCtls[0]), FALSE);
		return TRUE;
	}

	case MSG_TSC_DISCONNECTED:
	{
		EnableControls(_hwndDlg, gadwDisableCtls, sizeof(gadwDisableCtls)/sizeof(gadwDisableCtls[0]), TRUE);
		return TRUE;
	}

	case WM_DESTROY:
	{
		nRet = SaveToSettings();
		SendDlgItemMessage(_hwndDlg, IDC_CBO_COLOR_DEPTH, CB_SETCURSEL, -1, 0);
		UpdateColorBox();
	}

	default:
		break;
	}
	return CDlgBase::DialogProc(hwndDlg, uMsg, wParam, lParam) || nRet;
}

void CPropDisplay::UpdateColorBox()
{
	int nSel = SendDlgItemMessage(_hwndDlg, IDC_CBO_COLOR_DEPTH, CB_GETCURSEL, 0, 0);
	UINT nID = (nSel < 0) ? 0 : ((nSel > 0) ? IDB_COLOR16 : IDB_COLOR8);

	HBITMAP hBmp = (HBITMAP)LoadImage(_hInst, MAKEINTRESOURCE(nID), IMAGE_BITMAP, 0, 0, 0);

	HBITMAP hBmpOld = (HBITMAP )SendDlgItemMessage(_hwndDlg, IDC_STC_COLORPREVIEW, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
	if (hBmpOld)
		DeleteObject(hBmpOld);
}

BOOL CPropDisplay::InitColorCombo()
{
    HDC hdc = GetDC(NULL);
	if (!hdc)
		return FALSE;

	int nBpp = GetDeviceCaps(hdc, BITSPIXEL);
	ReleaseDC(NULL, hdc);

	InitCombo(_hInst, _hwndDlg, IDC_CBO_COLOR_DEPTH, gaColorDepth, min(nBpp/8, NUM_BIT_DEPTHS));

	return TRUE;
}

int CPropDisplay::IndexFromSize(DWORD dwCx, DWORD dwCy)
{
	int nCount = NUM_DESKTOP_SIZES;
	int i;
	for (i=0; i<nCount; i++)
	{
		if ((LOWORD(gadwDesktopSize[i]) > dwCx) || (HIWORD(gadwDesktopSize[i]) > dwCy))
			break;
	}
	ASSERT(i < nCount);
	return i - 1;
}

BOOL CPropDisplay::InitDesktopSizeSlider()
{
    DWORD dwCx, dwCy;
	dwCx = GetSystemMetrics(SM_CXSCREEN);
	dwCy = GetSystemMetrics(SM_CYSCREEN);
	
	_nMaxSliderPos = IndexFromSize(dwCx, dwCy) + 1;	//+1 for the FullScreen point at the end
	
	_hwndSlider = GetDlgItem(_hwndDlg, IDC_SLD_DESKTOP_SIZE);
	ASSERT(_hwndSlider);

	SendMessage(_hwndSlider, TBM_SETRANGE, TRUE, MAKELONG(0, _nMaxSliderPos));
	SendMessage(_hwndSlider, TBM_SETTICFREQ, 1, 0);
	SendMessage(_hwndSlider, TBM_SETPAGESIZE, 0, 1);
	return TRUE;
}

void CPropDisplay::UpdateDesktopSizeSlider()
{
	int nSel = SendMessage(_hwndSlider, TBM_GETPOS, 0, 0);
	if ((nSel >=0 ) && (nSel < NUM_DESKTOP_SIZES))
	{
		DWORD dwStrId = (nSel == _nMaxSliderPos) ? dwStrId = IDS_FULLSCREEN : gadwDesktopSizeIds[nSel];
		SetWindowText(GetDlgItem(_hwndDlg, IDC_STC_DESKTOP_SIZE), (LPCWSTR )LoadString(_hInst, dwStrId, NULL, 0));
	}
}


void CPropDisplay::RecalcLayout()
{
	TscPropType eType = TscTypeBOOL;
	DWORD dwVal, dwDefVal, dwSize = sizeof(DWORD);
	dwDefVal = 1;
	_pSettings->Read(PROP_FULLSCREENALWAYS, &eType, (PBYTE)&dwDefVal, (PBYTE)&dwVal, &dwSize);
	if (!dwVal)
		return;

	POINT ptGrpSize = {0,0}, ptTemp = {0, 0};
	HWND hwndTemp;
	int cy;
	
	hwndTemp = GetDlgItem(_hwndDlg, IDC_GRP_DESKTOP_SIZE);
	MapWindowPoints(hwndTemp, _hwndDlg, &ptGrpSize, 1);
	ShowWindow(hwndTemp, SW_HIDE);

	ShowWindow(GetDlgItem(_hwndDlg, IDC_STC_MONITOR), SW_HIDE);
	ShowWindow(GetDlgItem(_hwndDlg, IDC_STC_CHOOSESIZE), SW_HIDE);
	ShowWindow(GetDlgItem(_hwndDlg, IDC_STC_LESS), SW_HIDE);
	ShowWindow(GetDlgItem(_hwndDlg, IDC_STC_MORE), SW_HIDE);
	ShowWindow(GetDlgItem(_hwndDlg, IDC_STC_DESKTOP_SIZE), SW_HIDE);
	ShowWindow(GetDlgItem(_hwndDlg, IDC_SLD_DESKTOP_SIZE), SW_HIDE);

	hwndTemp = GetDlgItem(_hwndDlg, IDC_GRP_COLORS);
	MapWindowPoints(hwndTemp, _hwndDlg, &ptTemp, 1);
	cy = ptTemp.y - ptGrpSize.y;
	
	ShiftWindow(GetDlgItem(_hwndDlg, IDC_CBO_COLOR_DEPTH), 0, -cy);
	ShiftWindow(GetDlgItem(_hwndDlg, IDC_STC_COLORPREVIEW), 0, -cy);
	ShiftWindow(GetDlgItem(_hwndDlg, IDC_STC_OVERRIDE), 0, -cy);
	ShiftWindow(GetDlgItem(_hwndDlg, IDC_CHK_DISPLAY_BBAR), 0, -cy);

	ShiftWindow(hwndTemp, 0, -cy);
}

BOOL CPropDisplay::LoadFromSettings()
{
	ASSERT(_pSettings);

	DWORD dwVal = 8, dwDefVal = 8, dwSize = sizeof(DWORD);
	TscPropType eType = TscTypeULONG;
	if ( (!_pSettings->Read(PROP_COLORDEPTH, &eType, (PBYTE)&dwDefVal, (PBYTE)&dwVal, &dwSize)) ||
		 (eType != TscTypeULONG) || (dwSize != sizeof(DWORD)) || ((dwVal < 8) && (dwVal > 32)) )
	{
		DEBUGMSG(ZONE_UI, (L"Invalid value for ColorDepth : %d\n", dwVal));
		return FALSE;
	}

	int nSel = 0;
	for (int i=0; i<NUM_BIT_DEPTHS; i++)
	{
		if (dwVal >= gaColorDepth[i].dwVal)
			nSel = i;
	}

	SendDlgItemMessage(_hwndDlg, IDC_CBO_COLOR_DEPTH, CB_SETCURSEL, nSel, 0);

	UpdateColorBox();

	dwDefVal = 1;
	if ( (!_pSettings->Read(PROP_BBARENABLED, &eType, (PBYTE)&dwDefVal, (PBYTE)&dwVal, &dwSize)) ||
		 (dwSize != sizeof(DWORD)) || ( (dwVal != 0) && (dwVal != 1) ) )
	{
		DEBUGMSG(ZONE_UI, (L"Invalid value for BBarEnabled : %d\n", dwVal));
		return FALSE;
	}

	SendDlgItemMessage(_hwndDlg, IDC_CHK_DISPLAY_BBAR, BM_SETCHECK, (dwVal == 1) ? BST_CHECKED : BST_UNCHECKED, 0);

	dwDefVal = TRUE;
	if ( (!_pSettings->Read(PROP_STARTFULLSCREEN, &eType, (PBYTE)&dwDefVal, (PBYTE)&dwVal, &dwSize)) ||
		 (dwSize != sizeof(DWORD)) || ( (dwVal != 0) && (dwVal != 1) ) )
	{
		DEBUGMSG(ZONE_UI, (L"Invalid value for StartFullScreen: %d\n", dwVal));
		return FALSE;
	}

	DWORD dwPos;
	if (dwVal == 1)
	{
		dwPos = _nMaxSliderPos;
	}
	else
	{
		DWORD dwCx, dwCy;
		dwDefVal = 640;
		if ((!_pSettings->Read(PROP_DESKTOPWIDTH, &eType, (PBYTE)&dwDefVal, (PBYTE )&dwCx, &dwSize)) || 
			(dwSize != sizeof(DWORD)))
			return FALSE;

		dwDefVal = 480;
		if ((!_pSettings->Read(PROP_DESKTOPHEIGHT, &eType, (PBYTE)&dwDefVal, (PBYTE )&dwCy, &dwSize)) || 
			(dwSize != sizeof(DWORD)))
			return FALSE;

		dwPos = IndexFromSize(dwCx, dwCy);
	}

	SendMessage(_hwndSlider, TBM_SETPOS, TRUE, dwPos);
	UpdateDesktopSizeSlider();
	
	RecalcLayout();

	return TRUE;
}

BOOL CPropDisplay::SaveToSettings()
{
	int nSel = SendDlgItemMessage(_hwndDlg, IDC_CBO_COLOR_DEPTH, CB_GETCURSEL, 0, 0);
	ASSERT(nSel != CB_ERR);

	if ((nSel < 0) || (nSel >= NUM_BIT_DEPTHS) || (!_pSettings->Write(PROP_COLORDEPTH, TscTypeULONG, (PBYTE )(&gaColorDepth[nSel].dwVal), sizeof(DWORD))))
	{
		return FALSE;
	}

	DWORD dwVal = (SendDlgItemMessage(_hwndDlg, IDC_CHK_DISPLAY_BBAR, BM_GETCHECK, 0, 0) == BST_CHECKED) ? 1 : 0;
	if (!_pSettings->Write(PROP_BBARENABLED, TscTypeBOOL, (PBYTE )&dwVal, sizeof(DWORD)))
	{
		return FALSE;
	}

	BOOL fStartFullscreen;
	DWORD dwCx, dwCy;
	nSel = SendMessage(_hwndSlider, TBM_GETPOS, 0, 0);
	if (nSel == _nMaxSliderPos)
	{
		fStartFullscreen = TRUE;
		dwCx = GetSystemMetrics(SM_CXSCREEN);
		dwCy = GetSystemMetrics(SM_CYSCREEN);
	}
	else if ((nSel >= 0) && (nSel < NUM_DESKTOP_SIZES))
	{
		fStartFullscreen = FALSE;
		dwCx = LOWORD(gadwDesktopSize[nSel]);
		dwCy = HIWORD(gadwDesktopSize[nSel]);
	}

	if (!_pSettings->Write(PROP_DESKTOPWIDTH, TscTypeULONG, (PBYTE)(&dwCx), sizeof(DWORD)))
		return FALSE;

	if (!_pSettings->Write(PROP_DESKTOPHEIGHT, TscTypeULONG, (PBYTE)(&dwCy), sizeof(DWORD)))
		return FALSE;

	if (!_pSettings->Write(PROP_STARTFULLSCREEN, TscTypeBOOL, (PBYTE )(&fStartFullscreen), sizeof(BOOL)))
	{
		return FALSE;
	}

	return TRUE;
}
