@echo off

echo Checking out files from Perforce ... ...
P4 edit //depot/game/exportart/frontend/scrooby/resource/fonts/*.p3d
echo.

echo Creating pure3d fonts ... ...
cd exportart\frontend\scrooby\resource\fonts
p3dmakefont -u ..\txtbible\srr2.X -f -o font0_16.p3d -N boulder_16 -s32 boulder.ttf
p3dmakefont -u ..\txtbible\srr2.X -f -o font0_24.p3d -N boulder_24 -s48 boulder.ttf
p3dmakefont -u ..\txtbible\srr2.X -f -o font1_14.p3d -N Tt2001m__14 -s24 UNIVER01.TTF
p3dmakefont -u ..\txtbible\srr2.X -f -o font2_12.p3d -N swz721mi_12 -s24 swz721mi.ttf
p3dmakefont -u ..\txtbible\srr2.X -f -o font3_12.p3d -N swz721n_10 -s20 Tt2001m_.ttf
p3ddel -n -d7000 *.p3d
cd ..\..\..\..\..
echo DONE creating pure3d fonts.
echo.
