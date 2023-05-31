opendir ( DIR, "." ) or die "Couldn't open directory, $!";

$ChunkName = $ARGV[0];
$ChunkId = $ARGV[1];

while ( $file = readdir DIR )
{
	if ( $file =~ m/(.*).p3d/ )
	{
		$/ = ".p3d";
		chomp( $file );

		`p3dmutate -m $ChunkName -i $ChunkId -f "Name" -a $file $file.p3d 1>&2` 
	}
}

close DIR