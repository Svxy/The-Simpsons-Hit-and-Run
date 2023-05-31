# Copyright 1997 by Mortice Kern Systems Inc.  All rights reserved.

# Make sure we use MKS Perl.
bindir=$(getconf _CS_BINDIR)
if [ $? -ne 0 ] ; then
	exit 1
fi
perl=$bindir/perl.exe
unset bindir

prog=$(cat <<'END_OF_MAPIMAIL'
#! perl
#
# mapimail.pl is a perl script that demonstrates how to use the COM.pm
# package by showing how to use the Microsoft MAPI Session and MAPI Messages
# COM objects.
#
# Options:
#  "-q" just quietly fail when necessary parameters are missing
#       (default: allow GUI dialog boxes when necessary parameters are missing)
#  "-R" adds the Return-Receipt-Requested: header to the
#       out-going email message
#  "-s subject" sets the subject line
#  "-t text" sets the body of the message to "text"
#  "-f file" sets the body of the message to the contents of "file"
#       (if neither -t nor -f are specified, then the body of the message
#        is taken from STDIN)
#  "-c Cc-recipient" adds "recipient" to the list of people getting
#       carbon copies of this email
#  "-c Bcc-recipient" adds "recipient" to the list of people getting
#       blind carbon copies of this email
#  "-a attachment" attaches the file "attachment"
#  "-u username" sets the username of the mapimail session
#  "-p password" sets the password of the mapimail session
#  "recipient" adds "recipient" to the list of people getting this email

require COM;

sub usage
{
    print STDERR "Usage: [-q] [-R] [-s subject] [[-f file]|[-t text]]\n";
    print STDERR "       [-c Cc-recipient] [-b Bcc-recipient]\n";
    print STDERR "       [-a attachment] [-u username] [-p password]\n";
    print STDERR "       recipient [recipient]\n";
    exit(1);
}

$options = "qRs:f:t:c:b:a:u:p:";
@options = split(/ */, $options);
$count_r = 0;
$count_c = 0;
$count_b = 0;
$count_a = 0;
while (@ARGV) {
    $_ = $ARGV[0];
    if ($_ =~ /^-(.)(.*)/) {
        ($first,$rest) = ($1,$2);
        $pos = index($options, $first);
        # $[ = position of first character in a substring
        #    = position of first element in an array
        if ($pos >= $[) {
	    if (@options[$pos+1] eq ':') {
	        shift(@ARGV);
	        if ($rest eq '') {
		    unless (@ARGV) {
    		        print STDERR "option: $first: requires argument\n";
		        usage();
		    }
		    $rest = shift(@ARGV);
	        }
	        if ($first eq 'c') {
	            @opt_c[$count_c] = $rest;
	            $count_c++;
	        } elsif ($first eq 'b') {
	            @opt_b[$count_b] = $rest;
	            $count_b++;
	        } elsif ($first eq 'a') {
		    # verify that the file to be attached exists
		    unless (-f $rest && -r $rest) {
    		        print STDERR "option: -a $rest: not valid attachment\n";
		        usage();
		    }
	            @opt_a[$count_a] = $rest;
	            $count_a++;
	        } else {
	            eval "\$opt_$first = \$rest;";
	        }
	    } else {
	        eval "\$opt_$first = 1";
	        if ($rest eq '') {
		    shift(@ARGV);
	        } else {
		    $ARGV[0] = "-$rest";
	        }
	    }
        } else {
	    print STDERR "option: $first: unknown\n";
	    usage();
        }
    } else {
        @opt_r[$count_r] = shift(@ARGV);
        $count_r++;
    }
}

if ($count_r == 0) {
	print STDERR "recipient required\n";
	usage();
}

$TRUE  = 1;
$FALSE = 0;

# Recipient types:
$mapToList  = 1; # Normal recipient.
$mapCcList  = 2; # Carbon-copy recipient.
$mapBccList = 3; # Blind-carbon-copy recipient.

# Attachment types:
$mapData    = 0; # Data file.
$mapEOLE    = 1; # Embedded OLE object.
$mapSOLE    = 2; # Static OLE object.

if ($opt_q) {
    $USE_GUI_FLAG = $FALSE;
} else {
    $USE_GUI_FLAG = $TRUE;
}

$session = new IDispatch "MSMAPI.MAPISession";
if ($session == 0) {
	die "Could not open the MAPI Session Control, stopped";
}

if (defined $opt_u) {
	$session->{UserName} = $opt_u;
}
if (defined $opt_p) {
	$session->{Password} = $opt_p;
}

$session->{DownLoadMail} = $FALSE;
$session->{LogonUI}      = $USE_GUI_FLAG;
$session->{NewSession}   = $FALSE;

$session->SignOn();
if ($session->{SessionID} == 0) {
	die "Could not SignOn using the MAPI Session Control, stopped";
}

$message = new IDispatch "MSMAPI.MAPIMessages";
if ($message == 0) {
	$session->SignOff();
	die "Could not open the MAPI Messages Control, stopped";
}

$message->{SessionID} = $session->{SessionID};

$message->Compose();

if ($opt_R) {
    $message->{MsgReceiptRequested} = $TRUE;
} else {
    $message->{MsgReceiptRequested} = $FALSE;
}
if ($opt_s) {
    $message->{MsgSubject} = $opt_s;
} else {
    $message->{MsgSubject} = "<no subject>";
}

# Note that we allow both -t and -f if both were specified on the command line.
if (defined $opt_t) {
    $opt_t = $opt_t . "\n"; # ensure message body ends in a newline
}
if ($opt_f) {
    if (defined $opt_t) {
        $opt_t = $opt_t . "\n"; # insert a blank line
    }
    unless (open FILE, $opt_f) {
	$session->SignOff();
    	die "Could not open $opt_f, stopped";
    }
    while ($_ = <FILE>) {
	$opt_t = $opt_t . $_;
    }
    $message->{MsgNoteText} = $opt_t;
}
# Only use STDIN if neither -t nor -f specified on the command line.
if (! defined $opt_t && ! defined $opt_f) {
    $opt_t = "";
    while ($_ = <STDIN>) {
	$opt_t = $opt_t . $_;
    }
}
if ($count_a > 0) {
    $opt_t = $opt_t . "\n\n"; # seperate the attachments from the message body
    # Add placeholders at the end of the message body for the attachments.
    for ($i = 0 ; $i < $count_a; $i++) {
        $opt_t = $opt_t . " ";
    }
}
$message->{MsgNoteText} = $opt_t;

$message->{RecipIndex} = 0;
for ($i = 0 ; $i < $count_r; $i++) {
    if ($i != 0) {
        $message->{RecipIndex}++;
    }
    $message->{RecipType}        = $mapToList;
    $message->{RecipAddress}     = @opt_r[$i];
    $message->{RecipDisplayName} = @opt_r[$i];
}
for ($i = 0 ; $i < $count_c; $i++) {
    $message->{RecipIndex}++;
    $message->{RecipType}        = $mapCcList;
    $message->{RecipAddress}     = @opt_c[$i];
    $message->{RecipDisplayName} = @opt_c[$i];
}
for ($i = 0 ; $i < $count_b; $i++) {
    $message->{RecipIndex}++;
    $message->{RecipType}        = $mapBccList;
    $message->{RecipAddress}     = @opt_b[$i];
    $message->{RecipDisplayName} = @opt_b[$i];
}

if ($count_a > 0) {
    # We put the attachments at the end of the message, in the order
    # that they were specified on the command line.
    $message->{AttachmentIndex} = 0;
    for ($i = $count_a-1 ; $i >= 0; $i--) {
        if ($i != $count_a-1) {
            $message->{AttachmentIndex}++;
        }
        $message->{AttachmentType}     = $mapData;
        $message->{AttachmentName}     = @opt_a[$i];
        $message->{AttachmentPathName} = @opt_a[$i];
        $message->{AttachmentPosition} = length($opt_t)-$count_a+$i;
    }
}

$message->Save();
$message->{AddressResolveUI} = $USE_GUI_FLAG;
$message->ResolveName();
$message->Send($USE_GUI_FLAG);

$session->SignOff();
END_OF_MAPIMAIL
)

$perl -e "$prog" -- "$@"

unset prog perl
