#include <supersprint/supersprintdata.h>

const char* SuperSprintData::CHARACTER_NAMES[] =
{
    "lisa",
    "bart",
    "homer",
    "marge",
    "apu" 
};
const unsigned int SuperSprintData::NUM_CHARACTER_NAMES = sizeof( SuperSprintData::CHARACTER_NAMES ) / sizeof( SuperSprintData::CHARACTER_NAMES[0] );

const SuperSprintData::DisplayNames SuperSprintData::VEHICLE_NAMES[] =
{
    //
    // SYNTAX:
    //      { "<vehicle name>", "<display name>" },
    //
    { "snake_v",  "" },
    { "bookb_v",  "" },
    { "marge_v",  "" },
    { "carhom_v", "" },
	{ "krust_v",  "" },
    { "bbman_v",  "" },
    { "elect_v",  "" },	   
    { "famil_v",  "" },
    { "bart_v",   "" },
	{ "scorp_v",  "" },
	{ "honor_v",  "" },
    { "hbike_v",  "" },
    { "frink_v",  "" },
 	{ "comic_v",  "" },
    { "lisa_v",   "" },
    { "smith_v",  "" },
    { "mrplo_v",  "" },
    { "fone_v",   "" },
	{ "cletu_v",  "" },
	{ "apu_v",    "" },
	{ "plowk_v",  "" },
    { "wiggu_v",  "" },
	{ "otto_v",   "" },
    { "moe_v",    ""  },
	{ "skinn_v",  "Sedan (Skinner - Lvl 3)" },
	{ "homer_v",  "" },
    { "zombi_v",  "" },
    { "burns_v",  "" },
	{ "willi_v",  "" },
    { "gramp_v",  "" },
    { "gramR_v",  "" }, 

    { "atv_v",    "" },
    { "knigh_v",  "" },
    { "mono_v",   "" },
	{ "oblit_v",  "" },
    { "hype_v",   "" },
    { "rocke_v",  "" },

	{ "cArmor",   "" },
    { "cCellA",   "" },
	{ "cSedan",   "" },
	{ "cCola",    ""  },
    { "cCube",    ""  },
    { "cCurator", "" },
    { "cDonut",   "" },
    { "cDuff",    "" },
    { "cBlbart",  "" },
	{ "cHears",   "" },
    { "cKlimo",   "" },
    { "cLimo",    "" },
	{ "cMilk",    "" },
    { "cNerd",    "" },
    { "cNonup",   "" },
    { "cPolice",  "" },
    { "cVan",     "" },
    { "cBone",    "" },  

//    { "huskA",    "Charred Husk (Traffic)" },
    { "compactA", "" },
    { "minivanA", "" },
    { "pickupA",  "" },
    { "sedanA",   "" },
    { "sedanB",   "" },
    { "sportsA",  "" },
    { "sportsB",  "" },
    { "wagonA",   "" },
	{ "SUVA",     "" },
    { "taxiA",    "" },
    { "coffin",   "" },
//    { "ship",     "Ghost Ship (Halloween Traffic)" },
    { "hallo",    "" },
//    { "witchcar", "Witch Broom (Halloween Traffic)" },
    
};
const unsigned int SuperSprintData::NUM_NAMES = sizeof( SuperSprintData::VEHICLE_NAMES ) / sizeof( SuperSprintData::VEHICLE_NAMES[0] );
