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
#ifndef CONWIZ_PROP_CLASSES_HPP
#define CONWIZ_PROP_CLASSES_HPP

#include "PropClasses.hpp"

class CConWizPropSheet;
class TsConnectionWizard;

class CConWizPropPage : public CPropPage
{
public:
	CConWizPropPage (CConWizPropSheet *pOwner = NULL);
	~CConWizPropPage();

	CConWizPropSheet& GetOwner();
	
	virtual BOOL LoadPage();
	virtual BOOL UnloadPage();
	virtual BOOL WriteRegistrySettings() = 0;
	virtual BOOL ReadRegistrySettings() = 0;
	virtual BOOL Validate() = 0;
	virtual BOOL StoreData() = 0;

	BEGIN_MSG_MAP(CConWizPropPage)
		CHAIN_MSG_MAP (CPropPage)
	END_MSG_MAP()
protected:
	TsConnectionWizard *m_pConnWiz;
};


class CConWizPropSheet : public CPropSheet
{
public:
	CConWizPropSheet (HWND hwndParent, TsConnectionWizard *pConnWiz, LPCTSTR szTitle);
	~CConWizPropSheet();

	void SetWizFlags (DWORD dwFlags)
	{
		CPropSheet::SetWizFlags (dwFlags);		
	}

	TsConnectionWizard *GetConnWiz()
	{
		return m_pConnWiz;
	}

	virtual void WriteSettings();
	
private:
	TsConnectionWizard *m_pConnWiz;
};


#endif //CONWIZ_PROP_CLASSES_HPP

