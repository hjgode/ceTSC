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
#include "stdafx.h"
#include "TsConWiz.hpp"


HINSTANCE ghInstance;

int WINAPI _tWinMain (HINSTANCE hInst, HINSTANCE hInstPrev, LPTSTR lpcmdLine, int nShow)
{
	DEBUGMSG (1, (L"Starting TsConWiz \n"));
	TsConnectionWizard connectionWizard;
	DEBUGMSG (1, (L"Constructed main object\n"));

	ghInstance = hInst;

	connectionWizard.Execute();

	return 0;
}
