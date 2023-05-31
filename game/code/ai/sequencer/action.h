#ifndef _SEQUENCER_ACTION_HPP
#define _SEQUENCER_ACTION_HPP
//-----------------------------------------------------------------------------
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// action.hpp
//
// Description: An atomic unit of execution in the Sequencer, the Action class
//              is the base class for all logical actions that operate on
//              a character for movement and animation.
//
// Modification History:
//  + Created Aug 14, 2001 -- Gary Keong
//      - Snapshot from Hair Club (rev 1) Owner: Laurent Ancessi
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------

//========================================
// Nested Includes
//========================================
#include <memory/memorypool.h>
#include <p3d/memory.hpp>

#include <ai/sequencer/task.h>
#include <ai/statemanager.h>
#include <p3d/p3dtypes.hpp>
#include <events/eventenum.h>
#include <events/eventmanager.h>

#include <worldsim/character/charactercontroller.h>
#include <radmath/radmath.hpp>
#include <worldsim/redbrick/vehicle.h>

class tName;

class Action: public Task
{
public:

	virtual ~Action();

	virtual bool IsSlave() const;

	virtual void WakeUp(float time);
	virtual void DoSimulation(float time);
	virtual void Update(float time);
	virtual void Clear();
    virtual void Abort() {};

    // Pool allocators.
    //
    static void* operator new( size_t size ) 
    { 
        void *memPtr = sMemoryPool.Allocate( size );
#ifdef RADLOAD_HEAP_DEBUGGING
	radLoadHeapDebugAddAddress( memPtr );
#endif
        return memPtr; 
    }
    static void  operator delete( void* deadObject, size_t size ) 
    { 
        sMemoryPool.Free( deadObject, size ); 
//This is done in the radObject...
//#ifdef RADLOAD_HEAP_DEBUGGING
//	radLoadHeapDebugRemoveAddress( deadObject );
//#endif
    }

    // Declared but not defined to prevent access.
    static void* operator new[]( size_t size );
    static void  operator delete[]( void* pMemory );

private:
    static FBMemoryPool sMemoryPool;
};

class SlaveAction: public Action
{
public:

	virtual bool IsSlave() const;
};
namespace choreo
{
    class AnimationDriver;
    class RootTransformController;
    class RootDriver;
    class Locomotion;
    class LocomotionDriver;
}

class TriggerEventAction
:
public Action
{
public:
    TriggerEventAction( EventEnum eventId, void* pUserData )
        :
    mEventId( eventId ),
        mpUserData( pUserData )
    {
    }
    virtual void WakeUp( float timeins )
    {
        GetEventManager()->TriggerEvent( mEventId, mpUserData );
        Done( );
    }
private:
    EventEnum mEventId;
    void* mpUserData;
};


class DelayAction
:
public Action
{
public:
    DelayAction( float timeins ) : mTime( timeins) { }
    
    virtual void Update( float timeins )
    {
        mTime -= timeins;
        if(mTime <= 0.0f)
        {
            Done( );
        }
    }

private:
    float mTime;
};

//////////////////////////////////////////////////////////////////////////
//  
// class WalkerLocomotionAction
//
//
//////////////////////////////////////////////////////////////////////////
class WalkerLocomotionAction : public SlaveAction
{
public:
	WalkerLocomotionAction( Character* pCharacter );
	~WalkerLocomotionAction( );
	
    virtual void WakeUp( float timeins );
    virtual void DoSimulation( float timeins);
	virtual void Update( float timeins);
    void PlayDriver(void);
    void StopDriver(void);

    void SetDesiredSpeed( float fSpeed )
    {
        mfDesiredSpeed = fSpeed;
    }

    void SwitchLocomotion(void);

    choreo::LocomotionDriver* GetDriver(void) { return mpLocomotionDriver;}

    void AllowIdle(bool b) { mAllowIdle = b; }

protected:
    static rmt::Randomizer sRandom;
    static bool sRandomSeeded;

    float SolveActualDir( float fDesiredDir, float timeins );
    float SolveActualSpeed( float fDesiredSpeed, float timeins );

    float mfDesiredSpeed;
    float mIdleTime;
    float mNextIdle;
    bool  mAllowIdle : 1;

    Character* mpCharacter;

    choreo::Locomotion* mpLocomotion;
    choreo::LocomotionDriver* mpLocomotionDriver;
};

//////////////////////////////////////////////////////////////////////////
class InCarAction : public Action
{
public:
	InCarAction( Character* pCharacter );
	~InCarAction( );
	
    virtual void WakeUp( float timeins );
    virtual void DoSimulation( float timeins);
	virtual void Update( float timeins);
	virtual void Clear( void );

protected:
    void IWannaRock(bool);

    Character* mpCharacter;
    choreo::AnimationDriver* mpAnimationDriver;
    bool mIsDriver           : 1;
    bool rockinIsMyBusiness  : 1;
    float timeBetweenBeats;
    float timeSinceLastBeat;
    float lastBeatValue;

    float mIdleTime;
};

//////////////////////////////////////////////////////////////////////////
class HoldAnimationAction : public Action
{
public:
	HoldAnimationAction( Character* pCharacter, const char* animName, float holdFrame);
	~HoldAnimationAction( );
	
    virtual void WakeUp( float timeins );
    virtual void DoSimulation( float timeins);
	virtual void Update( float timeins);
	virtual void Clear( void );

protected:
    virtual bool ShouldRelease(void) = 0;

    Character* mpCharacter;
    tUID mAnimUID;
    choreo::AnimationDriver* mpAnimationDriver;
    float mHoldFrame;
    float mOrigFrames;
};

class SteerAction : public HoldAnimationAction
{
public:

    SteerAction( Character* pCharacter, const char* anim, float frame);
	
protected:
    virtual bool ShouldRelease(void);

};

class ReverseAction : public HoldAnimationAction
{
public:
    ReverseAction( Character* pCharacter, const char* anim, float frame);
	
protected:
    virtual bool ShouldRelease(void);
    float mRelease;

};

class AccelAction : public HoldAnimationAction
{
public:
    AccelAction( Character* pCharacter, const char* anim, float frame);
	
protected:
    virtual bool ShouldRelease(void);

};

class DecelAction : public HoldAnimationAction
{
public:
    DecelAction( Character* pCharacter, const char* anim, float frame);
	
protected:
    virtual bool ShouldRelease(void);

};


//////////////////////////////////////////////////////////////////////////
//  
// class JumpAction
//
//
//////////////////////////////////////////////////////////////////////////
class JumpAction
:
public Action
{
public:
    JumpAction( Character* pCharacter, const char* name, float fUpVelocity );
    virtual ~JumpAction( void );
    enum eJumpState
    {
        NoTranslate = 1 << 16,
        InitJump,
        PostTurboJumpRecover,
        IdleJump,
        
        AllowTranslate = 1 << 17,
        PreJump,
        Jump,
        PostJump,
        JumpDone
    };
    virtual void WakeUp( float timeins );
    float Reset( float fJumpHeight, bool bFalling = false );
    void Reset( const rmt::Vector& jumpTarget, bool bFalling = false );
    virtual void DoSimulation( float timeins );
    virtual void Update( float timeins ); 
    void TestForJumpAgain( const rmt::Vector& currentPos, float fUpVelocity, rmt::Vector& groundPos );
    virtual void Clear( );
    
    void SetRootTransform( void );
    void SetRootPosition( const rmt::Vector& position );
    bool IsInJump( void )
    { 
        return mJumpState == Jump 
            // if we are in the PreJump state with no animation,
            // then we are as good as jumping.
            // had to do it for the launching platforms.
            // Bounce.
            //
            || ( mJumpState == PreJump && mpAnimationDriver == 0 )
            ||  ( mJumpState == PreJump && mbJumpAgain );
    }
    
    bool IsJumpState( eJumpState state )
    {
        return ( state == mJumpState );
    }
    const rmt::Vector& GetJumpVelocity( void ) const
    {
        return mVelocity;
    }

    float GetGravity( void ) const
    {
        return mfGravity;
    }
    
protected:
    void SetJumpVelocity( float fVelocity )
    {
        mVelocity.y = fVelocity;
    }
    float SolveActualDir( float fDesiredDir, float timeins );
    float SolveActualSpeed( float fDesiredSpeed, float timeins );
private:

    Character* mpCharacter;
    eJumpState mJumpState;
    rmt::Vector mVelocity;
    tUID mAnimUid;
    float mfTime;
    float mfGravity;
    float mfStartHeight;
    float mfJumpHeight;
    float mfMaxSpeed;
    choreo::AnimationDriver* mpAnimationDriver;
    choreo::RootTransformController* mpRootController;
    choreo::RootDriver* mpRootDriver;
    bool mbBoost      : 1;
    bool mbFalling    : 1;
    bool mbJumpAgain  : 1;
    bool mbTurboJump  : 1;
    bool mbInJumpKick : 1;
    bool mbDoJumpKick : 1;
    bool mbSlam       : 1;
    bool mbPreSlam    : 1;
};

//////////////////////////////////////////////////////////////////////////
//  
// class DodgeAction
//
//
//////////////////////////////////////////////////////////////////////////

class DodgeAction
: 
public Action
{

public:
    DodgeAction( Character* pCharacter );

    virtual void WakeUp( float timeins );
    virtual void DoSimulation( float timeins);
	virtual void Update( float timeins);
    virtual void Clear();

protected:
    float SolveActualDir( float fDesiredDir, float timeins );
    float SolveActualSpeed( float fDesiredSpeed, float timeins );

    choreo::AnimationDriver* mpAnimationDriver; // stores handle for dodge animation
    Character* mpCharacter;
};


//////////////////////////////////////////////////////////////////////////
//  
// class CringeAction
//
//
//////////////////////////////////////////////////////////////////////////

class CringeAction
: 
public Action
{

public:
    CringeAction( Character* pCharacter );

    virtual void WakeUp( float timeins );
    virtual void DoSimulation( float timeins);
	virtual void Update( float timeins);
    virtual void Clear();

protected:
    float SolveActualDir( float fDesiredDir, float timeins );
    float SolveActualSpeed( float fDesiredSpeed, float timeins );
    choreo::AnimationDriver* mpAnimationDriver; // stores handle for dodge animation
    Character* mpCharacter;
};

//////////////////////////////////////////////////////////////////////////
//  
// class FlailAction
//
//
//////////////////////////////////////////////////////////////////////////

class FlailAction
: 
public Action
{

public:
    FlailAction( Character* pCharacter );

    virtual void WakeUp( float timeins );
    virtual void DoSimulation( float timeins);
	virtual void Update( float timeins);
	virtual void Clear();


protected:
    float SolveActualDir( float fDesiredDir, float timeins );
    float SolveActualSpeed( float fDesiredSpeed, float timeins );

    choreo::AnimationDriver* mpAnimationDriver; // stores handle for dodge animation
    Character* mpCharacter;
};


//////////////////////////////////////////////////////////////////////////
//  
// class GetUpAction
//
//
//////////////////////////////////////////////////////////////////////////

class GetUpAction
: 
public Action
{

public:
    GetUpAction( Character* pCharacter );

    virtual void WakeUp( float timeins );
    virtual void DoSimulation( float timeins);
	virtual void Update( float timeins);
    virtual void Clear();

protected:
    float SolveActualDir( float fDesiredDir, float timeins );
    float SolveActualSpeed( float fDesiredSpeed, float timeins );
    choreo::AnimationDriver* mpAnimationDriver; // stores handle for dodge animation
    Character* mpCharacter;
};


//////////////////////////////////////////////////////////////////////////
//  
// class PlayAnimationAction
//
//
//////////////////////////////////////////////////////////////////////////

class PlayAnimationAction: public Action
{
public:
	PlayAnimationAction( Character* pCharacter, const char* anim, float fFrameRate = 30.0f );
    PlayAnimationAction( Character* pCharacter, const tName& anim, float fFrameRate = 30.0f );

    virtual void WakeUp( float timeins );
    virtual void DoSimulation( float timeins );
    virtual void Update( float timeins );
    virtual void Clear( void );
    void AbortWhenMovementOccurs( bool abort );
    void SetLoop(bool b) { loop = b;}

private:
    PlayAnimationAction( void );

protected:
    virtual bool ShouldAbort(void);

	Character* mpCharacter;
	tUID mAnimUID;
    float mfFrameRate;
    choreo::AnimationDriver* mpAnimationDriver;
    bool mbAbortWhenMovementOccurs : 1;
    bool loop                      : 1;
};

//////////////////////////////////////////////////////////////////////////
//  
// class PlayIdleAnimationAction
//
//
//////////////////////////////////////////////////////////////////////////

class PlayIdleAnimationAction: public PlayAnimationAction
{
public:
	PlayIdleAnimationAction( Character* pCharacter, const char* anim, float fFrameRate = 30.0f );
    PlayIdleAnimationAction( Character* pCharacter, const tName& anim, float fFrameRate = 30.0f );

    virtual void WakeUp( float timeins );
    virtual void Clear( void );
    virtual void Abort( void );

private:
    PlayIdleAnimationAction( void );

protected:
};



//////////////////////////////////////////////////////////////////////////
//  
// class CarDoorAction
//
// Action for opening and closing car doors
//
//////////////////////////////////////////////////////////////////////////

class Sequencer;

class CarDoorAction: public Action
{
public:
    CarDoorAction( Vehicle* vehicle, Vehicle::DoorAction, Vehicle::Door, float delay, float timeins, Character* character = NULL, Sequencer* seq = NULL);
    
    virtual void WakeUp( float timeins );
    virtual void DoSimulation( float timeins );
    virtual void Update( float timeins );
    virtual void Clear( void );

private:
    CarDoorAction( void );

    Vehicle* mpVehicle;
    Vehicle::DoorAction mAction;
    Vehicle::Door mDoor;
    float mTime;
    float mDelay;
    float mCurrentTime;

    Character* character;
    Sequencer* sequencer;
};

class ReleaseDoorsAction: public Action
{
public:
    ReleaseDoorsAction( Vehicle* v ) : mVehicle(v) {};
    virtual void Update( float timeins );

protected:
    Vehicle* mVehicle;
};

//////////////////////////////////////////////////////////////////////////
//  
// class Arrive
//
//
//////////////////////////////////////////////////////////////////////////
class Arrive
:
public Action
{
public:
    Arrive( Character* pCharacter, rmt::Vector& destination, bool strict = false );
    virtual ~Arrive( void );
    virtual void WakeUp( float timeins );
    virtual void DoSimulation( float timeins );
    virtual void Update( float timeins );
    virtual void Clear( );
protected:
private:
    Character* mpCharacter;
    rmt::Vector mDestination;
    float mfDecelTime;
    float mfMaxSpeed;
    float mEstTime;
    float mElapsedTime;
    bool mbSolveState   : 1;
    bool mbPrevLocoMode : 1;
    bool mStrict : 1;
};

class FragileArrive : public Arrive
{
public:
    FragileArrive( Character* pCharacter, rmt::Vector& destination, bool strict = false ) :
       Arrive( pCharacter, destination, strict ) {}
    bool IsSlave() const { return true; }
};

//////////////////////////////////////////////////////////////////////////
//  
// class Orient
//
//
//////////////////////////////////////////////////////////////////////////
class Orient
:
public Action
{
public:
    Orient( Character* pCharacter, rmt::Vector& facing );
    virtual ~Orient( void );

    virtual void WakeUp( float timeins );
    virtual void DoSimulation( float timeins );
    virtual void Update( float timeins );
protected:
private:
    Character* mpCharacter;
    float mfTarget;
};
//////////////////////////////////////////////////////////////////////////
//  
// class Position
//
//
//////////////////////////////////////////////////////////////////////////
class Position
:
public Action
{
public:
    Position( Character* pCharacter, rmt::Vector& position, float fDuration, bool local = false );
    virtual ~Position( void );

    virtual void WakeUp( float timeins );
    virtual void DoSimulation( float timeins );
    virtual void Update( float timeins );
protected:
private:
    Character* mpCharacter;
    rmt::Vector mDestination;
    float mfDuration;
    float mfTimeLeft;
    bool mLocal : 1;
};

class GroundSnap
:
public Action
{
public:
    GroundSnap( Character* pCharacter) : mpCharacter( pCharacter ) {}
    virtual ~GroundSnap( void ) {}

    virtual void WakeUp( float timeins );
    virtual void DoSimulation( float timeins ) {}
    virtual void Update( float timeins ) {}
protected:
private:
    Character* mpCharacter;
};

//////////////////////////////////////////////////////////////////////////
//  
// class AssignValueToFloat
//
//
//////////////////////////////////////////////////////////////////////////
class AssignValueToFloat
:
public Action
{
public:
    AssignValueToFloat( float& refFloat, float fValue )
        :
    mRefFloat( refFloat ),
        mfValue( fValue )
    {
    }
    virtual void WakeUp( float timeins )
    {
        mRefFloat = mfValue;
        Done( );
    }
private:
    float& mRefFloat;
    float mfValue;
};


class ChangeLocomotion
:
public Action
{
public:
    enum LocoType  { INCAR, WALKING };

    ChangeLocomotion( Character* c, LocoType type) : mpCharacter(c), mType(type) { }
    
    virtual void Update( float timeins );

private:
    Character* mpCharacter;
    LocoType mType;
};


class ChangeNPCControllerState
:
public Action
{
public:
    ChangeNPCControllerState( Character* c, NPCController::State state) : mpCharacter(c), mState(state) { }
    virtual void Update( float timeins );
    virtual void Abort( void );
private:
    Character* mpCharacter;
    NPCController::State mState;
};


class KickAction
:
public SlaveAction
{
public:
    KickAction( Character* c, float contact) : mpCharacter(c), mTime(0.0f), mContact(contact) { }
    
    virtual void Update( float timeins );

private:
    Character* mpCharacter;
    float mTime;
    float mContact;
};

class SurfAction : public PlayAnimationAction
{
public:
    SurfAction(Character*);

protected:
    unsigned mLowCount;
    bool ShouldAbort(void);
    void Update(float);
};

template <class STATE> class ChangeState
:
public Action
{
public:
    ChangeState( Character* c) : mpCharacter(c) { }
    
    virtual void Update( float timeins )
    {
        CharacterAi::SetState<STATE>( mpCharacter->GetStateManager() );
        Done();
    }

private:
    Character* mpCharacter;
};


#endif // HC_SEQUENCER_ACTION_HPP
