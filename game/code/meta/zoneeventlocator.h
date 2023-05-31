//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        zoneeventlocator.h
//
// Description: Blahblahblah
//
// History:     18/06/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef ZONEEVENTLOCATOR_H
#define ZONEEVENTLOCATOR_H

//========================================
// Nested Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>

#include <meta/eventlocator.h>
#include <render/Culling/UseArray.h>
//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class ZoneEventLocator : public EventLocator
{
public:
    ZoneEventLocator();
    virtual ~ZoneEventLocator();

    virtual LocatorType::Type GetDataType() const;

    const char* const GetZone() const;
    void SetZone( const char* zone );
    void SetZoneSize( unsigned char size ); //This is to prevent fragmentation.

    const char* const GetLoadZone(int i);
    const char* const GetDumpZone(int i);
    const char* const GetInterLoadZone(int i);
    const char* const GetInterDumpZone(int i);
    const char* const GetLWSActivates(int i);
    const char* const GetLWSDeactivates(int i);

    int GetNumLoadZones();
    int GetNumDumpZones();
    int GetNumLWSActivates();
    int GetNumLWSDeactivates();

    bool IsInteriorLoad(void) { return mInteriorLoad; }
    bool IsInteriorDump(void) { return mInteriorDump; }
    const char* GetInteriorSection();

private:

    unsigned char mZoneSize;
    UseArray<int> mLoadZones;
    UseArray<int> mDumpZones;
    UseArray<int> mLWSActivates;
    UseArray<int> mLWSDeactivates;
    int mInteriorSection;

    bool mInteriorLoad;
    bool mInteriorDump;
    char* mZone;


    //Prevent wasteful constructor creation.
    ZoneEventLocator( const ZoneEventLocator& zoneeventlocator );
    ZoneEventLocator& operator=( const ZoneEventLocator& zoneeventlocator );
};

//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// ZoneEventLocator::GetDataType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
inline LocatorType::Type ZoneEventLocator::GetDataType() const
{
    return( LocatorType::DYNAMIC_ZONE );
}

#endif //ZONEEVENTLOCATOR_H
