#!/bin/perl

#tools come first
require("tools\\directory.pl");
require("tools\\arguments.pl");
require("tools\\print.pl");
require("tools\\templog.pl");
require("tools\\commands.pl");
require("tools\\process.pl");

#setup info next
require("setup\\types.pl");
require("setup\\directories.pl");


#GLOBAL VALUES
#=============

@TARGETS = GetDirs( $EXPORTART_DIR );
@ACTIVE_TARGETS;

$CLEAN = 0;
$ARTCHECK = 0;

$DEBUG_ENABLED = 0;
$COMPRESSION_ENABLED = 0;
$QUICK_ENABLED = 0;
$UNSAFE = 0;

$TEMP_LOG = "temp.log";
$TEMP_ERR = "temp.err";

$ERRORLEVEL = $SUCCESS;


#======================= M A I N =========================

my $oldpath = $ENV{'PATH'};

foreach my $path (@BIN)
{
    $ENV{'PATH'} = "$ENV{'PATH'};$CURR$path";
}

#Add this so that batch files know where we are.
$ENV{'BUILDART'} = "$CURR"; 

my $passed = 0;

$passed = ParseArgs();

if ( $passed == 0 )
{
    goto PRINT_USAGE;
}

foreach my $option ( @ACTIVE_OPTIONS )
{
    if ( grep( /HELP/, $option ) )
    {
        goto PRINT_HELP;
    }

    if ( grep( /DEBUG/, $option) )
    {
        $DEBUG_ENABLED = 1;
    }

	if ( grep( /COMPRESS/, $option) )
	{
		$COMPRESSION_ENABLED = 1;
	}

	if ( grep( /QUICK/, $option) )
	{
		$QUICK_ENABLED = 1;
	}

	if ( grep( /UNSAFE/, $option) )
	{
		$UNSAFE = 1;
	}
}

DisplayHeader();
CreateDirs();

if ( $CLEAN )
{
    CleanPipeline();
}
else
{
    RunPipeline();
}
goto END;


PRINT_HELP:
PrintHelpFile();

PRINT_USAGE:
PrintUsage();

END:
#I think this is redundant.
$ENV{'PATH'} = $oldpath;