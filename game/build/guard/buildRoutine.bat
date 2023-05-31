@echo off
rem Deleting old log files
rem ---------------------
md logs\

rem ENTER BUILD DIRECTORY
rem ---------------------
cd..

rem BUILD PS2 CODE
rem ---------------------
cd ps2
echo Building PS2 Code  (release)
echo --------------------Configuration: - PS2 Release -----------------  
rem ..\guard\logs\\ps2r.log
del /f ..\guard\logs\ps2r.log
call build r >> ..\guard\logs\ps2r.log 2>&1

echo Building PS2 Code  (tune)
echo --------------------Configuration: - PS2 Tune --------------------  
rem ..\guard\logs\\ps2t.log
del /f ..\guard\logs\ps2t.log
call build t >> ..\guard\logs\ps2t.log 2>&1

echo Building PS2 Code  (debug)
echo --------------------Configuration: - PS2 Debug -------------------  
rem ..\guard\logs\\ps2d.log
del /f ..\guard\logs\ps2d.log
call build d >> ..\guard\logs\ps2d.log 2>&1

cd..
REM Build all Xbox Code
rem ---------------------
cd xbox

echo Rebuilding Xbox Code (release) 
del /f ..\guard\logs\Xboxr.log
devenv /build release srr2.sln > ..\guard\logs\xboxr.log 2>&1

echo Rebuilding Xbox Code (tune)
del /f ..\guard\logs\Xboxt.log
devenv /build tune srr2.sln > ..\guard\logs\xboxt.log 2>&1

echo Rebuilding Xbox Code (debug)
del /f ..\guard\logs\Xboxd.log
devenv /build debug srr2.sln > ..\guard\logs\xboxd.log 2>&1
cd..

rem BUILD GC CODE
rem ---------------------
cd gc
echo Building GC Code  (release)
echo --------------------Configuration: - GC Release -----------------  
del /f ..\guard\logs\gcr.log
call build r >> ..\guard\logs\gcr.log 2>&1

echo Building GC Code  (tune)
echo --------------------Configuration: - GC Tune --------------------  
del /f ..\guard\logs\gct.log
call build t >> ..\guard\logs\gct.log 2>&1

echo Building gc Code  (debug)
echo --------------------Configuration: - gc Debug -------------------  
del /f ..\guard\logs\gcd.log
call build d >> ..\guard\logs\gcd.log 2>&1
cd..


REM Update xbox symbols
rem ---------------------
rem md ..\cd\XBsymbols\
rem copy xbox\release\SRR2xr.exe ..\cd\XBsymbols\ /y
rem copy xbox\release\SRR2xr.pdb ..\cd\XBsymbols\ /y
rem copy xbox\release\SRR2xr.xbe ..\cd\XBsymbols\ /y
rem copy xbox\Debug\SRR2xd.exe ..\cd\XBsymbols\ /y
rem copy xbox\Debug\SRR2xd.pdb ..\cd\XBsymbols\ /y
rem copy xbox\Debug\SRR2xd.xbe ..\cd\XBsymbols\ /y
rem copy xbox\tune\SRR2xt.exe ..\cd\XBsymbols\ /y
rem copy xbox\tune\SRR2xt.pdb ..\cd\XBsymbols\ /y
rem copy xbox\tune\SRR2xt.xbe ..\cd\XBsymbols\ /y


rem SET BACK TO START
rem ---------------------
cd guard
..\tools\mksnt\grep.exe -f summarypat.txt logs\*.log > logs\summary.txt