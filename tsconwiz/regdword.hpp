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
#ifndef REGDWORD_HPP
#define REGDWORD_HPP

class CRegDword  
{
public:
	CRegDword();
	~CRegDword();

	HKEY m_baseKey;
	LPCTSTR m_keyName;
	LPCTSTR m_valueName;

	HKEY m_Key;

public:
	BOOL Set();
	BOOL Get();

public:
	DWORD m_value;
};

#endif // REGDWORD_HPP
