//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement OutOfBoundsCondition
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

#include <mission/conditions/outofboundscondition.h>

#include <meta/eventlocator.h>

#include <constants/maxplayers.h>

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
// OutOfBoundsCondition::OutOfBoundsCondition
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
OutOfBoundsCondition::OutOfBoundsCondition()
{
    this->SetType( COND_OUT_OF_BOUNDS );
}

//==============================================================================
// OutOfBoundsCondition::~OutOfBoundsCondition
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
OutOfBoundsCondition::~OutOfBoundsCondition()
{
}

//=============================================================================
// OutOfBoundsCondition::OnInitialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void OutOfBoundsCondition::OnInitialize()
{
    GetEventManager()->AddListener( this, (EventEnum)(EVENT_LOCATOR + LocatorEvent::DEATH) );
}

//=============================================================================
// OutOfBoundsCondition::OnFinalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void OutOfBoundsCondition::OnFinalize()
{
    GetEventManager()->RemoveListener( this, (EventEnum)(EVENT_LOCATOR + LocatorEvent::DEATH) );
}

//=============================================================================
// OutOfBoundsCondition::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void OutOfBoundsCondition::HandleEvent( EventEnum id, void* pEventData )
{
    switch( id )
    {
    case (EventEnum)(EVENT_LOCATOR + LocatorEvent::DEATH):
        {
            EventLocator* evtLoc = static_cast<EventLocator*>(pEventData);
            if ( evtLoc->GetPlayerID() < static_cast<unsigned int>(MAX_PLAYERS) )
            {
                //This is a player
                SetIsViolated( true );
            }
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
// OutOfBoundsCondition::IsClose
//=============================================================================
// Description: Indicates whether this condition is at all close to failure.
//              Very subjective, used for interactive music
//
// Parameters:  None
//
// Return:      True if close, false otherwise 
//
//=============================================================================
bool OutOfBoundsCondition::IsClose()
{
    //
    // I don't really know what this does, let's say not close
    //
    return( false );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
