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
#include "Stdafx.h"
#include "TsConWiz.hpp"
#include "DescriptionProperties.hpp"
#include "LogonProperties.hpp"
#include "ApplicationProperties.hpp"
#include "FinishProperties.hpp"
#include "RegString.hpp"
#include "regmgr.h"
#include "PropClasses.hpp"


#define REGISTRATION_CONNECTIONS_KEY _T("Software\\Microsoft\\WBT\\Clients\\Registration\\RDP\\Connections")

#define REG_TSC_PATH      _T("Software\\Microsoft\\Terminal Server Client")
#define MSTSC_CLIENT_NAME _T("cetsc.exe")

#define REGISTERED_CLIENT_KEY _T("Software\\Microsoft\\WBT\\Clients\\Registration\\RDP")
#define CONNECTIONS_KEY		  _T("Connections")

#define TSCONMAN_CLIENT_NAME _T("TSConMan")


TsConnectionWizard::TsConnectionWizard()
{

	memset(&m_szConnectionName,0, sizeof(m_szConnectionName));
	memset(&m_szServerName,0, sizeof(m_szServerName));
	
	memset(&m_szUserName,0, sizeof(m_szUserName));
	memset(&m_szPassword ,0, sizeof(m_szPassword));
	memset(&m_szDomain ,0, sizeof(m_szDomain));
	
	memset(&m_szAlternateShell ,0, sizeof(m_szAlternateShell));
	memset(&m_szShellWorkingDirectory ,0, sizeof(m_szShellWorkingDirectory));
	 
	m_szConnectionName[0] = 0;
	m_szServerName[0] = 0;
	
	m_bAutoLogon = FALSE;
	m_szUserName[0] = 0;
	m_szPassword[0] = 0;	
	m_szDomain[0] = 0;
	
	m_bRunningDesktop = TRUE;
	m_bLowSpeedConnection = TRUE;	//Making it a "Low Speed Connection" will turn on compression
	
	m_szAlternateShell[0] = 0;
	m_szShellWorkingDirectory[0] = 0;

	m_dwRdpPort = DEFAULT_RDP_PORT;

	// note: the busy flag is written by wbttskmn before
	// we are created, so no need to do it again here

}

TsConnectionWizard::~TsConnectionWizard()
{
	// clear the busy flag
	HKEY hKey = OpenRegistryKey(HKCU, REG_TSC_PATH, KEY_WRITE);
	if (hKey != NULL)
	{
		SetRegistryKeyDWORD(hKey, _T("ConnBeingEdited"), 0);
		DEBUGMSG (1, (L"TsConnectionWizard::Clearing ConnBeingEdit flag\n"));
		RegCloseKey(hKey);
	} // if
}

BOOL TsConnectionWizard::Execute()
{
	const int nPages = 4;
	int i			 = 0;
	BOOL bPage = TRUE;
	CConWizPropSheet* sheet = NULL;
	LPCTSTR pszTitle = LoadStringEx (IDS_CONWIZ_TITLE);
	// create some pages
	CPropPage *pages[nPages];

	for (i=0;i<nPages; i++)
		pages[i] = NULL;

	__try	
	{
		// create property sheet
		sheet = new CConWizPropSheet (NULL, this, pszTitle);

		// create pages in reverse order due to ATL window mappings
		// being chained
		pages[0] = new CDescriptionPropPage (sheet);
		pages[1] = new CLogonPropPage (sheet);
		pages[2] = new CApplicationPropPage (sheet);
		pages[3] = new CFinishPropPage (sheet);

	}__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DEBUGMSG (1, (L"TsConnectionWizard::Execute() -- No Owner specified"));
		goto EXIT_PROC;
	}

	if (!sheet)
		goto EXIT_PROC;

	for ( i = 0;i < nPages;i++)
	{
		if(pages[i] == NULL)
		{
			bPage = FALSE;
			break;
		}
		sheet->AddPropPage (pages[i]);
	}
	
	if(bPage)
	{
		// activate page on startup
		sheet->Run();
	}
	
EXIT_PROC:	
	for (i=0; (i < nPages) && pages[i]; i++)
	{
		delete pages[i];
	}

	if (sheet)
		delete sheet;
	else
		return FALSE;

	return TRUE;
}



int MessageBox (LPCTSTR lpszText, LPCTSTR lpszCaption, UINT nType)
{
#ifdef DEBUG
	DebugBreak();
#endif
	return ::MessageBox (NULL, lpszText, lpszCaption, nType);
}



void TsConnectionWizard::WriteSettings()
{
	// write all the settings to the registry
	DEBUGMSG (1, (L"TsConnectionWizard::WriteSettings()\n"));

	/****************************/
	/* Client Registration Area */
	/****************************/
	DWORD dwDisp =0;
	DWORD dwResult =0;

	CRegString strPropertiesCmdLineArgs;
	CRegString strDeleteCmdLineArgs;
	CRegString strDeleteExe;
	CRegString strDescription;
	CRegString strStartExe;
	CRegString strStartCmdLineArgs;
	CRegString strPropertiesExe;

	// Create a new connection key under 
	// HKLM\Software\Microsoft\WBT\Client\Registration\MSTSC

	// First, make sure that the "Connections" key exists under the 
	// current registered client key.
	HKEY hKeyConnections= NULL;
	HKEY hKeyCurrentLMConnection= NULL;

	TCHAR szConnectionsKey [500] = {0};
	TCHAR szCurrentConnection [500] = {0};
	
	StringCchCopy (szConnectionsKey, 500, REGISTERED_CLIENT_KEY);
	StringCchCat (szConnectionsKey, 500, L"\\");
	StringCchCat (szConnectionsKey, 500, CONNECTIONS_KEY);

	// Build the specific connection name to delete
	StringCchCopy (szCurrentConnection, 500, szConnectionsKey);
	StringCchCat (szCurrentConnection, 500, L"\\");
	StringCchCat (szCurrentConnection, 500, m_szConnectionName);

	// Delete the key regardless of whether it exists or not.
	RegDeleteKey(HKEY_LOCAL_MACHINE, szCurrentConnection);

	// Open the "Connections" key
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, szConnectionsKey, 0, KEY_ALL_ACCESS, &hKeyConnections))
	{
		// Try to create the "Connections" key.
		RegCreateKeyEx(HKEY_LOCAL_MACHINE, szConnectionsKey, 0, NULL, 
					   REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKeyConnections, &dwDisp);
	}

	// Create the current connection as a key under hKeyConnections
	dwResult = RegCreateKeyEx(hKeyConnections, m_szConnectionName, 
							  0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, 
							  &hKeyCurrentLMConnection, &dwDisp);

	if (dwResult != ERROR_SUCCESS)
		goto RegFailed;

	/* Description value */
	
	strDescription.m_baseKey = HKEY_LOCAL_MACHINE;
	strDescription.m_keyName = szCurrentConnection;
	strDescription.m_valueName = L"Description";
	StringCchCopy (strDescription.m_szValue, BUFFER_SIZE, m_szConnectionName);
	
	if (!strDescription.Set())
		goto RegFailed;

	/* StartExe value */
	
	strStartExe.m_baseKey = HKEY_LOCAL_MACHINE;
	strStartExe.m_keyName = szCurrentConnection;
	strStartExe.m_valueName = L"StartExe";
	StringCchCopy (strStartExe.m_szValue, BUFFER_SIZE, MSTSC_CLIENT_NAME);
	
	if (!strStartExe.Set())
		goto RegFailed;

	/* StartCmdLineArgs value */
	TCHAR szQuotedCurrentConnection [512];

	StringCchPrintf (szQuotedCurrentConnection, 512, L"/s %s%s%s", L"\"", m_szConnectionName, L"\"");


	strStartCmdLineArgs.m_baseKey = HKEY_LOCAL_MACHINE;
	strStartCmdLineArgs.m_keyName = szCurrentConnection;
	strStartCmdLineArgs.m_valueName = L"StartCmdLineArgs";
	StringCchCopy (strStartCmdLineArgs.m_szValue, BUFFER_SIZE, szQuotedCurrentConnection);
	
	if (!strStartCmdLineArgs.Set())
		goto RegFailed;

	/* PropertiesExe value */

	strPropertiesExe.m_baseKey = HKEY_LOCAL_MACHINE;
	strPropertiesExe.m_keyName = szCurrentConnection;
	strPropertiesExe.m_valueName = L"PropertiesExe";
	StringCchCopy (strPropertiesExe.m_szValue, BUFFER_SIZE, TSCONMAN_CLIENT_NAME);
	
	if (!strPropertiesExe.Set())
		goto RegFailed;

	/* PropertiesCmdLineArgs value */
	TCHAR szPropCmdLine [512];

	StringCchPrintf (szPropCmdLine, 512, L"%s%s", L"/e ", m_szConnectionName);
	DEBUGMSG (1, (L"****** szPropCmdLine = [%s]\n", szPropCmdLine));

	strPropertiesCmdLineArgs.m_baseKey = HKEY_LOCAL_MACHINE;
	strPropertiesCmdLineArgs.m_keyName = szCurrentConnection;
	strPropertiesCmdLineArgs.m_valueName = L"PropertiesCmdLineArgs";
	StringCchCopy (strPropertiesCmdLineArgs.m_szValue, BUFFER_SIZE, szPropCmdLine);
	
	if (!strPropertiesCmdLineArgs.Set())
		goto RegFailed;

	/* DeleteExe value */

	strDeleteExe.m_baseKey = HKEY_LOCAL_MACHINE;
	strDeleteExe.m_keyName = szCurrentConnection;
	strDeleteExe.m_valueName = L"DeleteExe";
	StringCchCopy (strDeleteExe.m_szValue, BUFFER_SIZE, TSCONMAN_CLIENT_NAME);
	
	if (!strDeleteExe.Set())
		goto RegFailed;

	/* DeleteCmdLineArgs value */
	TCHAR szDelCmdLine [512];

	StringCchPrintf (szDelCmdLine, 512, L"%s%s", L"/d ", m_szConnectionName);
	DEBUGMSG (1, (L"****** szDelCmdLine = [%s]\n", szDelCmdLine));


	strDeleteCmdLineArgs.m_baseKey = HKEY_LOCAL_MACHINE;
	strDeleteCmdLineArgs.m_keyName = szCurrentConnection;
	strDeleteCmdLineArgs.m_valueName = L"DeleteCmdLineArgs";
	StringCchCopy (strDeleteCmdLineArgs.m_szValue, BUFFER_SIZE, szDelCmdLine);
	
	if (!strDeleteCmdLineArgs.Set())
		goto RegFailed;

	/*************************************/
	/* RDP-Specific Connection Info Area */
	/*************************************/
	HKEY hKeyTSClient;
	HKEY hKeyCUConnection;
	HKEY hKeyMicrosoft;	

	// Set stuff that is relevant only to an RDP connection into the
	// HKCU\Software\Microsoft\Terminal Server Client\<Connection Name> key.

	StringCchCopy (szConnectionsKey, 500, REG_TSC_PATH);
	StringCchCat (szConnectionsKey, 500, L"\\");
	StringCchCat (szConnectionsKey, 500, m_szConnectionName);

	// Make sure the current connection key is gone first. Do this regardless of whether
	// the path in the registry is there or not. Whether it succeeds or fails, the key
	// will be "fresh" when we start writing values to it.
	RegDeleteKey(HKEY_CURRENT_USER, szConnectionsKey);

	// Make sure that the HKCU\Software\Microsoft\Terminal Server Client key exists.
	dwResult = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft"), 0, KEY_WRITE, &hKeyMicrosoft);
	if (dwResult != ERROR_SUCCESS)
	{
		// Create the Microsoft key under Software
		RegCreateKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft"), 0, NULL, 
					   REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKeyMicrosoft, &dwDisp);
	}

	// Make sure that the Terminal Server Client key exists.
	dwResult = RegOpenKeyEx(hKeyMicrosoft, _T("Terminal Server Client"), 0, KEY_WRITE, &hKeyTSClient);
	if (dwResult != ERROR_SUCCESS)
	{
		// Create the Terminal Server Client key under Microsoft
		RegCreateKeyEx(hKeyMicrosoft, _T("Terminal Server Client"), 0, NULL, 
					   REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKeyTSClient, &dwDisp);
	}

	// Create a new subkey under the Terminal Server Client key with the specified name
	DWORD dwAutoConnect;
	dwAutoConnect = 1;
	dwResult = RegCreateKeyEx(hKeyTSClient, m_szConnectionName,
							  0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKeyCUConnection, 
							  &dwDisp);
	// Close open keys
	RegCloseKey(hKeyTSClient);
	RegCloseKey(hKeyMicrosoft);

	if (dwResult != ERROR_SUCCESS)
		goto RegFailed;


	// Set "MRU0" value
	dwResult = RegSetValueEx(hKeyCUConnection, _T("MRU0"), 0, REG_SZ,
							(BYTE *)m_szServerName, 
							(wcslen(m_szServerName) * sizeof(TCHAR)) 
							+ sizeof(TCHAR));
	if (dwResult != ERROR_SUCCESS)
		goto RegFailed;

	// Set "server" value
	dwResult = RegSetValueEx(hKeyCUConnection, PROP_SERVER, 0, REG_SZ,
							(BYTE *)m_szServerName, 
							(wcslen(m_szServerName) * sizeof(TCHAR)) 
							+ sizeof(TCHAR));
	if (dwResult != ERROR_SUCCESS)
		goto RegFailed;

	dwResult = RegSetValueEx(hKeyCUConnection, PROP_RDPPORT, 0, REG_DWORD, 
							(BYTE *)&m_dwRdpPort,
							sizeof(DWORD));
	if (dwResult != ERROR_SUCCESS)
		goto RegFailed;


	
	// Set "Auto Connect" value
	dwResult = RegSetValueEx(hKeyCUConnection, _T("Auto Connect"), 0, REG_DWORD,
							 (BYTE *)&dwAutoConnect, sizeof(DWORD));
	if (dwResult != ERROR_SUCCESS)
		goto RegFailed;

    /* "AutoLogon" if the checkbox is marked. */
#ifdef WBT_USEAUTOLOGON
	if (m_bAutoLogon)
#endif
	{	
		DWORD dwAutoLogon;
		dwAutoLogon = 1;
		dwResult = RegSetValueEx(hKeyCUConnection, _T("AutoLogon"), 0, REG_DWORD,
								 (BYTE *)&dwAutoLogon, sizeof(DWORD));
		if (dwResult != ERROR_SUCCESS)
			goto RegFailed;

		// "UserName" value...Only if we actually got something
		dwResult = RegSetValueEx(hKeyCUConnection, PROP_USERNAME, 0, REG_SZ, 
								 (BYTE *)(LPTSTR)m_szUserName, 
								 (wcslen((LPTSTR)m_szUserName) * sizeof(TCHAR)) 
								 + sizeof(TCHAR));
		if (dwResult != ERROR_SUCCESS)
			goto RegFailed;

		// "Domain" value...Only if we actually got something
		dwResult = RegSetValueEx(hKeyCUConnection, PROP_DOMAIN, 0, REG_SZ, 
								 (BYTE *)(LPTSTR)m_szDomain, 
								 (wcslen((LPTSTR)m_szDomain) * sizeof(TCHAR)) 
								 + sizeof(TCHAR));
		if (dwResult != ERROR_SUCCESS)
			goto RegFailed;

		// "Password" value...Only if we actually got something
		if (_tcslen(m_szPassword) != 0)
		{
			if (_tcscmp(m_szPassword, L"******") != 0)   // Original password for connection not changed 
			{
				// Encrypt the password
				DATA_BLOB blobOut;
				blobOut.cbData = 0;
				blobOut.pbData = NULL;
				if (EncryptPassword (m_szPassword, &blobOut))
				{
					dwResult = RegSetValueEx(hKeyCUConnection, PROP_CLEARTEXTPASSWORD, 0, REG_BINARY, 
											blobOut.pbData, blobOut.cbData);
					LocalFree(blobOut.pbData);
				}
				else 
					dwResult = ERROR_INVALID_PARAMETER;
			}		

			if (dwResult != ERROR_SUCCESS)
				goto RegFailed;
		}
		else
		{
			DATA_BLOB blobOut;
			blobOut.cbData = 0;
			blobOut.pbData = NULL;
			if (EncryptPassword (_T(""), &blobOut))
			{
				dwResult = RegSetValueEx(hKeyCUConnection, PROP_CLEARTEXTPASSWORD, 0, REG_BINARY, 
										blobOut.pbData, blobOut.cbData);
				LocalFree(blobOut.pbData);
			}
			else 
				dwResult = ERROR_INVALID_PARAMETER;

			if (dwResult != ERROR_SUCCESS)
				goto RegFailed;
		}
	}

	// Always full screen
	DWORD dwScreenModeId;
	dwScreenModeId = 1;
	dwResult = RegSetValueEx(hKeyCUConnection, PROP_STARTFULLSCREEN, 0, REG_DWORD,
							 (BYTE *)&dwScreenModeId, sizeof(DWORD));
	if (dwResult != ERROR_SUCCESS)
		goto RegFailed;

	//Do not use local keyboard hooking
	DWORD dwKbdHook;
	dwKbdHook = 0;
	dwResult = RegSetValueEx(hKeyCUConnection, PROP_KEYBOARDHOOKMODE, 0, REG_DWORD,
							 (BYTE *)&dwKbdHook, sizeof(DWORD));
	if (dwResult != ERROR_SUCCESS)
		goto RegFailed;

	// Low Speed Connection --
	// Write the Compression key: 1 for low speed connection, 0 for regular connection
	// NOTE: Compression=1 is the same as the Compression key missing
	dwResult = RegSetValueEx(hKeyCUConnection, PROP_COMPRESS, 0, REG_DWORD,
							 (BYTE *)&m_bLowSpeedConnection, sizeof(DWORD));
	if (dwResult != ERROR_SUCCESS)
		goto RegFailed;


////////////////////////////////////////////////
	// For the Application page, check to see what the user has selected
	if (m_bRunningDesktop)
	{
		dwResult = RegSetValueEx(hKeyCUConnection, PROP_STARTPROGRAM, 0, REG_SZ, 
								 (BYTE *)(_T("")), (wcslen(_T("")) * sizeof(TCHAR)) 
								 + sizeof(TCHAR));
		if (dwResult != ERROR_SUCCESS)
			goto RegFailed;
	}
	else
	{
		dwResult = RegSetValueEx(hKeyCUConnection, PROP_STARTPROGRAM, 0, REG_SZ,
								 (BYTE *)(LPTSTR)m_szAlternateShell, 
								 (wcslen((LPTSTR)m_szAlternateShell) * sizeof(TCHAR)) 
								 + sizeof(TCHAR));
		if (dwResult != ERROR_SUCCESS)
			goto RegFailed;

		DWORD dwDesktop;
		dwDesktop = 2;
		dwResult = RegSetValueEx(hKeyCUConnection, _T("Desktop"), 0 , REG_DWORD,
							     (BYTE *)&dwDesktop, sizeof(DWORD));
		if (dwResult != ERROR_SUCCESS)
			goto RegFailed;

		dwResult = RegSetValueEx(hKeyCUConnection, PROP_WORKDIR, 0, REG_SZ,
								 (BYTE *)(LPTSTR)m_szShellWorkingDirectory, 
								 (wcslen((LPTSTR)m_szShellWorkingDirectory) * sizeof(TCHAR)) 
								 + sizeof(TCHAR));
	}

RegFailed:
	if (dwResult != ERROR_SUCCESS)
	{
		LPCTSTR pszErrorTitle = LoadStringEx (IDS_REG_SET_FAILED);
		LPCTSTR pszMsg = LoadStringEx (IDS_REG_SET_FAILED_MESSAGE);
		
		TCHAR szMsg [MAX_DISPLAY_STRING] = {0};
		wsprintf (szMsg, pszMsg, GetLastError());
		::MessageBox (NULL, szMsg, pszErrorTitle, MB_ICONINFORMATION | MB_OK);
	}

	// Close keys
	RegCloseKey(hKeyCurrentLMConnection);
	RegCloseKey(hKeyConnections);

	// Close open keys
	RegCloseKey(hKeyCUConnection);

	//*******************************************************************************
	// Put up the wait cursor
	HCURSOR hOldCursor = SetCursor (LoadCursor (NULL, IDC_WAIT));

	// Flush the MSTSC key
	HKEY hKeyFlush;

	RegOpenKeyEx(HKEY_CURRENT_USER, REG_TSC_PATH, 0, KEY_WRITE, &hKeyFlush);
	RegFlushKey(hKeyFlush);
	RegCloseKey(hKeyFlush);

	// Flush the registered connections key
	_tcscpy (szConnectionsKey, REGISTERED_CLIENT_KEY);
	_tcscat (szConnectionsKey, L"\\");
	_tcscat (szConnectionsKey, CONNECTIONS_KEY);

	RegOpenKeyEx(HKEY_LOCAL_MACHINE, szConnectionsKey, 0, KEY_WRITE, &hKeyFlush);
	RegFlushKey(hKeyFlush);
	RegCloseKey(hKeyFlush);

	// Remove the wait cursor
	SetCursor (hOldCursor);
	//*******************************************************************************
}
