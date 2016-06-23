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
#include "PropClasses.hpp"

#pragma warning( disable : 4715 )  // Disable warning message 4715

BOOL CPropSheet::m_bCallbackThisSet = FALSE;
CPropSheet *CPropSheet::m_pThis = NULL;


CPropPage::CPropPage (CPropSheet *pOwner) : m_pOwner (pOwner), m_bActivated(FALSE)
{
	if (!pOwner)
	{
		RaiseException (NO_OWNER, EXCEPTION_NONCONTINUABLE, 0, 0);
	}
	DEBUGMSG (1, (L"CPropPage::CPropPage\n"));
}

CPropPage::~CPropPage()
{
}

void CPropPage::Init (int nResId)
{
	DEBUGMSG (1, (L"CPropPage::Init, resId [%d], making [%s]\n", nResId, GetId()));
	Init (MAKEINTRESOURCE(nResId));
}

void CPropPage::Init (LPCTSTR szResId)
{
	memset (&m_page, 0, sizeof m_page);
	m_page.dwSize = sizeof PROPSHEETPAGE;
	m_page.dwFlags = PSP_DEFAULT | PSP_USECALLBACK;
    m_page.hInstance = _Module.GetModuleInstance();
    m_page.pszTemplate = szResId;
    m_page.pszIcon = NULL;
    m_page.pszTitle = NULL;
    m_page.pfnDlgProc = (DLGPROC) StartDialogProc; // use Dialog boxes base window proc and hope this does the trick
    
	// pass in 'this' pointer to callback
	m_page.lParam = reinterpret_cast<LPARAM> (this);
	
	// we need a cllback so we can hook into the ATL window map
	m_page.pfnCallback = PropPageCallback;
}


UINT CALLBACK CPropPage::PropPageCallback (HWND hwnd, UINT uMsg, LPPROPSHEETPAGE ppsp)
{
	DEBUGMSG (1, (L"CPropPage::PropSheetPageProc\n"));

	// check
	if (!ppsp)
	{
		DEBUGMSG (1, (L"Invalid ppsp in callback !!\n"));
		return 1;
	}
	
	// get property page pointer
	CPropPage *pThis = reinterpret_cast<CPropPage *> (ppsp->lParam);

	// check
	if (!pThis)
	{
		DEBUGMSG (1, (L"Couldn't get pThis in callback !!\n"));
		return 1;
	}

	switch (uMsg)
	{
		case PSPCB_CREATE:
		{
			DEBUGMSG (1, (L"Psp Callback: PSPCB_CREATE-> creating window %x\n", hwnd));

			// add Window creation data to ATL's window list
			ATLASSERT(pThis->m_hWnd == NULL);
			_Module.AddCreateWndData (&(pThis->m_thunk.cd), (CDialogImplBaseT<CWindow>*)pThis);
		}
		return TRUE;

		case PSPCB_RELEASE:
		{
			DEBUGMSG (1, (L"Psp Callback: PSPCB_RELEASE-> destroying window %x\n", hwnd));
			// at this point let us try and call fn's
			if (pThis->m_pOwner)
			{
				if (pThis->m_pOwner->GetCloseReason() == CPropSheet::CLOSE_OK)
				{
					DEBUGMSG (1, (L"CPropSheet::PropPageCallback UnloadPage for page [%s]\n", pThis->GetId()));
					pThis->UnloadPage();
				}
				else if (pThis->m_pOwner->GetCloseReason() == CPropSheet::CLOSE_CANCEL)
				{
					DEBUGMSG (1, (L"CPropSheet::PropPageCallback Page [%s] was cancelled\n", pThis->GetId()));
				}
			}
		}
		break;
	}
	return 0;
}

LRESULT CPropPage::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DEBUGMSG (1, (L"CPropPage::OnInitDialog - page [%s]\n", GetId()));
	m_bActivated = TRUE;
	
	// call the page's load function
	if (!LoadPage())
	{
		DEBUGMSG (1, (L"Could not load property page [%s]\n", GetId()));
	}
	return 0;
}

LPCTSTR CPropPage::GetTitle()
{
	return m_page.pszTitle;
}



int CPropPage::MessageBox (LPCTSTR lpszText, LPCTSTR lpszCaption, UINT nType)
{
	return ::MessageBox (GetOwner().GetHwnd(), lpszText, lpszCaption, nType);
}





CPropSheet::CPropSheet (HWND hwndParent, LPCTSTR szTitle, BOOL bApply, BOOL bWizard) :
	m_hwndParent (hwndParent), m_bApply (bApply), m_bWizard(bWizard), m_closeReason(CLOSE_OK), m_hWnd(NULL)
{
	// clear struct
	memset (&m_psh, 0, sizeof PROPSHEETHEADER);
	//memset (&m_pages, 0, sizeof m_pages);
	if (szTitle)
		_tcscpy (m_szTitle, szTitle);
	else
		m_szTitle[0] = TCHAR('\0');
	DEBUGMSG (1, (L"Constructed property sheet, title [%s]\n", m_szTitle));
}

CPropSheet::~CPropSheet()
{
	DEBUGMSG (1, (L"CPropSheet::~CPropSheet\n"));
}

BOOL CPropSheet::AddPropPage (CPropPage *pPage)
{
	DEBUGMSG (1, (L"Add property page, page [%x]\n", pPage));
	if (pPage && (m_pages.GetSize() < sc_nMaxPropPages))
	{
		m_pages.Add (pPage);
		return TRUE;
	}
	return FALSE;
}

CPropPage *CPropSheet::GetPage (int n) const
{
	if ((n >= 0) && (n < m_pages.GetSize()))
		return m_pages[n];
	return NULL;
}


int CALLBACK CPropSheet::PropSheetCallback (HWND hwndDlg, UINT uMsg, LPARAM lParam)
{
	if (m_pThis != NULL)
	{
		if (uMsg == PSCB_INITIALIZED)
			m_pThis->m_hWnd = hwndDlg;
		m_pThis->m_bCallbackThisSet = FALSE;
	}
	return 0;
}

void CPropSheet::GotoPage (int nPage)
{
	if ((nPage >= 0) && (nPage < m_pages.GetSize()))
	{
		PropSheet_SetCurSel (m_hWnd, NULL, nPage);
	}
}


int CPropSheet::Run (int nStartPage)
{
	DEBUGMSG (1, (L"CPropSheet::Run, npages = %d\n", m_pages.GetSize()));
	
	// check we have pages
	if (m_pages.GetSize() > 0)
	{
		// fill in values
		m_psh.dwSize = sizeof PROPSHEETHEADER;
		m_psh.dwFlags = PSH_PROPSHEETPAGE | PSH_USECALLBACK;
		if (m_bWizard)
		{
			m_dwWizFlags = PSWIZB_BACK | PSWIZB_NEXT;
			m_psh.dwFlags |= PSH_WIZARD;
		}
		if (!m_bApply)
			m_psh.dwFlags |= PSH_NOAPPLYNOW;
		m_psh.hwndParent = m_hwndParent;
		m_psh.pszCaption = m_szTitle;
		m_psh.nPages = m_pages.GetSize();

		// verify index of start page
		if ((nStartPage < 0) || (nStartPage > m_pages.GetSize()))
			nStartPage = 0;
		m_psh.nStartPage = nStartPage;

		// add our callback
		m_psh.pfnCallback = PropSheetCallback;
		if (m_bCallbackThisSet)
		{
			DEBUGMSG (1, (L"Error - m_bCallbackThisSet set already inside Run\n"));
			return -1;
		}
		m_bCallbackThisSet = TRUE;
		m_pThis = this;

		// actual PROPSHEETPAGE array
		PROPSHEETPAGE *pPsp = new PROPSHEETPAGE [m_pages.GetSize()];
		if (!pPsp)
		{
			DEBUGMSG (1, (L"CPropSheet::Run -- Cannot allocate memory\n"));
			return -1;
		}
		memset (pPsp, 0, sizeof(PROPSHEETPAGE) * m_pages.GetSize());

		// copy over values
		for (int i = 0;i < m_pages.GetSize();i++)
		{
			CPropPage *pPage = GetPage(i);
			if (pPage)
			{
				pPsp[i] = *pPage;
				DEBUGMSG (1, (L"Added page [%s] to sheet array\n", pPage->GetTitle()));
			}
			else
				DEBUGMSG (1, (L"!! Failed to get page %d !!\n", i));
		}

		m_psh.ppsp = (LPCPROPSHEETPAGE) pPsp;
		
		// run it
		int ret = PropertySheet (&m_psh);
		DEBUGMSG (1, (L"PropertySheet rets %d\n", ret));

		// give the app a chance to store final settings
		if (m_closeReason != CLOSE_CANCEL)
			WriteSettings();

		// delete alloc'd array
		delete [] pPsp;

		return ret;
	}
	return 0;
}

void CPropSheet::WriteSettings()
{
	// Final chance to store settings
	//for (int i = 0;i < m_pWbtWiz
}

CPropSheet& CPropPage::GetOwner()
{
	return *m_pOwner;
}


void CPropSheet::SetCloseReason (CloseReason closeReason)
{
	m_closeReason = closeReason;
}

LRESULT CPropPage::OnKillActive (int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	DEBUGMSG (1, (L"CPropPage::OnKillActive for page [%s]\n", GetId()));
	GetOwner().SetCloseReason (CPropSheet::CLOSE_OK);

	// store the page's data
	if (!StoreData())
	{
		DEBUGMSG (1, (L"Couldn't store data for page [%s]\n", GetId()));
		return 0;
	}
	
	// validate the page
	BOOL bOkToGoAway = Validate();

	if (!bOkToGoAway)
	{
		DEBUGMSG (1, (L"CPropPage::OnKillActive Page cannot lose focus\n"));
		SetWindowLong (DWL_MSGRESULT, TRUE);
		return TRUE;
	}
	return 0;
}


LRESULT CPropPage::OnSetActive (int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	DEBUGMSG (1, (L"CPropPage::OnSetActive for page [%s]\n", GetId()));
	return 0;
}



LRESULT CPropPage::OnReset (int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	DEBUGMSG (1, (L"CPropPage::OnReset for page [%s]\n", GetId()));
	GetOwner().SetCloseReason (CPropSheet::CLOSE_CANCEL);
	return 0;
}


LRESULT CPropPage::OnWizBack (int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	DEBUGMSG (1, (L"CPropPage::OnWizBack for page [%s]\n", GetId()));
	//UnloadPage();
	return 0;
}


LRESULT CPropPage::OnWizNext (int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	DEBUGMSG (1, (L"CPropPage::OnWizNext for page [%s]\n", GetId()));
	//UnloadPage();
	return 0;
}



LRESULT CPropPage::OnWizFinish (int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	DEBUGMSG (1, (L"CPropPage::OnWizFinish for page [%s]\n", GetId()));
	//UnloadPage();
	return 0;
}


void CPropSheet::SetWizFlags (DWORD dwFlags)
{
	if (m_bWizard)
	{
		if (IsWindow(m_hWnd))
		{
			m_dwWizFlags = dwFlags;
			PropSheet_SetWizButtons (m_hWnd, m_dwWizFlags);
		}
	}
}
