@echo off

echo.
echo Checking out files from Perforce ... ...
P4 edit //depot/game/cd/gc/art/frontend/...
P4 edit //depot/game/cd/ps2/art/frontend/...
P4 edit //depot/game/cd/xbox/art/frontend/...
P4 edit //depot/game/cd/pc/art/frontend/...
echo.

::::: GAMECUBE :::::::::::::::
:::

echo Cleaning frontend folder in GameCube CD ... ...
rmdir /s /q cd\gc\art\frontend
mkdir cd\gc\art\frontend
echo.


::::: PS2 :::::::::::::::
:::

echo Cleaning frontend folder in PS2 CD ... ...
rmdir /s /q cd\ps2\art\frontend
mkdir cd\ps2\art\frontend
echo.


::::: XBOX :::::::::::::::
:::

echo Cleaning frontend folder in Xbox CD ... ...
rmdir /s /q cd\xbox\art\frontend
mkdir cd\xbox\art\frontend
echo.

::::: PC :::::::::::::::
:::

echo Cleaning frontend folder in PC CD ... ...
rmdir /s /q cd\pc\art\frontend
mkdir cd\pc\art\frontend
echo.

