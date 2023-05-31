# Run stdin through a compiler's C preprocessor.

# Some compilers require "\" as the directory/filename separator.
args=${1+$@}
while true ; do
	case $args in
		*/*)	args=${args%%/*}\\${args#*/};;
		*)	break;;
	esac
done

# Try Microsoft's compiler.
whence cl.exe 1> nul 2>&1
if [ $? -eq 0 ] ; then
	cat > $$.c
	cl -E $args $$.c 2> nul
	rm $$.c
	exit 0
fi

# Try Watcom's 32-bit compiler.
whence wcl386.exe 1> nul 2>&1
if [ $? -eq 0 ] ; then
	cat > $$.c
	wcl386 -zq -pc $args $$.c 2> nul
	rm $$.c
	exit 0
fi

# Try Watcom's 16-bit compiler.
whence wcl.exe 1> nul 2>&1
if [ $? -eq 0 ] ; then
	cat > $$.c
	wcl -zq -pc $args $$.c 2> nul
	rm $$.c
	exit 0
fi

# Try Borland's 32-bit C preprocessor.
whence cpp32.exe 1> nul 2>&1
if [ $? -eq 0 ] ; then
	cat > $$.c
	cpp32 -P- $args $$.c 2> nul 1>&2
	cat $$.i
	rm $$.c $$.i
	exit 0
fi

# Try Borland's 16-bit C preprocessor.
whence cpp.exe 1> nul 2>&1
if [ $? -eq 0 ] ; then
	cat > $$.c
	cpp -P- $args $$.c 2> nul 1>&2
	cat $$.i
	rm $$.c $$.i
	exit 0
fi
