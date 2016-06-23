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
#ifndef LOGON_PROPERTIES_HPP
#define LOGON_PROPERTIES_HPP

#include "ConWizPropClasses.hpp"
#include "tsconwiz.hpp"

class CLogonPropPage : public CConWizPropPage
{
public:
	CLogonPropPage (CConWizPropSheet *pOwner = NULL);
	~CLogonPropPage();

	virtual BOOL WriteRegistrySettings();
	virtual BOOL ReadRegistrySettings();
	virtual BOOL Validate();
	virtual BOOL StoreData();
	virtual LRESULT OnSetActive (int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	
	LPCTSTR GetId()
	{ return TEXT("Logon"); }

	BEGIN_MSG_MAP(CLogonPropPage)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER (IDC_AUTO_LOGON_CHECK, BN_CLICKED, OnAutomaticLogonCheck)
		COMMAND_HANDLER (IDC_USERNAME, EN_UPDATE, OnUserChange)
		COMMAND_HANDLER (IDC_DOMAIN, EN_UPDATE, OnDomainChange)
		
		CHAIN_MSG_MAP (CConWizPropPage)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnUserChange (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDomainChange (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

private:
	void SetAutoLogon();
	LRESULT	OnAutomaticLogonCheck (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);
	void ProcessUserOrDomainChange();

	CWindow m_wndAutoLogon;
	CWindow m_wndUsername;
	CWindow m_wndPwd;
	CWindow m_wndDomain;

	CWindow m_lblUsername;
	CWindow m_lblPwd;
	CWindow m_lblDomain;

	BOOL m_bAutoLogon;
	BOOL m_bUserValid;
	BOOL m_bDomainValid;

	TCHAR m_szUserName [MAX_USERNAME_LENGTH + 1];
	TCHAR m_szPassword [MAX_PASSWORD_LENGTH + 1];
	TCHAR m_szDomain [MAX_DOMAIN_LENGTH + 1];
};


#endif // CONNECTION_PROPERTIES_HPP
