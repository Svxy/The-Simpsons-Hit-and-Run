::: FE - IMAGES - GAMECUBE :::

@echo off

@set GOTODIR=%1
@set BACKDIR=%2

cd %GOTODIR%

convert2dxtn -f 1 *.p3d
p3dgc --ms --mg -r 1 -c 16 -u 16 *.p3d
p3ddel -n -d7000 *.p3d

cd %BACKDIR%
