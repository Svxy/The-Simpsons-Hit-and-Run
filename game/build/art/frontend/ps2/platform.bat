::: FE - IMAGES - PS2 :::

@echo off

@set GOTODIR=%1
@set BACKDIR=%2

cd %GOTODIR%

p3ddeindex *.p3d
p3dshaderopt -s *.p3d
p3dps2 *.p3d
p3ddel -n -d7000 *.p3d

cd %BACKDIR%
