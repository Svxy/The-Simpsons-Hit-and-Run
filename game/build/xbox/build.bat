@echo off
cls
echo ------------------------------------------------------------------------------
echo Simpsons 2 - XBOX  build process
echo ------------------------------------------------------------------------------

rem ### Use this to chain commands together ###
set WHENDONE=END

rem ### Use this to signal cleaning ###
set DOCLEAN=false

if "%1" == "d" goto DEBUG
if "%1" == "r" goto RELEASE
if "%1" == "t" goto TUNE
://if "%1" == "all" goto ALL
goto ERROR:

:DEBUG
SET XBOXCONFIG=debug
goto MAKE

:RELEASE
SET XBOXCONFIG=release
goto MAKE

:TUNE
set XBOXCONFIG=Tune
goto MAKE


:MAKE
if "%2" == "clean" SET CLEANOPT=/CLEAN


:// build the XBOX xbe with the specified options
://

:// -----------------------------------------------------------------------
:// Full path and name of the Visual Studio batch file for setting up
:// the MSDev environment for Win32 builds. 
:// *** THIS _MUST_ ACCURATELY REFLECT WHERE THE FILE IS ON YOUR SYSTEM ***
:// -----------------------------------------------------------------------
SET VCVARS=C:\Program Files\Microsoft Visual Studio .NET\Vc7\bin\vcvars32.bat

IF EXIST "%VCVARS%" GOTO XboxBuild
@ECHO ***ERROR*** Could not find MSDev environment.
@ECHO Make sure the VCVARS environment variable in make.bat is correct!
GOTO END

:XboxBuild


:// This calls the Visual Studio batch file for setting up the MSDev Environment
://CALL "%VCVARS%"

:// Here's where the MSDev executable is
:// NOTE : %MSDevDir% is set by the VCVARS batch file

SET MSDEV=C:\Program Files\Microsoft Visual Studio .NET\Common7\IDE\devenv.exe 


"%MSDEV%" SRR2.sln /build %XBOXCONFIG% /project srr2  "srr2.sln" 
IF "%1" == "d" IF EXIST Debug\SRR2xd.xbe echo Debug XBOX build completed successfully.
IF "%1" == "d" IF NOT EXIST Debug\SRR2xd.xbe echo Debug XBOX build failed (see file err for details).
IF "%1" == "t" IF EXIST Tune\SRR2xt.xbe echo Optimized XBOX build completed successfully.
IF "%1" == "t" IF NOT EXIST Tune\SRR2xt.xbe echo Optimized XBOX build failed (see file err for details).
IF "%1" == "r" IF EXIST Release\SRR2xr.xbe echo Release XBOX build completed successfully.
IF "%1" == "r" IF NOT EXIST Release\SRR2xr.xbe echo Release XBOX build failed (see file err for details).

goto END


:ERROR
echo Please specify option all\d\r\t [clean]

:END
echo ------------------------------------------------------------------------------

pause
