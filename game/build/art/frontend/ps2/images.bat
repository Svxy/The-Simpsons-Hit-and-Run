::: FE - IMAGES - PS2 :::

@echo off

@set GOTODIR=%1
@set BACKDIR=%2

cd %GOTODIR%

p3dimage --ntsc_fix -S -p -b8 *.png
del *.png

cd %BACKDIR%
