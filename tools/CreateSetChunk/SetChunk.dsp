# Microsoft Developer Studio Project File - Name="Image2LUT" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Image2LUT - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SetChunk.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SetChunk.mak" CFG="Image2LUT - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Image2LUT - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Image2LUT - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Image2LUT"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Image2LUT - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../perl" /I "../../game/libs/pure3d/toollib/inc" /I "../../game/libs/pure3d/constants" /I "../../game/libs/pure3d/toollib/chunks16/inc" /I "../../game/code/constants" /I "../../game/libs/radmath" /I "../../game/libs" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "P3DDEBUG" /D "P3DWIN95" /D "RAD_RELEASE" /D "RAD_WIN32" /D "RAD_PC" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /nodefaultlib:"LIBC.lib" /out:"..\bin\CreateSetChunk.exe"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "Image2LUT - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../perl" /I "../../game/libs/pure3d/toollib/inc" /I "../../game/libs/pure3d/constants" /I "../../game/libs/pure3d/toollib/chunks16/inc" /I "../../game/libs/radmath" /I "../../game/code/constants" /I "../../game/libs" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "P3DDEBUG" /D "P3DWIN95" /D "RAD_DEBUG" /D "RAD_WIN32" /D "RAD_PC" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"LIBC.lib LIBCMTD.lib" /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "Image2LUT - Win32 Release"
# Name "Image2LUT - Win32 Debug"
# Begin Group "Source"

# PROP Default_Filter "*.cpp, *.c"
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\Param.cpp
# End Source File
# Begin Source File

SOURCE=.\version.cpp
# End Source File
# End Group
# Begin Group "Include"

# PROP Default_Filter "*.hpp, *.h"
# Begin Source File

SOURCE=.\Param.hpp
# End Source File
# Begin Source File

SOURCE=.\version.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\SetChunk.sc

!IF  "$(CFG)" == "Image2LUT - Win32 Release"

# Begin Custom Build - ToolsParamCreator
InputPath=.\SetChunk.sc
InputName=SetChunk

BuildCmds= \
	..\..\game\libs\pure3d\tools\commandline\bin\ToolsParamCreator  -s  $(InputName).sc

"Param.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Param.hpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Image2LUT - Win32 Debug"

# Begin Custom Build
InputPath=.\SetChunk.sc
InputName=SetChunk

BuildCmds= \
	..\..\game\libs\pure3d\tools\commandline\bin\ToolsParamCreator  -s  $(InputName).sc

"Param.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Param.hpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
