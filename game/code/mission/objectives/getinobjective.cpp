//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        GetInObjective.cpp
//
// Description: Implement GetInObjective
//
// History:     09/09/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <p3d/entity.hpp>

//========================================
// Project Includes
//========================================
#include <mission/objectives/GetInObjective.h>
#include <mission/animatedicon.h>
#include <mission/gameplaymanager.h>

#include <worldsim/vehiclecentral.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/character/character.h>
#include <worldsim/character/charactermanager.h>

#include <worldsim/avatarmanager.h>

#include <ai/state.h>
#include <memory/srrmemory.h>


//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// GetInObjective::GetInObjective
//=============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
GetInObjective::GetInObjective() :
    mHUDID( -1 ),
    mAnimatedIcon( NULL ),
    mStrict(false),
    mVehicleUID(0)
{
}

//=============================================================================
// GetInObjective::~GetInObjective
//=============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
GetInObjective::~GetInObjective()
{
}

//=============================================================================
// GetInObjective::OnInitalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GetInObjective::OnInitialize()
{
MEMTRACK_PUSH_GROUP( "Mission - GetInObjective" );

    GameplayManager* gpm = GetGameplayManager();

    // TC: No need to do this anymore since phone booths are now disabled
    //     all thoughout missionmode.
    //
//    gpm->DisablePhoneBooths();

    Vehicle* vehicle = NULL;
    if(mStrict)
    {
        vehicle = GetVehicleCentral()->GetVehicleByUID(mVehicleUID);
    }
    else
    {
        vehicle = gpm->GetCurrentVehicle();
    }

    rAssert(vehicle);
    if(!vehicle)
    {
        //fail
        return;
    }

    rmt::Vector pos = vehicle->GetPosition();

//    RegisterPosition( pos, mHUDID, true, HudMapIcon::ICON_PLAYER_CAR, this );

    //========================= SET UP THE ICON

    rAssert( mAnimatedIcon == NULL );

    GameMemoryAllocator gma = gpm->GetCurrentMissionHeap();
    mAnimatedIcon = new(gma) AnimatedIcon();
 
    //TODO put in the actual name...
    mAnimatedIcon->Init( ARROW, pos );
    mAnimatedIcon->ScaleByCameraDistance( MIN_ARROW_SCALE, MAX_ARROW_SCALE, MIN_ARROW_SCALE_DIST, MAX_ARROW_SCALE_DIST );

MEMTRACK_POP_GROUP("Mission - GetInObjective");
}

//=============================================================================
// GetInObjective::OnFinalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GetInObjective::OnFinalize()
{
//    UnregisterPosition( mHUDID );

    rAssert( mAnimatedIcon );



    if ( mAnimatedIcon )
    {
        delete mAnimatedIcon;
        mAnimatedIcon = NULL;
    }

    // TC: No need to do this anymore since phone booths are now disabled
    //     all thoughout missionmode.
    //
//    GetGameplayManager()->EnablePhoneBooths();
}

//=============================================================================
// GetInObjective::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void GetInObjective::OnUpdate( unsigned int elapsedTime )
{
    Vehicle* vehicle = NULL;
    if(mStrict)
    {
        vehicle = GetVehicleCentral()->GetVehicleByUID(mVehicleUID);
    }
    else
    {
        vehicle = GetGameplayManager()->GetCurrentVehicle();
    }

    rAssert(vehicle);
    if(!vehicle)
    {
        //fail???
        return;
    }

    //Update the position of the bv...
    rmt::Vector carPos;
    vehicle->GetPosition( &carPos );

    rmt::Box3D bbox;
    vehicle->GetBoundingBox( &bbox );
    carPos.y = bbox.high.y;

    mAnimatedIcon->Move( carPos );
    mAnimatedIcon->Update( elapsedTime );

    if(mStrict)
    {
        if ( GetAvatarManager()->GetAvatarForPlayer( 0 )->GetVehicle() == vehicle  &&
            GetCharacterManager()->GetCharacter( 0 )->GetStateManager()->GetState() == CharacterAi::INCAR )
        {
            SetFinished( true );
        }
    }
    else
    {
        if ( GetAvatarManager()->GetAvatarForPlayer( 0 )->GetVehicle() &&
            GetCharacterManager()->GetCharacter( 0 )->GetStateManager()->GetState() == CharacterAi::INCAR )
        {
            SetFinished( true );
        }
    }
}

void GetInObjective::SetStrict( const char* name )
{
    mVehicleUID = tEntity::MakeUID(name);
    Vehicle* vehicle = GetVehicleCentral()->GetVehicleByUID(mVehicleUID);
    if(vehicle)
    {
        mStrict = true;
    }
    else
    {
        mStrict = false;
        mVehicleUID = 0;
    }
}

//=============================================================================
// GetInObjective::GetPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* currentLoc )
//
// Return:      void 
//
//=============================================================================
void GetInObjective::GetPosition( rmt::Vector* currentLoc )
{
    Vehicle* vehicle = NULL;
    if(mStrict)
    {
        vehicle = GetVehicleCentral()->GetVehicleByUID(mVehicleUID);
    }
    else
    {
        vehicle = GetGameplayManager()->GetCurrentVehicle();
    }
    rAssert( vehicle );
    if(!vehicle)
    {
        return;
    }

    vehicle->GetPosition( currentLoc );
}

//=============================================================================
// GetInObjective::GetHeading
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* heading )
//
// Return:      void 
//
//=============================================================================
void GetInObjective::GetHeading( rmt::Vector* heading )
{
    Vehicle* vehicle = NULL;
    if(mStrict)
    {
        vehicle = GetVehicleCentral()->GetVehicleByUID(mVehicleUID);
    }
    else
    {
        vehicle = GetGameplayManager()->GetCurrentVehicle();
    }
    rAssert( vehicle );
    if(!vehicle)
    {
        return;
    }

    vehicle->GetHeading( heading );
}


//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************
