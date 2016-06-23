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
#include "FinishProperties.hpp"
#include "TsConWiz.hpp"
#include "RegString.hpp"


CFinishPropPage::CFinishPropPage (CConWizPropSheet *pOwner) : 
						CConWizPropPage(pOwner)
{
	DEBUGMSG (1, (L"CFinishPropPage::CFinishPropPage\n"));
	Init (IDD_FINISH_DIALOG);
}

CFinishPropPage::~CFinishPropPage()
{
}

LRESULT CFinishPropPage::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DEBUGMSG (1, (L"CFinishPropPage::OnInitDialog, class is [%s]\n", GetId()));
	CConWizPropPage::OnInitDialog (uMsg, wParam, lParam, bHandled);

	m_pConnWiz = GetOwner().GetConnWiz();
	if (!m_pConnWiz)
	{
		DEBUGMSG (1, (L"!! Error -  couldn't get owner !!\n"));
		EndDialog (0);
		return 0;
	}
	
	m_wndName.Attach (GetDlgItem(IDC_FINISH_CONNECTION_NAME));
	m_wndName.SetWindowText (m_pConnWiz->m_szConnectionName);
	
	return 1;
}


BOOL CFinishPropPage::Validate()
{
	return TRUE;
}



LRESULT CFinishPropPage::OnSetActive (int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	DEBUGMSG (1, (L"CFinishPropPage::OnSetActive for page [%s]\n", GetId()));
	GetOwner().SetWizFlags (PSWIZB_BACK | PSWIZB_FINISH);
	return 0;
}


BOOL CFinishPropPage::ReadRegistrySettings()
{
	return TRUE;
}


BOOL CFinishPropPage::WriteRegistrySettings()
{
	return TRUE;
}


BOOL CFinishPropPage::StoreData()
{
	return TRUE;
}
