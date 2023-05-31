//=============================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// File:        PersistentWorldManager.cpp
//
// Description: Implementation of class PersistenWorldManager
//
// History:
//  25 Feb 2003     James Harrison      Created for SRR2
//
//=============================================================================

//========================================
// System Includes
//========================================
#ifndef RAD_RELEASE
#include <raddebug.hpp>
#include <raddebugwatch.hpp>
#endif

//========================================
// Project Includes
//========================================
#include <data/persistentworldmanager.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <mission/charactersheet/charactersheet.h>
#include <mission/gameplaymanager.h>
#include <render/Enums/RenderEnums.h>
//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
PersistentWorldManager* PersistentWorldManager::spInstance = 0;

#include <data/PersistentSectors.cpp>

/*=============================================================================
Construct manager and allocate arrays. Makes sure you have an appropriate
memory pushed before you get here. The state information isn't that big though.
=============================================================================*/
PersistentWorldManager::PersistentWorldManager() :
    mCurSectorLoad( 0 )
{}

/*=============================================================================
All the memory for the world state is freed.
=============================================================================*/
PersistentWorldManager::~PersistentWorldManager()
{}

/*=============================================================================
Ye'olde standard methods for creating, getting, and destroying the singleton.
This should be the only way for outsiders to get at the manager.
=============================================================================*/
PersistentWorldManager* PersistentWorldManager::CreateInstance( void )
{
	rAssertMsg( spInstance == 0, "PersistentWorldManager already created.\n" );
	spInstance = new PersistentWorldManager;
    rAssert( spInstance );
    return PersistentWorldManager::GetInstance();
}

PersistentWorldManager* PersistentWorldManager::GetInstance( void )
{
	rAssertMsg( spInstance != 0, "PersistentWorldManager has not been created yet.\n" );
	return spInstance;
}

void PersistentWorldManager::DestroyInstance( void )
{
	rAssertMsg( spInstance != 0, "PersistentWorldManager has not been created.\n" );
	delete spInstance;
    spInstance = 0;
}

/*=============================================================================
Call this when you begin loading a dynamic zone/rail (i.e. a sector). This is
very important because to save space the manager assumes that objects are
always loaded in the same order after this call.
=============================================================================*/
void PersistentWorldManager::OnSectorLoad( tUID Sector )
{
    int i = 0;
    for( ; i < NUM_SECTORS; ++i )
    {
        if( sSectorMap[ i ].Sector == (unsigned short)( Sector & static_cast< tUID >( 0xFFFF ) ) )
        {
            break;
        }
    }
    if( i >= NUM_SECTORS )
    {
        // this isn't a sector we are concerned with.
        return;
    }
    sSectorMap[ i ].CurIndex = 0;
    mCurSectorLoad = i;
}

void PersistentWorldManager::OnLevelLoad( int LevelNum )
{
    int i = NUM_PERSISTENT_SECTORS - ( (int)RenderEnums::numLevels - LevelNum );

    rTuneAssert( i >= 0 && i < NUM_SECTORS );
    sSectorMap[ i ].CurIndex = 0;
}

/*=============================================================================
Call this to get a unique ID for a persistent object. Pass the ID back to the
manager when the object breaks. Note that the object name isn't used right now,
the manager assumes the objects are always loaded (i.e. this method gets called)
in the same order.
If you get back a -1 we don't track objects in that sector. If you get back a
-2 then we do track the object but it's already been busted.
=============================================================================*/
short PersistentWorldManager::GetPersistentObjectID( tUID Sector, tUID ObjectName )
{
    unsigned char sectorIndex = mCurSectorLoad;
    short rv = -1;
    int i = 0;
    for( ; i < NUM_SECTORS; ++i )
    {
        if( sSectorMap[ sectorIndex ].Sector == (unsigned short)( Sector & static_cast< tUID >( 0xFFFF ) ) )
        {
            mCurSectorLoad = sectorIndex;
            if( CheckObject( sectorIndex, sSectorMap[ sectorIndex ].CurIndex ) )
            {
                rAssert( static_cast<unsigned char>( sSectorMap[sectorIndex].CurIndex ) < NUM_PERSISTENT_OBJECTS ); // Out of bits for this zone/rail.
                rv = ( sectorIndex << 8 ) | sSectorMap[ sectorIndex ].CurIndex;
            }
            else
            {
                rv = -2;
            }
            ++(sSectorMap[ sectorIndex ].CurIndex);
            return rv;
        }
        sectorIndex = ( sectorIndex + 1 ) % NUM_SECTORS;
    }
    rAssert(i < NUM_SECTORS); // Oh-oh. We requested an ID for an object in a sector we aren't tracking.
    return rv;
}

/*=============================================================================
This call is for objects which you don't want to associate with a particular
sector. Basically there is an extra sector for each level which is for then
global. Needless to say you can't put too many objects in there.
=============================================================================*/
short PersistentWorldManager::GetPersistentObjectID( void )
{
    // Figure out a sector index by going to the last sectors in the array.
    RenderEnums::LevelEnum levelEnum = GetGameplayManager()->GetCurrentLevelIndex();
    int sectorIndex = NUM_PERSISTENT_SECTORS - ( RenderEnums::numLevels - levelEnum );
    short rv = -1;
    if( CheckObject( sectorIndex, sSectorMap[ sectorIndex ].CurIndex ) )
    {
        rv = ( sectorIndex << 8 ) | sSectorMap[ sectorIndex ].CurIndex;
    }
    else
    {
        rv = -2;
    }
    ++(sSectorMap[ sectorIndex ].CurIndex);
    return rv;
}

/*=============================================================================
Call this when you want to flag an object as gone. Pass back the ID you got
with the call to GetPersistentObjectID() method.
=============================================================================*/
void PersistentWorldManager::OnObjectBreak( short ObjectID )
{
    if( ObjectID < 0 )
    {
        return;
    }
    unsigned char sectorIndex = ( ObjectID >> 8 );
    rAssert( sectorIndex < NUM_PERSISTENT_SECTORS );
    unsigned char objectIndex = ( ObjectID & 0xFF );
    rAssert( objectIndex < NUM_PERSISTENT_OBJECTS );
    unsigned char bitMask = ~( 1 << ( objectIndex & 0x7 ) );
    int stateIndex = ( sectorIndex * NUM_BYTES_PER_SECTOR ) + ( objectIndex >> 3 );
    unsigned char* objectStates = GetCharacterSheetManager()->GetPersistentObjectStates();
    objectStates[ stateIndex ] &= bitMask;
}

/*=============================================================================
Check if an object is gone. Just checks the array to see if the bit is cleared.
=============================================================================*/
bool PersistentWorldManager::CheckObject( unsigned char SectorIndex, unsigned char ObjectIndex ) const
{
    rAssert( SectorIndex < NUM_PERSISTENT_SECTORS );
    rAssert( ObjectIndex < NUM_PERSISTENT_OBJECTS );
    unsigned char bitMask = 1 << ( ObjectIndex & 0x7 );
    short stateIndex = ( SectorIndex * NUM_BYTES_PER_SECTOR ) + ( ObjectIndex >> 3 );
    unsigned char* objectStates = GetCharacterSheetManager()->GetPersistentObjectStates();
    return ( objectStates[ stateIndex ] & bitMask ) != 0;
}