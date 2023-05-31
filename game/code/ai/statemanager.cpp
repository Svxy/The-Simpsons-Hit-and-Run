//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        statemanager.cpp
//
// Description: Implementation of class StateManager
//
// History:     6/12/2002 + Created -- NAME
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
#include <ai/statemanager.h>
#include <ai/state.h>
#include <worldsim/character/character.h>
#include <ai/sequencer/actioncontroller.h>

namespace CharacterAi
{

//==============================================================================
StateManager::StateManager( Character* pCharacter )
:
mpCharacter(pCharacter),
mpCurrentState( new NoState(pCharacter) ),
mpNextState( NULL )
{
}

//==============================================================================
StateManager::~StateManager()
{
    delete mpCurrentState;
    delete mpNextState;
}

//==============================================================================
Character* StateManager::GetCharacter()
{
    return mpCharacter;
}
//==============================================================================
CharacterState StateManager::GetState( void ) const
{
	return mpCurrentState->GetStateID();
}

//==============================================================================
void StateManager::SetState( State* state)
{
    if(mpNextState)
    {
        delete mpNextState;
    }

    mpNextState = state;
}

//==============================================================================
void StateManager::ResetState( void )
{
    if(mpNextState)
    {
        delete mpNextState;
        mpNextState = NULL;
    }

    mpCurrentState->Exit( );
    mpCurrentState->Enter( );
}

//==============================================================================
void StateManager::Update( float timeins )
{    
    if(mpNextState)
    {
        mpCurrentState->Exit( );
	    delete mpCurrentState;
        mpCurrentState = mpNextState;
        mpNextState = NULL;
        mpCurrentState->Enter( );
    }

    mpCurrentState->Update( timeins );

    // sequence actions, if we can
    if ( !mpCharacter->GetActionController()->IsBusy())
    {
        mpCurrentState->SequenceAction();
    }
}

//==============================================================================
const State* StateManager::GetCurrentState( void ) const
{
	return mpCurrentState;
}

}; // namespace CharacterAi