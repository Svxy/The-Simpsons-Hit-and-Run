SET PS2DIR="..\..\..\cd\PS2"
SET TEMPDISKDIR="d:\Burn\PS2Disk"
SET ISOPATH="d:\Burn\PS2ISO"
SET ISONAME="NTSC"
SET EXENAME="srr2pr.elf"
SET LICENCE="d:\burn\imager\dvdlicence.000"

MD %TEMPDISKDIR%
REM ROBOCOPY %PS2DIR%\ %TEMPDISKDIR% /MIR /XD art /XF art.rcf /XF *.elf
ROBOCOPY %PS2DIR%\ %TEMPDISKDIR% ambience.rcf carsound.rcf dialog.rcf music.rcf nis.rcf scripts.rcf soundfx.rcf system.cnf srr2.map
ROBOCOPY %PS2DIR%\irx\ %TEMPDISKDIR%\irx /MIR

DEL %TEMPDISKDIR%\art.rcf
SET CEMENT="..\..\..\libs\radcore\bin\filecementer.exe"

%CEMENT% %TEMPDISKDIR%\art.rcf /basedir %PS2DIR% /add delete.ico /add copy.ico /add list.ico /add snapshot.p3d /add art\* /add scripts\* /add sound\* /add movies\* 

MD %ISOPATH%
DEL /q %ISOPATH%\%ISONAME%.iso 
DEL /q %ISOPATH%\%ISONAME%.cue 

DEL %TEMPDISKDIR%\SLUS_206.24
DEL %TEMPDISKDIR%\*.elf
COPY %PS2DIR%\%EXENAME% %TEMPDISKDIR%
RENAME %TEMPDISKDIR%\%EXENAME% SLUS_206.24
CALL ee-strip %TEMPDISKDIR%\SLUS_206.24

CALL mkps2iso.exe -nolimit -dvd %TEMPDISKDIR% %ISOPATH%\\%ISONAME%.iso %LICENCE%