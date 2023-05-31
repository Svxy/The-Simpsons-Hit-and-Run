# Microsoft Developer Studio Project File - Name="objectsnapper" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=objectsnapper - Win32 Tools Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "objectsnapper.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "objectsnapper.mak" CFG="objectsnapper - Win32 Tools Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "objectsnapper - Win32 Tools Release" (based on "Win32 (x86) Application")
!MESSAGE "objectsnapper - Win32 Tools Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "objectsnapper"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "objectsnapper - Win32 Tools Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "objectsnapper___Win32_Tools_Release"
# PROP BASE Intermediate_Dir "objectsnapper___Win32_Tools_Release"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\code\\" /I "C:\AW\Maya4.0\include" /I "..\..\..\game\libs\pure3d\toollib\inc" /I "..\..\..\game\libs\pure3d\toollib\chunks16\inc" /I "." /I "..\..\..\game\libs\pure3d\constants" /D "_WIN32" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\game\libs\pure3d\sdks\Maya4_0\include\\" /I "..\code\\" /I "..\..\..\game\libs\pure3d\toollib\inc" /I "..\..\..\game\libs\pure3d\toollib\chunks16\inc" /I "." /I "..\..\..\game\libs\pure3d\constants" /I "..\..\globalcode" /I "..\..\..\game\libs\radmath" /D "_WIN32" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /D "RAD_WIN32" /D "RAD_PC" /D "RAD_RELEASE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x1009 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib glu32.lib version.lib Foundation.lib OpenGL32.lib OpenMaya.lib OpenMayaFX.lib OpenMayaRender.lib OpenMayaUI.lib OpenMayaAnim.lib /nologo /subsystem:windows /dll /machine:I386 /out:"Release\objectsnapper.mll" /libpath:"C:\AW\Maya4.0\lib" /export:initializePlugin /export:uninitializePlugin
# ADD LINK32 user32.lib gdi32.lib glu32.lib version.lib Foundation.lib OpenGL32.lib OpenMaya.lib OpenMayaFX.lib OpenMayaRender.lib OpenMayaUI.lib OpenMayaAnim.lib /nologo /subsystem:windows /dll /machine:I386 /out:"Release\objectsnapper.mll" /libpath:"..\..\..\game\libs\pure3d\sdks\Maya4_0\lib" /export:initializePlugin /export:uninitializePlugin
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copy trackeditor.mll to Distribution and Maya dir.
PostBuild_Cmds=copy .\Release\objectsnapper.mll ..\..\MayaTools\Maya4.0\bin\plug-ins\objectsnapper.mll	copy .\Release\objectsnapper.mll C:\AW\Maya4.0\bin\plug-ins\objectsnapper.mll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "objectsnapper - Win32 Tools Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "objectsnapper___Win32_Tools_Debug"
# PROP BASE Intermediate_Dir "objectsnapper___Win32_Tools_Debug"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\code\\" /I "C:\AW\Maya4.0\include" /I "..\..\..\game\libs\pure3d\toollib\inc" /I "..\..\..\game\libs\pure3d\toollib\chunks16\inc" /I "." /I "..\..\..\game\libs\pure3d\constants" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\code\\" /I "..\..\..\game\libs\pure3d\toollib\inc" /I "..\..\..\game\libs\pure3d\toollib\chunks16\inc" /I "." /I "..\..\..\game\libs\pure3d\constants" /I "..\..\globalcode" /I "..\..\..\game\libs\radmath" /I "..\..\..\game\libs\pure3d\sdks\Maya4_0\include\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NT_PLUGIN" /D "RAD_WIN32" /D "RAD_PC" /D "RAD_DEBUG" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x1009 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib glu32.lib version.lib Foundation.lib OpenGL32.lib OpenMaya.lib OpenMayaFX.lib OpenMayaRender.lib OpenMayaUI.lib OpenMayaAnim.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug\objectsnapper.mll" /pdbtype:sept /libpath:"C:\AW\Maya4.0\lib" /export:initializePlugin /export:uninitializePlugin
# ADD LINK32 user32.lib gdi32.lib glu32.lib version.lib Foundation.lib OpenGL32.lib OpenMaya.lib OpenMayaFX.lib OpenMayaRender.lib OpenMayaUI.lib OpenMayaAnim.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug\objectsnapper.mll" /pdbtype:sept /libpath:"..\..\..\game\libs\pure3d\sdks\Maya4_0\lib" /export:initializePlugin /export:uninitializePlugin
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copy trackeditor.mll to Distribution and Maya dir.
PostBuild_Cmds=copy .\Debug\objectsnapper.mll C:\AW\Maya4.0\bin\plug-ins\objectsnapperd.mll
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "objectsnapper - Win32 Tools Release"
# Name "objectsnapper - Win32 Tools Debug"
# Begin Group "main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\code\main\mayaincludes.h
# End Source File
# Begin Source File

SOURCE=..\code\main\pluginMain.cpp
# ADD CPP /YX"precompiled\PCH.h"
# End Source File
# Begin Source File

SOURCE=..\code\main\pluginMain.h
# End Source File
# Begin Source File

SOURCE=..\code\main\stdafx.h
# End Source File
# End Group
# Begin Group "mel scripts"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\code\scripts\os_cleanup.mel

!IF  "$(CFG)" == "objectsnapper - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\os_cleanup.mel
InputName=os_cleanup

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "objectsnapper - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\os_cleanup.mel
InputName=os_cleanup

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

SOURCE=..\code\scripts\os_main.mel

!IF  "$(CFG)" == "objectsnapper - Win32 Tools Release"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\os_main.mel
InputName=os_main

BuildCmds= \
	copy $(InputPath) C:\AW\Maya4.0\scripts\others\$(InputName).mel \
	copy $(InputPath) ..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel \
	

"C:\AW\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\MayaTools\Maya4.0\scripts\others\$(InputName).mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "objectsnapper - Win32 Tools Debug"

# Begin Custom Build - Copy $(InputPath) to Maya (dist and install)
InputPath=..\code\scripts\os_main.mel
InputName=os_main

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
# Begin Group "commands"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\code\commands\snapselected.cpp
# ADD CPP /YX"precompiled\PCH.h"
# End Source File
# Begin Source File

SOURCE=..\code\commands\snapselected.h
# End Source File
# End Group
# Begin Group "utility"

# PROP Default_Filter ""
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
# ADD CPP /YX"precompiled\PCH.h"
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\MUI.h
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\util.c
# ADD CPP /YX"precompiled\PCH.h"
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\util.h
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\winutil.c
# ADD CPP /YX"precompiled\PCH.h"
# End Source File
# Begin Source File

SOURCE=..\..\globalcode\utility\winutil.h
# End Source File
# End Group
# Begin Group "precompiled header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\code\precompiled\PCH.cpp
# ADD CPP /Yc"PCH.h"
# End Source File
# Begin Source File

SOURCE=..\code\precompiled\PCH.h
# End Source File
# End Group
# End Target
# End Project
