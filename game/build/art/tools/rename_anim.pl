opendir ( DIR, "." ) or die "Couldn't open directory, $!";

$type = $ARGV[0];


while ( $file = readdir DIR )
{
	if ( $file =~ m/(.*).p3d/ )
	{
		$/ = ".p3d";
		chomp( $file );

		`p3dmutate -m "PTRN_Motion_Root" -f "Name" -a $file $file.p3d 1>&2` 
	}
}

close DIR