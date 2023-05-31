SET PS2DIR="..\..\Game\cd\PS2"
SET TEMPDISKDIR="d:\Burn\PS2Disk"
SET ISOPATH="d:\Burn\PS2ISO"
SET ISONAME="PS2"
SET LICENCE=".\licence.000"
SET MOVIESDIR=".\..\..\Game\ps2movies\real_movies"

ECHO//////////////////////////////////////////////////////
ECHO//MAKE PS2 DIrectory
ECHO//////////////////////////////////////////////////////
MD %TEMPDISKDIR%
ROBOCOPY %PS2DIR%\ %TEMPDISKDIR% /MIR /XD art /XD movies

ECHO//////////////////////////////////////////////////////
ECHO//CEMENT ART
ECHO//////////////////////////////////////////////////////
..\..\game\libs\radcore\bin\filecementer.exe %TEMPDISKDIR%\art.rcf /basedir %PS2DIR% /add art\*
RD /s /q %TEMPDISKDIR%\art\

ECHO//////////////////////////////////////////////////////
ECHO//COPY Movies
ECHO//////////////////////////////////////////////////////
ROBOCOPY %MOVIESDIR%\ %TEMPDISKDIR%\MOVIES /MIR

ECHO/////////////////////////////////////////////////////
ECHO//STRIP ExE
ECHO/////////////////////////////////////////////////////
DEL %TEMPDISKDIR%\slps_123.45
RENAME %TEMPDISKDIR%\srr2pr.elf slps_123.45
CALL ee-strip %TEMPDISKDIR%\slps_123.45

DEL %TEMPDISKDIR%\*.elf

