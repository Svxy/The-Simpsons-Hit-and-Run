$Outfile = "file_list.txt";
open( DAT,">>$Outfile") || die("Cannot Open File");

push( @ARGV, " " );
$Outfilelist = join(",",@ARGV );
print DAT "$Outfilelist";
close(DAT);
