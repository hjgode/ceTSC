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
#ifndef APPLICATION_PROPERTIES_HPP
#define APPLICATION_PROPERTIES_HPP

#include "ConWizPropClasses.hpp"
#include "tsconwiz.hpp"

class CApplicationPropPage : public CConWizPropPage
{
public:
	CApplicationPropPage (CConWizPropSheet *pOwner = NULL);
	~CApplicationPropPage();

	virtual BOOL WriteRegistrySettings();
	virtual BOOL ReadRegistrySettings();
	virtual BOOL Validate();
	virtual BOOL StoreData();
	virtual LRESULT OnSetActive (int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	
	LPCTSTR GetId()
	{ return TEXT("Application"); }

	BEGIN_MSG_MAP(CApplicationPropPage)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)

		COMMAND_HANDLER (IDC_DESKTOP_BUTTON, BN_CLICKED, OnDesktop)
		COMMAND_HANDLER (IDC_APPLICATION_FILE_NAME_BUTTON, BN_CLICKED, OnDesktop)
		COMMAND_HANDLER (IDC_APPLICATION_FILE_NAME_EDIT, EN_UPDATE, OnFilenameChange)
		
		CHAIN_MSG_MAP (CConWizPropPage)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnFilenameChange (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT	OnDesktop (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);

private:
	void SetDesktopRadio();
	void ProcessFilenameChange();
	
	CWindow m_wndDesktopRadio;
	CWindow m_wndFileName;
	CWindow m_wndWorkingDir;
	CWindow m_wndFileNameRadio;
	CWindow m_lblWorkingDir;

	BOOL m_bDesktop;
	BOOL m_bFilenameValid;

	TCHAR m_szAlternateShell [MAX_PATH];
	TCHAR m_szShellWorkingDirectory [MAX_PATH];
};


#endif // APPLICATION_PROPERTIES_HPP
