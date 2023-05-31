#
# The `assoc' command.  Modeled after the assoc command built into cmd.exe,
# but using normal filter style delimiters.
# This command isn't much use without the `ftype' command, if we
# were designing these commands ourselves, rather than using the syntax from
# cmd.exe, we would merge them in some way.
#
# Copyright 1997 by Mortice Kern Systems Inc.  All rights reserved.
#
# Written by Alex White
#

usage() {
	print >&2 "Usage: assoc .ext filetype"
	exit 2
}

format() {
	awk '
		BEGIN {
			FS = "\t";
		}
		$1 ~ /^HKEY_CLASSES_ROOT\\\./ && $2 == "" {

			# Remove registry prefix
			sub(/^HKEY_CLASSES_ROOT\\/, "", $1)

			# Remove entries which have subentries -- they
			# should not be there, but buggy programs do it!
			if ($1 ~ /\\/)
				next

			# The registry value is quoted, remove the quotes
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
0)	# Print all file associations

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
	echo "$val" | format
	;;

2)	# Set a file type association $1 to $2
	registry -s -k HKEY_CLASSES_ROOT\\"$1" -n "" -v "$2"
	;;

*)	usage
	;;
esac
