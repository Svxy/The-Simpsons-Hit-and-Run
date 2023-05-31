################################################################################
#
# FE Pipeline - Main
# 
# Author(s):	Tony C.T. Chu, Programmer
#
################################################################################

print "\n";

$argc = $#ARGV + 1; # number of command-line arguments

if( $argc == 0 )
{
	goto USAGE;
}

@PLATFORMS =
(
	'gc',
	'ps2',
	'xbox',
	'pc',

	''
);

$allplatforms = 1;
$gc = '';
$ps2 = '';
$xbox = '';
$pc = '';

$buildwhat = '';

$compress = 0; # compression OFF, by default

for( $i = 0; $i < $argc; $i++ )
{
	$arg = $ARGV[ $i ];

	if( $arg eq '-h' || $arg eq 'help' )
	{
		goto USAGE;
	}
	elsif( $arg eq '-g' )
	{
		$allplatforms = 0;
		$gc = 'gc';
	}
	elsif( $arg eq '-p' )
	{
		$allplatforms = 0;
		$ps2 = 'ps2';
	}
	elsif( $arg eq '-x' )
	{
		$allplatforms = 0;
		$xbox = 'xbox';
	}
	elsif( $arg eq '-w' )
	{
	    $allplatforms = 0;
	    $pc = 'pc';
	}
	elsif( $arg eq 'compress' || $arg eq 'final' )
	{
		$compress = 1;
	}
	elsif( $arg eq 'nocompress' )
	{
		$compress = 0;
	}
	else
	{
		$buildwhat = $arg;
	}
}

if( $allplatforms == 0 )
{
	$PLATFORMS[ 0 ] = $gc;
	$PLATFORMS[ 1 ] = $ps2;
	$PLATFORMS[ 2 ] = $xbox;
	$PLATFORMS[ 3 ] = $pc;
}

for( $i = 0; $i < $#PLATFORMS; $i++ )
{
	$platform = $PLATFORMS[ $i ];

	if( $platform eq '' )
	{
		next;
	}

	if( $buildwhat =~ /all/ )
	{
	}
	elsif( $buildwhat =~ /clean/ )
	{
	}
	elsif( $buildwhat =~ /scrooby/ )
	{
		print "<< START >> Building FE - Scrooby for [$platform] ... ...\n";

		if( $platform eq 'gc' )
		{
			system "build\\art\\frontend\\buildgc.bat";
		}

		if( $platform eq 'ps2' )
		{
			system "build\\art\\frontend\\buildps2.bat";
		}

		if( $platform eq 'xbox' )
		{
			system "build\\art\\frontend\\buildxbox.bat";
		}
		
		if( $platform eq 'pc' )
		{
		    system "build\\art\\frontend\\buildpc.bat";
		}

		print "<<  END  >> DONE building FE - Scrooby for [$platform].\n\n";
	}
	elsif( $buildwhat =~ /quick/ )
	{
	}
	elsif( $buildwhat =~ /fonts/ )
	{
	}
	elsif( $buildwhat =~ /text/ )
	{
	}
	elsif( $buildwhat =~ /dynamic/ )
	{
		print "<< START >> Building FE - Dynamic Resources for [$platform] ... ...\n";

		$options = $buildwhat;
		$options =~ s/dynamic//;
		$options =~ s/://;

		system "perl build\\art\\frontend\\builddynamic.pl $platform $compress $options";

		print "<<  END  >> DONE building FE - Dynamic Resources for [$platform].\n\n";
	}
	else
	{
		goto USAGE;
	}
}

### TC: [TEMP ONLY] Until FE pipeline revisions are fully complete.

if( $buildwhat =~ /all/ )
{
	print "Building FE - All ... ...\n";

	system "build\\art\\frontend\\buildtext.bat";
	system "build\\art\\frontend\\buildfonts.bat";
	system "build\\art\\frontend\\buildgc.bat";
	system "build\\art\\frontend\\buildps2.bat";
	system "build\\art\\frontend\\buildxbox.bat";

	if( $compress )
	{
		system "build\\art\\frontend\\buildfinal.bat";
	}
}
elsif( $buildwhat =~ /clean/ )
{
	print "Cleaning FE ... ...\n";

	system "build\\art\\frontend\\buildclean.bat";
}
elsif( $buildwhat =~ /quick/ )
{
	print "Building FE - Quick ... ...\n";

	system "build\\art\\frontend\\buildquick.bat";
}
elsif( $buildwhat =~ /fonts/ )
{
	print "Building FE - Fonts ... ...\n";

	system "build\\art\\frontend\\buildfonts.bat";
}
elsif( $buildwhat =~ /text/ )
{
	print "Building FE - Text Bible ... ...\n";

	system "build\\art\\frontend\\buildtext.bat";
}

exit;

USAGE:
print "USAGE: build [-g] [-p] [-x] [-w] [all|clean|scrooby|quick|fonts|text|dynamic][:<options>] [nocompress|compress|final]\n\n";
exit;

################################################################################
