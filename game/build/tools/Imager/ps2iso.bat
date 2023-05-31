SET PS2DIR="..\..\..\cd\PS2"
SET ISOPATH="d:\Burn\PS2ISO"
SET ISONAME="PS2"
SET LICENCE=".\licence.000"

MD %ISOPATH%
DEL /q %ISOPATH%\%ISONAME%.iso 
DEL /q %ISOPATH%\%ISONAME%.cue 

COPY %PS2Dir%\srr2pr.elf %PS2Dir%\slps_123.45
CALL ee-strip %PS2Dir%\slps_123.45


REM DEL /q %PS2Dir%\srr2pt.elf 
REM DEL /q %PS2Dir%\srr2pd.elf 
REM DEL /q %PS2Dir%\srr2pr.elf 
REM DEL /q %PS2Dir%\srr2fpt.elf

MOVE %PS2Dir%\srr2pt.elf %PS2Dir%\..
MOVE %PS2Dir%\srr2pr.elf %PS2Dir%\..
MOVE %PS2Dir%\srr2pd.elf %PS2Dir%\..
MOVE %PS2Dir%\srr2fpt.elf %PS2Dir%\..

CALL mkps2iso.exe -nolimit %PS2DIR% %ISOPATH%\\%ISONAME%.iso %LICENCE%

MOVE %PS2Dir%\..\srr2pt.elf  %PS2Dir%\
MOVE %PS2Dir%\..\srr2pr.elf  %PS2Dir%\
MOVE %PS2Dir%\..\srr2pd.elf  %PS2Dir%\
MOVE %PS2Dir%\..\srr2fpt.elf  %PS2Dir%\
