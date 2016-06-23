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
#include "stdafx.h"
#include "ApplicationProperties.hpp"
#include "TsConWiz.hpp"

#define REG_TSC_PATH      _T("Software\\Microsoft\\Terminal Server Client")
#define REGISTRATION_CONNECTIONS_KEY _T("Software\\Microsoft\\WBT\\Clients\\Registration\\RDP\\Connections")


CApplicationPropPage::CApplicationPropPage (CConWizPropSheet *pOwner)
						: CConWizPropPage(pOwner), m_bDesktop (TRUE), m_bFilenameValid(FALSE)
{
	DEBUGMSG (1, (L"CApplicationPropPage::CApplicationPropPage\n"));
	Init (IDD_APPLICATION_DIALOG);

	memset(&m_szAlternateShell ,0, sizeof(m_szAlternateShell));
	memset(&m_szShellWorkingDirectory ,0, sizeof(m_szShellWorkingDirectory));

	m_szAlternateShell[0] = 0;
	m_szShellWorkingDirectory[0] = 0;
}

CApplicationPropPage::~CApplicationPropPage()
{
}

LRESULT CApplicationPropPage::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DEBUGMSG (1, (L"CApplicationPropPage::OnInitDialog, class is [%s]\n", GetId()));
	CConWizPropPage::OnInitDialog (uMsg, wParam, lParam, bHandled);

	m_pConnWiz = GetOwner().GetConnWiz();
	if (!m_pConnWiz)
	{
		DEBUGMSG (1, (L"!! Error -  couldn't get owner !!\n"));
		EndDialog (0);
		return 0;
	}
	
	m_bDesktop = m_pConnWiz->m_bRunningDesktop;

	m_wndDesktopRadio.Attach (GetDlgItem(IDC_DESKTOP_BUTTON));
	m_wndFileNameRadio.Attach (GetDlgItem(IDC_APPLICATION_FILE_NAME_BUTTON));
	m_wndFileName.Attach (GetDlgItem(IDC_APPLICATION_FILE_NAME_EDIT));
	m_wndWorkingDir.Attach (GetDlgItem(IDC_WORKING_DIRECTORY_EDIT));
	m_lblWorkingDir.Attach (GetDlgItem(IDC_WORKING_DIRECTORY_LBL));

	// fill in values into UI
	if (m_bDesktop)
	{
		m_wndDesktopRadio.SendMessage (BM_SETCHECK, BST_CHECKED);
		m_wndFileName.SetWindowText (L"Desktop");
	}
	else
	{
		m_wndFileNameRadio.SendMessage (BM_SETCHECK, BST_CHECKED);
		
		m_wndFileName.SetWindowText (m_pConnWiz->m_szAlternateShell);
		m_wndWorkingDir.SetWindowText (m_pConnWiz->m_szShellWorkingDirectory);
	}
	SetDesktopRadio();
	
	// Limit the application to run and the working directory
	m_wndFileName.SendMessage (EM_SETLIMITTEXT, MAX_PATH);
	m_wndWorkingDir.SendMessage (EM_SETLIMITTEXT, MAX_PATH);

	return 1;
}


LRESULT	CApplicationPropPage::OnDesktop (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled)
{
	// get check state
	int state = Button_GetCheck (m_wndDesktopRadio);
	DEBUGMSG (1, (L"CApplicationPropPage::OnDesktop, state %d\n", state));

	m_bDesktop = (state == BST_CHECKED);

	SetDesktopRadio();
	return 1;
}




BOOL CApplicationPropPage::ReadRegistrySettings()
{
	return TRUE;
}

BOOL CApplicationPropPage::WriteRegistrySettings()
{
	m_pConnWiz->m_bRunningDesktop = m_bDesktop;
	if (!m_bDesktop)
	{
		_tcscpy (m_pConnWiz->m_szAlternateShell, m_szAlternateShell);
		_tcscpy (m_pConnWiz->m_szShellWorkingDirectory, m_szShellWorkingDirectory);
	}
	return TRUE;
}


void CApplicationPropPage::SetDesktopRadio()
{
	if (m_bDesktop)
	{
		m_wndFileName.EnableWindow (FALSE);
		m_wndWorkingDir.EnableWindow (FALSE);
        m_lblWorkingDir.EnableWindow( FALSE );
	}
	else
	{
		m_wndFileName.EnableWindow (TRUE);
		m_wndWorkingDir.EnableWindow (TRUE);
        m_lblWorkingDir.EnableWindow( TRUE );
	}
	ProcessFilenameChange();
}

BOOL CApplicationPropPage::Validate()
{
	if (!m_bDesktop)
	{
		// Make sure that all the characters in the filename are valid.
		for (unsigned int i = 0;i < _tcslen(m_szAlternateShell);i++)
		{
			TCHAR tCurrentChar = m_szAlternateShell[i]; 
			if (tCurrentChar == TCHAR('"')  ||
				tCurrentChar == TCHAR('/')  ||
				tCurrentChar == TCHAR('*')  ||
				tCurrentChar == TCHAR('\\')  ||
				tCurrentChar == TCHAR('<')  ||
				tCurrentChar == TCHAR('>')  ||
				tCurrentChar == TCHAR('|')  ||
				tCurrentChar == TCHAR('?')  ||
				tCurrentChar == TCHAR(':'))
			{
				LPCTSTR pszTitle = LoadStringEx (IDS_INVALID_FILE_NAME_TITLE);
				LPCTSTR pszMsg = LoadStringEx (IDS_INVALID_FILE_NAME_MESSAGE);
				
				TCHAR szMsg [MAX_DISPLAY_STRING] = {0};
				_tcscpy (szMsg, pszMsg);
				_tcscat (szMsg, L"\n\n");
				_tcscat (szMsg, L"\t\t");
				_tcscat (szMsg, L"\\");
				_tcscat (szMsg, L"\"");
				_tcscat (szMsg, L"/ < > * | ?");
				
				MessageBox (szMsg, pszTitle, MB_ICONINFORMATION | MB_OK);
				m_wndFileName.SendMessage (EM_SETSEL, 0, -1);
				m_wndFileName.SetFocus();
				return FALSE;			
			}
		}
		// Make sure that all the characters in the filename are valid.
	int i;
		for (i = 0;i < _tcslen(m_szShellWorkingDirectory);i++)
		{
			TCHAR tCurrentChar = m_szShellWorkingDirectory[i]; 
			if (tCurrentChar == TCHAR('*')  ||
				tCurrentChar == TCHAR('<')  ||
				tCurrentChar == TCHAR('>')  ||
				tCurrentChar == TCHAR('|')  ||
				tCurrentChar == TCHAR('?'))
			{
				LPCTSTR pszTitle = LoadStringEx (IDS_INVALID_FILE_NAME_TITLE);
				LPCTSTR pszMsg = LoadStringEx (IDS_INVALID_FILE_NAME_MESSAGE);
				
				TCHAR szMsg [MAX_DISPLAY_STRING] = {0};
				_tcscpy (szMsg, pszMsg);
				_tcscat (szMsg, L"\n\n");
				_tcscat (szMsg, L"\t\t");
				_tcscat (szMsg, L"< > * | ?");
				
				MessageBox (szMsg, pszTitle, MB_ICONINFORMATION | MB_OK);
				m_wndWorkingDir.SendMessage (EM_SETSEL, 0, -1);
				m_wndWorkingDir.SetFocus();
				return FALSE;			
			}
		}
	}
	return TRUE;
}


LRESULT CApplicationPropPage::OnSetActive (int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	DEBUGMSG (1, (L"CApplicationPropPage::OnSetActive for page [%s]\n", GetId()));
	if (m_bDesktop)
		GetOwner().SetWizFlags (PSWIZB_BACK | PSWIZB_NEXT);
	else
	{
		if (m_bFilenameValid)
			GetOwner().SetWizFlags (PSWIZB_BACK | PSWIZB_NEXT);
		else
			GetOwner().SetWizFlags (PSWIZB_BACK);
	}
	return 0;
}


LRESULT CApplicationPropPage::OnFilenameChange (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ProcessFilenameChange();
	return 0;
}

void CApplicationPropPage::ProcessFilenameChange()
{
	m_bFilenameValid = (m_wndFileName.GetWindowTextLength() > 0);
	
	BOOL bDummy;
	OnSetActive (0, 0, bDummy);
}

BOOL CApplicationPropPage::StoreData()
{
	if (!m_bDesktop)
	{
		m_wndFileName.GetWindowText (m_szAlternateShell, MAX_PATH);
		m_wndWorkingDir.GetWindowText (m_szShellWorkingDirectory, MAX_PATH);
	}
	return TRUE;
}
