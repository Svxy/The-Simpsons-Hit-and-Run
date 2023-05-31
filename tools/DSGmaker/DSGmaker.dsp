# Microsoft Developer Studio Project File - Name="DSGmaker" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=DSGmaker - Win32 Tools Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DSGmaker.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DSGmaker.mak" CFG="DSGmaker - Win32 Tools Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DSGmaker - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "DSGmaker - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "DSGmaker - Win32 Tools Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "DSGmaker - Win32 Tools Release" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "DSGmaker"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DSGmaker - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\game\libs\pure3d\toollib\inc" /I "..\..\game\libs\pure3d\constants" /I "..\..\game\code\constants" /I "..\..\game\libs\radmath" /I "..\..\game\libs\pure3d\toollib\chunks16\inc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "RAD_RELEASE" /D "RAD_WIN32" /D "RAD_PC" /YX /FD /c
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 mysql++.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "DSGmaker - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "..\..\game\libs\pure3d\toollib\inc" /I "..\..\game\libs\pure3d\constants" /I "..\..\game\code\constants" /I "..\..\game\libs\radmath" /I "..\..\game\libs\pure3d\toollib\chunks16\inc" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS_DEBUG" /D "RAD_WIN32" /D "RAD_PC" /D "RAD_DEBUG" /YX /FD /GZ /c
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 mysql++.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "DSGmaker - Win32 Tools Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DSGmaker___Win32_Tools_Debug"
# PROP BASE Intermediate_Dir "DSGmaker___Win32_Tools_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "..\..\game\libs\pure3d\toollib\inc" /I "..\..\game\libs\pure3d\constants" /I "..\..\game\code\constants" /I "..\..\game\libs\radmath" /I "..\..\game\libs\pure3d\toollib\chunks16\inc" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS_DEBUG" /D "RAD_WIN32" /D "RAD_PC" /D "RAD_DEBUG" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "..\..\game\libs\pure3d\toollib\inc" /I "..\..\game\libs\pure3d\constants" /I "..\..\game\code\constants" /I "..\..\game\libs\radmath" /I "..\..\game\libs\pure3d\toollib\chunks16\inc" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "RAD_WIN32" /D "RAD_PC" /D "RAD_DEBUG" /YX /FD /GZ /c
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 mysql++.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 mysql++.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "DSGmaker - Win32 Tools Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DSGmaker___Win32_Tools_Release"
# PROP BASE Intermediate_Dir "DSGmaker___Win32_Tools_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\..\game\libs\pure3d\toollib\inc" /I "..\..\game\libs\pure3d\constants" /I "..\..\game\code\constants" /I "..\..\game\libs\radmath" /I "..\..\game\libs\pure3d\toollib\chunks16\inc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\game\libs\pure3d\toollib\inc" /I "..\..\game\libs\pure3d\constants" /I "..\..\game\code\constants" /I "..\..\game\libs\radmath" /I "..\..\game\libs\pure3d\toollib\chunks16\inc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "RAD_RELEASE" /D "RAD_WIN32" /D "RAD_PC" /YX /FD /c
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 mysql++.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 mysql++.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ENDIF 

# Begin Target

# Name "DSGmaker - Win32 Release"
# Name "DSGmaker - Win32 Debug"
# Name "DSGmaker - Win32 Tools Debug"
# Name "DSGmaker - Win32 Tools Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DSGList.cpp
# End Source File
# Begin Source File

SOURCE=.\dsgmaker.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DSGList.hpp
# End Source File
# Begin Source File

SOURCE=..\..\game\code\constants\phyprop.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "libs"

# PROP Default_Filter ""
# Begin Source File

SOURCE="C:\mysql++\lib\mysql++.lib"
# End Source File
# End Group
# End Target
# End Project
