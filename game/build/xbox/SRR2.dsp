# Microsoft Developer Studio Project File - Name="SRR2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Application" 0x0b01

CFG=SRR2 - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SRR2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SRR2.mak" CFG="SRR2 - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SRR2 - Xbox Release" (based on "Xbox Application")
!MESSAGE "SRR2 - Xbox Debug" (based on "Xbox Application")
!MESSAGE "SRR2 - Xbox Tune" (based on "Xbox Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "SRR2"
# PROP Scc_LocalPath "..\.."
CPP=cl.exe

!IF  "$(CFG)" == "SRR2 - Xbox Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /W3 /GR /GX /Zi /O2 /I "..\..\radscript\inc" /I "..\..\code" /I "..\..\libs\radcore\inc" /I "..\..\libs\radmovie\inc" /I "..\..\libs\radsound\inc" /I "..\..\libs\radscript\inc" /I "..\..\libs\radmusic\inc" /I "..\..\libs\pure3d" /I "..\..\libs\scrooby\inc" /I "..\..\libs\scrooby\src" /I "..\..\libs\radmath" /I "..\..\libs\sim" /I "..\..\libs\poser\inc" /I "..\..\libs\choreo\inc" /I "..\..\libs\radcontent\inc" /I "..\..\libs\radcontent" /FI"pchsrr2.h" /D "NDEBUG" /D "RAD_RELEASE" /D "WIN32" /D "_XBOX" /D "RAD_XBOX" /D "RAD_CONSOLE" /Yu"pchsrr2.h" /G6 /Ztmp /c
# SUBTRACT CPP /Fr
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xnet.lib xboxkrnl.lib /nologo /machine:I386 /subsystem:xbox /fixed:no /debugtype:vc6 /OPT:REF
# ADD LINK32 xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xnet.lib xboxkrnl.lib xmv.lib /nologo /debug /machine:I386 /out:"Release/SRR2xr.exe" /subsystem:xbox /fixed:no /TMP /OPT:REF /OPT:ICF
# SUBTRACT LINK32 /pdb:none
XBE=imagebld.exe
# ADD BASE XBE /nologo /stack:0x10000
# ADD XBE /nologo /testname:"Simpsons2 - Release" /stack:0x10000 /debug /out:"Release/SRR2xr.xbe"
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=..\..\libs\radscript\tools\bin\ridl.exe -I..\..\libs\radcore\inc\radobject.hpp -I..\..\code\sound\soundrenderer\idasoundresource.h -I..\..\code\sound\soundrenderer\soundresource.h -I..\..\code\sound\avatar\icarsoundparameters.h -I..\..\code\sound\avatar\carsoundparameters.h -I..\..\code\sound\tuning\iglobalsettings.h -I..\..\code\sound\tuning\globalsettings.h -I..\..\code\sound\soundfx\ireverbsettings.h -I..\..\code\sound\soundfx\reverbsettings.h -I..\..\libs\radsound\inc\radsound.hpp -I..\..\libs\radsound\inc\radsound_hal.hpp -O..\..\cd\xbox\sound\typ\srrtypes.typ
PostBuild_Desc=Copying files to Xbox
PostBuild_Cmds=@echo Copying files to Xbox	release_to_xbox
# End Special Build Tool

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /W3 /Gm /GR /GX /Zi /Od /I "..\..\..\radcontent" /I "..\..\code" /I "..\..\libs\radcore\inc" /I "..\..\libs\radmovie\inc" /I "..\..\libs\radsound\inc" /I "..\..\libs\radscript\inc" /I "..\..\libs\radmusic\inc" /I "..\..\libs\pure3d" /I "..\..\libs\scrooby\inc" /I "..\..\libs\scrooby\src" /I "..\..\libs\radmath" /I "..\..\libs\sim" /I "..\..\libs\poser\inc" /I "..\..\libs\choreo\inc" /I "..\..\libs\radcontent\inc" /FI"pchsrr2.h" /D "_DEBUG" /D "RAD_DEBUG" /D "WIN32" /D "_XBOX" /D "RAD_XBOX" /D "RAD_CONSOLE" /D "DEBUGWATCH" /FR /Yu"pchsrr2.h" /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xnetd.lib xboxkrnl.lib /nologo /incremental:no /debug /machine:I386 /subsystem:xbox /fixed:no /debugtype:vc6
# ADD LINK32 xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xnetd.lib xboxkrnl.lib xmvd.lib /nologo /incremental:no /debug /machine:I386 /out:"Debug/SRR2xd.exe" /subsystem:xbox /fixed:no /tmp
# SUBTRACT LINK32 /pdb:none
XBE=imagebld.exe
# ADD BASE XBE /nologo /stack:0x10000 /debug
# ADD XBE /nologo /testname:"Simpsons2 - Debug" /stack:0x10000 /debug /out:"Debug/SRR2xd.xbe"
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=..\..\libs\radscript\tools\bin\ridl.exe -I..\..\libs\radcore\inc\radobject.hpp -I..\..\code\sound\soundrenderer\idasoundresource.h -I..\..\code\sound\soundrenderer\soundresource.h -I..\..\code\sound\avatar\icarsoundparameters.h -I..\..\code\sound\avatar\carsoundparameters.h -I..\..\code\sound\tuning\iglobalsettings.h -I..\..\code\sound\tuning\globalsettings.h -I..\..\code\sound\soundfx\ireverbsettings.h -I..\..\code\sound\soundfx\reverbsettings.h -I..\..\libs\radsound\inc\radsound.hpp -I..\..\libs\radsound\inc\radsound_hal.hpp -O..\..\cd\xbox\sound\typ\srrtypes.typ
PostBuild_Desc=Copying files to Xbox
PostBuild_Cmds=@echo Copying files to Xbox
# End Special Build Tool

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SRR2___Xbox_Tune"
# PROP BASE Intermediate_Dir "SRR2___Xbox_Tune"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Tune"
# PROP Intermediate_Dir "Tune"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GR /GX /Zi /Od /I "..\..\code" /I "..\..\libs\radcore\inc" /I "..\..\libs\pure3d" /I "..\..\libs\radmath" /FI"pch.h" /D "_DEBUG" /D "RAD_DEBUG" /D "WIN32" /D "_XBOX" /D "RAD_XBOX" /D "RAD_CONSOLE" /FR /Yu"pch.h" /FD /G6 /Zvc6 /c
# ADD CPP /nologo /MT /W3 /Gm /GR /GX /Zi /O2 /I "..\..\radscript\inc" /I "..\..\code" /I "..\..\libs\radcore\inc" /I "..\..\libs\radmovie\inc" /I "..\..\libs\radsound\inc" /I "..\..\libs\radscript\inc" /I "..\..\libs\radmusic\inc" /I "..\..\libs\pure3d" /I "..\..\libs\scrooby\inc" /I "..\..\libs\scrooby\src" /I "..\..\libs\radmath" /I "..\..\libs\sim" /I "..\..\libs\poser\inc" /I "..\..\libs\choreo\inc" /I "..\..\libs\radcontent\inc" /I "..\..\libs\radcontent" /FI"pchsrr2.h" /D "_DEBUG" /D "RAD_TUNE" /D "WIN32" /D "_XBOX" /D "RAD_XBOX" /D "RAD_CONSOLE" /D "DEBUGWATCH" /FR /Yu"pchsrr2.h" /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xnetd.lib xboxkrnl.lib /nologo /incremental:no /debug /machine:I386 /out:"Debug/SRR2d.exe" /subsystem:xbox /fixed:no /debugtype:vc6
# ADD LINK32 xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xnet.lib xboxkrnl.lib xmv.lib /nologo /incremental:no /debug /machine:I386 /out:"Tune/SRR2xt.exe" /subsystem:xbox /fixed:no /tmp
# SUBTRACT LINK32 /pdb:none
XBE=imagebld.exe
# ADD BASE XBE /nologo /testname:"SRR 2 - Debug" /stack:0x10000 /debug /out:"Debug/SRR2d.xbe"
# ADD XBE /nologo /testname:"Simpsons2 - Tune" /stack:0x10000 /debug /out:"Tune/SRR2xt.xbe"
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=..\..\libs\radscript\tools\bin\ridl.exe -I..\..\libs\radcore\inc\radobject.hpp -I..\..\code\sound\soundrenderer\idasoundresource.h -I..\..\code\sound\soundrenderer\soundresource.h -I..\..\code\sound\avatar\icarsoundparameters.h -I..\..\code\sound\avatar\carsoundparameters.h -I..\..\code\sound\tuning\iglobalsettings.h -I..\..\code\sound\tuning\globalsettings.h -I..\..\code\sound\soundfx\ireverbsettings.h -I..\..\code\sound\soundfx\reverbsettings.h -I..\..\libs\radsound\inc\radsound.hpp -I..\..\libs\radsound\inc\radsound_hal.hpp -O..\..\cd\xbox\sound\typ\srrtypes.typ
PostBuild_Desc=Copying files to Xbox
PostBuild_Cmds=@echo Copying files to Xbox	tune_to_xbox
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "SRR2 - Xbox Release"
# Name "SRR2 - Xbox Debug"
# Name "SRR2 - Xbox Tune"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Group "ai"

# PROP Default_Filter ""
# Begin Group "sequencer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\ai\sequencer\action.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\ai\sequencer\action.h
# End Source File
# Begin Source File

SOURCE=..\..\code\ai\sequencer\actioncontroller.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\ai\sequencer\actioncontroller.h
# End Source File
# Begin Source File

SOURCE=..\..\code\ai\sequencer\sequencer.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\ai\sequencer\sequencer.h
# End Source File
# Begin Source File

SOURCE=..\..\code\ai\sequencer\task.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\ai\sequencer\task.h
# End Source File
# End Group
# Begin Group "vehicle"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\ai\vehicle\chaseai.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\ai\vehicle\chaseai.h
# End Source File
# Begin Source File

SOURCE=..\..\code\ai\vehicle\potentialfield.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\ai\vehicle\potentialfield.h
# End Source File
# Begin Source File

SOURCE=..\..\code\ai\vehicle\potentials.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\ai\vehicle\potentials.h
# End Source File
# Begin Source File

SOURCE=..\..\code\ai\vehicle\trafficai.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\ai\vehicle\trafficai.h
# End Source File
# Begin Source File

SOURCE=..\..\code\ai\vehicle\vehicleai.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\ai\vehicle\vehicleai.h
# End Source File
# Begin Source File

SOURCE=..\..\code\ai\vehicle\waypointai.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\ai\vehicle\waypointai.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\code\ai\actionbuttonhandler.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\ai\actionbuttonhandler.h
# End Source File
# Begin Source File

SOURCE=..\..\code\ai\actionbuttonmanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\ai\actionbuttonmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\ai\actionlist.h
# End Source File
# Begin Source File

SOURCE=..\..\code\ai\actionnames.h
# End Source File
# Begin Source File

SOURCE=..\..\code\ai\aliencamera.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\ai\aliencamera.h
# End Source File
# Begin Source File

SOURCE=..\..\code\ai\automaticdoor.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\ai\automaticdoor.h
# End Source File
# Begin Source File

SOURCE=..\..\code\ai\playanimonce.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\ai\playanimonce.h
# End Source File
# Begin Source File

SOURCE=..\..\code\ai\state.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\ai\state.h
# End Source File
# Begin Source File

SOURCE=..\..\code\ai\statefactory.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\ai\statefactory.h
# End Source File
# Begin Source File

SOURCE=..\..\code\ai\statemanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\ai\statemanager.h
# End Source File
# End Group
# Begin Group "atc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\atc\atcloader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\atc\atcloader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\atc\atcmanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\atc\atcmanager.h
# End Source File
# End Group
# Begin Group "camera"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\camera\bumpercam.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\camera\bumpercam.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\bumpercamdata.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\burnoutcam.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\camera\burnoutcam.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\chasecam.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\camera\chasecam.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\chasecamdata.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\conversationcam.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\camera\conversationcam.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\conversationcamdata.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\debugcam.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\camera\debugcam.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\followcam.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\camera\followcam.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\followcamdata.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\followcamdatachunk.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\frustrumdrawable.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\icamerashaker.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\isupercamtarget.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\kullcam.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\camera\kullcam.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\railcam.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\camera\railcam.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\reversecam.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\camera\reversecam.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\sinecosshaker.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\camera\sinecosshaker.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\snapshotcam.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\camera\snapshotcam.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\staticcam.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\camera\staticcam.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\supercam.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\camera\supercam.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\supercamcentral.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\camera\supercamcentral.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\supercamcontroller.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\camera\supercamcontroller.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\supercammanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\camera\supercammanager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\surveillancecam.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\camera\surveillancecam.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\trackercam.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\camera\trackercam.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\trackercamdata.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\walkercam.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\camera\walkercam.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\walkercamdata.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\walkercamdatachunk.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\wrecklesscam.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\camera\wrecklesscam.h
# End Source File
# Begin Source File

SOURCE=..\..\code\camera\wrecklesseventlistener.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\camera\wrecklesseventlistener.h
# End Source File
# End Group
# Begin Group "cards"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\cards\bonuscard.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\cards\bonuscard.h
# End Source File
# Begin Source File

SOURCE=..\..\code\cards\card.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\cards\card.h
# End Source File
# Begin Source File

SOURCE=..\..\code\cards\cardgallery.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\cards/cardgallery.h
# End Source File
# Begin Source File

SOURCE=..\..\code\cards\cards.h
# End Source File
# Begin Source File

SOURCE=..\..\code\cards\cardsdb.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\cards\cardsdb.h
# End Source File
# Begin Source File

SOURCE=..\..\code\cards\collectorcard.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\cards\collectorcard.h
# End Source File
# End Group
# Begin Group "console"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\console\console.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\console\console.h
# End Source File
# Begin Source File

SOURCE=..\..\code\console\debugconsolecallback.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\console\debugconsolecallback.h
# End Source File
# Begin Source File

SOURCE=..\..\code\console\fbstricmp.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\console\fbstricmp.h
# End Source File
# End Group
# Begin Group "constants"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\constants\blobshadownames.h
# End Source File
# Begin Source File

SOURCE=..\..\code\constants\breakablesenum.h
# End Source File
# Begin Source File

SOURCE=..\..\code\constants\characterenum.h
# End Source File
# Begin Source File

SOURCE=..\..\code\constants\maxnpccharacters.h
# End Source File
# Begin Source File

SOURCE=..\..\code\constants\maxplayers.h
# End Source File
# Begin Source File

SOURCE=..\..\code\constants\movienames.h
# End Source File
# Begin Source File

SOURCE=..\..\code\constants\particleenum.h
# End Source File
# Begin Source File

SOURCE=..\..\code\constants\physprop.h
# End Source File
# Begin Source File

SOURCE=..\..\code\constants\srrchunks.h
# End Source File
# Begin Source File

SOURCE=..\..\code\constants\vehicleenum.h
# End Source File
# End Group
# Begin Group "contexts"

# PROP Default_Filter ""
# Begin Group "gameplay"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\contexts\gameplay\gameplaycontext.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\contexts\gameplay\gameplaycontext.h
# End Source File
# Begin Source File

SOURCE=..\..\code\contexts\gameplay\loadinggameplaycontext.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\contexts\gameplay\loadinggameplaycontext.h
# End Source File
# End Group
# Begin Group "demo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\contexts\demo\alldemo.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\contexts\demo\democontext.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\contexts\demo\democontext.h
# End Source File
# Begin Source File

SOURCE=..\..\code\contexts\demo\loadingdemocontext.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\contexts\demo\loadingdemocontext.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\code\contexts\bootupcontext.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\contexts\bootupcontext.h
# End Source File
# Begin Source File

SOURCE=..\..\code\contexts\context.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\contexts\context.h
# End Source File
# Begin Source File

SOURCE=..\..\code\contexts\contextenum.h
# End Source File
# Begin Source File

SOURCE=..\..\code\contexts\entrycontext.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\contexts\entrycontext.h
# End Source File
# Begin Source File

SOURCE=..\..\code\contexts\exitcontext.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\contexts\exitcontext.h
# End Source File
# Begin Source File

SOURCE=..\..\code\contexts\frontendcontext.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\contexts\frontendcontext.h
# End Source File
# Begin Source File

SOURCE=..\..\code\contexts\loadingcontext.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\contexts\loadingcontext.h
# End Source File
# Begin Source File

SOURCE=..\..\code\contexts\pausecontext.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\contexts\pausecontext.h
# End Source File
# Begin Source File

SOURCE=..\..\code\contexts\playingcontext.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\contexts\playingcontext.h
# End Source File
# End Group
# Begin Group "debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\debug\debuginfo.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\debug\debuginfo.h
# End Source File
# Begin Source File

SOURCE=..\..\code\debug\profiler.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\debug\profiler.h
# End Source File
# Begin Source File

SOURCE=..\..\code\debug\section.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\debug\section.h
# End Source File
# End Group
# Begin Group "events"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\events\eventdata.h
# End Source File
# Begin Source File

SOURCE=..\..\code\events\eventenum.h
# End Source File
# Begin Source File

SOURCE=..\..\code\events\eventlistener.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\events\eventlistener.h
# End Source File
# Begin Source File

SOURCE=..\..\code\events\eventmanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\events\eventmanager.h
# End Source File
# End Group
# Begin Group "font"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\font\defaultfont.h
# End Source File
# End Group
# Begin Group "gameflow"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\gameflow\gameflow.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\gameflow\gameflow.h
# End Source File
# End Group
# Begin Group "input"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\input\allinput.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\input\basedamper.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\input\basedamper.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\code\input\button.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\input\button.h
# End Source File
# Begin Source File

SOURCE=..\..\code\input\controller.h
# End Source File
# Begin Source File

SOURCE=..\..\code\input\inputmanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\input\inputmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\input\mappable.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\input\mappable.h
# End Source File
# Begin Source File

SOURCE=..\..\code\input\mapper.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\input\mapper.h
# End Source File
# Begin Source File

SOURCE=..\..\code\input\rumbleeffect.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\input\rumbleeffect.h
# End Source File
# Begin Source File

SOURCE=..\..\code\input\rumblexbox.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\input\steeringspring.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\input\steeringspring.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\code\input\usercontroller.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\input\usercontroller.h
# End Source File
# End Group
# Begin Group "interiors"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\interiors\gagdrawable.h
# End Source File
# Begin Source File

SOURCE=..\..\code\interiors\interiormanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\interiors\interiormanager.h
# End Source File
# End Group
# Begin Group "loading"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\loading\cameradataloader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\loading\cameradataloader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\loading\cementfilehandler.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\loading\cementfilehandler.h
# End Source File
# Begin Source File

SOURCE=..\..\code\loading\choreofilehandler.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\loading\choreofilehandler.h
# End Source File
# Begin Source File

SOURCE=..\..\code\loading\consolefilehandler.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\loading\consolefilehandler.h
# End Source File
# Begin Source File

SOURCE=..\..\code\loading\filehandler.h
# End Source File
# Begin Source File

SOURCE=..\..\code\loading\filehandlerenum.h
# End Source File
# Begin Source File

SOURCE=..\..\code\loading\filehandlerfactory.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\loading\filehandlerfactory.h
# End Source File
# Begin Source File

SOURCE=..\..\code\loading\intersectionloader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\loading\intersectionloader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\loading\loadingmanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\loading\loadingmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\loading\locatorloader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\loading\locatorloader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\loading\p3dfilehandler.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\loading\p3dfilehandler.h
# End Source File
# Begin Source File

SOURCE=..\..\code\loading\pathloader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\loading\pathloader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\loading\roaddatasegmentloader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\loading\roaddatasegmentloader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\loading\roadloader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\loading\roadloader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\loading\scroobyfilehandler.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\loading\scroobyfilehandler.h
# End Source File
# Begin Source File

SOURCE=..\..\code\loading\soundfilehandler.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\loading\soundfilehandler.h
# End Source File
# End Group
# Begin Group "main"

# PROP Default_Filter ""
# Begin Group "ps2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\main\ps2main.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\main\ps2platform.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\main\ps2platform.h

!IF  "$(CFG)" == "SRR2 - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "gc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\main\gcmain.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\main\gamecube_extras\gcmanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\main\gamecube_extras\gcmanager.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\code\main\gcplatform.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\main\gcplatform.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\code\main\gamecube_extras\license.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\code\main\gamecube_extras\screenshot.c

!IF  "$(CFG)" == "SRR2 - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\main\gamecube_extras\screenshot.h
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\code\main\commandlineoptions.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\main\commandlineoptions.h
# End Source File
# Begin Source File

SOURCE=..\..\code\main\game.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\main\game.h
# End Source File
# Begin Source File

SOURCE=..\..\code\main\pchsrr2.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

# ADD CPP /Yc"pchsrr2.h"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

# ADD CPP /Yc"pchsrr2.h"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

# ADD CPP /Yc"pchsrr2.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\main\pchsrr2.h
# End Source File
# Begin Source File

SOURCE=..\..\code\main\platform.h
# End Source File
# Begin Source File

SOURCE=..\..\code\main\singletons.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\main\singletons.h
# End Source File
# Begin Source File

SOURCE=..\..\code\main\xboxmain.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\main\xboxplatform.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\main\xboxplatform.h
# End Source File
# End Group
# Begin Group "memory"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\memory\allmemory.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\memory\allocpool.h
# End Source File
# Begin Source File

SOURCE=..\..\code\memory\leakdetection.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\memory\leakdetection.h
# End Source File
# Begin Source File

SOURCE=..\..\code\memory\map.h
# End Source File
# Begin Source File

SOURCE=..\..\code\memory\memorypool.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\memory\memorypool.h
# End Source File
# Begin Source File

SOURCE=..\..\code\memory\memorytracker.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\memory\memorytracker.h
# End Source File
# Begin Source File

SOURCE=..\..\code\memory\memoryutilities.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\memory\memoryutilities.h
# End Source File
# Begin Source File

SOURCE=..\..\code\memory\srrmemory.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\memory\srrmemory.h
# End Source File
# Begin Source File

SOURCE=..\..\code\memory\stlallocators.h
# End Source File
# End Group
# Begin Group "meta"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\meta\actioneventlocator.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\meta\actioneventlocator.h
# End Source File
# Begin Source File

SOURCE=..\..\code\meta\carstartlocator.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\meta\carstartlocator.h
# End Source File
# Begin Source File

SOURCE=..\..\code\meta\directionallocator.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\meta\directionallocator.h
# End Source File
# Begin Source File

SOURCE=..\..\code\meta\eventlocator.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\meta\eventlocator.h
# End Source File
# Begin Source File

SOURCE=..\..\code\meta\fovlocator.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\meta\fovlocator.h
# End Source File
# Begin Source File

SOURCE=..\..\code\meta\interiorentrancelocator.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\meta\interiorentrancelocator.h
# End Source File
# Begin Source File

SOURCE=..\..\code\meta\locator.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\meta\locator.h
# End Source File
# Begin Source File

SOURCE=..\..\code\meta\locatorevents.h
# End Source File
# Begin Source File

SOURCE=..\..\code\meta\locatortypes.h
# End Source File
# Begin Source File

SOURCE=..\..\code\meta\occlusionlocator.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\meta\occlusionlocator.h
# End Source File
# Begin Source File

SOURCE=..\..\code\meta\recttriggervolume.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\meta\recttriggervolume.h
# End Source File
# Begin Source File

SOURCE=..\..\code\meta\scriptlocator.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\meta\scriptlocator.h
# End Source File
# Begin Source File

SOURCE=..\..\code\meta\spheretriggervolume.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\meta\spheretriggervolume.h
# End Source File
# Begin Source File

SOURCE=..\..\code\meta\splinelocator.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\meta\splinelocator.h
# End Source File
# Begin Source File

SOURCE=..\..\code\meta\staticcamlocator.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\meta\staticcamlocator.h
# End Source File
# Begin Source File

SOURCE=..\..\code\meta\triggerlocator.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\meta\triggerlocator.h
# End Source File
# Begin Source File

SOURCE=..\..\code\meta\triggervolume.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\meta\triggervolume.h
# End Source File
# Begin Source File

SOURCE=..\..\code\meta\triggervolumetracker.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\meta\triggervolumetracker.h
# End Source File
# Begin Source File

SOURCE=..\..\code\meta\zoneeventlocator.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\meta\zoneeventlocator.h
# End Source File
# End Group
# Begin Group "mission"

# PROP Default_Filter "*.cpp; *.h"
# Begin Group "objectives"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\mission\objectives\collectdumpedobjective.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\objectives\collectdumpedobjective.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\objectives\collectibleobjective.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\objectives\collectibleobjective.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\objectives\deliveryobjective.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\objectives\deliveryobjective.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\objectives\destroyobjective.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\objectives\destroyobjective.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\objectives\dialogueobjective.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\objectives\dialogueobjective.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\objectives\followobjective.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\objectives\followobjective.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\objectives\getinobjective.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\objectives\getinobjective.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\objectives\gotoobjective.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\objectives\gotoobjective.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\objectives\loseobjective.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\objectives\loseobjective.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\objectives\missionobjective.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\objectives\missionobjective.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\objectives\raceobjective.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\objectives\raceobjective.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\objectives\talktoobjective.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\objectives\talktoobjective.h
# End Source File
# End Group
# Begin Group "conditions"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\mission\conditions\damagecondition.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\conditions\damagecondition.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\conditions\followcondition.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\conditions\followcondition.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\conditions\leaveinteriorcondition.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\conditions\leaveinteriorcondition.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\conditions\missioncondition.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\conditions\missioncondition.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\conditions\outofboundscondition.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\conditions\outofboundscondition.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\conditions\positioncondition.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\conditions\positioncondition.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\conditions\racecondition.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\conditions\racecondition.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\conditions\timeoutcondition.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\conditions\timeoutcondition.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\conditions\vehiclecondition.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\conditions\vehiclecondition.h
# End Source File
# End Group
# Begin Group "safezone"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\mission\safezone\safezone.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\safezone\safezone.h
# End Source File
# End Group
# Begin Group "charactersheet"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\mission\charactersheet\allcharactersheet.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\charactersheet\charactersheet.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\charactersheet\charactersheetmanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\charactersheet\charactersheetmanager.h
# End Source File
# End Group
# Begin Group "rewards"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\mission\rewards\reward.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\rewards\reward.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\rewards\rewardsmanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\rewards\rewardsmanager.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\code\mission\animatedicon.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\animatedicon.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\bonusmissioninfo.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\bonusmissioninfo.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\bonusobjective.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\gameplaydatahandler.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\gameplaydatahandler.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\gameplaymanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\gameplaymanager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\headtoheadmanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\headtoheadmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\headtoheadmission.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\headtoheadmission.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\mission.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\mission.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\missionmanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\missionmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\missionscriptloader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\missionscriptloader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\missionstage.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\missionstage.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\nocopbonusobjective.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\nocopbonusobjective.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\nodamagebonusobjective.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\nodamagebonusobjective.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\racepositionbonusobjective.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\racepositionbonusobjective.h
# End Source File
# Begin Source File

SOURCE=..\..\code\mission\timeremainbonusobjective.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\mission\timeremainbonusobjective.h
# End Source File
# End Group
# Begin Group "presentation"

# PROP Default_Filter "*.cpp; *.h"
# Begin Group "presevents"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\presentation\presevents\fmvevent.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\presevents\fmvevent.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\presevents\nisevent.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\presevents\nisevent.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\presevents\presentationevent.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\presevents\presentationevent.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\presevents\transevent.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\presevents\transevent.h
# End Source File
# End Group
# Begin Group "gui"

# PROP Default_Filter ""
# Begin Group "backend"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\presentation\gui\backend\allbackend.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\backend\guimanagerbackend.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\backend\guimanagerbackend.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\backend\guiscreendemo.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\backend\guiscreendemo.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\backend\guiscreenloading.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\backend\guiscreenloading.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\backend\guiscreenloadingfe.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\backend\guiscreenloadingfe.h
# End Source File
# End Group
# Begin Group "frontend"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guimanagerfrontend.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guimanagerfrontend.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guiscreencollectiblecards.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guiscreencollectiblecards.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guiscreencontroller.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guiscreencontroller.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guiscreenloadgame.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guiscreenloadgame.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guiscreenmainmenu.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guiscreenmainmenu.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guiscreenmultichoosechar.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guiscreenmultichoosechar.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guiscreenmultisetup.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guiscreenmultisetup.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guiscreenoptions.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guiscreenoptions.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guiscreenplaymovie.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guiscreenplaymovie.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guiscreensound.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guiscreensound.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guiscreensplash.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guiscreensplash.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guiscreenviewcard.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guiscreenviewcard.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guiscreenviewmovies.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\frontend\guiscreenviewmovies.h
# End Source File
# End Group
# Begin Group "ingame"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guimanageringame.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guimanageringame.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenhud.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenhud.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenhudmap.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenhudmap.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreeniriswipe.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreeniriswipe.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenletterbox.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenletterbox.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenmissionload.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenmissionload.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenmissionover.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenmissionover.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenmissionselect.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenmissionselect.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenmultihud.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenmultihud.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenpause.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenpause.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenpausemission.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenpausemission.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenpauseoptions.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenpauseoptions.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenpausesound.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenpausesound.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenpausesunday.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenpausesunday.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenphonebooth.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreenphonebooth.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreensavegame.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\ingame\guiscreensavegame.h
# End Source File
# End Group
# Begin Group "utility"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\presentation\gui\utility\hudmap.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\utility\hudmap.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\utility\numerictext.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\utility\numerictext.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\utility\scrollingtext.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\utility\scrollingtext.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\utility\slider.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\utility\slider.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\utility\specialfx.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\utility\specialfx.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\utility\teletypetext.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\utility\teletypetext.h
# End Source File
# End Group
# Begin Group "bootup"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\presentation\gui\bootup\guimanagerbootup.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\bootup\guimanagerbootup.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\bootup\guiscreenlicense.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\bootup\guiscreenlicense.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\code\presentation\gui\guientity.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\guientity.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\guimanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\guimanager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\guimenu.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\guimenu.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\guiscreen.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\guiscreen.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\guiscreenmemcardcheck.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\guiscreenmemcardcheck.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\guiscreenmemorycard.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\guiscreenmemorycard.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\guiscreenmessage.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\guiscreenmessage.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\guiscreenprompt.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\guiscreenprompt.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\guisystem.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\guisystem.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\guiuserinputhandler.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\guiuserinputhandler.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\guiwindow.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\gui\guiwindow.h
# End Source File
# End Group
# Begin Group "fmvplayer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\presentation\fmvplayer\fmvplayer.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\fmvplayer\fmvplayer.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\fmvplayer\fmvuserinputhandler.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\fmvplayer\fmvuserinputhandler.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\code\presentation\animplayer.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\animplayer.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\blinker.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\blinker.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\cameraplayer.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\cameraplayer.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\language.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\language.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\mouthflapper.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\mouthflapper.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\nisplayer.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\nisplayer.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\playerdrawable.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\playerdrawable.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\presentation.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\presentation.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\presentationanimator.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\presentationanimator.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\simpleanimationplayer.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\simpleanimationplayer.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\transitionplayer.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\transitionplayer.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\tutorialmanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\tutorialmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\presentation\tutorialmode.h
# End Source File
# End Group
# Begin Group "render"

# PROP Default_Filter ""
# Begin Group "RenderFlow"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\render\RenderFlow\renderflow.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\RenderFlow\renderflow.h
# End Source File
# End Group
# Begin Group "Culling"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\render\Culling\BlockCoord.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\Bounds.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\BoxPts.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\BoxPts.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\Cell.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\Cell.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\CellBlock.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\CellBlock.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\ContiguousBinNode.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\CoordSubList.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\CoordSubList.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\CullData.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\CullData.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\FixedArray.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\FloatFuncs.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\HexahedronP.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\HexahedronP.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\ISpatialProxy.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\ISpatialProxy.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\Matrix3f.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\NodeFLL.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\OctTreeConst.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\OctTreeData.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\OctTreeData.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\OctTreeNode.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\OctTreeNode.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\OctTreeParams.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\Plane3f.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\Point3f.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\ReserveArray.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\ScratchArray.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\SpatialNode.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\SpatialTree.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\SpatialTree.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\SpatialTreeFactory.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\SpatialTreeIter.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\SpatialTreeIter.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\SphereSP.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\SphereSP.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\srrRenderTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\SwapArray.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\UseArray.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\Vector3f.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\Vector3i.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\VectorLib.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\VectorLib.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\WorldScene.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Culling\WorldScene.h
# End Source File
# End Group
# Begin Group "DSG"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\render\DSG\animcollisionentitydsg.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\animcollisionentitydsg.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\animentitydsg.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\animentitydsg.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\breakableobjectdsg.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\breakableobjectdsg.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\collisionentitydsg.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\collisionentitydsg.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\DSGFactory.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\DSGFactory.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\DynaLoadListDSG.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\DynaPhysDSG.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\DynaPhysDSG.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\FenceEntityDSG.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\FenceEntityDSG.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\IEntityDSG.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\IEntityDSG.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\InstAnimDynaPhysDSG.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\InstAnimDynaPhysDSG.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\InstDynaPhysDSG.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\InstDynaPhysDSG.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\InstStatEntityDSG.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\InstStatEntityDSG.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\InstStatPhysDSG.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\InstStatPhysDSG.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\IntersectDSG.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\IntersectDSG.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\LensFlareDSG.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\LensFlareDSG.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\StaticEntityDSG.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\StaticEntityDSG.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\StaticPhysDSG.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\StaticPhysDSG.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\TriStripDSG.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\TriStripDSG.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\WorldSphereDSG.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\DSG\WorldSphereDSG.h
# End Source File
# End Group
# Begin Group "RenderManager"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\render\RenderManager\FrontEndRenderLayer.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\RenderManager\FrontEndRenderLayer.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\RenderManager\RenderLayer.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\RenderManager\RenderLayer.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\RenderManager\RenderManager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\RenderManager\RenderManager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\RenderManager\TriStripDSGManager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\RenderManager\TriStripDSGManager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\RenderManager\WorldRenderLayer.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\RenderManager\WorldRenderLayer.h
# End Source File
# End Group
# Begin Group "Enums"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\render\Enums\RenderEnums.h
# End Source File
# End Group
# Begin Group "Loaders"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\render\Loaders\AllWrappers.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\AllWrappers.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\AnimCollLoader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\AnimCollLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\AnimDSGLoader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\AnimDSGLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\AnimDynaPhysLoader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\AnimDynaPhysLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\BillboardWrappedLoader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\BillboardWrappedLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\breakableobjectloader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\breakableobjectloader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\ChunkListenerCallback.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\DynaPhysLoader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\DynaPhysLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\FenceLoader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\FenceLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\GeometryWrappedLoader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\GeometryWrappedLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\instparticlesystemloader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\instparticlesystemloader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\InstStatEntityLoader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\InstStatEntityLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\InstStatPhysLoader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\InstStatPhysLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\IntersectLoader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\IntersectLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\IWrappedLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\LensFlareLoader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\LensFlareLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\StaticEntityLoader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\StaticEntityLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\StaticPhysLoader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\StaticPhysLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\TreeDSGLoader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\TreeDSGLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\WorldSphereLoader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Loaders\WorldSphereLoader.h
# End Source File
# End Group
# Begin Group "IntersectManager"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\render\IntersectManager\IntersectManager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\IntersectManager\IntersectManager.h
# End Source File
# End Group
# Begin Group "particles"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\render\Particles\particlemanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Particles\particlemanager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Particles\particlesystemdsg.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Particles\particlesystemdsg.h
# End Source File
# Begin Source File

SOURCE=..\..\code\render\Particles\vehicleparticleemitter.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\Particles\vehicleparticleemitter.h
# End Source File
# End Group
# Begin Group "breakables"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\render\breakables\breakablesmanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\breakables\breakablesmanager.h
# End Source File
# End Group
# Begin Group "animentitydsgmanager"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\render\animentitydsgmanager\animentitydsgmanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\animentitydsgmanager\animentitydsgmanager.h
# End Source File
# End Group
# Begin Group "SkidMarks"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\render\SkidMarks\SkidMarkGenerator.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\render\SkidMarks\SkidMarkGenerator.h
# End Source File
# End Group
# End Group
# Begin Group "roads"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\roads\geometry.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\roads\geometry.h
# End Source File
# Begin Source File

SOURCE=..\..\code\roads\intersection.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\roads\intersection.h
# End Source File
# Begin Source File

SOURCE=..\..\code\roads\lane.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\roads\lane.h
# End Source File
# Begin Source File

SOURCE=..\..\code\roads\road.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\roads\road.h
# End Source File
# Begin Source File

SOURCE=..\..\code\roads\roadmanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\roads\roadmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\roads\roadrender.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\roads\roadrendertest.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\roads\roadrendertest.h
# End Source File
# Begin Source File

SOURCE=..\..\code\roads\roadsegment.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\roads\roadsegment.h
# End Source File
# Begin Source File

SOURCE=..\..\code\roads\roadsegmentdata.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\roads\roadsegmentdata.h
# End Source File
# Begin Source File

SOURCE=..\..\code\roads\trafficcontrol.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\roads\trafficcontrol.h
# End Source File
# End Group
# Begin Group "sound"

# PROP Default_Filter "*.cpp;*.h"
# Begin Group "soundrenderer"

# PROP Default_Filter "*.cpp;*.h"
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\dasoundgroup.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\dasoundplayer.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\fader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\fader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\idaplayermanager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\idasoundallocatedresource.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\idasounddebug.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\idasounddynaload.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\idasoundplayer.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\idasoundrenderingmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\idasoundresource.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\idasoundresourcemanager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\idasoundtuner.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\playermanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\playermanager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\soundallocatedresource.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\soundallocatedresource.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\sounddynaload.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\sounddynaload.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\soundplayer.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\soundrenderingmanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\soundrenderingmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\soundresource.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\soundresource.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\soundresourcemanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\soundresourcemanager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\soundsystem.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\soundtuner.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\soundtuner.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\wireplayers.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderer\wiresystem.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# End Group
# Begin Group "avatar"

# PROP Default_Filter "*.cpp;*.h"
# Begin Source File

SOURCE=..\..\code\sound\avatar\avatarsoundplayer.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\avatar\avatarsoundplayer.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\avatar\carsoundparameters.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\avatar\carsoundparameters.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\avatar\icarsoundparameters.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\avatar\soundavatar.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\avatar\soundavatar.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\avatar\vehiclesounddebugpage.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\avatar\vehiclesounddebugpage.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\avatar\vehiclesoundplayer.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\avatar\vehiclesoundplayer.h
# End Source File
# End Group
# Begin Group "music"

# PROP Default_Filter "*.cpp;*.h"
# Begin Source File

SOURCE=..\..\code\sound\music\musicplayer.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\music\musicplayer.h
# End Source File
# End Group
# Begin Group "soundfx"

# PROP Default_Filter "*.cpp;*.h"
# Begin Source File

SOURCE=..\..\code\sound\soundfx\ireverbsettings.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundfx\positionalsoundsettings.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundfx\positionalsoundsettings.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundfx\reverbcontroller.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundfx\reverbcontroller.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundfx\reverbsettings.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundfx\reverbsettings.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundfx\soundeffectplayer.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundfx\soundeffectplayer.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundfx\soundfxfrontendlogic.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundfx\soundfxfrontendlogic.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundfx\soundfxgameplaylogic.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundfx\soundfxgameplaylogic.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundfx\soundfxlogic.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundfx\soundfxlogic.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundfx\soundfxpauselogic.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundfx\soundfxpauselogic.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundfx\xboxreverbcontroller.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundfx\xboxreverbcontroller.h
# End Source File
# End Group
# Begin Group "tuning"

# PROP Default_Filter "*.cpp;*.h"
# Begin Source File

SOURCE=..\..\code\sound\tuning\globalsettings.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\tuning\globalsettings.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\tuning\iglobalsettings.h
# End Source File
# End Group
# Begin Group "dialog"

# PROP Default_Filter "*.cpp;*.h"
# Begin Source File

SOURCE=..\..\code\sound\dialog\conversation.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\dialog\conversation.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\dialog\conversationmatcher.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\dialog\conversationmatcher.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\dialog\dialogcoordinator.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\dialog\dialogcoordinator.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\dialog\dialogline.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\dialog\dialogline.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\dialog\dialoglist.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\dialog\dialoglist.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\dialog\dialogpriorityqueue.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\dialog\dialogpriorityqueue.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\dialog\dialogqueueelement.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\dialog\dialogqueueelement.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\dialog\dialogselectiongroup.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\dialog\dialogselectiongroup.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\dialog\playabledialog.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\dialog\playabledialog.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\dialog\selectabledialog.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\dialog\selectabledialog.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\dialog\selectabledialoglist.h
# End Source File
# End Group
# Begin Group "nis"

# PROP Default_Filter "*.cpp;*.h"
# Begin Source File

SOURCE=..\..\code\sound\nis\nissoundplayer.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\nis\nissoundplayer.h
# End Source File
# End Group
# Begin Group "sounddebug"

# PROP Default_Filter "*.cpp;*.h"
# Begin Source File

SOURCE=..\..\code\sound\sounddebug\sounddebugdisplay.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\sounddebug\sounddebugdisplay.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\sounddebug\sounddebugpage.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\sounddebug\sounddebugpage.h
# End Source File
# End Group
# Begin Group "aivehicle"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\sound\aivehicle\aivehiclesoundmanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\aivehicle\aivehiclesoundmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\aivehicle\aivehiclesoundplayer.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\aivehicle\aivehiclesoundplayer.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\code\sound\listener.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\listener.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\nisenum.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\positionalsoundplayer.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\positionalsoundplayer.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\simpsonssoundplayer.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\simpsonssoundplayer.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundcluster.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundcluster.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundclusternameenum.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundcollisiondata.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundloader.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundloader.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundmanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderercallback.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\sound\soundrenderercallback.h
# End Source File
# End Group
# Begin Group "worldsim"

# PROP Default_Filter ""
# Begin Group "redbrick"

# PROP Default_Filter ""
# Begin Group "vehiclecontroller"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\vehiclecontroller\aivehiclecontroller.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\vehiclecontroller\aivehiclecontroller.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\vehiclecontroller\humanvehiclecontroller.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\vehiclecontroller\humanvehiclecontroller.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\vehiclecontroller\vehiclecontroller.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\vehiclecontroller\vehiclecontroller.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\vehiclecontroller\vehiclemappable.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\vehiclecontroller\vehiclemappable.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\geometryvehicle.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\geometryvehicle.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\physicslocomotion.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\physicslocomotion.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\physicslocomotioncontrollerforces.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\redbrickcollisionsolveragent.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\redbrickcollisionsolveragent.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\rootmatrixdriver.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\rootmatrixdriver.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\suspensionjointdriver.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\suspensionjointdriver.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\trafficbodydrawable.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\trafficbodydrawable.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\trafficlocomotion.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\trafficlocomotion.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\vehicle.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\vehicle.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\vehicleeventlistener.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\vehicleeventlistener.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\vehicleinit.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\vehiclelocomotion.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\vehiclelocomotion.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\wheel.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\redbrick\wheel.h
# End Source File
# End Group
# Begin Group "character"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\worldsim\character\aicharactercontroller.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\character\aicharactercontroller.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\character\character.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\character\character.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\character\charactercontroller.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\character\charactercontroller.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\character\charactermanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\character\charactermanager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\character\charactermappable.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\character\charactermappable.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\character\characterrenderable.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\character\characterrenderable.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\character\charactertarget.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\character\charactertarget.h
# End Source File
# End Group
# Begin Group "traffic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\worldsim\traffic\trafficmanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\traffic\trafficmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\traffic\trafficvehicle.h
# End Source File
# End Group
# Begin Group "ped"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\worldsim\ped\pedestrian.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\ped\pedestrian.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\ped\pedestrianmanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\ped\pedestrianmanager.h
# End Source File
# End Group
# Begin Group "harass"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\worldsim\harass\chasemanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\harass\chasemanager.h
# End Source File
# End Group
# Begin Group "spawn"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\worldsim\spawn\spawnmanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\spawn\spawnmanager.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\code\worldsim\avatar.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\avatar.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\avatarmanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\avatarmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\groundplanepool.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\groundplanepool.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\physicsairef.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\vehiclecentral.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\vehiclecentral.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\worldcollisionsolveragent.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\worldcollisionsolveragent.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\worldobject.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\worldobject.h
# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\worldphysicsmanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\worldsim\worldphysicsmanager.h
# End Source File
# End Group
# Begin Group "data"

# PROP Default_Filter ""
# Begin Group "memcard"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\data\memcard\memorycardmanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\data\memcard\memorycardmanager.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\code\data\gamedata.h
# End Source File
# Begin Source File

SOURCE=..\..\code\data\gamedatamanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\data\gamedatamanager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\data\savegameinfo.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\data\savegameinfo.h
# End Source File
# End Group
# Begin Group "cheats"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\cheats\cheatinputhandler.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\cheats\cheatinputhandler.h
# End Source File
# Begin Source File

SOURCE=..\..\code\cheats\cheatinputs.h
# End Source File
# Begin Source File

SOURCE=..\..\code\cheats\cheatinputsystem.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\cheats\cheatinputsystem.h
# End Source File
# Begin Source File

SOURCE=..\..\code\cheats\cheats.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\cheats\cheats.h
# End Source File
# End Group
# Begin Group "pedpaths"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\pedpaths\path.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\pedpaths\path.h
# End Source File
# Begin Source File

SOURCE=..\..\code\pedpaths\pathmanager.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\pedpaths\pathmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\code\pedpaths\pathsegment.cpp

!IF  "$(CFG)" == "SRR2 - Xbox Release"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "SRR2 - Xbox Tune"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\code\pedpaths\pathsegment.h
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=..\..\libs\sdks\XBox\Bink\binkxbox.lib
# End Source File
# End Target
# End Project
