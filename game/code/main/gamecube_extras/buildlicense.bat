..\..\..\libs\pure3d\tools\commandline\bin\p3dimage -b 4 -c -o licensee.p3d ..\..\..\exportart\frontend\scrooby\resource\images\_gc\license.png
@rem..\..\..\libs\pure3d\tools\commandline\bin\convert2dxtn -f 1 licensee.p3d
..\..\..\libs\pure3d\tools\commandline\bin\p3dgc licensee.p3d
attrib -r license.h
..\..\..\build\tools\bin2h licensee.p3d > license.h
..\..\..\libs\pure3d\lib\perl\bin\perl ..\..\..\build\tools\sizeprint.pl license.h license.png >> license.h 
attrib +r license.h
rem @del licensee.p3d
