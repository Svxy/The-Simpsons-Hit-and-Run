//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        locatorloader.h
//
// Description: Blahblahblah
//
// History:     24/05/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef LOCATORLOADER_H
#define LOCATORLOADER_H

//========================================
// Nested Includes
//========================================
#include <p3d/loadmanager.hpp>
#include <p3d/p3dtypes.hpp>

#include <constants/srrchunks.h>
#include <render/Loaders/IWrappedLoader.h>
//========================================
// Forward References
//========================================

class TriggerLocator;
class tEntity;
class RailCam;
class SplineLocator;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class LocatorLoader 
: public tSimpleChunkHandler,
  public IWrappedLoader 
{
public:
    LocatorLoader();
    virtual ~LocatorLoader();

    //////////////////////////////////////////////////////////////////////
    // IWrappedLoader
    //////////////////////////////////////////////////////////////////////
    void SetRegdListener( ChunkListenerCallback* pListenerCB,
                          int   iUserData );

    void ModRegdListener( ChunkListenerCallback* pListenerCB,
                          int   iUserData );

private:

    //////////////////////////////////////////////////////////////////////
    // IWrappedLoader
    //////////////////////////////////////////////////////////////////////

    //Prevent wasteful constructor creation.
    LocatorLoader( const LocatorLoader& locatorloader );
    LocatorLoader& operator=( const LocatorLoader& locatorloader );

    // P3D chunk loader.
    virtual tEntity* LoadObject(tChunkFile* f, tEntityStore* store);

    void LoadTriggerLocator( tChunkFile* f, 
                             tEntityStore* store, 
                             TriggerLocator* locator );
    bool LoadTriggerVolume( tChunkFile* f, 
                            TriggerLocator* locator,
                            bool addToTracker = true );
    RailCam* LoadSpline( tChunkFile* f, SplineLocator* splineLoc );

    static int msZoneNameCount;
};

//******************************************************************************
//
// Inline Public Member Functions
//
//******************************************************************************

#endif //LOCATORLOADER_H
