// SectorTable.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <tlentity.hpp>

int _tmain(int argc, _TCHAR* argv[])
{
    const int NameCount = 75;
    char* sectorNames[ NameCount ] = {
        "l1r1.p3d", "l1r2.p3d", "l1r3.p3d", "l1r4a.p3d", "l1r4b.p3d", "l1r6.p3d", "l1r7.p3d",
        "l1z1.p3d", "l1z2.p3d", "l1z3.p3d", "l1z4.p3d", "l1z6.p3d", "l1z7.p3d",
        "l2r1.p3d", "l2r2.p3d", "l2r3.p3d", "l2r4.p3d",
        "l2z1.p3d", "l2z2.p3d", "l2z3.p3d", "l2z4.p3d",
        "l3r1.p3d", "l3r2.p3d", "l3r3.p3d", "l3r4.p3d", "l3r5.p3d",
        "l3z1.p3d", "l3z2.p3d", "l3z3.p3d", "l3z4.p3d", "l3z5.p3d",
        "l4r1.p3d", "l4r2.p3d", "l4r3.p3d", "l4r4a.p3d", "l4r4b.p3d", "l4r6.p3d", "l4r7.p3d",
        "l4z1.p3d", "l4z2.p3d", "l4z3.p3d", "l4z4.p3d", "l4z6.p3d", "l4z7.p3d",
        "l5r1.p3d", "l5r2.p3d", "l5r3.p3d", "l5r4.p3d",
        "l5z1.p3d", "l5z2.p3d", "l5z3.p3d", "l5z4.p3d",
        "l6r1.p3d", "l6r2.p3d", "l6r3.p3d", "l6r4.p3d", "l6r5.p3d",
        "l6z1.p3d", "l6z2.p3d", "l6z3.p3d", "l6z4.p3d", "l6z5.p3d",
        "l7r1.p3d", "l7r2.p3d", "l7r3.p3d", "l7r4a.p3d", "l7r4b.p3d", "l7r6.p3d", "l7r7.p3d",
        "l7z1.p3d", "l7z2.p3d", "l7z3.p3d", "l7z4.p3d", "l7z6.p3d", "l7z7.p3d" };

    char* typeStr[ 4 ] = { "unsigned char", "unsigned short", "int", "TLUID" };
    TLUID sectorIDs[ NameCount ];
    for( int i = 0; i < NameCount; ++i )
    {
        sectorIDs[ i ] = tlEntity::MakeUID( sectorNames[ i ] );
    }

    // Check for dups in the lower order bits.
    bool noDups = true;
    int type = 0;
    int mask = 0xFF;
    for( int i = 0; ( i < NameCount - 1 ) && ( noDups ); ++i )
    {
        char first = (char)( sectorIDs[ i ] & mask );
        for( int j = i + 1; j < NameCount; ++j )
        {
            char second = (char)( sectorIDs[ j ] & mask );
            if( first == second )
            {
                noDups = false;
                break;
            }
        }
    }
    if( !noDups )
    {
        ++type;
        noDups = true;
        mask = 0xFFFF;
        for( int i = 0; ( i < NameCount - 1 ) && ( noDups ); ++i )
        {
            short first = (short)( sectorIDs[ i ] & mask );
            for( int j = i + 1; j < NameCount; ++j )
            {
                short second = (short)( sectorIDs[ j ] & mask );
                if( first == second )
                {
                    noDups = false;
                    break;
                }
            }
        }
    }
    if( !noDups )
    {
        ++type;
        noDups = true;
        mask = 0xFFFFFFFF;
        for( int i = 0; ( i < NameCount - 1 ) && ( noDups ); ++i )
        {
            int first = (int)( sectorIDs[ i ] & mask );
            for( int j = i + 1; j < NameCount; ++j )
            {
                int second = (int)( sectorIDs[ j ] & mask );
                if( first == second )
                {
                    noDups = false;
                    break;
                }
            }
        }
    }
    if( !noDups )
    {
        ++type;
    }

    printf( "// Auto generated with SectorTable.\n" );
    printf( "struct SectorMap\n{\n    %s Sector;\n    char CurIndex;\n};\n\n", typeStr[ type ] );
    printf( "static const int NUM_SECTORS = %d;\n\n", NameCount );
    printf( "SectorMap sSectorMap[ NUM_SECTORS ] = {\n" );
    for( int i = 0; i < NameCount; ++i )
    {
        TLUID id = tlEntity::MakeUID( sectorNames[ i ] );
        unsigned hi = (unsigned)( id >> 32 );
        unsigned lo = (unsigned)( id & 0xFFFFFFFF );
        printf( "{ 0x" );
        switch( type )
        {
        case 0:
            printf("%02X", lo & 0xFF );
            break;
        case 1:
            printf("%04X", lo & 0xFFFF );
            break;
        case 2:
            printf("%08X", lo );
            break;
        default:
            printf( "%08X%08X", hi, lo );
            break;
        }
        printf( ", 0 }%s  //%s\n", i != NameCount -1 ? ",  " : " };", sectorNames[ i ] );
    }
	return 0;
}