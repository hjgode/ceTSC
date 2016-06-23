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
#ifndef REGSTRING_HPP
#define REGSTRING_HPP

static const int BUFFER_SIZE = 512;

class CRegString 
{
public:
	CRegString();
	~CRegString();

	HKEY m_baseKey;
	LPCTSTR m_keyName;
	LPCTSTR m_valueName;

	HKEY m_Key;

public:
	BOOL Set();
	BOOL Get();

public:
	TCHAR m_szValue[BUFFER_SIZE + 1];
};

#endif //REGSTRING_HPP