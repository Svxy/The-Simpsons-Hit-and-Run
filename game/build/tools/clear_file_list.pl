$Outfile = "file_list.txt";
open( DAT,">$Outfile") || die("Cannot Open File");

print DAT "";
close(DAT);
