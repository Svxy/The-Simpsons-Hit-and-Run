sub filechecker
{
	local $artfileslistfilename = $_[0];
	local $filepath = $_[1];
       
	open(ARTFILESLIST,$artfileslistfilename);
	local @artfiles= <ARTFILESLIST>;
	
	$sendmail = 0;
	foreach $file (@artfiles) {
		chomp $file;
		$filename = $filepath.$file;
		if( not -e $filename) {
			print "not found: $filename\n";
			$sendmail = 1;
			push(@badfiles, $file);
		}
		else {
			print "file found: $filename\n";
		}
	}

	close(ARTFILESLIST);
	return $sendmail;
}

sub mailresults
{
	print("postie $postie\n");
	system("$postie -host:smtp.radical.ca -from:nightlypipe -to:$maillist -s:\"ArtPipe $platform: Results for $level\" -msg:\"The following files were not created when piping $level: @badfiles\"");
}

sub SendSummaryEmail
{
	$msg = $_[0];
	print("postie $postie\n");
	system("$postie -host:smtp.radical.ca -from:nightlypipe -to:$maillist -s:\"ArtPipe Summary for nightly artpipe\" -msg:\"$msg\"");
}

#Do not edit below this line
1;