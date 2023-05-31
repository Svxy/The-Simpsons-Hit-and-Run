#==============================================================================
# Print.pl
#
# This file contains the print commands used by the pipeline.  This does not
# include the PRINT command.
#
# + Created March 26 - C Brisebois
#==============================================================================


#==============================================================================
# PrintUsage
#
# Returns: Nothing.
#
# This subroutine prints the available settings of the pipeline as specified by
# the setup\types.pl, the commands.pl, and the export art directory
#==============================================================================
sub PrintUsage
{
    #Display usage
    print "\n\nArt Pipeline V2.0\n";
    print "USAGE: pipe [PLATFORM] [TARGET] [clean] [artcheck] [OPTIONS]\n\n";

    #Display platforms
    print"\tPLATFORM : [ ";

    foreach my $platform ( @PLATFORMS )
    {
        print "$platform | ";
    }

    print "ALL_PLAT ]\n";
    print "\t\t default - PS2\n\n";

    #Display targets
    print"\tTARGET : [ ";
    foreach my $target ( @TARGETS )
    {
        print "$target | ";
    }

    print "all ]\n";
    print "\t\t default - all\n\n";

    #Display options
    print"\tOPTIONS : [ ";
    foreach my $option ( @OPTIONS )
    {
        print "$option | ";
    }

    print "NONE ]\n";
    print "\t\t default - NONE\n\n";

}

#==============================================================================
# PrintHelpFile
#
# Returns: Nothing.
#
# This subroutine prints the help.txt file.
#==============================================================================
sub PrintHelpFile
{
    if ( open(HELPFILE, "help.txt") )
    {    
        while ( <HELPFILE> )
        {
            print $_;
        }

        close(HELPFILE);
    }   
}

#==============================================================================
# PrintUnderline
#
# Returns: Nothing.
#
# This subroutine prints a number of '=' equal to the value passed in.
#==============================================================================
sub PrintUnderline
{
#parameters: number of =

    my $length = @_[0];

    for ( my $i = 0; $i < $length; $i++ )
    {
        print "=";
    }
    print"\n";
}
#Do not edit below this line
1;
