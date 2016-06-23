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
#include <windows.h>
#include <stdio.h>
#include "svsutil.h"
#include "mstsc.h"
#include "settings.h"

FixedMemDescr	*gpfmdPropSet = NULL;

LONG CSettings::Open(LPWSTR pszName, BOOL bForRead)
{
	HRESULT hr = StringCchCopy(_szName, MAX_PATH, pszName); 
	return (LONG)hr;
}

BOOL CSettings::Commit()
{
	return TRUE;
}

BOOL CSettings::Close()
{
	while(_pPropSet)
	{
		Property *pProp = _pPropSet->pNext;
		delete (_pPropSet);
		_pPropSet = pProp;
	}
	return TRUE;
}

BOOL CSettings::Read(LPWSTR pszName, TscPropType *peType, PBYTE pbDefault, PBYTE pbVal, PDWORD pdwSize)
{
	ASSERT(peType && pdwSize);

	Property *pProp = _pPropSet;
	while ( (pProp) && (0 != _wcsicmp(pszName, pProp->szName)) )
	{
		pProp = pProp->pNext;
	}

	if (pProp)
	{
		DWORD dwOrigSize = *pdwSize;
		*pdwSize = pProp->dwSize;
		
		*peType = pProp->eType;

		if (pProp->dwSize > dwOrigSize)
		{
			DEBUGMSG(ZONE_SETTING, (L"Value of property '%s'(%d) bigger than supplied buffer (%d)\n", pszName, pProp->dwSize, dwOrigSize));
			return (pbVal == NULL);		//return TRUE if the caller is asking for the size of the value
		}

		if (pProp->eType <= TscTypeBOOL)
			*(PDWORD )pbVal = pProp->uVal.dwVal;
		else
			memcpy(pbVal, pProp->uVal.pbVal, pProp->dwSize);
		return TRUE;
	}
	else if (pbDefault)
	{
		DEBUGMSG(ZONE_SETTING, (L"Property '%s'not found. Returning default value\n", pszName));
		memcpy(pbVal, pbDefault, *pdwSize);
	}
	return FALSE;
}


BOOL CSettings::Write(LPWSTR pszName, TscPropType eType, PBYTE pbVal, DWORD dwSize)
{
	ASSERT(pbVal);

	if ((pszName == NULL) || (*pszName == L'\0'))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	Property *pProp = _pPropSet;
	while ( (pProp) && (0 != _wcsicmp(pszName, pProp->szName)) )
	{
		pProp = pProp->pNext;
	}

	if (pProp)
	{
		if (pProp->eType > TscTypeBOOL)
		{
			PBYTE pbNewVal = (PBYTE )LocalReAlloc(pProp->uVal.pbVal, dwSize, LMEM_MOVEABLE);
			if (!pbNewVal)
			{
				DEBUGMSG(ZONE_ERROR, (L"Out of memory trying to write property : %s\n", pszName));
				return FALSE;
			}

			pProp->uVal.pbVal = pbNewVal;
		}
	}
	else 
	{
		BOOL bRet = TRUE;

		if ((pProp = new Property()) == NULL)
		{
			DEBUGMSG(ZONE_ERROR, (L"Out of memory trying to write property : %s\n", pszName));
			return FALSE;
		}
		
		bRet = (S_OK == StringCchCopy(pProp->szName, MAX_PROPNAME_LEN, pszName));


		if (eType > TscTypeBOOL)
		{
			pProp->uVal.pbVal = (PBYTE )LocalAlloc(0, dwSize);
			if (!pProp->uVal.pbVal)
				bRet = FALSE;
		}
		else if (dwSize != sizeof(DWORD))
		{
			DEBUGMSG(ZONE_ERROR, (L"Invalid size for property %s\n", pszName));
			bRet = FALSE;
		}

		if (!bRet)
		{
			delete pProp;
			return FALSE;
		}

		pProp->pNext = _pPropSet;
		_pPropSet = pProp;

	}

	pProp->eType  = eType;
	pProp->dwSize = dwSize;
	if (eType <= TscTypeBOOL)
		pProp->uVal.dwVal = *(PDWORD )pbVal;
	else
		memcpy(pProp->uVal.pbVal, pbVal, dwSize);

	return TRUE;
}

BOOL CSettings::Delete(LPWSTR pszName)
{
	Property *ppt = _pPropSet;
	Property *pptPrev = NULL;
	while ((ppt) && (0 != wcsicmp(pszName, ppt->szName)) )
	{
		pptPrev = ppt;
		ppt = ppt->pNext;
	}

	if (!ppt)
		return FALSE;

	if (pptPrev)
		pptPrev->pNext = ppt->pNext;
	else
		_pPropSet = ppt->pNext;

	delete ppt;
	return TRUE;
}

BOOL CSettings::Copy(ISettings *pSrc, LPWSTR pszName, BOOL fOverwrite)
{
	ASSERT(pSrc);
	Property *pProp = NULL;
	if (!pSrc->GetPropertySet(&pProp))
		return TRUE;

	if (pszName)
	{
		while( (pProp) && (0 != wcsicmp(pProp->szName, pszName)) )
			pProp = pProp->pNext;
	}

	while(pProp)
	{
		BOOL bNew = FALSE;
		Property *pSelf = _pPropSet;
		while ((pSelf) && (0 != wcsicmp(pSelf->szName, pProp->szName)) )
			pSelf = pSelf->pNext;

		if (pSelf && !fOverwrite)
		{
			pProp = pProp->pNext;
			continue;
		}

		if (!pSelf)
		{
			if ((pSelf = new Property()) == NULL)
				return FALSE;
			bNew = TRUE;
		}

		if ((pSelf->eType == TscTypeBINARY) || (pSelf->eType == TscTypeSTRING))
		{
			ASSERT(pSelf->uVal.pbVal);
			LocalFree(pSelf->uVal.pbVal);
		}

		if ((pProp->eType == TscTypeBINARY) || (pProp->eType == TscTypeSTRING))
		{
			ASSERT(pProp->dwSize > 0);

			pSelf->uVal.pbVal = (PBYTE )LocalAlloc(0, pProp->dwSize);
			if (!pSelf->uVal.pbVal)
			{
				delete pSelf;
				return FALSE;
			}
		}

		if (pProp->eType <= TscTypeBOOL)
			pSelf->uVal.dwVal = pProp->uVal.dwVal;
		else
			memcpy(pSelf->uVal.pbVal, pProp->uVal.pbVal, pProp->dwSize);

		pSelf->eType = pProp->eType;
		pSelf->dwSize = pProp->dwSize;

		if (S_OK != StringCchCopy(pSelf->szName, MAX_PROPNAME_LEN, pProp->szName) )
		{
			delete pSelf;
			return FALSE;
		}

		if (bNew)
		{
			pSelf->pNext = _pPropSet;
			_pPropSet = pSelf;
		}


		if (!pszName)
		{
			pProp = pProp->pNext;
		}
		else
		{
			break;
		}

	}
	return TRUE;
}


BOOL CSettings::GetNumProperties()
{
	return _dwNumProps;
}

BOOL CSettings::GetPropertySet(Property **ppPrpt)
{
	ASSERT(ppPrpt);
	*ppPrpt = _pPropSet;
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////										REGISTRY SETTINGS							/////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

LONG CRegSettings::Open(LPWSTR pszName, BOOL bForRead)
{
	HRESULT hr = StringCchCopy(_szName, MAX_PATH, TSC_SETTINGS_REG_ROOT); 
	if (S_OK != hr)
		return (LONG)hr;

	if (pszName && *pszName)
	{
		hr = StringCchCat(_szName, MAX_PATH, pszName);
		if (S_OK != hr)
			return (LONG)hr;
	}

	if (!bForRead)
		return TRUE;

	HKEY hkey;
	LONG lRet = RegOpenKeyEx(HKEY_CURRENT_USER, _szName, 0, 0, &hkey);
	if (ERROR_SUCCESS != lRet)
		return ERROR_FILE_NOT_FOUND;

	Property *pPrpt = NULL;

	BOOL bRet = FALSE;
	WCHAR szValName[MAX_PROPNAME_LEN];
	DWORD dwType, cchName = MAX_PROPNAME_LEN, dwSize, dwIndex = 0;
	while (ERROR_SUCCESS == RegEnumValue(hkey, dwIndex++, szValName, &cchName, NULL, &dwType, NULL, NULL) )
	{
		cchName = MAX_PROPNAME_LEN;
		PBYTE pbVal = NULL;

		//RegEnumValue doesnt return the size of the value. So call RegQueryValue here
		if (ERROR_SUCCESS != (lRet = RegQueryValueEx(hkey, szValName, NULL, &dwType, NULL, &dwSize)) )
		{
			DEBUGMSG(ZONE_SETTING, (L"RegQueryValueEx on %s failed\n", szValName));
			goto Cleanup;
		}

		pPrpt = new Property();
		if (!pPrpt)
		{
			DEBUGMSG(ZONE_ERROR, (L"Out of memory\n"));
			lRet = ERROR_OUTOFMEMORY;
			goto Cleanup;
		}

		if ((dwType == REG_SZ) || (dwType == REG_BINARY))
		{
			pbVal = pPrpt->uVal.pbVal = (PBYTE )LocalAlloc(0, dwSize);
		}
		else if (dwType == REG_DWORD)
		{
			ASSERT(dwSize == sizeof(DWORD));
			pbVal = (PBYTE )&(pPrpt->uVal.dwVal);
		}

		if (pbVal == NULL)
		{
			delete pPrpt;
			pPrpt = NULL;
			continue;
		}

		if (ERROR_SUCCESS != (lRet = RegQueryValueEx(hkey, szValName, NULL, &dwType, pbVal, &dwSize)))
		{
			DEBUGMSG(ZONE_ERROR, (L"RegQueryValueEx on %s failed\n", szValName));
			goto Cleanup;
		}
		
		if (S_OK != (hr = StringCchCopy(pPrpt->szName, MAX_PROPNAME_LEN, szValName) ) )
		{
			lRet = (LONG)hr;
			goto Cleanup;
		}

		pPrpt->dwSize = dwSize;
		pPrpt->eType = RegTypeToTscPropType(dwType);

		//Delete, if the property already exists
		Delete(szValName);

		pPrpt->pNext = _pPropSet;
		_pPropSet = pPrpt;

		pPrpt = NULL;

		_dwNumProps++;
	}
	bRet = ERROR_SUCCESS;

Cleanup:
	if (pPrpt)
		delete pPrpt;
	RegCloseKey(hkey);
	return lRet;
}


BOOL CRegSettings::Commit()
{
	if (_szName[0] == L'\0')
	{
		if (S_OK != StringCchCopy(_szName, MAX_PATH, TSC_SETTINGS_REG_ROOT))
			return FALSE;
	}

	HKEY hkey;
	DWORD dwDisp;
	if (ERROR_SUCCESS != RegCreateKeyEx(HKEY_CURRENT_USER, _szName, 0, NULL, 0, 0, NULL, &hkey, &dwDisp))
		return FALSE;

	Property *pProp = _pPropSet;
	BOOL bRet = FALSE;
	while(pProp)
	{
		PBYTE pbData = (pProp->eType <= TscTypeBOOL) ? (PBYTE )&(pProp->uVal.dwVal) : pProp->uVal.pbVal;
		ASSERT(pbData);

		if (ERROR_SUCCESS != RegSetValueEx(hkey, pProp->szName, NULL, TscPropTypeToRegType(pProp->eType), pbData, pProp->dwSize))
		{
			DEBUGMSG(ZONE_ERROR, (L"RegSetValueEx failed for value=%s\n", pProp->szName));
			goto Cleanup;
		}
		pProp = pProp->pNext;
	}

	bRet = TRUE;
Cleanup:
	RegCloseKey(hkey);
	return bRet;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////										FILE SETTINGS								/////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

#define TYPE_INTEGER	L'i'
#define TYPE_STRING		L's'
#define TYPE_BINARY		L'b'

#define BINARY_TO_STRING_LENGTH(binLen) ((2 * (binLen)) + 3)	//The +3 is for two trailing zeros and NULL character. The desktop does it this way

LONG CFileSettings::GetProperty(HANDLE hFile, LPWSTR pszVal, LONG cchVal)
{
	WCHAR szStr[MAX_PATH+1];
	LONG cchTotalLen = 0;
	BOOL fOnlyCR = FALSE;
	WCHAR *pszCR = NULL;
	WCHAR *pszStart = szStr;
	WCHAR *pszCur = pszVal;
	DWORD cch;
	
	DWORD dwStart = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
	if (dwStart == 0xffffffff)
		return -1;

	do
	{
		DWORD dwRead;
		
		if ((!ReadFile(hFile, pszStart, MAX_PATH*sizeof(WCHAR), &dwRead, NULL)) || (dwRead % 2) )
			return -1;

		if (dwRead == 0)
			break;


		cch = dwRead/sizeof(WCHAR);
		pszStart[cch] = L'\0';
		
		pszCR = wcschr(szStr, L'\r');
		if (pszCR)
		{
			if (pszCR + 1 < pszStart + cch)	//if there is a character beyond the carriage return
			{
				if (pszCR[1] != '\n')		//		and if it is not not a new line character, the file is bad
					return -1;
				cch = pszCR - szStr;
			}
			else							//else mark fOnlyCR to true so we can copy the carriage return to the beginning of the buffer 
			{								//so we can check for successive \r\n after the next read file
				fOnlyCR = TRUE;
				cch--;						//and decrement number of characters read by one
			}
		}

		cchTotalLen += cch;
		
		if ((pszVal) && (cchTotalLen < cchVal))
		{
			memcpy(pszCur, szStr, cch*sizeof(WCHAR));
			pszCur += cch;
		}

		if (fOnlyCR)
		{
			szStr[0] = '\r';			//carriage return copied to start of buffer here.
			pszStart = szStr + 1;
			pszCR = NULL;
			fOnlyCR = FALSE;
		}

	} while (pszCR == NULL);

	DWORD dwNew = ((!pszVal) || (cchVal < cchTotalLen)) ? dwStart : dwStart + (cchTotalLen*sizeof(WCHAR));

	if (cchTotalLen)
	{
		if ((pszVal) && (cchTotalLen < cchVal))
		{
			pszVal[cchTotalLen] = L'\0';
		}
		cchTotalLen++;
	}


	SetFilePointer(hFile, dwNew, NULL, FILE_BEGIN);

	return cchTotalLen;
}


BOOL CFileSettings::GetPropertyNameAndType(HANDLE hFile, LPWSTR pszPropName, LONG *pcchPropNameLen, WCHAR *pcType)
{
	ASSERT(pszPropName && pcchPropNameLen && *pcchPropNameLen && pcType);

	WCHAR szProp[MAX_PROPNAME_LEN + 16];
	WCHAR *pszColon = NULL;
	DWORD dwRead;
	DWORD dwCur = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);

	BOOL bRet = FALSE;
	if (!ReadFile(hFile, szProp, sizeof(szProp), &dwRead, NULL))
		goto Cleanup;

	if (dwRead == 0)	//end of file
	{
		*pcchPropNameLen = 0;
		return TRUE;
	}

	pszColon = wcschr(szProp, L':');
	if ((!pszColon) || ((pszColon - szProp) >= *pcchPropNameLen) )
		goto Cleanup;

	dwRead = pszColon - szProp;

	*pszColon++ = L'\0';
	
	*pcType = *pszColon++;
	
	if (*pszColon++ != L':')
		goto Cleanup;

	if (S_OK != StringCchCopy(pszPropName, *pcchPropNameLen, szProp))
		goto Cleanup;

	*pcchPropNameLen = dwRead;

	dwCur += (pszColon - szProp)*sizeof(WCHAR);
	bRet = TRUE;

Cleanup:
	SetFilePointer(hFile, dwCur, NULL, FILE_BEGIN);
	return bRet;
}


LONG CFileSettings::Open(LPWSTR pszName, BOOL bForRead)
{
	Property *pPrpt = NULL;

	HRESULT hr;
	if (S_OK != (hr = StringCchCopy(_szName, MAX_PATH, pszName)))
		return (LONG)hr;

	if (!bForRead)
		return ERROR_SUCCESS;

	HANDLE hFile = CreateFile(_szName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DEBUGMSG(ZONE_ERROR, (L"Failed to open settings file %s\n", _szName));
		return GetLastError();
	}

	LONG lRet = ERROR_INTERNAL_ERROR;
	WORD dwUnicodeSig;
	LONG lSize;
	if ( (!ReadFile(hFile, &dwUnicodeSig, sizeof(WORD), (PDWORD)(&lSize), NULL)) || 
		 (lSize != sizeof(WORD))  ||
		 (dwUnicodeSig != 0xFEFF) )
	{
		DEBUGMSG(ZONE_ERROR, (L"Unicode file signature not found. Invalid file %s\n", _szName));
		lRet = ERROR_INVALID_DATA;
		goto Cleanup;
	}

	while(TRUE)
	{
		WCHAR szPropName[MAX_PROPNAME_LEN];
		WCHAR szDword[16];
		WCHAR cType;

		LPWSTR pszVal = NULL;
		WCHAR szNL[] = {L'\r', L'\n'};


		lSize = MAX_PROPNAME_LEN;
		if (!GetPropertyNameAndType(hFile, szPropName, &lSize, &cType))
		{
			DEBUGMSG(ZONE_ERROR, (L"GetPropertyNameAndType failed.(%s)\n", _szName));
			goto Cleanup;
		}

		if (lSize == 0)	//end of file reached.
		{
			break;
		}

		cType = tolower(cType);
		lSize = GetProperty(hFile, NULL, 0);
		if (lSize < 0)
		{
			DEBUGMSG(ZONE_ERROR, (L"Invalid entry for property '%s' in file %s\n", szPropName, _szName));
			goto Cleanup;
		}
		else if (lSize == 0)
		{
			LONG lCur = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
			if (lCur == 0xffffffff)
				goto Cleanup;
			
			lCur = SetFilePointer(hFile, lCur + sizeof(szNL), NULL, FILE_BEGIN);
			if (lCur == 0xffffffff)
				goto Cleanup;

			continue;
		}

		pPrpt = new Property();
		if (!pPrpt)
		{
			DEBUGMSG(ZONE_ERROR, (L"Out of memory\n"));
			goto Cleanup;
		}

		if (cType == TYPE_INTEGER)
		{
			if (lSize >= sizeof(szDword)/sizeof(WCHAR))
			{
				DEBUGMSG(ZONE_ERROR, (L"Invalid integer value\n"));
				goto Cleanup;
			}

			pszVal = szDword;
		}
		else
		{
			pPrpt->dwSize = lSize*sizeof(WCHAR);
			pPrpt->uVal.pbVal =  (lSize) ? (PBYTE )LocalAlloc(0, lSize*sizeof(WCHAR)) : NULL;
			pszVal = (LPWSTR )pPrpt->uVal.pbVal;
		}

		if (pszVal == NULL)
		{
			delete pPrpt;
			pPrpt = NULL;
			continue;
		}

		switch(towlower(cType))
		{
		case TYPE_INTEGER:
			pPrpt->eType = TscTypeULONG;
			break;
		case TYPE_STRING:
			pPrpt->eType = TscTypeSTRING;
			break;
		case TYPE_BINARY:
			pPrpt->eType = TscTypeBINARY;
			break;
		default:
			ASSERT(FALSE);
			goto Cleanup;
		}	

		LONG lSize1 = GetProperty(hFile, (LPWSTR)pszVal, lSize);

		ASSERT(lSize1 == lSize);

		
		//convert string data if necessary
		if (pPrpt->eType == TscTypeULONG)
		{
			DWORD dwVal = _wtoi(pszVal);
			pPrpt->uVal.dwVal = dwVal;
			pPrpt->dwSize = sizeof(DWORD);
		}
		else if (pPrpt->eType == TscTypeBINARY)
		{
			PBYTE pbVal = NULL;
			DWORD dwSize = pPrpt->dwSize - sizeof(WCHAR);	//we dont want the trailing NULL for the string if we are going to convert it to binary

			if (dwSize % 2)
			{
				lRet = ERROR_INVALID_DATA;
				goto Cleanup;
			}

			pbVal = (PBYTE )LocalAlloc(0, dwSize);
			if (!pbVal)
				goto Cleanup;

			if (!StringToBinary((LPWSTR )pPrpt->uVal.pbVal, pbVal, &dwSize))
			{
				lRet = ERROR_INVALID_DATA;
				LocalFree(pbVal);
				goto Cleanup;
			}

			LocalFree(pPrpt->uVal.pbVal);
			pPrpt->uVal.pbVal = pbVal;
			pPrpt->dwSize = dwSize;
		}

		if (S_OK != (hr = StringCchCopy(pPrpt->szName, MAX_PROPNAME_LEN, szPropName)))
		{
			lRet = (LONG)hr;
			goto Cleanup;
		}

		//Delete, if the property already exists
		Delete(szPropName);

		pPrpt->pNext = _pPropSet;
		_pPropSet = pPrpt;

		pPrpt = NULL;

		_dwNumProps++;

		//skip over the \r\n
		if ( (!ReadFile(hFile, szDword, sizeof(szNL), (PDWORD)(&lSize), NULL)) || 
			 (szDword[0] != L'\r') ||
			 (szDword[1] != L'\n') )
		{
			DEBUGMSG(ZONE_ERROR, (L"Invalid rdp file\n"));
			goto Cleanup;
		}
	}
	lRet = ERROR_SUCCESS;

Cleanup:
	if (pPrpt)
		delete(pPrpt);
	
	CloseHandle(hFile);

	return lRet;
}


BOOL CFileSettings::Commit()
{
	HANDLE hFile = CreateFile(_szName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD dwSize, dwWrit;
	WORD dwUnicodeSig = 0xfeff;
	WCHAR szProp[MAX_PROPNAME_LEN + 16];
	WCHAR szDword[16];

	BOOL bRet = FALSE;
	BOOL fBinary = FALSE;
	LPWSTR pszVal = NULL;

	if ((!WriteFile(hFile, &dwUnicodeSig, sizeof(WORD), &dwWrit, NULL)) || (sizeof(WORD) != dwWrit) )
	{
		DEBUGMSG(ZONE_ERROR, (L"WriteFile failed. GLE=0x%x\n", GetLastError()));
		goto Cleanup;
	}


	Property *pProp;
	pProp = _pPropSet;
	while(pProp)
	{
		fBinary = FALSE;
		WCHAR c = TYPE_INTEGER;
		switch(pProp->eType)
		{
		case TscTypeULONG:
		case TscTypeBOOL:
			_itow(pProp->uVal.dwVal, szDword, 10);
			pszVal = szDword;
			c = TYPE_INTEGER;
			break;

		case TscTypeSTRING:
			pszVal = (LPWSTR )pProp->uVal.pbVal;
			c = TYPE_STRING;
			break;

		case TscTypeBINARY:
		{
			fBinary = TRUE;
			int nSize = ((2*pProp->dwSize) + 1) * sizeof(WCHAR);
			pszVal = (LPWSTR )LocalAlloc(0, nSize);
			if (!pszVal)
			{
				DEBUGMSG(ZONE_ERROR, (L"Out of memory\n"));
				goto Cleanup;
			}
			
			//Convert binary data to string
			if (!BinaryToString(pProp->uVal.pbVal, pProp->dwSize, pszVal, nSize))
			{
				DEBUGMSG(ZONE_ERROR, (L"Failed to convert binary data to string\n"));
				goto Cleanup;
			}
			c = TYPE_BINARY;
			break;
		}
		default:
			ASSERT (FALSE);
			goto Cleanup;
		}

		if (S_OK != StringCchPrintf(szProp, MAX_PROPNAME_LEN + 16, L"%s:%c:", pProp->szName, c))
			goto Cleanup;
		
		dwSize = wcslen(szProp) * sizeof(WCHAR);

		if ((!WriteFile(hFile, szProp, dwSize, &dwWrit, NULL)) || (dwWrit != dwSize) )
			goto Cleanup;

		
		dwSize = wcslen(pszVal) * sizeof(WCHAR);

		ASSERT(pszVal);
		if ((!WriteFile(hFile, pszVal, dwSize, &dwWrit, NULL)) || (dwWrit != dwSize) )
			goto Cleanup;


		WCHAR szNL[] = {L'\r', L'\n'};
		if ((!WriteFile(hFile, szNL, sizeof(szNL), &dwWrit, NULL)) || (dwWrit != sizeof(szNL)) )
			goto Cleanup;

		pszVal = NULL;

		pProp = pProp->pNext;

	}

	bRet = TRUE;
Cleanup:
	if ((fBinary) && (pszVal))
		LocalFree(pszVal);
	SetEndOfFile(hFile);
	CloseHandle(hFile);
	return bRet;
}

BOOL CFileSettings::BinaryToString(PBYTE pbData, DWORD cbData, LPWSTR pszData, DWORD dwLen)
{
	ASSERT(pbData && cbData && pszData && dwLen);

	if (dwLen < (cbData*2) + sizeof(WCHAR))	//for the trailing NULL
		return FALSE;

	LPWSTR pszCur = pszData;
	WCHAR  digits[] = L"0123456789ABCDEF";
	for (DWORD i=0; i<cbData; i++)
	{
		*pszCur++ = digits[(pbData[i] >> 4) & 0x0F];
		*pszCur++ = digits[pbData[i] & 0x0F];
	}
	*pszCur++ = L'\0';

	ASSERT(wcslen(pszData) == (2 * cbData));
	return TRUE;
}

BOOL CFileSettings::StringToBinary(LPWSTR pszData, PBYTE pbData, PDWORD pcbData)
{
	ASSERT(pszData && pbData && pcbData);

	int nLen = wcslen(pszData);

	if (*pcbData < (DWORD)(nLen/2) )
		return FALSE;

	LPWSTR pszCur = pszData;
	int j;
	for (j=0; j<nLen/2; j++)
	{
		BYTE b = 0;
		for (int i=0; i<2; i++)
		{
			if ((*pszCur >= L'0') && (*pszCur <= L'9'))
				b = (b << 4) | (*pszCur - L'0');
			else if ((*pszCur >= L'A') && (*pszCur <= L'F'))
				b = (b << 4) | (*pszCur - L'A') + 10;
			else
				return FALSE;
	
			pszCur++;
		}

		pbData[j] = b;
	}

	*pcbData = j;

	return TRUE;
}
