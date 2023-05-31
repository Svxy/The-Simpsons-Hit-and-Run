//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        PositionCondition.cpp
//
// Description: Implement PositionCondition
//
// History:     11/20/2002 + Created -- Cary Brisebois
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
#include <mission/conditions/PositionCondition.h>

#include <events/eventmanager.h>
#include <events/eventenum.h>

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
// PositionCondition::PositionCondition
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
PositionCondition::PositionCondition() :
    mRequiredPosition( 1 ),
    mNumOthersFinished( 0 )
{
    this->SetType( COND_POSITION );
}

//==============================================================================
// PositionCondition::~PositionCondition
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
PositionCondition::~PositionCondition()
{
}

//=============================================================================
// PositionCondition::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void PositionCondition::HandleEvent( EventEnum id, void* pEventData )
{
    if ( id == EVENT_WAYAI_AT_DESTINATION )
    {
        mNumOthersFinished++;

        if ( mNumOthersFinished >= mRequiredPosition )
        {
            SetIsViolated( true );
        }
    }

    MissionCondition::HandleEvent( id, pEventData );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// PositionCondition::OnInitialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PositionCondition::OnInitialize()
{
    
    mNumOthersFinished = 0;
    GetEventManager()->AddListener( this, EVENT_WAYAI_AT_DESTINATION );
}

//=============================================================================
// PositionCondition::OnFinalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PositionCondition::OnFinalize()
{
    GetEventManager()->RemoveListener( this, EVENT_WAYAI_AT_DESTINATION );
}
