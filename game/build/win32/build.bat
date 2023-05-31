@echo off
cls
echo ------------------------------------------------------------------------------
echo Simpsons 2 - Win32 build process
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
SET WIN32CONFIG=debug
goto MAKE

:RELEASE
SET WIN32CONFIG=release
goto MAKE

:TUNE
set WIN32CONFIG=Tune
goto MAKE


:MAKE
if "%2" == "clean" SET CLEANOPT=/CLEAN


:// build the WIN32 exe with the specified options
://

:// -----------------------------------------------------------------------
:// Full path and name of the Visual Studio batch file for setting up
:// the MSDev environment for Win32 builds. 
:// *** THIS _MUST_ ACCURATELY REFLECT WHERE THE FILE IS ON YOUR SYSTEM ***
:// -----------------------------------------------------------------------
SET VCVARS=C:\Program Files\Microsoft Visual Studio .NET\Vc7\bin\vcvars32.bat

IF EXIST "%VCVARS%" GOTO Win32Build
@ECHO ***ERROR*** Could not find MSDev environment.
@ECHO Make sure the VCVARS environment variable in make.bat is correct!
GOTO END

:Win32Build


:// This calls the Visual Studio batch file for setting up the MSDev Environment
://CALL "%VCVARS%"

:// Here's where the MSDev executable is
:// NOTE : %MSDevDir% is set by the VCVARS batch file

SET MSDEV=C:\Program Files\Microsoft Visual Studio .NET\Common7\IDE\devenv.exe 


"%MSDEV%" SRR2.sln /build %WIN32CONFIG% /project srr2  "srr2.sln" 
IF "%1" == "d" IF EXIST Debug\SRR2wd.exe echo Debug WIN32 build completed successfully.
IF "%1" == "d" IF NOT EXIST Debug\SRR2wd.exe echo Debug WIN32 build failed (see file err for details).
IF "%1" == "t" IF EXIST Tune\SRR2wt.exe echo Optimized WIN32 build completed successfully.
IF "%1" == "t" IF NOT EXIST Tune\SRR2wt.exe echo Optimized WIN32 build failed (see file err for details).
IF "%1" == "r" IF EXIST Release\SRR2wr.exe echo Release WIN32 build completed successfully.
IF "%1" == "r" IF NOT EXIST Release\SRR2wr.exe echo Release WIN32 build failed (see file err for details).

goto END


:ERROR
echo Please specify option all\d\r\t [clean]

:END
echo ------------------------------------------------------------------------------

pause
