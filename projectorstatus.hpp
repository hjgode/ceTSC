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

#ifndef		__INCLUDED_PROJECTORSTATUS_HPP__
#define		__INCLUDED_PROJECTORSTATUS_HPP__

class	ProjectorStatus_t
{
private:
	HANDLE		m_RdpShutdownEvent;
public:
	ProjectorStatus_t(
		)
		{
		m_RdpShutdownEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"RDPClientShutdownEvent");
		}

	~ProjectorStatus_t(
		)
		{
		if( m_RdpShutdownEvent )
			{
			SetEvent(m_RdpShutdownEvent);
			CloseHandle(m_RdpShutdownEvent);
			}
		}
};

#endif	//__INCLUDED_PROJECTORSTATUS_HPP__
