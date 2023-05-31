SET PS2DIR="..\..\..\cd\PS2"
SET TEMPDISKDIR="d:\Burn\PS2Disk"
SET ISOPATH="d:\Burn\PS2ISO"
SET ISONAME="PS2"
SET LICENCE=".\licence.000"
SET MOVIESDIR="..\..\..\ps2movies\placeholder_movies"

ECHO//////////////////////////////////////////////////////
ECHO//MAKE PS2 DIrectory
ECHO//////////////////////////////////////////////////////
MD %TEMPDISKDIR%
ROBOCOPY %PS2DIR%\ %TEMPDISKDIR% /MIR /XD art /XF art.rcf /XF *.elf

ECHO//////////////////////////////////////////////////////
ECHO//CEMENT ART
ECHO//////////////////////////////////////////////////////
DEL %TEMPDISKDIR%\art.rcf
..\..\..\libs\radcore\bin\filecementer.exe %TEMPDISKDIR%\art.rcf /basedir %PS2DIR% /add art\*

ECHO/////////////////////////////////////////////////////
ECHO//MAKE ISO IMAGE
ECHO/////////////////////////////////////////////////////
MD %ISOPATH%
DEL /q %ISOPATH%\%ISONAME%.iso 
DEL /q %ISOPATH%\%ISONAME%.cue 

DEL %TEMPDISKDIR%\slps_123.45
DEL %TEMPDISKDIR%\*.elf
COPY %PS2DIR%\srr2pr.elf %TEMPDISKDIR%
RENAME %TEMPDISKDIR%\srr2pr.elf slps_123.45
CALL ee-strip %TEMPDISKDIR%\slps_123.45

CALL mkps2iso.exe -nolimit %TEMPDISKDIR% %ISOPATH%\\%ISONAME%.iso %LICENCE%