$ChunkName = shift( @ARGV );
$ChunkId = shift( @ARGV );
$Outfile = shift( @ARGV );

$count = 0;
$Outfilelist = "";

foreach $infile (@ARGV )
{
	`copy /B /Y $infile $count.p3d 1>&2`;
	$/ = ".p3d";
	chomp( $infile );
	`p3dmutate -m $ChunkName -i $ChunkId -f "Name" -a $infile $count.p3d 1>&2`;
	 push( @OutList, $count );
	 $count = $count + 1;
}
push( @OutList, " " );
$Outfilelist = join(".p3d ",@OutList );
`p3djoin -o $Outfile $Outfilelist 1>&2`;