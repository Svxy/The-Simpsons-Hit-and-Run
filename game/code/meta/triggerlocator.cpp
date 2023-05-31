//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement TriggerLocator
//
// History:     04/04/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>

//========================================
// Project Includes
//========================================
#ifndef WORLD_BUILDER
#include <meta/TriggerLocator.h>
#include <meta/TriggerVolume.h>
#include <meta/triggervolumetracker.h>
#include <memory/srrmemory.h>
#else
#include "TriggerLocator.h"
#include "TriggerVolume.h"
#include "triggervolumetracker.h"
#define new(s) new
#endif

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
// TriggerLocator::TriggerLocator
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
TriggerLocator::TriggerLocator() :
    mTriggerVolumes( NULL ),
    mNumTriggers( 0 ),
    mMaxNumTriggers( 0 ),
    mPlayerEntered( false ),
    mPlayerID( -1 )
{
}

//==============================================================================
// TriggerLocator::~TriggerLocator
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
TriggerLocator::~TriggerLocator()
{
    unsigned int i;

    for ( i = 0; i < mMaxNumTriggers; ++i )
    {
        if ( mTriggerVolumes[ i ] != NULL )
        {
            //Chuck We should check if the Trig Track even exists
            // since it maybe destroyed when we are clearing the inventory of these things.
            if(GetTriggerVolumeTracker() != NULL)
            {
                GetTriggerVolumeTracker( )->RemoveTrigger( mTriggerVolumes[ i ] );
            }
            mTriggerVolumes[ i ]->Release( );
            mTriggerVolumes[ i ] = NULL;
        }
    }

    delete[] mTriggerVolumes;
    mTriggerVolumes = NULL;
}

//=============================================================================
// TriggerLocator::SetNumTriggers
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int num, int allocID )
//
// Return:      void 
//
//=============================================================================
void TriggerLocator::SetNumTriggers( unsigned int num, int allocID )
{
    MEMTRACK_PUSH_GROUP( "TriggerLocator" );
    rAssert( !mTriggerVolumes );

    #ifdef RAD_DEBUG
        if ( allocID == 0 )
        {
            rDebugString( "Someone is allocating trigger volume space in the DEFAULT heap!\n");
            rDebugString( "Pass the correct allocator id to SetNumTriggers please!!\n");
        }
    #endif


    mTriggerVolumes = new( (GameMemoryAllocator)allocID ) TriggerVolume*[num];
    mMaxNumTriggers = (short)num;
 
    unsigned short i;
    for ( i = 0; i < mMaxNumTriggers; ++i )
    {
        mTriggerVolumes[ i ] = NULL;
    }
    MEMTRACK_POP_GROUP( "TriggerLocator" );
}

//=============================================================================
// TriggerLocator::AddTriggerVolume
//=============================================================================
// Description: Comment
//
// Parameters:  ( TriggerVolume* volume )
//
// Return:      void 
//
//=============================================================================
void TriggerLocator::AddTriggerVolume( TriggerVolume* volume )
{
    rAssert( mNumTriggers < mMaxNumTriggers );
    rAssert( mTriggerVolumes );

    if ( volume )
    {
        volume->AddRef();

        mTriggerVolumes[ mNumTriggers ] = volume;
        mNumTriggers++;
    }
}

//=============================================================================
// TriggerLocator::GetTriggerVolume
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int i )
//
// Return:      TriggerVolume
//
//=============================================================================
TriggerVolume* TriggerLocator::GetTriggerVolume( unsigned int i )
{
    rAssert( i < mMaxNumTriggers );
    rAssert( mTriggerVolumes );

    return mTriggerVolumes[ i ];
}

//=============================================================================
// TriggerLocator::IsPlayerTracked
//=============================================================================
// Description: Comment
//
// Parameters:  ( int playerID )
//
// Return:      unsigned int 
//
//=============================================================================
unsigned int TriggerLocator::IsPlayerTracked( int playerID ) const
{
    unsigned int count = 0;
    unsigned int i;
    for ( i = 0; i < mNumTriggers; ++i )
    {
        if ( mTriggerVolumes[ i ] && mTriggerVolumes[ i ]->IsPlayerTracking( playerID ) )
        {
            ++count;
        }
    }

    return count;
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
