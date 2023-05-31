::: FE - IMAGES - XBOX :::

@echo off

@set GOTODIR=%1
@set BACKDIR=%2

cd %GOTODIR%

p3dxbox *.p3d
p3ddel -n -d7000 *.p3d

cd %BACKDIR%
