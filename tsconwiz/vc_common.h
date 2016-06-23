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
// common.h: project-wide common definitions
//
// LJG, 30-SEP-99

#ifndef __COMMON_H__
#define __COMMON_H__

#define DIM( _x )	( sizeof(_x) / sizeof( (_x)[0] ) )

#ifdef UNDER_CE
	// since it's undefined in CE
	#define _MAX_FNAME  256 /* max. length of file name component */
#endif

#endif // COMMON_H
