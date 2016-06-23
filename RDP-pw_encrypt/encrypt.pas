function CryptRDPPassword(sPassword: string): string; 
var DataIn: DATA_BLOB; 
    DataOut: DATA_BLOB; 
    pwDescription: PWideChar; 
    PwdHash: string; 
begin 
  PwdHash := '';                                      

  DataOut.cbData := 0; 
  DataOut.pbData := nil;                                      

  // RDP uses UniCode 
  DataIn.pbData := Pointer(WideString(sPassword)); 
  DataIn.cbData := Length(sPassword) * SizeOf(WChar);                                      

  // RDP always sets description to psw 
  pwDescription := WideString('psw');                                      

  if CryptProtectData(@DataIn, 
                      pwDescription, 
                      nil, 
                      nil, 
                      nil, 
                      CRYPTPROTECT_UI_FORBIDDEN,  // Never show interface 
                      @DataOut) then 
  begin 
    PwdHash := BlobDataToHexStr(DataOut.pbData, DataOut.cbData); 
  end; 
  Result := PwdHash;                                      

  // Cleanup 
  LocalFree(Cardinal(DataOut.pbData)); 
  LocalFree(Cardinal(DataIn.pbData));                                      

end;