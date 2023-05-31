//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement FollowCondition
//
// History:     04/07/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>

#include <radmath/radmath.hpp>

//========================================
// Project Includes
//========================================

#include <mission/conditions/followcondition.h>

#include <ai/vehicle/vehicleai.h>

#include <mission/gameplaymanager.h>

#include <roads/road.h>
#include <roads/roadmanager.h>

#include <worldsim/avatar.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/redbrick/vehicle.h>

#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>

#include <interiors/interiormanager.h>

#include <worldsim/hitnrunmanager.h>


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
// FollowCondition::FollowCondition
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
FollowCondition::FollowCondition() :
    mMinDistance( 0.0f ),
    mMaxDistance( 0.0f )
{
    this->SetType( COND_FOLLOW_DISTANCE );
}

//==============================================================================
// FollowCondition::~FollowCondition
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
FollowCondition::~FollowCondition()
{
}

//=============================================================================
// FollowCondition::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void FollowCondition::Update( unsigned int elapsedTime )
{
    rTuneAssertMsg( !rmt::Epsilon( mMaxDistance, 0.0f ), "FollowCondition must have a max distance greater than 0!\n" );

    muTime += elapsedTime;

    if( muTime > MIN_UPDATE_PERIOD )
    {
        muTime = 0;

        if ( !GetInteriorManager()->IsEntering() && 
             !GetInteriorManager()->IsExiting() &&
             !GetHitnRunManager()->BustingPlayer() ) 
        {
            float dist = CalculateDistanceToTarget();

            if( dist > mMaxDistance * mMaxDistance )
            {
                SetIsViolated( true );
            }
            else
            {
                CGuiScreenHud* pHUD = GetCurrentHud();
                if( pHUD != NULL )
                {
                    // update proximity meter on HUD
                    //
                    pHUD->SetProximityMeter( 1.0f - rmt::Sqrt( dist ) / mMaxDistance );

                    //                pHUD->GetHudMap( 0 )->UpdateAICarDistance( rmt::Sqrt(dist), mMaxDistance );
                }
            }
        }
    }
}

//=============================================================================
// FollowCondition::IsChaseCondition
//=============================================================================
// Description: Indicate that this condition involves chasing
//
// Parameters:  None
//
// Return:      True 
//
//=============================================================================
bool FollowCondition::IsChaseCondition()
{
    return( true );
}

//=============================================================================
// FollowCondition::IsClose
//=============================================================================
// Description: Indicates whether this condition is at all close to failure.
//              Very subjective, used for interactive music
//
// Parameters:  None
//
// Return:      True if close, false otherwise 
//
//=============================================================================
bool FollowCondition::IsClose()
{
    //
    // For now, assume that follow conditions are always tense, since it's
    // hard to say whether you're running away with this one
    //
    return( true );
}

//******************************************************************************
//
// Protected Member Functions
//
//******************************************************************************

//=============================================================================
// FollowCondition::OnInitialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FollowCondition::OnInitialize()
{
    //Set this vehicle as the focus of the HUD.
    rAssert( GetVehicle() );
    VehicleAI* vehicleAI = GetVehicleCentral()->GetVehicleAI( GetVehicle() );
    rAssert( vehicleAI );

    int hudID = vehicleAI->GetHUDIndex();
    if ( GetCurrentHud() )
    {
        GetCurrentHud()->GetHudMap( 0 )->SetFocalPointIcon( hudID );
    }
}

//=============================================================================
// FollowCondition::OnFinalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void FollowCondition::OnFinalize()
{
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// FollowCondition::CalculateDistanceToTarget
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
float FollowCondition::CalculateDistanceToTarget()
{
    //
    // TODO: Find the distance along the road?
    //
    rmt::Vector myPos;

    GetAvatarManager()->GetAvatarForPlayer( 0 )->GetPosition( myPos );

    Vehicle* target = GetVehicle();
    rAssert( target != NULL );

    rmt::Vector targetPos;
    target->GetPosition( &targetPos );

    targetPos.Sub( myPos );
    return targetPos.MagnitudeSqr();

}