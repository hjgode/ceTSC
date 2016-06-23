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

#ifndef		__INCLUDED_PROJECTORINFO_HPP__
#define		__INCLUDED_PROJECTORINFO_HPP__


#ifndef __STRING__
#include	"string.hxx"
#endif // __STRING__

class	ProjectorInfo_t
{
public:
	static	const	int		s_MAX_FriendlyNameLength = 50;
	static	const	WCHAR*		s_pPictorRegistryRoot;
private:
	static	const	int		s_MAX_BufferLength = 50;
	static	const	WCHAR*			s_pPictorServiceName;
	static	const	WCHAR*			s_pProjectorURLPrefix;
	static	const	WCHAR*		s_pDeviceNameKey;
	
	static	WCHAR			s_ProjectorHostName[MAX_COMPUTERNAME_LENGTH+1];
	static	WCHAR			s_ReadBuffer[s_MAX_BufferLength];
	static	const	int		s_PasswordTimeGranularity = 60*1000; //minute
	static	const	int		s_SecondCountdownPasswordTimeGranularity = 10*1000; //10 sec
	
	static	UINT			s_SecondCountdownPasswordTimerId; 
	static	int				s_SecondCountdownPasswordTimeRemaining; //in 10 secs
	
	static	const	int		s_CheckPictorServiceTimeout=10*60*1000;
	static	int				s_PasswordTimeRemaining; //in minutes
	static	UINT			s_PasswordTimerId; 
	static	HANDLE			s_WorkerThreadHandle;
	static	HANDLE			s_StopEventHandle;
	static	bool				s_ServiceRefreshInProgress;
public:
	static
	bool
	Initialize(
		);

	static
	bool
	UnInitialize(
		);

	static
	ce::wstring
	FriendlyDeviceName(
		);

	static
	bool
	FriendlyDeviceName(
		ce::wstring	NewName
		);

	static
	ce::wstring
	UniqueDeviceName(
		);

	static
	bool
	Password(
		ce::wstring	NewPassword
		);

	static
	ce::wstring
	Password(
		);

	static
	ce::wstring
	PasswordRemainingTime(
		);

	static
	ce::wstring
	ProjectorURL(
		);
	
private:
	static
	bool
	ReadPictorValue(
		HANDLE		hPictorService,
		DWORD		IoControlCode
		);

	static
	bool
	WritePictorValue(
		HANDLE		hPictorService,
		DWORD		IoControlCode,
		const	WCHAR*		WriteBuffer,
		int			LengthInWchar
		);

	static
	void 
	CALLBACK 
	PasswordTimerRefreshProc(
		  HWND		hwnd, 
		  UINT		uMsg, 
		  UINT		idEvent, 
		  DWORD		dwTime 
		);

	static
	void 
	CALLBACK 
	SecondCountdownPasswordTimerRefreshProc(
		  HWND		hwnd, 
		  UINT		uMsg, 
		  UINT		idEvent, 
		  DWORD		dwTime 
		);


	static
	bool
	Initialize_Impl(
		);


	static
	DWORD
	WorkerThreadProc(
		void*
		);

	static
	void
	CheckForIPAddressChange(
		);

	static
	void
	CheckPictorServiceStatus(
		);

	static
	bool
	RefreshPictorService(
		);

	static
	void
	ReadProjectorHostName(
		);

	static
	ce::wstring
	ReadDeviceHostNameFromRegistry(
		);

	static
	ce::wstring
	ReadDeviceHostNameFromAPI(
		);
};


#endif //__INCLUDED_PROJECTORINFO_HPP__
