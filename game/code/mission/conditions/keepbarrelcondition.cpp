//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        keepbarrelcondition.cpp
//
// Description: Implement KeepBarrelCondition
//
// History:     5/30/2003 + Created -- Cary Brisebois
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
#include <mission/conditions/keepbarrelcondition.h>
#include <mission/gameplaymanager.h>
#include <mission/mission.h>

#include <events/eventenum.h>
#include <events/eventmanager.h>


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
// KeepBarrelCondition::KeepBarrelCondition
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
KeepBarrelCondition::KeepBarrelCondition() :
    mJumpBackBy( 1 )
{
}

//=============================================================================
// KeepBarrelCondition::~KeepBarrelCondition
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
KeepBarrelCondition::~KeepBarrelCondition()
{
}

//=============================================================================
// KeepBarrelCondition::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void KeepBarrelCondition::HandleEvent( EventEnum id, void* pEventData )
{
    if ( id == EVENT_STATEPROP_COLLECTIBLE_DESTROYED )
    {
        //This is kinda hacky, but it's only for this mission anyway.
        //Maybe I should signal and have the stage change in the update
        //of the mission.
        //TODO:  Think about it.
        GetGameplayManager()->GetCurrentMission()->SpecialCaseStageBackup( mJumpBackBy );
    }

    MissionCondition::HandleEvent( id, pEventData );
}



//*****************************************************************************
//
// Protected Member Functions
//
//*****************************************************************************

//=============================================================================
// KeepBarrelCondition::OnInitialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void KeepBarrelCondition::OnInitialize()
{
    //Add a listener for the barrel exploding
    GetEventManager()->AddListener( this, EVENT_STATEPROP_COLLECTIBLE_DESTROYED );
}

//=============================================================================
// KeepBarrelCondition::OnFinalize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void KeepBarrelCondition::OnFinalize()
{
    //Remove the listeners
    GetEventManager()->RemoveListener( this, EVENT_STATEPROP_COLLECTIBLE_DESTROYED );
}
