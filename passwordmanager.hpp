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
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//

#ifndef	__INCLUDED_PASSWORDMANAGER_HPP__
#define	__INCLUDED_PASSWORDMANAGER_HPP__

// Pictor registry information is stored at HKEY_LOCAL_MACHINE\Software\Wireless Projector
//
// 1. PasswordLength = 8(default) (Valid - 4 - 24)
// 2. PasswordChars = Numeric or Alphanumeric (0, 1)
// 3. AutomaticPasswordChange = 0 (No change), 1(Timer based change), 2(Policy Based Change)
// 4. PasswordTimer = 5min(default), Valid (1-1440)
// 5. PasswordPolicy = AfterSuccessfullSession, AfterUnsuccessfullSession
// 6. PasswordPolicySessionNums = 1(default) Valid( 1-20)

#include<windows.h>

#ifndef __STRING__
#include	<string.hxx>
#endif //__STRING__

class	ConfigCommand_t;

enum	AutoPasswordChangeEnum_t
{
	NoChange = 0,
	TimerBasedChange,
	PolicyBasedChange
};

enum	PasswordPolicyEnum_t
{
	SuccessBased = 0,
	FailureBased
};

enum	PasswordCharEnum_t
{
	Numeric = 0,
	AlphaNumeric
};


class	PasswordManager_t
{

public:
	static	const	int	s_PasswordLengthDefault = 8;
	static	const	int	s_PasswordLengthMin = 4;
	static	const	int	s_PasswordLengthMax = 24;

	static	const	int	s_PasswordCharNumeric = 0;
	static	const	int	s_PasswordCharAlphaNumeric = 1;

	static	const	int	s_AutoPasswordNoChange = 0;
	static	const	int	s_AutoPasswordTimerBased = 1;
	static	const	int	s_AutoPasswordPolicyBased = 2;

	static	const	int	s_PasswordTimerDefault = 5;
	static	const	int	s_PasswordTimerMin = 2;
	static	const	int	s_PasswordTimerMax = 1440;

	static	const	int	s_PasswordPolicySuccessBased = 0;
	static	const	int	s_PasswordPolicyFailureBased = 1;

	static	const	int	s_PasswordPolicyNumSessionDefault = 1;
	static	const	int	s_PasswordPolicyNumSessionMin = 1;
	static	const	int	s_PasswordPolicyNumSessionMax = 20;

private:
	static	int		s_PasswordLength;
	static	int		s_PasswordTimer;
	static	int		s_PasswordPolicyNumSessions;
	static	bool			s_IsInitialized;
	static	HKEY		s_RootKeyHandle;
	
	static	ConfigCommand_t*	s_pConfigCommand;
	static	PasswordCharEnum_t		s_PasswordChar;
	static	AutoPasswordChangeEnum_t		s_AutoPasswordChange;
	static	PasswordPolicyEnum_t		s_PasswordPolicy;
public:
	static
	bool
	Initialize(
		);

	static
	void
	UnInitialize(
		);


	static
	bool
	IsTimerBasedPassword(
		);

	static
	int
	PasswordTimer(
		);
	
private:
	static
	void
	ReadProjectorRegistryInfo(
		);

	static
	void
	ReadPasswordChar(
		);

	static	
	void
	ReadAutoPasswordChange(
		);

	static
	void
	ReadPasswordTimer(
		);

	static
	void
	ReadPasswordPolicy(
		);

	static
	void
	ReadPasswordLength(
		);

};


#endif //__INCLUDED_PASSWORDMANAGER_HPP__

