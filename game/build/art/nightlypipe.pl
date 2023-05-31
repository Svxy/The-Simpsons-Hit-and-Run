require(".\\nightlypipe\\functions.pl");
require(".\\nightlypipe\\p4.pl");

$postie = ".\\..\\tools\\mail\\postie.exe";
my $CDDir = ".\\..\\..\\cd";
my $levelslistfilename = ".\\nightlypipe\\levels.txt";
my $maillistfilename = ".\\nightlypipe\\maillist.txt";
my $platformlistfilename = ".\\nightlypipe\\platforms.txt";

@PLATFORMS = 
(
    "GC",
    "PS2", 
    "XBOX",
    "PC"
);

open(MAILLIST, $maillistfilename);
$maillist = <MAILLIST>;
close(MAILLIST);

open(LEVELSLIST,$levelslistfilename);
my @levelslist= <LEVELSLIST>;
close(LEVELSLIST);

$platformsFound = 0;
if(@ARGV > 0)
{
	print("getting arguements from the command line\n");
	$arg = pop(@ARGV);
	{
		for ( my $i = 0; $i < @PLATFORMS; $i++ )
		{
			if ( grep(/@{PLATFORMS[$i]}/, $arg) )
			{
				$platformsFound += 1;
				push(@$platformlist, $arg);
			}
		}
	}
}else
{
	print("loading platforms.txt\n");
	open(PLATFORMLIST, $platformlistfilename);
	@platformlist = <PLATFORMLIST>;
	close(PLATFORMLIST);
	$platformsFound = @platformlist;
	print("$platformsFound will be processed\n");
	print("platformlist: @platformlist\n");

}

if($platformsFound == 0 )
{
	print("NO PLATFORM SPECIFIED\n");
	exit 1;
}

my $emailsummary;
foreach $platform (@platformlist)
{
	chomp $platform;
	print("CURRENT PLATFORM: $platform\n");
	$emailsummary = $emailsummary."CURRENT PLATFORM: $platform\n";

	foreach $level (@levelslist)
	{
		@badfiles = "";
		chomp $level;
		print("WORKING ON $level\n");
		
		P4Checkout(".\\nightlypipe\\$level.txt", $platform);

		system("no_pause_pipe.bat $platform levels:$level clean");
		system("no_pause_pipe.bat $platform levels:$level COMPRESS");
		$CDPlatformDir = "$CDDir\\$platform\\art\\";
		print("CDPlatformDir: $CDPlatformDir\n");
		
		if(filechecker(".\\nightlypipe\\$level.txt", $CDPlatformDir))
		{
			&mailresults(@badfiles, $maillist);
			$emailsummary = $emailsummary."$level: FAILED - run for the hills!\n";
			print("The bad files are: \n@badfiles\n");
			P4RevertAll(".\\nightlypipe\\$level.txt", $platform);
		}
		else
		{
			$emailsummary = $emailsummary."$level: OK\n";
			#P4RevertUnchanged(".\\nightlypipe\\$level.txt", $platform);
			P4Checkin(".\\nightlypipe\\$level.txt", $platform);
		}
	}
	$emailsummary = $emailsummary."\n";
}
SendSummaryEmail($emailsummary);



