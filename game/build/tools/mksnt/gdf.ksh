#
# gdf -- graphical display free space
#
# Copyright 1996 by Mortice Kern Systems Inc.  All rights reserved.
#
# Written by Steven Church
#

# Locate the resources
ResFile=$(whence "$0")

# Define the controls
Refresh=1
Exit=2
Help=3
ListBox=4
Headers=5
BlockGroup=20
Size512=21
Size1024=22
InfoGroup=30
InfoPart=31
InfoComp=32
DrivesGroup=40
Drivebox=41

# Initialize variables
options=""
alldrives="$(sysinf drives)"
export NULLDEV=$(getconf _CS_NULLDEV)

# Parse options
while getopts ckPt? o
do	case "$o" in
	c)	cflag="1"
		;;
	k)	kflag="1"
		options=$options"-k "
		;;
	P)	Pflag="1"
		options=$options"-P "
		;;
	t)	
		;;
	\?)	print >&2 "Usage: $0 [-c] [-k] [-P] [-t] [filesystem ...]"
		print >&2 ""
		print >&2 "	-c	use command line version"
		print >&2 "	-k	use 1024-byte blocks"
		print >&2 "	-P	display complete information"
		print >&2 "	-t	display used space and free space"
		print >&2 "   filesystem	drive letter"
		exit 2
		;;
	[?])	print >&2 "Usage: $0 [-c] [-k] [-P] [-t] [filesystem ...]"
		exit 2
		;;
	esac
done
shift $OPTIND-1 
operands=$@

if [ "$cflag" -eq "1" ]
then
	df $options $operands
	exit 0
fi

# Load the dialog
dlg load -x -1 -y -1 "$ResFile" DIALOG_3
dlg clear -c $ListBox
	#"move left($InfoGroup) right($BlockGroup) +6" \

dlg layout \
	"move bottom($Refresh) bottom(parent) -6" \
	"move bottom($Exit) bottom($Refresh)" \
	"move bottom($Help) bottom($Refresh)" \
	"move left($Exit) right($Refresh) +6" \
	"move left($Help) right($Exit) +6" \
	"move bottom($ListBox) top($Refresh) -6" \
	"move left($ListBox) left(parent) +6" \
	"move left($BlockGroup) left(parent) +6" \
	"move right($DrivesGroup) right(parent) -6" \
	"hcenter child($InfoGroup) child(parent)" \
	"stretch right($BlockGroup) left($InfoGroup) -6" \
	"stretch left($DrivesGroup) right($InfoGroup) +6" \
	"stretch top($ListBox) bottom($BlockGroup) +6" \
	"stretch right($ListBox) right(parent) -6" \
	"hcenter group($Size512, $Size1024) child($BlockGroup)" \
	"hcenter group($InfoPart, $InfoComp) child($InfoGroup)" \
	"hcenter child($Drivebox) child($DrivesGroup)" \
	"stretch width($Drivebox) widthof($DrivesGroup, 80)" \
	"hcenter group($Refresh, $Help) child(parent)"

dlg checkbutton -c $Size512 1
if [ "$kflag" -eq "1" ]
then
	dlg checkbutton -c $Size1024 1
	dlg checkbutton -c $Size512 0
fi
dlg checkbutton -c $InfoPart 1
if [ "$Pflag" -eq "1" ]
then
	dlg checkbutton -c $InfoComp 1
	dlg checkbutton -c $InfoPart 0
fi

# Create "Drives" Listbox
dlg addtext -c $Drivebox "From Operands"
dlg addtext -c $Drivebox "All Drives"
for i in A $alldrives
do
	dlg addtext -c $Drivebox "$i:"
done

function refresh
{
	# Display output
	df $options $1 > tmpgdf 2>$NULLDEV
	if [ $? -ne "0" ]
	then
		msgbox -q -i stop "GDF Error" "Not all drives are accessible."
	fi

	if [ "$Pflag" -ne 0 ]
	then
		dlg columns -n -c $ListBox Filesystem 175 Blocks 80 Used 80 Available 80 "Capacity" 70 "Mounted on" 90
		i=0
		while read -r fs bl used avail cap mount
		do
			if [ $i -eq 0 ]
			then
				# Skip header
				i=1
				continue
			fi
			dlg addtext -c $ListBox "$fs" "$bl" "$used" "$avail" "$cap" "$mount"
		done <tmpgdf
	else
		dlg columns -n -c $ListBox Drive 50 Device 200 Free/Total 200
		while read -r drive device free
		do
			dlg addtext -c $ListBox "$drive" "$device" "$free"
		done <tmpgdf
	fi


	rm -f tmpgdf
}

refresh "$operands"

# Start a loop to read events from the dialog
while dlg event msg ctrl
do
	case $ctrl in
	$Refresh)
		# Clear Display
		dlg clear -c $ListBox

		# Parse Options
		dlg isbuttonchecked -c $Size1024 s1024
		dlg isbuttonchecked -c $InfoComp icomp
		options=""
		if [ $s1024 -eq 1 ]
		then
			options=$options"-k "
			kflag=1
		else
			kflag=0
		fi
		if [ $icomp -eq 1 ]
		then
			options=$options"-P"
			Pflag=1
		else
			Pflag=0
		fi
		dlg getcursel -c $Drivebox drivesel
		dlg gettext -c $Drivebox -i $drivesel res_text 2>$NULLDEV
		case $drivesel in
		"-1")	drives=$operands ;;
		0)	drives=$operands ;;
		1)	drives="" ;;
		*)	drives=$res_text ;;
		default)	drives="" ;;
		esac

		refresh "$drives"
		;;
	$Exit)
		break
		;;
	$Help)
		dlg winhelp -C 1235 "$ROOTDIR/etc/toolkit.hlp"
		;;
	esac
done

dlg close
#[]mks internal resource : dialog : DIALOG_3
#[ ]begin : size 616
#[  ]M0 #.D      ,  0 #P!: <L      $0 :0!S '  ; !A 'D ( !& '( 90!E
#[  ]M "  4P!P &$ 8P!E    " !- %, ( !3 &$ ;@!S "  4P!E '( :0!F    
#[  ]M   '  !0      < !0!S #( % #__X  0@!L &\ 8P!K "  4P!I 'H 90  
#[  ]M       )  -0     "< %  R  P %0#__X  )@ U #$ ,@ @ &( >0!T &4 
#[  ]M<P         )  !0     "< (P R  P %@#__X  )@ Q #  ,@ T "  8@!Y
#[  ]M '0 90!S       '  !0     (0 !0!S #( '@#__X  20!N &8 ;P!R &T 
#[  ]M80!T &D ;P!N       )  -0     *0 %  R  P 'P#__X  )@!0 &$ <@!T
#[  ]M &D 80!L          D  %      I  C #( #  @ /__@  F $, ;P!M '  
#[  ]M; !E '0 90      !P  4     #[  4 5@ R "@ __^  $0 <@!I '8 90!S
#[  ]M          , HU       P$3 $@ (P I /__@P         ! (-0      @ 
#[  ]M.P!( 78 ! !3 'D <P!, &D <P!T %8 :0!E '< ,P R    3 !I ', =  Q
#[  ]M       !  -0     %4 N0 R  X  0#__X  )@!2 &4 9@!R &4 <P!H    
#[  ]M         5      D0"Y #( #@ " /__@  F $4 > !I '0          5  
#[  ]?    S0"Y #( #@ # /__@  F $@ 90!L '       . !
#[ ]end
#[]mks internal resource : icon : \0 : width 32 height 32 colors 16
#[ ]begin : size 744
#[  ]M*    "    !      0 $      "  @                              
#[  ]M (   (    " @ "     @ "  ("   # P,  @("     _P  _P   /__ /\ 
#[  ]M  #_ /\ __\  /___P                                          
#[  ]M                                "9                    F0    
#[  ]M               )D                   "9                    F0
#[  ]M                   )D                   "9                  
#[  ]M  F0          #__P   )D)D)D   #P    \     "9F9F9    \    /  
#[  ]M    "9F9D    /    #_\     "9F0    #_\   \       "9      \   
#[  ]M /               /    #__P            #__P                  
#[  ]M                          #P                #P  _P  L       
#[  ]M  L  /\   __"[L   NP  "[L/_P    \+NP   +L   "[L/      N[#_  
#[  ]M"[  #_"[L     "[L/___PNP____"[L     "P  #_\+L/_P  "P        
#[  ]M    "[                    NP                                
#[  ]M                                         .    ?    #@    0  
#[  ]M                                                            
#[  ]M                                                            
#[  ]8                @    <    /@   '
#[ ]end
