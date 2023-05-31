@echo off
rem ************************************************** Pre

rem D:\project\srr2\Art\Character-animations\chars\normal\%1\pure3d\animations
rem D:\project\srr2\game\build\art\tools

del /Q %1.p3d

rd /S /Q temp
mkdir temp

rem ************************************************** Animations

mkdir temp\animations
call %BUILDART%tools\rename_anim

FOR %%f IN (*.p3d) DO ..\..\..\..\..\..\..\game\libs\pure3d\tools\commandline\bin\p3djoin.exe -o temp\animations\%1.p3d temp\animations\%1.p3d %%f

echo Ah, found it.

..\..\..\..\..\..\..\game\libs\pure3d\tools\commandline\bin\p3ddel -k 121000 -o temp\animations\%1.p3d temp\animations\%1.p3d


rem ************************************************** Models

mkdir temp\models

xcopy /Q /Y ..\..\..\..\..\..\..\game\exportart\characters\%1\models\*.p3d temp\models\

FOR %%f IN (temp\models\*.p3d) DO ..\..\..\..\..\..\..\game\libs\pure3d\tools\commandline\bin\p3djoin.exe -o temp\models\%1.p3d temp\models\%1.p3d %%f

echo Ah, found it.

..\..\..\..\..\..\..\game\libs\pure3d\tools\commandline\bin\p3ddel -a * -o temp\models\%1.p3d temp\models\%1.p3d

rem ************************************************** Post

..\..\..\..\..\..\..\game\libs\pure3d\tools\commandline\bin\p3djoin.exe -o %1.p3d temp\models\%1.p3d temp\animations\%1.p3d

..\..\..\..\..\..\..\game\libs\pure3d\tools\commandline\bin\p3ddel -D -o %1.p3d %1.p3d 

..\..\..\..\..\..\..\game\libs\pure3d\tools\commandline\bin\p3ddel -d 7000 -o %1.p3d %1.p3d 

..\..\..\..\..\..\..\game\libs\pure3d\tools\commandline\bin\p3danim -o homer.p3d homer.p3d

call ..\..\..\..\..\..\..\game\build\art\tools\animmem %2 %3 %4 %1.p3d > %1.txt

notepad %1.txt
rd /S /Q temp