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
#ifndef _STDAFX_H_
#define _STDAFX_H_

#include <windows.h>

#include "Trace.hpp"
#include "Util.hpp"

#include <windowsx.h>
#include <commctrl.h>

#include "cwindow.h"

#ifdef UNDER_CE
	#include "resource.h"
	#include "common.h"
#else
	#include "vc_resource.h"
	#include "vc_common.h"
#endif

#include "propnames.h"

extern HINSTANCE ghInstance;

#endif // _STDAFX_H_