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
#include <tsperf.h>		//CHECK IF THIS CAN BE IN THE PUBLIC TREE

//for the first four items dwVal represents the number of checkboxes that have to be checked
//for the fifth, (custom) dwVal is a bitmask of the checkboxes to be checked

ComboItem gaPerfCombo[] = {	{0, MAKELPARAM(1, 1 ),	IDS_PERF_28K		},
							{1, MAKELPARAM(2, 3 ),	IDS_PERF_56K		},
							{2, MAKELPARAM(4, 15),	IDS_PERF_BROADBAND	},
							{3,	MAKELPARAM(5, 31),	IDS_PERF_LAN		},
							{4,	MAKELPARAM(0, 0 ),	IDS_PERF_CUSTOM		}
						};

DWORD gadwPerfChkBox[] = {	IDC_CHK_BITMAP_CACHING, 
							IDC_CHK_THEMES, 
							IDC_CHK_MENU_ANIMATIONS, 
							IDC_CHK_SHOW_FWD,
							IDC_CHK_DESKTOP_BKND,
						};

static DWORD gadwDisableCtls[] = {	IDC_STC_CHOOSE_SPEED,
									IDC_CBO_PERF_OPTIMIZE,
									IDC_STC_OPTIMIZE_PERF,
									IDC_CHK_DESKTOP_BKND,
									IDC_CHK_SHOW_FWD,
									IDC_CHK_MENU_ANIMATIONS,
									IDC_CHK_THEMES,
									IDC_CHK_BITMAP_CACHING,
									IDC_CHK_ENABLE_ARC
								};


#define MAX_COMBO_ITEMS			(sizeof(gaPerfCombo)/sizeof(gaPerfCombo[0]))
#define MAX_PERF_CHECK_BOXES	(sizeof(gadwPerfChkBox)/sizeof(gadwPerfChkBox[0]))

int CPropPerf::DialogProc(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
	{
		if (!CDlgBase::DialogProc(hwndDlg, uMsg, wParam, lParam))
			return FALSE;

		InitCombo(_hInst, _hwndDlg, IDC_CBO_PERF_OPTIMIZE, gaPerfCombo, sizeof(gaPerfCombo)/sizeof(ComboItem));

		return LoadFromSettings();
		break;
	}

	case WM_COMMAND:
	{	
		if (HIWORD(wParam) == CBN_SELCHANGE)
		{
			int nSel = SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);
			if (nSel < 0)
				break;

			for (int i=0; i<MAX_PERF_CHECK_BOXES; i++)
			{
				BOOL fCheck = (nSel == MAX_COMBO_ITEMS - 1) ?  (HIWORD(gaPerfCombo[nSel].dwVal) & (1 << i) ) : (i < LOWORD(gaPerfCombo[nSel].dwVal) );
				SendDlgItemMessage(_hwndDlg, gadwPerfChkBox[i], BM_SETCHECK, fCheck ? BST_CHECKED : BST_UNCHECKED, 0);
			}
			break;
		}

		if (HIWORD(wParam) == BN_CLICKED)
		{
			UpdatePerfComboBox();
			break;
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
		EnableControls(_hwndDlg, gadwDisableCtls, sizeof(gadwDisableCtls)/sizeof(gadwDisableCtls[0]), TRUE);
		return TRUE;
	}

	case WM_DESTROY:
	{
		return SaveToSettings();
	}

	default:
		break;
	}
	return CDlgBase::DialogProc(hwndDlg, uMsg, wParam, lParam);

}

void CPropPerf::UpdatePerfComboBox()
{
	DWORD dwVal = 0;
	int nSel = CB_ERR;
	for (int i=0; i<MAX_PERF_CHECK_BOXES; i++)
	{
		if (SendDlgItemMessage(_hwndDlg, gadwPerfChkBox[i], BM_GETCHECK, 0, 0) == BST_CHECKED)
			dwVal |= (1 << i);
	}
	int i;
	for (i=0; i<MAX_PERF_CHECK_BOXES; i++)
	{
		if (dwVal == HIWORD(gaPerfCombo[i].dwVal) )
		{
			nSel = i;
			break;
		}
	}

	if (nSel == CB_ERR)
	{
		nSel = MAX_COMBO_ITEMS - 1;
		gaPerfCombo[nSel].dwVal = MAKELONG(0, dwVal);
	}

	SendDlgItemMessage(_hwndDlg, IDC_CBO_PERF_OPTIMIZE, CB_SETCURSEL, nSel, 0);

}

BOOL CPropPerf::LoadFromSettings()
{
	DWORD dwVal, dwDefVal;
	TscPropType eType = TscTypeBOOL;
	DWORD dwSize = sizeof(DWORD);

	ASSERT(_pSettings);

	dwDefVal = 1;
	if ( (!_pSettings->Read(PROP_ENABLEAUTORECONNECT, &eType, (PBYTE)&dwDefVal, (PBYTE)&dwVal, &dwSize)) ||
		 (dwSize != sizeof(DWORD) || ((dwVal != 0) && (dwVal != 1)) ) )
	{
		DEBUGMSG(ZONE_UI, (L"Invalid value for EnableAutoReconnect : %d\n", dwVal));
		return FALSE;
	}

	SendDlgItemMessage(_hwndDlg, IDC_CHK_ENABLE_ARC, BM_SETCHECK, dwVal ? BST_CHECKED : BST_UNCHECKED, 0);


	//Performance Flags
	dwDefVal = 1;
	if ( (!_pSettings->Read(PROP_DISABLEWALLPAPER, &eType, (PBYTE)&dwDefVal, (PBYTE)&dwVal, &dwSize)) ||
		 (dwSize != sizeof(DWORD) || ((dwVal != 0) && (dwVal != 1)) ) )
	{
		DEBUGMSG(ZONE_UI, (L"Invalid value for Disable Wallpaper : %d\n", dwVal));
		return FALSE;
	}
	SendDlgItemMessage(_hwndDlg, IDC_CHK_DESKTOP_BKND, BM_SETCHECK, dwVal ? BST_UNCHECKED : BST_CHECKED, 0);

	dwDefVal = 1;
	if ( (!_pSettings->Read(PROP_DISABLEFULLWINDOWDRAG, &eType, (PBYTE)&dwDefVal, (PBYTE)&dwVal, &dwSize)) ||
		 (dwSize != sizeof(DWORD) || ((dwVal != 0) && (dwVal != 1)) ) )
	{
		DEBUGMSG(ZONE_UI, (L"Invalid value for Disable Full Window Drag : %d\n", dwVal));
		return FALSE;
	}
	SendDlgItemMessage(_hwndDlg, IDC_CHK_SHOW_FWD, BM_SETCHECK, dwVal ? BST_UNCHECKED : BST_CHECKED, 0);
	
	dwDefVal = 1;
	if ( (!_pSettings->Read(PROP_DISABLEMENUANIMS, &eType, (PBYTE)&dwDefVal, (PBYTE)&dwVal, &dwSize)) ||
		 (dwSize != sizeof(DWORD) || ((dwVal != 0) && (dwVal != 1)) ) )
	{
		DEBUGMSG(ZONE_UI, (L"Invalid value for Disable Menu Animations: %d\n", dwVal));
		return FALSE;
	}
	SendDlgItemMessage(_hwndDlg, IDC_CHK_MENU_ANIMATIONS, BM_SETCHECK, dwVal ? BST_UNCHECKED : BST_CHECKED, 0);

	dwDefVal = 1;
	if ( (!_pSettings->Read(PROP_DISABLETHEMES, &eType, (PBYTE)&dwDefVal, (PBYTE)&dwVal, &dwSize)) ||
		 (dwSize != sizeof(DWORD) || ((dwVal != 0) && (dwVal != 1)) ) )
	{
		DEBUGMSG(ZONE_UI, (L"Invalid value for Disable Themes: %d\n", dwVal));
		return FALSE;
	}
	SendDlgItemMessage(_hwndDlg, IDC_CHK_THEMES, BM_SETCHECK, dwVal ? BST_UNCHECKED : BST_CHECKED, 0);

	dwDefVal = 1;
	if ( (!_pSettings->Read(PROP_BITMAPPERSISTENCE, &eType, (PBYTE)&dwDefVal, (PBYTE)&dwVal, &dwSize)) ||
		 (dwSize != sizeof(DWORD) || ((dwVal != 0) && (dwVal != 1)) ) )
	{
		DEBUGMSG(ZONE_UI, (L"Invalid value for Bitmap Cache Persistence: %d\n", dwVal));
		return FALSE;
	}
	SendDlgItemMessage(_hwndDlg, IDC_CHK_BITMAP_CACHING, BM_SETCHECK, dwVal ? BST_CHECKED : BST_UNCHECKED, 0);

	UpdatePerfComboBox();

	return TRUE;
}

BOOL CPropPerf::SaveToSettings()
{
	DWORD dwVal = 0;

	ASSERT(_pSettings);

	dwVal = (SendDlgItemMessage(_hwndDlg, IDC_CHK_ENABLE_ARC, BM_GETCHECK, 0, 0) == BST_CHECKED) ? 1 : 0;
	if (!_pSettings->Write(PROP_ENABLEAUTORECONNECT, TscTypeBOOL, (PBYTE )&dwVal, sizeof(DWORD)))
	{
		return FALSE;
	}

	// The following values are converted into PerformanceFlags before they are applied
	dwVal = (SendDlgItemMessage(_hwndDlg, IDC_CHK_DESKTOP_BKND, BM_GETCHECK, 0, 0) == BST_CHECKED) ? 0 : 1;
	if (!_pSettings->Write(PROP_DISABLEWALLPAPER, TscTypeBOOL, (PBYTE )&dwVal, sizeof(DWORD)))
	{
		return FALSE;
	}

	dwVal = (SendDlgItemMessage(_hwndDlg, IDC_CHK_SHOW_FWD, BM_GETCHECK, 0, 0) == BST_CHECKED) ? 0 : 1;
	if (!_pSettings->Write(PROP_DISABLEFULLWINDOWDRAG, TscTypeBOOL, (PBYTE )&dwVal, sizeof(DWORD)))
	{
		return FALSE;
	}

	dwVal = (SendDlgItemMessage(_hwndDlg, IDC_CHK_MENU_ANIMATIONS, BM_GETCHECK, 0, 0) == BST_CHECKED) ? 0 : 1;
	if (!_pSettings->Write(PROP_DISABLEMENUANIMS, TscTypeBOOL, (PBYTE )&dwVal, sizeof(DWORD)))
	{
		return FALSE;
	}

	dwVal = (SendDlgItemMessage(_hwndDlg, IDC_CHK_THEMES, BM_GETCHECK, 0, 0) == BST_CHECKED) ? 0 : 1;
	if (!_pSettings->Write(PROP_DISABLETHEMES, TscTypeBOOL, (PBYTE )&dwVal, sizeof(DWORD)))
	{
		return FALSE;
	}

	dwVal = (SendDlgItemMessage(_hwndDlg, IDC_CHK_BITMAP_CACHING, BM_GETCHECK, 0, 0) == BST_CHECKED) ? 1 : 0;
	if (!_pSettings->Write(PROP_BITMAPPERSISTENCE, TscTypeBOOL, (PBYTE )&dwVal, sizeof(DWORD)))
	{
		return FALSE;
	}

	return TRUE;
}

