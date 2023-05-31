//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement RaceCondition
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

#include <mission/conditions/racecondition.h>

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
// RaceCondition::RaceCondition
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
RaceCondition::RaceCondition() :
    m_playerWaypointsRemaining( 0 ),
    m_aiWaypointsRemaining( 0 )
{
    this->SetType( COND_RACE );
}

//==============================================================================
// RaceCondition::~RaceCondition
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
RaceCondition::~RaceCondition()
{
}

//=============================================================================
// RaceCondition::OnInitalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void RaceCondition::OnInitialize()
{
    GetEventManager()->AddListener( this, EVENT_WAYAI_AT_DESTINATION );
    GetEventManager()->AddListener( this, EVENT_WAYAI_HIT_CHECKPOINT );
    GetEventManager()->AddListener( this, EVENT_WAYAI_HIT_LAST_WAYPOINT );
}

//=============================================================================
// RaceCondition::OnFinalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void RaceCondition::OnFinalize()
{
    GetEventManager()->RemoveListener( this, EVENT_WAYAI_AT_DESTINATION );
    GetEventManager()->RemoveListener( this, EVENT_WAYAI_HIT_CHECKPOINT );
    GetEventManager()->RemoveListener( this, EVENT_WAYAI_HIT_LAST_WAYPOINT );
}

//=============================================================================
// RaceCondition::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void RaceCondition::HandleEvent( EventEnum id, void* pEventData )
{
    switch( id )
    {
    case EVENT_WAYAI_HIT_LAST_WAYPOINT: // fall thru
    case EVENT_WAYAI_AT_DESTINATION:
        {
            if( pEventData == (void*)GetVehicle() )
            {
                SetIsViolated( true );
            }

            m_aiWaypointsRemaining = 0;
            break;
        }
    case EVENT_WAYAI_HIT_CHECKPOINT:
        {
            m_aiWaypointsRemaining = *(static_cast<int*>( pEventData ));
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
// RaceCondition::IsChaseCondition
//=============================================================================
// Description: Indicate that this condition involves chasing
//
// Parameters:  None
//
// Return:      True 
//
//=============================================================================
bool RaceCondition::IsChaseCondition()
{
    return( true );
}

//=============================================================================
// RaceCondition::IsClose
//=============================================================================
// Description: Indicates whether this condition is at all close to failure.
//              Very subjective, used for interactive music
//
// Parameters:  None
//
// Return:      True if close, false otherwise 
//
//=============================================================================
bool RaceCondition::IsClose()
{
    //
    // TEMPORARY: this is certainly wrong, but let's use it until I have
    //            a chance to ask someone how to implement this correctly -- Esan
    //
    return( m_aiWaypointsRemaining <= 1 );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
