@PLATFORMS = 
(
    "GC",
    "PS2", 
    "XBOX",
    "PC"
);

@ACTIVE_PLATFORMS;
$CURRENT_PLATFORM;

@OPTIONS = 
(
    "HELP",
    "QUICK",
    "DEBUG",
    "COMPRESS",
    "UNSAFE"
);

@ACTIVE_OPTIONS;

#Error Levels
#============

$SUCCESS            = 0;
$BAD_COMMAND        = 1;
$BAD_FILE           = 2;
$GENERAL_FAILURE    = 3;
$BAD_TEMPLATE       = 4;
$COMMAND_ERROR      = 5;
$RULE_FAILURE       = 6;
$SYNTAX_ERROR       = 7;

$SEVERITY = 0;

#Do not edit below this line
1;
