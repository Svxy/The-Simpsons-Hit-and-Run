#==============================================================================
# Process.pl
#
# This file contains all the subroutines used in processing the pipeline files.
# These subroutines are the first level of routines called by pipe.pl
#
# + Created March 26 - C Brisebois
#==============================================================================

#==============================================================================
# CheckDependencies
#
# Returns: sets the $$rebuildRef parameter to true if the file is out of date.
#
# Tests the given pipe.ini file for dependent file dates that are newer than 
# the STORE or RESULT files.
#==============================================================================
sub CheckDependencies
{
#parameters: .ini file, reference to rebuilding.
#files can use sub dir or this dir.  Eg. apu\<filename> or <filename>

    $ERRORLEVEL = $SUCCESS;
    $SEVERITY = 0;

    my @dependFiles;
    my @libraryDependFiles;
    my @resultFiles;
    my @storeFiles;
    my @storeDependFiles;
    my @templateFiles;
    my @ruleFiles;

    my $iniFile = @_[0];
    my $rebuildRef = @_[1];
    $$rebuildRef = 0;

    my $newestFileName = "";

    #add the ini file to the dependFile list.
    push( @dependFiles, $iniFile );

    if ( open(INI, "$iniFile") )
    {
        if ( $ARTCHECK == 1 )
        {
            #Rebuild everything!
            $$rebuildRef = 1;
            close( INI );

            if ( $DEBUG_ENABLED )
            {
                print "Artcheck rebuild!\n";
            }

            goto DIE;
        }

        while ( <INI> )
        {
            #Look for DEPEND
            if ( grep(/^ *DEPEND /, $_) )
			{
				#remove the DEPEND part
				$tempstr = $_;
				$tempstr =~ s/^ *DEPEND //;
				chomp($tempstr);

				my @files = split( / /, $tempstr );

                push ( @dependFiles, @files );
            }

            #Look for LIB_DEPEND
            if ( grep(/^ *LIB_DEPEND /, $_) )
			{
				#remove the LIB_DEPEND part
				$tempstr = $_;
				$tempstr =~ s/^ *LIB_DEPEND //;
				chomp($tempstr);

				my @files = split( / /, $tempstr );

                push ( @libraryDependFiles, @files );
            }

            #Look for RESULT
            if ( grep(/^ *RESULT /, $_) )
			{
				#remove the RESULT part
				$tempstr = $_;
				$tempstr =~ s/^ *RESULT //;
				chomp($tempstr);

				(my $source, my $dest) = split( / /, $tempstr );

                if ( !grep(/.+/, $dest) )
                {
                    #This is a syntatic error
                    print "========== Syntax Error: $_ needs a destination ==========\n";
                    $ERRORLEVEL = $SYNTAX_ERROR;
                    $SEVERITY = $255;
                    goto DIE;
                }                    

                push ( @resultFiles, $dest );
            }

            #Look for STORE
            if ( grep(/^ *STORE /, $_) )
			{
				#remove the STORE part
				$tempstr = $_;
				$tempstr =~ s/^ *STORE //;
				chomp($tempstr);

				(my $source, my $dest) = split( / /, $tempstr );

                if ( !grep(/.+/, $dest) )
                {
                    #This is a syntatic error
                    print "========== Syntax Error: $_ needs a destination ==========\n";
                    $ERRORLEVEL = $SYNTAX_ERROR;
                    $SEVERITY = $255;
                    goto DIE;
                }                    

                push ( @storeFiles, $dest );
            }

            #Look for STORE_DEPEND
            if ( grep(/^ *STORE_DEPEND /, $_) )
            {
                #remove the STORE_DEPEND part
				$tempstr = $_;
				$tempstr =~ s/^ *STORE_DEPEND //;
				chomp($tempstr);

				my @files = split( / /, $tempstr );

                push ( @storeDependFiles, @files );
            }

            #Look for TEMPLATE
            if ( grep(/^ *TEMPLATE /, $_) )
            {
                #Remove the TEMPLATE part
                $tempstr = $_;
                $tempstr =~ s/^ *TEMPLATE //;
                chomp($tempstr);
                (my $type, my $input, my $output) = split( /\s+/, $tempstr );

                push ( @templateFiles, $type );
            }

            #Look for RULE
            if ( grep(/^ *RULE /, $_) )
            {
                #Remove the RULE part
                $tempstr = $_;
                $tempstr =~ s/^ *RULE //;
                chomp($tempstr);
                (my $ruleFile, my $dieLevel, my $inputFile) = split( / /, $tempstr );

                push ( @ruleFiles, $ruleFile );
            }

        }

        close( INI );
    }
    else
    {
        #print "========== $iniFile file missing ==========\n";
        #$ERRORLEVEL = $BAD_FILE;
        #TODO: is this a problem?
        goto DIE;
    }

    my $newestFile = 0;

    #**************************************************************************

    #Test the dependencies for rebuilding.
	foreach my $file (@dependFiles)
	{
		my @stats = stat("$ACTIVE_EXPORT\\$file");

		if ( @stats )
		{
            #Find the newest file in the dependency list.
			if ( @stats[9] > $newestFile )
			{
				$newestFile = @stats[9];
                $newestFileName = $file;
			}
		}
        else
        {
            #This file no longer exists.
            print "========== Dependency file: $file does not exist ==========\n";
            $ERRORLEVEL = $BAD_FILE;
            $SEVERITY = $255;
            goto DIE;
        }
	}

	foreach my $file (@libraryDependFiles)
	{
		my @stats = stat("$EXPORTARTLIBRARY_DIR\\$file");

		if ( @stats )
		{
            #Find the newest file in the dependency list.
			if ( @stats[9] > $newestFile )
			{
				$newestFile = @stats[9];
                $newestFileName = $file;
			}
		}
        else
        {
            #This file no longer exists.
            print "========== Library Dependency file: $file does not exist ==========\n";
            $ERRORLEVEL = $BAD_FILE;
            $SEVERITY = $255;
            goto DIE;
        }
	}

    foreach my $file (@storeDependFiles)
	{
		my @stats = stat("$ACTIVE_STORE\\$file");

		if ( @stats )
		{
            #Find the newest file in the dependency list.
			if ( @stats[9] > $newestFile )
			{
				$newestFile = @stats[9];
                $newestFileName = $file;
			}
		}
        else
        {
            #This file no longer exists.
            print "========== Stored Dependency file: $file does not exist ==========\n";
            $ERRORLEVEL = $BAD_FILE;
            $SEVERITY = $255;
            goto DIE;
        }
	}

    #Test the template files.
    foreach my $file (@templateFiles)
    {
        if ( $DEBUG_ENABLED )
        {
            print "Testing TEMPLATE file: $TEMPLATES_DIR\\$file.$CURRENT_PLATFORM\n";
        }

        my @stats = stat("$TEMPLATES_DIR\\$file.$CURRENT_PLATFORM");

        #Find the newest file in the dependency list.
        if ( @stats )
        {
		    if ( @stats[9] > $newestFile )
		    {
			    $newestFile = @stats[9];
                $newestFileName = $file;
		    }
        }
        else
        {
            #This file no longer exists.
            print "========== Template file: $file does not exist ==========\n";
            $ERRORLEVEL = $BAD_FILE;
            $SEVERITY = $255;
            goto DIE;
        }
    }  

    #Test the rule files.
    foreach my $file (@ruleFiles)
    {
        if ( $DEBUG_ENABLED )
        {
            print "Testing RULE file: $TEMPLATES_DIR$file.$CURRENT_PLATFORM\n";
        }

        my @stats = stat("$TEMPLATES_DIR$file.$CURRENT_PLATFORM");

        #Find the newest file in the dependency list.
        if ( @stats )
        {
		    if ( @stats[9] > $newestFile )
		    {
			    $newestFile = @stats[9];
                $newestFileName = $file;
		    }
        }
        else
        {
            #This file no longer exists.
            print "========== Template file: $file does not exist ==========\n";
            $ERRORLEVEL = $BAD_FILE;
            $SEVERITY = $255;
            goto DIE;
        }
    }  

    #**************************************************************************

    #Test the stored files.
    foreach my $file (@storeFiles)
    {
        my @stats = stat("$ACTIVE_STORE\\$file");

        if ( @stats )
        {
            if ( @stats[9] < $newestFile )
            {
                if ( $DEBUG_ENABLED )
                {
                    print "STOREFILE: $ACTIVE_STORE\\$file OLDER\n";
                    print "Newest file: $newestFileName\n";
                }
                #A stored file is older than a dependency file.
                #Need to rebuild.
                $$rebuildRef = 1;
                goto DIE;
            }
        }
        else
        {
            if ( $DEBUG_ENABLED )
            {
                print "STOREFILE: $ACTIVE_STORE\\$file MISSING\n";
            }

            #This file has is missing, need to rebuild.
            $$rebuildRef = 1;
            goto DIE;
        }

    }
                
    #Test the result files.
    foreach my $file (@resultFiles)
    {
        my @stats = stat("$RESULT_DIR\\$file");

        if ( @stats )
        {
            if ( @stats[9] < $newestFile )
            {
                if ( $DEBUG_ENABLED )
                {
                    print "RESULTFILE: $RESULT_DIR\\$file OLDER\n";
                    print "Newest file: $newestFileName\n";
                }
                #A result file is older than a dependency file.
                #Need to rebuild.
                $$rebuildRef = 1;
                goto DIE;
            }
        }
        else
        {
            #This file has is missing, need to rebuild.
            if ( $DEBUG_ENABLED )
            {
                print "RESULTFILE: $RESULT_DIR\\$file MISSING\n";
            }
            $$rebuildRef = 1;
            goto DIE;
        }
    }  

DIE:
    $ERRORLEVEL;
}

#==============================================================================
# ProcessIni
#
# Returns: Nothing.
#
# This subroutine opens the pipe.ini file in the current dir and processes each
# line in it by calling ProcessCommand and passing in the current line.
#==============================================================================
sub ProcessIni
{
#parameters: filename.ini

    $ERRORLEVEL = $SUCCESS;
    $SEVERITY = 0;

    my $lineCount = 0;

    if ( open(INI, "@_[0]") )
    {
        print "Processing @_[0]\n";
        PrintUnderline( (11 + (length @_[0])) );

        while ( <INI> )
        {
            ProcessCommand( $_ );

            #If there is text on the line and it is not a valid command.           
            if ( $ERRORLEVEL == $BAD_COMMAND )
            {
                print "\n========== Invalid command in file: @_[0] line: $lineCount ==========\n";
                print "$_\n\n";
                $ERRORLEVEL = $BAD_COMMAND;
                goto DIE;
            }

            if ( $ERRORLEVEL != $SUCCESS )
            {
                goto DIE;
            }

            $lineCount++;
        }

DIE:
        close(INI);
    }
    else
    {
        print "========== Nothing to do ==========\n\n";
    }

    $ERRORLEVEL;
}

#==============================================================================
# RecursiveProcess
#
# Returns: Nothing.
#
# This subroutine recurses the given directory tree to the bottom and then 
# works its way back up calling ProcessIni on every pipe.ini file that needs
# rebuilding.  The rebuild dependency is determined by calling 
# CheckDependencies on a given pipe.ini file.
#
# This subroutine will create a directory tree that mirrors the export art
# tree as it processes the different levels.
#
# This subroutine also cleans up after itself to prevent data leftovers from
# a previous run.
#==============================================================================
sub RecursiveProcess
{
#parameters: target dir.
    
    $ERRORLEVEL = $SUCCESS;
    $SEVERITY = 0;
  
    #Setup the dirs
    my $setupDir = GetCurDirRelativeToTEMP();
    DoSetup( "$setupDir" );

    foreach my $dir (GetDirs("@_[0]"))
    {
        `mkdir $dir 1>$TEMP_LOG 2>$TEMP_ERR`;
        chdir( "$dir" );
        RecursiveProcess( "@_[0]\\$dir" );
        if ( $ERRORLEVEL != $SUCCESS )
        {
            goto DIE;
        }
        chdir( ".." );
        `rmdir /S /Q $dir 1>$TEMP_LOG 2>$TEMP_ERR`;

        KillTempLog();
    }

    #Note: We barf here because this level in the heirarcy could effect the dependencies of the next level.
    #   Perhaps we will need to set a severity level in case it's alright to continue the pipeline.
    #TODO: Test for other conditions.
    if ( GetFiles( "@_[0]" ) && $ERRORLEVEL == $SUCCESS )
    {
        print "\nWorking in: @_[0]\n";

        `copy @_[0] .`;
        `attrib -r *.*`;

        #Test to see if there are any dependencies that need updating.

        my $relativeDir = GetCurDirRelativeToTEMP();

        $ACTIVE_EXPORT = "$EXPORTART_DIR$relativeDir";
        $ACTIVE_TEMP = "$TEMP_DIR$relativeDir";
        $ACTIVE_STORE = "$STORE_DIR$CURRENT_PLATFORM\\$relativeDir";

        my $shouldRebuild = 0;

        CheckDependencies( "pipe.ini", \$shouldRebuild );  
        
        if ( $ERRORLEVEL != $SUCCESS )
        {
            goto DIE;
        }    

        if ( $shouldRebuild )
        {
            #TODO: make this work with debug and or other ini files as well.
            ProcessIni( "pipe.ini" );
            if ( $ERRORLEVEL != $SUCCESS )
            {
                goto DIE;
            }
        }
        else
        {
            if ( $DEBUG_ENABLED )
            {
                print "No change in dependencies.\n";
            }
        }

        `del /Q *.*`;
    }

DIE:
    DoCleanup( "$setupDir" );

    $ERRORLEVEL;
}

#==============================================================================
# RunPipeline
#
# Returns: Nothing.
#
# This subroutine is the main engine for running the pipeline.
# It will call RecursiveProcess on each target directory supplied by the 
# command-line arguments.
#==============================================================================
sub RunPipeline
{
    chdir ( "$TEMP_DIR" );

    foreach my $platform (@ACTIVE_PLATFORMS)
    {
        $CURRENT_PLATFORM = $platform;
        $RESULT_DIR = "$CD_DIR$CURRENT_PLATFORM\\$ART_DIR";

        print "ACTIVE PLATFORM: $platform\n\n";

        foreach my $target (@ACTIVE_TARGETS)
        {
            my $dirTarget = $target;
            my $count = 1;
            my @dirs = split( /:/, $dirTarget );

            #Test to see if this is a sub dir.
            if ( grep( /:/, $dirTarget ) )
            {
                #This is a sub dir target.
                $count = @dirs;

                while ( grep( /:/, $dirTarget) )
                {
                    $dirTarget =~ s/:/\\\\/;
                }
            }

            if ( $DEBUG_ENABLED )
            {
                print "Dir Target: $dirTarget\n";
            }


            #Do setup
            DoSetup( "$dirTarget" );

            `mkdir $dirTarget 1>$TEMP_LOG 2>$TEMP_ERR`;
            chdir( "$dirTarget" );
            RecursiveProcess( "$EXPORTART_DIR$dirTarget" );
            for ( my $i = 0; $i < $count; $i++ )
            {
                chdir( ".." );
            }

            my $delDir = @dirs[0];
            `rmdir /S /Q $delDir 1>$TEMP_LOG 2>$TEMP_ERR`;

            KillTempLog();

            #Do Cleanup
            DoCleanup( "$dirTarget" );

	    if ( $ERRORLEVEL != $SUCCESS )
            {
                goto DIE;
            }
        }

        #Formatting.
        print "\n\n";
        print "COMPLETED PLATFORM: $platform\n\n\n";
    }

DIE:
	if ( $ERRORLEVEL != $SUCCESS )
	{
		printf("\n\n\n*****************************************************************\n\n");
	    printf("If you do not know how do deal with this error, contact Chuck, Devin \n");
	    printf("or Cary\n\n");
		printf("*****************************************************************\n\n\n");
	}    
    chdir( ".." );
}

#==============================================================================
# CleanPipeline
#
# Returns: Nothing.
#
# This subroutine will clean the STORE directory and the RESULT files created
# by the current level in the export art tree.
#==============================================================================
sub CleanPipeline
{
    foreach my $platform (@ACTIVE_PLATFORMS)
    {
        $CURRENT_PLATFORM = $platform;
        $RESULT_DIR = "$CD_DIR$CURRENT_PLATFORM\\$ART_DIR";

        if ( $DEBUG_ENABLED )
        {
            print "rmdir /S /Q $EXPORT_LIBRARIES$CURRENT_PLATFORM\n";
        }
        `rmdir /S /Q $EXPORT_LIBRARIES$CURRENT_PLATFORM 1>$TEMP_LOG 2>$TEMP_ERR`;

        foreach my $target (@ACTIVE_TARGETS)
        {
            my @dirs = split( /:/, $target );

            #Test to see if this is a sub dir.
            if ( grep( /:/, $target ) )
            {
                #This is a sub dir target.
                while ( grep( /:/, $target) )
                {
                    $target =~ s/:/\\\\/;
                }
            }
            
            if ( $DEBUG_ENABLED )
            {
                print "rmdir /S /Q $STORE_DIR$CURRENT_PLATFORM\\$target\n";
            }
            `rmdir /S /Q $STORE_DIR$CURRENT_PLATFORM\\$target 1>$TEMP_LOG 2>$TEMP_ERR`;

            KillTempLog();

            DoSetup( $target );

            CleanResults( "$target" );

            DoCleanup( $target );
        }
    }        
}


#==============================================================================
# CleanResults
#
# Returns: Nothing.
#
# This subroutine will clean the RESULT files created by the current level in 
# the export art tree.  This is a recursive subroutine.
#==============================================================================
sub CleanResults
{
    my $target = @_[0];
    my $amHere = "$EXPORTART_DIR\\$target";

    my @dirs = GetDirs( $amHere );

    if ( $DEBUG_ENABLED )
    {
        print "\n$amHere\n";
    }

    foreach my $dir ( @dirs )
    {
        DoSetup( "$target\\$dir" );

        CleanResults( "$target\\$dir" );

        DoCleanup( "$target\\$dir" );
    }

    if ( open(INI, "$EXPORTART_DIR\\$target\\pipe.ini") )
    {
        while ( <INI> )
        {
            if ( grep(/^ *RESULT /, $_) )
			{
				#remove the RESULT part
				$tempstr = $_;
				$tempstr =~ s/^ *RESULT //;
				chomp($tempstr);

				(my $source, my $dest) = split( / /, $tempstr );
                print "Source: $source\n";
                print "Dest: $dest\n";

                if ( !grep(/.+/, $dest) )
                {
                    #This is a syntatic error
                    print "========== Syntax Error: $_ needs a destination ==========\n";
                    print "========== $amHere: pipe.ini ==========\n";
                    $ERRORLEVEL = $SYNTAX_ERROR;
                    $SEVERITY = $255;
                    goto DIE;
                }   
                
                if ( $DEBUG_ENABLED )
                {
                    print "\n\nDelete Target: $RESULT_DIR\\$dest\n\n";
                }

                `del $RESULT_DIR\\$dest 1>$TEMP_LOG 2>$TEMP_ERR`;
                
                KillTempLog();
            }
        }
    }
    else
    {
        if ( $DEBUG_ENABLED )
        {
            print "At: ";
            print `cd`;
            print "\n";
            print "\n\nCan't find INI file\n\n";
        }
    }


DIE:
}


sub DoSetup
{
#parameters: directory

    my $thisDir = `cd`;
    chomp( $thisDir );

    if ( $DEBUG_ENABLED )
    {
        print "\n\nCalling Setup: @_[0]\\setup.bat\n";
        print "From here: $thisDir\n";
        print "Going to: $EXPORTART_DIR@_[0]\n"
    }

    chdir( "$EXPORTART_DIR@_[0]" );

    if ( $DEBUG_ENABLED )
    {
        print "Now here: ";
        print `cd`;
        print "\n";
    }

    `setup.bat 1>$TEMP_LOG 2>$TEMP_ERR`;

    if ( $DEBUG_ENABLED )
    {
        print GetTempLog();
    }
    
    KillTempLog();

    chdir( $thisDir );
    if ( $DEBUG_ENABLED )
    {
        print "Returned here: ";
        print `cd`;
        print "\n";
    }
}

sub DoCleanup
{
#parameters: directory

    my $thisDir = `cd`;
    chomp( $thisDir );

    if ( $DEBUG_ENABLED )
    {
        print "\n\nCalling Cleanup: @_[0]\\cleanup.bat\n";
        print "From here: $thisDir\n";
        print "Going to: $EXPORTART_DIR@_[0]\n"
    }

    chdir( "$EXPORTART_DIR@_[0]" );

    if ( $DEBUG_ENABLED )
    {
        print "Now here: ";
        print `cd`;
        print "\n";
    }

    `cleanup.bat 1>$TEMP_LOG 2>$TEMP_ERR`;
    
    if ( $DEBUG_ENABLED )
    {
        print GetTempLog();
    }

    KillTempLog();

    chdir( $thisDir );
    if ( $DEBUG_ENABLED )
    {
        print "Returned here: ";
        print `cd`;
        print "\n";
    }
}

#Do not edit below this line
1;
