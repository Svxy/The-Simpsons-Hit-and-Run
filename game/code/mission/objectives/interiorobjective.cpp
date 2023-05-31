//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        InteriorObjective.cpp
//
// Description: Implement InteriorObjective
//
// History:     09/09/2002 + Created -- Cary Brisebois
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
#include <mission/objectives/InteriorObjective.h>
#include <mission/animatedicon.h>
#include <mission/gameplaymanager.h>

#include <worldsim/redbrick/vehicle.h>
#include <worldsim/character/character.h>
#include <events/eventmanager.h>

#include <interiors/interiormanager.h>

#include <meta/locator.h>

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
// InteriorObjective::InteriorObjective
//=============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
InteriorObjective::InteriorObjective() :
    mHUDID( -1 ),
    mAnimatedIcon( NULL )
{
    mArrowPath.mPathRoute.Allocate( RoadManager::GetInstance()->GetNumRoads() );    

    mIcon[0] = '\0';
}

//=============================================================================
// InteriorObjective::~InteriorObjective
//=============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
InteriorObjective::~InteriorObjective()
{
}

//=============================================================================
// InteriorObjective::OnInitalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void InteriorObjective::OnInitialize()
{
MEMTRACK_PUSH_GROUP( "Mission - InteriorObjective" );
    // if we are already inside the interior, need to trigger the objective
    if(GetInteriorManager()->IsInside() && (GetInteriorManager()->GetInterior() == tEntity::MakeUID(mDestination)))
    {
        SetFinished( true );

        // don't wan't to show stage complete for doing nothing
        GetGameplayManager()->GetCurrentMission()->GetCurrentStage()->ShowStageComplete(false);
        return;
    }

    GameplayManager* gpm = GetGameplayManager();

    GetEventManager()->AddListener( this, EVENT_ENTER_INTERIOR_TRANSITION_START );

    Locator* l = p3d::find<Locator>(mDestination);
    rAssert(l);

    l->GetPosition(&mPosition);

    RegisterPosition( mPosition, mHUDID, true, HudMapIcon::ICON_MISSION, this );

    //========================= SET UP THE ICON

    rAssert( mAnimatedIcon == NULL );

 
    //TODO put in the actual name...
    if ( mIcon[0] != '\0' )
    {
        GameMemoryAllocator gma = gpm->GetCurrentMissionHeap();
        mAnimatedIcon = new(gma) AnimatedIcon();
        mAnimatedIcon->Init( mIcon, rmt::Vector( 0.0f, 0.0f, 0.0f ) );  //These are built in worldspace
    }


    LightPath( mPosition, mArrowPath );
MEMTRACK_POP_GROUP("Mission - InteriorObjective");
}

//=============================================================================
// InteriorObjective::OnFinalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void InteriorObjective::OnFinalize()
{
    GetEventManager()->RemoveListener( this, EVENT_ENTER_INTERIOR_TRANSITION_START );

    UnregisterPosition( mHUDID );

    if ( mAnimatedIcon )
    {
        delete mAnimatedIcon;
        mAnimatedIcon = NULL;
    }
    UnlightPath( mArrowPath.mPathRoute );
}

//=============================================================================
// InteriorObjective::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void InteriorObjective::OnUpdate( unsigned int elapsedTime )
{
    if ( mAnimatedIcon )
    {
        mAnimatedIcon->Update( elapsedTime );
    }
    UpdateLightPath(mArrowPath);
}


//=============================================================================
// InteriorObjective::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void InteriorObjective::HandleEvent( EventEnum id, void* pEventData )
{
    switch( id )
    {
    case EVENT_ENTER_INTERIOR_TRANSITION_START:
        {
            if(*((tUID*)pEventData) == tEntity::MakeUID(mDestination))
            {
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
// InteriorObjective::GetPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* currentLoc )
//
// Return:      void 
//
//=============================================================================
void InteriorObjective::GetPosition( rmt::Vector* currentLoc )
{
    *currentLoc = mPosition;
}

//=============================================================================
// InteriorObjective::GetHeading
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* heading )
//
// Return:      void 
//
//=============================================================================
void InteriorObjective::GetHeading( rmt::Vector* heading )
{
    heading->Set(0.0f,0.0f,1.0f);
}

//=============================================================================
// InteriorObjective::SetDestination
//=============================================================================
void InteriorObjective::SetDestination(const char* d)
{
    strcpy(mDestination, d);
}

//=============================================================================
// InteriorObjective::SetDestination
//=============================================================================
void InteriorObjective::SetIcon(const char* d)
{
    rAssert( strlen(d) < 64 );

    strcpy(mIcon, d);
    mIcon[ strlen( d ) ] = '\0';
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************
