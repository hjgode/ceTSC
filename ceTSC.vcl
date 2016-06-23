<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: ceTSC - Win32 (WCE ARMV4I) Debug--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\Temp\RSP349.tmp" with contents
[
/nologo /W3 /Zi /Od /D "DEBUG" /D "ARM" /D "_ARM_" /D "ARMV4I" /D UNDER_CE=500 /D _WIN32_WCE=500 /D "WCE_PLATFORM_CK60Prem" /D "UNICODE" /D "_UNICODE" /FR"ARMV4IDbg/" /Fp"ARMV4IDbg/ceTSC.pch" /YX /Fo"ARMV4IDbg/" /Fd"ARMV4IDbg/" /QRarch4T /QRinterwork-return /MC /c 
"D:\C-Source\test\ceTSC\mainwnd.cpp"
]
Creating command line "clarm.exe @C:\Temp\RSP349.tmp" 
<h3>Output Window</h3>
Compiling...
mainwnd.cpp
c:\programme\windows ce tools\wce500\windows mobile 5.0 pocket pc sdk\include\armv4i\windbase.h(662) : warning C4068: unknown pragma
d:\c-source\test\cetsc\auto_xxx.hxx(539) : error C2964: invalid expression as template parameter
d:\c-source\test\cetsc\auto_xxx.hxx(540) : error C2964: invalid expression as template parameter
d:\c-source\test\cetsc\auto_xxx.hxx(548) : error C2964: invalid expression as template parameter
d:\c-source\test\cetsc\auto_xxx.hxx(549) : error C2964: invalid expression as template parameter
d:\c-source\test\cetsc\auto_xxx.hxx(649) : error C2440: 'specialization' : cannot convert from '' to 'int (__cdecl *)(const unsigned long)'
        None of the functions with this name in scope match the target type
d:\c-source\test\cetsc\auto_xxx.hxx(649) : error C2975: 'auto_xxx' : invalid template argument for 'fnClose', constant expression expected
        d:\c-source\test\cetsc\auto_xxx.hxx(409) : see declaration of 'auto_xxx'
d:\c-source\test\cetsc\auto_xxx.hxx(650) : error C2440: 'specialization' : cannot convert from '' to 'int (__cdecl *)(const unsigned long)'
        None of the functions with this name in scope match the target type
d:\c-source\test\cetsc\auto_xxx.hxx(650) : error C2975: 'auto_xxx' : invalid template argument for 'fnClose', constant expression expected
        d:\c-source\test\cetsc\auto_xxx.hxx(409) : see declaration of 'auto_xxx'
d:\c-source\test\cetsc\auto_xxx.hxx(656) : error C2440: 'specialization' : cannot convert from '' to 'int (__cdecl *)(const unsigned long)'
        None of the functions with this name in scope match the target type
d:\c-source\test\cetsc\auto_xxx.hxx(656) : error C2975: 'auto_xxx' : invalid template argument for 'fnClose', constant expression expected
        d:\c-source\test\cetsc\auto_xxx.hxx(409) : see declaration of 'auto_xxx'
d:\c-source\test\cetsc\auto_xxx.hxx(657) : error C2440: 'specialization' : cannot convert from '' to 'int (__cdecl *)(const unsigned long)'
        None of the functions with this name in scope match the target type
d:\c-source\test\cetsc\auto_xxx.hxx(657) : error C2975: 'auto_xxx' : invalid template argument for 'fnClose', constant expression expected
        d:\c-source\test\cetsc\auto_xxx.hxx(409) : see declaration of 'auto_xxx'
d:\c-source\test\cetsc\allocator.hxx(289) : warning C4068: unknown pragma
d:\c-source\test\cetsc\hash_map.hxx(122) : fatal error C1083: Cannot open include file: 'hash.hxx': No such file or directory
Error executing clarm.exe.



<h3>Results</h3>
mainwnd.obj - 13 error(s), 2 warning(s)
</pre>
</body>
</html>
