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

CMainDlg	*gpMainDlg = NULL;

HWND StartTscUI(HWND hwndParent, HINSTANCE hInst, ISettings *pSettings)
{
	ASSERT(!gpMainDlg);

    INITCOMMONCONTROLSEX cex;
    cex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    cex.dwICC  = ICC_TAB_CLASSES;

    if(!InitCommonControlsEx( &cex ))
    {
		ASSERT(FALSE);
		return FALSE;
    }

	gpMainDlg = new CMainDlg(NULL, hInst, pSettings);
	if (!gpMainDlg)
		return NULL;

	HWND hWnd = gpMainDlg->DoModeless();
	gpMainDlg->SetOwner(hwndParent);

	return hWnd;
}

void CloseTscUI()
{
	ASSERT(gpMainDlg);
	gpMainDlg->Destroy();
	
	delete gpMainDlg;
	
	gpMainDlg = NULL;
}

