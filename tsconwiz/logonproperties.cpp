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
#include "LogonProperties.hpp"
#include "TsConWiz.hpp"
#include "RegString.hpp"



#define REG_TSC_PATH      _T("Software\\Microsoft\\Terminal Server Client")
#define MSTSC_CLIENT_NAME _T("cetsc.exe")
#define REGISTRATION_CONNECTIONS_KEY _T("Software\\Microsoft\\WBT\\Clients\\Registration\\RDP\\Connections")
#define TSConWiz_CLIENT_NAME _T("TSConWiz")

CLogonPropPage::CLogonPropPage (CConWizPropSheet *pOwner) : 
						CConWizPropPage(pOwner), m_bAutoLogon(FALSE), m_bUserValid(FALSE), m_bDomainValid(FALSE)
{
	DEBUGMSG (1, (L"CLogonPropPage::CLogonPropPage\n"));
	Init (IDD_LOGON_DIALOG);
	m_szUserName[0] = 0;
	m_szPassword[0] = 0;
	m_szDomain[0] = 0;
}

CLogonPropPage::~CLogonPropPage()
{
}

LRESULT CLogonPropPage::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DEBUGMSG (1, (L"CLogonPropPage::OnInitDialog, class is [%s]\n", GetId()));
	CConWizPropPage::OnInitDialog (uMsg, wParam, lParam, bHandled);

	m_pConnWiz = GetOwner().GetConnWiz();
	if (!m_pConnWiz)
	{
		DEBUGMSG (1, (L"!! Error -  couldn't get owner !!\n"));
		EndDialog (0);
		return 0;
	}

	m_wndAutoLogon.Attach (GetDlgItem(IDC_AUTO_LOGON_CHECK));
	m_wndUsername.Attach (GetDlgItem(IDC_USERNAME));
	m_wndPwd.Attach (GetDlgItem(IDC_PASSWORD));
	m_wndDomain.Attach (GetDlgItem(IDC_DOMAIN));

	m_lblUsername.Attach (GetDlgItem(IDC_USERNAME_LBL));
	m_lblPwd.Attach (GetDlgItem(IDC_PASSWORD_LBL));
	m_lblDomain.Attach (GetDlgItem(IDC_DOMAIN_LBL));

	m_wndUsername.SendMessage (EM_SETLIMITTEXT, MAX_USERNAME_LENGTH);
	m_wndPwd.SendMessage (EM_SETLIMITTEXT, MAX_PASSWORD_LENGTH);
	m_wndDomain.SendMessage (EM_SETLIMITTEXT, MAX_DOMAIN_LENGTH);

	// fill in values into UI
/****************************************************************************************************/
/*											IMPORTANT												*/
/*	Since CE is not designed for a multi user environment and there is no consistent methood to lock*/
/*	out non-users, saving the password and leaving the  device unattended  poses a  serious security*/
/*	risk. There for AutoLogon has been disabled on purpose. If you think this is not a serious risk */
/*  for your environment,you can make the relevant code change and will be doing so at your own risk*/
/****************************************************************************************************/
#ifndef WBT_USEAUTOLOGON
	m_bAutoLogon = FALSE;
	m_wndAutoLogon.SendMessage (BM_SETCHECK,  m_bAutoLogon ? BST_CHECKED : BST_UNCHECKED);
	m_wndAutoLogon.EnableWindow (FALSE);
	m_lblPwd.EnableWindow (FALSE);
	m_wndPwd.EnableWindow (FALSE);
#else
	// Check to see if the current connection is set up for auto logon
	m_bAutoLogon = m_pConnWiz->m_bAutoLogon;
	m_wndAutoLogon.SendMessage (BM_SETCHECK,  m_bAutoLogon ? BST_CHECKED : BST_UNCHECKED);
	if (m_bAutoLogon)
#endif
	{
		m_wndUsername.SetWindowText (m_pConnWiz->m_szUserName);
		m_wndPwd.SetWindowText (m_pConnWiz->m_szPassword);
		m_wndDomain.SetWindowText (m_pConnWiz->m_szDomain);
	}
	SetAutoLogon();
	
	return 1;
}


LRESULT CLogonPropPage::OnAutomaticLogonCheck (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled)
{
	// get check state
	int state = Button_GetCheck (m_wndAutoLogon);
	DEBUGMSG (1, (L"CLogonPropPage::OnAutomaticLogonCheck, state %d\n", state));

	m_bAutoLogon = (state == BST_CHECKED);

	SetAutoLogon();
	return 1;
}



BOOL CLogonPropPage::Validate()
{
	// Net Connections page...make sure that the user has supplied enough information
	// for Auto Logon if they have the checkbox marked
#ifdef WBT_USEAUTOLOGON
	if (m_bAutoLogon)
#endif
	{	
		int nLen = m_wndUsername.GetWindowTextLength();
	
		if (nLen > MAX_USERNAME_LENGTH)
		{
			// Too long.
			LPCTSTR pszTitle = LoadStringEx (IDS_INVALID_USERNAME_TITLE);
			LPCTSTR pszMsg = LoadStringEx (IDS_INVALID_USERNAME_MESSAGE_1);
			
			MessageBox (pszMsg, pszTitle, MB_ICONINFORMATION | MB_OK);
			m_wndUsername.SendMessage (EM_SETSEL, 0, -1);
			m_wndUsername.SetFocus();
			return FALSE;			
		}

		// Check all the characters in the username
		for (int i = 0;i < nLen;i++)
		{
			TCHAR tCurrentChar = m_szUserName[i]; 
			if (tCurrentChar == TCHAR(' ') ||
				tCurrentChar == TCHAR('"')  ||
				tCurrentChar == TCHAR('/')  ||
				tCurrentChar == TCHAR('[')  ||
				tCurrentChar == TCHAR(']')  ||
				tCurrentChar == TCHAR(':')  ||
				tCurrentChar == TCHAR(';')  ||
				tCurrentChar == TCHAR('|')  ||
				tCurrentChar == TCHAR('=')  ||
				tCurrentChar == TCHAR(',')  ||
				tCurrentChar == TCHAR('+')  ||
				tCurrentChar == TCHAR('*')  ||
				tCurrentChar == TCHAR('?')  ||
				tCurrentChar == TCHAR('<')  ||
				tCurrentChar == TCHAR('>'))
			{
				// Bad character found.
				LPCTSTR pszTitle = LoadStringEx (IDS_INVALID_USERNAME_TITLE);
				LPCTSTR pszMsg = LoadStringEx (IDS_INVALID_USERNAME_MESSAGE_2);
				LPCTSTR pszSpace = LoadStringEx (IDS_SPACE);
				
				TCHAR szMsg [200];
				_tcscpy (szMsg, pszMsg);
				_tcscat (szMsg, L"\n\n");
				_tcscat (szMsg, L"\t\t");
				_tcscat (szMsg, L"\"");
				_tcscat (szMsg, L" /");
				_tcscat (szMsg, L" [ ] : ; | = , + * ? < > ");
				if (pszSpace)
					_tcscat (szMsg, pszSpace);
				
				MessageBox (szMsg, pszTitle, MB_ICONINFORMATION | MB_OK);
				m_wndUsername.SendMessage (EM_SETSEL, 0, -1);
				m_wndUsername.SetFocus();
				return FALSE;			
			}
		}

		// Make sure the user has provided a domain name.
		nLen = m_wndDomain.GetWindowTextLength();
		
        // Make sure that all the characters in the server name are valid.
	int i;
		for (i = 0;i < nLen;i++)
		{
			TCHAR tCurrentChar = m_szDomain[i]; 
			if (tCurrentChar == TCHAR(' ')  ||
				tCurrentChar == TCHAR('"')  ||
				tCurrentChar == TCHAR('<')  ||
				tCurrentChar == TCHAR('>')  ||
				tCurrentChar == TCHAR('*')  ||
				tCurrentChar == TCHAR('+')  ||
				tCurrentChar == TCHAR('=')  ||
				tCurrentChar == TCHAR('\\')  ||
				tCurrentChar == TCHAR('|')  ||
				tCurrentChar == TCHAR('?')  ||
				tCurrentChar == TCHAR(':')  ||
				tCurrentChar == TCHAR(','))
			{
				LPCTSTR pszTitle = LoadStringEx (IDS_INVALID_DOMAIN_NAME_TITLE);
				LPCTSTR pszMsg = LoadStringEx (IDS_INVALID_DOMAIN_NAME_MESSAGE);
				LPCTSTR pszSpace = LoadStringEx (IDS_SPACE);
				
				TCHAR szMsg [200];
				_tcscpy (szMsg, pszMsg);
				_tcscat (szMsg, L"\n\n");
				_tcscat (szMsg, L"\t\t");
				_tcscat (szMsg, L"\\");
				_tcscat (szMsg, L" : ");
				_tcscat (szMsg, L"\"");
				_tcscat (szMsg, L" < > * + = | ? , ");
				if (pszSpace)
					_tcscat (szMsg, pszSpace);
				
				MessageBox (szMsg, pszTitle, MB_ICONINFORMATION | MB_OK);
				m_wndDomain.SendMessage (EM_SETSEL, 0, -1);
				m_wndDomain.SetFocus();
				return FALSE;			
			}
		}
	}
	return TRUE;
}



LRESULT CLogonPropPage::OnSetActive (int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	DEBUGMSG (1, (L"CLogonPropPage::OnSetActive for page [%s]\n", GetId()));
	if (m_bAutoLogon)
	{
		if (m_bUserValid && m_bDomainValid)
			GetOwner().SetWizFlags (PSWIZB_BACK | PSWIZB_NEXT);
		else
			GetOwner().SetWizFlags (PSWIZB_BACK);
	}
	else
		GetOwner().SetWizFlags (PSWIZB_BACK | PSWIZB_NEXT);
	return 0;
}



void CLogonPropPage::SetAutoLogon()
{
#ifdef WBT_USEAUTOLOGON
	if (m_bAutoLogon)
	{
		m_wndUsername.EnableWindow (TRUE);
		m_wndPwd.EnableWindow (TRUE);
		m_wndDomain.EnableWindow (TRUE);
		m_lblUsername.EnableWindow (TRUE);
		m_lblPwd.EnableWindow (TRUE);
		m_lblDomain.EnableWindow (TRUE);
	}
	else
	{
		m_wndUsername.EnableWindow (FALSE);
		m_wndPwd.EnableWindow (FALSE);
		m_wndDomain.EnableWindow (FALSE);
		m_lblUsername.EnableWindow (FALSE);
		m_lblPwd.EnableWindow (FALSE);
		m_lblDomain.EnableWindow (FALSE);
	}
#endif
	ProcessUserOrDomainChange();
}



BOOL CLogonPropPage::ReadRegistrySettings()
{
	return TRUE;
}


BOOL CLogonPropPage::WriteRegistrySettings()
{
	m_pConnWiz->m_bAutoLogon = m_bAutoLogon;
	StringCchCopy (m_pConnWiz->m_szUserName, MAX_USERNAME_LENGTH, m_szUserName);
	StringCchCopy (m_pConnWiz->m_szDomain, MAX_DOMAIN_LENGTH, m_szDomain);
	StringCchCopy (m_pConnWiz->m_szPassword, MAX_PASSWORD_LENGTH, m_szPassword);

	return TRUE;
}


LRESULT CLogonPropPage::OnUserChange (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ProcessUserOrDomainChange();
	return 0;
}

LRESULT CLogonPropPage::OnDomainChange (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ProcessUserOrDomainChange();
	return 0;
}

void CLogonPropPage::ProcessUserOrDomainChange()
{
	m_bUserValid = (m_wndUsername.GetWindowTextLength() > 0);
	m_bDomainValid = (m_wndDomain.GetWindowTextLength() > 0);
	
	BOOL bDummy;
	OnSetActive (0, 0, bDummy);
}

BOOL CLogonPropPage::StoreData()
{
#ifdef WBT_USEAUTOLOGON
	if (m_bAutoLogon)
#endif
	{	
		m_wndUsername.GetWindowText (m_szUserName, MAX_USERNAME_LENGTH + 1);
		m_wndDomain.GetWindowText (m_szDomain, MAX_DOMAIN_LENGTH + 1);
		m_wndPwd.GetWindowText (m_szPassword, MAX_PASSWORD_LENGTH + 1);
	}
	return TRUE;
}
