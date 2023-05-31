sub P4Checkout
{
	local $artfileslistfilename = $_[0];
	local $platform = $_[1];
       
	open(ARTFILESLIST,$artfileslistfilename);
	local @artfiles= <ARTFILESLIST>;
	close(ARTFILESLIST);
	foreach $file (@artfiles) {
		chomp $file;
		$p4checkout = "p4 -p RADFS4:1675 -c artpipe -u mmiller edit //depot/game/cd/$platform/art/$file";
		system($p4checkout);
	}
}

sub P4Checkin
{
	my @p4filelist;
	local $artfileslistfilename = $_[0];
	local $platform = $_[1];
       
	open(P4COMMAND, "|p4 -p RADFS4:1675 -c artpipe -u mmiller submit -i");

	print P4COMMAND<<EOM;

Change:	new

Client:	artpipe

User:	mmiller

Status:	new

Description:
	$platform artpipe for level $level

Files: 
EOM
	open(ARTFILESLIST,$artfileslistfilename);
	local @artfiles= <ARTFILESLIST>;
	close(ARTFILESLIST);

	foreach $file (@artfiles) {
		chomp $file;
		print P4COMMAND "	//depot/game/cd/$platform/art/$file	#edit\n";
	}

	close(P4COMMAND);
}

sub P4RevertAll
{
	print("REVERTING ALL\n");
	my @p4filelist;
	local $artfileslistfilename = $_[0];
	local $platform = $_[1];
       
	#open(P4COMMAND, "|p4 -p RADFS4:1675 -c artpipe -u mmiller revert ");
	$p4command = "p4 -p RADFS4:1675 -c artpipe -u mmiller revert ";

	open(ARTFILESLIST,$artfileslistfilename);
	local @artfiles= <ARTFILESLIST>;
	close(ARTFILESLIST);
	foreach $file (@artfiles) {
		chomp $file;
		#print P4COMMAND "	//depot/game/cd/$platform/art/$file	#edit\n";
		$p4command = $p4command . "	//depot/game/cd/$platform/art/$file";
	}
	system("$p4command");
}

sub P4RevertUnchanged
{
	print("REVERTING UNCHANGED\n");
	system("p4 -p RADFS4:1675 -c artpipe -u mmiller revert -a");
}

#Do not edit below this line
1;