@echo off

echo Checking out files from Perforce ... ...
P4 edit //depot/game/exportart/frontend/scrooby/resource/txtbible/srr2.h
P4 edit //depot/game/exportart/frontend/scrooby/resource/txtbible/srr2.p3d
P4 edit //depot/game/exportart/frontend/scrooby/resource/txtbible/srr2.E
P4 edit //depot/game/exportart/frontend/scrooby/resource/txtbible/srr2.F
P4 edit //depot/game/exportart/frontend/scrooby/resource/txtbible/srr2.G
P4 edit //depot/game/exportart/frontend/scrooby/resource/txtbible/srr2.I
P4 edit //depot/game/exportart/frontend/scrooby/resource/txtbible/srr2.S
P4 edit //depot/game/exportart/frontend/scrooby/resource/txtbible/srr2.X
P4 edit //depot/game/exportart/frontend/scrooby/_pc/resource/txtbible/srr2.h
P4 edit //depot/game/exportart/frontend/scrooby/_pc/resource/txtbible/srr2.p3d
P4 edit //depot/game/exportart/frontend/scrooby/_pc/resource/txtbible/srr2.E
P4 edit //depot/game/exportart/frontend/scrooby/_pc/resource/txtbible/srr2.F
P4 edit //depot/game/exportart/frontend/scrooby/_pc/resource/txtbible/srr2.G
P4 edit //depot/game/exportart/frontend/scrooby/_pc/resource/txtbible/srr2.I
P4 edit //depot/game/exportart/frontend/scrooby/_pc/resource/txtbible/srr2.S
P4 edit //depot/game/exportart/frontend/scrooby/_pc/resource/txtbible/srr2.X
P4 edit //depot/game/cd/gc/art/frontend/scrooby/resource/txtbible/...
P4 edit //depot/game/cd/ps2/art/frontend/scrooby/resource/txtbible/...
P4 edit //depot/game/cd/xbox/art/frontend/scrooby/resource/txtbible/...
P4 edit //depot/game/cd/pc/art/frontend/scrooby/resource/txtbible/...
echo .

echo Compiling Scrooby text bible ... ...
libs\scrooby\tools\bin\textbiblecompiler /m:2990119 /u /f:exportart\frontend\scrooby\resource\txtbible\srr2.xls /o:exportart\frontend\scrooby\resource\txtbible\srr2
libs\scrooby\tools\bin\textbiblecompiler /m:2990119 /u /f:exportart\frontend\scrooby\_pc\resource\txtbible\srr2.xls /o:exportart\frontend\scrooby\_pc\resource\txtbible\srr2

del /f /q exportart\frontend\scrooby\resource\txtbible\*.X
build\tools\mksnt\cat exportart\frontend\scrooby\resource\txtbible\srr2.E >> exportart\frontend\scrooby\resource\txtbible\srr2.X
build\tools\mksnt\cat exportart\frontend\scrooby\resource\txtbible\srr2.F >> exportart\frontend\scrooby\resource\txtbible\srr2.X
build\tools\mksnt\cat exportart\frontend\scrooby\resource\txtbible\srr2.G >> exportart\frontend\scrooby\resource\txtbible\srr2.X
build\tools\mksnt\cat exportart\frontend\scrooby\resource\txtbible\srr2.I >> exportart\frontend\scrooby\resource\txtbible\srr2.X
build\tools\mksnt\cat exportart\frontend\scrooby\resource\txtbible\srr2.S >> exportart\frontend\scrooby\resource\txtbible\srr2.X
build\tools\mksnt\cat build\art\frontend\nullchar.txt >> exportart\frontend\scrooby\resource\txtbible\srr2.X
del /f /q exportart\frontend\scrooby\_pc\resource\txtbible\*.X
build\tools\mksnt\cat exportart\frontend\scrooby\_pc\resource\txtbible\srr2.E >> exportart\frontend\scrooby\_pc\resource\txtbible\srr2.X
build\tools\mksnt\cat exportart\frontend\scrooby\_pc\resource\txtbible\srr2.F >> exportart\frontend\scrooby\_pc\resource\txtbible\srr2.X
build\tools\mksnt\cat exportart\frontend\scrooby\_pc\resource\txtbible\srr2.G >> exportart\frontend\scrooby\_pc\resource\txtbible\srr2.X
build\tools\mksnt\cat exportart\frontend\scrooby\_pc\resource\txtbible\srr2.I >> exportart\frontend\scrooby\_pc\resource\txtbible\srr2.X
build\tools\mksnt\cat exportart\frontend\scrooby\_pc\resource\txtbible\srr2.S >> exportart\frontend\scrooby\_pc\resource\txtbible\srr2.X
build\tools\mksnt\cat build\art\frontend\nullchar.txt >> exportart\frontend\scrooby\_pc\resource\txtbible\srr2.X
echo .

echo Copying text bible files to CD folders ... ...
xcopy exportart\frontend\scrooby\resource\txtbible\*.p3d cd\gc\art\frontend\scrooby\resource\txtbible\ /D /S /Y
xcopy exportart\frontend\scrooby\resource\txtbible\*.p3d cd\ps2\art\frontend\scrooby\resource\txtbible\ /D /S /Y
xcopy exportart\frontend\scrooby\resource\txtbible\*.p3d cd\xbox\art\frontend\scrooby\resource\txtbible\ /D /S /Y
xcopy exportart\frontend\scrooby\_pc\resource\txtbible\*.p3d cd\pc\art\frontend\scrooby\resource\txtbible\ /D /S /Y
echo DONE Copying text bible files to CD folders
echo .

echo Compressing text bible for PS2 and XBox ... ...
cd cd\ps2\art\frontend\scrooby\resource\txtbible
p3dcompress -n -f *.p3d
cd ..\..\..\..\..\..\..
cd cd\xbox\art\frontend\scrooby\resource\txtbible
p3dcompress -n -f *.p3d
cd ..\..\..\..\..\..\..
echo DONE compressing text bible for PS2 and Xbox.
echo.

