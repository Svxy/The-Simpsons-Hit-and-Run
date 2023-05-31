# Microsoft Developer Studio Project File - Name="artchecker" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=artchecker - Win32 Tools Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "artchecker.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "artchecker.mak" CFG="artchecker - Win32 Tools Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "artchecker - Win32 Tools Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "artchecker - Win32 Tools Release" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "artchecker"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "artchecker - Win32 Tools Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "artchecker___Win32_Tools_Debug"
# PROP BASE Intermediate_Dir "artchecker___Win32_Tools_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "DEBUG" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /GR /GX /ZI /Od /I "..\..\game\libs\pure3d\toollib\inc" /I "..\..\game\libs\pure3d\constants" /I "..\..\game\code\constants" /I "..\..\game\libs\radmath" /I "..\..\game\libs\pure3d\toollib\chunks16\inc" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "RAD_WIN32" /D "RAD_PC" /D "RAD_DEBUG" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 mysql++.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"C:\mysql++\lib\\"
# SUBTRACT BASE LINK32 /incremental:no
# ADD LINK32 mysql++.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"C:\mysql++\lib\\"
# SUBTRACT LINK32 /incremental:no

!ELSEIF  "$(CFG)" == "artchecker - Win32 Tools Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "artchecker___Win32_Tools_Release"
# PROP BASE Intermediate_Dir "artchecker___Win32_Tools_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\game\libs\pure3d\toollib\inc" /I "..\..\game\libs\pure3d\constants" /I "..\..\game\code\constants" /I "..\..\game\libs\radmath" /I "..\..\game\libs\pure3d\toollib\chunks16\inc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "RAD_RELEASE" /D "RAD_PC" /D "RAD_WIN32" /FR /YX /FD /c
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ENDIF 

# Begin Target

# Name "artchecker - Win32 Tools Debug"
# Name "artchecker - Win32 Tools Release"
# Begin Group "sourcefile"

# PROP Default_Filter ".cpp"
# Begin Source File

SOURCE=.\artchecker.cpp
# End Source File
# Begin Source File

SOURCE=.\artobject.cpp
# End Source File
# Begin Source File

SOURCE=.\badlist.cpp
# End Source File
# Begin Source File

SOURCE=.\outputbuffer.cpp
# End Source File
# End Group
# Begin Group "header"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=.\artlimits.h
# End Source File
# Begin Source File

SOURCE=.\artobject.h
# End Source File
# Begin Source File

SOURCE=.\badlist.hpp
# End Source File
# Begin Source File

SOURCE=.\outputbuffer.hpp
# End Source File
# End Group
# End Target
# End Project
