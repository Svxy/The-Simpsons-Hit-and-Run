################################################################################
#
# FE Pipeline - Dynamic Resources
# 
# Author(s):	Tony C.T. Chu, Programmer
#
################################################################################

$PLATFORM = $ARGV[ 0 ];
$COMPRESS = $ARGV[ 1 ];
$OPTIONS = $ARGV[ 2 ];

$PLATFORM_DIR = "$PLATFORM\\";
$CD_DIR = "cd\\";
$FE_DYNALOAD_DIR = "art\\frontend\\dynaload\\";
$FE_BUILD_DIR = "build\\art\\frontend\\";

$IMAGES_BAT = "images.bat";
$PLATFORM_BAT = "platform.bat";
$COMPRESS_BAT = "compress.bat";

if( $OPTIONS eq '' || $OPTIONS =~ /images/ )
{
	system "P4 edit //depot/game/cd/$PLATFORM/art/frontend/dynaload/images/...";
	system "xcopy exportart\\frontend\\dynaload\\images\\*.* cd\\$PLATFORM\\art\\frontend\\dynaload\\images\\ /D /S /Y";
	system "xcopy exportart\\frontend\\dynaload\\_$PLATFORM\\images\\*.* cd\\$PLATFORM\\art\\frontend\\dynaload\\images\\ /S /Y";

	@IMAGE_DIRS =
	(
		"images\\",
		"images\\cars2D\\",
		"images\\license\\",
		"images\\license\\french\\",
		"images\\license\\german\\",
		"images\\license\\spanish\\",
		"images\\loading\\",
		"images\\loading\\french\\",
		"images\\loading\\german\\",
		"images\\loading\\spanish\\",
		"images\\msnicons\\char\\",
		"images\\msnicons\\location\\",
		"images\\msnicons\\object\\",
		"images\\msnicons\\vehicle\\",
		"images\\scrapbook\\",
		"images\\skins2D\\",

		""
	);

	for( $j = 0; $j < $#IMAGE_DIRS; $j++ )
	{
		$imagedir = $IMAGE_DIRS[$j];

		$dir = $CD_DIR.$PLATFORM_DIR.$FE_DYNALOAD_DIR.$imagedir;

		$backdir = $dir;
		$backdir =~ s/.\w*\\/..\\/g;

		$command = $FE_BUILD_DIR.$PLATFORM_DIR.$IMAGES_BAT;
		print ">> Running: $command\n";
		system "$command $dir $backdir";

		$command = $FE_BUILD_DIR.$PLATFORM_DIR.$PLATFORM_BAT;
		print ">> Running: $command\n";
		system "$command $dir $backdir";

		if( $COMPRESS )
		{
			$command = $FE_BUILD_DIR.$PLATFORM_DIR.$COMPRESS_BAT;
			print ">> Running: $command\n";
			system "$command $dir $backdir";
		}
	}
}

if( $OPTIONS eq '' || $OPTIONS =~ /cars/ )
{
	system "P4 edit //depot/game/cd/$PLATFORM/art/frontend/dynaload/cars/...";
	system "xcopy exportart\\frontend\\dynaload\\cars\\*.* cd\\$PLATFORM\\art\\frontend\\dynaload\\cars\\ /S /Y";

	@CAR_DIRS =
	(
		"cars\\",

		""
	);

	for( $j = 0; $j < $#CAR_DIRS; $j++ )
	{
		$cardir = $CAR_DIRS[$j];

		$dir = $CD_DIR.$PLATFORM_DIR.$FE_DYNALOAD_DIR.$cardir;

		$backdir = $dir;
		$backdir =~ s/.\w*\\/..\\/g;

		$command = $FE_BUILD_DIR.$PLATFORM_DIR.$PLATFORM_BAT;
		print ">> Running: $command\n";
		system "$command $dir $backdir";

		if( $COMPRESS )
		{
			$command = $FE_BUILD_DIR.$PLATFORM_DIR.$COMPRESS_BAT;
			print ">> Running: $command\n";
			system "$command $dir $backdir";
		}
	}
}

################################################################################
