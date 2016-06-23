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
#include "utils.h"
#include <commctrl.h>

DWORD gdwTabOrder[] = {	IDS_TAB_GENERAL, 
						IDS_TAB_DISPLAY, 
						IDS_TAB_LOCALRESOURCES, 
						IDS_TAB_PROGRAMS, 
						IDS_TAB_EXPERIENCE
						};

static DWORD gadwDisableCtls[] = {	IDOK,
									IDC_STC_COMPUTER_NAME,
									IDC_CBO_SERVERS,
									IDC_BTN_OPTIONS
								};


CMainDlg::CMainDlg(HWND hwndParent, HINSTANCE hInst, ISettings *pSettings) : 
	CDlgBase(hwndParent, hInst, IDD_TS_LOGON), 
	_pSettings(pSettings)
{	
	ASSERT(_pSettings);

	_pCurTab = NULL;
	_hwndTab = NULL;
	_fExpanded = FALSE;

	_nOptionsDelta = 0;
	_state = Disconnected;
	
}


CMainDlg::~CMainDlg()
{
	for (int i=0; i<MAX_TABS; i++)
	{
		if (_pdlgTabs[i])
			delete _pdlgTabs[i];
	}
}


int CMainDlg::DialogProc(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(hwndDlg, wParam, lParam);
		
	case WM_COMMAND:
	{
		switch(LOWORD(wParam))
		{
		case IDOK:
			OnConnect();
			return TRUE;

		case IDC_BTN_OPTIONS:
			OnOptions();
			return TRUE;
		
		case IDCANCEL:
		{
			ASSERT(_state != LoggedIn);

			if (_state == Disconnected) 
			{	
				PostMessage(_hwndOwner, MSG_TSC_CLOSE, 0, 0L);	
				return TRUE;
			}
			else
			{
				_state = Disconnecting;
				PostMessage(_hwndOwner, MSG_TSC_DISCONNECT, 0, 0L);	
				return TRUE;
			}
			break;
		}

		case IDC_PREVTAB:
		case IDC_NEXTTAB:
		{
			if ((_fExpanded)	&& (_state == Disconnected))
			{
				int nCur = TabCtrl_GetCurSel(_hwndTab);
				nCur += (LOWORD(wParam) == IDC_PREVTAB) ? -1 : 1;
				nCur = (nCur < 0) ? (MAX_TABS - 1) : ((nCur >= MAX_TABS) ? 0 : nCur);
				TabCtrl_SetCurSel(_hwndTab, nCur);
				OnTabSelChange();
			}
			return TRUE;
		}

		default:
			break;
		}
		break;
	}

	case WM_HELP:
	{
		if ( (GetFileAttributes(PEGHELP_EXE) != -1) &&
			(GetFileAttributes(TSC_HELP_FILE) != -1) )
		{
			WCHAR szExe[MAX_PATH];
			WCHAR szCmdLine[MAX_PATH];
			StringCchCopy(szExe, MAX_PATH, PEGHELP_EXE);
			StringCchCopy(szCmdLine, MAX_PATH, TSC_HELP_FILE_TOPIC);
			CreateProcess(szExe, szCmdLine, 0,0,0,0,0,0,0,0);
		}
		else
		{
			LPWSTR pszMsg = (LPWSTR )LoadString(_hInst, IDS_NOHTMLHELP, NULL, 0);
			LPWSTR pszTitle = (LPWSTR )LoadString(_hInst, IDS_RDP, NULL, 0);
			if (pszMsg && pszTitle)
			{
				MessageBox(_hwndDlg, pszMsg, pszTitle, MB_OK|MB_SETFOREGROUND);
			}
		}
		break;
	}

	case MSG_TSC_CONNECTED:
	{
		_state = Connected;
		
		//if it has been connected once, change the text of the cancel button
		SetWindowText(GetDlgItem(_hwndDlg, IDCANCEL), (LPWSTR )LoadString(_hInst, IDS_CLOSE, NULL, 0));

		UpdateServersCombo(_pSettings,GetDlgItem(_hwndDlg, IDC_CBO_SERVERS));
		ShowWindow(_hwndDlg, SW_HIDE);
		if (_pCurTab)
		{
			SendMessage(_pCurTab->GetHwnd(), MSG_TSC_CONNECTED, 0, 0);
		}
		return TRUE;
	}

	case MSG_TSC_DISCONNECTED:
	{
		_state = Disconnected;

		if (_pCurTab)
		{
			SendMessage(_pCurTab->GetHwnd(), MSG_TSC_DISCONNECTED, 0, 0);
		}
		ShowWindow(_hwndDlg, SW_SHOW);
		SetForegroundWindow(_hwndDlg);
		EnableControls(_hwndDlg, gadwDisableCtls, sizeof(gadwDisableCtls)/sizeof(gadwDisableCtls[0]), TRUE);

		return TRUE;
	}

	case MSG_TSC_GETRDRSUPPORT:
		SetWindowLong(_hwndDlg, DWL_MSGRESULT, (LPARAM)(LRESULT)(SendMessage(_hwndOwner, MSG_TSC_GETRDRSUPPORT, 0, 0)) ) ;
		return TRUE;


	case WM_NOTIFY:
	{
		NMHDR *pNmhdr = (NMHDR *)lParam;
		if ( (pNmhdr->idFrom == IDC_TAB_MAIN) && (pNmhdr->code == TCN_SELCHANGE) )
			OnTabSelChange();
		return TRUE;
	}
	case WM_CLOSE:
	{
		PostMessage(_hwndOwner, MSG_TSC_CLOSE, 0, 0L);	
		return TRUE;
	}

	case WM_GETICON:
		SetWindowLong(_hwndDlg, DWL_MSGRESULT, (LPARAM)(LRESULT)(SendMessage(_hwndOwner, WM_GETICON, wParam, lParam)) ) ;
		return TRUE;

	default:
		break;
	}
	return CDlgBase::DialogProc(hwndDlg, uMsg, wParam, lParam);
}


int CMainDlg::OnInitDialog(HWND hwndDlg, WPARAM wParam, LPARAM lParam)
{
	if (!CDlgBase::DialogProc(hwndDlg, WM_INITDIALOG, wParam, lParam))
		return FALSE;

	_hwndTab = GetDlgItem(hwndDlg, IDC_TAB_MAIN);
	_pdlgTabs[0] = new CPropGeneral(hwndDlg, _hInst, _pSettings);
	_pdlgTabs[1] = new CPropDisplay(hwndDlg, _hInst, _pSettings);
	_pdlgTabs[2] = new CPropLocalRes(hwndDlg, _hInst, _pSettings);
	_pdlgTabs[3] = new CPropRun(hwndDlg, _hInst, _pSettings);
	_pdlgTabs[4] = new CPropPerf(hwndDlg, _hInst, _pSettings);

	if ( (GetFileAttributes(PEGHELP_EXE) == -1) ||
		(GetFileAttributes(TSC_HELP_FILE) == -1) )
	{
		DWORD dwStyle = GetWindowLong(hwndDlg, GWL_EXSTYLE);
		SetWindowLong(hwndDlg, GWL_EXSTYLE, (dwStyle & (~WS_EX_CONTEXTHELP)) );
	}

	for (int i=0; i<MAX_TABS; i++)
	{
		if (!_pdlgTabs[i])
			return FALSE;
	}

	_hwndServers = GetDlgItem(_hwndDlg, IDC_CBO_SERVERS);
	ASSERT(_hwndServers);

	SendMessage(_hwndServers, CB_LIMITTEXT, MAX_SERVERNAMELEN, 0);

	RECT rc;
	SetRect(&rc, 0, 0, 100, LOGON_DLG_EXPAND_AMOUNT);
	
	

	BOOL bRet = MapDialogRect(hwndDlg, &rc);
	ASSERT(bRet);
	
	_nOptionsDelta = rc.bottom - rc.top;

	InitTabs();

	RecalcLayout(FALSE);

	ShiftWindow(hwndDlg, 0, -GetSystemMetrics(SM_CYSCREEN)/4);

	if (!InitServersCombo(_pSettings, _hwndServers) )
	{
		DEBUGMSG(ZONE_ERROR, (L"Failed to initialize servers combo\n"));
		return FALSE;
	}
	
	return TRUE;
}

BOOL CMainDlg::AddTab(int nIndex, UINT uCaptionID)
{
	TCITEM ti;

	ti.mask = TCIF_TEXT;
	ti.pszText = (LPWSTR)LoadString(_hInst, uCaptionID, NULL, 0);
	if (!ti.pszText)
		return FALSE;

	if (nIndex != TabCtrl_InsertItem(_hwndTab, nIndex, &ti))
		return FALSE;

	return TRUE;
}

BOOL CMainDlg::InitTabs()
{
	for (int i=0; i<MAX_TABS; i++)
	{
		if (!AddTab(i, gdwTabOrder[i]))
			return FALSE;
	}

	GetWindowRect(_hwndTab, &_rcTab);
	TabCtrl_AdjustRect(_hwndTab, FALSE, &_rcTab);
	MapWindowPoints(NULL, _hwndDlg, (LPPOINT )&_rcTab, 2);

	return TRUE;
}

void CMainDlg::OnConnect()
{
	WCHAR szServer[MAX_PATH];
	WCHAR pszEmpty[] = L"";

	if (_pCurTab)
	{
		SendMessage(_pCurTab->GetHwnd(), MSG_TSC_CONNECT, 0, 0);
	}
	else
	{
		SaveServersCombo(_pSettings, _hwndServers);
	}

	EnableControls(_hwndDlg, gadwDisableCtls, sizeof(gadwDisableCtls)/sizeof(gadwDisableCtls[0]), FALSE);

	TscPropType eType = TscTypeSTRING;
	DWORD dwSize = sizeof(szServer);
	
	if ( (!_pSettings->Read(PROP_SERVER, &eType, (PBYTE)pszEmpty, (PBYTE)szServer, &dwSize)) || 
		 (!ValidateServerName(szServer)) )
	{
		LPWSTR pszMsg = (LPWSTR )LoadString(_hInst, IDS_INVALID_SERVER_NAME, NULL, 0);
		LPWSTR pszTitle = (LPWSTR )LoadString(_hInst, IDS_RDP, NULL, 0);
		if (pszMsg && pszTitle)
		{
			MessageBox(_hwndDlg, pszMsg, pszTitle, MB_OK|MB_SETFOREGROUND);
		}

		goto Cleanup;
	}

	if (!_pSettings->Write(PROP_SERVER, TscTypeSTRING, (PBYTE)szServer, (wcslen(szServer) + 1)*sizeof(WCHAR) ) )
	{
		goto Cleanup;
	}

	if (SendMessage(_hwndOwner, MSG_TSC_CONNECT, 0, 0) == TRUE)
	{
		_state = Connecting;
		return;
	}

Cleanup:
	if (_pCurTab)
	{
		SendMessage(_pCurTab->GetHwnd(), MSG_TSC_DISCONNECTED, 0, 0);
	}
	EnableControls(_hwndDlg, gadwDisableCtls, sizeof(gadwDisableCtls)/sizeof(gadwDisableCtls[0]), TRUE);
}

void CMainDlg::OnOptions()
{
	RecalcLayout(!_fExpanded);

	if (_fExpanded)
	{
		SaveServersCombo(_pSettings, _hwndServers);
	}

	OnTabSelChange();	//This will save the text in the server edit control back to settings
	
	if (!_fExpanded)
	{
		InitServersCombo(_pSettings, _hwndServers);
	}

	UINT uID = (_fExpanded) ? IDS_OPTIONS_LESS : IDS_OPTIONS_MORE;
	SetWindowText(GetDlgItem(_hwndDlg, IDC_BTN_OPTIONS), (LPWSTR)LoadString(_hInst, uID, NULL, 0));
}


void CMainDlg::OnTabSelChange()
{
	int nCurTab = TabCtrl_GetCurSel(_hwndTab);
	if ((nCurTab < 0) || (nCurTab > MAX_TABS))
		nCurTab = 0;

	if (_pCurTab)
	{
		_pCurTab->Destroy();
	}

	if (_fExpanded)
	{
		_pCurTab = _pdlgTabs[nCurTab];
		HWND hWnd = _pCurTab->DoModeless();
		SetWindowPos(hWnd, HWND_TOP, _rcTab.left, _rcTab.top, _rcTab.right - _rcTab.left, _rcTab.bottom - _rcTab.top, SWP_SHOWWINDOW);
	}
	else
	{
		_pCurTab = NULL;
	}
}

void CMainDlg::RecalcLayout(BOOL fExpanded)
{
	int nCmdShow = (fExpanded) ? SW_SHOW : SW_HIDE;
	int nCmdShow1 = (fExpanded) ? SW_HIDE : SW_SHOW;
	ShowWindow(GetDlgItem(_hwndDlg, IDC_TAB_MAIN), nCmdShow);
	ShowWindow(GetDlgItem(_hwndDlg, IDC_STC_COMPUTER_NAME), nCmdShow1);
	ShowWindow(GetDlgItem(_hwndDlg, IDC_CBO_SERVERS), nCmdShow1);

	ASSERT(_nOptionsDelta);
	int cy = (fExpanded ^ _fExpanded) ? ((fExpanded) ? _nOptionsDelta : -_nOptionsDelta) : 0;
	ShiftWindow(GetDlgItem(_hwndDlg, IDOK), 0, cy);
	ShiftWindow(GetDlgItem(_hwndDlg, IDCANCEL), 0, cy);
	ShiftWindow(GetDlgItem(_hwndDlg, IDC_BTN_OPTIONS), 0, cy);

	ShiftWindow(_hwndDlg, 0, 0, 0, cy);

	_fExpanded = fExpanded;
}
