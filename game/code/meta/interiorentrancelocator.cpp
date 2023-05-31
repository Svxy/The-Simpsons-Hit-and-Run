//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        InteriorEntranceLocator.cpp
//
// Description: Implement InteriorEntranceLocator
//
// History:     30/07/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <string.h>

//========================================
// Project Includes
//========================================
#include <meta/InteriorEntranceLocator.h>
#include <memory/srrmemory.h>
#include <meta/locatorevents.h>
#include <events/eventmanager.h>
#include <ai/actionbuttonmanager.h>
#include <ai/actionbuttonhandler.h>
#include <worldsim/character/charactermanager.h>
#include <worldsim/character/character.h>
#include <debug/profiler.h>

#include <mission/gameplaymanager.h>

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
// InteriorEntranceLocator::InteriorEntranceLocator
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
InteriorEntranceLocator::InteriorEntranceLocator() :
    mInteriorFileName( NULL ),
    mInteriorFileNameSize( 0 )
{
    SetEventType( LocatorEvent::INTERIOR_ENTRANCE );
    mTransform.Identity();
    mpEnterInteriorAction = new ActionButton::EnterInterior( this );
    mpEnterInteriorAction->AddRef();
}

//==============================================================================
// InteriorEntranceLocator::~InteriorEntranceLocator
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
InteriorEntranceLocator::~InteriorEntranceLocator()
{
    if ( mInteriorFileName )
    {
        delete[] mInteriorFileName;
        mInteriorFileName = NULL;
    }

    // Tell the button handler to forget about us, then release it
    //
    mpEnterInteriorAction->SetLocator( 0 );
    tRefCounted::Release( mpEnterInteriorAction );
    mpEnterInteriorAction = 0;
    mInteriorFileNameSize = 0;
}

//=============================================================================
// InteriorEntranceLocator::SetInteriorFileName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* fileName )
//
// Return:      void 
//
//=============================================================================
void InteriorEntranceLocator::SetInteriorFileName( const char* fileName )
{
    rAssert( fileName );
    rAssert( mInteriorFileNameSize );   
    rAssert( mInteriorFileName );

    if ( mInteriorFileName )
    {
        unsigned int length = (mInteriorFileNameSize - 1) > strlen(fileName) ? strlen(fileName) : mInteriorFileNameSize - 1;
        strncpy( mInteriorFileName, fileName, length );
        mInteriorFileName[length] = '\0';
    }
}

//=============================================================================
// InteriorEntranceLocator::SetInteriorFileNameSize
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int size )
//
// Return:      void 
//
//=============================================================================
void InteriorEntranceLocator::SetInteriorFileNameSize( unsigned int size )
{
MEMTRACK_PUSH_GROUP( "InteriorEntranceLocator" );
    rAssert( !mInteriorFileNameSize );
    
    if ( mInteriorFileName )
    {
        rAssertMsg( false, "Why is someone changing this!  BAD! Get Cary!" );
        mInteriorFileNameSize = 0;
        delete[] mInteriorFileName;
        mInteriorFileName = NULL;
    }
    else
    {
        mInteriorFileNameSize = size + 1;
    }

    mInteriorFileName = new(GMA_LEVEL_OTHER) char[ mInteriorFileNameSize ];        
MEMTRACK_POP_GROUP( "InteriorEntranceLocator" );   
}
/*
==============================================================================
InteriorEntranceLocator::OnTrigger
==============================================================================
Description:    Comment

Parameters:     ( unsigned int playerID )

Return:         void 

=============================================================================
*/
void InteriorEntranceLocator::OnTrigger( unsigned int playerID )
{
BEGIN_PROFILE( "IEL OnTrigger" );
    //This will fire an event off that uses the data field
    GetEventManager()->TriggerEvent( (EventEnum)( EVENT_LOCATOR + GetEventType() ), (void*)this );

    rAssert( mpEnterInteriorAction );

    bool safeToEnter = true;

    if(GetGameplayManager()->GetCurrentMission() != NULL)
    {
        //chuck: adding a check, if we are in a street race or gamble race interiors are off.
        if(
            GetGameplayManager()->GetCurrentMission()->IsRaceMission() 
            ||
            GetGameplayManager()->GetCurrentMission()->IsWagerMission()
            )
        {
            safeToEnter = false;
        }
    }

    if ( this->GetPlayerEntered() && safeToEnter )
    {
        if ( mpEnterInteriorAction )
        {
            // Trigger the enter context.
            //
            unsigned int playerId = GetPlayerID();
            Character* pCharacter = GetCharacterManager( )->GetCharacter( playerId );
            rAssert( pCharacter );

            // Entered an action volume.
            //
            pCharacter->AddActionButtonHandler( mpEnterInteriorAction );
            mpEnterInteriorAction->Enter( pCharacter );
        }
    }
    else
    {
        if ( mpEnterInteriorAction )
        {
        
            // Trigger the exit context.
            //
            unsigned int playerId = GetPlayerID();
            Character* pCharacter = GetCharacterManager( )->GetCharacter( playerId );      
        
            mpEnterInteriorAction->Exit( pCharacter );      
            pCharacter->RemoveActionButtonHandler( mpEnterInteriorAction );
        }
    }
END_PROFILE( "IEL OnTrigger" );
}
//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
