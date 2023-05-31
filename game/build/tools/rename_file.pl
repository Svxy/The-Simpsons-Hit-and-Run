$file = $ARGV[0];
$ChunkId = $ARGV[1];
$FieldName = $ARGV[2];

$/ = ".p3d";
chomp( $file );

`p3dmutate -i $ChunkId -f $FieldName -a $file $file.p3d -o $file.p3d 1>&2` 