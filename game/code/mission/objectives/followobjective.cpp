//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        followobjective.cpp
//
// Description: Implement FollowObjective
//
// History:     10/06/2002 + Created -- NAME
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

#include <events/eventmanager.h>

#include <meta/locator.h>

#include <mission/objectives/followobjective.h>
#include <mission/animatedicon.h>
#include <mission/gameplaymanager.h>

#include <memory/srrmemory.h>

#include <worldsim/redbrick/vehicle.h>
#include <worldsim/vehiclecentral.h>

#include <presentation/gui/guisystem.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>

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
// FollowObjective::FollowObjective
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
FollowObjective::FollowObjective() :
    mFollowVehicle( NULL ),
    mAnimatedIcon( NULL )
    //mDestinationLocator( NULL )
{
    
}

//==============================================================================
// FollowObjective::~FollowObjective
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
FollowObjective::~FollowObjective()
{
    
}

//=============================================================================
// FollowObjective::OnInitalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FollowObjective::OnInitialize()
{
MEMTRACK_PUSH_GROUP( "Mission - FollowObjective" );
//    rAssert( mDestinationLocator != NULL );
//    mDestinationLocator->SetFlag( Locator::ACTIVE, true );

    GetEventManager()->AddListener( this, EVENT_WAYAI_HIT_LAST_WAYPOINT );

    //========================= SET UP THE ICON

    rAssert( mAnimatedIcon == NULL );

    GameMemoryAllocator gma = GetGameplayManager()->GetCurrentMissionHeap();
    mAnimatedIcon = new(gma) AnimatedIcon();
 
    //TODO put in the actual name...
    const rmt::Vector pos = mFollowVehicle->GetPosition();
    mAnimatedIcon->Init( ARROW_EVADE, pos );
    mAnimatedIcon->ScaleByCameraDistance( MIN_ARROW_SCALE, MAX_ARROW_SCALE, MIN_ARROW_SCALE_DIST, MAX_ARROW_SCALE_DIST );

    //Set this vehicle as the focus of the HUD.
    rAssert( mFollowVehicle );
    VehicleAI* vehicleAI = GetVehicleCentral()->GetVehicleAI( mFollowVehicle );
    rAssert( vehicleAI );

    int hudID = vehicleAI->GetHUDIndex();
    if ( GetCurrentHud() ) 
    {
        GetCurrentHud()->GetHudMap( 0 )->SetFocalPointIcon( hudID );
    }

    mFollowVehicle->SetVehicleCanSustainDamage(false);

MEMTRACK_POP_GROUP("Mission - FollowObjective");
}

//=============================================================================
// FollowObjective::OnFinalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FollowObjective::OnFinalize()
{
//    rAssert( mDestinationLocator != NULL );
//    mDestinationLocator->SetFlag( Locator::ACTIVE, false );
    mFollowVehicle->SetVehicleCanSustainDamage(true);

    GetEventManager()->RemoveListener( this, EVENT_WAYAI_HIT_LAST_WAYPOINT );

    rAssert( mAnimatedIcon );

    if ( mAnimatedIcon )
    {
        delete mAnimatedIcon;
        mAnimatedIcon = NULL;
    }
}

//=============================================================================
// FollowObjective::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void FollowObjective::OnUpdate( unsigned int elapsedTime )
{
    //Update the position of the bv...
    rmt::Vector carPos;
    mFollowVehicle->GetPosition( &carPos );

    rmt::Box3D bbox;
    mFollowVehicle->GetBoundingBox( &bbox );
    carPos.y = bbox.high.y;

    mAnimatedIcon->Move( carPos );
    mAnimatedIcon->Update( elapsedTime );
}


//=============================================================================
// FollowObjective::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void FollowObjective::HandleEvent( EventEnum id, void* pEventData )
{
    switch( id )
    {
/*
    case EVENT_LOCATOR + LocatorEvent::CHECK_POINT:
        {
            Locator* locator = static_cast<Locator*>( pEventData );

            if( locator == mDestinationLocator )
            {
                SetFinished( true );
            }

            break;
        }
*/
    case EVENT_WAYAI_HIT_LAST_WAYPOINT:
        {
            if( pEventData == (void*)mFollowVehicle )
            {
                SetFinished( true );
            }
        }
    default:
        {
            break;
        }
    }
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
