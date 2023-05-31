# UltraGrep - A find/grep dlg example.
#
# This shell script accepts user input from a dialog box, then runs
# 'find <directory> -name <filemask> | xargs grep <grep options> <pattern>'
# The output from this command is piped into a results dialog box, which
# the user can then browse through.
#
# Useful for searching through a tree of source files for keywords, e.g.
# 'find c:/msvc20/samples/win32 -name "*.cpp" | xargs grep GetOpenFileName'

# Dialog resources are included at the end of this file
RES=$(whence "$0")

# Here are some constants for the controls used in the 2 dialogs
IDOK=1
IDCANCEL=2

IDC_PATTERN=10
IDC_FILES=11
IDC_DIRECTORY=12
IDC_BROWSE=13
IDC_NOT_PATTERN=15
IDC_ENTIRE_LINE=16
IDC_IGNORE_CASE=17
IDC_DIR_ALWAYS=14
IDC_DIR_NEVER=18
IDC_DIR_TO=19
IDC_DIR_TO_EDIT=20
IDC_LISTBOX=101
IDC_EDIT=102

# Here are some strings we use to store the directory setting in the Registry
DirKey='HKEY_CURRENT_USER\Software\Mortice Kern Systems\Toolkit\UltraGrep'
DirKeyName='Starting Directory'

# Make these variables into integers to make math faster
typeset -i count width height dlgwidth dlgheight
count=0

# This function is called from the second dialog loop, when a user double
# clicks on a line or presses the 'Edit File' button.
function edit_file
{
	# Get the current listbox selection
	dlg getcursel -c $IDC_LISTBOX index
	if [ index -ne -1 ]
	then
		# If there is a current selection, get the filename and
		# line number for that selection, and run 'vi' on the
		# file.  The -c option causes vi to jump to the specified
		# line number.
		dlg gettext -c $IDC_LISTBOX -i $index filename lineno
		start vi -c"$lineno" \"$filename\"
	fi
}

# This function resizes the controls in the results dialog, so when the
# user resized the dialog, the controls move to fill the space.
function resize_results
{
	dlg getpos -w dlgwidth -h dlgheight
	dlg getpos -c $IDC_LISTBOX -x xpos -y ypos -w width -h height
	width=$dlgwidth-30
	height=$dlgheight-85
	dlg setpos -c $IDC_LISTBOX -x $xpos -y $ypos -w $width -h $height
	width=$width-66
	height=$height+25
	dlg setpos -c $IDOK -x $width -y $height
	width=$width-90
	dlg setpos -c $IDC_EDIT -x $width -y $height
}

# This is where the program really begins.  Load the first dialog to query
# the user.
if ! dlg load -x -1 -y -1 "$RES" DIALOG_1
then
	exit
fi

# Add some common file types to the 'pattern' combo box
dlg addtext -c $IDC_FILES "*"
dlg addtext -c $IDC_FILES "*.awk"
dlg addtext -c $IDC_FILES "*.c*"
dlg addtext -c $IDC_FILES "*.c"
dlg addtext -c $IDC_FILES "*.h"
dlg addtext -c $IDC_FILES "*.h*"
dlg addtext -c $IDC_FILES "*.ksh"
dlg addtext -c $IDC_FILES "*.mk"
dlg addtext -c $IDC_FILES "*.txt"
dlg setcursel -c $IDC_FILES -i 0

# Set the default starting directory.  We might have saved an old value in
# the registry, so try to get it.
directory="$(registry -p -r -k "$DirKey" -n "$DirKeyName" 2>nul)"
if [ $? -ne 0 ]
then
	# Set the default directory to the current directory
	directory="$PWD"
fi
dlg settext -c $IDC_DIRECTORY "$directory"

dlg checkbutton -c $IDC_DIR_ALWAYS 1

# This is the event loop for the first dialog.  We keep going until the
# user hits 'Ok' or 'Cancel'.
while dlg event msg control; do
	case $control in
	$IDC_BROWSE)
		# The user hit the 'Browse' button, so bring 
		# up a directory browser
		directory="$(filebox -D)"
		if [ $? -eq 0 ]
		then
			dlg settext -c 12 "$directory"
		fi 
		# Get rid of any pending messages
		dlg event flush ;;
	$IDCANCEL)
		# The user hit 'Cancel'
		dlg close
		exit ;;
	$IDOK)
		# The user hit 'OK'.  We need to validate all of the input
		# before we can continue.

		# Get the search pattern
		dlg gettext -c $IDC_PATTERN pattern
		if [ -z "$pattern" ]
		then
			msgbox -i exclamation "UltraGrep" "You must specify a pattern to grep for."
			dlg setfocus -c $IDC_PATTERN
			continue
		fi

		# Get the file pattern
		dlg gettext -c $IDC_FILES files
		if [ -z "$files" ]
		then
			msgbox -i exclamation "UltraGrep" "You must specify a filename pattern to search for."
			dlg setfocus -c $IDC_FILES
			continue
		fi

		# Get the starting directory
		dlg gettext -c $IDC_DIRECTORY directory
		if [ -z "$directory" ]
		then
			msgbox -i exclamation "UltraGrep" "You must specify a starting directory."
			dlg setfocus -c $IDC_DIRECTORY
			continue
		elif [ ! -d "$directory" ]
		then
			msgbox -i exclamation "UltraGrep" "The directory you specified does not exist."
			dlg setfocus -c $IDC_DIRECTORY
			continue
		fi

		# Save the directory settting in the registry
		registry -s -k "$DirKey" -n "$DirKeyName" -v "$directory" 2>nul

		# Get the options for grep
		if { dlg isbuttonchecked -c $IDC_NOT_PATTERN result; test result -eq 1; }
		then
			grepopts=-v
		fi
		if { dlg isbuttonchecked -c $IDC_ENTIRE_LINE result; test result -eq 1; }
		then
			grepopts="$grepopts -x"
		fi
		if { dlg isbuttonchecked -c $IDC_IGNORE_CASE result; test result -eq 1; }
		then
			grepopts="$grepopts -i"
		fi

		# Get the options for find
		if { dlg isbuttonchecked -c $IDC_DIR_NEVER result; test result -eq 1; }
		then
			findopts="-level 0"
		elif { dlg isbuttonchecked -c $IDC_DIR_TO result; test result -eq 1; }
		then
			dlg gettext -c $IDC_DIR_TO_EDIT result
			if [ -z $result ]
			then
				msgbox -i exclamation "UltraGrep" "You must specifiy a valid level number."
				dlg setfocus -c $IDC_DIR_TO_EDIT
				continue
			elif [ $result -lt 0 ]
			then
				msgbox -i exclamation "UltraGrep" "You must specifiy a valid level number."
				dlg setfocus -c $IDC_DIR_TO_EDIT
				continue
			else
				findopts="-level $result"
			fi
		fi

		break ;;
	esac
done

# Close the query dialog
dlg close

# Load up the results dialog
if ! dlg load -x -1 -y -1 "$RES" DIALOG_2
then
	exit
fi

# Add the columns to the list box, and size the controls to
# fit the size of the dialog.
dlg columns -c $IDC_LISTBOX File 200 "Line #" 50 Result 200
resize_results

dlg settext "UltraGrep - Searching..."

# This is where the actual command gets run.  We want the user to be able 
# to cancel the find by pressing the 'Cancel' button on the results dialog,
# so run it as a co-process, using dlg -P -W to read the output.
# To get grep to output the filename every time, it needs 2 input files, so
# give it 'nul' as the second file.

exec 2>nul
find "$directory" $findopts -type f -name "$files" -exec grep -n $grepopts "$pattern" {} nul \; |&

# Set the shell's input field separator so we can parse the output from
# grep easily.
OIFS="$IFS"
IFS=:

# Start the event loop for the results dialog.  Remember we started the
# command as a co-process, so we use the -P and -W flags on 'dlg event'
# to read the results from the co-process.  This loop will continue until
# our co-process finishes, or the user hits 'Cancel'.

while dlg event -P -W msg control; do

	case $msg in
	coprocess)
		# We got some data from the co-process, which will be output
		# from our grep command.

		# The output from grep looks like this:
		# <filename>:<line number>:<matching line>
		#
		# If we specified a full directory name to search from 
		# (e.g. "c:/msvc20/samples/win32"), the colon in the drive
		# specifier is going to confuse the shell, because we've 
		# set IFS=:.  So if the filename has a drive specifier, 
		# we need to parse the output from grep slightly differently.

		if [ "${directory#?:}" = "$directory" ]
		then
			read -p -r filename lineno match || break
			dlg addtext -c $IDC_LISTBOX "$filename" "$lineno" "$match"
		else
			read -p -r drive filename lineno match || break
			# Reconstruct the filename
			filename="$drive":"$filename"
			dlg addtext -c $IDC_LISTBOX "$filename" "$lineno" "$match"
		fi
		count=$count+1 ;;

	child-exit)
		# Our co-process is done.
		break ;;

	break)
		break ;;

	size)
		resize_results ;;

	command)
		case $control in
		$IDOK)
			# The user pressed 'Cancel'
			break ;;

		esac ;;
	esac
done
IFS="$OIFS"

# Make sure our co-process process is dead.
kill %1 
wait

# Change the title and the name of the 'Cancel' button on the results
# dialog.
dlg settext "UltraGrep Results - $count file(s) found"
dlg settext -c $IDOK Close
dlg enabled -c $IDC_EDIT 1

# Now we start another event loop on the results dialog, which allows the
# user to browse through the results of the command.

while dlg event msg control; do
	
	case $msg in
	command)
		case $control in
		$IDOK)
			break ;;

		$IDC_EDIT)
			edit_file ;;

		esac ;;

	double)
		# The user double clicked on the list box, which
		# is the same as clicking 'Edit File'
		edit_file ;;

	size)
		# The user resized the frame so resize 
		# our child windows to fit
		resize_results ;;

	esac
done

dlg close

# That's all folks.

# Dialog resources follow this line
#[]mks internal resource : dialog : DIALOG_1
#[ ]begin : size 1012
#[  ]MP #(D      3 !< %0#Y (X      %4 ; !T '( 80!' '( 90!P    " !-
#[  ]M %, ( !3 &$ ;@!S "  4P!E '( :0!F       "4      +  H )@ ( /__
#[  ]M__^" "8 4P!E &$ <@!C &@ ( !& &\ <@ Z            @5      -0 (
#[  ]M 'P #  * /__@0            )0      L 'P!C  @ _____X( 20!N "  
#[  ]M1@!I &P 90!S "  30!A '0 8P!H &D ;@!G "  = !H &4 (  F %  80!T
#[  ]M '0 90!R &X .@       @(A4     !R !P 0  \  L __^%            
#[  ]M E      "P V #( " #_____@@!3 '0 80!R '0 :0!N &< (  F $8 <@!O
#[  ]M &T .@        "!4      ^ #4 <P ,  P __^!          $  5      
#[  ]MO@ & #( #@ ! /__@ !/ $L            !4     "^ !@ ,@ .  ( __^ 
#[  ]M $, 80!N &, 90!L             5      O@ T #( #@ - /__@  F $( 
#[  ]M<@!O '< <P!E "X +@ N          <  %      !P!/ (@ -P#_____@ !-
#[  ]M &$ = !C &@ :0!N &< ( !/ '  = !I &\ ;@!S          ,  5      
#[  ]M#0!; 'X #  / /__@  F $0 :0!S '  ; !A 'D ( !L &D ;@!E ', ( !.
#[  ]M $\ 5  @ &T 80!T &, : !I &X 9P @ '  80!T '0 90!R &X       , 
#[  ]M 5      #0!H '\ #  0 /__@  F $T 80!T &, :  @ &\ ;@ @ &4 ;@!T
#[  ]M &D <@!E "  ; !I &X 90!S "  ;P!N &P >0       P !4      - '4 
#[  ]M/  , !$ __^  "8 20!G &X ;P!R &4 ( !C &$ <P!E          <  5  
#[  ]M    DP!/ %X -P#_____@ !$ &4 <P!C &4 ;@!D "  1 !I '( 90!C '0 
#[  ]M;P!R &D 90!S       )  -0     )H 6@ \  P #@#__X  )@!! &P =P!A
#[  ]M 'D <P      "0  4     ": &@ /  , !( __^  "8 3@!E '8 90!R    
#[  ]M      D  %      F@!V !4 #  3 /__@  F %0 ;P        "!4     "S
#[  ]M '< $  + !0 __^!             E      QP!X "4 " #_____@@!L &4 
#[  ]6=@!E &P <P @ &\ ;@!L 'D         
#[ ]end
#[]mks internal resource : dialog : DIALOG_2
#[ ]begin : size 242
#[  ]MP #,D      #  8 #P L =(      %4 ; !T '( 80!' '( 90!P "  4@!E
#[  ]M ', =0!L '0 <P    @ 30!3 "  4P!A &X <P @ %, 90!R &D 9@      
#[  ]M 5      ]0"^ #( #@ ! /__@ !# &$ ;@!C &4 ;             %8    
#[  ]M +P O@ R  X 9@#__X  )@!% &0 :0!T "  1@!I &P 90         % (%0
#[  ]M      4 !0 B :\ 90!3 'D <P!, &D <P!T %8 :0!E '< ,P R    1P!E
#[  ]1 &X 90!R &D 8P Q       A
#[ ]end
#[]mks internal resource : icon : \0 : width 32 height 32 colors 16
#[ ]begin : size 744
#[  ]M*    "    !      0 $      "  @                              
#[  ]M (   (    " @ "     @ "  ("   # P,  @("     _P  _P   /__ /\ 
#[  ]M  #_ /\ __\  /___P#_________^O /__O[\   _W]_?W]_<*^@#_\/O[  
#[  ]M /_W]_?W]__Z\ __^_OP  #_?_#P\/#PKZ /_P^_L   __</____"OKP#__[
#[  ]M^_   /]_____\*^OH __#[^P  #_]P___P(B(B /__O[\   _W____ B(B(@
#[  ]M#_\/O[   /_W#_\       __^_OP  #_?__P(/KZ^O#__P^_L   __</ B"O
#[  ]MKZ\/___[^_   /]_\/(@^OKP?W]_#[^P  #P\*^B(*^O!_?W]_O[\   ^OKZ
#[  ]M\B#Z\ \/#P\/O[   *^OKZ(@KP_____P^_OP  #Z^OKR(/#P____#[^_L   
#[  ]M       /?___\/O[^P(        /]_#__P^_O[ B( #_______]___#[^_L*
#[  ]M(B( _______W\/\/O[^PJJHB(/_______W_P^_O["JJJHB+_______?P#[^_
#[  ]ML*JJJJHB#P\/#P\/#_O[^PJJJJJJHK^_O[^_O[^_O[#ZJJJJJJ#[^_O[^_O[
#[  ]M^_L/_ZJJJJH"O[^_O[^_O[^PJO_ZJJJ@HOO[^_O[^_O["JJO_ZJJ"J(     
#[  ]M     *^JJO_ZH*JJ           *^JJO_PJJH            *^JJO#_J@( 
#[  ]M           *^JH*K_ B             *^@JJH/(@   !\    ?    'P  
#[  ]M !\    ?    'P   !\    ?    'P   !\    ?    'P   !\    ?    
#[  ]M'P   !\    /    !P    ,    !                                
#[  ]8     /__  #__X  ___  /__X #___  
#[ ]end
#[]mks internal resource : icon : \1 : width 16 height 16 colors 16
#[ ]begin : size 296
#[  ]M*    !     @     0 $      #                                 
#[  ]M (   (    " @ "     @ "  ("   # P,  @("     _P  _P   /__ /\ 
#[  ]M  #_ /\ __\  /___P#_B(B(H/B_ /^/_XJ@^/L _X_X(B#XOP#_CX   /C[
#[  ]M /^("JH/^+\ B((*H/_X^P"JH@H(B(B_ *JB (__B_L    /C_B_L"#___^/
#[  ]MB_L*(O___XB_L*HBB(B(B_L*JJ*_O[^_L/JJH/O[^_L*KZH*     "JJ\*( 
#[  ]M     JH/H@ #6 4  P#_  /_^P #_P   P    ,'_P #__\  P("  $" @  
#[  ]: /\   #_  #[         /\'_P#___^   +P
#[ ]end
