#==============================================================================
# Commands.pl
#
# This file contains the available commands and keywords, and all associated
# subroutines for executing them.
#
# + Created March 26 - C Brisebois
#==============================================================================


%COMMANDS = 
(
    ("PRINT",       "PrintExecute"),
    ("TEMPLATE",    "TemplateExecute"),
    ("COMMAND",     "CommandExecute"),
    ("STORE",       "StoreExecute"),
    ("RESULT",      "ResultExecute"),
    ("RULE",        "RuleExecute"),
    ("STORE_DEPEND","StoreDependExecute"),
    ("LIB_DEPEND",  "LibraryDependExecute"),
    ("LIB_REPLACE", "LibraryReplaceExecute")
);

@KEYWORDS =
(
    "DEPEND",
);

#==============================================================================
# ProcessCommand
#
# Returns: Nothing.
#
# Determines if the given command can be processed.  If it can be processed, 
# the subroutine will call the appropriate subroutine for executing the 
# command.
#==============================================================================
sub ProcessCommand
{
    $ERRORLEVEL = $SUCCESS;
    $SEVERITY = 0;

    my $commandFound = 0;

    #if this isn't a comment or an empty line.
    if ( !grep( /^ *\#/, @_[0] ) && !( length @_[0] == 1 ) )
    {
        #Find the command in the COMMANDS hash and evaluate it.
        if ( $ARTCHECK != 1 )
        {
            while ( (my $key, my $value) = each %COMMANDS )
            {
                if ( grep( /^ *${key} /, @_[0] ) )
                {
                    #remove the command
                    my $tempstr = @_[0];
                    $tempstr =~ s/^ *${key} //;
                    chomp($tempstr);
                
                    if ( $DEBUG_ENABLED )
                    {
                        print "$value(\"$tempstr\")\n";
                    }
                
                    #This evaluates the value of the key/value command hash.
                    eval "$value(\"$tempstr\")";

                    $commandFound = 1;
                }

                if ( $ERRORLEVEL != $SUCCESS )
                {
                    goto DIE;
                }
            }
        }

        if ( !$commandFound )
        {
            my $isKeyword = 0;

            #Ignore keywords
            foreach my $keyword (@KEYWORDS)
            {
                if ( grep( /^ *${keyword}/, @_[0] ) )
                {
                    $isKeyword = 1;

                    if ( $ARTCHECK == 1 && grep( /^ *DEPEND /, @_[0] ) )
                    {
                        ArtCheck( @_[0] );                        
                    }

                    break;
                }
            }

            if ( !$isKeyword && $ARTCHECK != 1 )
            {
                $ERRORLEVEL = $BAD_COMMAND;
                goto DIE;
            }
        }
    }

DIE:
    $ERRORLEVEL;
}

#==============================================================================
# PrintExecute
#
# Returns: Nothing.
#
# Executes PRINT commands by printing the string argument.
#==============================================================================
sub PrintExecute
{
#parameters: print line.
    
    print "@_\n";
    
    $SUCCESS;
}

#==============================================================================
# TemplateExecute
#
# Returns: Nothing.
#
# Executes TEMPLATE commands by finding the appropriate template and calling 
# process command on its contents.  This subroutine also replaces the
# templates INPUT and OUTPUT directives with the given input and output file
# names.
#==============================================================================
sub TemplateExecute
{
#parameters: string "template_type input_file output_file"

    my (my $type, my $input, my $output) = split( /\s+/, @_[0] );

    $ERRORLEVEL = $SUCCESS;
    $SEVERITY = 0;

    my $lineCount = 0;

    if ( open( TEMPLATE, "$TEMPLATES_DIR$type.$CURRENT_PLATFORM" ) )
    {
        if ( $DEBUG_ENABLED )
        {
            print "opened Template $type.$CURRENT_PLATFORM\n";
        }

        while ( <TEMPLATE> )
        {
            #need to modify the command to replace OUTPUT and INPUT with the correct values.
            my $newCommand = $_;

            while ( grep( /OUTPUT/, $newCommand ) )
            {
                $newCommand =~ s/OUTPUT/$output/;
            }

            while ( grep( /INPUT/, $newCommand ) )
            {
                $newCommand =~ s/INPUT/$input/;
            }

            ProcessCommand( $newCommand );

            #If there is text on the line and it is not a valid command.           
            if ( $ERRORLEVEL == $BAD_COMMAND )
            {
                print "\n========== Invalid command in template: $type line: $lineCount ==========\n";
                print "$_\n\n";
                $ERRORLEVEL = $GENERAL_FAILURE;
                goto DIE;
            }
            
            if ( $ERRORLEVEL != $SUCCESS )
            {
                #TODO: deal with different levels of severity
                print "========== Error too severe to continue. Severity: $SEVERITY ==========\n";
                $ERRORLEVEL = $GENERAL_FAILURE;
                goto DIE;
            }

            $lineCount++;
        }
        
DIE:
        close(TEMPLATE);       
    }
    else
    {
        print "========== Error opening template: $type.$CURRENT_PLATFORM ==========\n";
        $ERRORLEVEL = $BAD_TEMPLATE;
    }

    $ERRORLEVEL;
}

#==============================================================================
# CommandExecute
#
# Returns: Nothing.
#
# Executes COMMAND commands by calling the command argument in DOS.  Note: This
# command assumes that the DOS command is available in the environment PATH.
#==============================================================================
sub CommandExecute
{
#parameters:  command line

    $ERRORLEVEL = $SUCCESS;
    $SEVERITY = 0;

    print " @_[0]\n";
    `@_[0] 1>$TEMP_LOG 2>$TEMP_ERR`;
    
    if ( $UNSAFE != 1 &&
         ( $? != 0 ||
           CheckTempLogForError() != 0 ) )
    {
        print "!!! Error executing command!!!\n";
        $ERRORLEVEL = $COMMAND_ERROR;
        $SEVERITY = $?;
    }

    print GetTempLog();

    KillTempLog();
    print "\n";

    $ERRORLEVEL;
}

#TODO: Can we test to see if the dates are the same and not copy?  SafeCopy?
#==============================================================================
# StoreExecute
#
# Returns: Nothing.
#
# Executes STORE commands by taking the input file and storing it in the 
# ACTIVE_STORE location with the destination name...  The destination name
# may contain relative path information.
#==============================================================================
sub StoreExecute
{
#parameters: "source file dest file"

    $ERRORLEVEL = $SUCCESS;
    $SEVERITY = 0;

    (my $source, my $dest) = split( / /, @_[0] );
    
    #create directories if needed.
    #The files names can contain extra dir info too.  Need to split that out.
    my @possibleDirs = split( /\\/, $dest );

    my $destName = pop @possibleDirs;
    my $dir = "";

    foreach my $thing (@possibleDirs)
    {
        $dir = "$dir$thing\\";
    }

    #make the Directory if needed.
    #does it already exist?
    if ( !stat( "$ACTIVE_STORE\\$dir" ) )
    {
        if ( $DEBUG_ENABLED )
        {
            print "mkdir $ACTIVE_STORE\\$dir\n";
        }
        `mkdir $ACTIVE_STORE\\$dir`;     
    }

    print "STORE $destName";

    if ( $DEBUG_ENABLED )
    {
        print "copy $source $ACTIVE_STORE\\$dest\n";
    }
    `copy $source $ACTIVE_STORE\\$dest 1>$TEMP_LOG 2>$TEMP_ERR`;
    utime( time, time, "$ACTIVE_STORE\\$dest" );

    if ( $? )
    {
        print "!!! Error executing command!!!\n";
        $ERRORLEVEL = $COMMAND_ERROR;
        $SEVERITY = $?;
    }

    print GetTempLog();

    KillTempLog();
    print "\n";

    $ERRORLEVEL;
}

#==============================================================================
# ResultExecute
#
# Returns: Nothing.
#
# Executes RESULT commands by copying the source file to the RESULT_DIR (usually
# cd\<platform>\art.  The file is saved using the destination name.  Note: the
# destination name may contain relative directory info.
#==============================================================================
sub ResultExecute
{
#parameters: "source file dest file"

    $ERRORLEVEL = $SUCCESS;
    $SEVERITY = 0;

    (my $source, my $dest) = split( / /, @_[0] );
    
    #create directories if needed.
    #The files names can contain extra dir info too.  Need to split that out.
    my @possibleDirs = split( /\\/, $dest );

    my $destName = pop @possibleDirs;
    my $dir = "";

    foreach my $thing (@possibleDirs)
    {
        $dir = "$dir$thing\\";
    }

    #make the Directory if needed.
    #does it already exist?
    if ( !stat( "$RESULT_DIR\\$dir" ) )
    {
        if ( $DEBUG_ENABLED )
        {
            print "mkdir $RESULT_DIR\\$dir\n";
        }
        `mkdir $RESULT_DIR\\$dir`;     
    }

    #Remove all the history, export, and camera chunks unless we're in debug mode
    if ( !$DEBUG_ENABLED )
    {
		if ( grep( /\.p3d/, $source ) )
        {
            `p3ddel -d 7000 -d 7030 -n $source -o $source`;
        }
    }

	if ( $COMPRESSION_ENABLED && grep( /\.p3d/, $source ) )
	{
	    $bigendianswitch = "";
	    
	    if ( grep( /GC/, $CURRENT_PLATFORM ) )
	    {
	        $bigendianswitch = "";
	    }
	    
		print "compressing $source\n";
		if ( $QUICK_ENABLED )
		{
    		print "compressing p3dcompress $bigendianswitch -f $source\n";
			`p3dcompress $bigendianswitch -f $source 1>$TEMP_LOG 2>$TEMP_ERR`;
		}
		else
		{
    		print "compressing p3dcompress $bigendianswitch $source\n";
			`p3dcompress $bigendianswitch $source 1>$TEMP_LOG 2>$TEMP_ERR`;
		}
		
	    print GetTempLog();		 
	}

    print "RESULT $destName";

    if ( $DEBUG_ENABLED )
    {
        print "copy $source $RESULT_DIR\\$dest\n";
    }
    `copy $source $RESULT_DIR\\$dest 1>$TEMP_LOG 2>$TEMP_ERR`;
    utime( time, time, "$RESULT_DIR\\$dest" );

    if ( $? )
    {
        print "!!! Error executing command!!!\n";
        $ERRORLEVEL = $COMMAND_ERROR;
        $SEVERITY = $?;
    }

    print GetTempLog();

    KillTempLog();
    print "\n";

    $ERRORLEVEL;   
}

#==============================================================================
# RuleExecute
#
# Returns: Nothing.
#
# Executes the RULE command by calling P3DRULE and passing in the rule file, 
# the error threshold for dieing and the input file to process.  If the given
# rule file is not in the current directory, then the templates directory will
# be searched for the same name PLUS a platform extension.  
# eg. mesh.rul->mesh.rul.XBOX
#==============================================================================
sub RuleExecute
{
    $ERRORLEVEL = $SUCCESS;
    $SEVERITY = 0;

    ( my $ruleFile, my $dieLevel, my $inputFile ) = split( / /, @_[0], 3 );

    #Test to see if the rule file is local, otherwise try to use a template rule.
    my $localRule = 0;
    if ( open( RULE, "$ruleFile" ) )
    {
        $localRule = 1;
        close(RULE);
    }

    if ( !$localRule )
    {
        $templateRuleFound = 0;
        #Try to retrieve a template dir rule.
        if ( open( RULE, "$TEMPLATES_DIR$ruleFile.$CURRENT_PLATFORM" ) )
        {
            $templateRuleFound = 1;
            $ruleFile = "$TEMPLATES_DIR$ruleFile.$CURRENT_PLATFORM";
            close(RULE);
        }

        if ( !$templateRuleFound )
        {
            $ERRORLEVEL = $BAD_FILE;
            $SEVERITY = 255;
            print "========== Error opening rule file: $ruleFile ==========\n";
            goto DIE;
        }
    }

    # ERRORLEVEL 1 is warning, 2 is error.   
    $ERRORLEVEL = CommandExecute( "p3drule -e $dieLevel -w 0 -r $ruleFile $inputFile" );
    
    print "ERRORLEVEL: $ERRORLEVEL\n";

    if ( $ERRORLEVEL > 0 )
    {
        if ( $SEVERITY == 1 )
        {
            #This is a warning.
            $ERRORLEVEL = $SUCCESS;
            $SEVERITY = 0;
        }

        if ( $SEVERITY == 2 )
        {
            #This is an error.
        }
    }

DIE:
    $ERRORLEVEL;
}

#==============================================================================
# StoreDependExecute
#
# Returns: Nothing.
#
# Executes STORE_DEPEND commands by copying the given file from the 
# ACTIVE_STORE directory.  Note: The filenames may contain relative path info.
#==============================================================================
sub StoreDependExecute
{
    $ERRORLEVEL = $SUCCESS;
    $SEVERITY = 0;

    my @files = split( / /, @_[0] );

    foreach my $file (@files)
    {
        if ( $DEBUG_ENABLED )
        {
            print "copy $ACTIVE_STORE\\$file\n";
        }

        `copy $ACTIVE_STORE\\$file . 1>$TEMP_LOG 2>$TEMP_ERR`;

        if ( $? )
        {
            print "!!! Error executing command!!!\n";
            $ERRORLEVEL = $COMMAND_ERROR;
            $SEVERITY = $?;
        }

        print GetTempLog();

        KillTempLog();
        print "\n";
    }

    $ERRORLEVEL;
}

#==============================================================================
# LibraryDependExecute
#
# Returns: Nothing.
#
# Executes LIB_DEPEND commands by copying the given file from the 
# LIBRARY directory.  Note: The filenames may contain relative path info.
#==============================================================================
sub LibraryDependExecute
{
    $ERRORLEVEL = $SUCCESS;
    $SEVERITY = 0;

    my @files = split( / /, @_[0] );

    foreach my $file (@files)
    {
        if ( $DEBUG_ENABLED )
        {
            print "copy $EXPORTARTLIBRARY_DIR\\$file\n";
        }

        `copy $EXPORTARTLIBRARY_DIR\\$file . 1>$TEMP_LOG 2>$TEMP_ERR`;

        if ( $? )
        {
            print "!!! Error executing command!!!\n";
            $ERRORLEVEL = $COMMAND_ERROR;
            $SEVERITY = $?;
        }

        print GetTempLog();

        KillTempLog();
        print "\n";
    }

    $ERRORLEVEL;
}

#==============================================================================
# LibraryReplaceExecute
#
# Returns: Nothing.
#
# Executes LIB_REPLACE by calling the instancer tool on the files passed in and 
# the library directory.
#==============================================================================
sub LibraryReplaceExecute
{
#parameters: filenames

    $ERRORLEVEL = $SUCCESS;
    $SEVERITY = 0;

    my ( my $dir, my $file ) = split( / /, @_[0] );

    #This is funny...
    if ( grep( /\.p3d/, $dir ) && !grep( /${file}/, $dir ) )
    {
        $file = $dir;
    }

    if ( grep( /\.p3d/, $file ) )
    {
        if ( grep( /${file}/, $dir ) )
        {
            $dir = "";
        }
                
        #This is a hack to process lib files for Devin
        
        my @libFiles = GetFiles( "$EXPORTARTLIBRARY_DIR\\$dir" );
        foreach my $exportFile ( @libFiles )
        {
            my @storedstats = stat("$EXPORT_LIBRARIES$CURRENT_PLATFORM\\$dir\\$exportFile");
            my @libstats = stat("$EXPORTARTLIBRARY_DIR\\$dir\\$exportFile");
            
		    if ( !@storedstats || @storedstats[9] < @libstats[9] )
		    {
                if ( $DEBUG_ENABLED )
                {
                    print " Applying HACK to $EXPORT_ART_WORK_DIR\\$exportFile";
                }

                `copy $EXPORTARTLIBRARY_DIR\\$dir\\$exportFile .`;
                TemplateExecute( "libanim $exportFile $exportFile" );
                mkdir( "$EXPORT_LIBRARIES$CURRENT_PLATFORM\\$dir", 0777 );
                `move .\\$exportFile $EXPORT_LIBRARIES$CURRENT_PLATFORM\\$dir\\$exportFile`;
            }
        }

        print " instancer -o $file $file -l $EXPORT_LIBRARIES$CURRENT_PLATFORM\\$dir \n";

        `instancer -o $file $file -l $EXPORT_LIBRARIES$CURRENT_PLATFORM\\$dir 1>$TEMP_LOG 2>$TEMP_ERR`;
# OLD    `instancer -o $file $file -l $EXPORTARTLIBRARY_DIR\\$dir 1>$TEMP_LOG 2>$TEMP_ERR`;
#        `instancer -d -o $file $file -l $EXPORTARTLIBRARY_DIR\\$dir 1>$TEMP_LOG 2>$TEMP_ERR`;

    if ( $UNSAFE != 1 &&
         ( $? != 0 ||
           CheckTempLogForError() != 0 ) )
        {
            print "!!! Error executing command!!!\n";
            $ERRORLEVEL = $COMMAND_ERROR;
            $SEVERITY = $?;
        }

        print GetTempLog();

        KillTempLog();
        print "\n";
    }

    $ERRORLEVEL;
}

##########################
# Keyword based commands #
##########################

#==============================================================================
# ArtCheck
#
# Returns: Nothing.
#
# Checks the files according tot he art checker and spews the data.
#==============================================================================
sub ArtCheck
{
    #remove the DEPEND
    my $tempstr = @_[0];
    $tempstr =~ s/^ *DEPEND //;
    chomp($tempstr);

    my @files = split( / /, $tempstr );

    foreach my $file (@files)
    {
        if ( $DEBUG_ENABLED )
        {
            print "artchecker $file\n";
        }

        `artchecker $file 1>$TEMP_LOG 2>$TEMP_ERR`;

        print GetTempLog();

        print "\nPaused for verification! Press any key to continue\n";
        `pause`;

        KillTempLog();
        print "\n";
    }
}

#Do not edit below this line
1;
