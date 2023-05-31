//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        ActionEventLocator.cpp
//
// Description: Implement ActionEventLocator
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
#include <p3d/inventory.hpp>

//========================================
// Project Includes
//========================================
#include <meta/ActionEventLocator.h>
#include <memory/srrmemory.h>
#include <ai/actionbuttonmanager.h>
#include <ai/actionbuttonhandler.h>
#include <worldsim/character/charactermanager.h>
#include <worldsim/character/character.h>
#include <debug/profiler.h>

void PrepareString( char** string, unsigned char* lengthHolder, unsigned int length )
{
MEMTRACK_PUSH_GROUP( "ActionEventLocator" );
    rAssert( !(*lengthHolder) );
    
    if ( *string )
    {
        rAssertMsg( false, "Why is someone changing this!  BAD! Get Cary!" );
        (*lengthHolder) = 0;
        delete[] *string;
        *string = NULL;
    }
    else
    {
        (*lengthHolder) = length + 1;
    }

    *string = new(GMA_LEVEL_OTHER) char[ (*lengthHolder) ];        
MEMTRACK_POP_GROUP( "ActionEventLocator" );    
}

void SetString( char** string, unsigned int length, const char* newString )
{
    rAssert( newString );
    rAssert( length );
    rAssert( *string );

    if ( *string )
    {
        unsigned int newLength = (length - 1) > strlen(newString) ? strlen(newString) : length - 1;
        strncpy( *string, newString, newLength );
        (*string)[newLength] = '\0';
    }
}

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
static int AELcount=0;
//==============================================================================
// ActionEventLocator::ActionEventLocator
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
ActionEventLocator::ActionEventLocator() :
    mObjNameSize( 0 ),
    mJointNameSize( 0 ),
    mActionNameSize( 0 ),
    mShouldTransform( false ),
    mObjName( NULL ),
    mJointName( NULL ),
    mActionName( NULL ),
    mButton( CharacterController::DoAction )
{
    SetData( -1 );
    AELcount++;
    mMatrix.Identity();
}

//==============================================================================
// ActionEventLocator::~ActionEventLocator
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
ActionEventLocator::~ActionEventLocator()
{
    if ( mObjName )
    {
        delete [] mObjName;
        mObjName = NULL;
    }

    mObjNameSize = 0;
    AELcount--;
    if ( mJointName )
    {
        delete [] mJointName;
        mJointName = NULL;
    }

    mJointNameSize = 0;

    if ( mActionName )
    {
        delete [] mActionName;
        mActionName = NULL;
    }

    mActionNameSize = 0;

    int actionId = (int)this->GetData( );   
    if ( actionId != -1 )
    {
        actionId = -1;
        //The Action Button Manager will clean this out when the Dump Dyna or Destroy are called.
        //This sucks.
    }
}

//=============================================================================
// ActionEventLocator::SetObjNameSize
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int size )
//
// Return:      void 
//
//=============================================================================
void ActionEventLocator::SetObjNameSize( unsigned char size )
{
    PrepareString( &mObjName, &mObjNameSize, size );
}

//=============================================================================
// ActionEventLocator::SetObjName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
void ActionEventLocator::SetObjName( const char* name )
{
    SetString( &mObjName, mObjNameSize, name );
}

//=============================================================================
// ActionEventLocator::SetJointNameSize
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int size )
//
// Return:      void 
//
//=============================================================================
void ActionEventLocator::SetJointNameSize( unsigned char size )
{
    PrepareString( &mJointName, &mJointNameSize, size );
}

//=============================================================================
// ActionEventLocator::SetJointName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
void ActionEventLocator::SetJointName( const char* name )
{
    SetString( &mJointName, mJointNameSize, name );
}

//=============================================================================
// ActionEventLocator::SetActionNameSize
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int size )
//
// Return:      void 
//
//=============================================================================
void ActionEventLocator::SetActionNameSize( unsigned char size )
{
    PrepareString( &mActionName, &mActionNameSize, size );
}

//=============================================================================
// ActionEventLocator::SetActionName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
void ActionEventLocator::SetActionName( const char* name )
{
    SetString( &mActionName, mActionNameSize, name );
}
/*
==============================================================================
ActionEventLocator::AddToGame
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         bool 

=============================================================================
*/
bool ActionEventLocator::AddToGame( tEntityStore* store )
{
    return GetActionButtonManager()->AddActionEventLocator( this, store );
}
//=============================================================================
// ActionEventLocator::Reset
//=============================================================================
// Description: Comment
//
// Parameters:  ( void )
//
// Return:      void 
//
//=============================================================================
void ActionEventLocator::Reset( void )
{
    // Get the index for the action ptr.
    //
    int actionId = (int)this->GetData( );   
    ActionButton::ButtonHandler* pActionButtonHandler = GetActionButtonManager()->GetActionByIndex( actionId );
    rAssert( pActionButtonHandler );
    pActionButtonHandler->Reset( );
}
//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// ActionEventLocator::OnTrigger
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int playerID, bool bActive )
//
// Return:      void 
//
//=============================================================================
void ActionEventLocator::OnTrigger( unsigned int playerID )
{
BEGIN_PROFILE( "AEL On Trigger" );
    if ( this->GetPlayerEntered() )
    {
        // Get the index for the action ptr.
        //
        int actionId = (int)this->GetData( );        
        
        // Trigger the enter context.
        //
        unsigned int playerId = GetPlayerID();
        Character* pCharacter = GetCharacterManager( )->GetCharacter( playerId );
        rAssert( pCharacter );

        ActionButton::ButtonHandler* pActionButtonHandler = GetActionButtonManager()->GetActionByIndex( actionId );
        rAssert( pActionButtonHandler );

        // Entered an action volume.
        //
        if(pActionButtonHandler->UsesActionButton())
        {
            pCharacter->AddActionButtonHandler( pActionButtonHandler );
        }

        GetActionButtonManager()->EnterActionTrigger( pCharacter, actionId );
    }
    else
    {
        // Get the index for the action ptr.
        //
        int actionId = (int)this->GetData( );
        rAssert( actionId >= 0 );
        
        ActionButton::ButtonHandler* pActionButtonHandler = GetActionButtonManager()->GetActionByIndex( actionId );
        rAssert( pActionButtonHandler );

        // Trigger the exit context.
        //
        unsigned int playerId = GetPlayerID();
        Character* pCharacter = GetCharacterManager( )->GetCharacter( playerId );      
        
        GetActionButtonManager()->ExitActionTrigger( pCharacter, actionId );
        pCharacter->RemoveActionButtonHandler( pActionButtonHandler );
    }
END_PROFILE( "AEL On Trigger" );
}

