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

#ifndef		__INCLUDED_CETSCKEEPALIVE_HPP__
#include	"CetscKeepAlive.hpp"
#endif		//__INCLUDED_CETSCKEEPALIVE_HPP__


CMainWnd*		CetscKeepAlive_t::s_pCetscMainWnd = NULL;
HANDLE		CetscKeepAlive_t::s_KeepAliveEvent = NULL;

CetscKeepAlive_t::
CetscKeepAlive_t(
	CMainWnd*		pCetscMainWnd
	):
	m_TimerId( 0 )
{
	s_pCetscMainWnd = pCetscMainWnd;
	s_KeepAliveEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"CetscKeepAliveEvent");

	if(s_KeepAliveEvent && s_pCetscMainWnd)
		{
		//Setup timer
		m_TimerId = SetTimer(NULL, 1, s_KeepAliveInterval, (TIMERPROC)TimerProc);
		}
}


CetscKeepAlive_t::
~CetscKeepAlive_t(
	)
{
	KillTimer(NULL, m_TimerId );
	CloseHandle( s_KeepAliveEvent );
	s_KeepAliveEvent = NULL;
	s_pCetscMainWnd = NULL;
}

void 
CALLBACK 
CetscKeepAlive_t::
TimerProc(
	  HWND		hwnd, 
	  UINT		uMsg, 
	  UINT		idEvent, 
	  DWORD		dwTime 
	)
{
	ASSERT(s_pCetscMainWnd);
	if( (s_pCetscMainWnd->GetState() == Connected) || (s_pCetscMainWnd->GetState() == LoggedIn))
		{
		SetEvent(s_KeepAliveEvent);
		}
	return;
}



