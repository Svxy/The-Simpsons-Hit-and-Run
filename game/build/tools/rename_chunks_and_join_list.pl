$ChunkName = shift( @ARGV );
$ChunkId = shift( @ARGV );
$FieldName = shift( @ARGV );
$Outfile = shift( @ARGV );
$InListFilename = "file_list.txt";

open(DAT, $InListFilename) || die("Could not open file!");
@raw_data = <DAT>;
close( DAT );

$count = 0;
$Outfilelist = "";

foreach $line (@raw_data )
{
	$/ = ", ";
	chomp( $line );	
	print "$line\n";
	@file_list = split( ",", $line );
	foreach $infile (@file_list )
	{
		$/ = " ";
		chomp( $infile );
		print "copy /B /Y $infile $count.p3d 1>&2\n";
		`copy /B /Y $infile $count.p3d 1>&2`;
		$/ = ".p3d";
		chomp( $infile );
		`p3dmutate -m $ChunkName -i $ChunkId -f $FieldName -a $infile $count.p3d 1>&2`;
		push( @OutList, $count );
		$count = $count + 1;
	}
}
push( @OutList, " " );
$Outfilelist = join(".p3d ",@OutList );
`p3djoin -o $Outfile $Outfilelist 1>&2`;