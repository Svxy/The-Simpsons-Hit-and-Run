//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        spawnmanager.cpp
//
// Description: SpawnManager Class Implementation
//
// History:     11/5/2002 + Created -- Dusit Eakkachaichanvet
//
//=============================================================================

#include <worldsim/spawn/spawnmanager.h>

void SpawnManager::Init()
{
    //////////////////////////////////////////////////////////////////
    // Sanity check subclass implementations
    rAssert( GetAbsoluteMaxObjects() >= 0 );
    rAssert( GetMaxModels() >= 0 );
    rAssert( GetSecondsBetwAdds() >= 0.0f );
    rAssert( GetSecondsBetwRemoves() >= 0.0f );
    rAssert( GetSecondsBetwUpdates() >= 0.0f );

    mSecondsSinceLastAdd = 0.0f;
    mSecondsSinceLastRemove = 0.0f;
    mSecondsSinceLastUpdate = 0.0f;

    mAddEnabled = true;
    mRemoveEnabled = true;
    mUpdateEnabled = true;
}


void SpawnManager::Update( float seconds )
{
    rAssert( seconds >= 0.0f );

    if( !IsActive() )
    {
        return;
    }

    ///////////////////////////////////////////////////////
    // Do some local sanity checks
    rAssert( mSpawnRadius >= 0.0f );
    rAssert( mRemoveRadius >= 0.0f );

    ///////////////////////////////////////////////////////
    // Do some enforcement of subclass implementations
    rAssert( 0 <= mNumObjects && mNumObjects <= GetAbsoluteMaxObjects() );
    rAssert( 0 <= GetMaxObjects() && GetMaxObjects() <= GetAbsoluteMaxObjects() );
    rAssert( 0 < GetNumRegisteredModels() && GetNumRegisteredModels() <= GetMaxModels() );

    ///////////////////////////////////////////////////////
    // Do things in this order:
    //   - Remove everything that's outside the radius
    //   - Add at the fringes of the radius
    //   - Update as necessary.

    /////////////////
    // REMOVE PHASE
    float secondsBetwRemoves = GetSecondsBetwRemoves();
    rAssert( secondsBetwRemoves >= 0.0f );
    if( mRemoveEnabled && mSecondsSinceLastRemove >= secondsBetwRemoves )
    {
        mSecondsSinceLastRemove = 0.0f;
        RemoveObjects( seconds );
    }
    else
    {
        mSecondsSinceLastRemove += seconds;
    }
    rAssert( 0 <= mNumObjects && mNumObjects <= GetAbsoluteMaxObjects() );
    /////////////////

    ////////////////
    // ADD PHASE
    float secondsBetwAdds = GetSecondsBetwAdds();
    rAssert( secondsBetwAdds >= 0.0f );
    if( mAddEnabled && mSecondsSinceLastAdd >= secondsBetwAdds )
    {
        mSecondsSinceLastAdd = 0.0f;
        AddObjects( seconds );
    }
    else
    {
        mSecondsSinceLastAdd += seconds;
    }
    rAssert( 0 <= mNumObjects && mNumObjects <= GetAbsoluteMaxObjects() );
    ////////////////

    ////////////////
    // UPDATE PHASE
    float secondsBetwUpdates = GetSecondsBetwUpdates();
    rAssert( secondsBetwUpdates >= 0.0f );
    if( mUpdateEnabled && mSecondsSinceLastUpdate >= secondsBetwUpdates )
    {
        mSecondsSinceLastUpdate = 0.0f;
        UpdateObjects( seconds );
    }
    else
    {
        mSecondsSinceLastUpdate += seconds;
    }
    rAssert( (0 <= mNumObjects) && (mNumObjects <= GetAbsoluteMaxObjects() ) );
    ////////////////
}
