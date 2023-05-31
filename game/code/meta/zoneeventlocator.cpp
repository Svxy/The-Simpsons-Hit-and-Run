//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        ZoneEventLocator.cpp
//
// Description: Implement ZoneEventLocator
//
// History:     18/06/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <meta/ZoneEventLocator.h>
#include <meta/locatorevents.h>
#include <memory/srrmemory.h>
#include <events/eventmanager.h>

#include <string.h>


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// ZoneEventLocator::ZoneEventLocator
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
ZoneEventLocator::ZoneEventLocator() :
    mZoneSize( 0 ),
    mInteriorSection( 0 ),
    mInteriorLoad(false),
    mInteriorDump(false),
    mZone( NULL )
{
    SetEventType( LocatorEvent::DYNAMIC_ZONE );
}

//==============================================================================
// ZoneEventLocator::~ZoneEventLocator
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
ZoneEventLocator::~ZoneEventLocator()
{
    if ( mZone )
    {
        delete[] mZone;
        mZone = NULL;
        mZoneSize = 0;
    }
}

//=============================================================================
// ZoneEventLocator::GetZone
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const 
//
//=============================================================================
const char* const ZoneEventLocator::GetZone() const
{
    return mZone;
}
//=============================================================================
// ZoneEventLocator::GetZone
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const 
//
//=============================================================================
int ZoneEventLocator::GetNumLoadZones()
{
    return mLoadZones.mUseSize;
}
//=============================================================================
// ZoneEventLocator::GetZone
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const 
//
//=============================================================================
int ZoneEventLocator::GetNumDumpZones()
{
    return mDumpZones.mUseSize;
}

//=============================================================================
// ZoneEventLocator::GetZone
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const 
//
//=============================================================================
int ZoneEventLocator::GetNumLWSActivates()
{
    return mLWSActivates.mUseSize;
}
//=============================================================================
// ZoneEventLocator::GetZone
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const 
//
//=============================================================================
int ZoneEventLocator::GetNumLWSDeactivates()
{
    return mLWSDeactivates.mUseSize;
}
//=============================================================================
// ZoneEventLocator::GetZone
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const 
//
//=============================================================================
const char* const ZoneEventLocator::GetLoadZone(int i) 
{
    return &(mZone[mLoadZones[i]]);
}
//=============================================================================
// ZoneEventLocator::GetZone
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const 
//
//=============================================================================
const char* const ZoneEventLocator::GetDumpZone(int i) 
{
    return &(mZone[mDumpZones[i]]);
}
//=============================================================================
// ZoneEventLocator::GetZone
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const 
//
//=============================================================================
const char* const ZoneEventLocator::GetLWSActivates(int i) 
{
    return &(mZone[mLWSActivates[i]]);
}
//=============================================================================
// ZoneEventLocator::GetZone
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const 
//
//=============================================================================
const char* const ZoneEventLocator::GetLWSDeactivates(int i) 
{
    return &(mZone[mLWSDeactivates[i]]);
}

//=============================================================================
// ZoneEventLocator::GetInteriorSection
//=============================================================================
const char* ZoneEventLocator::GetInteriorSection()
{
    return &(mZone[mInteriorSection]);
}

//=============================================================================
// ZoneEventLocator::SetZone
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* zone )
//
// Return:      void 
//
//=============================================================================
void ZoneEventLocator::SetZone( const char* zone )
{
    int zoneStrLength;

    rAssert( mZoneSize );
    rAssert( strlen(zone) + 1 <= mZoneSize );

    zoneStrLength = strlen(zone);

    strncpy( mZone, zone, zoneStrLength );
    mZone[zoneStrLength] = '\0';

    mLoadZones.Allocate(mZoneSize/6);
    mDumpZones.Allocate(mZoneSize/6);
    
    mLWSDeactivates.Allocate(mZoneSize/6);
    mLWSActivates.Allocate(mZoneSize/6);

    int i, startPosn;
    for(startPosn=0,i=0; i<zoneStrLength; i++)
    {
        switch(mZone[i])
        {
        case ';':
            mLoadZones.Add(startPosn);
            mZone[i]='\0';
            i++;
            startPosn = i;
            break;
        case ':':
            mDumpZones.Add(startPosn);
            mZone[i]='\0';
            i++;
            startPosn = i;
            break;
        case '@':
            mInteriorLoad = true;
            mInteriorSection = startPosn;
            mLoadZones.Add(startPosn);
            mZone[i]='\0';
            i++;
            startPosn = i;
            break;
        case '$':
            mInteriorDump = true;
            mDumpZones.Add(startPosn);
            mZone[i]='\0';
            i++;
            startPosn = i;
            break;
        case '&':
            mLWSDeactivates.Add(startPosn);
            mZone[i]='\0';
            i++;
            startPosn = i;
            break;
        case '*':
            mLWSActivates.Add(startPosn);
            mZone[i]='\0';
            i++;
            startPosn = i;
            break;
        case '\0':
            break;
        default:
            break;
        }
    }
}

//=============================================================================
// ZoneEventLocator::SetZoneSize
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned char size )
//
// Return:      void 
//
//=============================================================================
void ZoneEventLocator::SetZoneSize( unsigned char size )
{
    MEMTRACK_PUSH_GROUP( "ZoneEventLocator" );
    rAssert( !mZoneSize );
    
    if ( mZone )
    {
        rAssertMsg( false, "Why is someone changing this!  BAD! Get Cary!" );
        mZoneSize = 0;
        delete[] mZone;
        mZone = NULL;
    }
    else
    {
        mZoneSize = size + 1;
    }

    mZone = new(GMA_LEVEL_OTHER) char[ mZoneSize ];        
    MEMTRACK_POP_GROUP( "ZoneEventLocator" );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
