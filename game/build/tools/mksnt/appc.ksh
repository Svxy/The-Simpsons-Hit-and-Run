#
# APPC.KSH - A Kornshell script, win32 implementation of bc(1) using dlg(1).
#
# Operation:	-See the man page for appc(1).
# 
# Assumptions:  -lib.b last default line includes the word "bessel".
#
# Todo:
#		-change output to listbox or combobox.
#		-Work on parse_lib() -USE CUSLIB.B new file.
#		-on base change, convert entire output list.
#		-max count output list.
#		-replace hotkeys w/o underscore.
#
#		-Combine .rc files into appc.rc.
#		-Icon needs work.
#		-LAST: Final arrangement of pushbuttons and tab order.
#		-LAST: Use "set list" for tidy tabs.
#
# Bugs:
#	-In function errfunc(), long error messages are clipped.
#	-In bc and I have not posted it yet: Modulo w/precision:
#		bc			
#		3%2
#		1	-ok
#		scale=2
#		3%2
#		0	-s/b "1" or "1.00"
#		quit
#	
# Programming notes:
#	 1) -Generally keep the focus on the "=" key then pressing
#            the Enter key while in an editbox works something like
#            like a calculator should.
#           -Then just keep building/displaying the input line until
#            the "=", any function key or the Enter key in the editbox.
#           -Display/keep the result until a numeric input which is
#	     not preceded by an operator.

debug=0		# Set to 1 to preserve temp files.

# Need to remove the trailing slash (if any) from ROOTDIR.
export ROOTDIR="${ROOTDIR:=c:}"; ROOTDIR="${ROOTDIR%/}"

# Remove all temporary files on ctrl-c exit.
if [ debug -eq 0 ]
then
    trap 'rm -f "$TMPDIR/appc.err" "$TMPDIR/*out" "$TMPDIR/temp.b"; exit' EXIT
fi

APPC_CID=1210	# Help context id.
BC_CID=1010	# Help context id.

# appc.rc
IDI_ICON1=150
ID_CLEAR=1100
ID_MR=1500 
ID_LROUNDBRACKET=1101; ID_RROUNDBRACKET=1102
ID_COMMA=1103
ID_NOT=1104
ID0=1000; ID1=1001; ID2=1002; ID3=1003; ID4=1004; ID5=1005; ID6=1006; ID7=1007
ID8=1008; ID9=1009; IDA=1010; IDB=1011; IDC=1012; IDD=1013; IDE=1014; IDF=1015
ID_DOT=1110; ID_EQUAL=1111; ID_ADD=1112; ID_SUBTRACT=1113
ID_MULTIPLY=1114; ID_DIVIDE=1115; ID_MODULO=1116; ID_POWER=1117
ID_EXP=1120; ID_LOG=1121; ID_LN=1122; ID_ATAN=1123
ID_ARCTAN=1124; ID_COS=1125; ID_SIN=1126; ID_JN=1127; ID_BESSEL=1128
ID_PROGRAM=3000
ID_CLOSE=2
ID_HLP=3
ID_f1=1201; ID_f2=1202; ID_f3=1203; ID_f4=1204; ID_f5=1205; ID_f6=1206
ID_f7=1207; ID_f8=1208; ID_f9=1209; ID_f10=1210; ID_f11=1211; ID_f12=1212
ID_BASE16=1300; ID_BASE10=1301; ID_BASE8=1302; ID_BASE2=1303
IDC_SCALE=2002
IDC_INPUT=2100
IDC_LISTOUT=2101

# Initialize variables.
logic_char[0]="("; logic_char[1]=")"; logic_char[2]=","; logic_char[3]="!"
op_char[0]="+"; op_char[1]="-"; op_char[2]="*"
op_char[3]="/"; op_char[4]="%"; op_char[5]="^"
lib_op[0]="exp"; lib_op[1]="log"; lib_op[2]="ln"; lib_op[3]="atan"
lib_op[4]="arctan"; lib_op[5]="cos"; lib_op[6]="sin"; lib_op[7]="jn"
lib_op[8]="bessel"
bc_str=""		# Number/operation string to feed bc.
bc_result=""		# Calculator result.
ibase="10"
obase="10"
precision="20"		# Default 20 decimal places.
clear_entry_flag=0	# 1: to clear input string when next entry begins.
			#    i.e. if next keypress is a number.
clear_button_count=0	# Clear button clear input string immediately.
			# Flag is set to indicate clear output list
			# If next button pressed is again Clear. 
hexstr="A B C D E F"; decstr="8 9 "; octstr="2 3 4 5 6 7 "
next=0			# Keep track of custom function displayed in progfunc().
reload=0		# Flag to reload fn button function names.
number=0		# Last custom function key pressed.

#
# Append an output list entry to the input line.
#
function list_to_input {
	clear_button_count=0
	dlg getcursel -c $IDC_LISTOUT index
	if [ index -ne -1 ]
	then
		dlg gettext -c $IDC_LISTOUT -i $index mr_str
		bc_str=$bc_str$mr_str
	fi
	dlg settext -c $IDC_INPUT -- "$bc_str"
	dlg setfocus -c $ID_EQUAL
}

#
# ERROR message function.
#
function errfunc {
	# appcerr.rc
	IDC_ERRMESS=9000
	ID_ERROK=9001 
	ID_ERRHELP=9002

	# Get current window coordinates.
	dlg getpos -x xpos -y ypos -w width -h height

	#Load the ERRMESS function dialog.
	if ! dlg load -m IDD_CALC -x -1 -y -1 -r err "$ResFile" IDD_ERRMESS
	then
		exit	#dlg will have given an error in a message box!
	fi

	# Size popup for error message. Build minimum width into it.
	# Assumes: fixedsys font where characters are 8 pixels wide.
	errmess=$(grep -v "bc" "$TMPDIR/appc.err")   # Retrieve error message.

	# Set error message box min width to allow for "Ok" and "Help" buttons.
	char_count=$[$(echo $errmess | wc -m) +17]
	if [ $char_count -gt 85 ]
	then
		char_count=85	# Limit window width.
	fi
	dlg setpos -x $[$xpos+$width/2-($char_count*8)/2] -y $[$ypos+19*8] -w $[$char_count*8] -h 125

	# Center the OK button
	dlg getpos -x xpos -y ypos -w width -h height
	dlg setpos -c $ID_ERROK -x $[($width/2)-100] -y 60 -w 90 -h 30
	dlg setpos -c $ID_ERRHELP -x $[($width/2)] -y 60 -w 90 -h 30
	dlg settext -c $IDC_ERRMESS "$errmess"	# Display the error message.
	while :
	do
		dlg event emsg econtrol
		case "$emsg" in
		command)	# Got a push button.
			case "$econtrol" in
			"$ID_ERROK" | "2") # Handle the CANCEL or ESCAPE button.
				break
				;;
			"$ID_ERRHELP") # Bring up bc(1) help.
				dlg winhelp -C $BC_CID "$ROOTDIR/etc/toolkit.hlp"
				continue
				;;
			*)		# Ignore any other control input.
				continue
				;;
			esac
			;;
		close)
			break
			;;	
		*)	# Ignore other windows messages.
			continue
			;;
		esac
	done
	if [ debug -eq 0 ]
	then
		rm -f "$TMPDIR/appc.err"
	fi
	dlg winhelp -q "$ROOTDIR/etc/toolkit.hlp"
	dlg close -d $err
}

#
# Auto load fn editboxes with user defined function names from lib.b.
#
function autoload {

	# Reset custom function button names.
	for i in 1 2 3 4 5 6 7 8 9 10 11 12
	do
		dlg settext -c $[ID_f$i] -- ""
	done
	i=1; count=1
	cat "$ROOTDIR/etc/lib.b" | awk '
	BEGIN {				# Predefined lib.b functions.
		x["e"]++
		x["l"]++
		x["a"]++
		x["c"]++
		x["s"]++
		x["j"]++
		x["exp"]++
		x["log"]++
		x["ln"]++
		x["atan"]++
		x["arctan"]++
		x["cos"]++
		x["sin"]++
		x["jn"]++
		x["bessel"]++
	}
	/^define / { 			# Isolate "define" lines.
		sub(/^define /, ""); 	# Parse for function name.
		sub(/\(.*$/, "")
		if (!($1 in x)) 	# Skip predefined lib.b functions.
			print
	} ' | while read bc_f[$i]
	do
		# Display custom function name in an editbox.
		dlg settext -c $[ID_f$i] -- "${bc_f[$i]}"
		i=$[$i+1]
	done
	count=$i		# Count user defined functions.
	
	# Load array with custom function names from function buttons.
	for i in 1 2 3 4 5 6 7 8 9 10 11 12
	do
		dlg gettext -c $[ID_f$i] -- bc_f[$i]
		if [ "${bc_f[$i]}" = "" ]
		then
			break
		fi
	done
	count=$[$i-1]
}

#
# PROGRAM button function.
#
function progfunc {
	# appcprog.rc
	IDC_PROG=4000
	IDCAN=100
	IDSAVE=101
	IDDEL=102
	IDPHELP=103
	IDPREV=110
	IDNEXT=111
	delete_func=0
	default_name="function_name"
	skeleton_func="/*\r\n * Sample function format.\r\n */\r\ndefine $default_name(x,y){\r\n        /* Body of function. */\r\n        return(0)\r\n}"

	#Load the PROGRAM function dialog.
	if ! dlg load -m IDD_CALC -x -1 -y -1 -r prog "$ResFile" IDD_PROG
	then
		exit	#dlg will have given an error in a message box!
	fi
	parse_lib

	# The last custom function button pressed is the function to display.
	if [ "${bc_f[$number]}" != "" ]
	then
		next=$number	# Function to view this time.
	fi

	# Load default skeleton function into editbox first time 
	# else display the last function viewed.
	if [ next -eq 0 ]
	then 
		dlg settext -c $IDC_PROG "$(echo "$skeleton_func")"
	else
		find_name="${bc_f[$next]}"	# Array was loaded in autoload()
		lookup
	fi

	while :
	do
		dlg event pmsg pcontrol
		case "$pmsg" in
		edit)	# Process edit box.
			case "$pcontrol" in
			*)	# Ignore any other windows edit messages.
				continue
				;;
			esac
				;;
		command)	# Got a push button.
			case "$pcontrol" in
			"$IDSAVE") 	# Handle the SAVE button.
				# Get the user defined function.
				dlg gettext -c $IDC_PROG -- lib_str

				# Get and test whether function name has changed
				find_name=$(echo "$lib_str" | grep "^define " | cut -f2 -d " " | cut -f1 -d"(")
				# Don't save sample function.
				if [ "$find_name" = "$default_name" ]
				then
					break
				fi 
				for i in 1 2 3 4 5 6 7 8 9 10 11 12
				do
					if [ "$find_name" = "${bc_f[$i]}" ]
					then
						delete_func=1
						lookup 	# Delete old function.
						break
					fi
				done
				# Convert the CRs to newlines then to CR/LFs and
				# append function to lib.b.
				# bc only looks 1 place for lib.b.
				echo "$lib_str" | tr -s "\r" "\n" | flip -d >> "$ROOTDIR/etc/lib.b"
				count=$[$count+1]
				next=$count	# View this function next time.
				reload=1
				break
				;;
			"$IDDEL") 	# Handle the DELETE button.
				delete_func=1
				lookup
				reload=1
				break
				;;
			"$IDPHELP") # Handle the HELP button.
				dlg winhelp -C $BC_CID "$ROOTDIR/etc/toolkit.hlp"
				continue
				;;
			"$IDPREV")
				next=$[$next-1]
				if [ $next -le 0 ]
				then
					next=1
					dlg enabled -c $IDPREV 0
				fi
				if [ $next -ge $count ]
				then
					dlg enabled -c $IDNEXT 0
				fi
				if [ $next -lt $count ]
				then
					dlg enabled -c $IDNEXT 1
				fi
				if [ $count -gt 0 ]
				then
					find_name="${bc_f[$next]}"
					lookup
				fi
				continue
				;;
			"$IDNEXT")
				next=$[$next+1]
				if [ $next -gt $count ]
				then
					next=$count
					dlg enabled -c $IDNEXT 0
				fi
				if [ $next -le 1 ]
				then
					next=1
					dlg enabled -c $IDPREV 0
				fi
				if [ $next -gt 1 ]
				then
					dlg enabled -c $IDPREV 1
				fi
				if [ $count -gt 0 ]
				then
					find_name="${bc_f[$next]}"
					lookup
				fi
				continue
				;;
			"$IDCAN" | "2") # Handle the CANCEL or ESCAPE button.
				break
				;;
			*)		# Ignore any other control input.
				continue
				;;
			esac
			;;
		close)		# For win95/nt 'X' button.
			break
			;;	
		*)	# Ignore other windows messages.
			continue
			;;
		esac
	done
	number=$next	# Function to view next time.
	dlg winhelp -q "$ROOTDIR/etc/toolkit.hlp"
	dlg close -d $prog
	if [ reload -ne 0 ]
	then
		reload=0
		autoload
	fi
	# Clean up temp files generated by this function.
	if [ debug -eq 0 ]
	then
		rm -f "$TMPDIR/*.out" 2> NUL
	fi
}

#
# Parse lib.b for progfunc.
#
function parse_lib {
	cat "$ROOTDIR/etc/lib.b" | awk -v tmpdir="$TMPDIR" '
	/bessel/, NR==10000 {	 	  # All lines from "bessel" to EOF.
		if (!("bessel(" == $2))	{ # Skip "bessel" line.

			# Parse lib.b custom functions into separate files.
			# Allow a comment before the define statement.
			# The comment and define must begin in column one.
			if ($0 ~ "^\/\*") {	# Comment preceding functon.
				funcs=funcs+1
				flag_comment=1
			}
			if ($0 ~ "^define ") {	# Function define line.
				flag_define=1
				if (flag_comment==0)
					funcs=funcs+1
			}
			if (flag_define==1) {
				flag_comment=0
				flag_define=0
			}
			# Output to numbered temp file.
			print $0 > tmpdir"/"funcs".out"
		}
	} '
}

#
# Lookup custom function for progfunc().
# Delete or display the lib.b function.
#
function lookup {
	if [ delete_func -ne 0 ]
	then
		# Rebuild lib.b without current custom function.
		count=$[$count-1]
		cat "$ROOTDIR/etc/lib.b" | awk -v tmpdir="$TMPDIR" '
		NR==1, /bessel/ {	# Keep default lib.b lines.
			print $0 > tmpdir"/temp.b"
		}'
		# Build temp lib without function to delete.
		for i in 1 2 3 4 5 6 7 8 9 10 11 12
		do
			if [ $i -ne $number ]
			then
				cat "$TMPDIR/$i.out" 2>NUL >> "$TMPDIR/temp.b"
			fi
		done
		mv "$TMPDIR/temp.b" "$ROOTDIR/etc/lib.b" # Replace lib.b.
	else
		# Display custom function. Format to make line breaks
		# in multi-line editbox.
		xstr=$(cat "$TMPDIR/$next.out" | awk ' {
			allstr=allstr $0"\r\n"
		}
		END {
			print substr(allstr, 1, length(allstr)-2)
		}') 
		dlg settext -c $IDC_PROG "$(echo "$xstr")"
	fi
}

#
#
# MAIN bc calculator script.
#
#

ResFile=$(whence "$0") 	# Tell dlg the .res file is located in the current file.
set -o guierror		# Specify gui error messages.

#Load and center the dialog.
if ! dlg load -x -1 -y -1 -r appc "$ResFile" IDD_CALC
then
	exit		#dlg will have given an error in a message box!
fi

dlg settext -c $IDC_SCALE $precision
dlg setfocus -c $ID_EQUAL
for i in $hexstr 	# Default is base 10 so disable hex entry digits.
do
	dlg enabled -c $[ID$i] 0
done
dlg checkbutton -c $ID_BASE10 1	# Set radio button to default base 10.
autoload

#Note the loop does not terminate on failure status from dlg
while :
do
	dlg event msg control			# Get a button press.
	case "$msg" in
	edit)	# Process edit boxes.
		clear_button_count=0
		case "$control" in
		"$IDC_SCALE")
			dlg gettext -c $IDC_SCALE -- precision
			continue
			;;
		"$IDC_INPUT")
			dlg gettext -c $IDC_INPUT -- bc_str
			continue
			;;
		*)	# Ignore any other windows edit messages.
			continue
			;;
		esac
		;;


	command)	# Got a push button.
		# Build and display the input string as each digit is entered.
		if  [ "$control" -ge "$ID0" -a "$control" -le "$IDF"  ]
		then
			clear_button_count=0
			
			# Reset the entire input string.
			if [ "$clear_entry_flag" -ne 0 ]
			then
				bc_str=""
				clear_entry_flag=0
			fi
			if  [ "$control" -le "$ID9" ]
			then
				tempstr=$[$control-1000]
			else
				case "$control" in
				"$IDA")
					tempstr="A"
					;;
				"$IDB")
					tempstr="B"
					;;
				"$IDC")
					tempstr="C"
					;;
				"$IDD")
					tempstr="D"
					;;
				"$IDE")
					tempstr="E"
					;;
				"$IDF")
					tempstr="F"
					;;
				esac
			fi
			# Append 0-F to input string and display it.
			bc_str=$bc_str$tempstr
			dlg settext -c $IDC_INPUT -- "$bc_str"
			dlg setfocus -c $ID_EQUAL
			continue
		fi

		# Process math operator buttons.
		# Unless continue, FALL THROUGH to bc.
		case "$control" in
		"$ID_CLOSE")	# Handle the CLOSE button.
			break	# TO END OF PROGRAM
			;;
		"$ID_HLP")	# Handle the HELP button.
			dlg winhelp -C $APPC_CID "$ROOTDIR/etc/toolkit.hlp"
			dlg setfocus -c $ID_EQUAL
			continue
			;;
		"$ID_CLEAR")
			# Effectively, double click clears list
			# else clear input line.
			if [ clear_button_count -ne 0 ]
			then
				clear_button_count=0
				dlg clear -c $IDC_LISTOUT
			else
				clear_button_count=1
				bc_str=""
				bc_result=""
			fi
			dlg setfocus -c $ID_EQUAL
			;;
		"$ID_LROUNDBRACKET" |
		"$ID_RROUNDBRACKET" |
		"$ID_COMMA"	    |
		"$ID_NOT")
			clear_button_count=0
			number=$[$control-$ID_LROUNDBRACKET]
			bc_str=$bc_str${logic_char[$number]}
			dlg settext -c $IDC_INPUT -- "$bc_str"
			clear_entry_flag=0
			dlg setfocus -c $ID_EQUAL
			continue
			;;
		"$ID_DOT")
			clear_button_count=0
			bc_str=$bc_str"."
			clear_entry_flag=0
			dlg setfocus -c $ID_EQUAL
			continue
			;;
		"$ID_ADD"      |
		"$ID_SUBTRACT" |
		"$ID_MULTIPLY" |
		"$ID_DIVIDE"   |
		"$ID_MODULO"   |
		"$ID_POWER")
			clear_button_count=0
			number=$[$control-$ID_ADD]
			bc_str=$bc_str${op_char[$number]}
			dlg settext -c $IDC_INPUT -- "$bc_str"
			clear_entry_flag=0
			dlg setfocus -c $ID_EQUAL
			continue
			;;
		"$ID_EQUAL")
			clear_button_count=0
			;;
		# Process user defined function buttons.
		# Wrap function around what ever has been entered
		# and feed it to bc(1).
		"$ID_f1"  | "$ID_f2"  | "$ID_f3"  | "$ID_f4"  |
		"$ID_f5"  | "$ID_f6"  | "$ID_f7"  | "$ID_f8"  |
		"$ID_f9"  | "$ID_f10" | "$ID_f11" | "$ID_f12")
			clear_button_count=0
			number=$[$control-$ID_f1+1]
			func_name="${bc_f[$number]}"
			if [ "$func_name" != "" ]
			then
				bc_str=$func_name"("$bc_str")"
				dlg settext -c $IDC_INPUT -- "$bc_str"
			fi
			clear_entry_flag=0
			dlg setfocus -c $ID_EQUAL
			if [ "$func_name" = "" ]
			then
				continue	# Do not do bc now.
			fi
			;;

		# Process functions built in to $ROOTDIR/etc/lib.b.
		"$ID_EXP"    | "$ID_LOG"    | "$ID_LN"     |
		"$ID_ATAN"   | "$ID_ARCTAN" | "$ID_COS"    |
		"$ID_SIN"    | "$ID_JN"     | "$ID_BESSEL")
			clear_button_count=0
			number=$[$control-$ID_EXP]
			func_name="${lib_op[$number]}"
			bc_str=$func_name"("$bc_str")"
			dlg settext -c $IDC_INPUT -- "$bc_str"
			clear_entry_flag=0
			dlg setfocus -c $ID_EQUAL
			;;

		"$ID_PROGRAM")
			clear_button_count=0
			progfunc
			dlg setfocus -c $ID_EQUAL
			continue
			;;

		# Base radio controls FALL THROUGH to show change.
		# Set output base for input line base conversion.
		# Enable/disable number buttons.
		"$ID_BASE16")
			clear_button_count=0
			obase="16"
			for i in $octstr$decstr$hexstr 
			do
				dlg enabled -c $[ID$i] 1
			done
			;;
		"$ID_BASE10")
			clear_button_count=0
			obase="10"
			for i in $octstr$decstr 
			do
				dlg enabled -c $[ID$i] 1
			done
			for i in $hexstr 
			do
				dlg enabled -c $[ID$i] 0
			done
			;;
		"$ID_BASE8")
			clear_button_count=0
			obase="8"
			for i in $octstr 
			do
				dlg enabled -c $[ID$i] 1
			done
			for i in $decstr$hexstr 
			do
				dlg enabled -c $[ID$i] 0
			done
			;;
		"$ID_BASE2")
			clear_button_count=0
			obase="2"
			for i in $octstr$decstr$hexstr 
			do
				dlg enabled -c $[ID$i] 0
			done
			;;
		"$ID_MR")	# Append list item to input string.
			list_to_input
			continue
			;;
		"1") 	# 1 is the Enter key.
			clear_button_count=0
			dlg setfocus -c $ID_EQUAL
			;;
		*)	# Ignore any other control input.
			clear_button_count=0
			continue
			;;
		esac	
		;;
	double)	# Process mouse double click.
		clear_button_count=0
		case "$control" in
		"$IDC_LISTOUT")	# Append list item to input string.
			list_to_input
			;;
		esac
		continue
		;;
	close)		# For win95/nt 'X' button.
		break
		;;	
	*)	# Ignore other windows messages.
		clear_button_count=0
		continue
		;;
 	esac		#$msg

	# Change dialog title while waiting for bc.
	dlg settext -d $appc "APPC - Input submitted to bc(1)"

	# Call bc and display the result.
	bc_result=$(echo "scale=$precision\nobase=$obase\nibase=$ibase\n $bc_str" | bc -l  2>"$TMPDIR/appc.err")

	if [ $? != "0" 	]		# Report any bc error.
	then	# Show the error message then allow user to re-edit the input. 
		errfunc

		# Here's a trick. The next keypress after setfocus blanks
		# the editbox field unless settext is used.
		dlg setfocus -c $IDC_INPUT
		dlg settext -c $IDC_INPUT -- "$bc_str"
	else	# Display bc(1) results.
		# Strip '\' line break noise from large numbers.
		bc_result=$(echo $bc_result | tr -d "\\ ")
		if [ "$bc_result" != "" ]	# Add to list.
		then
			dlg inserttext -c $IDC_LISTOUT -- "$bc_result" 
			clear_entry_flag=1
		fi
		# Output result to input line.
		dlg settext -c $IDC_INPUT -- "$bc_result"
	fi
	
	# Restore dialog title after waiting for bc.
	dlg settext -d $appc "Arbitrary Precision Programmable Calculator"

	if [ $ibase != $obase ]
	then
		ibase=$obase
	fi
done
dlg winhelp -q "$ROOTDIR/etc/toolkit.hlp"
dlg close -d $appc
#[]mks internal resource : dialog : IDD_CALC
#[ ]begin : size 2102
#[  ]M0 #.D      ^       V :\      $$ <@!B &D = !R &$ <@!Y "  4 !R
#[  ]M &4 8P!I ', :0!O &X ( !0 '( ;P!G '( 80!M &T 80!B &P 90 @ $, 
#[  ]M80!L &, =0!L &$ = !O '(    ( $8 :0!X &4 9 !S 'D <P         !
#[  ]M4      % %  %  / $P$__^  $, 10 O $,            !4      % %\ 
#[  ]M%  / -P%__^  $T )@!2          %0      4 ;@ 4  \ 303__X  )@ H
#[  ]M             5      !0!] !0 #P!.!/__@  F "D            !4   
#[  ]M   % (P %  / $\$__^  "8 +             %0      4 FP 4  \ 4 3_
#[  ]M_X  )@ A             5      &0"; !0 #P#H ___@  F #          
#[  ]M   !4      9 (P %  / .D#__^  "8 ,0            %0     "T C  4
#[  ]M  \ Z@/__X  )@ R             5      00", !0 #P#K ___@  F #, 
#[  ]M           !4      9 'T %  / .P#__^  "8 -             %0    
#[  ]M "T ?0 4  \ [0/__X  )@ U             5      00!] !0 #P#N ___
#[  ]M@  F #8            !4      9 &X %  / .\#__^  "8 -P          
#[  ]M  %0     "T ;@ 4  \ \ /__X  )@ X             5      00!N !0 
#[  ]M#P#Q ___@  F #D            !4      9 %\ %  / /(#__^  "8 00  
#[  ]M          %0     "T 7P 4  \ \P/__X  )@!"             5      
#[  ]M00!? !0 #P#T ___@  F $,            !4      9 %  %  / /4#__^ 
#[  ]M "8 1             %0     "T 4  4  \ ]@/__X  )@!%            
#[  ]M 5      00!0 !0 #P#W ___@  F $8            !4      M )L %  /
#[  ]M %8$__^  "8 +@            %0     $$ FP 4  \ 5P3__X  )@ ]    
#[  ]M         5      50!0 !0 #P!8!/__@  F "L            !4     !5
#[  ]M %\ %  / %D$__^  "8 +0            %0     %4 ;@ 4  \ 6@3__X  
#[  ]M)@ J             5      50!] !0 #P!;!/__@  F "\            !
#[  ]M4     !5 (P %  / %P$__^  "8 )0            %0     %4 FP 4  \ 
#[  ]M703__X  )@!>             5      ;@!0 !X #P!@!/__@ !E '@ <   
#[  ]M       !4     !N %\ '@ / &$$__^  &P ;P!G          %0     &X 
#[  ]M;@ >  \ 8@3__X  ; !N             5      ;@!] !X #P!C!/__@ !A
#[  ]M '0 80!N             5      ;@", !X #P!D!/__@ !A '( 8P!T &$ 
#[  ]M;@            %0     (P 4  >  \ 903__X  8P!O ',          5  
#[  ]M    C !? !X #P!F!/__@ !S &D ;@         !4     ", &X '@ / &<$
#[  ]M__^  &H ;@            %0     (P ?0 >  \ : 3__X  8@!E ', <P!E
#[  ]M &P            !4     ", (P '@ / +@+__^  %  <@!O &< <@!A &T 
#[  ]M         5      " %# "@ #0 " /__@ !# &P ;P!S &4          5  
#[  ]M    X !# "@ #0 # /__@ !( &4 ; !P "           5      K@!0 $$ 
#[  ]M#P"Q!/__@             %0     *X 7P!!  \ L@3__X             !
#[  ]M4     "N &X 00 / +,$__^              5      K@!] $$ #P"T!/__
#[  ]M@             %0     *X C !!  \ M03__X             !4     "N
#[  ]M )L 00 / +8$__^              5      [P!0 $$ #P"W!/__@       
#[  ]M      %0     .\ 7P!!  \ N 3__X             !4     #O &X 00 /
#[  ]M +D$__^              5      [P!] $$ #P"Z!/__@             %0
#[  ]M     .\ C !!  \ NP3__X             !4     #O )L 00 / +P$__^ 
#[  ]M          D  E      !0!& !D "  4!?__@ !( &4 >       "0  4   
#[  ]M   > $8 &0 ( !4%__^  $0 90!C       )  !0     #< 1@ 9  @ %@7_
#[  ]M_X  3P!C '0       D  %      4 !& !D "  7!?__@ !" &D ;@      
#[  ]M   "4     "4 $8 &0 ( /____^" "8 4P!# $$ 3 !%          @0@5  
#[  ]M    ;@!$ ", "@#2!___@0        ""$(%0      4 -P K 0P - C__X$ 
#[  ]@         0"A4      %  , *P$T #4(__^#        
#[ ]end
#[]mks internal resource : dialog : IDD_PROG
#[ ]begin : size 334
#[  ]MP #(@      '  H ,@ = 6X      %  <@!O &< <@!A &T ( !! %  4 !#
#[  ]M    " !& &D > !E &0 <P!Y ',       %0     /  !0 H  \ 9 #__X  
#[  ]M)@!# &$ ;@!C &4 ;        0 !4     #P !D *  / &4 __^  "8 4P!A
#[  ]M '8 90         !4     #P "T *  / &8 __^  "8 1 !E &P 90!T &4 
#[  ]M         5      \ !! "@ #P!G /__@  F $@ 90!L '           5  
#[  ]M    00!: "@ #P!N /__@  \ #P (  F %  <@!E '8            !4   
#[  ]M  !S %H *  / &\ __^  "8 3@!E '@ =  @ #X /@        #$$(%0    
#[  ]3  4 !0#F %  H ___X$         
#[ ]end
#[]mks internal resource : dialog : IDD_ERRMESS
#[ ]begin : size 162
#[  ]MP #(@      #                 $$ 4 !0 $, (  M "  10!R '( ;P!R
#[  ]M    " !& &D > !E &0 <P!Y ',   "   )0      4 !0!  1D *"/__X( 
#[  ]M(          !4        !X +0 - "DC__^  $\ :P            %0    
#[  ]; &0 '@ M  T *B/__X  2 !E &P <       
#[ ]end
#[]mks internal resource : icon : \0 : width 32 height 32 colors 16
#[ ]begin : size 744
#[  ]M*    "    !      0 $      "  @                              
#[  ]M (   (    " @ "     @ "  ("   # P,  @("     _P  _P   /__ /\ 
#[  ]M  #_ /\ __\  /___P                         $B(B(B(B(B(B(@   
#[  ]M    ! =W=W=W=W=W=X        0'B(B(AXB'B(>        $!P=W=X<'AP>'
#[  ]M@       ! <   "' (<'AX        0'=W=W=W=W!X>        $!XB'B(>(
#[  ]MAP>'@       ! <'AP>'!X<'AX        0' (< AP"' (>        $!W=W
#[  ]M=W=W=W=W@       ! >(AXB'B(>(AX        0'!X<'AP>'!X>        $
#[  ]M!P"' (< AP"'@       ! =W=W=W=W=W=X        0'B(>(AXB'B(>     
#[  ]M   $!P>'!X<'AP>'@       ! < AP"' (< AX        0'=W=W=W=W=W> 
#[  ]M       $!XB'B(>(AXB'@       ! <'AP>'!X<'AX        0' (< AP"'
#[  ]M (>        $!W=W=W=W=W=W@       ! =$1$1$1$1$1X        0'0 X.
#[  ]M#@X.#D>        $!T#@X.#@X.!'@       ! = #@X.#@X.1X        0'
#[  ]M0        $>        $!T1$1$1$1$1'@       ! =W=W=W=W=W=X      
#[  ]M  0                     1$1$1$1$1$1$0    /P  '_X   _^@  /_H 
#[  ]M #_Z@(@_^OS(/_H "#_Z  @_^HB(/_K,S#_Z   _^@  /_J(B#_ZS,P_^@  
#[  ]M/_H  #_ZB(@_^LS,/_H  #_Z   _^HB(/_K,S#_Z   _^@  /_IJJ#_Z550_
#[  ]8^FJH/_I__#_Z   _^@  /_O__[_\  !_
#[ ]end
#[]mks internal resource : icon : \1 : width 16 height 16 colors 16
#[ ]begin : size 296
#[  ]M*    !     @     0 $      #                                 
#[  ]M (   (    " @ "     @ "  ("   # P,  @("     _P  _P   /__ /\ 
#[  ]M  #_ /\ __\  /___P            "(>'AX>    (!'1T='    @ <'!P@ 
#[  ]M  " 1T='1P   ( '!P<(    @$='1T<   " !P<'"    (!'1T='    @ <'
#[  ]M!P@   " =W=W=P   ( (B(B(    @ #@X(<   "     "    (          
#[  ]M"(B(B(@  . /  # !P  T <  -JG  #0!P  VJ<  - '  #:IP  T <  -JG
#[  ]:  #0!P  T <  -5'  #0!P  W_<  . /    
#[ ]end
