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
#ifndef _PROPNAMES_H_
#define _PROPNAMES_H_

//adv settings                  
#define PROP_CONNECTTOSERVERCONSOLE                     L"ConnectToServerConsole"
#define PROP_BBARENABLED                                L"BBarEnabled"
#define PROP_BBARPINNED                                 L"BBarPinned"
#define PROP_BBARSHOWMINIMIZEBTN                        L"BBarShowMinimizeBtn"
#define PROP_BBARSHOWRESTOREBTN                         L"BBarShowRestoreBtn"
#define PROP_BBARSHOWPINBTN                             L"BBarShowPinBtn"   
#define PROP_REDIRECTDRIVES                             L"EnableDriveRedirection"       
#define PROP_REDIRECTPRINTERS                           L"EnablePrinterRedirection"
#define PROP_REDIRECTPORTS                              L"EnablePortRedirection"     
#define PROP_REDIRECTSMARTCARDS                         L"EnableSCardRedirection"
#define PROP_REDIRECTCLIPBOARD                          L"EnableClipboardRedirection"
#define PROP_ENABLEAUTORECONNECT                        L"AutoReconnectEnabled"
#define PROP_CLEARTEXTPASSWORD                          L"Password"
#define PROP_AUTOLOGON                                  L"AutoLogon"
#define PROP_BITMAPPERSISTENCE                          L"BitmapPersistenceEnabled"
#define PROP_BITMAPPERSISTCACHELOCATION                 L"BitmapPersistCacheLocation"
#define PROP_BITMAPCACHESIZE                            L"BitmapCacheSize"
#define PROP_RDPPORT                                    L"MCSPort"
#define PROP_CONTAINERHANDLESFULLSCREEN                 L"ContainerHandlesFullScreen"
#define PROP_FULLSCREENALWAYS                           L"FullScreenAlways"
#define PROP_PLUGINDLLS                                 L"PluginDlls"
#define PROP_SMARTSIZING                                L"EnableSmartSizing"
#define PROP_KEYBOARDLAYOUT                             L"KeyboardLayoutString"
#define PROP_KEYBOARDTYPE                               L"KeyboardType"
#define PROP_KEYBOARDSUBTYPE                            L"KeyboardSubType"
#define PROP_KEYBOARDFUNCTIONKEY                        L"KeyboardFunctionKey"

#define PROP_COMPRESS                                   L"Compress"      
#define PROP_MAXRECONNECTATTEMPTS                       L"MaxReconnectAttempts"

#define PROP_PERFORMANCEFLAGS                           L"PerformanceFlags"
#define PROP_DISABLEWALLPAPER                           L"Disable Wallpaper"
#define PROP_DISABLEFULLWINDOWDRAG                      L"Disable Full Window Drag"
#define PROP_DISABLEMENUANIMS                           L"Disable Menu Anims"
#define PROP_DISABLETHEMES                              L"Disable Themes"

//securedsettings               
#define PROP_STARTFULLSCREEN                            L"StartFullScreen"
#define PROP_KEYBOARDHOOKMODE                           L"KeyboardHookMode"
#define PROP_AUDIOREDIRECTIONMODE                       L"AudioRedirectionMode"
#define PROP_STARTPROGRAM                               L"AlternateShell"
#define PROP_WORKDIR                                    L"WorkingDir"              


//Rdpclient 
#define PROP_COLORDEPTH                                 L"ColorDepthID"
#define PROP_SERVER                                     L"ServerName"         
#define PROP_DOMAIN                                     L"Domain"
#define PROP_USERNAME                                   L"UserName"
#define PROP_DESKTOPWIDTH                               L"DesktopWidth"
#define PROP_DESKTOPHEIGHT                              L"DesktopHeight"
#define PROP_EXTDISCONREASON                            L"ExtendedDisconnecteReason"

//TscDebug
#define PROP_CLXCMDLINE                                 L"CLXCmdLine"

#define PROP_MRU                                        L"MRU"
#define PROP_SAVEPASSWORD                               L"SavePassword"
#define PROP_DISABLEFILEACCESS                          L"DisableFileAccess"
#define PROP_PROMPTUSER                                 L"PromptUser"
#endif
