#
# Set up the registry to do an automatic run of a command/set of commands
# at either the next boot, or the next user login.  It can also be set
# to run once, or run every time.
#
# Note that this program simply manipulates the registry, actual implementation
# of the running of the commands is the responsibility of the operating system.
# Note that this is fully implemented in Windows 95; not all versions of
# Windows NT yet support the running of these entries.  In future, it is
# assumed that this will become standard, and all Win32 based systems will
# support these entries.
#

#
# Good programming practice to set shell variables for each control id.
# The resource editors usually generate a C header file with #defines
# for each control ID.  They must be manually converted at this point.
#

# Generic ids, found normally in windows header files.
IDOK=1
IDCANCEL=2
IDHELP=9

# For the main dialog
IDD_AUTORUN=101		# The dialog number
IDC_MACHINE_ONCE=1000
IDC_MACHINE_EVERY=1001
IDC_USER_ONCE=1002
IDC_USER_EVERY=1003
IDC_LIST=1004
IDC_ADD=1005
IDC_DELETE=1006
IDC_MODIFY=1007

# For the add dialog
IDD_ADD=102		# The dialog number
IDC_AID=1008
IDC_CMDLINE=1009

AUTORUN_CID=1005	# Help Context id to the autorun page.

reg='\Software\Microsoft\Windows\CurrentVersion\'

#
# Find out where this shell script is executing from, since the shell script
# includes the dialog resources that we display.
#
RESFILE=$(whence "$0")

if ! dlg load -x -1 -y -1 -r h "$RESFILE" $IDD_AUTORUN	# Create the dialog box
then
	exit		# Should have been an error from dlg in a msgbox
fi

# Setup the listview columns
dlg columns -c $IDC_LIST "Application Identifier" 130 "Command Line" 240

#
# Save the current list back into the registry, by deleting all current
# registry entries, and then replacing them with the contents of our listbox.
#
function SaveList {
	# Key may not exist at all, discard error
	registry -d -k "$registry" 2>nul
	i=0
	while dlg gettext -d $h -c $IDC_LIST -i $i aid cmdline
	do
		registry -s -k "$registry" -n "$aid" -v "$cmdline"
		let i=i+1
	done
}

#
# Given the current settings of the radio buttons, load up the listbox
# with the current registry values.
#
function LoadList {
	if [ "$modified" -ne 0 ]
	then
		msgbox -b yesno -f -i question -q "AutoRun" "Current list has changed, do you want to save it now?"
		if [ $? -eq 6 ]
		then
			SaveList
		fi
	fi
	if { dlg isbuttonchecked -d $h -c $IDC_USER_ONCE val; test val -eq 1; }
	then
		registry=HKEY_CURRENT_USER${reg}RunOnce
	elif { dlg isbuttonchecked -d $h -c $IDC_USER_EVERY val; test val -eq 1; }
	then
		registry=HKEY_CURRENT_USER${reg}Run
	elif { dlg isbuttonchecked -d $h -c $IDC_MACHINE_ONCE val; test val -eq 1; }
	then
		registry=HKEY_LOCAL_MACHINE${reg}RunOnce
	else
		registry=HKEY_LOCAL_MACHINE${reg}Run
	fi

	dlg clear -d $h -c $IDC_LIST

	#
	# Note: the following registry command may give an error
	# that the key doesn't exist, since there may not be (probably aren't)
	# any Run entries
	#
	# Note: This should be:     registry | while read
	# but the `dlg set' when operating on a ListView control must not
	# be in a forked sub-shell.  (Why it works if using a normal listbox
	# control is probably due to the fact that Listbox controls are part
	# of windows 3.1 and have to be shared for compatability purposes.)
	#
	v=$(registry -p -k "$registry" 2>nul)
	[ ! -z "$v" ] && while IFS="	" read -r key aid cmdline
	do
		# Note that cmdline has a " at the beginning and end
		cmdline="${cmdline#?}"
		dlg addtext -d $h -c $IDC_LIST "$aid" "${cmdline%?}"
	done <<EOF
$v
EOF
	modified=0
}

#
# Add/Edit single entry dialog box.
# Either called with no args, in which case its an add,
# or called with $1=index of line to edit
#
function RunAddDlg {
	if ! dlg load -m $h -x -1 -y -1 -r add "$RESFILE" $IDD_ADD
	then
		exit	# Fatal error!!!
	fi
	if [ $# -eq 0 ]
	then
		dlg settext -d $add "Add New Run Command"
	else
		dlg settext -d $add "Edit Run Command"
		dlg gettext -d $h -c $IDC_LIST -i $index aid cmdline
		dlg settext -d $add -c $IDC_CMDLINE "$cmdline"
		dlg settext -d $add -c $IDC_AID "$aid"
	fi

	while dlg event -d $add msg control
	do
		case "$msg $control" in
		"command $IDOK")
			dlg gettext -d $add -c $IDC_CMDLINE cmdline
			dlg gettext -d $add -c $IDC_AID aid
			if [ -z "$cmdline" -o -z "$aid" ]
			then
				continue
			fi
			if [ $# -ne 0 ]	# This was an edit, replace it
			then
				dlg replacerow -d $h -c $IDC_LIST -i $1 "$aid" "$cmdline"
			else
				dlg addtext -d $h -c $IDC_LIST "$aid" "$cmdline"
			fi
			modified=1
			break	# out of while loop
			;;
		"command $IDCANCEL")
			break
			;;
		esac
	done
	dlg close -d $add
}

#
# Set up the defaults: load the per-user, run once registry entries.
#
dlg checkbutton -d $h -c $IDC_USER_ONCE 1	# Default is the user, once
modified=0
LoadList

#
# Standard dlg event loop: Get the user's actions
# We will exit this loop automatically if the user selects the Close system
# menu; the message will be `close', but the exit status will be non-zero.
#
while dlg event -d $h msg control
do
	case "$msg $control" in
	"command $IDHELP"|"help "*)
		dlg winhelp -C $AUTORUN_CID "$ROOTDIR/etc/toolkit.hlp"
		;;
	
	"command $IDOK")
		break
		;;
	"command $IDCANCEL")
		dlg winhelp -q "$ROOTDIR/etc/toolkit.hlp"
		dlg close -d $h
		exit
		;;
	"command $IDC_USER_ONCE"|
	"command $IDC_USER_EVERY"|
	"command $IDC_MACHINE_ONCE"|
	"command $IDC_MACHINE_EVERY")
		# Radio button selecting which type of startup list is to
		# be shown.  Reload.
		LoadList
		;;
	"command $IDC_ADD")
		RunAddDlg
		;;
	"command $IDC_DELETE")
		dlg getcursel -d $h -c $IDC_LIST index
		if [ $index -eq -1 ]
		then
			continue	# Ignore, nothing selected
		fi
		dlg gettext -d $h -c $IDC_LIST -i $index cur
		msgbox -b okcancel -f -i question -q "AutoRun" "Ok to delete entry "$cur
		if [ $? -eq 2 ]
		then
			continue	# Ignore, cancel selected
		fi
		dlg delete -d $h -c $IDC_LIST -i $index
		modified=1
		;;
	"command $IDC_MODIFY")
		dlg getcursel -d $h -c $IDC_LIST index
		if [ $index = -1 ]
		then
			continue	# Ignore, nothing selected
		fi
		RunAddDlg $index
		;;
	"double $IDC_LIST")
		dlg getcursel -d $h -c $IDC_LIST index
		if [ $index = -1 ]
		then
			continue	# Better be, we got a dblclk
		fi
		RunAddDlg $index
		;;
	esac
done

# Now save it all...
SaveList

# Close out the winhelp: this is ignored if we didn't start up a winhelp;
# or if the winhelp was started by somebody other than this process.
dlg winhelp -q "$ROOTDIR/etc/toolkit.hlp"

dlg close -d $h
#[]mks internal resource : dialog : \101
#[ ]begin : size 700
#[  ]MP #(D      +      #D *0      $$ =0!T &\ ;0!A '0 :0!C "  0P!O
#[  ]M &T ;0!A &X 9  @ %( =0!N    " !- %, ( !3 &$ ;@!S "  4P!E '( 
#[  ]M:0!F     0 !4     "K  < ,@ .  $ __^  $\ 2P            %0    
#[  ]M *L &  R  X  @#__X  0P!A &X 8P!E &P         "0  4      '  < 
#[  ]M?P , .@#__^  $\ ;@ @ &T 80 F &, : !I &X 90 @ &( ;P!O '0 =0!P
#[  ]M "P ( !O &X 8P!E          D  %      !P 3 '\ # #I ___@ !/ &X 
#[  ]M( !M &$ 8P!H "8 :0!N &4 ( !B &\ ;P!T '4 <  L "  90!V &4 <@!Y
#[  ]M "  = !I &T 90         )  !0      < 'P!_  P Z@/__X  3P!N "  
#[  ]M)@!U ', 90!R "  ; !O &< :0!N "P ( !O &X 8P!E          D  %  
#[  ]M    !P K '\ # #K ___@ !/ &X ( !U "8 <P!E '( ( !L &\ 9P!I &X 
#[  ]M+  @ &4 =@!E '( >0 @ '0 :0!M &4            !4      9 (X )0 +
#[  ]M .T#__^  "8 00!D &0            !4     !? (X )0 + .X#__^  "8 
#[  ]M1 !E &P 90!T &4          5      I0". "4 "P#O ___@  F $T ;P!D
#[  ]M &D 9@!Y       % (%0      < .0#6 $X [ -3 'D <P!, &D <P!T %8 
#[  ]M:0!E '< ,P R    3 !I ', = !V &D 90!W #$          5      JP I
#[  ]9 #( #@ ) /__@  F $@ 90!L '          
#[ ]end
#[]mks internal resource : dialog : \102
#[ ]begin : size 342
#[  ]MP #(D      &      #T &P      $$ 9 !D "  3@!E '< ( !2 '4 ;@ @
#[  ]M $, ;P!M &T 80!N &0    ( $T 4P @ %, 80!N ', ( !3 &4 <@!I &8 
#[  ]M   !  %0     +L !P R  X  0#__X  3P!+             5      NP 8
#[  ]M #( #@ " /__@ !# &$ ;@!C &4 ;             )0      < $P"*  D 
#[  ]M_____X( 00!P '  ; !I &, 80!T &D ;P!N "  20!D &4 ;@!T &D 9@!I
#[  ]M &4 <@        "  (%0      < (0"?  T \ /__X$            "4   
#[  ]M   ' #L ;@ * /____^" $, ;P!M &T 80!N &0 ( !, &D ;@!E        
#[  ]; (  @5      !P!* .8 #@#Q ___@0      
#[ ]end
#[]mks internal resource : icon : \0 : width 32 height 32 colors 16
#[ ]begin : size 744
#[  ]M*    "    !      0 $      "  @                              
#[  ]M (   (    " @ "     @ "  ("   # P,  @("     _P  _P   /__ /\ 
#[  ]M  #_ /\ __\  /___P#,S,S,S  +N_"9N[FY$#L S,S,S,P "[OP&9N9L9 [
#[  ]M ,S,S,S,  ___P&;F;D0,S/,S,S,S  /            S,S,S,P #_#NX)D1
#[  ]M$0S. \S,S,S,  O_#N"?$1$,X##,S,S,S  +O_#@GQ$1#@, S,S,S,P "[O_
#[  ]M )\1$0 [ ,S,S,S,  N[O_"?$1$#NP            #PGQ$1            
#[  ]M    \)\1$0               /"?$1$   "9F9F9F0 *JJKPGQ$1 JH F9F9
#[  ]MF9D "JJJ\)\1$0*J )F9F9F9  JJJO"?$1$"J@"9F9F9F0 *JJKPGQ$1 JH 
#[  ]MF9F9F9D "JJJ\)\1$0*J )F9F9F9  JJJO"?$1$"J@"9F9F9F0 *JJKPGQ$1
#[  ]M JH F9F9F9D "JJJ\)\1$0*J )F9F9F9  JJJO"?$1$"J@"9F9F9F0 *JJKP
#[  ]MGQ$1 JH F9F9F9D "JJJ\)D1$0*J )F9F9F9  JJJO     *J@"9F9F9F0 *
#[  ]MJJK_"9$0*JH F9F9F9D "JJJKPGQ$"JJ              \)\1          
#[  ]M       /"9$0                #P                      \0@     
#[  ]M             /$(                                            
#[  ]M                                                            
#[  ]M                                                            
#[  ]8                ___X/___^#____S_
#[ ]end
#[]mks internal resource : icon : \1 : width 16 height 16 colors 16
#[ ]begin : size 296
#[  ]M*    !     @     0 $      #                                 
#[  ]M (   (    " @ "     @ "  ("   # P,  @("     _P  _P   /__ /\ 
#[  ]M  #_ /\ __\  /___P#,S,#_D;.1B,S,P,     ,S,S _$GQ!,C,S, _R?$,
#[  ]MB$1$0#/Y\0@P     /GQ" "9F9 J^?$(H)F9D"KY\0B@F9F0*OGQ"*"9F9 J
#[  ]M^?$(H)F9D"KY\0B@F9F0*OGQ"J 1$1 B_Y&"(      /D8        "1    
#[  ]M            6 4        )D0                                  
#[  ]:      #Q                _\\ \?__  "9
#[ ]end
