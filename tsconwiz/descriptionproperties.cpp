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
#include "DescriptionProperties.hpp"
#include "TsConWiz.hpp"
#include "RegString.hpp"


#define REG_TSC_PATH      _T("Software\\Microsoft\\Terminal Server Client")

CDescriptionPropPage::CDescriptionPropPage (CConWizPropSheet *pOwner) : 
						CConWizPropPage(pOwner), m_bConnectionNameChanged(FALSE), m_bNameValid(FALSE), m_bServerValid(FALSE)
{
	DEBUGMSG (1, (L"CDescriptionPropPage::CDescriptionPropPage\n"));
	Init (IDD_DESCRIPTION_DIALOG);

	memset(&m_szConnectionName ,0, sizeof(m_szConnectionName));
	memset(&m_szServerName ,0, sizeof(m_szServerName));
	m_szConnectionName[0] = 0;
	m_szServerName[0] = 0;
	m_dwRdpPort = DEFAULT_RDP_PORT;
}

CDescriptionPropPage::~CDescriptionPropPage()
{
}

LRESULT CDescriptionPropPage::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DEBUGMSG (1, (L"CDescriptionPropPage::OnInitDialog, class is [%s]\n", GetId()));
	CConWizPropPage::OnInitDialog (uMsg, wParam, lParam, bHandled);

	m_pConnWiz = GetOwner().GetConnWiz();
	if (!m_pConnWiz)
	{
		DEBUGMSG (1, (L"!! Error -  couldn't get owner !!\n"));
		EndDialog (0);
		return 0;
	}

	m_wndName.Attach (GetDlgItem(IDC_CONNECTION_DESCRIPTION));
	m_wndServer.Attach (GetDlgItem(IDC_SERVER));

	// fill in values into UI

	// Limit the text in these edit fields to respective maximums
	m_wndName.SendMessage (EM_SETLIMITTEXT, MAX_CONNECTION_DESCRIPTION-1);
	m_wndServer.SendMessage (EM_SETLIMITTEXT, MAX_SERVER_NAME);

	// Place the connection and server names into the respective edit
	// fields.
	m_wndName.SetWindowText (m_pConnWiz->m_szConnectionName);
	m_wndServer.SetWindowText (m_pConnWiz->m_szServerName);

#ifdef WBT_USELOWSPEED
	m_bLowSpeed = m_pConnWiz->m_bLowSpeedConnection;

	// If a low speed connection, check the appropriate box.
	m_wndLowSpeed.Attach (GetDlgItem(IDC_LOW_SPEED_CONNECTION));
	m_wndLowSpeed.SendMessage (BM_SETCHECK, m_bLowSpeed ? BST_CHECKED : BST_UNCHECKED);
	m_wndLowSpeed.EnableWindow (FALSE);
#else
	// This turns on Compression and this should be used as the default.
	// Define WBT_USELOWSPEED and make appropriate changes if you want to turn off compression for some reason
	m_bLowSpeed = TRUE;	
#endif

	return 1;
}


LRESULT CDescriptionPropPage::OnLowSpeedCheck (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled)
{
#ifdef WBT_USELOWSPEED
	// get check state
	int state = Button_GetCheck (m_wndLowSpeed);
	DEBUGMSG (1, (L"CDescriptionPropPage::OnLowSpeedCheck, state %d\n", state));

	m_bLowSpeed = (state == BST_CHECKED);
#endif

	return 1;
}



BOOL CDescriptionPropPage::Validate()
{
/*
	// Quick check to see if the user changed the connection name from what it 
	// was before (case-sensitive).
	m_bConnectionNameChanged = (CSTR_EQUAL != CompareString (LOCALE_SYSTEM_DEFAULT, 0, 
						m_pConnWiz->m_szConnectionName, -1,
						m_szConnectionName, -1));
*/
/*
	
	if (m_bConnectionNameChanged)
	{
*/
		// validate length
		int nLen = m_wndName.GetWindowTextLength();
		for (int i = 0;i < nLen;i++)
		{
			TCHAR tCurrentChar = m_szConnectionName[i];
			
			// Special condition...
			if (i == 0)
			{
				// Make sure that the first character is not a space
				if (tCurrentChar == TCHAR(' '))
				{
					LPCTSTR pszTitle = LoadStringEx (IDS_INVALID_DESCRIPTION_NAME_TITLE);
					LPCTSTR pszMsg = LoadStringEx (IDS_NO_SPACE_IN_DESCRIPTION_MESSAGE);
					
					MessageBox (pszMsg, pszTitle, MB_ICONINFORMATION | MB_OK);

					m_wndName.SendMessage (EM_SETSEL, 0, -1);
					m_wndName.SetFocus();
					return FALSE;			
				}
			}

			if (tCurrentChar == TCHAR('\\') ||
				tCurrentChar == TCHAR('/')  ||
				tCurrentChar == TCHAR(':')  ||
				tCurrentChar == TCHAR('*')  ||
				tCurrentChar == TCHAR('?')  ||
				tCurrentChar == TCHAR('"')  ||
				tCurrentChar == TCHAR('<')  ||
				tCurrentChar == TCHAR('>')  ||
				tCurrentChar == TCHAR('|')  ||
				tCurrentChar == TCHAR(',')  ||
				tCurrentChar == TCHAR('.')  ||
				tCurrentChar == TCHAR('[')  ||
				tCurrentChar == TCHAR(']')  ||
				tCurrentChar == TCHAR('(')  ||
				tCurrentChar == TCHAR(')')  ||
				tCurrentChar == TCHAR(')'))
			{
				LPCTSTR pszTitle = LoadStringEx (IDS_INVALID_DESCRIPTION_NAME_TITLE);
				LPCTSTR pszMsg = LoadStringEx (IDS_INVALID_DESCRIPTION_NAME_MESSAGE);
				
				TCHAR szMsg [MAX_DISPLAY_STRING] = {0};
				_tcscpy (szMsg, pszMsg);
				_tcscat (szMsg, L"\n\n");
				_tcscat (szMsg, L"\t\t");
				_tcscat (szMsg, L"\\");
				_tcscat (szMsg, L" / : * ?");
				_tcscat (szMsg, L" \"");
				_tcscat (szMsg, L" < > | , . [ ] ( )");
				
				MessageBox (szMsg, pszTitle, MB_ICONINFORMATION | MB_OK);
				m_wndName.SendMessage (EM_SETSEL, 0, -1);
				m_wndName.SetFocus();
				return FALSE;			
			}
		}	
		
		// Check the registry to be sure that another connnection with the
		// same name doesn't already exist.
		HKEY hKeyTest = NULL;
		HKEY hKeyTestConnection = NULL;
		
		if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_CURRENT_USER, REG_TSC_PATH, 0, 
										   KEY_READ, &hKeyTest))
		{
			if (ERROR_SUCCESS == RegOpenKeyEx(hKeyTest, m_szConnectionName, 0,
									KEY_READ, &hKeyTestConnection))
			{
				LPCTSTR pszTitle = LoadStringEx (IDS_CONNECTION_EXISTS_TITLE);
				LPCTSTR pszMsg = LoadStringEx (IDS_CONNECTION_EXISTS_MESSAGE);
				
				MessageBox (pszMsg, pszTitle, MB_ICONINFORMATION | MB_OK);
				m_wndName.SendMessage (EM_SETSEL, 0, -1);
				m_wndName.SetFocus();

				// Close the key
				RegCloseKey(hKeyTest);
				RegCloseKey(hKeyTestConnection);
				
				return FALSE;			
			}	

			// Close the key
			RegCloseKey(hKeyTest);
		}
/*
	}
*/
	// Net Connections page...
	// Make sure the user has provided a server name.
	nLen = m_wndServer.GetWindowTextLength();

	TCHAR *pszPort = _tcschr(m_szServerName, TCHAR(':'));
	if (pszPort)
	{
		*pszPort++ = TCHAR('\0');
		m_dwRdpPort = _ttoi(pszPort);
		if (m_dwRdpPort == 0)
			m_dwRdpPort = DEFAULT_RDP_PORT;
	}
		
        // Make sure that all the characters in the server name are valid.
	int i;
		for (i = 0;i < nLen;i++)
		{
			TCHAR tCurrentChar = m_szServerName[i]; 
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
				tCurrentChar == TCHAR(';')  ||
				tCurrentChar == TCHAR('~')  ||
				tCurrentChar == TCHAR('$')  ||
				tCurrentChar == TCHAR('%')  ||
				tCurrentChar == TCHAR('^')  ||
				tCurrentChar == TCHAR('{')  ||
				tCurrentChar == TCHAR('}')  ||
				tCurrentChar == TCHAR('@')  ||
				tCurrentChar == TCHAR('/')  ||
				tCurrentChar == TCHAR('\'')  ||
				tCurrentChar == TCHAR('#')  ||
				tCurrentChar == TCHAR(','))
			{
				LPCTSTR pszTitle = LoadStringEx (IDS_INVALID_SERVER_NAME_TITLE);
				LPCTSTR pszMsg = LoadStringEx (IDS_INVALID_SERVER_NAME_MESSAGE);
				LPCTSTR pszSpace = LoadStringEx (IDS_SPACE);
				
				TCHAR szMsg [MAX_DISPLAY_STRING] = {0};
				_tcscpy (szMsg, pszMsg);
				_tcscat (szMsg, L"\n\n");
				_tcscat (szMsg, L"\t\t");
				_tcscat (szMsg, L"\\");
				_tcscat (szMsg, L" : ");
				_tcscat (szMsg, L"\"");
				_tcscat (szMsg, L"; / ~ ' @ # $ % ^ { } < > * + = | ? , ");
				if (pszSpace)
					_tcscat (szMsg, pszSpace);
				
				MessageBox (szMsg, pszTitle, MB_ICONINFORMATION | MB_OK);
				m_wndServer.SendMessage (EM_SETSEL, 0, -1);
				m_wndServer.SetFocus();
				return FALSE;			
			}
		}
//	}
	return TRUE;
}


LRESULT CDescriptionPropPage::OnSetActive (int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	DEBUGMSG (1, (L"CDescriptionPropPage::OnSetActive for page [%s]\n", GetId()));
	if (m_bNameValid && m_bServerValid)
		GetOwner().SetWizFlags (PSWIZB_NEXT);
	else
		GetOwner().SetWizFlags (0);
	return 0;
}


BOOL CDescriptionPropPage::ReadRegistrySettings()
{
	return TRUE;
}


BOOL CDescriptionPropPage::WriteRegistrySettings()
{
	StringCchCopy (m_pConnWiz->m_szServerName, MAX_SERVER_NAME, m_szServerName);
	StringCchCopy (m_pConnWiz->m_szConnectionName, MAX_CONNECTION_DESCRIPTION, m_szConnectionName);
	m_pConnWiz->m_bLowSpeedConnection = m_bLowSpeed;
	m_pConnWiz->m_dwRdpPort = m_dwRdpPort;

	return TRUE;
}


LRESULT CDescriptionPropPage::OnNameChange (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ProcessNameOrServerChange();
	return 0;
}

LRESULT CDescriptionPropPage::OnServerChange (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ProcessNameOrServerChange();
	return 0;
}

void CDescriptionPropPage::ProcessNameOrServerChange()
{
	m_bNameValid = (m_wndName.GetWindowTextLength() > 0);
	m_bServerValid = (m_wndServer.GetWindowTextLength() > 0);
	
	BOOL bDummy;
	OnSetActive (0, 0, bDummy);
}

BOOL CDescriptionPropPage::StoreData()
{
	m_wndName.GetWindowText (m_szConnectionName, MAX_CONNECTION_DESCRIPTION);
	m_wndServer.GetWindowText (m_szServerName, MAX_SERVER_NAME);
	
	// because the Finish page needs this
	StringCchCopy (m_pConnWiz->m_szConnectionName, MAX_CONNECTION_DESCRIPTION, m_szConnectionName);

	return TRUE;
}
