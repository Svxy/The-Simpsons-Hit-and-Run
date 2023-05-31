@echo off
cls
echo ------------------------------------------------------------------------------
echo Simpsons 2 - Radbuild build process (PS2MW)
echo ------------------------------------------------------------------------------

set makeshell=..\..\libs\radbuild\GlobalTools\sh.exe

rem ### Use this to chain commands together ###
set WHENDONE=END

rem ### Use this to signal cleaning ###
set DOCLEAN=false

if "%1" == "d" goto DEBUG
if "%1" == "r" goto RELEASE
if "%1" == "t" goto TUNE
if "%1" == "all" goto ALL
goto ERROR:

:DEBUG
set CONFIG=DEBUG
goto MAKE

:RELEASE
set CONFIG=RELEASE
goto MAKE

:TUNE
set CONFIG=TUNE
goto MAKE

:ALL
:ALL_DEBUG
set WHENDONE=ALL_RELEASE
goto DEBUG
:ALL_RELEASE
set WHENDONE=ALL_TUNE
goto RELEASE
:ALL_TUNE
set WHENDONE=END
goto TUNE
goto ERROR

:MAKE
if "%2" == "clean" goto CLEAN
if "%2" == "allclean" goto ALLCLEAN
if "%2" == "libsclean" goto LIBSCLEAN
if "%2" == "quick" goto QUICK
..\..\libs\radbuild\GlobalTools\make -r 2>&1 | ..\tools\tee err
goto %WHENDONE%

:QUICK
..\..\libs\radbuild\GlobalTools\make -r quick 2>&1 | ..\tools\tee err
goto %WHENDONE%

:CLEAN
set DOCLEAN=true
..\..\libs\radbuild\GlobalTools\make -r clean
set DOCLEAN=false
goto %WHENDONE%

:LIBSCLEAN
set DOCLEAN=true
..\..\libs\radbuild\GlobalTools\make -r libsclean
set DOCLEAN=false
goto %WHENDONE%

:ALLCLEAN
set DOCLEAN=true
..\..\libs\radbuild\GlobalTools\make -r allclean
set DOCLEAN=false
goto %WHENDONE%

:ERROR
echo Please specify option all\d\r\t [clean]

:END
echo ------------------------------------------------------------------------------

pause
