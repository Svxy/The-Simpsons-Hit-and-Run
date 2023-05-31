# Microsoft Developer Studio Project File - Name="worldbuilder" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=worldbuilder - Win32 Tools Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "worldbuilder.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "worldbuilder.mak" CFG="worldbuilder - Win32 Tools Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "worldbuilder - Win32 Tools Debug" (based on "Win32 (x86) Application")
!MESSAGE "worldbuilder - Win32 Tools Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "worldbuilder"
# PROP Scc_LocalPath "."
CPP=snCl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "worldbuilder___Win32_Tools_Debug"
# PROP BASE Intermediate_Dir "worldbuilder___Win32_Tools_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /I "C:\AW\Maya4.0\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /YX"precompiled\\" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GR /GX /ZI /Od /I "..\..\..\game\libs" /I "..\..\..\game\libs\pure3d\sdks\Maya4_0\include\\" /I "..\code\\" /I "..\..\..\game\libs\pure3d\toollib\inc" /I "..\..\..\game\libs\pure3d\toollib\chunks16\inc" /I "." /I "..\..\..\game\libs\pure3d\constants" /I "..\..\globalcode" /I "..\..\..\game\libs\radmath" /I "..\..\..\game\libs\pure3d\\" /I "..\..\..\game\libs\radcore\inc" /I "..\..\..\game\code\constants" /I "..\..\..\game\code" /I "..\..\..\game\libs\radcontent\inc" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /D "RAD_PC" /D "RAD_WIN32" /D "RAD_DEBUG" /D "WORLD_BUILDER" /FR /YX"precompiled\PCH.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x1009 /d "_DEBUG" /d "_AFXDLL"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 Foundation.lib OpenGL32.lib OpenMaya.lib OpenMayaFX.lib OpenMayaRender.lib OpenMayaUI.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug\myCmd.mll" /pdbtype:sept /libpath:"C:\AW\Maya4.0\lib" /export:initializePlugin /export:uninitializePlugin
# ADD LINK32 dxguid.lib user32.lib gdi32.lib glu32.lib version.lib Foundation.lib OpenGL32.lib OpenMaya.lib OpenMayaFX.lib OpenMayaRender.lib OpenMayaUI.lib OpenMayaAnim.lib WS2_32.LIB /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug\worldbuilder.mll" /pdbtype:sept /libpath:"..\..\..\game\libs\pure3d\sdks\Maya4_0\lib" /libpath:"..\..\..\game\libs\pure3d\sdks\dx8\lib" /export:initializePlugin /export:uninitializePlugin
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copy worldbuilder.mll to Distribution and Maya dir.
PostBuild_Cmds=copy .\Debug\worldbuilder.mll ..\..\MayaTools\Maya4.0\bin\plug-ins\worldbuilder.mll	copy .\Debug\worldbuilder.mll C:\AW\Maya4.0\bin\plug-ins\worldbuilderd.mll	copy .\Debug\worldbuilder.mll C:\AW\Maya4.0\bin\plug-ins\worldbuilder.mll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "worldbuilder___Win32_Tools_Release"
# PROP BASE Intermediate_Dir "worldbuilder___Win32_Tools_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "." /I "C:\AW\Maya4.0\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /Zi /O2 /I "..\..\..\game\libs" /I "..\..\..\game\libs\pure3d\sdks\Maya4_0\include\\" /I "..\code\\" /I "..\..\..\game\libs\pure3d\toollib\inc" /I "..\..\..\game\libs\pure3d\toollib\chunks16\inc" /I "." /I "..\..\..\game\libs\pure3d\constants" /I "..\..\globalcode" /I "..\..\..\game\libs\radmath" /I "..\..\..\game\libs\pure3d\\" /I "..\..\..\game\libs\radcore\inc" /I "..\..\..\game\code\constants" /I "..\..\..\game\code" /I "..\..\..\game\libs\radcontent\inc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /D "RAD_PC" /D "RAD_WIN32" /D "RAD_RELEASE" /D "WORLD_BUILDER" /FAs /YX"precompiled\PCH.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x1009 /d "NDEBUG" /d "_AFXDLL"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 Foundation.lib OpenGL32.lib OpenMaya.lib OpenMayaFX.lib OpenMayaRender.lib OpenMayaUI.lib /nologo /subsystem:windows /dll /machine:I386 /out:"Release\myCmd.mll" /libpath:"C:\AW\Maya4.0\lib" /export:initializePlugin /export:uninitializePlugin
# ADD LINK32 dxguid.lib user32.lib gdi32.lib glu32.lib version.lib Foundation.lib OpenGL32.lib OpenMaya.lib OpenMayaFX.lib OpenMayaRender.lib OpenMayaUI.lib OpenMayaAnim.lib WS2_32.LIB /nologo /subsystem:windows /dll /machine:I386 /out:"Release\worldbuilder.mll" /libpath:"..\..\..\game\libs\pure3d\sdks\Maya4_0\lib" /libpath:"..\..\..\game\libs\pure3d\sdks\dx8\lib" /export:initializePlugin /export:uninitializePlugin
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copy worldbuilder.mll to Distribution and Maya dir.
PostBuild_Cmds=copy .\Release\worldbuilder.mll ..\..\MayaTools\Maya4.0\bin\plug-ins\worldbuilder.mll	copy .\Release\worldbuilder.mll C:\AW\Maya4.0\bin\plug-ins\worldbuilderr.mll	copy .\Release\worldbuilder.mll C:\AW\Maya4.0\bin\plug-ins\worldbuilder.mll
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "worldbuilder - Win32 Tools Debug"
# Name "worldbuilder - Win32 Tools Release"
# Begin Group "main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\code\main\constants.h
# End Source File
# Begin Source File

SOURCE=..\code\main\pluginMain.cpp
# End Source File
# Begin Source File

SOURCE=..\code\main\pluginMain.h
# End Source File
# Begin Source File

SOURCE=..\code\main\toolhack.h
# End Source File
# Begin Source File

SOURCE=..\code\main\worldbuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\code\main\worldbuilder.h
# End Source File
# End Group
# Begin Group "commands"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\code\commands\export.cpp
# End Source File
# Begin Source File

SOURCE=..\code\commands\export.h
# End Source File
# Begin Source File

SOURCE=..\code\commands\worldbuildercommands.cpp
# End Source File
# Begin Source File

SOURCE=..\code\commands\worldbuildercommands.h
# End Source File
# End Group
# Begin Group "nodes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\code\nodes\actioneventlocatornode.cpp
# End Source File
# Begin Source File

SOURCE=..\code\nodes\actioneventlocatornode.h
# End Source File
# Begin Source File

SOURCE=..\code\nodes\breakablecameralocatornode.cpp
# End Source File
# Begin Source File

SOURCE=..\code\nodes\breakablecameralocatornode.h
# End Source File
# Begin Source File

SOURCE=..\code\nodes\carstartlocatornode.cpp
# End Source File
# Begin Source File

SOURCE=..\code\nodes\carstartlocatornode.h
# End Source File
# Begin Source File

SOURCE=..\code\nodes\directionallocatornode.cpp
# End Source File
# Begin Source File

SOURCE=..\code\nodes\directionallocatornode.h
# End Source File
# Begin Source File

SOURCE=..\code\nodes\eventlocatornode.cpp
# End Source File
# Begin Source File

SOURCE=..\code\nodes\eventlocatornode.h
# End Source File
# Begin Source File

SOURCE=..\code\nodes\fovlocatornode.cpp
# End Source File
# Begin Source File

SOURCE=..\code\nodes\fovlocatornode.h
# End Source File
# Begin Source File

SOURCE=..\code\nodes\genericlocatornode.cpp
# End Source File
# Begin Source File

SOURCE=..\code\nodes\genericlocatornode.h
# End Source File
# Begin Source File

SOURCE=..\code\nodes\interiorentrancelocatornode.cpp
# End Source File
# Begin Source File

SOURCE=..\code\nodes\interiorentrancelocatornode.h
# End Source File
# Begin Source File

SOURCE=..\code\nodes\occlusionlocatornode.cpp
# End Source File
# Begin Source File

SOURCE=..\code\nodes\occlusionlocatornode.h
# End Source File
# Begin Source File

SOURCE=..\code\nodes\pedgrouplocator.cpp
# End Source File
# Begin Source File

SOURCE=..\code\nodes\pedgrouplocator.h
# End Source File
# Begin Source File

SOURCE=..\code\nodes\railcamlocatornode.cpp
# End Source File
# Begin Source File

SOURCE=..\code\nodes\railcamlocatornode.h
# End Source File
# Begin Source File

SOURCE=..\code\nodes\scriptlocatornode.cpp
# End Source File
# Begin Source File

SOURCE=..\code\nodes\scriptlocatornode.h
# End Source File
# Begin Source File

SOURCE=..\code\nodes\splinelocatornode.cpp
# End Source File
# Begin Source File

SOURCE=..\code\nodes\splinelocatornode.h
# End Source File
# Begin Source File

SOURCE=..\code\nodes\staticcameralocatornode.cpp
# End Source File
# Begin Source File

SOURCE=..\code\nodes\staticcameralocatornode.h
# End Source File
# Begin Source File

SOURCE=..\code\nodes\triggertypes.h
# End Source File
# Begin Source File

SOURCE=..\code\nodes\triggervolumenode.cpp
# End Source File
# Begin Source File

SOURCE=..\code\nodes\triggervolumenode.h
# End Source File
# Begin Source File

SOURCE=..\code\nodes\wbspline.cpp
# End Source File
# Begin Source File

SOURCE=..\code\nodes\wbspline.h
# End Source File
# Begin Source File

SOURCE=..\code\nodes\zoneeventlocatornode.cpp
# End Source File
# Begin Source File

SOURCE=..\code\nodes\zoneeventlocatornode.h
# End Source File
# End Group
# Begin Group "contexts"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\code\contexts\locatorcontext.cpp
# End Source File
# Begin Source File

SOURCE=..\code\contexts\locatorcontext.h
# End Source File
# Begin Source File

SOURCE=..\code\contexts\triggercontext.cpp
# End Source File
# Begin Source File

SOURCE=..\code\contexts\triggercontext.h
# End Source File
# End Group
# Begin Group "utility"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\globalcode\utility\GLExt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\GLExt.h
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\mayahandles.cpp
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\mayahandles.h
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\MExt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\MExt.h
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\MExt_template.cpp
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\MExt_template.h
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\MUI.cpp
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\MUI.h
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\nodehelper.h
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\stdafx.h
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\transformmatrix.cpp
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\transformmatrix.h
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\util.c
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\util.h
# End Source File
# End Group
# Begin Group "mel scripts"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\code\scripts\wb_cleanup.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\wb_cleanup.mel
InputName=wb_cleanup

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\wb_cleanup.mel
InputName=wb_cleanup

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\scripts\wb_coinsplines.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\wb_coinsplines.mel
InputName=wb_coinsplines

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\wb_coinsplines.mel
InputName=wb_coinsplines

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\scripts\wb_locator.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\wb_locator.mel
InputName=wb_locator

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\wb_locator.mel
InputName=wb_locator

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\scripts\wb_main.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\wb_main.mel
InputName=wb_main

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\wb_main.mel
InputName=wb_main

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\scripts\wb_setup.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\wb_setup.mel
InputName=wb_setup

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\wb_setup.mel
InputName=wb_setup

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\scripts\wb_splines.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\wb_splines.mel
InputName=wb_splines

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\wb_splines.mel
InputName=wb_splines

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "precompiled header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\code\precompiled\PCH.cpp
# ADD BASE CPP /Yc"PCH.h"
# ADD CPP /Yc"PCH.h"
# End Source File
# Begin Source File

SOURCE=..\code\precompiled\PCH.h
# End Source File
# End Group
# Begin Group "resources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\code\resources\resource.h
# End Source File
# Begin Source File

SOURCE=..\code\resources\resource.rc
# End Source File
# End Group
# Begin Group "AETemplates"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\code\AETemplates\AEActionEventLocatorNodeTemplate.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEActionEventLocatorNodeTemplate.mel
InputName=AEActionEventLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEActionEventLocatorNodeTemplate.mel
InputName=AEActionEventLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\AETemplates\AEBreakableCameraLocatorNodeTemplate.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEBreakableCameraLocatorNodeTemplate.mel
InputName=AEBreakableCameraLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEBreakableCameraLocatorNodeTemplate.mel
InputName=AEBreakableCameraLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\AETemplates\AECarStartLocatorNodeTemplate.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AECarStartLocatorNodeTemplate.mel
InputName=AECarStartLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AECarStartLocatorNodeTemplate.mel
InputName=AECarStartLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\AETemplates\AEDirectionalLocatorNodeTemplate.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEDirectionalLocatorNodeTemplate.mel
InputName=AEDirectionalLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEDirectionalLocatorNodeTemplate.mel
InputName=AEDirectionalLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\AETemplates\AEEventLocatorNodeTemplate.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEEventLocatorNodeTemplate.mel
InputName=AEEventLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEEventLocatorNodeTemplate.mel
InputName=AEEventLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\AETemplates\AEFOVLocatorNodeTemplate.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEFOVLocatorNodeTemplate.mel
InputName=AEFOVLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEFOVLocatorNodeTemplate.mel
InputName=AEFOVLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\AETemplates\AEGenericLocatorNodeTemplate.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEGenericLocatorNodeTemplate.mel
InputName=AEGenericLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEGenericLocatorNodeTemplate.mel
InputName=AEGenericLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\AETemplates\AEInteriorEntranceLocatorNodeTemplate.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEInteriorEntranceLocatorNodeTemplate.mel
InputName=AEInteriorEntranceLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEInteriorEntranceLocatorNodeTemplate.mel
InputName=AEInteriorEntranceLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\AETemplates\AEOcclusionLocatorNodeTemplate.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEOcclusionLocatorNodeTemplate.mel
InputName=AEOcclusionLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEOcclusionLocatorNodeTemplate.mel
InputName=AEOcclusionLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\AETemplates\AEPedGroupLocatorNodeTemplate.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEPedGroupLocatorNodeTemplate.mel
InputName=AEPedGroupLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEPedGroupLocatorNodeTemplate.mel
InputName=AEPedGroupLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\AETemplates\AERailCamLocatorNodeTemplate.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AERailCamLocatorNodeTemplate.mel
InputName=AERailCamLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AERailCamLocatorNodeTemplate.mel
InputName=AERailCamLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\AETemplates\AEScriptLocatorNodeTemplate.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEScriptLocatorNodeTemplate.mel
InputName=AEScriptLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEScriptLocatorNodeTemplate.mel
InputName=AEScriptLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\AETemplates\AESplineLocatorNodeTemplate.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AESplineLocatorNodeTemplate.mel
InputName=AESplineLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AESplineLocatorNodeTemplate.mel
InputName=AESplineLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\AETemplates\AEStaticCameraLocatorNodeTemplate.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEStaticCameraLocatorNodeTemplate.mel
InputName=AEStaticCameraLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEStaticCameraLocatorNodeTemplate.mel
InputName=AEStaticCameraLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\AETemplates\AETriggerVolumeNodeTemplate.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AETriggerVolumeNodeTemplate.mel
InputName=AETriggerVolumeNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AETriggerVolumeNodeTemplate.mel
InputName=AETriggerVolumeNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\AETemplates\AEWBLocatorSuppress.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEWBLocatorSuppress.mel
InputName=AEWBLocatorSuppress

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEWBLocatorSuppress.mel
InputName=AEWBLocatorSuppress

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\AETemplates\AEWBSelectTarget.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEWBSelectTarget.mel
InputName=AEWBSelectTarget

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEWBSelectTarget.mel
InputName=AEWBSelectTarget

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\AETemplates\AEWBTriggerButton.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEWBTriggerButton.mel
InputName=AEWBTriggerButton

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEWBTriggerButton.mel
InputName=AEWBTriggerButton

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\code\AETemplates\AEZoneEventLocatorNodeTemplate.mel

!IF  "$(CFG)" == "worldbuilder - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEZoneEventLocatorNodeTemplate.mel
InputName=AEZoneEventLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "worldbuilder - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install) Templates
InputPath=..\code\AETemplates\AEZoneEventLocatorNodeTemplate.mel
InputName=AEZoneEventLocatorNodeTemplate

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\AETemplates\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "SuperCam"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\game\code\camera\icamerashaker.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\camera\isupercamtarget.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\camera\railcam.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\camera\railcam.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\camera\sinecosshaker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\camera\sinecosshaker.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\camera\staticcam.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\camera\staticcam.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\camera\supercam.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\camera\supercam.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\camera\supercamcontroller.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\camera\supercamcontroller.h
# End Source File
# End Group
# Begin Group "TriggerVolumes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\game\code\meta\locator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\meta\locator.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\meta\recttriggervolume.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\meta\recttriggervolume.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\meta\spheretriggervolume.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\meta\spheretriggervolume.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\meta\triggerlocator.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\meta\triggervolume.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\meta\triggervolume.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\meta\triggervolumetracker.h
# End Source File
# End Group
# Begin Group "game engine"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\code\gameengine\gameengine.cpp
# End Source File
# Begin Source File

SOURCE=..\code\gameengine\gameengine.h
# End Source File
# Begin Source File

SOURCE=..\code\gameengine\mayacamera.cpp
# End Source File
# Begin Source File

SOURCE=..\code\gameengine\mayacamera.h
# End Source File
# Begin Source File

SOURCE=..\code\gameengine\wbcamtarget.cpp
# End Source File
# Begin Source File

SOURCE=..\code\gameengine\wbcamtarget.h
# End Source File
# End Group
# Begin Group "from game"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\game\code\ai\actionnames.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\input\button.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\input\button.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\worldsim\character\charactercontroller.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\main\commandlineoptions.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\input\controller.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\data\gamedata.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\data\gamedatamanager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\data\gamedatamanager.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\render\DSG\IEntityDSG.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\render\DSG\IEntityDSG.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\input\inputmanager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\input\inputmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\meta\locatorevents.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\meta\locatortypes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\input\mappable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\input\mappable.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\input\mapper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\input\mapper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\data\memcard\memorycardmanager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\data\memcard\memorycardmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\input\rumbleeffect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\input\rumbleeffect.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\input\rumblexbox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\data\savegameinfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\data\savegameinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\input\usercontroller.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\input\usercontroller.h
# End Source File
# Begin Source File

SOURCE=..\..\..\game\code\input\virtualinputs.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\code\resources\icon1.ico
# End Source File
# End Target
# End Project
