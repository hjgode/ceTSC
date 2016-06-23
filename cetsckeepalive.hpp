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
#define		__INCLUDED_CETSCKEEPALIVE_HPP__

#include	<windows.h>

#ifndef	_MAINWND_H_
#include	"MainWnd.h"
#endif	// _MAINWND_H_

class	CetscKeepAlive_t
{
private:
	static	const	int		s_KeepAliveInterval = 5*60*1000;//5 minutes in millisec
	static	CMainWnd*		s_pCetscMainWnd;
	static	HANDLE		s_KeepAliveEvent;
	
	int			m_TimerId;
	
	
public:
	CetscKeepAlive_t(
		CMainWnd*		pCetscMainWnd
		);

	~CetscKeepAlive_t(
		);

private:
	static
	void 
	CALLBACK 
	TimerProc(
		  HWND		hwnd, 
		  UINT		uMsg, 
		  UINT		idEvent, 
		  DWORD		dwTime 
		);

};


#endif //__INCLUDED_CETSCKEEPALIVE_HPP__

