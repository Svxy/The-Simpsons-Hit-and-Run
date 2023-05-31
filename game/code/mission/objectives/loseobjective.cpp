//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement LoseObjective
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

#include <mission/objectives/loseobjective.h>
#include <mission/animatedicon.h>
#include <mission/gameplaymanager.h>

#include <presentation/gui/guisystem.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>

#include <worldsim/avatar.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/redbrick/vehicle.h>

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
// LoseObjective::LoseObjective
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
LoseObjective::LoseObjective() :
    mDistance( 0 ),
    mTargetVehicle( NULL ),
    mAnimatedIcon( NULL )
{
}

//==============================================================================
// LoseObjective::~LoseObjective
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
LoseObjective::~LoseObjective()
{
}

//=============================================================================
// LoseObjective::OnInitialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void LoseObjective::OnInitialize()
{
MEMTRACK_PUSH_GROUP( "Mission - LoseObjective" );
    GetGuiSystem()->HandleMessage( GUI_MSG_SHOW_HUD_OVERLAY, HUD_PROXIMITY_METER, 0 /* 0 = red bar */ );

    rAssert( mTargetVehicle );
    rmt::Vector pos = mTargetVehicle->GetPosition();

    //Set this vehicle as the focus of the HUD.
    VehicleAI* vehicleAI = GetVehicleCentral()->GetVehicleAI( mTargetVehicle );
    rAssert( vehicleAI != NULL );

    CGuiScreenHud* currentHud = GetCurrentHud();
    if( currentHud != NULL )
    {
        currentHud->GetHudMap( 0 )->SetFocalPointIcon( vehicleAI->GetHUDIndex() );
    }

    //========================= SET UP THE ICON

    rAssert( mAnimatedIcon == NULL );

    GameMemoryAllocator gma = GetGameplayManager()->GetCurrentMissionHeap();
    mAnimatedIcon = new(gma) AnimatedIcon();
 
    //TODO put in the actual name...
    mAnimatedIcon->Init( ARROW_CHASE, pos );
    mAnimatedIcon->ScaleByCameraDistance( MIN_ARROW_SCALE, MAX_ARROW_SCALE, MIN_ARROW_SCALE_DIST, MAX_ARROW_SCALE_DIST );

MEMTRACK_POP_GROUP("Mission - LoseObjective");
}

//=============================================================================
// LoseObjective::OnFinalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void LoseObjective::OnFinalize()
{
    GetGuiSystem()->HandleMessage( GUI_MSG_HIDE_HUD_OVERLAY, HUD_PROXIMITY_METER );

    rAssert( mAnimatedIcon );

    if ( mAnimatedIcon )
    {
        delete mAnimatedIcon;
        mAnimatedIcon = NULL;
    }
}

//=============================================================================
// LoseObjective::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void LoseObjective::OnUpdate( unsigned int elapsedTime )
{
    //
    // TODO: Find the distance along the road?
    //
    rmt::Vector myPos;

    GetAvatarManager()->GetAvatarForPlayer( 0 )->GetPosition( myPos );

    Vehicle* target = mTargetVehicle;
    rAssert( target != NULL );

    rmt::Vector targetPos;
    target->GetPosition( &targetPos );

    rmt::Vector arrowPos = targetPos;
    rmt::Box3D bbox;
    target->GetBoundingBox( &bbox );
    arrowPos.y = bbox.high.y;

    //Update the icon while the pos is good.
    mAnimatedIcon->Move( arrowPos );
    mAnimatedIcon->Update( elapsedTime );

    targetPos.Sub( myPos );
    float dist = targetPos.MagnitudeSqr();

    if( dist > (mDistance * mDistance) )
    {
        SetFinished( true );
    }
    else
    {
        // update proximity meter on HUD
        //
        CGuiScreenHud* pHUD = GetCurrentHud();
        if( pHUD != NULL )
        {
            pHUD->SetProximityMeter( 1.0f - rmt::Sqrt( dist ) / mDistance );
        }
    }
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
