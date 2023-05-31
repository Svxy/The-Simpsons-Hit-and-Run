#==============================================================================
# Directory.pl
#
# This file contains the utilities for dealing with directories.
#
# + Created March 26 - C Brisebois
#==============================================================================


#==============================================================================
# GetFiles
#
# Returns: a list of files in the given directory.
#==============================================================================
sub GetFiles
{
    my @ALL = `dir /b /A-D @_ 2> $TEMP_LOG`;

    KillTempLog();

    foreach my $file (@ALL)
    {
        chomp($file);
    }

    @ALL;
}

#==============================================================================
# GetDirs
#
# Returns: a list of directories in the given directory.
#==============================================================================
sub GetDirs
{
    my @ALL = `dir/b /A:D @_`;

    foreach my $file (@ALL)
    {
        chomp($file);
    }
    @ALL;
}

#==============================================================================
# GetCurDir
#
# Returns: the current directory.
#==============================================================================
sub GetCurDir
{
    my $dir = `cd`;

    chomp($dir);
    $dir;
}

#==============================================================================
# GetCurDirRelativeToTEMP
#
# Returns: the current directory with the components of the TEMP dir removed.
#==============================================================================
sub GetCurDirRelativeToTEMP
{
    my $dir = `cd`;

    chomp($dir);

    $dir = reverse( $dir );

    #Remove the directory stuff from TEMP_DIR
    for ( my $i = 0; $i < length $TEMP_DIR; $i++ )
    {
        chop( $dir );
    }

    $dir = reverse( $dir );

    $dir;
}

#==============================================================================
# PrintDir
#
# Returns: a string containing all the files in the given dir.
#==============================================================================
sub PrintDir
{
    my $tempString;

    foreach my $file (@_)
    {
        $tempString = "$tempString $file";
    }
 
    $tempString;
}        

#==============================================================================
# CreateDirs
#
# Returns: Nothing.
#
# Creates the TEMP, STORE and ERRORS dir if they do not already exist.
#==============================================================================
sub CreateDirs()
{
    my @dirs = GetDirs( $WORK_DIR );
    my $tempgood = 0;
    my $errorgood = 0;
    my $storegood = 0;
    my $elgood = 0;

    foreach my $dir ( @dirs )
    {
        if ( grep(/${dir}/, $TEMP_DIR_NAME) )
        {
            $tempgood = 1;
        }

        if ( grep(/${dir}/, $ERRORS_DIR_NAME) )
        {
            $errorgood = 1;
        }

        if ( grep(/${dir}/, $STORE_DIR_NAME) )
        {
            $storegood = 1;
        }
        if ( grep(/${dir}/, $EXPORT_LIBRARIES) )
        {
            $elgood = 1;
        }
    }

    if ( $tempgood )
    {
        `del /Q $TEMP_DIR*.*`;
    }
    else
    {
        mkdir( "$TEMP_DIR", 0777 );
    }
    
    if ( $errorgood )
    {
        `del /Q $ERRORS_DIR*.*`;
    }
    else
    {
        mkdir( "$ERRORS_DIR", 0777 );
    }

    if ( $storegood )
    {
        #Check the sub dirs
        @dirs = GetDirs($STORE_DIR);

        int @platgood[@PLATFORMS];
        for ( my $i = 0; $i < @PLATFORMS; $i++ )
        {
            @platgood[$i] = 0;
        }

        foreach my $dir (@dirs)
        {
            for ( my $i = 0; $i < @PLATFORMS; $i++ )
            {
                if ( grep(/${dir}/, @PLATFORMS[$i] ) )
                {
                    @platgood[$i] = 1;
                }
            }
        }

        for ( my $i = 0; $i < @PLATFORMS; $i++ )
        {
            if ( @platgood[$i] == 0 )
            {
                mkdir( "$STORE_DIR@PLATFORMS[$i]", 0777 );
            }
        }
    }
    else
    {
        `mkdir $STORE_DIR`;
        
        for ( my $i = 0; $i < @PLATFORMS; $i++ )
        {
            mkdir( "$STORE_DIR@PLATFORMS[$i]", 0777 );
        }
    }

    if ( $elgood )
    {
        #Check the sub dirs
        @dirs = GetDirs($EXPORT_LIBRARIES);

        int @platgood[@PLATFORMS];
        for ( my $i = 0; $i < @PLATFORMS; $i++ )
        {
            @platgood[$i] = 0;
        }

        foreach my $dir (@dirs)
        {
            for ( my $i = 0; $i < @PLATFORMS; $i++ )
            {
                if ( grep(/${dir}/, @PLATFORMS[$i] ) )
                {
                    @platgood[$i] = 1;
                }
            }
        }

        for ( my $i = 0; $i < @PLATFORMS; $i++ )
        {
            if ( @platgood[$i] == 0 )
            {
                mkdir( "$EXPORT_LIBRARIES@PLATFORMS[$i]", 0777 );
            }
        }
    }
    else
    {
        `mkdir $EXPORT_LIBRARIES`;
        
        for ( my $i = 0; $i < @PLATFORMS; $i++ )
        {
            mkdir( "$EXPORT_LIBRARIES@PLATFORMS[$i]", 0777 );
        }
    }
}
    
#Do not edit below this line
1;

