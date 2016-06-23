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
#ifndef TSCONMAN_HPP
#define TSCONMAN_HPP

#define MAX_SERVER_NAME				256
#define MAX_CONNECTION_DESCRIPTION	32
#define MAX_USERNAME_LENGTH			512
#define MAX_DOMAIN_LENGTH			512
#define MAX_PASSWORD_LENGTH			512

#define DEFAULT_RDP_PORT			3389

class TsConnectionWizard
{
public:
	void WriteSettings();
	BOOL Execute();
	TsConnectionWizard();
	~TsConnectionWizard();
	
	TCHAR m_szConnectionName [MAX_CONNECTION_DESCRIPTION];
	TCHAR m_szServerName [MAX_SERVER_NAME];
	
	BOOL m_bAutoLogon;
	TCHAR m_szUserName [MAX_USERNAME_LENGTH];
	TCHAR m_szPassword [MAX_PASSWORD_LENGTH];
	TCHAR m_szDomain [MAX_DOMAIN_LENGTH];
	
	BOOL m_bRunningDesktop;
	BOOL m_bLowSpeedConnection;
	
	TCHAR m_szAlternateShell [MAX_PATH];
	TCHAR m_szShellWorkingDirectory [MAX_PATH];

	DWORD m_dwRdpPort;
};

#endif // TSCONMAN_HPP
