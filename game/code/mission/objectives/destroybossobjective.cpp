//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   destroybossobjective
//
// Description: destroybossobjective
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <mission/objectives/destroybossobjective.h>
#include <ai/actor/actor.h>
#include <ai/actor/actormanager.h>
#include <stateprop/statepropdata.hpp>
#include <mission/gameplaymanager.h>
#include <atc/atcmanager.h>
#include <mission/statepropcollectible.h>
#include <render/rendermanager/worldrenderlayer.h>
#include <render/rendermanager/rendermanager.h>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================


//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================


//===========================================================================
// Member Functions
//===========================================================================

DestroyBossObjective::DestroyBossObjective()
{

}

DestroyBossObjective::~DestroyBossObjective()
{

}

void
DestroyBossObjective::SetTarget( Actor* actor )
{
    m_Boss = actor;  
    // Don't let the actor despawn if it goes out of range of the avatar
    rTuneAssertMsg( actor != NULL, "DestroyBossObjective: can't find object in memory!" );
    if( m_Boss != NULL )
    {
        m_Boss->SetShouldDespawn( false );
    }
}


void 
DestroyBossObjective::OnInitialize()
{
    // Register an event listener so that we will know if the ufo has eaten the
    // player's vehicle. This is failure  
    GetEventManager()->AddListener( this, EVENT_BOSS_DESTROYED_PLAYER_CAR );
    // Event indicating boss has been damaged. This is success
    GetEventManager()->AddListener( this, EVENT_BOSS_DAMAGED );
}

void 
DestroyBossObjective::OnFinalize()
{
    // We are done with these listeners.
    GetEventManager()->RemoveListener( this, EVENT_BOSS_DESTROYED_PLAYER_CAR );
    GetEventManager()->RemoveListener( this, EVENT_BOSS_DAMAGED );
}

void 
DestroyBossObjective::OnUpdate( unsigned int elapsedTime )
{

}

void 
DestroyBossObjective::HandleEvent( EventEnum id, void* pEventData )
{
    switch ( id )
    {
    case EVENT_BOSS_DESTROYED_PLAYER_CAR:
        // Boss has destroyed the players can, register failure
        SetFinished( true );
        break;
    case EVENT_BOSS_DAMAGED:
        // Boss has been damaged by the player. This stage has been completed successfully
        SetFinished( true );
        break;

    default:
        rAssert(0);
        break;
    }
}
