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
#ifndef _BASEUI_H_
#define _BASEUI_H_

#include "resid.h"

#define IDC_STATIC					-1

//ICONS AND BITMAPS
#define IDI_MSTSC									10101
#define IDI_DISCON									10102
#define IDI_ICO_WARN								10103

#define IDB_BAND8									10150
#define IDB_WINFLAG8								10151

//DIALOGS
#define IDD_CONNECTING								10201
#define IDD_ARCDLG									10202
#define IDD_RDRWARNDLG								10203

//CONTROLS
#define IDC_STC_CONNECTINGTO						10501
													
//#define IDC_STC_STATUS								10502
#define IDC_STC_STATUSDESC							10503
#define IDC_STC_DISCON								10504
#define IDC_STC_TITLEFLAG							10505
#define IDC_STC_TITLETEXT							10506
#define IDC_STC_DEVICES								10507

#define IDC_CHK_NOPROMPT							10525

//STRINGS
#define IDS_CONFIRMCLOSE_TITLE						11001
#define IDS_CONFIRMCLOSE_MSG						11002
													
#define IDS_FATALERROR_TITLE						11003
#define IDS_FATALERROR_MSG							11004
													
#define IDS_BMPCACHEFAIL_TITLE						11005
#define IDS_BMPCACHEFAIL_MSG						11006
													
#define IDS_CONNECTING_TO							11007
//#define IDS_ARC_STATUS								11008
#define IDS_PROMPTDRIVES							11009
#define IDS_PROMPTPORTS								11010
#define IDS_USAGE									11011

//ACCELERATOR
#define IDR_ACCELERATORS							12000

//"Disconnected..." strings
#define IDS_DISCONNECTED_CAPTION					15001
#define IDS_ERR_CONNECTCALLFAILED					15002
#define IDS_DISCONNECT_REMOTE_BY_SERVER				15003
#define IDS_LOW_MEMORY								15004
#define IDS_SECURITY_ERROR							15005
#define IDS_BAD_SERVER_NAME							15006
#define IDS_CONNECT_FAILED_PROTOCOL					15007
#define IDS_CANNOT_LOOPBACK_CONNECT					15008
#define IDS_NETWORK_ERROR							15009
#define IDS_INTERNAL_ERROR							15010
#define IDS_NOT_RESPONDING							15011
#define IDS_VERSION_MISMATCH						15012
#define IDS_ENCRYPTION_ERROR						15013
#define IDS_PROTOCOL_ERROR							15014
#define IDS_ILLEGAL_SERVER_NAME						15015
#define IDS_CONNECTION_TIMEOUT						15016
#define IDS_DISCONNECT_IDLE_TIMEOUT					15017
#define IDS_DISCONNECT_LOGON_TIMEOUT				15018
#define IDS_PROTOCOL_ERROR_WITH_CODE				15019
#define IDS_LICENSING_TIMEDOUT						15020
#define IDS_LICENSING_NEGOT_FAILED					15021
#define IDS_DISCONNECT_REMOTE_BY_SERVER_TOOL		15022
#define IDS_LOGOFF_REMOTE_BY_SERVER					15023
#define IDS_DISCONNECT_BYOTHERCONNECTION			15024
#define IDS_CONNECTION_BROKEN						15025
#define IDS_SERVER_OUT_OF_MEMORY					15026
#define IDS_LICENSE_INTERNAL						15027
#define IDS_LICENSE_NO_LICENSE_SERVER				15028
#define IDS_LICENSE_NO_LICENSE						15029
#define IDS_LICENSE_BAD_CLIENT_MSG					15039
#define IDS_LICENSE_HWID_DOESNT_MATCH_LICENSE		15040
#define IDS_LICENSE_BAD_CLIENT_LICENSE				15041
#define IDS_LICENSE_CANT_FINISH_PROTOCOL			15042
#define IDS_LICENSE_CLIENT_ENDED_PROTOCOL			15043
#define IDS_LICENSE_BAD_CLIENT_ENCRYPTION			15044		
#define IDS_LICENSE_CANT_UPGRADE_LICENSE			15045
#define IDS_LICENSE_NO_REMOTE_CONNECTIONS			15046
#define IDS_SERVER_DENIED_CONNECTION				15047
#define IDS_TITLE_CONNECTED_DEFAULT                 15048
#define IDS_TITLE_CONNECTED                         15049

#endif
