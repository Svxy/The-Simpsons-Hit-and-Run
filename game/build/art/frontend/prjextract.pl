
for( $i = 0; $i < $#ARGV+1; $i++ )
{
	if( ($ARGV[$i] eq "i") || ($ARGV[$i] eq "--images") )
	{
		$printimages = 1;
	}
	if( ($ARGV[$i] eq "s") || ($ARGV[$i] eq "--text-styles") )
	{
		$printtextstyles = 1;
	}
	if( ($ARGV[$i] eq "b") || ($ARGV[$i] eq "--text-bibles") )
	{
		$printtextbibles = 1;
	}
	if( ($ARGV[$i] eq "p") || ($ARGV[$i] eq "--p3d-files") )
	{
		$printp3dfiles = 1;
	}
	if( ($ARGV[$i] eq "a") || ($ARGV[$i] eq "--all") )
	{
# TC: exclude text bible
#		$printtextbibles = 1;

		$printtextstyles = 1;
		$printimages = 1;

# TC: exclude p3d files
#		$printp3dfiles = 1;
	}
	if( $ARGV[$i] eq "--nice" )
	{
		$printnice = 1;
	}
	if( $ARGV[$i] eq "--srr2" )
	{
		$srr2 = 1;

		do
		{
			$i++;
			$srr2_params = "$srr2_params $ARGV[$i]";
		}
		while( $i < $#ARGV+1 );

		last;
	}
	if( $ARGV[$i] eq "--help" )
	{
		print "Project File Resource Extractor\n";
		print "\tusage : prjextract.pl <projectname> [options]\n";
		print "\nParameters : \n";
		print "  projectname     \tthe scrooby .prj file to load\n";
		print "\nOptions :\n";
		print "  a, --all        \tprint all resources\n";
		print "  i, --images     \tprint image resources\n";
		print "  s, --text-styles\tprint textstyle resources\n";
		print "  b, --text-bibles\tprint textbible resources\n";
		print "  --nice          \tprint out in line by line format\n";
		print "  --help          \tdisplay this message\n";
		exit;
	}
}

open( INPUT, "<$ARGV[0]" ) or die( "Failed to open file $ARGV[0]" );
$projectname = $ARGV[0];
$projectname =~ s/.prj//;

print "\n*** Extracting resource files for $projectname.prj\n";

@screens;
$screenindex = 0;
$pagepath = "Pages/";
$screenpath = "Screens/";
$resourcepath = "Resource/";
while( $line = <INPUT> )
{
	if( $line =~ /<Screen file=\"(.*)\">/ )
	{
		$screens[$#screens+1] = $1;
	}
	if ( $line =~ /<ScreenPath value=\"(.*)\">/ )
	{
		$screenpath = $1;
	}
	if ( $line =~ /<PagePath value=\"(.*)\">/ )
	{
		$pagepath = $1;
	}
	if ( $line =~ /<ResourcePath value=\"(.*)\">/ )
	{
		$resourcepath = $1;
	}
}
close INPUT;

@pages;
for( $i=0; $i < $#screens+1; $i++ )
{
	open( INPUT, "<$screenpath$screens[$i]" ) or die( "Could not open screen file $screens[$i]" );
	while( $line = <INPUT> )
	{
		if( $line =~ /<Page file=\"(.*)\" export/ )
		{
			$pages[$#pages+1] = $1;
		}
	}
	close INPUT;	
}

@images;
@textbibles;
@textstyles;
for( $i=0; $i < $#pages+1; $i++ )
{
	open( INPUT, "<$pagepath$pages[$i]" ) or die( "Could not open screen file $pages[$i]" );
	while( $line = <INPUT> )
	{
		if( $line =~ /<TextBible name=\"(.*)\" data=\"(.*)\" / )
		{
			$found = false;
			for( $j=0; $j < $#textbibles+1; $j++ )
			{
				if( $2 eq $textbibles[$j] )
				{
					$found = true;
					break;
				}				
			}
			if( $found == false )
			{
				$textbibles[$#textbibles+1] = $2;
			}
		}
		elsif( $line =~ /<TextStyle name=\"(.*)\" data=\"(.*)\" / )
		{
			$found = false;
			for( $j=0; $j < $#textstyles+1; $j++ )
			{
				if( $2 eq $textstyles[$j] )
				{
					$found = true;
					break;
				}				
			}
			if( $found == false )
			{
				$textstyles[$#textstyles+1] = $2;
			}
		}
		elsif( $line =~ /<Image name=\"(.*)\" data=\"(.*)\"/ )
		{
			$found = false;
			for( $j=0; $j < $#images+1; $j++ )
			{
				if( $2 eq $images[$j] )
				{
					$found = true;
					break;
				}				
			}
			if( $found == false )
			{
				$images[$#images+1] = $2;
			}
		}
		elsif( $line =~ /<Pure3dFile name=\"(.*)\" data=\"(.*)\" Pure3dInventoryName=\"(.*)\"/ )
		{
			$found = false;
			for( $j=0; $j < $#p3dfiles+1; $j++ )
			{
				if( $2 eq $p3dfiles[$j] )
				{
					$found = true;
					break;
				}				
			}
			if( $found == false )
			{
				$p3dfiles[$#p3dfiles+1] = $2;
			}
		}
	}
	close INPUT;	
}

if( $srr2 )
{
	system "mkdir resource\\_$projectname";

	$exclude_common = 0; # ($projectname eq "ingame" || $projectname eq "pause");

	if( $exclude_common )
	{
		print ">> Excluding common files ... ...\n";
	}
}

if( $printimages )
{
	if( $printnice )
	{
		print "Images : \n";
		for( $i = 0; $i < $#images+1; $i++ )
		{
			print "  $images[$i]\n";
		}
	}
	else
	{
#		print "@images ";
	}

	if( $srr2 )
	{
		for( $i = 0; $i < $#images+1; $i++ )
		{
			# run p3dimage on each image file
			$image = $images[$i];

			# parse image filename (strip out file path and extension)
			$image =~ s/.*\\//;
			$image =~ s/.png//;

			if( $exclude_common )
			{
				if( -s "resource\\_common\\$image.p3d" > 0 )
				{
#					print "Excluded common file: $images[$i]\n";
					next;
				}
			}

			if( -s "resource\\_$projectname\\$image.p3d" == 0 )
			{
				print "Processing image file: $images[$i]\n";
				if( $srr2_params =~ /-G/ && $image =~ /loadbgd1/ )
				{
					# GC Hack! Don't reduce loadbgd1.png to 8-bit
					#
					print "GC ONLY: *** NOT reducing $image to 8-bit ***\n";

					system "..\\..\\..\\..\\..\\libs\\pure3d\\tools\\commandline\\bin\\p3dimage --ntsc_fix -S -G -o resource\\_$projectname\\$image.p3d resource\\$images[$i]";
				}
				else
				{
					system "..\\..\\..\\..\\..\\libs\\pure3d\\tools\\commandline\\bin\\p3dimage --ntsc_fix $srr2_params -o resource\\_$projectname\\$image.p3d resource\\$images[$i]";
				}
			}
			else
			{
#				print "Image already processed: $images[$i]\n";
			}
		}
	}
}
if( $printtextstyles )
{
	if( $printnice )
	{
		print "Text Styles : \n";
		for( $i = 0; $i < $#textstyles+1; $i++ )
		{
			print "  $textstyles[$i]\n";
		}
	}
	else
	{
#		print "@textstyles ";
	}

	if( $srr2 )
	{
		for( $i = 0; $i < $#textstyles+1; $i++ )
		{
			$textstyle = $textstyles[$i];
			$textstyle =~ s/.*\\//;
			$textstyle =~ s/.p3d//;

			if( $exclude_common )
			{
				if( -s "resource\\_common\\$textstyle.p3d" > 0 )
				{
#					printf "Excluded common file: $textstyles[$i]\n";
					next;
				}
			}

			if( -s "resource\\_$projectname\\$textstyle.p3d" == 0 )
			{
				print "Copying font file: $textstyles[$i]\n";
				system "copy resource\\$textstyles[$i] resource\\_$projectname\\$textstyle.p3d";
			}
		}
	}
}
if( $printtextbibles )
{
	if( $printnice )
	{
		print "Text Bibles : \n";
		for( $i = 0; $i < $#textbibles+1; $i++ )
		{
			print "  $textbibles[$i]\n";
		}
	}
	else
	{
#		print "@textbibles ";
	}

	if( $srr2 )
	{
		for( $i = 0; $i < $#textbibles+1; $i++ )
		{
			$textbible = $textbibles[$i];
			$textbible =~ s/.*\\//;
			$textbible =~ s/.p3d//;
			if( -s "resource\\_$projectname\\$textbible.p3d" == 0 )
			{
				print "Copying text bible file: $textbibles[$i]\n";
				system "copy resource\\$textbibles[$i] resource\\_$projectname\\$textbible.p3d";
			}
		}
	}
}
if( $printp3dfiles )
{
	if( $printnice )
	{
		print "P3D Files : \n";
		for( $i = 0; $i < $#p3dfiles+1; $i++ )
		{
			print "  $p3dfiles[$i]\n";
		}
	}
	else
	{
#		print "@p3dfiles ";
	}

	if( $srr2 )
	{
		for( $i = 0; $i < $#p3dfiles+1; $i++ )
		{
			$p3dfile = $p3dfiles[$i];
			$p3dfile =~ s/.*\\//;
			$p3dfile =~ s/.p3d//;
			if( -s "resource\\_$projectname\\$p3dfile.p3d" == 0 )
			{
				print "Copying pure3d file: $p3dfiles[$i]\n";
				system "copy resource\\$p3dfiles[$i] resource\\_$projectname\\$p3dfile.p3d";
			}
		}
	}
}
