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
#ifndef PROP_CLASSES_HPP
#define PROP_CLASSES_HPP

#include <atlbase.h>
#include <atlwin.h>
#include <prsht.h>

#define NO_OWNER 0xFFFFFFFF

/////////////////////////////////////////////////////////////////
/////////////////////// Property Pages //////////////////////////
/////////////////////////////////////////////////////////////////

class CPropSheet;

class CPropPage : public CDialogImpl <CPropPage>
{
public:
	// exception class
	class XNoOwner {};

public:
	CPropPage (CPropSheet *pOwner = NULL);
	~CPropPage();

	void Init (LPCTSTR szResId);
	void Init (int nResId);

	virtual BOOL LoadPage() = 0;
	virtual BOOL UnloadPage() = 0;
	virtual BOOL Validate() = 0;
	virtual BOOL StoreData() = 0;

	CPropSheet& GetOwner();
	BOOL SetupAtlMapping();

	BEGIN_MSG_MAP(CPropPage)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		NOTIFY_HANDLER (0, PSN_KILLACTIVE, OnKillActive)
		NOTIFY_HANDLER (0, PSN_SETACTIVE, OnSetActive)
		NOTIFY_HANDLER (0, PSN_RESET, OnReset)
		NOTIFY_HANDLER (0, PSN_WIZBACK, OnWizBack)
		NOTIFY_HANDLER (0, PSN_WIZNEXT, OnWizNext)
		NOTIFY_HANDLER (0, PSN_WIZFINISH, OnWizFinish)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnKillActive (int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	virtual LRESULT OnSetActive (int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	virtual LRESULT OnReset (int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	virtual LRESULT OnWizBack (int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	virtual LRESULT OnWizNext (int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	virtual LRESULT OnWizFinish (int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	operator PROPSHEETPAGE()
	{
		return m_page;
	}

	LPCTSTR GetTitle();
	virtual LPCTSTR GetId() = 0;

	BOOL Activated()
	{
		return m_bActivated;
	}

	HINSTANCE GetInstance()
	{
		return m_page.hInstance;
	}

	// override std MessageBox	
	int MessageBox (LPCTSTR lpszText, LPCTSTR lpszCaption = NULL, UINT nType = MB_OK);

private:
	static UINT CALLBACK PropPageCallback (HWND hwnd, UINT uMsg, LPPROPSHEETPAGE ppsp);

    PROPSHEETPAGE m_page;
	CPropSheet *m_pOwner;
	BOOL m_bActivated;
};


/////////////////////////////////////////////////////////////////
/////////////////////// Property Sheet //////////////////////////
/////////////////////////////////////////////////////////////////

const int sc_nMaxPropPages = 30;

class CPropSheet
{
public:
	CPropSheet (HWND hwndParent, LPCTSTR szTitle, BOOL bApply = TRUE, BOOL bWizard = FALSE);
	~CPropSheet();

	enum CloseReason {CLOSE_OK, CLOSE_CANCEL};
	
	virtual void WriteSettings();

	BOOL AddPropPage (CPropPage *pPage);
	CPropPage *GetPage (int n) const;
	void GotoPage (int nPage);	
	int Run (int nStartPage = 0);	
	void SetCloseReason (CloseReason closeReason);
	CloseReason GetCloseReason() const{
		return m_closeReason;
	}

	static int CALLBACK PropSheetCallback (HWND hwndDlg, UINT uMsg, LPARAM lParam);

	void SetWizFlags (DWORD dwFlags);
	HWND GetHwnd()
	{
		return m_hWnd;
	}

	HINSTANCE GetInstance()
	{
		return ((m_pages.GetSize() > 0) ? m_pages[0]->GetInstance() : NULL);
	}

private:
	// array of property pages
	CSimpleArray <CPropPage *> m_pages;
	int m_nPages;
    PROPSHEETHEADER m_psh;
	BOOL m_bApply;
	BOOL m_bWizard;
	HWND m_hwndParent;
	TCHAR m_szTitle [255];
	CloseReason m_closeReason;
	HWND m_hWnd;
	DWORD m_dwWizFlags;
	static BOOL m_bCallbackThisSet;
	static CPropSheet *m_pThis;
};

#endif //PROP_CLASSES_HPP

