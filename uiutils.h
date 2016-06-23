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
#ifndef _UIUTILS_H_
#define _UIUTILS_H_

struct ComboItem
{
	int		nIndex;
	DWORD	dwVal;
	DWORD	dwStrId;
};

void InitCombo(HINSTANCE hInst, HWND hwndDlg, DWORD dwId, ComboItem *pItem, int nCount);

#define MAX_SERVERS_LIST	10

BOOL InitServersCombo(ISettings *pSettings, HWND hwndCombo);
BOOL SaveServersCombo(ISettings *pSettings, HWND hwndCombo);
BOOL UpdateServersCombo(ISettings *pSettings, HWND hwndCombo);

VOID EnableControls(HWND hwndParent, PDWORD pdwIds, int nCount, BOOL bEnable);
void ShiftWindow(HWND hwnd, int nXDelta, int nYDelta, int nCXDelta = 0, int nCYDelta = 0);
#endif
