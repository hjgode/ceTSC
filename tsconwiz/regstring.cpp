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
#include "StdAfx.h"
#include "RegString.hpp"

// For Registry Flush
//#include "utils.h"

CRegString::CRegString()
{
	m_szValue[0] = 0;
}

CRegString::~CRegString()
{
}

BOOL CRegString::Get()
{
	// Open the specified key
	if (ERROR_SUCCESS != RegOpenKeyEx(m_baseKey, m_keyName, 0, KEY_READ, &m_Key))
		return FALSE;

	DWORD   type;
	DWORD   size = BUFFER_SIZE;
	TCHAR   buffer [BUFFER_SIZE];
	TCHAR * bufPointer = buffer;
	BOOL    result     = TRUE;

	if (ERROR_SUCCESS != RegQueryValueEx(m_Key, m_valueName, NULL, &type, 
										(unsigned char *)bufPointer,&size))
		result = FALSE;

	_tcscpy (m_szValue, buffer);

	// Close the key
	RegCloseKey(m_Key);

	return result;
}

BOOL CRegString::Set()
{
	// Open the specified key
	if (RegOpenKeyEx(m_baseKey, m_keyName, 0, KEY_ALL_ACCESS, &m_Key) !=
					ERROR_SUCCESS)
		return FALSE;

	BOOL result = TRUE;

	if (ERROR_SUCCESS != RegSetValueEx(m_Key, m_valueName, NULL, REG_SZ, 
										(BYTE *)m_szValue, (_tcslen(m_szValue) * sizeof(TCHAR)) + sizeof(TCHAR)))
		result = FALSE;

	RegCloseKey(m_Key);
	return result;
}
