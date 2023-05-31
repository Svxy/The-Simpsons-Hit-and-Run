@echo off

echo.
echo Checking out files from Perforce ... ...
P4 edit //depot/game/cd/gc/art/frontend/scrooby/...
P4 edit //depot/game/cd/ps2/art/frontend/scrooby/...
P4 edit //depot/game/cd/xbox/art/frontend/scrooby/...
echo.

::::: GAMECUBE :::::::::::::::
:::

echo Copying files to GameCube CD ... ...
del /f /q cd\gc\art\frontend\scrooby\*.p3d
xcopy exportart\frontend\scrooby\*.* cd\gc\art\frontend\scrooby\ /D /S /Y /EXCLUDE:build\art\frontend\exclude.txt+build\art\frontend\excludeq.txt
xcopy exportart\frontend\scrooby\resource\images\_gc\*.* cd\gc\art\frontend\scrooby\resource\images\ /S /Y
echo DONE Copying files to GameCube CD
echo .

::::: PS2 :::::::::::::::
:::

echo Copying files to PS2 CD ... ...
del /f /q cd\ps2\art\frontend\scrooby\*.p3d
xcopy exportart\frontend\scrooby\*.* cd\ps2\art\frontend\scrooby\ /D /S /Y /EXCLUDE:build\art\frontend\exclude.txt+build\art\frontend\excludeq.txt
xcopy exportart\frontend\scrooby\resource\images\_ps2\*.* cd\ps2\art\frontend\scrooby\resource\images\ /S /Y
echo DONE Copying files to PS2 CD
echo.

::::: XBOX :::::::::::::::
:::

echo Copying files to Xbox CD ... ...
del /f /q cd\xbox\art\frontend\scrooby\*.p3d
xcopy exportart\frontend\scrooby\*.* cd\xbox\art\frontend\scrooby\ /D /S /Y /EXCLUDE:build\art\frontend\exclude.txt+build\art\frontend\excludeq.txt
xcopy exportart\frontend\scrooby\resource\images\_xbox\*.* cd\xbox\art\frontend\scrooby\resource\images\ /S /Y
echo DONE Copying files to Xbox CD
echo.

