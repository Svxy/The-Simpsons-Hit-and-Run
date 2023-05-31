::: FE - IMAGES - PS2 :::

@echo off

@set GOTODIR=%1
@set BACKDIR=%2

cd %GOTODIR%

p3dcompress -n -f *.p3d

cd %BACKDIR%
