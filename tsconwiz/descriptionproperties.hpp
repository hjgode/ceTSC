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
#ifndef CONNECTION_PROPERTIES_HPP
#define CONNECTION_PROPERTIES_HPP

#include "ConWizPropClasses.hpp"
#include "tsconwiz.hpp"

class CDescriptionPropPage : public CConWizPropPage
{
public:
	CDescriptionPropPage (CConWizPropSheet *pOwner = NULL);
	~CDescriptionPropPage();

	virtual BOOL WriteRegistrySettings();
	virtual BOOL ReadRegistrySettings();
	virtual BOOL Validate();
	virtual BOOL StoreData();
	virtual LRESULT OnSetActive (int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	
	LPCTSTR GetId()
	{ return TEXT("Connection"); }

	BEGIN_MSG_MAP(CConnectionPropPage)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER (IDC_LOW_SPEED_CONNECTION, BN_CLICKED, OnLowSpeedCheck)
		COMMAND_HANDLER (IDC_CONNECTION_DESCRIPTION, EN_UPDATE, OnNameChange)
		COMMAND_HANDLER (IDC_SERVER, EN_UPDATE, OnServerChange)
		
		CHAIN_MSG_MAP (CConWizPropPage)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnServerChange (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnNameChange (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

private:
	void ProcessNameOrServerChange();
	void SetAutoLogon();
	LRESULT	OnLowSpeedCheck (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);

	CWindow m_wndName;
	CWindow m_wndServer;
	CWindow m_wndLowSpeed;

	BOOL m_bLowSpeed;
	BOOL m_bConnectionNameChanged;
	BOOL m_bNameValid;
	BOOL m_bServerValid;
	TCHAR m_szConnectionName [MAX_CONNECTION_DESCRIPTION];
	TCHAR m_szServerName [MAX_SERVER_NAME];
	DWORD m_dwRdpPort;
};

const int c_nMaxlenServerName = 32;
const int c_nMaxlenConnectionDescription = 32;

#endif // CONNECTION_PROPERTIES_HPP
