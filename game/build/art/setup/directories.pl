#WORKING DIRECTORIES
#===================
$CURR = GetCurDir();
$CURR = "$CURR\\";

$WORK_DIR = "${CURR}WORK\\";

$ERRORS_DIR_NAME = "ERRORS";
$ERRORS_DIR = "$WORK_DIR$ERRORS_DIR_NAME\\";

$TEMP_DIR_NAME = "TEMP";
$TEMP_DIR = "$WORK_DIR$TEMP_DIR_NAME\\";

$STORE_DIR_NAME = "STORE";
$STORE_DIR = "$WORK_DIR$STORE_DIR_NAME\\";

$EXPORT_LIBRARIES_NAME = "EL";
$EXPORT_LIBRARIES = "$WORK_DIR$EXPORT_LIBRARIES_NAME\\";

#Active directory levels relative to place in exportart dir.
############################################################
$ACTIVE_EXPORT;
$ACTIVE_TEMP;
$ACTIVE_STORE;

#Where do the files go?
#######################

$CD_DIR = "${CURR}..\\..\\cd\\";
$ART_DIR = "art";

#This will be CD_DIR+PLATFORM+ART_DIR
$RESULT_DIR;

$TO_EXPORTART_DIR = "..\\..\\exportart\\";
$EXPORTART_DIR = "${CURR}$TO_EXPORTART_DIR";
$EXPORTARTLIBRARY_DIR = "${CURR}..\\..\\exportartlibrary\\";
$EXPORT_ART_WORK_DIR = $TEMP_DIR;

$TEMPLATES_DIR = "${CURR}templates\\";


#Where do the executables come from?
#===================================
@BIN = 
(
    "..\\..\\libs\\pure3d\\tools\\commandline\\bin\\",
    "..\\..\\libs\\radcore\\bin\\",
    "..\\tools\\",
    "..\\..\\..\\tools\\ArtChecker\\"
);

#Do not edit below this line
1;

