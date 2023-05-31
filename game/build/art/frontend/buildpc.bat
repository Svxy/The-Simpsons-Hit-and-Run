@echo off

::::: PC :::::::::::::::
:::

echo -----
echo ---------- Building Simpsons2 FrontEnd for PC ----------
echo -----

call build\art\frontend\pc\fonts.bat

echo Checking out files from Perforce ... ...
P4 edit //depot/game/cd/pc/art/frontend/scrooby/...
echo.

echo Copying files to pc CD ... ...
rmdir /s /q cd\pc\art\frontend\scrooby
xcopy exportart\frontend\scrooby\_pc\*.* cd\pc\art\frontend\scrooby\ /D /S /Y /EXCLUDE:build\art\frontend\excludepc.txt
xcopy exportart\frontend\scrooby\_pc\resource\images\_pc2\*.* cd\pc\art\frontend\scrooby\resource\images\ /S /Y
xcopy exportart\frontend\scrooby\_pc\resource\pure3d\_pc2\*.* cd\pc\art\frontend\scrooby\resource\pure3d\ /S /Y
xcopy exportart\frontend\scrooby\_pc\resource\fonts\_pc2\*.* cd\pc\art\frontend\scrooby\resource\fonts\ /S /Y
echo DONE Copying files to pc CD.
echo.

echo [pc]: Processing pure3d files ... ...
cd cd\pc\art\frontend\scrooby\resource\pure3d
p3danim -C -O --quaternion-tolerance 0.1 *.p3d
p3dimage --ntsc_fix -n -O -Z1024 *.p3d
p3ddel -n -d7000 *.p3d
cd ..\..\..\..\..\..\..
echo DONE processing pure3d files.
echo.

echo [pc]: Processing project resource files ... ...
cd cd\pc\art\frontend\scrooby\
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl language.prj a --srr2 -S
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl bootup.prj a --srr2 -S
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl backend.prj a --srr2 -S
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl frontend.prj a --srr2 -S
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl minigame.prj a --srr2 -S
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl ingame.prj a --srr2 -S
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl ingamel1.prj a --srr2 -S
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl ingamel2.prj a --srr2 -S
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl ingamel3.prj a --srr2 -S
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl ingamel4.prj a --srr2 -S
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl ingamel5.prj a --srr2 -S
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl ingamel6.prj a --srr2 -S
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl ingamel7.prj a --srr2 -S
cd ..\..\..\..\..\
echo DONE processing project resource files.
echo.

echo [pc]: Joining project resource files ... ...

cd cd\pc\art\frontend\scrooby\resource\_language\
p3djoin -s -o ..\language.p3d *.p3d
cd ..\..\..\..\..\..\..

cd cd\pc\art\frontend\scrooby\resource\_bootup\
p3djoin -s -o ..\bootup.p3d *.p3d
cd ..\..\..\..\..\..\..

cd cd\pc\art\frontend\scrooby\resource\_backend\
p3djoin -s -o ..\backend.p3d *.p3d
cd ..\..\..\..\..\..\..

cd cd\pc\art\frontend\scrooby\resource\_frontend\
p3djoin -s -o ..\frontend.p3d *.p3d
cd ..\..\..\..\..\..\..

cd cd\pc\art\frontend\scrooby\resource\_minigame\
p3djoin -s -o ..\minigame.p3d *.p3d
cd ..\..\..\..\..\..\..

cd cd\pc\art\frontend\scrooby\resource\_ingame\
p3djoin -s -o ..\ingame.p3d *.p3d
cd ..\..\..\..\..\..\..

rmdir /s /q cd\pc\art\frontend\scrooby\resource\_language
rmdir /s /q cd\pc\art\frontend\scrooby\resource\_bootup
rmdir /s /q cd\pc\art\frontend\scrooby\resource\_backend
rmdir /s /q cd\pc\art\frontend\scrooby\resource\_frontend
rmdir /s /q cd\pc\art\frontend\scrooby\resource\_minigame
rmdir /s /q cd\pc\art\frontend\scrooby\resource\_ingame

p3djoin -o cd\pc\art\frontend\scrooby\language.p3d cd\pc\art\frontend\scrooby\resource\language.p3d cd\pc\art\frontend\scrooby\language.p3d
p3djoin -o cd\pc\art\frontend\scrooby\bootup.p3d cd\pc\art\frontend\scrooby\resource\bootup.p3d cd\pc\art\frontend\scrooby\bootup.p3d
p3djoin -o cd\pc\art\frontend\scrooby\backend.p3d cd\pc\art\frontend\scrooby\resource\backend.p3d cd\pc\art\frontend\scrooby\backend.p3d
p3djoin -o cd\pc\art\frontend\scrooby\frontend.p3d cd\pc\art\frontend\scrooby\resource\frontend.p3d cd\pc\art\frontend\scrooby\frontend.p3d
p3djoin -o cd\pc\art\frontend\scrooby\minigame.p3d cd\pc\art\frontend\scrooby\resource\minigame.p3d cd\pc\art\frontend\scrooby\minigame.p3d
p3djoin -o cd\pc\art\frontend\scrooby\ingame.p3d cd\pc\art\frontend\scrooby\resource\ingame.p3d cd\pc\art\frontend\scrooby\ingame.p3d

cd cd\pc\art\frontend\scrooby\resource\_ingamel1\
p3djoin -s -o ..\ingamel1.p3d *.p3d
cd ..\..\..\..\..\..\..

cd cd\pc\art\frontend\scrooby\resource\_ingamel2\
p3djoin -s -o ..\ingamel2.p3d *.p3d
cd ..\..\..\..\..\..\..

cd cd\pc\art\frontend\scrooby\resource\_ingamel3\
p3djoin -s -o ..\ingamel3.p3d *.p3d
cd ..\..\..\..\..\..\..

cd cd\pc\art\frontend\scrooby\resource\_ingamel4\
p3djoin -s -o ..\ingamel4.p3d *.p3d
cd ..\..\..\..\..\..\..

cd cd\pc\art\frontend\scrooby\resource\_ingamel5\
p3djoin -s -o ..\ingamel5.p3d *.p3d
cd ..\..\..\..\..\..\..

cd cd\pc\art\frontend\scrooby\resource\_ingamel6\
p3djoin -s -o ..\ingamel6.p3d *.p3d
cd ..\..\..\..\..\..\..

cd cd\pc\art\frontend\scrooby\resource\_ingamel7\
p3djoin -s -o ..\ingamel7.p3d *.p3d
cd ..\..\..\..\..\..\..

rmdir /s /q cd\pc\art\frontend\scrooby\resource\_ingamel1
rmdir /s /q cd\pc\art\frontend\scrooby\resource\_ingamel2
rmdir /s /q cd\pc\art\frontend\scrooby\resource\_ingamel3
rmdir /s /q cd\pc\art\frontend\scrooby\resource\_ingamel4
rmdir /s /q cd\pc\art\frontend\scrooby\resource\_ingamel5
rmdir /s /q cd\pc\art\frontend\scrooby\resource\_ingamel6
rmdir /s /q cd\pc\art\frontend\scrooby\resource\_ingamel7

p3djoin -o cd\pc\art\frontend\scrooby\ingamel1.p3d cd\pc\art\frontend\scrooby\resource\ingamel1.p3d cd\pc\art\frontend\scrooby\ingamel1.p3d
p3djoin -o cd\pc\art\frontend\scrooby\ingamel2.p3d cd\pc\art\frontend\scrooby\resource\ingamel2.p3d cd\pc\art\frontend\scrooby\ingamel2.p3d
p3djoin -o cd\pc\art\frontend\scrooby\ingamel3.p3d cd\pc\art\frontend\scrooby\resource\ingamel3.p3d cd\pc\art\frontend\scrooby\ingamel3.p3d
p3djoin -o cd\pc\art\frontend\scrooby\ingamel4.p3d cd\pc\art\frontend\scrooby\resource\ingamel4.p3d cd\pc\art\frontend\scrooby\ingamel4.p3d
p3djoin -o cd\pc\art\frontend\scrooby\ingamel5.p3d cd\pc\art\frontend\scrooby\resource\ingamel5.p3d cd\pc\art\frontend\scrooby\ingamel5.p3d
p3djoin -o cd\pc\art\frontend\scrooby\ingamel6.p3d cd\pc\art\frontend\scrooby\resource\ingamel6.p3d cd\pc\art\frontend\scrooby\ingamel6.p3d
p3djoin -o cd\pc\art\frontend\scrooby\ingamel7.p3d cd\pc\art\frontend\scrooby\resource\ingamel7.p3d cd\pc\art\frontend\scrooby\ingamel7.p3d

echo DONE joining project resource files.
echo.

echo [pc]: Finalizing Scrooby project files ... ...
cd cd\pc\art\frontend\scrooby
p3ddel -n -d7000 *.p3d
cd ..\..\..\..\..
echo DONE finalizing Scrooby project files.
echo.

echo [pc]: Cleaning Scrooby CD folder ... ...
rmdir /s /q cd\pc\art\frontend\scrooby\resource\fonts
rmdir /s /q cd\pc\art\frontend\scrooby\resource\images
xcopy cd\pc\art\frontend\scrooby\resource\txtbible\*.p3d cd\pc\art\frontend\scrooby\resource\txtbible\p3d\
del /f /q cd\pc\art\frontend\scrooby\resource\txtbible\*.*
copy cd\pc\art\frontend\scrooby\resource\txtbible\p3d\*.p3d cd\pc\art\frontend\scrooby\resource\txtbible
rmdir /s /q cd\pc\art\frontend\scrooby\resource\txtbible\p3d
del /f /q cd\pc\art\frontend\scrooby\resource\*.p3d
rmdir /s /q cd\pc\art\frontend\scrooby\pages
rmdir /s /q cd\pc\art\frontend\scrooby\screens
del /f /q cd\pc\art\frontend\scrooby\*.prj
echo DONE cleaning Scrooby CD folder.
echo.

echo -----
echo ---------- Finished building Simpsons2 FrontEnd for PC ----------
echo -----

