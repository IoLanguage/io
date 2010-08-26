@echo off
REM Make sure you disable error Windows reporting before running tests!
REM Also set the registry
REM HKEY_CURRENT_USER\Software\ Microsoft\Windows\Windows Error Reporting\DontShowUI
REM to 1 so that failed test doesn't block on pop-up window
REM
echo Failed Tests >failed.txt
for %%T in (libffi.call\*.c) DO (
    REM Because tests are written for GCC we need to force C++ code
    REM compilation. MSVC doesn't allow mixing code and declarations.
    REM 
    cl -nologo /Tp%%T -MD -DNDEBUG -Od -DWIN32 -DWINNT -EHsc -I . -I ..\ -I ..\include -I ..\src\x86 ..\.libs\ffi-3.lib
    %%~nT.exe
    if not ERRORLEVEL 0 echo %%~nT.c >>failed.txt
    del %%~nT.*
)

