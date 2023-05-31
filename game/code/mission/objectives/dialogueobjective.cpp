//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        DialogueObjective.cpp
//
// Description: Implement DialogueObjective
//
// History:     03/09/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <string.h>
#include <p3d/refcounted.hpp>

//========================================
// Project Includes
//========================================
#include <mission/objectives/DialogueObjective.h>
#include <mission/gameplaymanager.h>

#include <events/eventmanager.h>
#include <worldsim/character/charactermanager.h>
#include <worldsim/character/character.h>
#include <worldsim/traffic/trafficmanager.h>
#include <worldsim/redbrick/vehicle.h>

#include <meta/carstartlocator.h>

#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>

#include <worldsim/traffic/trafficmanager.h>
#include <worldsim/vehiclecentral.h>


//MS11 Hack
#include <gameflow/gameflow.h>
#include <contexts/context.h>

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
// DialogueObjective::DialogueObjective
//=============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
DialogueObjective::DialogueObjective():
    mDialogueName( 0 ),
    mCameraDistance( 3.0f ),
    mChar1Pos( NULL ),
    mChar2Pos( NULL ),
    mCarPos( NULL ),
    mChar1Rotation( 0.0f ),
    mChar2Rotation( 0.0f ),
    mCarRotation( 0.0f ),
    mReset( false ),
    mMoved( false ),
    mCharacter1WasInCarToStartWith( false ),
    mCharacter2WasInCarToStartWith( false ),
    mHidTheCar( false ),
    mHidDefault( false ),
    mDontReset( false )
{
    mDialogEventData.char1 = NULL;
    mDialogEventData.char2 = NULL;
    mChar1Name[0] = '\0';
    mChar2Name[0] = '\0';

    mChar1OldPos.Set( 0.0f, 0.0f, 0.0f );
    mChar2OldPos.Set( 0.0f, 0.0f, 0.0f );
    mCarOldPos.Set( 0.0f, 0.0f, 0.0f );
}

//=============================================================================
// DialogueObjective::~DialogueObjective
//=============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
DialogueObjective::~DialogueObjective()
{
    if( mDialogEventData.char1 != NULL )
    {
        mDialogEventData.char1->Release();
        mDialogEventData.char1 = NULL;
    }
    if( mDialogEventData.char2 != NULL )
    {
        mDialogEventData.char2->Release();
        mDialogEventData.char2 = NULL;
    }

    if ( mChar1Pos )
    {
        mChar1Pos->Release();
        mChar1Pos = NULL;
    }

    if ( mChar2Pos )
    {
        mChar2Pos->Release();
        mChar2Pos = NULL;
    }

    if ( mCarPos )
    {
        mCarPos->Release();
        mCarPos = NULL;
    }
}

//=============================================================================
// DialogueObjective::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void DialogueObjective::HandleEvent( EventEnum id, void* pEventData )
{
    rAssert( id == EVENT_CONVERSATION_DONE_AND_FINISHED );

    //Wait for the dialogue system to announce that the dialogue is finished
    //playing.
    SetFinished( true );
}

//=============================================================================
// DialogueObjective::SetCameraDistance
//=============================================================================
// Description: Sets the distance that the camera will use when the dialog 
//              is played back
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
void DialogueObjective::SetCameraDistance( const float distance )
{
    mCameraDistance = distance;
}

//=============================================================================
// DialogueObjective::SetPositions
//=============================================================================
// Description: Comment
//
// Parameters:  ( CarStartLocator* pos1, CarStartLocator* pos2, CarStartLocator* carPos, bool dontReset )
//
// Return:      void 
//
//=============================================================================
void DialogueObjective::SetPositions( CarStartLocator* pos1, CarStartLocator* pos2, CarStartLocator* carPos, bool dontReset )
{
    tRefCounted::Assign( mChar1Pos, pos1 );
    tRefCounted::Assign( mChar2Pos, pos2 );
    tRefCounted::Assign( mCarPos, carPos );

    mDontReset = dontReset;
}

//=============================================================================
// DialogueObjective::CharactersReset
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool DialogueObjective::CharactersReset()
{
    return mMoved;
}


//=============================================================================
// DialogueObjective::SetChar1Name
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
void DialogueObjective::SetChar1Name( const char* name )
{
    unsigned int len = strlen(name) < MAX_CHAR_NAME_LEN - 1 ? strlen(name) : MAX_CHAR_NAME_LEN - 2;
    strncpy( mChar1Name, name, len );
    mChar1Name[len] = '\0';
}

//=============================================================================
// DialogueObjective::SetChar2Name
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
void DialogueObjective::SetChar2Name( const char* name )
{
    unsigned int len = strlen(name) < MAX_CHAR_NAME_LEN - 1 ? strlen(name) : MAX_CHAR_NAME_LEN - 2;
    strncpy( mChar2Name, name, len );
    mChar2Name[len] = '\0';
}

//=============================================================================
// DialogueObjective::SetDialogueName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
void DialogueObjective::SetDialogueName( const char* name )
{
    mDialogueName = ::radMakeKey32( name );
}

//*****************************************************************************
//
// Protected Member Functions
//
//*****************************************************************************

//=============================================================================
// DialogueObjective::OnInitialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void DialogueObjective::OnInitialize()
{
    rAssert( strlen(mChar1Name) != 0 );
    rAssert( strlen(mChar2Name) != 0 );

    //Register with the event system that you are listening for the dialogue end
    //event.
    GetEventManager()->AddListener( this, EVENT_CONVERSATION_DONE_AND_FINISHED );
    
    //Send the alert to the dialogue system to start playing this bad-ass
    Character* c1 = GetCharacterManager()->GetMissionCharacter( mChar1Name );
    tRefCounted::Assign( mDialogEventData.char1, c1 );
    TrafficManager::GetInstance()->AddCharacterToStopFor( c1 );

    Character* c2 = GetCharacterManager()->GetMissionCharacter( mChar2Name );
    tRefCounted::Assign( mDialogEventData.char2, c2 );
    TrafficManager::GetInstance()->AddCharacterToStopFor( c2 );


    //POSITIONS, PLEASE!
    mCharacter1WasInCarToStartWith = c1->IsInCar();
    c1->GetPosition( &mChar1OldPos );
    mChar1Rotation = c1->GetFacingDir();

    rmt::Vector location;
    if ( mChar1Pos )
    {
        mChar1Pos->GetLocation( &location );
        c1->RelocateAndReset( location, mChar1Pos->GetRotation() );
    }


    mCharacter2WasInCarToStartWith = c2->IsInCar();
    c2->GetPosition( &mChar2OldPos );
    mChar2Rotation = c2->GetFacingDir();

    if ( mChar2Pos )
    {
        mChar2Pos->GetLocation( &location );
        c2->RelocateAndReset( location, mChar2Pos->GetRotation() );
    }

    Vehicle* v = GetGameplayManager()->GetCurrentVehicle();
    v->GetPosition( &mCarOldPos );
    mCarRotation = v->GetFacingInRadians();

    bool needToHide = false;

    float dist = 0.0f;
    
    if ( mChar1Pos )
    {
        mChar1Pos->GetLocation( &location );
        rmt::Vector c1ToCar;
        c1ToCar.Sub( mCarOldPos, location );
        if ( c1ToCar.MagnitudeSqr() < 49.0f )
        {
            needToHide = true;
        }
    }

    if ( !needToHide && mChar2Pos )
    {
        mChar2Pos->GetLocation( &location );
        rmt::Vector c2ToCar;
        c2ToCar.Sub( mCarOldPos, location );
        if ( c2ToCar.MagnitudeSqr() < 49.0f )
        {
            needToHide = true;
        }
    }

    if ( needToHide )
    {
        //hide this car
        v->CarDisplay( false );
        GetVehicleCentral()->RemoveVehicleFromActiveList( v );
        mHidTheCar = true;
    }

    //Should also hide the default car if it is not your current vehicle.
    Vehicle* defaultCar = GetGameplayManager()->GetVehicleInSlot( GameplayManager::eDefaultCar );
    if ( defaultCar && defaultCar != v )
    {
        bool needToHideDefault = false;

        rmt::Vector defaultPos;
        defaultCar->GetPosition( &defaultPos );

        if ( mChar1Pos )
        {
            mChar1Pos->GetLocation( &location );
            rmt::Vector c1ToCar;
            c1ToCar.Sub( defaultPos, location );
            if ( c1ToCar.MagnitudeSqr() < 49.0f )
            {
                needToHideDefault = true;
            }
        }

        if ( !needToHideDefault && mChar2Pos )
        {
            mChar2Pos->GetLocation( &location );
            rmt::Vector c2ToCar;
            c2ToCar.Sub( defaultPos, location );
            if ( c2ToCar.MagnitudeSqr() < 49.0f )
            {
                needToHideDefault = true;
            }
        }

        if ( needToHideDefault )
        {
            //hide default
            defaultCar->CarDisplay( false );
            GetVehicleCentral()->RemoveVehicleFromActiveList( defaultCar );

            mHidDefault = true;
        }
    }

    if ( mChar1Pos || mChar2Pos )
    {
        GetSuperCamManager()->GetSCC( 0 )->DoCameraCut();

        mChar1Pos->GetLocation( &location );
        TrafficManager::GetInstance()->ClearTrafficInSphere( rmt::Sphere( location, 50.0f ) );

        mMoved = true;
    }

    mDialogEventData.dialogName = mDialogueName;
    GetEventManager()->TriggerEvent( EVENT_CONVERSATION_INIT, (void*)(&mDialogEventData) );
    GetEventManager()->TriggerEvent( EVENT_CONVERSATION_INIT_DIALOG, (void*)(&mDialogEventData) );

    //Maybe we can move the start event into the update for this objective....
    GetEventManager()->TriggerEvent( EVENT_CONVERSATION_START, (void*)(NULL) );

    //MS11 Hack
    //Something else should be responsible for suspending us.
    GetGameFlow()->GetContext( CONTEXT_GAMEPLAY )->Suspend();

    mReset = false;
}

//=============================================================================
// DialogueObjective::OnFinalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void DialogueObjective::OnFinalize()
{
    ResetCharacterPositions();

    if( mDialogEventData.char1 != NULL )
    {
        TrafficManager::GetInstance()->RemoveCharacterToStopFor( mDialogEventData.char1 );
        mDialogEventData.char1->Release();
        mDialogEventData.char1 = NULL;
    }

    if( mDialogEventData.char2 != NULL )
    {
        TrafficManager::GetInstance()->RemoveCharacterToStopFor( mDialogEventData.char2 );
        mDialogEventData.char2->Release();
        mDialogEventData.char2 = NULL;
    }

    GetEventManager()->RemoveListener( this, EVENT_CONVERSATION_DONE_AND_FINISHED );
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************

//=============================================================================
// DialogueObjective::ResetCharacterPositions
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void DialogueObjective::ResetCharacterPositions()
{
    if ( mReset || !mMoved )
    {
        return;
    }

    if( !mCharacter1WasInCarToStartWith )
    {
        if ( mChar1Pos && mDialogEventData.char1 && !mDontReset )
        {
            mDialogEventData.char1->RelocateAndReset( mChar1OldPos, mChar1Rotation );
        }
    }

    if( !mCharacter2WasInCarToStartWith )
    {
        if ( mChar2Pos && mDialogEventData.char2 && !mDontReset )
        {
            mDialogEventData.char2->RelocateAndReset( mChar2OldPos, mChar2Rotation );
        }
    }

    if ( mHidTheCar )
    {
        Vehicle* v = GetGameplayManager()->GetCurrentVehicle();
        
        v->CarDisplay( true );
        GetVehicleCentral()->AddVehicleToActiveList( v );
        mHidTheCar = false;
    }

    if ( mHidDefault )
    {
        Vehicle* defaultCar = GetGameplayManager()->GetVehicleInSlot( GameplayManager::eDefaultCar );
        defaultCar->CarDisplay( true );
        GetVehicleCentral()->AddVehicleToActiveList( defaultCar );

        mHidDefault = false;
    }

    mReset = true;
    mMoved = false;

    if ( mChar1Pos || mChar2Pos )
    {
        GetSuperCamManager()->GetSCC( 0 )->DoCameraCut();
        GetSuperCamManager()->GetSCC( 0 )->Update( 16 );
    }
}
