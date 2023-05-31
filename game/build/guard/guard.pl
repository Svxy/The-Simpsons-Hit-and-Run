#!/usr/bin/perl
#Builds changes submitted into perforce as they arrive
#informs individuals of changlists that cause pipeline
#or compile errors.
#
#Matthew Miller (Simpsons Driving) (FishBulb)


$BuildError = "FALSE";
$p4infoOutput = `p4 changes -m1 -s submitted //...#have`;
($change, $lastBuiltChange, $on, $date, $remainder ) = split(/ /, $p4infoOutput);
$Waiting = 10;
$errorUnknown = 0;
$updateDistribution = 1;
$doNotification = 1;
$donightbuild = 1;
$pause = 1;


while ($BuildError = "FALSE") {

$PS2_error = 0;
$XBerrorCount = 0;

open(ARGS, "args\\args.txt");
@args=<ARGS>;
foreach $parameter (@args) {
	if($parameter =~ /updateDistribution/){
		$updateDistribution = 0;
	}
	if($parameter =~ /notifyUD/){
		$doNotification = 0;
		($notifyUD, $notifyName) = split(/ /,$parameter);
	}
	if($parameter =~ /donightbuild/){
		$donightbuild = 0;
	}
	if($parameter =~ /pause/){
		$pause = 0;
	}
}
#clear out all arguments
close(ARGS);
$#args = -1;
system("echo none > args\\args.txt");

#pause if requested
if($pause == 0){
	print "\nGuard in paused state, ";
	system("pause");
	$pause = 1;
}

#Do a night build if one was requested
if($donightbuild == 0){
	print "\n----------------------------------------\n";
	print "Doing Night Build\n";
	print "----------------------------------------\n";
	system("call ..\\nightly\\DoNightBuild.bat");
	$donightbuild = 1;
}

	$p4infoOutput = `p4 changes -m1 -s submitted`;
	($change, $DepotChangeNumber, $on, $date, $remainder ) = split(/ /, $p4infoOutput);

	if($DepotChangeNumber != $lastBuiltChange) {
		print "\n----------------------------------------\n";
		print "Attempting another build\n";
		print "----------------------------------------\n";
		print("Sync to head revision: ".$DepotChangeNumber."\n");
		system("call sync.bat");
		system("call buildRoutine.bat");
		system("echo ".$DepotChangeNumber." > ..\\..\\..\\game\\cd\\ver.txt");
		$lastBuiltChange = $DepotChangeNumber;
		$Waiting = 10;

		print "----------------------------------------\n";
		print "Checking for errors\n";
		print "----------------------------------------\n";
		open(E_SUMMARY,"logs\\summary.txt");
		@error_summary=<E_SUMMARY>;
		foreach $line (@error_summary) {
			#Check for PS2 errors
			if($line =~ /\*\*\*|\s\sStop/){
				$PS2_error = 1;
			}
			#Check for Xbox errors
			if($line =~ /error\(s\)/){
				($xbe, $dash, $XBerrorTemp, $errors, $warningCount, $warnings ) = split(/ /, $line);
				if($XBerrorTemp > $XBerrorCount){
					$XBerrorCount = $XBerrorTemp;
				}
					
			}
		}#loop end		
		close (E_SUMMARY);

#React to found errors
		if ($PS2_error != 0) {
			print ("PS2 : errors detected\n");
		}else{
			print ("PS2 : Build OK\n");
		}
		print "Xbox: ".$XBerrorCount." Errors and ". $warningCount. " warnings detected\n";

		if(($XBerrorCount != 0) || ($PS2_error != 0)){
		#we had problems
			if($errorUnknown == 0){
				open(EMAIL_LIST,"emaillist.txt");
				$mailingList=<EMAIL_LIST>;
				system("..\\tools\\mail\\postie.exe -from:DepotGuard -to:".$mailingList." -s:Error_Detected\(".$DepotChangeNumber."\) -a:logs\\p4sync.txt -a:logs\\summary.txt -a:logs\\xboxr.log -a:logs\\xboxd.log -a:logs\\xboxt.log -a:logs\\ps2r.log -a:logs\\ps2d.log -a:logs\\ps2t.log -a:logs\\gcr.log -a:logs\\gcd.log -a:logs\\gct.log -nomsg -host:radex");	
				$errorUnknown = 1;
			}
		}else{
		#everything is good
			$goodBuild = $DepotChangeNumber;
			print("Updating CD Approved/(".$goodBuild."\)\n");
			#system("..\\tools\\mksnt\\robocopy.exe ..\\..\\..\\game\\cd\\ D:\\BUILD\\CD_Tested\\ /S /PURGE /R:10 /W:10");
			print("Last build created successfuly ".$goodBuild."\n");		
			if($errorUnknown != 0){
				open(EMAIL_LIST,"emaillist.txt");
				$mailingList=<EMAIL_LIST>;
				system("..\\tools\\mail\\postie.exe -from:DepotGuard -to:".$mailingList." -s:Error_Fixed\(".$DepotChangeNumber."\) -nomsg -host:radex");	
				$errorUnknown = 0;
			}
		}
	}
	if ($Waiting) {
		if($updateDistribution == 0){
			print("Upadting Distribution Directory with last safe build /(".$goodBuild."\)\n");

#this should be changed to the T drive... or wherever the public copy of the game is going to be kept

			#system("..\\tools\\mksnt\\robocopy.exe D:\\BUILD\\CD_Tested\\ D:\\Build\\CD\\ /S /PURGE /R:10 /W:10");
			$updateDistribution = 1;
			if($doNotification == 0){
				system("net send ".$notifyName." Distribution Build has been updated to ".$goodBuild." at : d:\\Build\\CD\\");
				$doNotification = 1;
			}
		}

		print "Waiting for changes ($Waiting seconds so far without changes)...\r";
		$Waiting += 10;
	}
	sleep 10;
}
