//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        state.h
//
// Description: Blahblahblah
//
// History:     6/12/2002 + Created -- NAME
//
//=============================================================================

#ifndef STATE_H
#define STATE_H

//========================================
// Nested Includes
//========================================
#include <memory/memorypool.h>
#include <radmath/radmath.hpp>
#include <events/eventenum.h>
#include <events/eventlistener.h>

//========================================
// Forward References
//========================================
class Character;
class InCarAction;

namespace CharacterAi
{
    
enum CharacterState
{
    NOSTATE,
    LOCO,
    INCAR,
    GET_IN,
    GET_OUT,
    INSIM
};


//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class State
{
public:
	State( Character* pCharacter );
	virtual ~State( void );

	virtual void Enter( void ) = 0;
	virtual void Exit( void ) = 0;

    virtual void SequenceAction( void ) = 0;
	virtual void Update( float timeins ) = 0;
    
	virtual CharacterState GetStateID( void ) const = 0;

    static void* operator new( size_t size ) { return( sMemoryPool.Allocate( size ) ); }
    static void  operator delete( void* deadObject, size_t size ) { sMemoryPool.Free( deadObject, size ); }

    // Declared but not defined to prevent access.
    static void* operator new[]( size_t size );
    static void  operator delete[]( void* pMemory );
protected:

	Character* mpCharacter;

private:
    //Prevent wasteful constructor creation.
    State();
	State( const State& state );
    State& operator=( const State& state );

    static FBMemoryPool sMemoryPool;
};

class NoState
:
public State
{
public:
    NoState(Character* c) : State(c) {}
    virtual ~NoState( void ) {}

    static CharacterState StateID( void ) { return NOSTATE; } 
    virtual CharacterState GetStateID( void ) const { return StateID(); }

protected:
    virtual void Enter( void ) {}
    virtual void Exit( void ) {}

    virtual void SequenceAction( void ) {}
    virtual void Update( float timeins ) {}
};

class InCar
:
public State
{
public:

	InCar( Character* pCharacter );
	virtual ~InCar( void );

    static CharacterState StateID( void ) { return INCAR; } 
    virtual CharacterState GetStateID( void ) const { return StateID(); }

protected:

    virtual void Enter( void );
	virtual void Exit( void );

    virtual void SequenceAction( void );
	virtual void Update( float timeins );

private:
    InCarAction* m_InCarAction;

    enum GetOutState 
    {
        GETOUT_NONE,
        GETOUT_TRYING,
        GETOUT_COMITTED
    };

    bool mIsDriver : 1;
    GetOutState m_GetOutState;
};

class Loco
:
public State
{
public:

	Loco( Character* pCharacter );
	virtual ~Loco( void );

    static CharacterState StateID( void ) { return LOCO; } 
    virtual CharacterState GetStateID( void ) const { return StateID(); }

protected:
    unsigned mLastActionFrame;

    virtual void Enter( void );
	virtual void Exit( void );

    virtual void SequenceAction( void );
	virtual void Update( float timeins );
};

class InSim : public State
{
public:

    InSim( Character* pCharacter );
    virtual ~InSim( void );

    static CharacterState StateID( void ) { return INSIM; } 
    virtual CharacterState GetStateID( void ) const { return StateID(); }

protected:

    virtual void Enter( void );
	virtual void Exit( void );

    virtual void SequenceAction( void );
	virtual void Update( float timeins );
};


class GetIn : public State, public EventListener
{
public:

	GetIn( Character* pCharacter );
	virtual ~GetIn( void );

    static CharacterState StateID( void ) { return GET_IN; } 
    virtual CharacterState GetStateID( void ) const { return StateID(); }

protected:

    virtual void Enter( void );
	virtual void Exit( void );

    virtual void SequenceAction( void );
	virtual void Update( float timeins );

    void HandleEvent( EventEnum id, void* pUserData );

private:
    void CalcGetInPath(int* nWaypoints, rmt::Vector* waypoints, bool* doingSlide, bool* jump);

    bool mObstructed : 1;
    bool mFirst      : 1;
    int  mCollisionFailure : 3;
};

class GetOut : public State, public EventListener
{
public:

	GetOut( Character* pCharacter );
	virtual ~GetOut( void );

    static CharacterState StateID( void ) { return GET_OUT; } 
    virtual CharacterState GetStateID( void ) const { return StateID(); }

protected:
    void DoGetOut(void);

    virtual void Enter( void );
	virtual void Exit( void );

    virtual void SequenceAction( void );
	virtual void Update( float timeins );

    void HandleEvent( EventEnum id, void* pUserData );

    bool mObstructed : 1;
    bool mFirst      : 1;
    bool mPanic      : 1;
};

}; // end of namespace
#endif //STATE_H
