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
#include "RegDword.hpp"

CRegDword::CRegDword()
{
}

CRegDword::~CRegDword()
{
}

BOOL CRegDword::Get() 
{
	// Open the specified key
	if (RegOpenKeyEx(m_baseKey, m_keyName, 0, KEY_READ, &m_Key) !=
		ERROR_SUCCESS)
		return FALSE;

    DWORD type;
    DWORD size = sizeof (m_value);
	BOOL  result = TRUE;

    if (::RegQueryValueEx (
        m_Key,
        m_valueName,
        NULL,
        &type,
        (unsigned char *)&m_value,
        &size) != ERROR_SUCCESS)
        result = FALSE;

	// Close the key
	RegCloseKey(m_Key);

    return result;
}

BOOL CRegDword::Set()
{
	BOOL result = TRUE;

	// Open the specified key
	if (RegOpenKeyEx(m_baseKey, m_keyName, 0, KEY_ALL_ACCESS, &m_Key) !=
		ERROR_SUCCESS)
		return FALSE;

    if (RegSetValueEx(m_Key, m_valueName, NULL, REG_DWORD,
					  (unsigned char *)&m_value, sizeof(DWORD)) != 
		ERROR_SUCCESS)
        result = FALSE;

	// Close the key
	RegCloseKey(m_Key);

    return result;
}
