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
#include "common.h"

void InitCombo(HINSTANCE hInst, HWND hwndDlg, DWORD dwId, ComboItem *pItem, int nCount)
{
	HWND hwnd  = GetDlgItem(hwndDlg, dwId);
	ASSERT(hwnd);

	for (int i=0; i<nCount; i++)
	{
		LPWSTR pszStr = (LPWSTR )LoadString(hInst, pItem[i].dwStrId, NULL, 0);
		ASSERT(pszStr);

		pItem[i].nIndex = SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM )pszStr);
		SendMessage(hwnd, CB_SETITEMDATA, (WPARAM )pItem[i].nIndex, (LPARAM )pItem[i].dwVal);
	}
}

BOOL InitServersCombo(ISettings *pSettings, HWND hwndCombo)
{
	WCHAR *pszServers = NULL;
	WCHAR pszEmpty[] = L"";
	TscPropType eType = TscTypeSTRING;

	ASSERT(pSettings);

	SendMessage(hwndCombo, CB_RESETCONTENT, 0, 0);

	BOOL bServer = FALSE;
	WCHAR szServer[MAX_SERVERNAMELEN];
	DWORD dwSize = sizeof(szServer);
	if ((pSettings->Read(PROP_SERVER, &eType, (PBYTE)pszEmpty, (PBYTE)szServer, &dwSize)) &&
		(szServer[0] != L'\0') )
	{
		DWORD dwPort = DEFAULT_RDP_PORT;
		dwSize = sizeof(DWORD);
		if ( (pSettings->Read(PROP_RDPPORT, &eType, NULL, (PBYTE)&dwPort, &dwSize)) && (dwPort != DEFAULT_RDP_PORT) )
		{
			WCHAR szPort[16];
			szPort[0] = L':';
			_itow(dwPort, &szPort[1], 10);
			if (S_OK != StringCchCat(szServer, MAX_SERVERNAMELEN, szPort)) 
				return FALSE;
		}

		SetWindowText(hwndCombo, szServer);	
		bServer = TRUE;
	}

	dwSize = 0;
	BOOL bRet = pSettings->Read(L"MRU", &eType, (PBYTE)pszEmpty, NULL, &dwSize);
	if ((!bRet) || (dwSize == 0))
	{
		return TRUE;
	}

	__try
	{
		pszServers = (WCHAR *)_alloca(dwSize);
	}
	__except(1)
	{
		DEBUGMSG(ZONE_ERROR, (L"Size of servers list too long(%d)\n", dwSize));
		return FALSE;
	}

	pszServers[dwSize] = L'\0';

	if (!pSettings->Read(L"MRU", &eType, (PBYTE)pszEmpty, (PBYTE)pszServers, &dwSize))
	{
		return FALSE;
	}


	int nCount = 0;
	WCHAR seps[] = L";";
	WCHAR *pTok = wcstok(pszServers, seps);
	while((pTok) && (nCount++ < MAX_MRUCOUNT))
	{
		SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)pTok);
		pTok = wcstok(NULL, seps);
	}

	if (!bServer)
	{
		SendMessage(hwndCombo, CB_SETCURSEL, 0, 0);
	}

	return TRUE;	
}

BOOL SaveServersCombo(ISettings *pSettings, HWND hwndCombo)
{
	
	WCHAR szServer[MAX_SERVERNAMELEN];
	szServer[0] = L'\0';
	GetWindowText(hwndCombo, szServer, MAX_SERVERNAMELEN);
	PWSTR pszPort = wcschr(szServer, L':');
	if (pszPort)
		*pszPort = L'\0';

	DWORD dwPort = ((pszPort) && *(pszPort+1)) ? _wtoi(pszPort + 1) : DEFAULT_RDP_PORT;

	if (!pSettings->Write(PROP_RDPPORT, TscTypeULONG, (PBYTE)&dwPort, sizeof(DWORD) ))
		return FALSE;

	return pSettings->Write(PROP_SERVER, TscTypeSTRING, (PBYTE)szServer, (wcslen(szServer)+1)*sizeof(WCHAR) );
}

BOOL UpdateServersCombo(ISettings *pSettings, HWND hwndCombo)
{
	WCHAR szServer[MAX_SERVERNAMELEN], pszEmpty[]=L"";
	TscPropType eType = TscTypeSTRING;
	DWORD dwSize = sizeof(szServer), dwPort = DEFAULT_RDP_PORT;

	if ( (!pSettings->Read(PROP_SERVER, &eType, (PBYTE)pszEmpty, (PBYTE)szServer, &dwSize)) ||
		 (eType != TscTypeSTRING) || (dwSize >= sizeof(szServer)) || (szServer[0] == L'\0') )
	{
		return FALSE;
	}

	dwSize = sizeof(DWORD);
	if ( (pSettings->Read(PROP_RDPPORT, &eType, NULL, (PBYTE)&dwPort, &dwSize)) && (dwPort != DEFAULT_RDP_PORT) )
	{
		WCHAR szPort[16];
		szPort[0] = L':';
		_itow(dwPort, &szPort[1], 10);
		if (S_OK != StringCchCat(szServer, MAX_SERVERNAMELEN, szPort)) 
			return FALSE;
	}
	
	int nIndex = SendMessage(hwndCombo, CB_FINDSTRINGEXACT, -1, (LPARAM)szServer);
	if (nIndex != CB_ERR)
	{
		SendMessage(hwndCombo, CB_DELETESTRING, nIndex, 0);
	}

	int nCount = SendMessage(hwndCombo, CB_GETCOUNT, 0, 0);
	if (nCount >= MAX_MRUCOUNT)
	{
		SendMessage(hwndCombo, CB_DELETESTRING, nCount - 1, 0);
	}
	SendMessage(hwndCombo, CB_INSERTSTRING, 0, (LPARAM)szServer);
	SendMessage(hwndCombo, CB_SETCURSEL, 0, NULL);

	return TRUE;
}


VOID EnableControls(HWND hwndParent, PDWORD pdwIds, int nCount, BOOL bEnable)
{
	for (int i=0; i<nCount; i++)
	{
		EnableWindow(GetDlgItem(hwndParent, pdwIds[i]), bEnable); 
	}
}

void ShiftWindow(HWND hwnd, int nXDelta, int nYDelta, int nCXDelta/*=0*/, int nCYDelta/*=0*/)
{
	RECT	rc;
	GetWindowRect(hwnd, &rc);

	MapWindowPoints(NULL, GetParent(hwnd), (LPPOINT )&rc, 2);

	MoveWindow(hwnd, rc.left + nXDelta, rc.top + nYDelta, (rc.right - rc.left) + nCXDelta, (rc.bottom - rc.top)+ nCYDelta, TRUE);
}
