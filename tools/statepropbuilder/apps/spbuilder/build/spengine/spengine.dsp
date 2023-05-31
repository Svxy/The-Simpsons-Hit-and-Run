# Microsoft Developer Studio Project File - Name="spengine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=spengine - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "spengine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "spengine.mak" CFG="spengine - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "spengine - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "spengine - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "spengine"
# PROP Scc_LocalPath "."
CPP=snCl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "spengine - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SPENGINE_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../code" /I "../../../../../ftech/radcore/inc/" /I "../../../../../radmath/" /I "../../../../../pure3d/" /I "../../../../../poser/inc/" /I "../../../../../pure3d/toollib/inc" /I "../../../../../pure3d/constants/" /I "../../../../../sim/" /D "NDEBUG" /D "RAD_TUNE" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RAD_WIN32" /D "RAD_PC" /D "SMARTPROP_ENGINE" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /dll /machine:I386 /out:"../../bin/spengine.dll"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\..\..\..\..\pure3d\build\lib\pddidx8t.dll ..\..\bin	coff2omf release\spengine.lib ..\..\bin\spengineb.lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "spengine - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SPENGINE_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /ZI /Od /I "../../code/" /I "../../../../../radcore/inc/" /I "../../../../../radmath" /I "../../../../../pure3d/" /I "../../../../../sim/" /I "../../../../../poser/inc/" /I "../../../../../pure3d/toollib/inc" /I "../../../../../pure3d/constants/" /I "../../../../../radcontent/inc" /D "_DEBUG" /D "RAD_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RAD_WIN32" /D "RAD_PC" /D "SMARTPROP_ENGINE" /FR /YX /FD /I /I /I /I /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"../../bin/spengine.dll" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\..\..\..\..\pure3d\build\lib\pddidx8d.dll ..\..\bin	coff2omf debug\spengine.lib ..\..\bin\spengineb.lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "spengine - Win32 Release"
# Name "spengine - Win32 Debug"
# Begin Group "interface"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\sp\interface\base.hpp
# End Source File
# Begin Source File

SOURCE=..\..\code\sp\interface\context.hpp
# End Source File
# Begin Source File

SOURCE=..\..\code\sp\interface.hpp
# End Source File
# Begin Source File

SOURCE=..\..\code\sp\interface\platform.hpp
# End Source File
# Begin Source File

SOURCE=..\..\code\sp\interface\workspace.hpp
# End Source File
# End Group
# Begin Group "engine"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\code\sp\engine\aicollisionsolveragent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\code\sp\engine\aicollisionsolveragent.hpp
# End Source File
# Begin Source File

SOURCE=..\..\code\sp\engine\context.cpp
# End Source File
# Begin Source File

SOURCE=..\..\code\sp\engine\context.hpp
# End Source File
# Begin Source File

SOURCE=..\..\code\sp\engine\dllmain.cpp
# End Source File
# Begin Source File

SOURCE=..\..\code\sp\engine\platform.cpp
# End Source File
# Begin Source File

SOURCE=..\..\code\sp\engine\stateprop.cpp
# End Source File
# Begin Source File

SOURCE=..\..\code\sp\engine\stateprop.hpp
# End Source File
# Begin Source File

SOURCE=..\..\code\sp\engine\statepropdata.cpp
# End Source File
# Begin Source File

SOURCE=..\..\code\sp\engine\statepropdata.hpp
# End Source File
# Begin Source File

SOURCE=..\..\code\sp\engine\statepropdatatypes.hpp
# End Source File
# Begin Source File

SOURCE=..\..\code\sp\engine\workspace.cpp
# End Source File
# Begin Source File

SOURCE=..\..\code\sp\engine\workspace.hpp
# End Source File
# End Group
# End Target
# End Project
