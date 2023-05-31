//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        destroyobjective.cpp
//
// Description: Implement DestroyObjective
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

#include <mission/objectives/destroyobjective.h>
#include <mission/animatedicon.h>
#include <mission/gameplaymanager.h>

#include <presentation/gui/guisystem.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>

#include <events/eventmanager.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/hitnrunmanager.h>

#include <memory/srrmemory.h>

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
// DestroyObjective::DestroyObjective
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
DestroyObjective::DestroyObjective() :
    mDestroyVehicle( NULL ),
    mAnimatedIcon( NULL )
{
    
}

//==============================================================================
// DestroyObjective::~DestroyObjective
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
DestroyObjective::~DestroyObjective()
{
    
}

//=============================================================================
// DestroyObjective::OnInitalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void DestroyObjective::OnInitialize()
{
MEMTRACK_PUSH_GROUP( "Mission - DestroyObjective" );
    GetEventManager()->AddListener( this, EVENT_VEHICLE_DAMAGED );
    GetEventManager()->AddListener( this, EVENT_VEHICLE_DESTROYED );

    // show damage meter
    GetGuiSystem()->HandleMessage( GUI_MSG_SHOW_HUD_OVERLAY, HUD_DAMAGE_METER );

    //Set this vehicle as the focus of the HUD.
    rAssert( mDestroyVehicle );
    VehicleAI* vehicleAI = GetVehicleCentral()->GetVehicleAI( mDestroyVehicle );
    rAssert( vehicleAI );

    GetHitnRunManager()->RegisterVehicleImmunity( mDestroyVehicle );

    CGuiScreenHud* currentHud = GetCurrentHud();
    if( currentHud != NULL )
    {
        currentHud->GetHudMap( 0 )->SetFocalPointIcon( vehicleAI->GetHUDIndex() );

        // update damage meter
        currentHud->SetDamageMeter( 0.0f ); // between 0.0 and 1.0
    }

    //========================= SET UP THE ICON

    rAssert( mAnimatedIcon == NULL );

    GameMemoryAllocator gma = GetGameplayManager()->GetCurrentMissionHeap();
    mAnimatedIcon = new(gma) AnimatedIcon();
 
    const rmt::Vector pos = mDestroyVehicle->GetPosition();
    //TODO put in the actual name...
    mAnimatedIcon->Init( ARROW_DESTROY, pos );
    mAnimatedIcon->ScaleByCameraDistance( MIN_ARROW_SCALE, MAX_ARROW_SCALE, MIN_ARROW_SCALE_DIST, MAX_ARROW_SCALE_DIST );
MEMTRACK_POP_GROUP("Mission - DestroyObjective");
}

//=============================================================================
// DestroyObjective::OnFinalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void DestroyObjective::OnFinalize()
{
    // hide damage meter
    GetGuiSystem()->HandleMessage( GUI_MSG_HIDE_HUD_OVERLAY, HUD_DAMAGE_METER );

    GetEventManager()->RemoveListener( this, EVENT_VEHICLE_DAMAGED );
    GetEventManager()->RemoveListener( this, EVENT_VEHICLE_DESTROYED );

    GetHitnRunManager()->RegisterVehicleImmunity( NULL );

    rAssert( mAnimatedIcon );

    if ( mAnimatedIcon )
    {
        delete mAnimatedIcon;
        mAnimatedIcon = NULL;
    }
}

//=============================================================================
// DestroyObjective::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void DestroyObjective::OnUpdate( unsigned int elapsedTime )
{
    //Update the position of the bv...
    rmt::Vector carPos;
    mDestroyVehicle->GetPosition( &carPos );

    rmt::Box3D bbox;
    mDestroyVehicle->GetBoundingBox( &bbox );
    carPos.y = bbox.high.y;

    mAnimatedIcon->Move( carPos );
    mAnimatedIcon->Update( elapsedTime );
}

//=============================================================================
// DestroyObjective::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void DestroyObjective::HandleEvent( EventEnum id, void* pEventData )
{
    switch( id )
    {
    case EVENT_VEHICLE_DAMAGED:
        {
            if( pEventData == (void*)mDestroyVehicle )
            {
                //Update the HUD.
                // update damage meter
                if ( GetCurrentHud() )
                {
                    GetCurrentHud()->SetDamageMeter( 1.0f - mDestroyVehicle->GetVehicleLifePercentage(mDestroyVehicle->mHitPoints) ); // between 0.0 and 1.0
                }
            }
            break;
        }
    case EVENT_VEHICLE_DESTROYED:
        {
            if( pEventData == (void*)mDestroyVehicle )
            {
                // update damage meter
                if ( GetCurrentHud() )
                {
                    GetCurrentHud()->SetDamageMeter( 1.0f ); // between 0.0 and 1.0
                }

                SetFinished( true );
            }
            break;
        }
    default:
        {
            break;
        }
    }
}

//=============================================================================
// DestroyObjective::SetTargetVehicle
//=============================================================================
// Description: Comment
//
// Parameters:  ( Vehicle* pVehicle )
//
// Return:      void 
//
//=============================================================================
void DestroyObjective::SetTargetVehicle( Vehicle* pVehicle )
{ 
    mDestroyVehicle = pVehicle; 
    //mDestroyVehicle->SetVehicleCanSustainDamage( true ); 
    mDestroyVehicle->VehicleIsADestroyObjective( true ); 
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
