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
#include <windows.h>
#include "arcdlg.h"
#include "mstsc.h"
#include "baseui.h"

#define RGB_TOPBAND RGB(0, 52, 156)
#define RGB_MIDBAND RGB(49,101,206)

#define PROGBAND_DELAY	20

CArcDlg::CArcDlg(HWND hwndParent, HINSTANCE hInst) : CDlgBase(hwndParent, hInst, IDD_ARCDLG)
{	
	_fContinue= TRUE;	
	_lProgress = 0;
}


int CArcDlg::DialogProc(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam)
{
	int nRet = FALSE;

	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		_hbrTopBand = CreateSolidBrush(RGB_TOPBAND);
		_hbrMidBand = CreateSolidBrush(RGB_MIDBAND);
		
		GetClientRect(hwndDlg, &_rcDlg);
		SetRect(&_rcTop, _rcDlg.left, _rcDlg.top, _rcDlg.right, _rcDlg.bottom/4);
		SetRect(&_rcMid, _rcDlg.left, _rcDlg.bottom/4, _rcDlg.right, 3*_rcDlg.bottom/4);
		SetRect(&_rcBtm, _rcDlg.left, 3*_rcDlg.bottom/4, _rcDlg.right, _rcDlg.bottom);
		
		BITMAP bmp;
		_hbmpProgband = LoadBitmap(_hInst, MAKEINTRESOURCE(IDB_BAND8));
		if (!_hbmpProgband)
			return FALSE;
		
		if (!GetObject(_hbmpProgband, sizeof(BITMAP), &bmp))
			return FALSE;

		SetRect(&_rcProgband, _rcTop.left, _rcTop.bottom - bmp.bmHeight, _rcTop.right, _rcTop.bottom);

		if (!SetTimer(hwndDlg, (UINT)this, PROGBAND_DELAY, StaticProgbandTimerCallback))
			return FALSE;

		
		SendMessage(hwndDlg, DM_SETDEFID, IDCANCEL, 0);
		SetFocus(hwndDlg);

		nRet = TRUE;
		break;
	}

	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)
		{
			EnableWindow(GetDlgItem(_hwndDlg, IDCANCEL), FALSE);
			_fContinue = FALSE;
			return TRUE;
		}
		break;

	case WM_ERASEBKGND:
		OnEraseBkgnd(hwndDlg, (HDC)wParam);
		return TRUE;

	case WM_CTLCOLORDLG:
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (int )GetStockObject(NULL_BRUSH);

	case WM_CTLCOLORSTATIC:
	{
		SetTextColor((HDC)wParam, RGB(255,255,255));
		SetBkMode((HDC)wParam, TRANSPARENT);
		LONG lId = GetWindowLong((HWND)lParam, GWL_ID);
		return (int ) ((lId == IDC_STC_TITLETEXT) ? _hbrTopBand : _hbrMidBand);
	}

	default:
		nRet = FALSE;
		break;
	};

	return (CDlgBase::DialogProc(hwndDlg, uMsg, wParam, lParam) || nRet);
}

void CArcDlg::OnNotifyAutoReconnecting(DWORD dwAttempt, BOOL* pfContinue)
{
	ASSERT(pfContinue);

    if (_fContinue) 
	{
        _dwAttempts = dwAttempt;
        UpdateConnectionAttempts();
    }
    else 
	{
        DEBUGMSG(ZONE_MAIN, (L"Stopping Autoreconnect. _fContinue = FALSE!"));
    }

    *pfContinue = (_fContinue) && (dwAttempt < _dwMaxAttempts);
}

void CArcDlg::ShowTopMost()
{
	ASSERT(_hwndDlg);
    
	ShowWindow(_hwndDlg, SW_SHOWNORMAL);
    
	SetWindowPos( _hwndDlg, HWND_TOP, 0, 0, 0, 0,  SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE );
}


VOID CArcDlg::UpdateConnectionAttempts()
{
	//Update the UI here if it needs to be updated
}

void CArcDlg::OnEraseBkgnd(HWND hwnd, HDC hdc)
{
	FillRect(hdc, &_rcTop, _hbrTopBand);
	FillRect(hdc, &_rcMid, _hbrMidBand);
	FillRect(hdc, &_rcBtm, _hbrTopBand);
}

void CArcDlg::StopAutoReconnect()
{
	DeleteObject(_hbrTopBand);
	DeleteObject(_hbrMidBand);
	DeleteObject(_hbmpProgband);
	KillTimer(_hwndDlg, (UINT)this);

	CDlgBase::Destroy();
}


void CALLBACK CArcDlg::StaticProgbandTimerCallback(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	CArcDlg *pArcDlg = (CArcDlg *)idEvent;
	ASSERT(pArcDlg);

	pArcDlg->ProgbandTimerCallback(hwnd, uMsg, idEvent, dwTime);
}

void CArcDlg::ProgbandTimerCallback(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{

	ASSERT((DWORD)this == idEvent);

	HDC hdc = GetDC(hwnd);
	if(!hdc)
		return;

	HDC hdcMem = CreateCompatibleDC(hdc);
	if (!hdcMem)
		goto Cleanup;

	int nHeight, nWidth;
	nHeight = _rcProgband.bottom - _rcProgband.top;
	nWidth = _rcProgband.right - _rcProgband.left;

	HBITMAP hBmpOld;
	hBmpOld = (HBITMAP)SelectObject(hdcMem, _hbmpProgband);

	BitBlt(hdc, _rcProgband.left, _rcProgband.top, _lProgress, nHeight,
			hdcMem, nWidth - _lProgress, 0, SRCCOPY);

	BitBlt(hdc, _lProgress, _rcProgband.top, nWidth -_lProgress, nHeight, hdcMem, 0, 0, SRCCOPY);

	SelectObject(hdcMem, hBmpOld);

	DeleteDC(hdcMem);

	_lProgress += 1;
	if (_lProgress > nWidth)
		_lProgress = 0;

Cleanup:
	ReleaseDC(hwnd, hdc);
}