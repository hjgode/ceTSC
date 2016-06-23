//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
#ifndef _DLGBASE_H_
#define _DLGBASE_H_

class CDlgBase
{
public:
	CDlgBase(HWND hwndOwner, HINSTANCE hInst, DWORD dwResId);
	virtual ~CDlgBase();

	static int CALLBACK StaticDialogProc(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam);

public:
	virtual int DoModal()
	{
		return DialogBoxParam(_hInst, MAKEINTRESOURCE(_dwResId), _hwndOwner, StaticDialogProc, (LONG)this);
	}

	virtual HWND DoModeless()
	{
		HWND hWnd = CreateDialogParam(_hInst, MAKEINTRESOURCE(_dwResId), _hwndOwner, StaticDialogProc, (LONG)this);
		ShowWindow(hWnd, SW_RESTORE);
		return  hWnd;
	}

	virtual void Destroy()
	{
		DestroyWindow(_hwndDlg);
	}

	HWND GetHwnd()	{	return _hwndDlg;	}
	HWND GetOwner()	{	return _hwndOwner;	}

	void SetOwner(HWND hwndOwner) {		_hwndOwner = hwndOwner;	}

protected:
	virtual int DialogProc(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam);
	void CenterWindow();

protected:
	HWND		_hwndOwner;
	HWND		_hwndDlg;
	HINSTANCE	_hInst;
	DWORD		_dwResId;
};

#endif
