@echo off

echo Checking out files from Perforce ... ...
: P4 edit //depot/game/cd/gc/art/frontend/scrooby/...
P4 edit //depot/game/cd/ps2/art/frontend/scrooby/...
P4 edit //depot/game/cd/xbox/art/frontend/scrooby/...
echo.


::::: GAMECUBE :::::::::::::::
:::

echo Compressing Scrooby project files for GameCube ... ...
echo *** WARNING: Compression is currently not available for GameCube! ***
echo DONE compressing Scrooby project files for GameCube.
echo.

::::: PS2 :::::::::::::::
:::

echo Compressing Scrooby project files for PS2 ... ...
cd cd\ps2\art\frontend\scrooby
p3dcompress -n -f *.p3d
cd ..\..\..\..\..
echo DONE compressing Scrooby project files for PS2.
echo.

echo Compressing resource files for PS2 ... ...
cd cd\ps2\art\frontend\scrooby\resource\pure3d
p3dcompress -n -f *.p3d
cd ..\..\..\..\..\..\..
echo DONE compressing resource files for PS2.
echo.

::::: XBOX :::::::::::::::
:::

echo Compressing Scrooby project files for Xbox ... ...
cd cd\xbox\art\frontend\scrooby
p3dcompress -n -f *.p3d
cd ..\..\..\..\..
echo DONE compressing Scrooby project files for Xbox.
echo.

echo Compressing resource files for Xbox ... ...
cd cd\xbox\art\frontend\scrooby\resource\pure3d
p3dcompress -n -f *.p3d
cd ..\..\..\..\..\..\..
echo DONE compressing resource files for Xbox.
echo.

