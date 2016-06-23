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
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//

#ifndef		__INCLUDED_PICTORAPPMESSAGES_HPP__
#define		__INCLUDED_PICTORAPPMESSAGES_HPP__


#ifndef		__INCLUDED_PICTORWMMESSAGES_H__
#include		"PictorWMMessages.h"
#endif		//__INCLUDED_PICTORWMMESSAGES_H__

static	const	WCHAR*		const	s_pPictorAppWndClassName = L"ODLibBorderWnd";

class	PictorAppMessages_t
{
public:
	static
	bool
	PostConnectionFailed(
		)
		{
			//Find pictor app's window
			HWND		PictorWnd = FindWindow( s_pPictorAppWndClassName, NULL );
			
			//Send message to the window
			if(PictorWnd)
				{
				return	!!PostMessage(PictorWnd, WM_APP_CONNECTIONFAILED, 0, 0);
				}
			
			return	false;
		}

	static
	bool
	PostClearStatus(
		)
		{
			//Find pictor app's window
			HWND		PictorWnd = FindWindow( s_pPictorAppWndClassName, NULL );
			
			//Send message to the window
			if(PictorWnd)
				{
				return	!!PostMessage(PictorWnd, WM_APP_CLEARSTATUS, 0, 0);
				}
			
			return	false;
		}

	static
	bool
	PostStatusConnecting(
		)
		{
			//Find pictor app's window
			HWND		PictorWnd = FindWindow( s_pPictorAppWndClassName, NULL );
			
			//Send message to the window
			if(PictorWnd)
				{
				return	!!PostMessage(PictorWnd, WM_APP_STATUSCONNECTING, 0, 0);
				}
			
			return	false;
		}

	static
	bool
	PostStatusReceivedInvitation(
		)
		{
			//Find pictor app's window
			HWND		PictorWnd = FindWindow( s_pPictorAppWndClassName, NULL );
			
			//Send message to the window
			if(PictorWnd)
				{
				return	!!PostMessage(PictorWnd, WM_APP_STATUSRECEIVEDINVITATION, 0, 0);
				}
			
			return	false;
		}

};

#endif //__INCLUDED_PICTORAPPMESSAGES_HPP__

