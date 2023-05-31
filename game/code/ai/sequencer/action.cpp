//-----------------------------------------------------------------------------
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// action.cpp
//
// Description: An atomic unit of execution in the Sequencer, the Action class
//              is the base class for all logical actions that operate on
//              a character for movement and animation.
//
// Modification History:
//  + Created Aug 14, 2001 -- Gary Keong
//      - Snapshot from Hair Club (rev 4) Owner: Laurent Ancessi
//-----------------------------------------------------------------------------

#include <ai/sequencer/action.h>

#include <ai/sequencer/actioncontroller.h>

// Needed for particle animation to be played upon impact after jumping
#include <render/particles/particlemanager.h>

#include <events/eventmanager.h>
#include <presentation/presentation.h>
#include <main/game.h>
#include <radtime.hpp>

#include <input/inputmanager.h>
#ifdef RAD_WIN32
#include <input/usercontrollerWin32.h>
#else
#include <input/usercontroller.h>
#endif

#include <worldsim/character/charactermanager.h>
#include <worldsim/coins/sparkle.h>

#include <sound/soundmanager.h>

#include <worldsim/character/character.h>
#include <choreo/utility.hpp>
#include <choreo/puppet.hpp>
#include <radmath/radmath.hpp>
#include <interiors/interiormanager.h>

inline bool AnyStickMovement(Character* character, float tol = 0.0f)
{
#ifdef RAD_WIN32
    float mright = character->GetController()->GetValue(CharacterController::MouseLookRight);
    float mleft = character->GetController()->GetValue(CharacterController::MouseLookLeft);
    float mouselook = ( mright > mleft ) ? mright : -mleft;
#endif

    return (rmt::Abs(character->GetController()->GetValue(CharacterController::LeftStickX)) > tol) ||
           (rmt::Abs(character->GetController()->GetValue(CharacterController::LeftStickY)) > tol) ||
#ifdef RAD_WIN32
           (
               GetInputManager()->GetController(0)->IsMouseLookOn() && 
               rmt::Abs(mouselook) > tol
           ) ||
#endif
           (character->GetController()->GetValue(CharacterController::DPadUp)) ||
           (character->GetController()->GetValue(CharacterController::DPadDown)) ||
           (character->GetController()->GetValue(CharacterController::DPadLeft)) ||
           (character->GetController()->GetValue(CharacterController::DPadRight));
}

//---------------------------------------------------------------------
// class Action
//---------------------------------------------------------------------
const int MAX_CLASS_SIZE = 128; // bytes.
const float JUMP_ACTION_SPEED_CLAMP = 25.0f;
FBMemoryPool Action::sMemoryPool( MAX_CLASS_SIZE, 32, GMA_LEVEL_OTHER );

Action::~Action() 
{
}

bool Action::IsSlave() const
{
    // by default, actions are NOT slaves
    return false;
}

void Action::WakeUp(float time)
{
}

void Action::DoSimulation(float time)
{
}

void Action::Update(float time)
{
}

void Action::Clear()
{
}

//---------------------------------------------------------------------
// class CSlaveAction
//---------------------------------------------------------------------

bool SlaveAction::IsSlave() const
{
    // descendents of CSlaveAction are slaves
    return true;
}


/*
==============================================================================
WalkerLocomotionAction::WalkerLocomotionAction
==============================================================================
Description:    Comment

Parameters:     (Character* pCharacter, float duration)

Return:         WalkerLocomotionAction

=============================================================================
*/
rmt::Randomizer WalkerLocomotionAction::sRandom(0);
bool WalkerLocomotionAction::sRandomSeeded = false;

WalkerLocomotionAction::WalkerLocomotionAction( Character* pCharacter ) :
mfDesiredSpeed( 0.0f ),
mIdleTime(0.0f),
mNextIdle(15.0f),
mAllowIdle(false),
mpCharacter(pCharacter),
mpLocomotion( 0 ),
mpLocomotionDriver( 0 )
{
    mNextIdle = (sRandom.Float() * 10.0f) + 10.0f;

    SwitchLocomotion();

    if (!sRandomSeeded)
    {
        sRandom.Seed (Game::GetRandomSeed ());
        sRandomSeeded = true;
    }
}

WalkerLocomotionAction::~WalkerLocomotionAction( )
{
    tRefCounted::Release( mpLocomotion );
    tRefCounted::Release( mpLocomotionDriver );
}

void WalkerLocomotionAction::PlayDriver( void )
{
    mpCharacter->GetPuppet()->PlayDriver( mpLocomotionDriver, -1.0f, false );
}

void WalkerLocomotionAction::StopDriver( void )
{
    mpCharacter->GetPuppet()->StopDriver( mpLocomotionDriver);
}

void WalkerLocomotionAction::SwitchLocomotion( void )
{
    tRefCounted::Assign(mpLocomotion, choreo::find<choreo::Locomotion>( mpCharacter->GetPuppet()->GetBank(), tEntity::MakeUID("walkerLoco")));
    rAssert(mpLocomotion );

    tRefCounted::Assign(mpLocomotionDriver, mpLocomotion->NewLocomotionDriver());
    rAssert(mpLocomotionDriver);
}

/*
==============================================================================
WalkerLocomotionAction::SolveActualDir
==============================================================================
Description:    Comment

Parameters:     ( float fDesiredDir, float timeins )

Return:         float 

=============================================================================
*/
float WalkerLocomotionAction::SolveActualDir( float fDesiredDir, float timeins )
{
    // Damping function.
    //
    float rotateDelta = choreo::GetSmallestArc( mpCharacter->GetFacingDir( ), fDesiredDir );
    
    float newRatio;
    if ( mpCharacter->IsTurbo() )
    {
        newRatio = CharacterTune::sfTurboRotateRate * timeins;
    }
    else
    {
        newRatio = CharacterTune::sfLocoRotateRate * timeins;
    }
    float oldRatio = 1.0f - newRatio;
    float newDir = ( mpCharacter->GetFacingDir( ) * oldRatio ) + ( ( mpCharacter->GetFacingDir( ) + rotateDelta ) * newRatio );

    return newDir;
}

/*
==============================================================================
WalkerLocomotionAction::SolveActualSpeed
==============================================================================
Description:    Comment

Parameters:     ( float fDesiredSpeed, float timeins )

Return:         float 

=============================================================================
*/
float WalkerLocomotionAction::SolveActualSpeed( float fInputSpeed, float timeins )
{
    float v = 0.0f;
    if ( fInputSpeed == mfDesiredSpeed )
    {
        return fInputSpeed;
    }
    // Damping function.
    //
    else if ( fInputSpeed > mfDesiredSpeed )
    {
        v = mfDesiredSpeed + CharacterTune::sfLocoAcceleration * timeins;
        return v > fInputSpeed ? fInputSpeed : v;
    }
    else
    {
        v = mfDesiredSpeed + CharacterTune::sfLocoDecceleration * timeins;
        return v < fInputSpeed ? fInputSpeed : v;
    }
}
/*
==============================================================================
WalkerLocomotionAction::WakeUp
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void WalkerLocomotionAction::WakeUp( float timeins )
{
}
/*
==============================================================================
WalkerLocomotionAction::DoSimulation
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void WalkerLocomotionAction::DoSimulation( float timeins)
{
    choreo::Puppet* pPuppet = mpCharacter->GetPuppet();
    rAssert( pPuppet );
    float fSpeed = 0.0f;
    rmt::Vector dir;
    mpCharacter->GetDesiredFacing( dir );

    // Only update the direction and speed if the 
    // controller has a direction input.
    //
    const float NO_DIRECTION = 0.0000001f;
    if ( dir.MagnitudeSqr( ) > NO_DIRECTION )
    {
        float fDesiredDir = choreo::GetWorldAngle( dir.x, dir.z );
        float fDir = SolveActualDir( fDesiredDir, timeins );
        mpCharacter->SetFacingDir( fDir );

        float fInputSpeed = mpCharacter->GetDesiredSpeed();
        
        if ( mpCharacter->IsTurbo() && fInputSpeed > 0.0f )
        {
            mfDesiredSpeed = mpCharacter->GetMaxSpeed();
        }
        else
        {
            mfDesiredSpeed = SolveActualSpeed( fInputSpeed, timeins );

            // Snap to MaxSpeed.  Don't decel out of turbo.
            //
            if ( mfDesiredSpeed > mpCharacter->GetMaxSpeed() )
            {
                mfDesiredSpeed = mpCharacter->GetMaxSpeed();
            }
        }
    }
    // Method A the motion direction matches the facing direction.
    // Quick turns at full speed result in a small U turn.
    // 
    // Method B the motion direction matches the desired direction.
    // Smoother 180 turns, but left right motion is jerkier.
    //  [6/21/2002]
    //
    if ( CharacterTune::bLocoTest || mpCharacter->IsTurbo() )
    {
        mpLocomotionDriver->SetDesiredMotionAngle( mpCharacter->GetFacingDir( ) );
        mpLocomotionDriver->SetDesiredFacingAngle( mpCharacter->GetFacingDir( ) );
        mpLocomotionDriver->SetDesiredVelocity( mfDesiredSpeed );
    }
    else
    {
        mpLocomotionDriver->SetDesiredMotionAngle( mpCharacter->GetDesiredDir( ) );
        mpLocomotionDriver->SetDesiredFacingAngle( mpCharacter->GetFacingDir( ) );
        mpLocomotionDriver->SetDesiredVelocity( mfDesiredSpeed );
    }
}
/*
==============================================================================
WalkerLocomotionAction::Update
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void WalkerLocomotionAction::Update( float timeins)
{
    rmt::Vector outnorm;
    rmt::Vector outposition;

    if( (mpCharacter->GetLocoVelocity().Magnitude() > 0.0f) || !mAllowIdle)
    {
        mIdleTime = 0.0f;
    }
    else
    {
        mIdleTime += timeins;
    }

    if(PresentationManager::GetInstance()->InConversation())
    {
        mIdleTime = 0.0f;
    }

    if((mIdleTime > mNextIdle) && (mpCharacter == GetCharacterManager()->GetCharacter(0)))
    {
        static int s_idleProbabilityTable[10] = { 0,0,0,0,0,1,1,1,2,2 };
        static int s_lastIdle = 0;
        int idle = s_idleProbabilityTable[sRandom.IntRanged(9)];

        if((s_lastIdle != 0) && (s_lastIdle == idle))
        {
            idle = 0;
        }
        s_lastIdle = idle;

        if(!(GetInteriorManager()->IsEntering() || GetInteriorManager()->IsExiting()))
        {
            char animName[64];
            sprintf(animName, "%s_idle%d", GetCharacterManager()->GetModelName(mpCharacter), idle);

            Sequencer* seq = mpCharacter->GetActionController()->GetNextSequencer();
            seq->BeginSequence();
            PlayIdleAnimationAction* a = new PlayIdleAnimationAction(mpCharacter,animName);
            a->AbortWhenMovementOccurs(true);
            seq->AddAction(a);
            seq->EndSequence();
        }

        mIdleTime = 0.0f;
        mNextIdle = (sRandom.Float() * 10.0f) + 10.0f;

        GetEventManager()->TriggerEvent( EVENT_PLAY_IDLE_MUSIC );
    }

    CharacterController* c = mpCharacter->GetController();
    rmt::Vector v;
    mpCharacter->GetVelocity(v);
    float velocity = v.Magnitude();
    if(c && mpCharacter->mbSurfing && 
        (c->GetIntention() == CharacterController::NONE) &&
        !AnyStickMovement(mpCharacter) &&
        (mpCharacter->GetLocoVelocity().Magnitude() < 0.1f) &&
        (velocity > 10.0f))
    {
        mpCharacter->GetActionController()->SequenceSingleAction(new PlayAnimationAction(mpCharacter, "surf_in"));
        mpCharacter->GetActionController()->SequenceSingleAction(new SurfAction(mpCharacter));
        mpCharacter->GetActionController()->SequenceSingleAction(new PlayAnimationAction(mpCharacter, "surf_out"));
    }
}

/*
==============================================================================
InCarAction::InCarAction
==============================================================================
Description:    Comment

Parameters:     (Character* pCharacter, float duration)

Return:         WalkerAction

=============================================================================
*/
InCarAction::InCarAction( Character* pCharacter )
:
mpCharacter( pCharacter ),
mpAnimationDriver (NULL),
rockinIsMyBusiness (false),
timeBetweenBeats(0.0f),
timeSinceLastBeat(0.0f),
lastBeatValue(0.0f),
mIdleTime(0.0f)
{
}

InCarAction::~InCarAction( )
{
    tRefCounted::Release(mpAnimationDriver);
}


/*
==============================================================================
InCarAction::WakeUp
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void InCarAction::WakeUp( float timeins )
{
    mIsDriver = (mpCharacter->GetTargetVehicle()->mpDriver == mpCharacter) || (mpCharacter->GetTargetVehicle()->mpDriver  == NULL);
    mIdleTime = 0.0f;
    IWannaRock(false);
}

void InCarAction::IWannaRock(bool whattaYouWannaDoWithYourLife)
{
    rockinIsMyBusiness = whattaYouWannaDoWithYourLife;

    if(mpAnimationDriver)
    {
        mpCharacter->GetPuppet()->StopDriver( mpAnimationDriver );
        tRefCounted::Release(mpAnimationDriver);
    }

    choreo::Puppet* pPuppet = mpCharacter->GetPuppet();
    rAssert( pPuppet );

    choreo::Animation* anim;

    if(rockinIsMyBusiness)
    {
        anim = choreo::find<choreo::Animation>( pPuppet->GetBank(), mIsDriver ? "in_car_victory_small_driver" : "in_car_victory_small" );
    }
    else
    {
        anim = choreo::find<choreo::Animation>( pPuppet->GetBank(), mIsDriver ? "in_car_idle_driver" : "in_car_idle" );
    }

    if (anim == 0)
    {
        Done();
        return;
    }
    
    choreo::AnimationDriver* animDriver = anim->NewAnimationDriver();
    tRefCounted::Assign(mpAnimationDriver, animDriver);

    animDriver->SetWeight(1.0f);
    animDriver->SetPriority(0);
    animDriver->SetIsCyclic(true);
    if(rockinIsMyBusiness && (timeBetweenBeats != 0.0f))
    {
        animDriver->SetSpeed((46.0f / 30.0f) / timeBetweenBeats);
        float biasedBeat = (lastBeatValue + 1.0f) - (12.0f / 46.0f);
        float frame = ((biasedBeat) - (float)(int)(biasedBeat)) * 46.0f;
        animDriver->SetFrame(frame);
    }
    else
    {
        animDriver->SetSpeed(1.0f);
    }

        
    pPuppet->PlayDriver( animDriver, -1.0f );
}

/*
==============================================================================
InCarAction::DoSimulation
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void InCarAction::DoSimulation( float timeins)
{
    mpCharacter->SetDesiredDir( rmt::PI );
    mpCharacter->SetFacingDir( rmt::PI );
    mpCharacter->SetDesiredSpeed( 0.0f );
    mpCharacter->SetSpeed( 0.0f );
}
/*
==============================================================================
InCarAction::Update
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void InCarAction::Update( float timeins)
{
    timeSinceLastBeat += timeins;

    float beat;
    
    if( CommandLineOptions::Get( CLO_MUTE ) )
        beat = 0.0f;
    else
        beat = GetSoundManager()->GetBeatValue();
    
    if(int(beat) != int(lastBeatValue))
    {
        timeBetweenBeats = timeSinceLastBeat;
        timeSinceLastBeat = 0.0f;
        if(rockinIsMyBusiness)
        {
            mpAnimationDriver->SetFrame(12.0f);
        }
    }

    lastBeatValue = beat;

    if((mIdleTime < 3.0f) && ((mIdleTime + timeins) > 3.0f))
    {
        if(mpCharacter->GetRockinIdle())
        {
            IWannaRock(true);
        }
    }

    mIdleTime += timeins;
}

void InCarAction::Clear()
{
    mpCharacter->GetPuppet()->StopDriver( mpAnimationDriver );
    tRefCounted::Release(mpAnimationDriver);
}


/*
==============================================================================
HoldAnimationAction

A animation that hold on a specified frame untul the condition in 
"ShouldRelease" is met.
==============================================================================
*/

HoldAnimationAction::HoldAnimationAction( Character* pCharacter, const char* animName, float hold)
:
mpCharacter( pCharacter ),
mpAnimationDriver(NULL),
mHoldFrame(hold)
{
    mAnimUID = tEntity::MakeUID(animName);

}

HoldAnimationAction::~HoldAnimationAction( )
{
    tRefCounted::Release(mpAnimationDriver);
}

void HoldAnimationAction::WakeUp( float timeins )
{
    choreo::Puppet* pPuppet = mpCharacter->GetPuppet();
    rAssert( pPuppet );

    choreo::Animation* anim = choreo::find<choreo::Animation>( pPuppet->GetBank(), mAnimUID );

    if (anim == 0)
    {
        Done();
        return;
    }
    
    choreo::AnimationDriver* animDriver = anim->NewAnimationDriver();
    tRefCounted::Assign(mpAnimationDriver, animDriver);

    animDriver->SetWeight(1.0f);
    animDriver->SetPriority(0);
    animDriver->SetIsCyclic(false);
    mOrigFrames = animDriver->GetEndFrame();
    animDriver->SetEndFrame(mHoldFrame);
    animDriver->SetHoldEndFrame(true);

    pPuppet->PlayDriver( mpAnimationDriver, -1.0f );
}

void HoldAnimationAction::DoSimulation( float timeins)
{
    mpCharacter->SetDesiredDir( rmt::PI );
    mpCharacter->SetFacingDir( rmt::PI );
    mpCharacter->SetDesiredSpeed( 0.0f );
    mpCharacter->SetSpeed( 0.0f );
}

void HoldAnimationAction::Update( float timeins)
{
    if(ShouldRelease())
    {
        mpAnimationDriver->SetEndFrame(mOrigFrames);
        mpAnimationDriver->SetHoldEndFrame(false);
    }


    if(mpAnimationDriver->IsFinished())
    {
        Done();
    }
}

void HoldAnimationAction::Clear()
{
    mpCharacter->GetPuppet()->StopDriver( mpAnimationDriver );
    tRefCounted::Release(mpAnimationDriver);
}

SteerAction::SteerAction( Character* pCharacter, const char* anim, float frame) :
  HoldAnimationAction(pCharacter, anim, frame)
{
}

bool SteerAction::ShouldRelease(void)
{
   if(!mpCharacter->GetTargetVehicle())
   {
       return true;
   }

   float steer = GetCharacterManager()->GetCharacter(0)->GetController()->GetValue(CharacterController::LeftStickX);
   return (rmt::Abs(steer) < 0.1f) || (mpCharacter->GetTargetVehicle()->IsInReverse()); 
;
}

ReverseAction::ReverseAction( Character* pCharacter, const char* anim, float frame) :
  HoldAnimationAction(pCharacter, anim, frame)
{
    mRelease = 0.0f;
}

bool ReverseAction::ShouldRelease(void)
{
   if(!mpCharacter->GetTargetVehicle())
   {
       return true;
   }

   mRelease = (mRelease * 0.9f) + (mpCharacter->GetTargetVehicle()->IsInReverse() ? 0.0f : 0.1f);
   return mRelease > 0.9f;
}

AccelAction::AccelAction( Character* pCharacter, const char* anim, float frame) :
  HoldAnimationAction(pCharacter, anim, frame)
{
}

bool AccelAction::ShouldRelease(void)
{
   if(!mpCharacter->GetTargetVehicle())
   {
       return true;
   }

   return mpCharacter->GetTargetVehicle()->GetAccelMss() < 7.0f;
}

DecelAction::DecelAction( Character* pCharacter, const char* anim, float frame) :
  HoldAnimationAction(pCharacter, anim, frame)
{
}

bool DecelAction::ShouldRelease(void)
{
   if(!mpCharacter->GetTargetVehicle())
   {
       return true;
   }

   return (mpCharacter->GetTargetVehicle()->GetAccelMss() > -9.0f) || 
           (mpCharacter->GetTargetVehicle()->GetSpeedKmh() < 1.0f) ||
           (mpCharacter->GetTargetVehicle()->IsInReverse()); 
}

/*
==============================================================================
Arrive::Arrive
==============================================================================
Description:    Comment

 Parameters:  ( Character* pCharacter, rmt::Vector& destination )
 
Return:         Arrive

=============================================================================
*/

Arrive::Arrive( Character* pCharacter, rmt::Vector& destination, bool strict )
:
mpCharacter( pCharacter ),
mDestination( destination ),
mfDecelTime( 0.1f ),
mfMaxSpeed( 1.0f ),
mbPrevLocoMode( CharacterTune::bLocoTest ),
mStrict(strict)
{
    // work in 2d.
    //
    mDestination.y = 0.0f;
    CharacterTune::bLocoTest = false;
};

/*
==============================================================================
Arrive::~Arrive
==============================================================================
Description:    Comment

 Parameters:   ( void )
 
Return:         Arrive

=============================================================================
*/
Arrive::~Arrive( void )
{
}
/*
==============================================================================
Arrive::WakeUp
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void Arrive::WakeUp( float timeins )
{
    rmt::Vector myPos;
    mpCharacter->GetPosition( myPos );
    myPos.y = 0.0f;
    rmt::Vector vectorBetween;
    vectorBetween.Sub( mDestination, myPos );
    float dist = vectorBetween.Magnitude();

    // figure out a resonable (conservative) time for how long it should take us to get there
    mEstTime = ((dist / CharacterTune::sfMaxSpeed) * 1.5f) + timeins;
    mElapsedTime = 0;

    mbSolveState = mpCharacter->GetSolveCollisions();
//    mpCharacter->SetSolveCollisions( false );
    mpCharacter->SetTurbo(false);
}
/*
==============================================================================
Arrive::DoSimulation
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void Arrive::DoSimulation( float timeins )
{
    mElapsedTime += timeins;

    if(mElapsedTime > mEstTime)
    {
        // taking too long, we probably got stuck, teleport
        rmt::Vector position;
        mpCharacter->GetPosition( position );
        mDestination.y = position.y;
        mpCharacter->SetPosition( mDestination );
        mpCharacter->SetDesiredSpeed( 0.0f );
        mpCharacter->ResetSpeed( );
        Done( );
        return;
    }

    rmt::Vector desiredVelocity( 0.0f, 0.0f, 0.0f );

    rmt::Vector myPos;
    mpCharacter->GetPosition( myPos );
    myPos.y = 0.0f;
    rmt::Vector vectorBetween;
    vectorBetween.Sub( mDestination, myPos );
    float dist = vectorBetween.Magnitude();
    if ( dist > 0.05f )
    {
        float speed = dist / mfDecelTime;

        float clipped_speed = rmt::Min( speed, CharacterTune::sfMaxSpeed );
        desiredVelocity.Sub( mDestination, myPos );
        desiredVelocity.Normalize();
        desiredVelocity.Scale( clipped_speed );

        //rmt::Vector velocity;
        //mpCharacter->GetVelocity( velocity );
        //desiredVelocity.Sub( velocity );
    
        // desiredVelocity is in m/s
        mpCharacter->SetDesiredSpeed( desiredVelocity.Magnitude( ));
        mpCharacter->SetDesiredDir( choreo::GetWorldAngle( desiredVelocity.x, desiredVelocity.z ) );
    }
    else
    {
        if(mStrict)
        {
            rmt::Vector position;
            mpCharacter->GetPosition( position );
            mDestination.y = position.y;
            mpCharacter->SetPosition( mDestination );
        }

        mpCharacter->SetDesiredSpeed( 0.0f );
        mpCharacter->ResetSpeed( );
        Done( );
    }
}

/*
==============================================================================
Arrive::Update
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void Arrive::Update( float timeins )
{
}

void Arrive::Clear( void )
{
    CharacterTune::bLocoTest = mbPrevLocoMode;
    mpCharacter->SetSolveCollisions( mbSolveState );
}
/*
==============================================================================
Orient::Orient
==============================================================================
Description:    Comment

 Parameters:  ( Character* pCharacter, rmt::Vector& facing )
 
Return:         Orient

=============================================================================
*/
Orient::Orient( Character* pCharacter, rmt::Vector& facing )
:
mpCharacter( pCharacter )
{
    rmt::Matrix worldToLocal = mpCharacter->GetInverseParentTransform();
    worldToLocal.RotateVector( facing, &facing );
    mfTarget = choreo::GetWorldAngle( facing.x, facing.z );
}


/*
==============================================================================
Orient::~Orient
==============================================================================
Description:    Comment

 Parameters:   ( void )
 
Return:         Orient

=============================================================================
*/
Orient::~Orient( void )
{
}

void Orient::WakeUp( float timeins )
{
    mpCharacter->SetDesiredDir( mfTarget );
}
/*
==============================================================================
Orient::DoSimulation
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void Orient::DoSimulation( float timeins )
{
    mfTarget = mpCharacter->GetDesiredDir();
}

/*
==============================================================================
Orient::Update
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void Orient::Update( float timeins )
{
    float fTolerance = 0.1f;
    float fFacingDir = mpCharacter->GetFacingDir();
    
    // We must do two compares because if the value of mfTarget is 
    // slightly less than 2PI and the actual facing dir is close to 0.0f
    // the first compare will return false.
    //
    if ( rmt::Epsilon( fFacingDir, mfTarget, fTolerance ) 
        || rmt::Epsilon( fFacingDir, rmt::Fabs( mfTarget - rmt::PI_2 ), fTolerance ) )
    {
        // Close enough, now fake it.
        //
        mpCharacter->SetFacingDir( mfTarget );
        Done( );
    }
}

/*
==============================================================================
Position::Position
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter, rmt::Vector& position, float fDuration )

Return:         Position

=============================================================================
*/
Position::Position( Character* pCharacter, rmt::Vector& position, float fDuration, bool local )
:
mpCharacter( pCharacter ),
mDestination( position ),
mfDuration( fDuration ),
mfTimeLeft( fDuration ),
mLocal(local)
{
}

/*
==============================================================================
Position::~Position
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         Position

=============================================================================
*/
Position::~Position( void )
{
}

/*
==============================================================================
Position::WakeUp
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void Position::WakeUp( float timeins )
{
}
/*
==============================================================================
Position::DoSimulation
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void Position::DoSimulation( float timeins )
{
    mfTimeLeft -= timeins;
    
    if ( mfTimeLeft > 0.0f )
    {
        rmt::Vector position;
        if(mLocal)
        {
            position = mpCharacter->GetPuppet()->GetPosition();
        }
        else
        {
            mpCharacter->GetPosition( position );
        }

        rmt::Vector delta = mDestination - position;
        delta.Scale(rmt::Min(timeins / mfTimeLeft, 1.0f));
        position.Add(delta);

        if(mLocal)
        {
            mpCharacter->GetPuppet()->SetPosition( position);
        }
        else
        {
            mpCharacter->SetPosition( position );
        }
    } 
    else
    {
        if(mLocal)
        {
            mpCharacter->GetPuppet()->SetPosition( mDestination );
        }
        else
        {
            mpCharacter->SetPosition( mDestination );
        }
        Done();
    }
}
/*
==============================================================================
Position::Update
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void Position::Update( float timeins )
{
}

void GroundSnap::WakeUp( float timeins )
{
    static float highOffset = 1.0f;
    static float lowOffset = 0.4f;

    mpCharacter->GetPuppet()->StopAllDrivers();

    bool high = false;
    
    if(mpCharacter->GetTargetVehicle())
    {
        high = mpCharacter->GetTargetVehicle()->GetDriverLocation().y > CharacterTune::sGetInHeightThreshold;
    }

    rmt::Vector position = mpCharacter->GetPuppet()->GetPosition();
    position.y -= high ? highOffset : lowOffset;
    mpCharacter->GetPuppet()->SetPosition(position);
    Done();
}

#include <choreo/animation.hpp>
/*
==============================================================================
PlayAnimationAction::PlayAnimationAction
==============================================================================
Description:    Comment

Parameters:     (Character* pCharacter, const char* anim, bool preCollideWithWorld, bool postCollideWithWorld )

Return:         PlayAnimationAction

=============================================================================
*/
PlayAnimationAction::PlayAnimationAction(Character* pCharacter, const char* anim, float fFrameRate /* = 30.0f  */ )
:
mpCharacter( pCharacter ),
mfFrameRate( fFrameRate ),
mpAnimationDriver( 0 ),
mbAbortWhenMovementOccurs( false ),
loop(false)
{
    mAnimUID = tEntity::MakeUID( anim );
}

PlayAnimationAction::PlayAnimationAction( Character* pCharacter, const tName& anim, float fFrameRate )
:
mpCharacter( pCharacter ),
mfFrameRate( fFrameRate ),
mpAnimationDriver( 0 ),
loop(false) 
{
    mAnimUID = anim.GetUID();
}


//=============================================================================
//PlayAnimationAction::AbortWhenMovementOccurs
//=============================================================================
//Description:    do we want to abort this action if the character moves? y/n
//
//Parameters:     abort - do we want to?
//
//Return:         void 
//
//=============================================================================
void PlayAnimationAction::AbortWhenMovementOccurs( bool abort )
{
    mbAbortWhenMovementOccurs = abort;
}

/*
==============================================================================
PlayAnimationAction::WakeUp
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void PlayAnimationAction::WakeUp( float timeins )
{
    choreo::Puppet* pPuppet = mpCharacter->GetPuppet();
    rAssert( pPuppet );

    choreo::Animation* anim = choreo::find<choreo::Animation>( pPuppet->GetBank(), mAnimUID );
    if (anim == 0)
        return;
    
    choreo::AnimationDriver* animDriver = anim->NewAnimationDriver();
    animDriver->AddRef();

    animDriver->SetWeight(1.0f);
    animDriver->SetPriority(0);
    animDriver->SetSpeed( mfFrameRate / animDriver->GetFramesPerSecond() );

    if(loop)
    {
        animDriver->SetIsCyclic(true);
    }

    if ( pPuppet->PlayDriver( animDriver, loop ? -1.0f : animDriver->GetDuration() ) )
    {
        mpAnimationDriver = animDriver;
        animDriver->Release();
    }
    else
    {
        animDriver->Release();
    }
}
/*
==============================================================================
PlayAnimationAction::DoSimulation
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void PlayAnimationAction::DoSimulation( float timeins )
{
}
/*
==============================================================================
PlayAnimationAction::Update
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void PlayAnimationAction::Update( float timeins )
{
    //
    // Abort the action if there was any movement
    //
    if( ShouldAbort() )
    {
        Done();
        return;
    }

    if( mpCharacter->GetPuppet()->IsDriverFinished( mpAnimationDriver ) == true )
    {
        Done();
    }

}

bool PlayAnimationAction::ShouldAbort(void)
{
    if(mbAbortWhenMovementOccurs)
    {
        CharacterController* c = mpCharacter->GetController(); 
        if((c->GetIntention() != CharacterController::NONE) || AnyStickMovement(mpCharacter))
        {
            return true;
        }
    }

    return false;
}

/*
==============================================================================
PlayAnimationAction::Clear
==============================================================================
Description:    Comment

Parameters:     ()

Return:         void 

=============================================================================
*/
void PlayAnimationAction::Clear()
{
    mpCharacter->SetDesiredDir(mpCharacter->GetFacingDir());
    mpCharacter->GetPuppet()->StopDriver( mpAnimationDriver );
}




/*
==============================================================================
PlayIdleAnimationAction::PlayIdleAnimationAction
==============================================================================
Description:    Comment

Parameters:     (Character* pCharacter, const char* anim, bool preCollideWithWorld, bool postCollideWithWorld )

Return:         PlayIdleAnimationAction

=============================================================================
*/
PlayIdleAnimationAction::PlayIdleAnimationAction(Character* pCharacter, const char* anim, float fFrameRate /* = 30.0f  */ )
:
PlayAnimationAction( pCharacter, anim, fFrameRate )
{
}

PlayIdleAnimationAction::PlayIdleAnimationAction( Character* pCharacter, const tName& anim, float fFrameRate )
:
PlayAnimationAction( pCharacter, anim, fFrameRate )
{
}

/*
==============================================================================
PlayIdleAnimationAction::WakeUp
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void PlayIdleAnimationAction::WakeUp( float timeins )
{
    choreo::Puppet* pPuppet = mpCharacter->GetPuppet();
    rAssert( pPuppet );

    choreo::Animation* anim = choreo::find<choreo::Animation>( pPuppet->GetBank(), mAnimUID );
    if (anim == 0)
        return;
    
    mpCharacter->mbIsPlayingIdleAnim = true;

    PlayAnimationAction::WakeUp( timeins );
}

/*
==============================================================================
PlayIdleAnimationAction::Clear
==============================================================================
Description:    Comment

Parameters:     ()

Return:         void 

=============================================================================
*/
void PlayIdleAnimationAction::Clear()
{
    mpCharacter->mbIsPlayingIdleAnim = false;
    PlayAnimationAction::Clear();
}

/*
==============================================================================
PlayIdleAnimationAction::Abort
==============================================================================
Description:    Comment

Parameters:     ()

Return:         void 

=============================================================================
*/
void PlayIdleAnimationAction::Abort()
{
    mpCharacter->mbIsPlayingIdleAnim = false;
}



/*
==============================================================================
CarDoorAction
==============================================================================
*/

CarDoorAction::CarDoorAction(Vehicle* pVehicle, Vehicle::DoorAction action, Vehicle::Door door, float delay, float time, Character* c, Sequencer* s)
:
mpVehicle( pVehicle),
mAction(action),
mDoor(door),
mTime(time),
mDelay(delay),
mCurrentTime(0.0f),
character(c),
sequencer(s)
{
}


void CarDoorAction::WakeUp( float timeins )
{
    //HACK : abort the action if we have a pending movement on the character controller
    // this needs to be handled more gracefully
    if(sequencer && character)
    {
        if(AnyStickMovement(character, 0.5f)) Done();
    }
}

void CarDoorAction::DoSimulation( float timeins )
{
}

void CarDoorAction::Update( float timeins )
{
    mCurrentTime += timeins;

    // make sure delay has elapsed
    if(mCurrentTime <= mDelay)
    {
        return;
    }

    // calcualte how far along in our action the door is
    float t = (mCurrentTime - mDelay) / mTime;

    // check for completion
    if(t > 1.0f)
    {
        t = 1.0f;
        Done();
    }

    // if we are closing the door, we go backwards
    if(mAction == Vehicle::DOORACTION_CLOSE)
    {
        t = 1.0f - t;
    }

    // move that sucker
    mpVehicle->MoveDoor(mDoor, mAction, t);
}

void CarDoorAction::Clear()
{
}

void ReleaseDoorsAction::Update( float timeins )
{
    mVehicle->ReleaseDoors();
    Done();
}

/*
==============================================================================
JumpAction::JumpAction
==============================================================================
Description:    Comment

Parameters:     ( Character* pCharacter, const char* name, float fUpVelocity )

Return:         JumpAction

=============================================================================
*/
#include <worldsim/character/charactercontroller.h>
#include <choreo/root.hpp>
#include <sound/soundmanager.h>

JumpAction::JumpAction( Character* pCharacter, const char* name, float fUpVelocity )
:
mpCharacter( pCharacter ),
mJumpState( InitJump ),
mAnimUid( 0 ),
mfTime( 0.0f ),
mfGravity( CharacterTune::sfAirGravity ),
mpAnimationDriver( 0 ),
mpRootController( 0 ),
mpRootDriver( 0 ),
mbBoost( false ),
mbFalling( false )

{
    mVelocity.Set( 0.0f, fUpVelocity, 0.0f );
    mAnimUid = tEntity::MakeUID( name );

    mpRootController = new choreo::RootTransformController;
    mpRootController->AddRef();

    // weight & priority
    mpRootController->SetRootPriority( 0 );
    mpRootController->SetRootWeight( 1.0f );

    // blend times
    mpRootController->SetRootBlendInTime( 0.0f );
    mpRootController->SetRootBlendOutTime( 0.0f );

    mpRootDriver = new choreo::RootDriver(mpRootController);
    mpRootDriver->AddRef();
}

/*
==============================================================================
JumpAction::~JumpAction
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         JumpAction

=============================================================================
*/
JumpAction::~JumpAction( void )
{
    tRefCounted::Release(mpRootDriver);
    tRefCounted::Release(mpRootController);
    tRefCounted::Release(mpAnimationDriver);
}


/*
==============================================================================
JumpAction::WakeUp
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void JumpAction::SetRootTransform( void )
{
    //poser::Transform rootTransform;
    //mpCharacter->GetRootTransform( rootTransform );
    //mpRootController->SetRootTransform( rootTransform );
    choreo::Puppet* pPuppet = mpCharacter->GetPuppet();
    rAssert( pPuppet );
    mpRootController->SetRootTransform( pPuppet->GetRootTransform( ) );
}
/*
==============================================================================
JumpAction::SetRootPosition
==============================================================================
Description:    Comment

Parameters:     ( const rmt::Vector& position )

Return:         void 

=============================================================================
*/
void JumpAction::SetRootPosition( const rmt::Vector& position )
{
    mpRootController->SetPosition( position );
}

/*
==============================================================================
JumpAction::Reset
==============================================================================
Description:    Comment

Parameters:     ( const rmt::Vector& jumpTarget, bool bFalling )

Return:         void 

=============================================================================
*/
void JumpAction::Reset( const rmt::Vector& jumpTarget, bool bFalling /* = false  */)
{
    float t = Reset( jumpTarget.y, bFalling );
    mpCharacter->ResetSpeed();
    rmt::Vector velocityXZ;
    velocityXZ = jumpTarget;
    velocityXZ.y = 0.0f;
    velocityXZ.Scale( 1.0f / t );
    mVelocity.x = velocityXZ.x;
    mVelocity.z = velocityXZ.z;
    mfMaxSpeed = velocityXZ.Magnitude( );

    // Clamp max speed
    if ( mfMaxSpeed > JUMP_ACTION_SPEED_CLAMP )
    {
        mfMaxSpeed = JUMP_ACTION_SPEED_CLAMP;
        velocityXZ.Normalize();
        velocityXZ.Scale( JUMP_ACTION_SPEED_CLAMP );
    }

    // Make the character smoothly rotate to face the direction of travel.
    //
    mpCharacter->SetDesiredDir( choreo::GetWorldAngle( mVelocity.x, mVelocity.z ) );

    mbPreSlam = mbSlam = false;
}
/*
==============================================================================
JumpAction::Reset
==============================================================================
Description:    Comment

Parameters:     ( float fJumpHeight, bool bFalling )

Return:         void 

=============================================================================
*/
static rmt::Vector startPosition;
static rmt::Vector endPosition;
static float sfJumpHeightHighWater;

float JumpAction::Reset( float fJumpHeight, bool bFalling )
{
    //rTuneAssertMsg( fJumpHeight >= 0, "JumpAction::Reset called with a negative height. Tell the designer who place the bouncepad to make sure the target is ABOVE the pad - MikeR"  );
      
    // Use a safe value 
    if ( fJumpHeight < 0.0f )
    {
        fJumpHeight = 0.2f;
    }
    mfTime = 0.0f;
    mbBoost = false;
    mbFalling = bFalling;
    mfGravity = CharacterTune::sfAirGravity;
    mVelocity.Clear( );
    rmt::Vector position;
    mpCharacter->GetPosition( position );
    mfStartHeight = position.y;
    
    if(mpAnimationDriver)
    {
        mpCharacter->GetPuppet()->StopDriver(mpAnimationDriver);
        tRefCounted::Release(mpAnimationDriver);
    }

    mbJumpAgain = false;
    // Overrides all animation translations.
    //
    mpRootController->SetRootPriority( -1 );

    choreo::Puppet* pPuppet = mpCharacter->GetPuppet();
    rAssert( pPuppet );

    // Init the position of the root controller.
    //
    SetRootTransform( );
    startPosition = mpRootController->GetPosition( );
    sfJumpHeightHighWater = 0.0f;

    mfMaxSpeed = rmt::Max( mpCharacter->GetMaxSpeed(), mpCharacter->GetSpeed( ) );
    
    rmt::Vector velocity;
    mpCharacter->GetVelocity(velocity);
    velocity.y = 0;

    // We care about the XZ velocity only, so we take the magnitude before we set the .y
    //
    float fActualVelocity = mVelocity.Magnitude( );
    
    // We don't want to use the XZ velocity via physics when we are colliding.
    // The impulse from the collision detection creates XZ velocity in the 
    // simstate when there is no visual velocity.
    // TBJ [7/24/2002] 
    //
    if ( mpCharacter->IsInCollision( ) )
    {
        velocity.x = velocity.z = 0.0f;
    }

    // Note, the mVelocity.y gets set in ::SetJumpVelocity
    // called below.
    //
    mVelocity.x = velocity.x;
    mVelocity.z = velocity.z;

    // Do the calculations.
    //
    float g = mfGravity;
    float d = fJumpHeight;
    // How long does it take to fall d metres, given gravity and v = 0.
    //
    float t = rmt::Fabs( .5000000000f/-g*(-2.f*rmt::Sqrt(2.f*-g*d)) );
    float v = 0.0f;
    // OR we are in a bounce pad.
    //
    if ( !bFalling || fJumpHeight > 0.0f )
    {
        v = -.5000000000f*(g*t*t-2.f*d)/t; 
    }
    // If we are on a platform, and running in the same direction as motion,
    // our speed can be gt mfMaxSpeed.
    // TBJ [7/24/2002] 
    //
    mfMaxSpeed = rmt::Max( mfMaxSpeed, fActualVelocity );

    if(mpCharacter->IsTurbo())
    {
        mfMaxSpeed -= CharacterTune::sfDashBurstMax;
    }
    
    // If we are on a bounce pad.
    //
    if ( bFalling && fJumpHeight > 0.0f )
    {
        velocity.y = 0.0f;
    }
    SetJumpVelocity( velocity.y + v );

    SetStatus(SLEEPING);
    
    mbPreSlam = mbSlam = false;
    mbInJumpKick = mbDoJumpKick = false;

    return t;
}

void JumpAction::WakeUp( float timeins )
{
    mpCharacter->SetJumping( true );

    // We care about the XZ velocity only, so we take the magnitude before we set the .y
    //
    float fActualVelocity = mVelocity.Magnitude( );

    if ( mbFalling 
        || mpCharacter->GetDesiredSpeed() != 0.0f 
        || mpCharacter->GetSpeed() != 0.0f 
        || fActualVelocity != 0 )
    {
        mJumpState = PreJump;
    }
    else
    {
        mJumpState = InitJump;
    }
    
    mpCharacter->GetPuppet()->PlayDriver(mpRootDriver, -1.0f);  
}
/*
==============================================================================
JumpAction::DoSimulation
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void JumpAction::DoSimulation( float timeins )
{
    // Do not update the control is the PostTurboJumpRecover state.
    // If the user wants to move, the action will transition to the
    // Loco action.  If we block that transition (turbo jump recovery)
    // then we don't want the character to slide around.
    // TBJ [7/24/2002] 
    //
    if ( (AllowTranslate & mJumpState) && !mbPreSlam)
    {
        // Air control
        //
        rmt::Vector dir;
        mpCharacter->GetDesiredFacing( dir );

        if(!mbSlam)
        {
            float desiredSpeed = mpCharacter->GetDesiredSpeed();
            rmt::Vector accel = dir;
            accel.Scale( desiredSpeed * CharacterTune::sfAirAccelScale );
            // v = v0 + at;
            mVelocity.Add( accel );
        }

        float up = mVelocity.y;
        mVelocity.y = 0.0f;
        float speed = mVelocity.Magnitude();
        if ( speed >= mfMaxSpeed )
        {
            mVelocity.Normalize();
            mVelocity.Scale( mfMaxSpeed );
        }
        mVelocity.y = up;

        if ( dir.MagnitudeSqr( ) > 0.0000001 && !mbSlam)
        {
            float fDesiredDir = choreo::GetWorldAngle( dir.x, dir.z );
            float fDir = SolveActualDir( fDesiredDir, timeins );
            mpRootController->SetOrientation( fDir );
        }
    
        rmt::Vector velocity;
        velocity = mVelocity;
        if ( Jump != mJumpState )
        {
            velocity.y = 0.0f;
        }

        if(mbSlam)
        {
            velocity.x = velocity.z = 0;
        }

        rmt::Vector position;
        position = mpRootController->GetPosition( );

        velocity.Scale( timeins  );

        // The root controller doesn't know about the parent transform,
        // so we have to take care of resolving the actual position updates
        // here.
        //
        rmt::Matrix localToWorldRotation = mpCharacter->GetParentTransform();    
        localToWorldRotation.IdentityTranslation( );
        velocity.Transform( localToWorldRotation );

        velocity.Add(position);
        SetRootPosition( velocity );

        float gravityDelta;
        gravityDelta = mfGravity;
        gravityDelta *= timeins;
        mVelocity.y += gravityDelta * (mbSlam ? CharacterTune::sfStompGravityScale : 1.0f);
    }
}
/*
==============================================================================
JumpAction::SolveActualDir
==============================================================================
Description:    Comment

Parameters:     ( float fDesiredDir, float timeins )

Return:         float 

=============================================================================
*/
float JumpAction::SolveActualDir( float fDesiredDir, float timeins )
{
    // Damping function.
    //
    float rotateDelta = choreo::GetSmallestArc( mpCharacter->GetFacingDir( ), fDesiredDir );
    float newRatio = CharacterTune::sfAirRotateRate * timeins;
    float oldRatio = 1.0f - newRatio;
    float newDir = ( mpCharacter->GetFacingDir( ) * oldRatio ) + ( ( mpCharacter->GetFacingDir( ) + rotateDelta ) * newRatio );

    return newDir;
}

/*
==============================================================================
JumpAction::Update
==============================================================================
Description:    Comment

Parameters:     ( float timeins )

Return:         void 

=============================================================================
*/
void JumpAction::Update( float timeins )
{
    mpCharacter->DoGroundIntersect(true);

    choreo::Puppet* pPuppet = mpCharacter->GetPuppet();
    rAssert( pPuppet );
    
    rmt::Vector outPos;
    rmt::Vector outNormal;

    rmt::Vector newPos = mpCharacter->LocalToWorld( mpRootController->GetPosition( ) );
    mpCharacter->GetTerrainIntersect( outPos, outNormal );
    
    switch ( mJumpState )
    {
    case InitJump:
        {
            tRefCounted::Assign(mpAnimationDriver,mpCharacter->GetPuppet()->PlayAnimation( "jump_idle_launch" ));
            // SetIsDriverIndefinite( true ) as we will manually control the stop
            // of the animation.  Length can change based on terrain/collision objects.
            //
            pPuppet->SetIsDriverIndefinite( mpAnimationDriver, false );
            mJumpState = IdleJump;
            break;
        }
        // We have two states here because they have different translate flags.
        //
    case IdleJump:
    case PreJump:
        {
            if ( !mpAnimationDriver || pPuppet->IsDriverFinished( mpAnimationDriver ) )
            {
                // if we are falling, not jumping.
                // don't play a sound.
                // Otherwise this is a jump, and we are beginning our vertical ascent.
                // Play a sound.
                //
                if ( !mbFalling )
                {
                    GetEventManager()->TriggerEvent( EVENT_JUMP_TAKEOFF );
                }
//animation "stomp_in_air"
//animation "stomp_land"
//animation "jump_kick"

                pPuppet->StopDriver( mpAnimationDriver );
                if ( mbJumpAgain )
                {
                    tRefCounted::Assign(mpAnimationDriver,mpCharacter->GetPuppet()->PlayAnimation( "jump_dash_in_air" ));
                }
                else
                {
                    tRefCounted::Assign(mpAnimationDriver,mpCharacter->GetPuppet()->PlayAnimation( "jump_idle" ));
                }
                // SetIsDriverIndefinite( true ) as we will manually control the stop
                // of the animation.  Length can change based on terrain/collision objects.
                //
                pPuppet->SetIsDriverIndefinite( mpAnimationDriver, true );

                mJumpState = Jump;
            }
            break;
        }
    case Jump:
        {
            TestForJumpAgain( newPos, mVelocity.y, outPos );
            float landingStr = 0.0f;
            if(mbPreSlam && ( !mpAnimationDriver || pPuppet->IsDriverFinished( mpAnimationDriver )))
            {
                mbPreSlam = false;
                tRefCounted::Assign(mpAnimationDriver,pPuppet->PlayAnimation( "stomp_in_air" ));
            }

            if ( mpCharacter->IsStanding() )
            {
                rmt::Vector pos;
                mpCharacter->GetPosition(pos);
                mpRootController->SetPosition( mpCharacter->WorldToLocal( pos ) );
                mVelocity.y = 0.0f;
                pPuppet->StopDriver( mpAnimationDriver );
                eJumpState nextState = PostJump;

                landingStr = 0.5f;
                if( mbJumpAgain )
                {
                    landingStr = 0.7f;
                }
                if ( mbSlam )
                {
                    mpCharacter->ResetSpeed( );
                    mVelocity.Clear( );
                    tRefCounted::Assign(mpAnimationDriver,pPuppet->PlayAnimation( "stomp_land" ));

                    // SetIsDriverIndefinite( true ) as we will manually control the stop
                    // of the animation.  Length can change based on terrain/collision objects.
                    //
                    pPuppet->SetIsDriverIndefinite( mpAnimationDriver, false );
                    
                    landingStr = 1.0f;
                    
                    mpCharacter->Slam();

                } else if ( mpCharacter->GetDesiredSpeed() == 0.0f )
                {
                    mpCharacter->ResetSpeed( );
                    mVelocity.Clear( );
                    tRefCounted::Assign(mpAnimationDriver, pPuppet->PlayAnimation( "jump_idle_land" ));
                    // SetIsDriverIndefinite( true ) as we will manually control the stop
                    // of the animation.  Length can change based on terrain/collision objects.
                    //
                    pPuppet->SetIsDriverIndefinite( mpAnimationDriver, false );
                }
                else
                {
                    tRefCounted::Release(mpAnimationDriver);
                }
                if( ( landingStr > 0.0f ) && !mpCharacter->IsNPC() )
                {
                    rmt::Vector p, n;
                    mpCharacter->GetTerrainIntersect( p, n );
                    GetSparkleManager()->AddLanding( p, landingStr );
                    if( landingStr >= 1.0f )
                    {
                        GetSparkleManager()->AddShockRing( p, landingStr );
                    }
                }
                GetEventManager()->TriggerEvent( EVENT_JUMP_LANDING );
            
                mJumpState = nextState;
            }
            else
            {
                if ( mpAnimationDriver )
                {
                    if(mbInJumpKick)
                    {
                        if(mbDoJumpKick && (mpAnimationDriver->GetFrame() > 6))
                        {
                            mpCharacter->Kick();
                            mbDoJumpKick = false;
                        }
                    }
                    else
                    {
                        // Thanks Maple!
                        //
                        float g = -mfGravity;
                        float v = mVelocity.y;
                        float d = newPos.y - outPos.y;
                        //float t0 = .5000000000f/g*(-2.f*v+2.f*rmt::Sqrt((v*v)+2.f*g*d));
                        float t1 = rmt::Fabs( .5000000000f/g*(-2.f*v-2.f*rmt::Sqrt((v*v)+2.f*g*d)) );
                        // t1 seems to be a better root for us.
                        // Why?
                        //
                        float airTime = t1;
                        float animTime = mpAnimationDriver->GetTimeRemaining( );
                        float speedScale = animTime / airTime;
                        if ( speedScale < 1.0f )
                        {
                            mpAnimationDriver->SetSpeed( mpAnimationDriver->GetSpeed( ) * speedScale );
                        }
                    }
                }
            }
            break;
        }
    case PostJump:
        {
            // bQuit is true when the user has pressed a direction.
            // Or pressed "Jump".
            // Allows the skip of the land animation and continue into Loco.
            //
            bool bQuit = (mpCharacter->GetDesiredSpeed() != 0.0f) && !mbSlam;
            bQuit |= ( CharacterController::Jump == mpCharacter->GetController()->GetIntention( ) );
                //mpCharacter->GetController()->IsButtonDown( CharacterController::Jump ) );
            
            mpRootController->SetPosition( mpCharacter->WorldToLocal( outPos ) );
            if ( bQuit || !mpAnimationDriver || pPuppet->IsDriverFinished( mpAnimationDriver ) )
            {   
                Done( );
                return;
            }
            break;
        }
    case PostTurboJumpRecover:
        {
            mpRootController->SetPosition( mpCharacter->WorldToLocal( outPos ) );
            if ( !mpAnimationDriver || pPuppet->IsDriverFinished( mpAnimationDriver ) )
            {   
                Done( );
                return;
            }
            break;
        }
    default:
        {
            break;
        }
    }
    {
        rmt::Vector position = mpRootController->GetPosition( );
        float fHeight = position.y - startPosition.y;
        if ( fHeight > sfJumpHeightHighWater )
        {
            sfJumpHeightHighWater = fHeight;
        }
    }
    mfTime += timeins;
}

/*
==============================================================================
JumpAction::TestForJumpAgain
==============================================================================
Description:    Comment

Parameters:     ( const rmt::Vector& currentPos, float fUpVelocity, rmt::Vector& groundPos )

Return:         void 

=============================================================================
*/
void JumpAction::TestForJumpAgain( const rmt::Vector& currentPos, float fUpVelocity, rmt::Vector& groundPos )
{
    if(mbPreSlam || mbSlam || mbInJumpKick || mbDoJumpKick || mbFalling)
        return;

    CharacterController::eIntention theIntention = mpCharacter->GetController()->GetIntention();
    if ( theIntention == CharacterController::Jump && !mbJumpAgain)
    {
        mpCharacter->GetController()->ClearIntention();

        rmt::Vector outnorm;
        mpCharacter->GetTerrainIntersect( groundPos, outnorm );

        static float allowJump = 4.0f;

        float fHeightAboveGround = currentPos.y - groundPos.y;
        bool doJump = (fUpVelocity > 0) ? 
            (fUpVelocity <= CharacterTune::sfDoubleJumpAllowUp) :
            (rmt::Abs(fUpVelocity) <= CharacterTune::sfDoubleJumpAllowDown);
        if ( doJump ) 
        {
            if(mpAnimationDriver)
            {
                mpCharacter->GetPuppet()->StopDriver(mpAnimationDriver);
            }
            Reset(CharacterTune::sfDoubleJumpHeight);
            mbJumpAgain = true;
            mJumpState = PreJump;
            GetEventManager()->TriggerEvent( EVENT_DOUBLEJUMP, mpCharacter );
        }
        else
        {
            // No chaining of actions.
            //
            mpCharacter->GetController()->SetIntention( CharacterController::NONE );
        }
    } 
    else if ( theIntention == CharacterController::Attack )
    {
        if(mbJumpAgain)
        {
            if(mpAnimationDriver )
            {
                mpCharacter->GetPuppet()->StopDriver( mpAnimationDriver );
            }

            tRefCounted::Assign(mpAnimationDriver,mpCharacter->GetPuppet()->PlayAnimation( "stomp_antic" ));
//            mpAnimationDriver = mpCharacter->GetPuppet()->PlayAnimation( "stomp_in_air" );
            mbPreSlam = true;
            mbSlam = true;
        }
        else
        {
            if(mpAnimationDriver )
            {
                mpCharacter->GetPuppet()->StopDriver( mpAnimationDriver );
            }

            tRefCounted::Assign(mpAnimationDriver,mpCharacter->GetPuppet()->PlayAnimation( "jump_kick" ));
            mbInJumpKick = mbDoJumpKick = true;
            mpCharacter->DoKickwave();
        }
    }
}

/*
==============================================================================
JumpAction::Clear
==============================================================================
Description:    Comment

Parameters:     ()

Return:         void 

=============================================================================
*/
void JumpAction::Clear()
{
    choreo::Puppet* pPuppet = mpCharacter->GetPuppet();
    mpCharacter->SetJumping( false );
    mJumpState = JumpDone;
    rAssert( pPuppet );
    pPuppet->StopDriver ( mpAnimationDriver );
    pPuppet->StopDriver( mpRootDriver );
    mpCharacter->SetTurbo( false );
    endPosition = mpRootController->GetPosition( );
    startPosition.y = endPosition.y = 0.0f;
    startPosition.Sub( endPosition );
    rDebugPrintf( "Distance: %.2f\n", startPosition.Magnitude( ) );
    rDebugPrintf( "Height: %.2f\n", sfJumpHeightHighWater );
    rDebugPrintf( "Time: %.2f\n", mfTime );
    rDebugPrintf( "-----------------\n" );
}

//
//=========================== DODGE ACTION ===============================
//

DodgeAction::DodgeAction( Character* pCharacter )
:
mpAnimationDriver( NULL ), // Set when we start playing the dodge animation and need to store its handle
mpCharacter(pCharacter)
{
    rAssert( mpCharacter != NULL );

}

void DodgeAction::WakeUp( float timeins )
{
    rAssert( mpAnimationDriver == NULL );
    mpAnimationDriver = mpCharacter->GetPuppet()->PlayAnimation( "dodge" );
    rAssert( mpAnimationDriver != NULL );

    mpCharacter->GetPuppet()->SetIsDriverIndefinite( mpAnimationDriver, false );
    mpCharacter->SetBusy( true );
}

void DodgeAction::DoSimulation( float timeins)
{
    rAssert( mpCharacter != NULL );

    // check that the character has a puppet... cuz that's where we're getting 
    // all our facing information.
    choreo::Puppet* pPuppet = mpCharacter->GetPuppet();
    rAssert( pPuppet );
    
    rmt::Vector dir;
    mpCharacter->GetDesiredFacing( dir );
    float fDesiredDir = choreo::GetWorldAngle( dir.x, dir.z );
    float fActualDir = SolveActualDir( fDesiredDir, timeins );
    mpCharacter->SetFacingDir( fActualDir );
}

void DodgeAction::Update( float timeins)
{
    rAssert( mpAnimationDriver != NULL );
    if( mpCharacter->GetPuppet()->IsDriverFinished( mpAnimationDriver ) )
    {
        Done();
    }
}

void DodgeAction::Clear()
{
    mpCharacter->SetBusy( false );
    mpCharacter->GetPuppet()->StopDriver( mpAnimationDriver );
    mpAnimationDriver = NULL;
}

float DodgeAction::SolveActualDir( float fDesiredDir, float timeins )
{
    float rotateDelta = choreo::GetSmallestArc( mpCharacter->GetFacingDir( ), fDesiredDir );
    float newDir = mpCharacter->GetFacingDir( ) + rotateDelta;
    return newDir;
}

float DodgeAction::SolveActualSpeed( float fInputSpeed, float timeins )
{
    return fInputSpeed;
}



//
//=========================== CRINGE ACTION ===============================
//


CringeAction::CringeAction( Character* pCharacter )
:
mpAnimationDriver( NULL ), // Set when we start playing the animation and need to store its handle
mpCharacter(pCharacter)
{
    rAssert( mpCharacter != NULL );
}

void CringeAction::WakeUp( float timeins )
{
    rAssert( mpAnimationDriver == NULL );
    mpAnimationDriver = mpCharacter->GetPuppet()->PlayAnimation( "cringe" );
    rAssert( mpAnimationDriver != NULL );

    mpCharacter->GetPuppet()->SetIsDriverIndefinite( mpAnimationDriver, false );
}

void CringeAction::DoSimulation( float timeins)
{
}

void CringeAction::Update( float timeins)
{
    rAssert( mpAnimationDriver != NULL );
    if( mpCharacter->GetPuppet()->IsDriverFinished( mpAnimationDriver ) )
    {
        Done();
    }
}

void CringeAction::Clear()
{
    mpCharacter->GetPuppet()->StopDriver( mpAnimationDriver );
    mpAnimationDriver = NULL;
}

float CringeAction::SolveActualDir( float fDesiredDir, float timeins )
{
    float rotateDelta = choreo::GetSmallestArc( mpCharacter->GetFacingDir( ), fDesiredDir );
    float newDir = mpCharacter->GetFacingDir( ) + rotateDelta;
    return newDir;
}

float CringeAction::SolveActualSpeed( float fInputSpeed, float timeins )
{
    return fInputSpeed;
}

//
//=========================== FLAIL ACTION ===============================
//
FlailAction::FlailAction( Character* pCharacter )
:
mpAnimationDriver( NULL ), // Set when we start playing the animation and need to store its handle
mpCharacter(pCharacter)
{
    rAssert( mpCharacter != NULL );

}

void FlailAction::WakeUp( float timeins )
{
    // The animation driver should modify the puppet's position while it's 
    // playing...
    rAssert( mpAnimationDriver == NULL );
    mpAnimationDriver = mpCharacter->GetPuppet()->PlayAnimation( "flail" );
    rAssert( mpAnimationDriver != NULL );

    mpAnimationDriver->SetIsCyclic( true );
    //mpAnimationDriver->SetPriority( 0 );
    mpCharacter->GetPuppet()->SetIsDriverIndefinite( mpAnimationDriver, true );

    mpCharacter->SetBusy( true );
}

void FlailAction::DoSimulation( float timeins)
{
    // Something else is changing my motion root for me, so I do nothing
}

void FlailAction::Update( float timeins)
{
    rAssert( mpAnimationDriver != NULL );

    if( mpCharacter->GetSimState()->GetControl() == sim::simAICtrl )
    {
        Done();
    }
}

float FlailAction::SolveActualDir( float fDesiredDir, float timeins )
{
    float rotateDelta = choreo::GetSmallestArc( mpCharacter->GetFacingDir( ), fDesiredDir );
    float newDir = mpCharacter->GetFacingDir( ) + rotateDelta;
    return newDir;
}

float FlailAction::SolveActualSpeed( float fInputSpeed, float timeins )
{
    return fInputSpeed;
}

void FlailAction::Clear()
{
    mpCharacter->GetPuppet()->StopDriver( mpAnimationDriver );
    mpAnimationDriver = NULL;
    mpCharacter->SetBusy( false );
}


//
//=========================== GETUP ACTION ===============================
//
GetUpAction::GetUpAction( Character* pCharacter ) :
mpAnimationDriver( NULL ) // Set when we start playing the animation and need to store its handle
{
    mpCharacter = pCharacter;
    rAssert( mpCharacter != NULL );

}

void GetUpAction::WakeUp( float timeins )
{
    /*
    static const float angle = 0.93969262078590838405410927732473f; // cos(20)
    static const rmt::Vector upVector(0.0f, 1.0f, 0.0f);
    if(mpCharacter->GetLean().Dot(upVector) > angle)
    {
        Done();
        return;
    }
    */

    rAssert( mpAnimationDriver == NULL );
    mpAnimationDriver = mpCharacter->GetPuppet()->PlayAnimation( "get_up" );
    rAssert( mpAnimationDriver != NULL );

    mpCharacter->GetPuppet()->SetIsDriverIndefinite( mpAnimationDriver, false );

    rmt::Vector up, right, forward;
    //right = mpCharacter->mPrevSimTransform.Row(0);
    up = mpCharacter->mPrevSimTransform.Row(1);
    //forward = mpCharacter->mPrevSimTransform.Row(2);

    // NOTE: 
    // Setting mDirection using SetDirection as done below doesn't
    // have any effect if speed is 0.0f. When character queries 
    // this controller for direction, it returns (0.0f,0.0f,0.0f) 
    // because it has been scaled by zero velocity.
    //SetDirection( up );

    // Use the "up" vector because the character comes out of flailing
    // when it stops "rolling" on the ground. So the "up" when it was on 
    // the ground should be the facing direction of the get-up anim.
    float dir = choreo::GetWorldAngle( up.x, up.z );
    mpCharacter->SetDesiredDir( dir );
    mpCharacter->SetFacingDir( dir );
}

void GetUpAction::DoSimulation( float timeins)
{
}

void GetUpAction::Update( float timeins)
{
    if( mpAnimationDriver == NULL )
    {
        return;
    }

    if( mpCharacter->GetPuppet()->IsDriverFinished( mpAnimationDriver ) )
    {
        Done();
    }

    // if we've gone back into simulation, abort, and cue another flail and get up
    if( mpCharacter->GetSimState()->GetControl() == sim::simSimulationCtrl)
    {
        // Sequence in the flail and get-up actions
        Sequencer* pSeq = mpCharacter->GetActionController()->GetNextSequencer();

        pSeq->BeginSequence();
        pSeq->AddAction( new FlailAction( mpCharacter ) );
        pSeq->EndSequence();

        pSeq->BeginSequence();
        pSeq->AddAction( new GetUpAction( mpCharacter ) );
        pSeq->EndSequence();

        Done();
    }
}

void GetUpAction::Clear()
{
    if(mpAnimationDriver)
    {
        mpCharacter->GetPuppet()->StopDriver( mpAnimationDriver );
        mpAnimationDriver = NULL;
    }
}

float GetUpAction::SolveActualDir( float fDesiredDir, float timeins )
{
    float rotateDelta = choreo::GetSmallestArc( mpCharacter->GetFacingDir( ), fDesiredDir );
    float newDir = mpCharacter->GetFacingDir( ) + rotateDelta;
    return newDir;
}

float GetUpAction::SolveActualSpeed( float fInputSpeed, float timeins )
{
    return fInputSpeed;
}

//
//=========================== CHANGELOCOMOTION ACTION ===============================
//
void ChangeLocomotion::Update( float timeins )
{
    if(mType == WALKING)
    {
        mpCharacter->SetInCar( false );
        mpCharacter->UpdateTransformToLoco();
        Done( );
    } 
    else if(mType == INCAR)
    {
        mpCharacter->SetInCar( true );
        mpCharacter->UpdateTransformToInCar();        
        Done( );
    }
}


//
//=========================== CHANGENPCCONTROLLERSTATE ACTION ===============================
//

void ChangeNPCControllerState::Update( float timeins )
{
    Abort();
    Done();
}

void ChangeNPCControllerState::Abort()
{
    // NOTE:
    // We put the actual call to TransitToState in Abort() because 
    // we want to guarantee that this is done. The problem is that 
    // we get into situations where a sequenced action (like this one)
    // can get prematurely cleared by one thing or another. Since this 
    // particular action is necessary for synching between the controller
    // state and the action performed, we ALWAYS want to do it... 

    rAssert( mpCharacter->IsNPC() );
    rAssert( dynamic_cast<NPCController*>(mpCharacter->GetController()) );
    ((NPCController*)mpCharacter->GetController())->TransitToState(mState);
    Action::Abort();
}

//
//=========================== KICK ACTION ===============================
//
void KickAction::Update( float timeins )
{
    mTime += timeins;
    if(mTime > mContact)
    {
        mpCharacter->Kick();
        Done();
    }
}

//
//=========================== SURF ACTION ===============================
//
SurfAction::SurfAction(Character* c) : PlayAnimationAction(c, "surf_cycle"), mLowCount(0)
{
    SetLoop(true);
    AbortWhenMovementOccurs(true);
}

bool SurfAction::ShouldAbort(void)
{
    return PlayAnimationAction::ShouldAbort() || (mLowCount > 16);
}

void SurfAction::Update(float t)
{
    rmt::Vector v;
    mpCharacter->GetVelocity(v);
    if(v.MagnitudeSqr() < 100.0f)
    {
        mLowCount += 1;
    }
    else
    {
        mLowCount = 0;
    }

    PlayAnimationAction::Update(t);

    CharacterController::eIntention theIntention = mpCharacter->GetController()->GetIntention();
    if ( theIntention == CharacterController::Jump )
    {
        Done();
        JumpAction* pAction = mpCharacter->GetJumpLocomotionAction();
        pAction->Reset(CharacterTune::sfJumpHeight, false);
        mpCharacter->GetActionController()->SequenceSingleAction(pAction);
        mpCharacter->GetController()->ClearIntention();
    }
}
