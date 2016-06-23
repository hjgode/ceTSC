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
#ifndef _MSTSC_H_
#define _MSTSC_H_

#include "settings.h"
#include "propnames.h"

#ifdef DEBUG
  #define ZONE_ERROR	DEBUGZONE(0)
  #define ZONE_MAIN		DEBUGZONE(1)
  #define ZONE_UI		DEBUGZONE(2)
  #define ZONE_SETTING	DEBUGZONE(3)
  #define ZONE_VERBOSE	DEBUGZONE(15)
#endif


#define TSC_AX_HOSTWND_CLS		L"TSSHELLWND"

#define PRINTER_APPLET_NAME		L"\\windows\\wbtprncpl.dll"
#define PEGHELP_EXE				L"\\windows\\peghelp.exe"
#define TSC_HELP_FILE			L"\\windows\\termservclient.htm"
#define TSC_HELP_FILE_TOPIC		L"\\windows\\TermServClient.htm#Main_Contents"

#define DEFAULT_RDP_PORT		0xD3D
#define DEFAULT_RDP_FILE		L"\\windows\\default.rdp"
#define DEFAULT_RDP_FILE_NAME	L"default.rdp"
#define DEFAULT_REG_SESSION		L"Default"

#define SAFERELEASE(x)			if (x) (x)->Release(); (x) = NULL;

#define INVALID_CHARS			L";<>*+=|? \t\""

#define RDRSUPPORT_SCARD		0x01
#define RDRSUPPORT_DRIVE		0x02
#define RDRSUPPORT_PORT			0x04
#define RDRSUPPORT_PRINTER		0x08
#define RDRSUPPORT_AUDIO		0x10
#define RDRSUPPORT_CLIPBD		0x20

#define AUDIOMODE_REDIRECT	0
#define AUDIOMODE_SERVER	1
#define AUDIOMODE_NONE		2

#define TSC_BASE				(WM_USER+1000)

#define MSG_TSC_CONNECT			(TSC_BASE + 1)
#define MSG_TSC_CONNECTED		(TSC_BASE + 2)
#define MSG_TSC_DISCONNECT		(TSC_BASE + 3)
#define MSG_TSC_DISCONNECTED	(TSC_BASE + 4)
#define MSG_TSC_CLOSE			(TSC_BASE + 5)
#define MSG_TSC_GETRDRSUPPORT	(TSC_BASE + 6)
#define MSG_TSC_ERROR			(TSC_BASE + 7)

#define MAX_ARC_ATTEMPTS		20
#define ARC_CONNECTION_DELAY	5000

#define TSC_SETTINGS_REG_ROOT	L"Software\\Microsoft\\Terminal Server Client\\"
#define DEVICES_ROOT			L"LocalDevices"

#define PROMPT_DEVICES_PORTS	0x1
#define PROMPT_DEVICES_DRIVES	0x2
#define PROMPT_DEVICES_MAX		0x3

#define MAX_SERVERNAMELEN		256
#define MAX_USERNAMELEN			512
#define MAX_PASSWORDLEN			512
#define MAX_DOMAINLEN			512

#define MAX_MRUCOUNT			10

enum State
{
	Disconnected,
	Disconnecting,
	Connecting,
	Connected,
	LoggedIn
};

enum Profile
{
	profileNone,
	profileServerName,
	profileRDPFile,
	profileRegSession
};


extern "C" void DeleteFixedupSettings(ISettings *pSettings);
extern "C" HWND StartTscUI(HWND hwndParent, HINSTANCE hInst, ISettings *pSettings);
extern "C" void CloseTscUI();

extern "C" BOOL IsClipbdRedirectorPresent();

#endif
