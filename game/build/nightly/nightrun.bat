://============================================================================
://
:// NIGHTRUN.BAT - A batch file designed to check that all is
:// well with the Perforce database for a game team.
://
:// This is designed to run in unattended mode, in the middle of the night
:// when network usage is low.  It gets the entire project, and makes
:// EVERYTHING.  If any errors are found then they have to be dealt with at
:// a later time.
://
:// Talk to Stan Jang if there are any questions or problems.
://
:// Parameters:
://   ps2       - Just make the current PS2 game.
://   gc       - Just make the current gc game.
://   xbox      - Just make the current XBOX game.
://   debug     - Just make debug builds.
://   optimized - Just make optimized builds.
://   release   - Just make release builds.
://   nocopy    - Don't copy the build to the network.
://
://============================================================================

@echo on

://============================================================================
:// Deal with the command line parameters
://============================================================================

:// Deal with some parameters.
://
@ECHO ===== Handling Parameters =====
@ECHO %0 %1 %2 %3 %4 %5 %6 %7 %8 %9 > NUL

SET doPs2=1
SET doGC=1
SET doXbox=1
SET doDebug=1
SET doOptimized=1
SET doRelease=1
SET doCopy=1
SET errors=0

:doArgs

IF "%1" == "" GOTO doArgEnd

:// Convert 1st argument to lower case before parsing
@echo %1| %mksDir%sed "y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/;s/^/set curArg=/"> lowerarg.bat
call lowerarg.bat
%mksDir%rm -f lowerarg.bat

:// For only building a specific platform.
://
IF "%curArg%" == "ps2" SET doGC=0
IF "%curArg%" == "ps2" SET doXbox=0
IF "%curArg%" == "ps2" SHIFT

IF "%curArg%" == "gc" SET doPs2=0
IF "%curArg%" == "gc" SET doXbox=0
IF "%curArg%" == "gc" SHIFT

IF "%curArg%" == "xbox" SET doPs2=0
IF "%curArg%" == "xbox" SET doGC=0
IF "%curArg%" == "xbox" SHIFT

:// For only building a specific build.
://
IF "%curArg%" == "debug" SET doOptimized=0
IF "%curArg%" == "debug" SET doRelease=0
IF "%curArg%" == "debug" SHIFT

IF "%curArg%" == "optimized" SET doDebug=0
IF "%curArg%" == "optimized" SET doRelease=0
IF "%curArg%" == "optimized" SHIFT

IF "%curArg%" == "release" SET doDebug=0
IF "%curArg%" == "release" SET doOptimized=0
IF "%curArg%" == "release" SHIFT

:// Should we copy the build to the network?  default = yes
://
IF "%curArg%" == "nocopy" SET doCopy=0
IF "%curArg%" == "nocopy" SHIFT

GOTO doArgs

:doArgEnd


@ECHO ===== Moving to the Nightlybuild directory =====
%projDrive%
CD %projRootDir%

://============================================================================
:// Setup the Perforce environment
://============================================================================
@ECHO ===== Setting up Perforce Environment =====

:// The client Setup below will map the project to desired location on the
:// local machine.

%P4% set P4USER=%perforceUser%
%P4% set P4PORT=%perforcePort%
SET P4CLIENT=
%P4% set P4CLIENT=nightlybuild

:// Create a Perforce client file based on the location of the nightbuild
:// Perforce is set up to copy the project into the directory 
:// %nightBuildDir%newbuild
:// The project is moved into a permanent location after the sync.
:// Using the newbuild directory is necessary because if we simply had Perforce
:// sync files to the real location, it would try to remove those files the
:// next night when it is asked to sync to a new location.

@ECHO Client:	nightlybuild> client.txt
@ECHO Update:	1999/12/06 12:34:44>> client.txt
@ECHO Access:	1999/12/06 14:42:48>> client.txt
@ECHO Owner:	%p4User%>> client.txt
@ECHO Description:>> client.txt
@ECHO 	Created by %p4User%.>> client.txt
@ECHO Root:	%depotProjRoot%>> client.txt
@ECHO Options:	noallwrite noclobber nocompress crlf unlocked nomodtime>> client.txt
@ECHO View:>> client.txt
@ECHO 	//depot/%depotProjName%/... //nightlybuild%nightBuildDir%newbuild/...| %mksDir%sed -e "s/.:\\//" -e "s/\\/\//g" >> client.txt
@ECHO   -//depot/%depotProjName%/tools/CartoonViewer/... //nightlybuild%nightBuildDir%newbuild/tools/CartoonViewer/...| %mksDir%sed -e "s/.:\\//" -e "s/\\/\//g" >> client.txt
@ECHO   -//depot/%depotProjName%/tools/dosvagconverter/... //nightlybuild%nightBuildDir%newbuild/dosvagconverter/...| %mksDir%sed -e "s/.:\\//" -e "s/\\/\//g" >> client.txt
@ECHO   -//depot/%depotProjName%/tools/ps2hostserver/... //nightlybuild%nightBuildDir%newbuild/tools/ps2hostserver/...| %mksDir%sed -e "s/.:\\//" -e "s/\\/\//g" >> client.txt
@ECHO   -//depot/%depotProjName%/tools/SoundTest/... //nightlybuild%nightBuildDir%newbuild/tools/SoundTest/...| %mksDir%sed -e "s/.:\\//" -e "s/\\/\//g" >> client.txt

:// Get Perforce to process the new client file
%mksDir%cat client.txt | %P4% client -i
%mksDir%rm -f client.txt


://============================================================================
:// Delete the oldest builds
:// Automatically remove the oldest build if we have too many.
://============================================================================
@ECHO ===== Checking for/Deleting old builds =====

CD %nightBuildDir%

:// Create a temporary batch helper batch file.
:// This batch file will be called once for each build directory that is
:// found.  The batch file keeps track of how many times is has been called
:// and sets a variable if a certain number (%maxDirs%) has been reached.
://
@ECHO set count=%%count%%x> countdir.bat
@ECHO if "%%count%%"=="%%maxDirs%%" set delOldestBuild=1 >> countdir.bat

SET count=
SET delOldestBuild=0

:// Check if we have too many build directories
:// If so, %delOldestBuild% is set to 1.
://
%mksDir%ls -1p | %mksDir%sed -n "s/^.*\/$/call countdir.bat/p" > count.bat
CALL count.bat
%mksDir%rm -f count.bat

if %delOldestBuild%==0 GOTO DelEnd

:// We have too many builds.  Delete the oldest one.
:// %oldestBuild% is set by oldest.bat
://
%mksDir%ls -1pt | %mksDir%sed -n "s/^.*\/$/set oldestBuild=&/;s/\///p" > oldest.bat
CALL oldest.bat
%mksDir%rm -f oldest.bat

%mksDir%rm -R -f %oldestBuild%

:DelEnd

://============================================================================
:// Delete Contents of the newbuild directory
://============================================================================
CD %nightBuildDir%
%mksDir%rm -R -f newbuild

://============================================================================
:// Get files for the build from Perforce database
://============================================================================
@ECHO ===== Syncing from Perforce =====
%P4% sync -f //depot/%depotProjName%/build/...
%P4% sync -f //depot/%depotProjName%/code/...
%P4% sync -f //depot/%depotProjName%/cd/...
%P4% sync -f //depot/%depotProjName%/libs/...
%P4% sync -f //depot/%depotProjName%/exportart/...
%P4% sync -f //depot/%depotProjName%/exportartlibrary/...
%P4% sync -f //depot/%depotProjName%/sound/scripts/...
%P4% sync -f //depot/%depotProjName%/ps2movies/...

://%P4% sync -f //depot/%depotProjName%/sdks/...
://%P4% sync -f //depot/%depotProjName%/tools/...

@ECHO ===== Copying Sound files from T:\ =====
://CD %nightBuildDir%\newbuild\cd\
://CALL downloadsound.bat
://mkdir ps2\sound\typ

://============================================================================
:// Create the build number based on today's date
://============================================================================
@ECHO ===== Determining Build Number =====

:// The build number identifies the build.  This number is used as the root
:// directory for the build and may be used as a version number in the
:// executable.  The build number is based on the current date.
:// The format is YYMMDD.VER, where VER is a one digit version number
:// beginning at 0.

CD %nightBuildDir%


:// Create a variable, %filename%, based on the current date
:// The DOS "date" command prints a string like "Current date is Mon 05-08-2000"
://
:// @ECHO.|date|%mksDir%sed -n "s/^Current date is .* \(..\)-\(..\)..\(..\).*/set filename=\3\1\2/p" > getdate.bat

://@echo.|date|%mksDir%sed -n "s/^.* \(..\)\/\(..\)\/..\(..\).*/set filename=\3\1\2/p" > getdate.bat

://The below line will get the current changelist number instead of the date
SET p4command=p4 -p %perforcePort% -u %perforceUser% -c nightlybuild changes -m1 #have
@echo.|%p4command%|%mksDir%sed -n "s/^.*Change \(.....\).*/set filename=\1/p" > getdate.bat

CALL getdate.bat
%mksDir%rm -f getdate.bat

:// There may already by builds for today.  Need to figure out the next version
:// number to use for this build.

:// Get the most recent build directory for today, store in %mostRecentDir%
:// Use helper batch file to set the variable (ex. set mostRecentDir=180400.1)
://
%mksDir%ls -1dr %filename%.* | %mksDir%sed -n "1s/^/set mostRecentDir=/p"> help.bat
CALL help.bat
%mksDir%rm -f help.bat

:// Create a helper batch file to find the directory name for this build
://
@ECHO if %filename%.%%1 == %mostRecentDir% set foundMostRecentVersion=1 > help.bat
@ECHO if not %%foundMostRecentVersion%% == 1 set version=%%1>> help.bat

:// Find the directory name to use for this build
:// Already have the date for the directory name.  Just need the version.
:// Allow up to 10 versions of a build for a given date
:// help.bat will set %version%
://
SET version=
SET foundMostRecentVersion=0
FOR %%d in (9 8 7 6 5 4 3 2 1 0) DO CALL help.bat %%d
SET foundMostRecentVersion=
SET mostRecentDir=
%mksDir%rm -f help.bat

:// If no more directory names to use, default to version 0
IF "%version%" == "" %mksDir%rm -R -f %filename.%0
IF "%version%" == "" SET version=0




://============================================================================
:// Setup the build dir variables
://============================================================================
SET buildNumber=%filename%.%version%
SET buildDir=%nightBuildDir%newbuild\


://============================================================================
:// PS2: Build the scripts, art and code
://============================================================================

CD %buildDir%build\ps2

IF %doPs2% == 0 GOTO BldPEnd
IF %doDebug% == 0 GOTO BldPs2R
@ECHO ===== Building PS2 Debug build =====

CALL build.bat d
 
%mksDir%cp -f err %buildDir%erps2d.txt

:// Check if build was successful (just check executable)
@ECHO ===== Looking for %buildDir%cd\ps2\%elfName%d.elf =====

IF EXIST %buildDir%cd\ps2\%elfName%d.elf echo Debug PS2 build completed: %buildNumber%
IF NOT EXIST %buildDir%cd\ps2\%elfName%d.elf echo Debug PS2 build failed: %buildNumber%
IF NOT EXIST %buildDir%cd\ps2\%elfName%d.elf %mksDir%..\gbmail -v -to %mailRecipients% desau@radical.ca -file %buildDir%erps2d.txt -h smtp.radical.ca -from nightlybuild@radical.ca    -s "Debug PS2 build failed: %buildNumber%"   >> %nightBuildDir%mailto.log
IF NOT EXIST %buildDir%cd\ps2\%elfName%d.elf SET errors=1

:BldPs2O

IF %doOptimized% == 0 GOTO BldPs2R
@ECHO ===== Building PS2 Tune build =====

CALL  build.bat t
%mksDir%cp -f err %buildDir%erps2t.txt

:// Check if build was successful (just check executable)
@ECHO ===== Looking for %buildDir%cd\ps2\%elfName%t.elf =====

IF EXIST %buildDir%cd\ps2\%elfName%t.elf echo Optimized PS2 build completed: %buildNumber%
IF NOT EXIST %buildDir%cd\ps2\%elfName%t.elf echo Optimized PS2 build failed: %buildNumber%
IF NOT EXIST %buildDir%cd\ps2\%elfName%t.elf %mksDir%..\gbmail -v -to %mailRecipients% desau@radical.ca -file %buildDir%erps2t.txt -h smtp.radical.ca -from nightlybuild@radical.ca    -s "Tune PS2 build failed: %buildNumber%"   >> %nightBuildDir%mailto.log
IF NOT EXIST %buildDir%cd\ps2\%elfName%t.elf SET errors=1



:BldPs2R

IF %doRelease% == 0 GOTO BldPEnd
@ECHO ===== Building PS2 Release build =====

CALL build.bat r 

%mksDir%cp -f err %buildDir%erps2r.txt

:// Check if build was successful (just check executable)
@ECHO ===== Looking for %buildDir%cd\ps2\%elfName%r.elf =====

IF EXIST %buildDir%cd\ps2\%elfName%r.elf echo Release PS2 build completed: %buildNumber%
IF NOT EXIST %buildDir%cd\ps2\%elfName%r.elf echo Release PS2 build failed: %buildNumber%
IF NOT EXIST %buildDir%cd\ps2\%elfName%r.elf %mksDir%..\gbmail -v -to %mailRecipients% desau@radical.ca -file %buildDir%erps2r.txt -h smtp.radical.ca -from nightlybuild@radical.ca    -s "Release PS2 build failed: %buildNumber%"   >> %nightBuildDir%mailto.log
IF NOT EXIST %buildDir%cd\ps2\%elfName%r.elf SET errors=1

:BldPEnd


://============================================================================
:// PS2: Copy the essential files into the Playable directory (release)
://============================================================================

IF %doCopy% == 0 GOTO CpyPEnd
IF %doPs2% == 0 GOTO CpyPEnd

CD %buildDir%cd\ps2

:// If release build was not successful, don't copy to network
IF NOT EXIST %elfName%r.elf GOTO CpyPEnd
@ECHO ===== Copying Successful PS2 Release build to T:\Simpsons2\Playable =====

:// Delete ps2 playable on team's drive
%mksDir%rm -R -f %networkDir%\night\ps2

%mksDir%cp -f -R . %networkDir%\night\ps2

://Calling automated Artchecker on art with summary option.
cd ..\..\cd\ps2\art

://batch file located in the depot\tools\artchecker
CALL summary.bat %buildDir%\build\ps2\LevelMemoryNumbers.txt

%mksDir%..\gbmail -v -to jdeyoung@radical.ca dmurnane@radical.ca yayoi@radical.ca cchow@radical.ca ryoung@radical.ca -file %buildDir%\build\ps2\LevelMemoryNumbers.txt -h smtp.radical.ca -from nightlybuild@radical.ca    -s "Level Memory Numbers PS2"  

:CpyPEnd


://============================================================================
://GC: Build the scripts, art and code
://============================================================================

CD %buildDir%build\GC

IF %doGC% == 0 GOTO BldGCEnd
IF %doDebug% == 0 GOTO BldGCR
@ECHO ===== Building GC Debug build =====

CALL build.bat d
%mksDir%cp -f err %buildDir%erGCd.txt

:// Check if build was successful (just check executable)
@ECHO ===== Looking for %buildDir%cd\GC\%GCName%d.elf =====

IF EXIST %buildDir%build\gc\%GCName%d.elf echo Debug GC build completed: %buildNumber%
IF NOT EXIST %buildDir%build\gc\%GCName%d.elf echo Debug GC build failed: %buildNumber%
IF NOT EXIST %buildDir%build\gc\%GCName%d.elf %mksDir%..\gbmail -v -to %mailRecipients% cbrisebois@radical.ca -file %buildDir%erGCd.txt -h smtp.radical.ca -from nightlybuild@radical.ca    -s "Debug GC build failed: %buildNumber%"   >> %nightBuildDir%mailto.log
IF NOT EXIST %buildDir%build\gc\%GCName%d.elf SET errors=1



:BldGCR

IF %doRelease% == 0 GOTO BldGCEnd
@ECHO ===== Building GC Release build =====

CALL build.bat r

%mksDir%cp -f err %buildDir%erGCr.txt


:// Check if build was successful (just check executable)
@ECHO ===== Looking for %buildDir%cd\GC\%GCName%r.elf =====

IF EXIST %buildDir%build\gc\%GCName%r.elf echo Release GC build completed: %buildNumber%
IF NOT EXIST %buildDir%build\gc\%GCName%r.elf echo Release GC build failed: %buildNumber%
IF NOT EXIST %buildDir%build\gc\%GCName%r.elf %mksDir%..\gbmail -v -to %mailRecipients% cbrisebois@radical.ca -file %buildDir%erGCr.txt -h smtp.radical.ca -from nightlybuild@radical.ca    -s "Release GC build failed: %buildNumber%"   >> %nightBuildDir%mailto.log
IF NOT EXIST %buildDir%build\gc\%GCName%r.elf SET errors=1


:BldGCEnd


://============================================================================
:// GC: Copy the essential files into the Playable directory
://============================================================================

://copying elfs to the cd folder

://copy %buildDir%build\gc\*.elf %buildDir%\cd\gc

IF %doCopy% == 0 GOTO CpyGCEnd
IF %doGC% == 0 GOTO CpyGCEnd

CD %buildDir%cd\gc

:// If release build was not successful, don't copy to network
IF NOT EXIST %GCName%r.elf GOTO CpyGCEnd

@ECHO ===== Copying Successful GC Release build to T:\Simpsons2\playable =====

:// Builds okay so copy to network

:// Delete GC playable on network drive
%mksDir%rm -R -f %networkDir%\night\gc

%mksDir%cp -f -R . %networkDir%\night\gc


://===============Calling automated Artchecker on art with summary option.=========
cd ..\..\cd\gc\art

://batch file located in the depot\tools\artchecker
://CALL summary.bat %buildDir%\build\gc\LevelMemoryNumbers.txt

%mksDir%..\gbmail -v -to cbrisebois@radical.ca -file %buildDir%\build\gc\LevelMemoryNumbers.txt -h smtp.radical.ca -from nightlybuild@radical.ca    -s "Level Memory Numbers GC"  




:CpyGCEnd


://============================================================================
:// XBOX: Build the scripts, art and code
://============================================================================

CD %buildDir%build\xbox

IF %doXbox% == 0 GOTO BldXEnd
IF %doDebug% == 0 GOTO BldXboxR

@ECHO ===== Auto Update XBOX PAL DSP =====
%P4% edit //depot/%depotProjName%/build/xbox/SRR2PAL.dsp
searchreplace rules.txt srr2.dsp SRR2PAL.dsp
%P4% revert -a //depot/%depotProjName%/build/xbox/SRR2PAL.dsp
%P4% submit -i < submit.txt


@ECHO ===== Building XBOX TUNE build =====

CALL build.bat t
%mksDir%cp -f err %buildDir%erxboxt.txt

:// Check if build was successful (just check executable)
@ECHO ===== Looking for %buildDir%build\xbox\tune\%xbename%t.xbe =====

IF EXIST %buildDir%build\xbox\tune\%xbename%t.xbe echo tune XBOX build completed: %buildNumber%
IF NOT EXIST %buildDir%build\xbox\tune\%xbename%t.xbe echo tune XBOX build failed: %buildNumber%
IF NOT EXIST %buildDir%build\xbox\tune\%xbename%t.xbe %mksDir%..\gbmail -v -to %mailRecipients% dchau@radical.ca -file %buildDir%erxboxt.txt -h smtp.radical.ca -from nightlybuild@radical.ca    -s "Debug XBOX build failed: %buildNumber%"   >> %nightBuildDir%mailto.log
IF NOT EXIST %buildDir%build\xbox\tune\%xbename%t.xbe SET errors=1


:BldXboxR

IF %doRelease% == 0 GOTO BldXEnd
@ECHO ===== Building XBOX Release build =====

CALL build.bat r
%mksDir%cp -f err %buildDir%erxboxr.txt


:// Check if build was successful (just check executable)
@ECHO ===== Looking for %buildDir%build\xbox\release\default.xbe =====

IF EXIST %buildDir%build\xbox\release\default.xbe echo Release XBOX build completed: %buildNumber%
IF NOT EXIST %buildDir%build\xbox\release\default.xbe echo Release XBOX build failed: %buildNumber%
IF NOT EXIST %buildDir%build\xbox\release\default.xbe %mksDir%..\gbmail -v -to %mailRecipients% dchau@radical.ca -file %buildDir%erxboxr.txt -h smtp.radical.ca -from nightlybuild@radical.ca    -s "Release XBOX build failed: %buildNumber%"   >> %nightBuildDir%mailto.log
IF NOT EXIST %buildDir%build\xbox\release\default.xbe SET errors=1

:BldXEnd


://============================================================================
:// XBOX: Copy the essential files into the Playable directory
://============================================================================

IF %doCopy% == 0 GOTO CpyXEnd
IF %doXbox% == 0 GOTO CpyXEnd

CD %buildDir%cd\xbox


:// If release build was not successful, don't copy to network
IF NOT EXIST %buildDir%build\xbox\release\%xbename%r.xbe GOTO CpyXEnd


@ECHO ===== Copying Successful Xbox Release and tune build to T:\Simpsons2\playable =====

://Copy Release.xbe to cd\xbox folder
copy %buildDir%build\xbox\release\default.xbe %buildDir%cd\xbox

copy %buildDir%build\xbox\tune\%xbename%t.xbe %buildDir%cd\xbox



:// Delete xbox playable on network drive
%mksDir%rm -R -f %networkDir%\night\xbox

%mksDir%cp -f -R . %networkDir%\night\xbox


:CpyXEnd


://============================================================================
:// If all of the builds were unsuccessful, mark the build as BAD
:// If all of the builds were successful, send out email
://============================================================================

CD %nightBuildDir%


@ECHO ===== Emailling Build Results to %mailRecipients% =====
IF NOT %errors% == 0 %mksDir%..\gbmail -v -to %mailRecipients% -h smtp.radical.ca -from nightlybuild@radical.ca    -s "Night build completed: Build %buildNumber% DID NOT completed successfully on all platforms" >> %nightBuildDir%mailto.log
IF     %errors% == 0 %mksDir%..\gbmail -v -to %mailRecipients% -h smtp.radical.ca -from nightlybuild@radical.ca    -s "Night build completed: Build %buildNumber%  completed successfully on all platforms" >> %nightBuildDir%mailto.log

://============================================================================
:// All done.
://============================================================================

:End
@ECHO ===== Build Process Finished (Clean-up) =====

cd %nightBuildDir%

://============================================================================
://Backup the new build
://============================================================================
%mksDir%cp -r newbuild %nightBuildDir%%buildNumber%\



%mksDir%rm -f %nightBuildDir%countdir.bat
%mksDir%rm -f %nightBuildDir%lowerarg.bat
SET doPs2=
SET doGC=
SET doXbox=
SET doDebug=
SET doOptimized=
SET doRelease=
SET doCopy=
SET curArg=
SET delOldestBuild=
SET oldestBuild=
SET filename=
SET version=
SET buildNumber=
SET buildDir=
SET errors=
