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
// -----------------------------------------------------------------------------
//
//      THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//      ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//      THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//      PARTICULAR PURPOSE.
//  
// -----------------------------------------------------------------------------

#ifndef __INCLUDED_DEBUG_H__
#define __INCLUDED_DEBUG_H__

#include <windows.h>

#ifdef DEBUG
extern	DBGPARAM dpCurSettings;
#endif //DEBUG


#define ZONE_INIT		DEBUGZONE(0)		// 0x0001
#define ZONE_OPEN		DEBUGZONE(1)		// 0x0002
#define ZONE_QUERY		DEBUGZONE(2)		// 0x0004
#define ZONE_SERVER		DEBUGZONE(3)		// 0x0008
#define ZONE_MEDIAITEM		DEBUGZONE(4)		// 0x0010

#define ZONE_FUNCTION		DEBUGZONE(12)		// 0x1000
#define ZONE_MISC		DEBUGZONE(13)		// 0x2000
#define ZONE_WARNING		DEBUGZONE(14)		// 0x4000
#define ZONE_ERROR		DEBUGZONE(15)		// 0x8000


//#define MediaQueryEngineFunc(Printf_exp)	DEBUGMSG( ZONE_FUNCTION, Printf_exp )


class FuncBoundaryMsg
{
	private:
		WCHAR*	m_pMessage;

	public:

	FuncBoundaryMsg( 
		WCHAR *pString
		)
	{
		m_pMessage = pString;
		DEBUGMSG( ZONE_FUNCTION, ( L"%s %s", L"Enter " ,pString ) );
	}

	~FuncBoundaryMsg( 
		void
		)
	{
		DEBUGMSG( ZONE_FUNCTION, ( L"%s %s", L"Exit " ,m_pMessage) );
	}
};


#ifdef DEBUG
#define	FUNCDEBUGMESSAGE(Msg)	FuncBoundaryMsg Message(Msg)
#else
#define	FUNCDEBUGMESSAGE(Msg)
#endif

#endif //__INCLUDED_DEBUG_H__


