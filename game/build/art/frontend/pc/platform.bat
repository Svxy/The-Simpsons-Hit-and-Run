::: FE - IMAGES - PC :::

@echo off

@set GOTODIR=%1
@set BACKDIR=%2

cd %GOTODIR%

p3ddel -n -d7000 *.p3d

cd %BACKDIR%
