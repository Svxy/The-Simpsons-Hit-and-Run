//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        
//
// Description: Implement MissionCondition
//
// History:     09/07/2002 + Created -- NAME
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

#include <mission/conditions/missioncondition.h>

#include <events/eventmanager.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

bool MissionCondition::mFailedHitNRun = false;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// MissionCondition::MissionCondition
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
MissionCondition::MissionCondition() :
    mType( COND_INVALID ),
    mbIsViolated( false ),
    mLeaveInterior( false )
{
}

//==============================================================================
// MissionCondition::~MissionCondition
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
MissionCondition::~MissionCondition()
{
}

//=============================================================================
// MissionCondition::Initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MissionCondition::Initialize()
{
    mbIsViolated = false;
    mLeaveInterior = false;

    mFailedHitNRun = false;
    //GetEventManager()->AddListener( this, EVENT_HIT_AND_RUN_CAUGHT );

    OnInitialize();
}

//=============================================================================
// MissionCondition::Finalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MissionCondition::Finalize()
{
    //GetEventManager()->RemoveListener( this, EVENT_HIT_AND_RUN_CAUGHT );

    OnFinalize();

    mFailedHitNRun = false;
}

//=============================================================================
// MissionCondition::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void MissionCondition::HandleEvent( EventEnum id, void* pEventData )
{
    //if ( id == EVENT_HIT_AND_RUN_CAUGHT )
    //{
    //    mbIsViolated = true;
    //    mFailedHitNRun = true;
    //}
}

//=============================================================================
// MissionCondition::IsChaseCondition
//=============================================================================
// Description: Indicates whether this condition involves chasing.  Assume
//              false, and let those classes which have chases override this
//              function
//
// Parameters:  None
//
// Return:      Default value of false, to be overridden by chase conditions 
//
//=============================================================================
bool MissionCondition::IsChaseCondition()
{
    return( false );
}

//=============================================================================
// MissionCondition::IsClose
//=============================================================================
// Description: Indicates whether this condition is at all close to failure.
//              Very subjective, used for interactive music
//
// Parameters:  None
//
// Return:      True if close, false otherwise 
//
//=============================================================================
bool MissionCondition::IsClose()
{
    return( true );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
