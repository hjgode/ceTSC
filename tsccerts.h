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
#ifndef _TSCCERTS_H_
#define _TSCCERTS_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef DWORD LS_STATUS;

#define LSSTAT_SUCCESS					0x00
#define	LSSTAT_ERROR					0x01
#define LSSTAT_INSUFFICIENT_BUFFER		0x02
#define LSSTAT_LICENSE_NOT_FOUND		0x03
#define LSSTAT_OUT_OF_MEMORY			0x04
#define LSSTAT_INVALID_HANDLE			0x05
#define LSSTAT_LICENSE_EXISTS			0x06


typedef DWORD   LICENSE_STATUS;

#define LICENSE_STATUS_OK				0x0

//Adding or replacing flags, to be used in LSAddLicenseToStore
#define		LS_REPLACE_LICENSE_OK	0x00000001
#define		LS_REPLACE_LICENSE_ERR	0x00000000


//This is the License Store index structure. Licenses are queried against this index
typedef struct	tagLSINDEX
{
	DWORD		dwVersion;	//Uper two bytes major version and lower two bytes Minor version
	DWORD		cbScope;
	BYTE	FAR *pbScope;	//Scope for the license
	DWORD		cbCompany;
	BYTE	FAR *pbCompany;	//Manufacturer
	DWORD		cbProductID;
	BYTE	FAR *pbProductID;//Product ID of the product for which the License is intended to be
}LSINDEX, FAR * PLSINDEX;

//Open a specified store. If the szStoreName is NULL, it will open default store
//Otherwise it will open the store specified by szStoreName parameter

LS_STATUS
LSOpenLicenseStore(
				 OUT HANDLE			*hStore,	 //The handle of the store
				 IN  LPCTSTR		szStoreName, //Optional store Name
				 IN  BOOL 			fReadOnly    //whether to open read-only
				 );

//Closes an open store
LS_STATUS
LSCloseLicenseStore(
				  IN HANDLE		hStore	//Handle of the store to be closed!
				  );

//Add or updates/replaces license against a given LSINDEX in an open store 
//pointed by hStore
LS_STATUS
LSAddLicenseToStore(
					IN HANDLE		hStore,	//Handle of a open store
					IN DWORD		dwFlags,//Flags either add or replace
					IN PLSINDEX		plsiName,	//Index against which License is added 
					__in_bcount(cbLicenseInfo) IN BYTE	 FAR   *pbLicenseInfo,	//License info to be added
					IN DWORD		cbLicenseInfo	// size of the License info blob
					);

//Deletes a license from the store refered by hStore and against the given LSINDEX
LS_STATUS
LSDeleteLicenseFromStore(
						 IN HANDLE		hStore,	//Handle of a open store
						 IN PLSINDEX	plsiName	//Index of the license to be deleted
						 );

//Finds a license in an open store against a particular store Index
LS_STATUS
LSFindLicenseInStore(
					 IN HANDLE		hStore,	//Handle of a open store
					 IN	PLSINDEX	plsiName,	//LSIndex against which store is searched
					 IN OUT	DWORD	FAR *pdwLicenseInfoLen,	//Size of the license found
					 __out_bcount_opt(*pdwLicenseInfoLen) OUT	BYTE	FAR *pbLicenseInfo	//License Data
					 );

LS_STATUS
LSEnumLicenses(
			   IN HANDLE		hStore,	//Handle of a open store
			   IN	DWORD		dwIndex, //numeric Index of the license to query
			   OUT	PLSINDEX	plsindex //The LSIndex structure corresponding to dwIndex
			   );

LS_STATUS
LSQueryInfoLicense(
				   IN HANDLE		hStore,	//Handle of a open store
				   OUT	DWORD	FAR *pdwLicenses, //Total no. of licenses available
				   OUT	DWORD	FAR *pdwMaxCompanyNameLen,	//Maximum length of the company length
				   OUT	DWORD	FAR *pdwMaxScopeLen,	//Maximum length of the company length
				   OUT	DWORD	FAR *pdwMaxProductIdLen	//Maximum length of the company length
				   );


LS_STATUS	
LSOpenLicenseHandle(
				   IN HANDLE		hStore,	//Handle of a open store
				   IN  BOOL         fReadOnly,
				   IN  PLSINDEX		plsiName,
				   OUT HANDLE		*phStore	//Handle of a open store
				   );
LS_STATUS
LSCloseLicenseHandle(
					 IN HANDLE		hStore,	//Handle of a open store
					 IN DWORD	dwFlags		//For future Use
					 );





//License related structures
#define LICENSED_VERSION_TEMPORARY  0x80000000
#define LICENSED_VERSION_RTM        0x00800000
#define LICENSE_ISSUER_ENFORCE_TYPE 0x00008000

#define GET_LICENSE_ISSUER_MAJORVERSION(dwVersion) \
    (((dwVersion) & 0x00700000) >> 20)

#define GET_LICENSE_ISSUER_MINORVERSION(dwVersion) \
    (((dwVersion) & 0x000F0000) >> 16)


typedef struct  _HWID
{
    DWORD       dwPlatformID;
    DWORD       Data1;
    DWORD       Data2;
    DWORD       Data3;
    DWORD       Data4;

} HWID, *PHWID;


typedef struct _Product_Info
{
    DWORD   dwVersion;
    DWORD   cbCompanyName;
    PBYTE   pbCompanyName;
    DWORD   cbProductID;
    PBYTE   pbProductID;
} Product_Info, *PProduct_Info;


typedef struct _LicensedVersionInfo {
    WORD    wMajorVersion;          // Product Major Version
    WORD    wMinorVersion;          // Product Minor Version
    DWORD   dwFlags;                // Product version specific flags
} LICENSED_VERSION_INFO, *PLICENSED_VERSION_INFO;


typedef struct _LicenseRequest
{
    PBYTE           pbEncryptedHwid;
    DWORD           cbEncryptedHwid;
    DWORD           dwLanguageID;
    DWORD           dwPlatformID;
    PProduct_Info   pProductInfo;

} LICENSEREQUEST, *PLICENSEREQUEST;

typedef struct _LicensedProduct
{
    DWORD                  dwLicenseVersion;   
    DWORD                  dwQuantity;

    PBYTE                  pbOrgProductID;      // original license request product Id
    DWORD                  cbOrgProductID;      // size of original license request product Id

    LICENSEREQUEST         LicensedProduct;     // licensed product
    LICENSED_VERSION_INFO* pLicensedVersion;    // licensed product version    
    DWORD                  dwNumLicensedVersion; // number of licensed product version

    LPTSTR                 szIssuer;
    LPTSTR                 szIssuerId;          // license server setup ID
    LPTSTR                 szIssuerScope;
    LPTSTR                 szLicensedClient;
    LPTSTR                 szLicensedUser;
    LPTSTR                 szIssuerDnsName;

    HWID                   Hwid;

    FILETIME               NotBefore;           // license's validity
    FILETIME               NotAfter;

    PBYTE                  pbPolicyData;       // Policy specfic extension
    DWORD                  cbPolicyData;       // size of policy specific extension
    ULARGE_INTEGER         ulSerialNumber;     // Client license's serial number
} LICENSEDPRODUCT, *PLICENSEDPRODUCT;



void
LSShutdownCertutilLib();

BOOL
LSInitCertutilLib( 
    HCRYPTPROV hProv 
);

void
LSFreeLicensedProduct(
    PLICENSEDPRODUCT pLicensedProduct
);

LICENSE_STATUS
LSVerifyDecodeClientLicense(
    __out_bcount(cbLicense) IN PBYTE                pbLicense,
    IN DWORD                cbLicense,
    __out_bcount(cbSecretKey) IN PBYTE                pbSecretKey,
    IN DWORD                cbSecretKey,
    IN OUT PDWORD           pdwNumLicensedInfo,
    __out_ecount(*pdwNumLicensedInfo) IN OUT PLICENSEDPRODUCT pLicensedInfo
);

LICENSE_STATUS
LicenseGetSecretKey(
    PDWORD  pcbSecretKey,
    __out_bcount_opt(*pcbSecretKey) BYTE FAR *   pSecretKey 
);

#ifdef __cplusplus
}
#endif

#endif  //_TSCCERTS_H_
