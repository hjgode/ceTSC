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

