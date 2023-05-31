//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        gotoobjective.cpp
//
// Description: Implement GoToObjective
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

#include <events/eventmanager.h>
#include <input/inputmanager.h>
#include <meta/eventlocator.h>
#include <meta/spheretriggervolume.h>
#include <meta/triggervolumetracker.h>

#include <mission/objectives/gotoobjective.h>
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

#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>
#include <camera/walkercam.h>

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
// GoToObjective::GoToObjective
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
GoToObjective::GoToObjective() :
    mDestinationLocator( NULL ),
    mAnimatedIcon( NULL ),
    mScaleFactor( 1.0f ),
    mHudMapIconID( -1 ),
    mCollectEffect( NULL ),
    mWaitingForEffect( false ),
    mGotoDialogOn( true ),
    mActionTrigger( false ),
    mOldEvent( LocatorEvent::NUM_EVENTS ),
    mOldData( -1 )
{
    mArrowPath.mPathRoute.Allocate( RoadManager::GetInstance()->GetNumRoads() );   

    mEffectName[0] = '\0';
}

//==============================================================================
// GoToObjective::~GoToObjective
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
GoToObjective::~GoToObjective()
{
    if ( mAnimatedIcon )
    {
        delete mAnimatedIcon;
        mAnimatedIcon = NULL;

        UnregisterLocator( mHudMapIconID );

        //Alert the walkercam that there is a drawable.
        WalkerCam* wc = static_cast<WalkerCam*>(GetSuperCamManager()->GetSCC( 0 )->GetSuperCam( SuperCam::WALKER_CAM ));
        if ( wc )
        {
            wc->SetCollectibleLocation( mAnimatedIcon );
        }

    }
    if ( mCollectEffect )
    {
        delete mCollectEffect;
        mCollectEffect = NULL;
    }
}

//=============================================================================
// GoToObjective::OnInitalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GoToObjective::OnInitialize()
{
MEMTRACK_PUSH_GROUP( "Mission - GoToObjective" );
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
        //rAssertMsg( sphTrig != NULL, "GOTO objectives should only use sphere triggers." );

        if ( sphTrig )
        {
            if ( mScaleFactor != 0.0f )
            {
                sphTrig->SetSphereRadius( mScaleFactor );
            }
        }

        //Only put up an icon in the HUD if there is a drawable in the world.
        RegisterLocator( mDestinationLocator, mHudMapIconID, true, HudMapIcon::ICON_MISSION );

        //Alert the walkercam that there is a drawable.
        WalkerCam* wc = static_cast<WalkerCam*>(GetSuperCamManager()->GetSCC( 0 )->GetSuperCam( SuperCam::WALKER_CAM ));
        if ( wc )
        {
            wc->SetCollectibleLocation( mAnimatedIcon );
        }
    }
    else
    {
//        mDestinationLocator->SetFlag( Locator::DRAWN, true );
    }

    if ( strcmp( mEffectName, "" ) != 0 && strcmp( mEffectName, "none" ) != 0 )
    {
        mCollectEffect = new AnimatedIcon();
        mCollectEffect->Init( mEffectName, rmt::Vector( 0.0f, 0.0f, 0.0f ), false, true );
    }

    mWaitingForEffect = false;

    rmt::Vector targetPosn;
    mDestinationLocator->GetLocation(&targetPosn);
    LightPath( targetPosn, mArrowPath );

    // if the goto objective is near the entrance of our current interior, 
    // lets assume that we don't need to do it
    if(GetInteriorManager()->IsInside())
    {
        Locator* entrance =  p3d::find<Locator>(GetInteriorManager()->GetInterior());
        if(entrance)
        {
            rmt::Vector e, d, dist;
            entrance->GetLocation(&e);
            mDestinationLocator->GetLocation(&d);
            dist.Sub(d,e);

            if(dist.Magnitude() < 20.0f)
            {
                SetFinished( true );

                // don't wan't to show stage complete for doing nothing
                GetGameplayManager()->GetCurrentMission()->GetCurrentStage()->ShowStageComplete(false);
            }
        }
    }

MEMTRACK_POP_GROUP("Mission - GoToObjective");
}

//=============================================================================
// GoToObjective::OnFinalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GoToObjective::OnFinalize()
{
    //
    // Disable the controller
    //
    GetInputManager()->SetGameState( Input::ACTIVE_GAMEPLAY );


    rAssert( mDestinationLocator != NULL );

    mDestinationLocator->SetFlag( Locator::ACTIVE, false );

    EventLocator* eventLocator = dynamic_cast<EventLocator*>( mDestinationLocator );
    if( eventLocator != NULL )
    {
        GetEventManager()->RemoveListener( this, (EventEnum)(EVENT_LOCATOR + eventLocator->GetEventType()));

        //This is to make sure that the exit volume triggers to kill characters hanging on
        //to action button handlers.  Semi-hack
        unsigned int i;
        for ( i = 0; i < eventLocator->GetNumTriggers(); ++i )
        {
            GetTriggerVolumeTracker()->RemoveTrigger( eventLocator->GetTriggerVolume( i ) );
            GetTriggerVolumeTracker()->AddTrigger( eventLocator->GetTriggerVolume( i ) );
        }
    }

    //Redundant if the old data is -1
    if ( mActionTrigger && mOldData != -1 )
    {
        int id = eventLocator->GetData();
        
        GetActionButtonManager()->RemoveActionByIndex( id );

        eventLocator->SetData( mOldData );
        eventLocator->SetEventType( mOldEvent );

        //reset the old data
        mOldData = -1;
    }

    //mDestinationLocator->Release();
    mDestinationLocator = NULL;

    if ( mAnimatedIcon )
    {
        delete mAnimatedIcon;
        mAnimatedIcon = NULL;            

        UnregisterLocator( mHudMapIconID );

        //Alert the walkercam that there is a drawable.
        WalkerCam* wc = static_cast<WalkerCam*>(GetSuperCamManager()->GetSCC( 0 )->GetSuperCam( SuperCam::WALKER_CAM ));
        if ( wc )
        {
            wc->SetCollectibleLocation( NULL );
        }
    } 

    if ( mCollectEffect )
    {
        delete mCollectEffect;
        mCollectEffect = NULL;
    }

    UnlightPath( mArrowPath.mPathRoute );
}

//=============================================================================
// GoToObjective::OnUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void GoToObjective::OnUpdate( unsigned int elapsedTime )
{
    if ( mAnimatedIcon )
    {
        mAnimatedIcon->Update( elapsedTime );
    }

    if ( mCollectEffect )
    {
        mCollectEffect->Update( elapsedTime );
    }

    //This is so that we can see the effect finish before we blow it away.
    if ( mWaitingForEffect )
    {
        if ( !mCollectEffect->IsRendering() )
        {
            //The effect is finished rendering.
            SetFinished( true );
        }
    }

    UpdateLightPath(mArrowPath);

    //Hard-code a test to see if we're already in the volume.  This will correct if
    //We're starting a mission inside it.
    if ( !GetFinished() && mDestinationLocator && !mActionTrigger )
    {
        unsigned int i;
        for ( i = 0; i < mDestinationLocator->GetNumTriggers(); ++i )
        {
            if ( reinterpret_cast<TriggerLocator*>(mDestinationLocator)->GetTriggerVolume( i )->IsPlayerTracking( 0 ) )
            {
                SetFinished( true );
            }
        }
    }
}


//=============================================================================
// GoToObjective::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void GoToObjective::HandleEvent( EventEnum id, void* pEventData )
{
    switch( id )
    {
    case EVENT_LOCATOR + LocatorEvent::CHECK_POINT:
        {
            Locator* locator = static_cast<Locator*>( pEventData );

            if( locator == mDestinationLocator && !mWaitingForEffect )
            {
                if ( mActionTrigger && mOldData != -1 )
                {
                    //This is a hack for if the Effect takes too long, we need
                    //to get rid of the frickin' button in the screen 
                    int id = locator->GetData();
                    GetCharacterManager()->GetCharacter( 0 )->RemoveActionButtonHandler( GetActionButtonManager()->GetActionByIndex( id ) );
                }

                if( mGotoDialogOn )
                {
                    GetEventManager()->TriggerEvent( EVENT_DESTINATION_REACHED );
                }
                
                if ( IsFinished() == false )
                {
                    //
                    // Disable the controller
                    //
                    GetInputManager()->SetGameState( Input::ACTIVE_NONE );


                    if ( mAnimatedIcon )
                    {
                        //Put the collection effect here and hit start.
                        rmt::Vector pos;
                        locator->GetLocation( &pos );

                        if ( mCollectEffect )
                        {
                            mCollectEffect->Reset();
                            mCollectEffect->Move( pos );
                            mCollectEffect->ShouldRender( true );

                            mWaitingForEffect = true;
                        }
                        else if ( strcmp( mEffectName, "none" ) != 0 )
                        {
                            //Only show the effect if there is a drawable.
                            if ( strcmp( mDestinationDrawableName, "" ) != 0 )
                            {
                                //USe the default one from the mission manager
                                GetMissionManager()->PutEffectHere( pos );  
                            }
                        }


                        //Also, get rid of the old one.
                        mAnimatedIcon->ShouldRender( false );
                    }
                }

                if ( !mWaitingForEffect )
                {
                    SetFinished( true );
                }
            }

            break;
        }
    default:
        {
            // this isn't a "real" event listener so it may get events
            // it doesn't care about
            break;
        }
    }
}

//=============================================================================
// GoToObjective::SetDestinationLocatorName
//=============================================================================
// Description: Comment
//
// Parameters:  ( char* locatorname, char* p3dname, float scale )
//
// Return:      void 
//
//=============================================================================
void GoToObjective::SetDestinationNames( char* locatorname, char* p3dname, float scale )
{
    strcpy( mDestinationLocatorName, locatorname );
	strcpy( mDestinationDrawableName, p3dname );
    mScaleFactor = scale;
}

//=============================================================================
// GoToObjective::SetCollectEffectName
//=============================================================================
// Description: Comment
//
// Parameters:  ( char* name )
//
// Return:      void 
//
//=============================================================================
void GoToObjective::SetCollectEffectName( char* name )
{
    rAssert( name );
    strcpy( mEffectName, name );
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
