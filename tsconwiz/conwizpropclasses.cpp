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
#include "ConWizPropClasses.hpp"
#include "TsConWiz.hpp"

CConWizPropPage::CConWizPropPage (CConWizPropSheet *pOwner) : CPropPage(pOwner), m_pConnWiz(NULL)
{
}

CConWizPropPage::~CConWizPropPage()
{
}


BOOL CConWizPropPage::ReadRegistrySettings()
{
	// not implemented
	return FALSE;
}

BOOL CConWizPropPage::WriteRegistrySettings()
{
	// not implemented
	return FALSE;
}

BOOL CConWizPropPage::LoadPage()
{
	return ReadRegistrySettings();
}

BOOL CConWizPropPage::UnloadPage()
{
	// save settings only if our page became active at some point
	if (Activated())
		return WriteRegistrySettings();
	else
	{
		DEBUGMSG (1, (L"UnloadPage(): Page [%s] never became active\n", GetId()));
		return FALSE;
	}
}



CConWizPropSheet::CConWizPropSheet (HWND hwndParent, TsConnectionWizard *pConnWiz, LPCTSTR szTitle) :
		CPropSheet (hwndParent, szTitle, FALSE, TRUE), m_pConnWiz (pConnWiz)
{
}

CConWizPropSheet::~CConWizPropSheet()
{
}

CConWizPropSheet& CConWizPropPage::GetOwner()
{
	//return dynamic_cast<CConWizPropSheet&> (CPropPage::GetOwner());
	return (CConWizPropSheet&) CPropPage::GetOwner();
}


void CConWizPropSheet::WriteSettings()
{
	if (m_pConnWiz)
		m_pConnWiz->WriteSettings();
}

