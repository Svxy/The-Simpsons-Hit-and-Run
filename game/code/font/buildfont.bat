echo ========================
echo Building baked in font
echo ========================
@..\..\libs\pure3d\tools\commandline\bin\p3dmakefont -o adlibn.p3d -f -n -u ..\..\exportart\frontend\scrooby\resource\txtbible\srr2.X -t 0 -s 20 adlibn.ttf
@attrib -r defaultfont.h
@..\..\build\tools\bin2h adlibn.p3d > defaultfont.h
@..\..\libs\pure3d\lib\perl\bin\perl ..\..\build\tools\sizeprint.pl defaultfont.h adlibn.p3d >> defaultfont.h
@attrib +r defaultfont.h
@del adlibn.p3d
