@if "%2" == "pal" (goto PALSTUFF) else (goto NTSCSTUFF)

:PALSTUFF
@set PAL=pal
@call setcountrycode eu
@goto START

:NTSCSTUFF
@call setcountrycode us

:START

del *%PAL%g%1.dlf
del *%PAL%g%1.dsf
del *%PAL%g%1.gcm

@echo Setting dvdroot to: ..\..\cd\gc

@setodenv dvdroot ..\..\cd\gc

if not exist srr2%PAL%g%1.elf (goto BUILDCLEAN) else (goto MAKEGCM)

:BUILDCLEAN
call build %2%1 allclean
call build %2%1

:MAKEGCM
odemrun srr2%PAL%g%1.elf
..\tools\makegcm.exe srr2%PAL%g%1.dlf srr2%PAL%g%1.gcm

@set PAL=