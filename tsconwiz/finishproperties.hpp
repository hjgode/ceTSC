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
#ifndef FINISH_PROPERTIES_HPP
#define FINISH_PROPERTIES_HPP

#include "ConWizPropClasses.hpp"

class CFinishPropPage : public CConWizPropPage
{
public:
	CFinishPropPage (CConWizPropSheet *pOwner = NULL);
	~CFinishPropPage();

	virtual BOOL WriteRegistrySettings();
	virtual BOOL ReadRegistrySettings();
	virtual BOOL Validate();
	virtual BOOL StoreData();
	virtual LRESULT OnSetActive (int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	
	LPCTSTR GetId()
	{ return TEXT("Finish"); }

	BEGIN_MSG_MAP(CFinishPropPage)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		
		CHAIN_MSG_MAP (CConWizPropPage)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	CWindow m_wndName;
};


#endif // LOGON_PROPERTIES_HPP
