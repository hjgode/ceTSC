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
#include "common.h"

#include <winscard.h>
typedef LONG (WINAPI *PFN_SCARDESTABLISHCONTEXT)(DWORD, LPCVOID, LPCVOID, LPSCARDCONTEXT);

static DWORD gadwDisableCtls[] = {  IDC_CBO_SOUND,
                                    IDC_CHK_REDIRECT_DRIVES,
                                    IDC_CHK_REDIRECT_PRINTERS,
                                    IDC_CHK_REDIRECT_COM,
                                    IDC_CHK_REDIRECT_SMARTCARD,
                                    IDC_BTN_SETUP_PRINTER,
                                    IDC_CBO_SEND_KEYS,
                                };

static DWORD gadwRedirectors[] = {  RDRSUPPORT_AUDIO,
                                    RDRSUPPORT_DRIVE,
                                    RDRSUPPORT_PRINTER,
                                    RDRSUPPORT_PORT,
                                    RDRSUPPORT_SCARD,
                                };


ComboItem gAudioRedir[] = { {0, AUDIOMODE_REDIRECT, IDS_PLAYSOUND_LOCAL     },
                            {1, AUDIOMODE_NONE,     IDS_PLAYSOUND_NOSOUND   },
                            {2, AUDIOMODE_SERVER,   IDS_PLAYSOUND_REMOTE    },
                            };

ComboItem gKbdRedir[] = {   {0, 0,  IDS_SENDKEYS_LOCAL      },
                            {1, 1,  IDS_SENDKEYS_REMOTE     },
                            {2, 2,  IDS_SENDKEYS_FSCREEN    }
                            };

int CPropLocalRes::DialogProc(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam)
{
	int nRet = FALSE;
	switch(uMsg)
	{
	case WM_INITDIALOG:
	{
		nRet = CDlgBase::DialogProc(hwndDlg, uMsg, wParam, lParam);
		if (!nRet)
			return FALSE;

		_dwSupportedRedirectors = SendMessage(_hwndOwner, MSG_TSC_GETRDRSUPPORT, 0, 0);

		if (_dwSupportedRedirectors & RDRSUPPORT_CLIPBD)
		{
			ShowWindow(GetDlgItem(hwndDlg, IDC_STC_NOCLIPRDR), SW_HIDE);
		}
		
		TscPropType eType = TscTypeULONG;
		DWORD dwVal = 0, dwCount = sizeof(gKbdRedir)/sizeof(ComboItem), dwSize = sizeof(DWORD);
		_pSettings->Read(PROP_FULLSCREENALWAYS, &eType, NULL, (PBYTE)&dwVal, &dwSize);
		if (dwVal == 1)
			dwCount--;

		InitCombo(_hInst, hwndDlg, IDC_CBO_SEND_KEYS, gKbdRedir, dwCount);
		InitCombo(_hInst, hwndDlg, IDC_CBO_SOUND, gAudioRedir, sizeof(gAudioRedir)/sizeof(ComboItem));

		InitPrinterInfo();
		InitSCardCheckBox();

		UpdateRedirectorControls();

		return LoadFromSettings();
	}

	case WM_DESTROY:
	{
		nRet = SaveToSettings();
		break;
	}

	case WM_COMMAND:
	{
		if (HIWORD(wParam) == BN_CLICKED)
		{
			if (LOWORD(wParam) == IDC_BTN_SETUP_PRINTER)
			{
				return SetupPrinter();
			}
			else if (LOWORD(wParam) == IDC_CHK_REDIRECT_PRINTERS)
			{
				nRet = (BST_CHECKED == SendDlgItemMessage(_hwndDlg, IDC_CHK_REDIRECT_PRINTERS, BM_GETCHECK, 0, 0));
				EnableWindow(GetDlgItem(_hwndDlg, IDC_BTN_SETUP_PRINTER), nRet);
				return TRUE;
			}
		}

		break;
	}

	case MSG_TSC_CONNECT:
	{
		SaveToSettings();
		EnableControls(_hwndDlg, gadwDisableCtls, sizeof(gadwDisableCtls)/sizeof(gadwDisableCtls[0]), FALSE);
		return TRUE;
	}

	case MSG_TSC_DISCONNECTED:
	{
		EnableWindow(GetDlgItem(_hwndDlg, IDC_CBO_SEND_KEYS), TRUE);
		UpdateRedirectorControls();
		return TRUE;
	}


	default:
		break;
	}
	return CDlgBase::DialogProc(hwndDlg, uMsg, wParam, lParam) || nRet;
}

void CPropLocalRes::UpdateRedirectorControls()
{
	int nCount = sizeof(gadwRedirectors)/sizeof(gadwRedirectors[0]);
	for (int i=0; i<nCount; i++)
	{
		EnableWindow(GetDlgItem(_hwndDlg, gadwDisableCtls[i]), (_dwSupportedRedirectors & gadwRedirectors[i]));
	}
}

BOOL CPropLocalRes::SetupPrinter()
{
    SHELLEXECUTEINFO sei;

    memset(&sei,0,sizeof(SHELLEXECUTEINFO));
    sei.cbSize = sizeof(sei);
    sei.hwnd = _hwndDlg;
    sei.lpFile = L"ctlpnl.EXE";
    sei.lpParameters = _T("wbtprncpl.dll,0");
    sei.lpDirectory = NULL;
    sei.nShow = SW_SHOWNORMAL;

    return ShellExecuteEx(&sei);
}


void CPropLocalRes::InitPrinterInfo()
{
	HWND hwnd = GetDlgItem(_hwndDlg, IDC_CHK_REDIRECT_PRINTERS);
	ASSERT(hwnd);

	BOOL bPrinters  = _dwSupportedRedirectors & RDRSUPPORT_PRINTER;
	if (!bPrinters)
	{
		HWND hwndChkPrn = GetDlgItem(_hwndDlg, IDC_CHK_REDIRECT_PRINTERS);
		SendMessage(hwndChkPrn, BM_SETCHECK, BST_UNCHECKED, 0);
		EnableWindow(hwndChkPrn, FALSE);
	}
		
    if ( (!bPrinters) || (GetFileAttributes(PRINTER_APPLET_NAME) == 0xffffffff) )
    {
        ShowWindow(GetDlgItem(_hwndDlg,IDC_BTN_SETUP_PRINTER), SW_HIDE);
    }
}

void CPropLocalRes::InitSCardCheckBox()
{
    HMODULE hDll = NULL;
    PFN_SCARDESTABLISHCONTEXT pSCardEstablishContext;
    BOOL fPresent = FALSE;

    hDll = LoadLibrary( _T("WINSCARD.DLL"));
    if (hDll)
    {
        pSCardEstablishContext = (PFN_SCARDESTABLISHCONTEXT) GetProcAddress( hDll, L"SCardEstablishContext");
        FreeLibrary(hDll);
		fPresent = (pSCardEstablishContext != NULL);
    }

	if ((!fPresent) || (!(_dwSupportedRedirectors & RDRSUPPORT_SCARD)))
	{
		ShowWindow(GetDlgItem(_hwndDlg, IDC_CHK_REDIRECT_SMARTCARD), SW_HIDE);
	}
}


BOOL CPropLocalRes::LoadFromSettings()
{
	int nVal, nValDef = 0; 
	TscPropType eType = TscTypeULONG;
	DWORD dwSize = sizeof(DWORD);

	ASSERT(_pSettings);

	if (_dwSupportedRedirectors & RDRSUPPORT_AUDIO)
	{
		if ( (!_pSettings->Read(PROP_AUDIOREDIRECTIONMODE, &eType, (PBYTE)&nValDef, (PBYTE)&nVal, &dwSize)) ||
			 (eType != TscTypeULONG) || (dwSize != sizeof(DWORD) || ((nVal < 0) && (nVal > 2))) )
		{
			DEBUGMSG(ZONE_UI, (L"Invalid value for AudioRedirectionMode : %d\n", nVal));
			return FALSE;
		}
		if (nVal == AUDIOMODE_REDIRECT)
			nVal = 0;
		else if (nVal == AUDIOMODE_NONE)
			nVal = 1;
		else if (nVal == AUDIOMODE_SERVER)
			nVal = 2;
		else
		{
			return FALSE;
			ASSERT(FALSE);
		}
		SendDlgItemMessage(_hwndDlg, IDC_CBO_SOUND, CB_SETCURSEL, nVal, 0);
	}

	nValDef = 1;
	if ( (!_pSettings->Read(PROP_KEYBOARDHOOKMODE, &eType, (PBYTE)&nValDef, (PBYTE)&nVal, &dwSize)) ||
		 (eType != TscTypeULONG) || (dwSize != sizeof(DWORD) || ((nVal < 0) && (nVal > 1))) )
	{
		DEBUGMSG(ZONE_UI, (L"Invalid value for KeyboardHookMode : %d\n", nVal));
		return FALSE;
	}
	SendDlgItemMessage(_hwndDlg, IDC_CBO_SEND_KEYS, CB_SETCURSEL, nVal, 0);


	if (_dwSupportedRedirectors & RDRSUPPORT_DRIVE)
	{
		nValDef = 0;
		if ( (!_pSettings->Read(PROP_REDIRECTDRIVES, &eType, (PBYTE)&nValDef, (PBYTE)&nVal, &dwSize)) ||
			 (dwSize != sizeof(DWORD) || ((nVal != 0) && (nVal != 1)) ) )
		{
			DEBUGMSG(ZONE_UI, (L"Invalid value for RedirectDrives : %d\n", nVal));
			return FALSE;
		}
		SendDlgItemMessage(_hwndDlg, IDC_CHK_REDIRECT_DRIVES, BM_SETCHECK, (nVal == 1) ? BST_CHECKED : BST_UNCHECKED, 0);
	}


	if (_dwSupportedRedirectors & RDRSUPPORT_PRINTER)
	{
		nValDef = 1;
		if ( (!_pSettings->Read(PROP_REDIRECTPRINTERS, &eType, (PBYTE)&nValDef, (PBYTE)&nVal, &dwSize)) ||
			 (dwSize != sizeof(DWORD) || ((nVal != 0) && (nVal != 1)) ) )
		{
			DEBUGMSG(ZONE_UI, (L"Invalid value for RedirectPrinters : %d\n", nVal));
			return FALSE;
		}
		SendDlgItemMessage(_hwndDlg, IDC_CHK_REDIRECT_PRINTERS, BM_SETCHECK, (nVal == 1) ? BST_CHECKED : BST_UNCHECKED, 0);
	}


	if (_dwSupportedRedirectors & RDRSUPPORT_PORT)
	{
		nValDef = 0;
		if ( (!_pSettings->Read(PROP_REDIRECTPORTS, &eType, (PBYTE)&nValDef, (PBYTE)&nVal, &dwSize)) ||
			 (dwSize != sizeof(DWORD) || ((nVal != 0) && (nVal != 1))) )
		{
			DEBUGMSG(ZONE_UI, (L"Invalid value for RedirectPorts : %d\n", nVal));
			return FALSE;
		}
		SendDlgItemMessage(_hwndDlg, IDC_CHK_REDIRECT_COM, BM_SETCHECK, (nVal == 1) ? BST_CHECKED : BST_UNCHECKED, 0);
	}


	if (_dwSupportedRedirectors & RDRSUPPORT_SCARD)
	{
		nValDef = 1;
		if ( (!_pSettings->Read(PROP_REDIRECTSMARTCARDS, &eType, (PBYTE)&nValDef, (PBYTE)&nVal, &dwSize)) ||
			 (dwSize != sizeof(DWORD) || ((nVal != 0) && (nVal != 1)) ) )
		{
			DEBUGMSG(ZONE_UI, (L"Invalid value for RedirectSmartCards : %d\n", nVal));
			return FALSE;
		}
		SendDlgItemMessage(_hwndDlg, IDC_CHK_REDIRECT_SMARTCARD, BM_SETCHECK, (nVal == 1) ? BST_CHECKED : BST_UNCHECKED, 0);
	}

	return TRUE;
}

BOOL CPropLocalRes::SaveToSettings()
{
	ASSERT(_pSettings);

	int nVal;

	nVal = SendDlgItemMessage(_hwndDlg, IDC_CBO_SOUND, CB_GETCURSEL, 0, 0);
	ASSERT(nVal != CB_ERR);
	nVal = SendDlgItemMessage(_hwndDlg, IDC_CBO_SOUND, CB_GETITEMDATA, nVal, 0);
	if (!_pSettings->Write(PROP_AUDIOREDIRECTIONMODE, TscTypeULONG, (PBYTE )(&nVal), sizeof(DWORD)))
	{
		return FALSE;
	}


	nVal = SendDlgItemMessage(_hwndDlg, IDC_CBO_SEND_KEYS, CB_GETCURSEL, 0, 0);
	if (!_pSettings->Write(PROP_KEYBOARDHOOKMODE, TscTypeULONG, (PBYTE )(&nVal), sizeof(DWORD)))
	{
		return FALSE;
	}


	nVal = (SendDlgItemMessage(_hwndDlg, IDC_CHK_REDIRECT_DRIVES, BM_GETCHECK, 0, 0) == BST_CHECKED) ? 1 : 0;
	if (!_pSettings->Write(PROP_REDIRECTDRIVES, TscTypeBOOL, (PBYTE )(&nVal), sizeof(DWORD)))
	{
		return FALSE;
	}

	nVal = (SendDlgItemMessage(_hwndDlg, IDC_CHK_REDIRECT_PRINTERS, BM_GETCHECK, 0, 0) == BST_CHECKED) ? 1 : 0;
	if (!_pSettings->Write(PROP_REDIRECTPRINTERS, TscTypeBOOL, (PBYTE )(&nVal), sizeof(DWORD)))
	{
		return FALSE;
	}

	nVal = (SendDlgItemMessage(_hwndDlg, IDC_CHK_REDIRECT_COM, BM_GETCHECK, 0, 0) == BST_CHECKED) ? 1 : 0;
	if (!_pSettings->Write(PROP_REDIRECTPORTS, TscTypeBOOL, (PBYTE )(&nVal), sizeof(DWORD)))
	{
		return FALSE;
	}

	nVal = (SendDlgItemMessage(_hwndDlg, IDC_CHK_REDIRECT_SMARTCARD, BM_GETCHECK, 0, 0) == BST_CHECKED) ? 1 : 0;
	if (!_pSettings->Write(PROP_REDIRECTSMARTCARDS, TscTypeBOOL, (PBYTE )(&nVal), sizeof(DWORD)))
	{
		return FALSE;
	}

	return TRUE;
}
