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
