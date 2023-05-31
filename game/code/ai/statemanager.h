//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        statemanager.h
//
// Description: Blahblahblah
//
// History:     6/12/2002 + Created -- NAME
//
//=============================================================================

#ifndef STATEMANAGER_H
#define STATEMANAGER_H

//========================================
// Nested Includes
//========================================
#include <ai/state.h>

//========================================
// Forward References
//========================================
class Character;

namespace CharacterAi
{

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class StateManager
{
public:

    StateManager( Character* pCharacter );
    ~StateManager();

    const Character* GetCharacter() const;
    Character*       GetCharacter();
    const State*     GetCurrentState( void ) const;
    const State*     GetNextState( void ) const { return mpNextState;};

	CharacterState   GetState( void ) const;
    
    template <class STATE> void SetState(void)
    {
        // Do nothing if same as old state.
        if ( (!mpNextState && (mpCurrentState->GetStateID() == STATE::StateID())) ||
             (mpNextState && (mpNextState->GetStateID() == STATE::StateID())) )
        {
            return;
        }

        SetState(new STATE(mpCharacter)); 
    }

	void SetState( State* pState );
    void ResetState( void );

	void Update( float timeins );

private:
    //Prevent wasteful constructor creation.
	StateManager( const StateManager& statemanager );
    StateManager& operator=( const StateManager& statemanager );

    Character* mpCharacter;
	State* mpCurrentState;
	State* mpNextState;
};

template <class STATE> 
void SetState( StateManager* sm )
{
    // Do nothing if same as old state.
    const State* currentState = sm->GetCurrentState();    
    const State* nextState = sm->GetNextState();    
    if ( (!nextState && (currentState->GetStateID() == STATE::StateID())) ||
         (nextState && (nextState->GetStateID() == STATE::StateID())) )
    {
        return;
    }
    {
        Character* character = sm->GetCharacter();
        sm->SetState( new STATE( character ) ); 
    }
}

};
#endif //STATEMANAGER_H
