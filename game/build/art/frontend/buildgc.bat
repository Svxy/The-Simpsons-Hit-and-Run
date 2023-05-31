@echo off

::::: GAMECUBE :::::::::::::::
:::

echo -----
echo ---------- Building Simpsons2 FrontEnd for GameCube ----------
echo -----

echo Checking out files from Perforce ... ...
P4 edit //depot/game/cd/gc/art/frontend/scrooby/...
echo.

echo Copying files to GameCube CD ... ...
rmdir /s /q cd\gc\art\frontend\scrooby
xcopy exportart\frontend\scrooby\*.* cd\gc\art\frontend\scrooby\ /D /S /Y /EXCLUDE:build\art\frontend\exclude.txt
xcopy exportart\frontend\scrooby\resource\images\_gc\*.* cd\gc\art\frontend\scrooby\resource\images\ /S /Y
echo DONE Copying files to GameCube CD.
echo.

echo [GC]: Processing pure3d files ... ...
cd cd\gc\art\frontend\scrooby\resource\pure3d
p3danim -C -O --quaternion-tolerance 0.1 *.p3d
p3dimage --ntsc_fix -n -O -r *.p3d
xcopy camset*.p3d temp\ /Y
convert2dxtn -f 1 *.p3d
xcopy temp\camset*.p3d . /Y
convert2dxtn -1 camset*.p3d
rmdir /s /q temp
p3dgc -n --ms --mg -r 1 -c 16 -u 16 *.p3d
p3ddel -n -d7000 *.p3d
cd ..\..\..\..\..\..\..
echo DONE processing pure3d files.
echo.

echo [GC]: Processing project resource files ... ...
cd cd\gc\art\frontend\scrooby\
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl language.prj a --srr2 -S -G -b8
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl bootup.prj a --srr2 -S -G -b8
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl backend.prj a --srr2 -S -G -b8
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl frontend.prj a --srr2 -S -G -b8
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl minigame.prj a --srr2 -S -G -b8
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl ingame.prj a --srr2 -S -G -b8
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl ingamel1.prj a --srr2 -S -G -b8
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl ingamel2.prj a --srr2 -S -G -b8
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl ingamel3.prj a --srr2 -S -G -b8
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl ingamel4.prj a --srr2 -S -G -b8
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl ingamel5.prj a --srr2 -S -G -b8
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl ingamel6.prj a --srr2 -S -G -b8
..\..\..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\..\..\build\art\frontend\prjextract.pl ingamel7.prj a --srr2 -S -G -b8
cd ..\..\..\..\..\
echo DONE processing project resource files.
echo.

echo [GC]: Joining project resource files ... ...

cd cd\gc\art\frontend\scrooby\resource\_language\
p3djoin -s -o ..\language.p3d *.p3d
cd ..\..\..\..\..\..\..

cd cd\gc\art\frontend\scrooby\resource\_bootup\
p3djoin -s -o ..\bootup.p3d *.p3d
cd ..\..\..\..\..\..\..

cd cd\gc\art\frontend\scrooby\resource\_backend\
p3djoin -s -o ..\backend.p3d *.p3d
cd ..\..\..\..\..\..\..

cd cd\gc\art\frontend\scrooby\resource\_frontend\
p3djoin -s -o ..\frontend.p3d *.p3d
cd ..\..\..\..\..\..\..

cd cd\gc\art\frontend\scrooby\resource\_minigame\
p3djoin -s -o ..\minigame.p3d *.p3d
cd ..\..\..\..\..\..\..

cd cd\gc\art\frontend\scrooby\resource\_ingame\
p3djoin -s -o ..\ingame.p3d *.p3d
cd ..\..\..\..\..\..\..

rmdir /s /q cd\gc\art\frontend\scrooby\resource\_language
rmdir /s /q cd\gc\art\frontend\scrooby\resource\_bootup
rmdir /s /q cd\gc\art\frontend\scrooby\resource\_backend
rmdir /s /q cd\gc\art\frontend\scrooby\resource\_frontend
rmdir /s /q cd\gc\art\frontend\scrooby\resource\_minigame
rmdir /s /q cd\gc\art\frontend\scrooby\resource\_ingame

p3djoin -o cd\gc\art\frontend\scrooby\language.p3d cd\gc\art\frontend\scrooby\resource\language.p3d cd\gc\art\frontend\scrooby\language.p3d
p3djoin -o cd\gc\art\frontend\scrooby\bootup.p3d cd\gc\art\frontend\scrooby\resource\bootup.p3d cd\gc\art\frontend\scrooby\bootup.p3d
p3djoin -o cd\gc\art\frontend\scrooby\backend.p3d cd\gc\art\frontend\scrooby\resource\backend.p3d cd\gc\art\frontend\scrooby\backend.p3d
p3djoin -o cd\gc\art\frontend\scrooby\frontend.p3d cd\gc\art\frontend\scrooby\resource\frontend.p3d cd\gc\art\frontend\scrooby\frontend.p3d
p3djoin -o cd\gc\art\frontend\scrooby\minigame.p3d cd\gc\art\frontend\scrooby\resource\minigame.p3d cd\gc\art\frontend\scrooby\minigame.p3d
p3djoin -o cd\gc\art\frontend\scrooby\ingame.p3d cd\gc\art\frontend\scrooby\resource\ingame.p3d cd\gc\art\frontend\scrooby\ingame.p3d

cd cd\gc\art\frontend\scrooby\resource\_ingamel1\
p3djoin -s -o ..\ingamel1.p3d *.p3d
cd ..\..\..\..\..\..\..

cd cd\gc\art\frontend\scrooby\resource\_ingamel2\
p3djoin -s -o ..\ingamel2.p3d *.p3d
cd ..\..\..\..\..\..\..

cd cd\gc\art\frontend\scrooby\resource\_ingamel3\
p3djoin -s -o ..\ingamel3.p3d *.p3d
cd ..\..\..\..\..\..\..

cd cd\gc\art\frontend\scrooby\resource\_ingamel4\
p3djoin -s -o ..\ingamel4.p3d *.p3d
cd ..\..\..\..\..\..\..

cd cd\gc\art\frontend\scrooby\resource\_ingamel5\
p3djoin -s -o ..\ingamel5.p3d *.p3d
cd ..\..\..\..\..\..\..

cd cd\gc\art\frontend\scrooby\resource\_ingamel6\
p3djoin -s -o ..\ingamel6.p3d *.p3d
cd ..\..\..\..\..\..\..

cd cd\gc\art\frontend\scrooby\resource\_ingamel7\
p3djoin -s -o ..\ingamel7.p3d *.p3d
cd ..\..\..\..\..\..\..

rmdir /s /q cd\gc\art\frontend\scrooby\resource\_ingamel1
rmdir /s /q cd\gc\art\frontend\scrooby\resource\_ingamel2
rmdir /s /q cd\gc\art\frontend\scrooby\resource\_ingamel3
rmdir /s /q cd\gc\art\frontend\scrooby\resource\_ingamel4
rmdir /s /q cd\gc\art\frontend\scrooby\resource\_ingamel5
rmdir /s /q cd\gc\art\frontend\scrooby\resource\_ingamel6
rmdir /s /q cd\gc\art\frontend\scrooby\resource\_ingamel7

p3djoin -o cd\gc\art\frontend\scrooby\ingamel1.p3d cd\gc\art\frontend\scrooby\resource\ingamel1.p3d cd\gc\art\frontend\scrooby\ingamel1.p3d
p3djoin -o cd\gc\art\frontend\scrooby\ingamel2.p3d cd\gc\art\frontend\scrooby\resource\ingamel2.p3d cd\gc\art\frontend\scrooby\ingamel2.p3d
p3djoin -o cd\gc\art\frontend\scrooby\ingamel3.p3d cd\gc\art\frontend\scrooby\resource\ingamel3.p3d cd\gc\art\frontend\scrooby\ingamel3.p3d
p3djoin -o cd\gc\art\frontend\scrooby\ingamel4.p3d cd\gc\art\frontend\scrooby\resource\ingamel4.p3d cd\gc\art\frontend\scrooby\ingamel4.p3d
p3djoin -o cd\gc\art\frontend\scrooby\ingamel5.p3d cd\gc\art\frontend\scrooby\resource\ingamel5.p3d cd\gc\art\frontend\scrooby\ingamel5.p3d
p3djoin -o cd\gc\art\frontend\scrooby\ingamel6.p3d cd\gc\art\frontend\scrooby\resource\ingamel6.p3d cd\gc\art\frontend\scrooby\ingamel6.p3d
p3djoin -o cd\gc\art\frontend\scrooby\ingamel7.p3d cd\gc\art\frontend\scrooby\resource\ingamel7.p3d cd\gc\art\frontend\scrooby\ingamel7.p3d

echo DONE joining project resource files.
echo.

echo [GC]: Finalizing Scrooby project files ... ...
cd cd\gc\art\frontend\scrooby
convert2dxtn -f 1 bootup.p3d
convert2dxtn -f 1 backend.p3d
convert2dxtn -1 frontend.p3d
convert2dxtn -f 1 ingame*.p3d
convert2dxtn -f 1 language.p3d
convert2dxtn -f 1 minigame.p3d
p3dgc -n --mg --ms -r 1 -c 16 -u 16 *.p3d
p3ddel -n -d7000 *.p3d
cd ..\..\..\..\..
echo DONE finalizing Scrooby project files.
echo.

echo [GC]: Cleaning Scrooby CD folder ... ...
rmdir /s /q cd\gc\art\frontend\scrooby\resource\fonts
rmdir /s /q cd\gc\art\frontend\scrooby\resource\images
xcopy cd\gc\art\frontend\scrooby\resource\txtbible\*.p3d cd\gc\art\frontend\scrooby\resource\txtbible\p3d\
del /f /q cd\gc\art\frontend\scrooby\resource\txtbible\*.*
copy cd\gc\art\frontend\scrooby\resource\txtbible\p3d\*.p3d cd\gc\art\frontend\scrooby\resource\txtbible
rmdir /s /q cd\gc\art\frontend\scrooby\resource\txtbible\p3d
del /f /q cd\gc\art\frontend\scrooby\resource\*.p3d
rmdir /s /q cd\gc\art\frontend\scrooby\pages
rmdir /s /q cd\gc\art\frontend\scrooby\screens
del /f /q cd\gc\art\frontend\scrooby\*.prj
echo DONE cleaning Scrooby CD folder.
echo.

echo -----
echo ---------- Finished building Simpsons2 FrontEnd for GameCube ----------
echo -----

