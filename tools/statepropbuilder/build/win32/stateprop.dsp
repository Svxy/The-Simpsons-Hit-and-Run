# Microsoft Developer Studio Project File - Name="stateprop" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=stateprop - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "stateprop.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "stateprop.mak" CFG="stateprop - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "stateprop - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "stateprop - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "stateprop"
# PROP Scc_LocalPath "..\..\.."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "stateprop - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../pure3d" /I "../../../radmath" /I "../../../radcore/inc" /I "../../../radcontent/inc" /I "../../../stateprop/inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "RAD_RELEASE" /D "RAD_PC" /D "RAD_WIN32" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "stateprop - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /ZI /Od /I "../../../pure3d" /I "../../../radmath" /I "../../../radcore/inc" /I "../../../radcontent/inc" /I "../../../stateprop/inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "RAD_DEBUG" /D "RAD_PC" /D "RAD_WIN32" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "stateprop - Win32 Release"
# Name "stateprop - Win32 Debug"
# Begin Group "src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\stateprop.cpp
# End Source File
# Begin Source File

SOURCE=..\..\inc\stateprop\stateprop.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\statepropdata.cpp
# End Source File
# Begin Source File

SOURCE=..\..\inc\stateprop\statepropdata.hpp
# End Source File
# Begin Source File

SOURCE=..\..\inc\stateprop\statepropdatatypes.hpp
# End Source File
# End Group
# End Target
# End Project
