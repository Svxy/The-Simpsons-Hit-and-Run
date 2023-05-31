#
# MKS Toolkit regps.ksh
#
# See pscript.5 man page for documentation for this command.
#
# $Id: regps.ksh 1.5 1998/10/09 17:10:15 fredw Exp $
#
# WARNING:
#  Do not edit/change this file.
#  It may be replaced on subsequent install of the MKS Toolkit.
#

ASP='HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\W3SVC\ASP'

# We need these keys for PScript.
regKey=$ASP'\LanguageEngines\PScript'
registry -s -k "$regKey" -n "Write" -v '$Response->Write(|);'
registry -s -k "$regKey" -n "WriteBlock" -v '$Response->WriteBlock(|);'

if [ $# -eq 0 ] ; then
	# no args; default is PScript
	Language="PScript"
else
	case $1 in
		JScript) ;;
		PScript) ;;
		VBScript) ;;
		*) echo "usage: regps [JScript|PScript|VBScript]" ; exit 1 ;;
	esac
	Language=$1
fi

# IIS 3 allows us to set the default scripting language in the registry.
# IIS 4 does not allow us to set the default scripting language in this manner.
NOTE=$(registry -p -r -k "$ASP" -n NOTE 2> nul)
if [ -z "$NOTE" ] ; then
	# IIS 3
	regKey=$ASP'\Parameters'
	registry -s -k "$regKey" -n "DefaultScriptLanguage" -v "$Language"
	exit 0
fi

# IIS 4

cat << EOI

In order to use PScript as the default scripting language in a web page
please put the following line at the top of said web page:
	<%@ LANGUAGE=PScript %>

Or, on Intel platforms, do the following in order to set the
default scripting language for all web pages:

 1) Start the "Internet Service Manager".
 2) Select the Web site or the starting point directory of an application. 
    Typically, this will be labelled "Default WEB Site".
 3) Right click on the above and then click on "Properties"
    to open the directory's property sheets.
 4) Click the "Home Directory", "Virtual Directory", or "Directory" tab. 
 3) Click the "Configuration" button.
 4) Click the "App Options" tab.
 5) Set the "Default ASP Language" property to
         $Language
 6) Click the "OK" button.

EOI
