::: FE - IMAGES - PC :::

@echo off

@set GOTODIR=%1
@set BACKDIR=%2

cd %GOTODIR%

p3dimage --ntsc_fix -S -x -O *.png
del *.png

cd %BACKDIR%
