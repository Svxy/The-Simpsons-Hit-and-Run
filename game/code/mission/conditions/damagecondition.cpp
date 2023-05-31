//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement DamageCondition
//
// History:     24/07/2002 + Created -- NAME
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
#include <mission/gameplaymanager.h>

#include <mission/conditions/damagecondition.h>

#include <worldsim/redbrick/vehicle.h>

const static float CLOSE_DAMAGE_THRESHOLD = 0.1f; // was .2

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
// DamageCondition::DamageCondition
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
DamageCondition::DamageCondition() :
    mMinValue( 0.0f ),
    mLastVehicleDamage( 1.0f ),
    mbCarDestroyed(false)
{
    this->SetType( COND_VEHICLE_DAMAGE );
}

//==============================================================================
// DamageCondition::~DamageCondition
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
DamageCondition::~DamageCondition()
{
}

//=============================================================================
// DamageCondition::OnInitialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void DamageCondition::OnInitialize()
{

    GetEventManager()->AddListener( this, EVENT_VEHICLE_DAMAGED );
    GetEventManager()->AddListener( this, EVENT_VEHICLE_DESTROYED );
    GetEventManager()->AddListener( this, EVENT_ENTERING_PLAYER_CAR );
    GetEventManager()->AddListener( this, EVENT_ENTERING_TRAFFIC_CAR );
    GetEventManager()->AddListener( this, EVENT_GETINTOVEHICLE_END );
    GetEventManager()->AddListener( this, EVENT_CAR_EXPLOSION_DONE );

    //chuck: update our vehicle pointer incase the user has switched cars
    if (GetGameplayManager()->GetCurrentVehicle() != NULL)
    {
        SetVehicle(GetGameplayManager()->GetCurrentVehicle());
    }


}

//=============================================================================
// DamageCondition::OnFinalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void DamageCondition::OnFinalize()
{
    GetEventManager()->RemoveAll(this);
}

//=============================================================================
// DamageCondition::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void DamageCondition::HandleEvent( EventEnum id, void* pEventData )
{
    switch( id )
    {

    case EVENT_VEHICLE_DESTROYED:
    case EVENT_VEHICLE_DAMAGED:
        {
            Vehicle* pVehicle =  reinterpret_cast<Vehicle*>(pEventData);

            if( pVehicle == GetVehicle())
            {
                mLastVehicleDamage = pVehicle->GetVehicleLifePercentage(pVehicle->mHitPoints);
                if( mLastVehicleDamage <= mMinValue )
                {
                    //flag that our car is destroyed and wait for explosion to play
                    mbCarDestroyed = true;
                }
            }
            break;
        }
    case EVENT_CAR_EXPLOSION_DONE:
        {
            if (mbCarDestroyed == true)
            {
                //explosion is done playing signal failure condition.
                SetIsViolated (true);
            }
            break;
        }

    case EVENT_ENTERING_PLAYER_CAR:
    case EVENT_ENTERING_TRAFFIC_CAR:
    case EVENT_GETINTOVEHICLE_END:
        //chuck: we should update our vehicle pointer incase user decides to switch cars during a this stage.
        {
            this->SetVehicle(GetGameplayManager()->GetCurrentVehicle());
            break;
        }

    default:
        {
            break;
        }
    }

    MissionCondition::HandleEvent( id, pEventData );
}

//=============================================================================
// DamageCondition::IsClose
//=============================================================================
// Description: Indicates whether this condition is at all close to failure.
//              Very subjective, used for interactive music
//
// Parameters:  None
//
// Return:      True if close, false otherwise 
//
//=============================================================================
bool DamageCondition::IsClose()
{
    //
    // Smashing stuff is exciting
    //
    return( true );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
