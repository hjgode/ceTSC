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
#ifndef _ARCDLG_H_
#define _ARCDLG_H_

#define STATUS_STRING_LEN	256

#include "dlgbase.h"
#include "baseui.h"

class CArcDlg : public CDlgBase
{
public:
	CArcDlg(HWND hwndParent, HINSTANCE hInst);

	~CArcDlg () {};

	virtual int DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static void CALLBACK	StaticProgbandTimerCallback(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);
	void					ProgbandTimerCallback(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);

    void OnNotifyAutoReconnecting(DWORD dwAttempt, BOOL* pfContinue);

	void ShowTopMost();
	void StopAutoReconnect();
	void SetMaxAttempts(DWORD dwMaxAttempts)	{ _dwMaxAttempts = dwMaxAttempts;	}

private:
	void UpdateConnectionAttempts();
    void OnEraseBkgnd(HWND hwnd, HDC hdc);

protected:
	BOOL	_fContinue;
	DWORD	_dwAttempts;
	DWORD	_dwMaxAttempts;

    HBRUSH				_hbrTopBand;
    HBRUSH				_hbrMidBand;
	HBITMAP				_hbmpProgband;
	LONG				_lProgress;	//varies from 0 to width of dialog
	RECT				_rcProgband;
	RECT				_rcDlg;
	RECT				_rcTop, _rcMid, _rcBtm;
};

#endif