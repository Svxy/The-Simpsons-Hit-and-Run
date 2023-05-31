#ifndef VEHICLE_ENUM
#define VEHICLE_ENUM

namespace VehicleEnum
{
    /*
    enum VehicleID
    {
        FERRINI,        //Bart
        TRANS_AM,       //Apu
        BANDIT,         //Snake
        BARRACUDA,      //Homer
        PINK_SEDAN,     //Homer
        JEEP,           //Grampa
        PICKUP,         //Cletus
        POLICE,         //Wiggum
        CLOWN_CAR,      //Krusty
        CANYONARO,      //Marge
        OTTOBUS,        //Otto
        DUFF_TRUCK,     //Moe
        LIMO,           //Smithers
        MOTORHOME,      //Flanders
        HUMMER1,        //McBain (regular)
        ALIEN,          //Kodos & Kang
        BUGGY,          //Zombie 1?
        MUNSTERS,       //Zombie 2?
        HUMMER2,        //McBain (Halloween)
    
        NUM_VEHICLES,
        INVALID
    };
    */

    // new enum list with enums matching new names:
    // ordered so that old numeric values will work

    enum VehicleID
    {
        BART_V = 0,                     // bart_v   0
        APU_V,                          // apu_v    1
        SNAKE_V,                        // snake_v  2
        HOMER_V,                        // homer_v  3
        FAMIL_V,                        // famil_v  4
        GRAMP_V,                        // gramp_v  5
        CLETU_V,                        // cletu_v  6
        WIGGU_V,                        // wiggu_v  7
        KRUSTYKAR_NOTYETIN,             //
        MARGE_V,                        // marge_v  9
        OTTOBUS_NOTYETIN,
        MOESDUFFTRUCK_NOTYETIN,
        SMITH_V,                        // smith_v  12
        FLANDERSMOTORHOME_NOTYETIN,
        MCBAINHUMMER_NOTYETIN,
        ALIEN_NOTYETIN,
        ZOMBI_V,                        // zombi_v  16
        MUNSTERS_NOTYETIN,
        HUMMER2_NOTYETIN,
        
        CVAN,                           // cVan     19
        COMPACTA,                       // compactA 20

        COMIC_V,                        // comic_v  21
        SKINN_V,                        // skinn_v  22


        // some more new ai cars
        CCOLA,      // 23
        CSEDAN,     // 24
        CPOLICE,    // 25
        CCELLA,     // 26
        CCELLB,     // 27
        CCELLC,     // 28
        CCELLD,     // 29

        // some more new traffic cars
        MINIVANA,   // 30
        PICKUPA,    // 31
        TAXIA,      // 32
        SPORTSA,    // 33
        SPORTSB,    // 34
        SUVA,       // 35
        WAGONA,     // 36

        // some more new cars: nov 12, 2002
        HBIKE_V,    // 37
        BURNS_V,    // 38
        HONOR_V,    // 39

        CARMOR,     // 40
        CCURATOR,   // 41
        CHEARS,     // 42
        CKLIMO,     // 43
        CLIMO,      // 44
        CNERD,      // 45

        FRINK_V,    // 46
        
        // some more new cars: dec 18, 2002
        CMILK,      // 47
        CDONUT,     // 48
        BBMAN_V,    // 49
        BOOKB_V,    // 50
        CARHOM_V,   // 51
        ELECT_V,    // 52
        FONE_V,     // 53
        GRAMR_V,    // 54
        MOE_V,      // 55
        MRPLO_V,    // 56
        OTTO_V,     // 57
        PLOWK_V,    // 58
        SCORP_V,    // 59
        WILLI_V,    // 60
        
        // new traffic cars: Jan 11, 2002
        SEDANA,     // 61
        SEDANB,     // 62

        // new Chase cars: Jan 11, 2002
        CBLBART,    // 63
        CCUBE,      // 64
        CDUFF,      // 65
        CNONUP,     // 66

        // new Driver cars: Jan 11, 2002
        LISA_V,     // 67
        KRUST_V,    // 68
                
        // new Traffic cars: Jan 13, 2002
        COFFIN,     // 69
        HALLO,      // 70
        SHIP,       // 71
        WITCHCAR,   // 72

        // new Traffic husk: Feb 10, 2002
        HUSKA,      // 73

        // new Driver cars: Feb 11, 2002
        ATV_V,      // 74
        DUNE_V,     // 75
        HYPE_V,     // 76
        KNIGH_V,    // 77
        MONO_V,     // 78
        OBLIT_V,    // 79
        ROCKE_V,    // 80

        // new Traffic cars: Feb 24, 2002
        AMBUL,      // 81
        BURNSARM,   // 82
        FISHTRUC,   // 83
        GARBAGE,    // 84
        ICECREAM,   // 85
        ISTRUCK,    // 86
        NUCTRUCK,   // 87
        PIZZA,      // 88
        SCHOOLBU,   // 89
        VOTETRUC,   // 90

        // new traffic car, april 2 2003
        GLASTRUC,
        CFIRE_V,
        
        // new chase car, may 23, 2003 - good grief
        CBONE,      // 93
        REDBRICK,   // 94
        
        NUM_VEHICLES,   
        INVALID
    };





}

#endif //VEHICLE_ENUM