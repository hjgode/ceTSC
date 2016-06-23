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
#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#define		MAX_PROPNAME_LEN	256

#include "svsutil.h"

extern FixedMemDescr	*gpfmdPropSet;

#include "tscctrl.h"

struct Property
{
public:
	Property	*pNext;
	WCHAR		szName[MAX_PROPNAME_LEN];
	TscPropType	eType;
	DWORD		dwSize;
	
	union Value
	{
		DWORD	dwVal;
		PBYTE   pbVal;
	};

	Value		uVal;

public:
	Property()
	{
		memset(this, 0, sizeof(*this));
	}

	~Property()
	{
		if  ( ((eType == TscTypeSTRING) || (eType == TscTypeBINARY) ) && 
			 (uVal.pbVal) )
		{
			LocalFree(uVal.pbVal);
		}
	}

	void *operator new(size_t nSize)
	{
		ASSERT(gpfmdPropSet);
		ASSERT(nSize == sizeof(Property));
		return svsutil_GetFixed(gpfmdPropSet);
	}

	void operator delete(void *ptr)
	{
		ASSERT(ptr);
		ASSERT(gpfmdPropSet);
		svsutil_FreeFixed(ptr, gpfmdPropSet);
	}
};

class ISettings
{
public:
	virtual LONG Open(LPWSTR pszName, BOOL bForRead) = 0;
	virtual BOOL Commit() = 0;
	virtual BOOL Close() = 0;

	virtual BOOL Read(LPWSTR pszName, TscPropType *peType, PBYTE pbDefault, PBYTE pbVal, PDWORD pdwSize) = 0;
	virtual BOOL Write(LPWSTR pszName, TscPropType eType, PBYTE pbVal, DWORD dwSize) = 0;
	virtual BOOL Delete(LPWSTR pszName) = 0;

	virtual BOOL Copy(ISettings *pSrc, LPWSTR pszName, BOOL fOverwrite) = 0;	//copies all properties if pszName is NULL;

	virtual BOOL GetNumProperties() = 0;
	virtual BOOL GetPropertySet(Property **ppPrpt) = 0;
};


class CSettings:public ISettings
{
public:
	CSettings()	{	_pPropSet = NULL;	_dwNumProps = 0;	_szName[0] = L'\0';	};
	virtual ~CSettings() {	Close();	};

public:
	virtual LONG Open(LPWSTR pszName, BOOL bForRead);
	virtual BOOL Commit();
	virtual BOOL Close();

	virtual BOOL Read(LPWSTR pszName, TscPropType *peType, PBYTE pbDefault, PBYTE pbVal, PDWORD pdwSize);
	virtual BOOL Write(LPWSTR pszName, TscPropType eType, PBYTE pbVal, DWORD dwSize);
	virtual BOOL Delete(LPWSTR pszName);

	virtual BOOL Copy(ISettings *pSrc, LPWSTR pszName, BOOL fOverwrite);

	virtual BOOL GetNumProperties();
	virtual BOOL GetPropertySet(Property **ppPrpt);

protected:
	Property	*_pPropSet;
	WCHAR		_szName[MAX_PATH];
	DWORD		_dwNumProps;
};


class CRegSettings:public CSettings
{
public:
	CRegSettings()	{};
	virtual ~CRegSettings(){};

public:
	virtual LONG Open(LPWSTR pszName, BOOL bForRead);
	virtual BOOL Commit();

protected:
	TscPropType RegTypeToTscPropType(DWORD dwRegType)
	{
		if (dwRegType == REG_DWORD)
			return TscTypeULONG;
		else if (dwRegType == REG_SZ)
			return TscTypeSTRING;
		else if (dwRegType == REG_BINARY)
			return TscTypeBINARY;
		else
			return TscTypeUNKNOWN;
	}

	DWORD TscPropTypeToRegType(TscPropType eType)
	{
		if (eType <= TscTypeBOOL)
			return REG_DWORD;
		else if (eType == TscTypeSTRING)
			return REG_SZ;
		else if (eType == TscTypeBINARY)
			return REG_BINARY;
		else 
			return REG_NONE;
	}
};


class CFileSettings:public CSettings
{
public:
	CFileSettings()	{};
	virtual ~CFileSettings(){};

public:
	virtual LONG Open(LPWSTR pszName, BOOL bForRead);
	virtual BOOL Commit();

protected:
	LONG  GetProperty(HANDLE hFile, LPWSTR pszVal, LONG cchVal);
	BOOL  GetPropertyNameAndType(HANDLE hFile, LPWSTR pszPropName, LONG *pcchPropNameLen, WCHAR *pcType);
	BOOL  BinaryToString(PBYTE pbData, DWORD cbData, LPWSTR pszData, DWORD dwLen);
	BOOL  StringToBinary(LPWSTR pszData, PBYTE pbData, PDWORD pcbData);
};

#endif
