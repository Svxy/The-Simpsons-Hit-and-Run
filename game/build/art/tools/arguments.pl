#==============================================================================
# Arguments.pl
#
# This file contains the argument parsing and header subroutines.
#
# + Created March 26 - C Brisebois
#==============================================================================


#==============================================================================
# ParseArgs
#
# Returns: Nothing.
#
# Parses the command-line arguments and sets the appropriate info.
# Prints the usage if an argument in unacceptable.
#==============================================================================
sub ParseArgs
{   
    my $passed = 1;
    my $argUsed = 0;
    my $platformsFound = 0;
    my $targetsFound = 0;
    my $optionsFound = 0;

    foreach my $arg (@ARGV)
    {
        $argUsed = 0;

        #Find platforms.
        for ( my $i = 0; $i < @PLATFORMS; $i++ )
        {
            if ( grep(/@{PLATFORMS[$i]}/, $arg) )
            {
                push @ACTIVE_PLATFORMS, $arg;

                $argUsed = 1;
                $platformsFound = 1
            }
        }

        #Find targets
        for ( my $i = 0; $i < @TARGETS; $i++ )
        {
            if ( grep(/@{TARGETS[$i]}/, $arg) )
            {
                push @ACTIVE_TARGETS, $arg;

                $argUsed = 1;
                $targetsFound = 1
            }
        }

        #Find clean
        if ( grep( /clean/i, $arg ) )
        {
            $CLEAN = 1;
            $argUsed = 1;
        }

        #Find artcheck
        if ( grep( /artcheck/i, $arg ) )
        {
            $ARTCHECK = 1;
            $argUsed = 1;
        }

        #Find options
        for ( my $i = 0; $i < @OPTIONS; $i++ )
        {
            if ( grep(/@{OPTIONS[$i]}/, $arg) )
            {
                push @ACTIVE_OPTIONS, $arg;

                $argUsed = 1;
                $optionsFound = 1
            }
        }


        if ( !$argUsed )
        {
            #This argument is not recognised.
            print "\nUnrecognised argument: $arg\n";
            $passed = 0;
        }
    }

    #Test to make sure all requirements are found.

    if ( !$platformsFound )
    {
        @ACTIVE_PLATFORMS = @PLATFORMS;
    }
        
    if ( !$targetsFound )
    {
        @ACTIVE_TARGETS = @TARGETS;
    }

    if ( !$optionsFound )
    {
        push @ACTIVE_OPTIONS, "NONE";
    }

    $passed;
}

#==============================================================================
# DisplayHeader
#
# Returns: Nothing.
#
# Prints the current settings of the running pipeline.
#==============================================================================
sub DisplayHeader
{
    print "\n\n";
    print "Running pipeline\n";
    print "=================\n";

    print "PLATFORMS: ";
    foreach my $target (@ACTIVE_PLATFORMS)
    {
        print "$target ";
    }
    print "\n";

    print "TARGETS: ";
    foreach my $target (@ACTIVE_TARGETS)
    {
        print "$target ";
    }
    print "\n";

    if ( $CLEAN )
    {
        print "clean\n";
    }

    print "OPTIONS: ";
    foreach my $option (@ACTIVE_OPTIONS)
    {
        print "$option ";
    }
    print "\n\n";

}


#Do not edit below this line
1;
