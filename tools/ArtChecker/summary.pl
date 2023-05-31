#!/usr/bin/perl

$filename=@ARGV[0];

############################################################################## 
#	MAIN
##############################################################################

#open file
open(FILE_HANDLE,$filename) || die "Could not open file ERROR: $filename!";


#while not EOF and read in string
while (<FILE_HANDLE>)
{
	$string = $_;
	chop($string);
	#print $string;	
	@todo =("artchecker ",$string," sum");
	system (@todo) # == 0 or  die "Failed to Execute Artchecker Command !\n";
}

close(FILE_HANDLE);