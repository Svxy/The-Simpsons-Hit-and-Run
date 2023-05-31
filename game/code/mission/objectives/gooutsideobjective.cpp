//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        gooutsidetoobjective.cpp
//
// Description: Implement GoOutsideObjective
//
// History:     16/04/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <p3d/utility.hpp>
//========================================
// Project Includes
//========================================


#include <mission/objectives/GoOutsideobjective.h>
#include <mission/objectives/gooutsideobjective.h>

#include <events/eventmanager.h>
#include <input/inputmanager.h>
#include <meta/eventlocator.h>
#include <meta/spheretriggervolume.h>
#include <meta/triggervolumetracker.h>

#include <mission/objectives/gotoobjective.h>
#include <mission/objectives/gooutsideobjective.h>
#include <mission/animatedicon.h>
#include <mission/gameplaymanager.h>
#include <mission/missionmanager.h>

#include <render/dsg/inststatentitydsg.h>

#include <ai/actionbuttonhandler.h>
#include <ai/actionbuttonmanager.h>

#include <worldsim/character/charactermanager.h>
#include <worldsim/character/character.h>

#include <memory/srrmemory.h>

#include <interiors/interiormanager.h>


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
// GoOutsideObjective::GoOutsideObjective
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
GoOutsideObjective::GoOutsideObjective()   
{

}

//==============================================================================
// GoOutsideObjective::~GoOutsideObjective
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
GoOutsideObjective::~GoOutsideObjective()
{
   
}

//=============================================================================
// GoOutsideObjective::OnInitalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GoOutsideObjective::OnInitialize()
{
MEMTRACK_PUSH_GROUP( "Mission - GoOutsideObjective" );
    GameMemoryAllocator gma = GetGameplayManager()->GetCurrentMissionHeap();

    rAssert( mDestinationLocator == NULL );

    mDestinationLocator = p3d::find<Locator>( mDestinationLocatorName );
    rAssert( mDestinationLocator != NULL );

    //mDestinationLocator->AddRef();
    
    mDestinationLocator->SetFlag( Locator::ACTIVE, true );
    //mDestinationLocator->SetFlag( Locator::DRAWN, true );

    EventLocator* eventLocator = dynamic_cast<EventLocator*>( mDestinationLocator );
    if( eventLocator != NULL )
    {
        GetEventManager()->AddListener( this, (EventEnum)(EVENT_LOCATOR + eventLocator->GetEventType()));
    }

    if ( mActionTrigger )
    {
        //Swap the settings of the event locator to make it go through the action button
        //handing system.
        mOldEvent = eventLocator->GetEventType();

        eventLocator->SetEventType( LocatorEvent::GENERIC_BUTTON_HANDLER_EVENT );

        ActionButton::GenericEventButtonHandler* pABHandler = dynamic_cast<ActionButton::GenericEventButtonHandler*>( ActionButton::GenericEventButtonHandler::NewAction( eventLocator, (EventEnum)(EVENT_LOCATOR + mOldEvent), gma ) ); 
        rAssert( pABHandler );

        pABHandler->SetEventData( static_cast<void*>(eventLocator) );

        int id = 0;
        bool bResult = GetActionButtonManager()->AddAction( pABHandler, id );

        rAssert( bResult );
        mOldData = eventLocator->GetData();
        eventLocator->SetData( id );
    }

    if( strcmp( mDestinationDrawableName, "" ) != 0 )
    {
	    rmt::Vector pos;
	    mDestinationLocator->GetLocation( &pos );

	    mAnimatedIcon = new AnimatedIcon();
        mAnimatedIcon->Init( mDestinationDrawableName, pos );

        SphereTriggerVolume* sphTrig = dynamic_cast<SphereTriggerVolume*>(eventLocator->GetTriggerVolume(0));
        //rAssertMsg( sphTrig != NULL, "GoOutside objectives should only use sphere triggers." );

        if ( sphTrig )
        {
            if ( mScaleFactor != 0.0f )
            {
                sphTrig->SetSphereRadius( mScaleFactor );
            }
        }

        //Only put up an icon in the HUD if there is a drawable in the world.
        RegisterLocator( mDestinationLocator, mHudMapIconID, true, HudMapIcon::ICON_MISSION );
    }
    else
    {
//        mDestinationLocator->SetFlag( Locator::DRAWN, true );
    }

    if ( strcmp( mEffectName, "" ) != 0 )
    {
        mCollectEffect = new AnimatedIcon();
        mCollectEffect->Init( mEffectName, rmt::Vector( 0.0f, 0.0f, 0.0f ), false, true );
    }

    mWaitingForEffect = false;

    rmt::Vector targetPosn;
    mDestinationLocator->GetLocation(&targetPosn);
    LightPath( targetPosn, mArrowPath );    

MEMTRACK_POP_GROUP("Mission - GoOutsideObjective");
}

