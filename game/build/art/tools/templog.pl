#==============================================================================
# Templog.pl
#
# This file contains all the subroutines used in manipulating the temp.log
#
# + Created March 26 - C Brisebois
#==============================================================================


#==============================================================================
# GetTempLog
#
# Returns: the contents of the temp.log in the current dir.
#==============================================================================
sub GetTempLog
{
	my $tempString;

	if ( open(LOG, $TEMP_LOG) )
	{    
		while ( <LOG> )
		{
			$tempString = "$tempString $_";
		}

		close(LOG);
	}

	if ( open(LOG, $TEMP_ERR) )
	{    
		while ( <LOG> )
		{
			$tempString = "$tempString $_";
		}

		close(LOG);
	}

	$tempString;
}

#==============================================================================
# KillTempLog
#
# Returns: deletes the temp.log in the current directory
#==============================================================================
sub KillTempLog
{
    if ( open(LOG, $TEMP_LOG) )
    {
        close(LOG);
        `del /Q $TEMP_LOG`;
    }
}

#==============================================================================
# CheckTempLogForError
#
# Returns: deletes the temp.log in the current directory
#==============================================================================
sub CheckTempLogForError
{
    my $error = 0;
 
    $errorLog = GetTempLog();
    
	if ( grep( /Could not/i, $errorLog ) )
	{
		$error = 1;
		goto DIE;
	}
	
	if ( grep( /ERROR!/, $errorLog ) )
	{
		$error = 1;
		goto DIE;
	}

	if ( grep( /Cannot/i, $errorLog ) )
	{
		$error = 1;
		goto DIE;
	}

DIE:    
    $error;
}
#Do not edit below this line
1;
