$mapfilename = "..\\..\\cd\\ps2\\srr2.map";

print "Arg count is $#ARGV\n";

if( $#ARGV < 0 )
{
	print "Syntax: perl mapfile.pl <address> [mapfile]";
	exit;
}

$address = hex("@ARGV[0]");

if( $#ARGV > 0 )
{
	$mapfilename = $ARGV[1];
}

print "Address: $address\n";
print "Mapfile: $mapfilename\n";

$mapname = $mapfilename;

print "Mapname: $mapname\n";

$found = 0;

open MAPFILE, $mapname or die "Couldn't find mapfile $mapname";
while (<MAPFILE>)
{
	#
	# Tokenize
	#
	@Tokens = split;

	if( $#Tokens >= 2 )
	{
		$baseaddress = "0x" . $Tokens[0];
		$baseaddress =~ tr/a-f/A-F/;
		$baseaddress = hex("$baseaddress");

		$size = "0x" . $Tokens[1];
		$size =~ tr/a-f/A-F/;
		$size = hex("$size");

		$sum = $baseaddress + $size;

		if( ( $baseaddress > 0 ) && ( $size > 0 ) )
		{
			if( ( $address >= $baseaddress ) && ( $address <= $sum ) )
			{
				print $_;
				$found = 1;
			}
		}
	}
}

#
# Line not found
#
close MAPFILE;
if( $found == 0 )
{
	print "Couldn't find specified address in the mapfile.  Uh-oh.";
}
