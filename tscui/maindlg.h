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
#ifndef _MAINDLG_H_
#define _MAINDLG_H_

#include "dlgbase.h"
#include "settings.h"
#include "tscui.h"

#define		MAX_TABS					5

#define		LOGON_DLG_EXPAND_AMOUNT		177

extern DWORD	gdwTabOrder[MAX_TABS];

class CMainDlg : public CDlgBase
{
public:
	CMainDlg(HWND hwndParent, HINSTANCE hInst, ISettings *pSettings);
	~CMainDlg();

protected:
	int		OnInitDialog(HWND hwndDlg, WPARAM wParam, LPARAM lParam);
	void	OnConnect();
	void	OnOptions();
	void	OnTabSelChange();

	void	RecalcLayout(BOOL fExpanded);

	BOOL	AddTab(int nIndex, UINT uCaptionID);
	BOOL	InitTabs();

	virtual int DialogProc(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam);

protected:
	HWND		_hwndTab;
	HWND		_hwndServers;
	
	CDlgBase	*_pCurTab;
	RECT		_rcTab;
	
	CDlgBase	*_pdlgTabs[MAX_TABS];
	ISettings   *_pSettings;
	
	BOOL		_fExpanded;
	int			_nOptionsDelta;
	State		_state;
};

#endif
