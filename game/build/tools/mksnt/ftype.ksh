#
# The `ftype' command.  Modeled after the ftype command built into cmd.exe,
# but using normal filter style delimiters.
#
# Copyright 1997 by Mortice Kern Systems Inc.  All rights reserved.
#
# Written by Alex White
#

usage() {
	print >&2 "Usage: ftype [filetype [openCommandString]]"
	exit 2
}

format() {
	awk '
		BEGIN {
			FS = "\t";
		}
		$1 ~ /^HKEY_CLASSES_ROOT\\[^\.]/ && $2 == "" {

			# Remove registry prefix
			sub(/^HKEY_CLASSES_ROOT\\/, "", $1)

			key = $1
			sub(/^.*$/, "\\l&", key)
			if (key !~ /\\[sS]hell\\[oO]pen\\[cC]ommand$/)
				next
			sub(/\\.....\\....\\.......$/, "", $1)

			# Make sure it is not under the CLSID subkey...
			if ($1 ~ /\\/)
				next

			# The registry value is quoted, remove the quotes
			# May lead with a $ to indicate expanded string
			sub(/^\$/, "", $3)
			sub(/^"/, "", $3)
			sub(/"$/, "", $3)
			printf "%s\t%s\n", $1, $3
		}
	'
}

# Parse options
while getopts "" o
do	case "$o" in
	[?])	usage
		;;
	esac
done
shift $OPTIND-1 

case $# in
0)	# Print all open strings

	### TODO: Speed this up with a don't recurse option to registry.
	registry -p -k HKEY_CLASSES_ROOT | format
	;;

1)	# Print the single file association for extention in $1
	# TODO: What if there is some other form of error than Key not found
	# from registry?  We will take that as association not found, rather
	# than a proper error to the end user.
	if ! val=$(registry -p -k HKEY_CLASSES_ROOT\\"$1" 2>nul)
	then
		print -r $0: No file association found for \""$1"\" >&2
		exit 1
	fi
	print -r "$val" | format
	;;

2)	# Set a file type association $1 to $2
	registry -s -k HKEY_CLASSES_ROOT\\"$1"\\Shell\\Open\\Command -n "" -v "$2"
	;;

*)	usage
	;;
esac
