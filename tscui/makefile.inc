!if 0
Copyright (c) Microsoft Corporation.  All rights reserved.
!endif
!if 0
Use of this source code is subject to the terms of the Microsoft end-user
license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
If you did not accept the terms of the EULA, you are not authorized to use
this source code. For a copy of the EULA, please see the LICENSE.RTF on your
install media.
!endif
{}.rc{$(_OBJDIR)\}.res::
	$(RCOMPILER) $(RCFLAGS) -fo$(_OBJDIR)\$(RCFILE).res $(MAKEDIR)\$(RCFILE).rc
	copy $(_OBJDIR)\$(RCFILE).res $(_RELEASELIBDIR)\$(TARGETNAME).res