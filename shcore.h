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

#pragma once

#include <windows.h>

#define GCT_INVALID             0x0000
#define GCT_LFNCHAR             0x0001
#define GCT_SHORTCHAR           0x0002
#define GCT_WILD                0x0004
#define GCT_SEPERATOR           0x0008

STDAPI_(void) PathCompactSlashes(LPTSTR lpszPath);
STDAPI_(LPCTSTR) PathFindRootDevice(LPTSTR szPath);
STDAPI_(BOOL) PathGetAssociation(LPCTSTR pszPath, LPTSTR pszAssoc);
STDAPI_(BOOL) PathIsDatabase(LPCTSTR pszPath);
STDAPI_(BOOL) PathIsExe(LPCTSTR szFile);
STDAPI_(BOOL) PathIsExtension(LPCTSTR szFile, LPCTSTR szExt);
STDAPI PathIsGUID(LPCTSTR pszPath);
STDAPI_(BOOL) PathIsLink(LPCTSTR szFile);
STDAPI_(BOOL) PathIsRemovableDevice(LPCTSTR lpszPath);
STDAPI_(BOOL) PathIsSameDevice(LPCTSTR sz1, LPCTSTR sz2);
STDAPI_(BOOL) PathIsValidFileName(LPCTSTR pPath);
STDAPI_(BOOL) PathIsValidPath(LPCTSTR pPath);
STDAPI_(BOOL) PathMakeUniqueName(LPCTSTR lpszPath, LPCTSTR lpszT1, LPCTSTR lpszT2, 
                                 BOOL bLink, LPTSTR  lpszUniqueName, UINT cchUniqueName);
STDAPI_(void) PathRemoveQuotesAndArgs(LPTSTR pszPath);
STDAPI_(void) PathRemoveQuotes(LPTSTR pszPath);
STDAPI_(void) PathRemoveTrailingSlashes(LPTSTR lpszString);
