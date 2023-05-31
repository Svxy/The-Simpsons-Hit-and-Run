//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        rumbleeffect.cpp
//
// Description: Implement RumbleEffect
//
// History:     12/19/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <raddebugwatch.hpp>

//========================================
// Project Includes
//========================================
#include <input/rumbleeffect.h>

//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************
#ifdef DEBUGWATCH
int gRECount = -1;
#endif

extern EffectValue VALUES[];

extern EffectValue DYNA_VALUES[];

float GAIN_FUDGE = 25.0f;

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// RumbleEffect::RumbleEffect
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
RumbleEffect::RumbleEffect() :
    mWheelEffect( NULL )
{
    unsigned int i;
    for ( i = 0; i < Input::MaxOutputMotor; ++i )
    {
        mMotors[ i ] = NULL;
        mMotorUpdated[ i ] = false;
    }

#ifdef DEBUGWATCH
    ++gRECount;
    mEffectNum = gRECount;
    char nameSpace[256];
    sprintf( nameSpace, "Controller\\RumbleEffect%d", mEffectNum );

    char name[256];
    for ( i = 0; i < NUM_EFFECTS; ++i )
    {
        sprintf( name, "%s Effect", VALUES[i].name );
        radDbgWatchAddUnsignedInt( &mCurrentEffects[i].mRumbleTimeLeft, name, nameSpace, NULL, NULL, 0, 100000 );
    }

    for ( i = 0; i < NUM_DYNA_EFFECTS; ++i )
    {
        sprintf( name, "%s Dyna Effect", DYNA_VALUES[i].name );
        radDbgWatchAddUnsignedInt( &mCurrentDynaEffects[i].mRumbleTimeLeft, name, nameSpace, NULL, NULL, 0, 100000 );
        radDbgWatchAddFloat( &mCurrentDynaEffects[i].mMaxGain, name, nameSpace, NULL, NULL, 0.0f, 1.0f );
    }
#endif

    InitEffects();
}

//=============================================================================
// RumbleEffect::~RumbleEffect
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
RumbleEffect::~RumbleEffect()
{
    unsigned int i;
    for ( i = 0; i < Input::MaxOutputMotor; ++i )
    {
        if( mMotors[ i ] )
        {
            mMotors[ i ]->Release();
            mMotors[ i ] = NULL;
        }
    }

#ifdef DEBUGWATCH
    gRECount--;
    for ( i = 0; i < NUM_EFFECTS; ++i )
    {
        radDbgWatchDelete( &mCurrentEffects[i].mRumbleTimeLeft );
    }

    for ( i = 0; i < NUM_DYNA_EFFECTS; ++i )
    {
        radDbgWatchDelete( &mCurrentDynaEffects[i].mRumbleTimeLeft );
        radDbgWatchDelete( &mCurrentDynaEffects[i].mMaxGain );
    }
#endif

    ShutDownEffects();
}

//=============================================================================
// RumbleEffect::SetMotor
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int whichMotor, IRadControllerOutputPoint* motor )
//
// Return:      void 
//
//=============================================================================
void RumbleEffect::SetMotor( unsigned int whichMotor, IRadControllerOutputPoint* motor )
{
    rAssert( whichMotor < Input::MaxOutputMotor );

    if ( mMotors[ whichMotor ] )
    {
        mMotors[ whichMotor ]->Release();
    }

    mMotors[ whichMotor ] = motor;
    mMotors[ whichMotor ]->AddRef();
}

//=============================================================================
// RumbleEffect::SetEffect
//=============================================================================
// Description: Comment
//
// Parameters:  ( Effect effect, unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void RumbleEffect::SetEffect( Effect effect, unsigned int milliseconds )
{
    if ( mCurrentEffects[ effect ].mRumbleTimeLeft < milliseconds )
    {
        mCurrentEffects[ effect ].mRumbleTimeLeft = milliseconds;
    }
}

//=============================================================================
// RumbleEffect::SetDynaEffect
//=============================================================================
// Description: Comment
//
// Parameters:  ( DynaEffect effect, unsigned int milliseconds, float gain )
//
// Return:      void 
//
//=============================================================================
void RumbleEffect::SetDynaEffect( DynaEffect effect, unsigned int milliseconds, float gain )
{
    if ( mCurrentDynaEffects[ effect ].mRumbleTimeLeft < milliseconds )
    {
        mCurrentDynaEffects[ effect ].mRumbleTimeLeft = milliseconds;

        if ( gain > 1.0f )
        {
            gain = 1.0f;
        }

        mCurrentDynaEffects[ effect ].mMaxGain = gain * GAIN_FUDGE;

#ifdef RAD_GAMECUBE
        if ( gain * GAIN_FUDGE < 0.3f )
        {
            mCurrentDynaEffects[ effect ].mMaxGain = 0;
        }
#endif
    }
}

//=============================================================================
// RumbleEffect::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void RumbleEffect::Update( unsigned int milliseconds )
{
    //The heaviest effect with time left is the one that runs.
    Effect currentEffect = NUM_EFFECTS;

    unsigned int i;

    //Update all the effects.
    for ( i = 0; i < NUM_EFFECTS; ++i )
    {
         if ( mCurrentEffects[ i ].mRumbleTimeLeft <= milliseconds )
         {
             if ( mCurrentEffects[ i ].mRumbleTimeLeft > 0 )
             {
                 UpdateEffect( static_cast<Effect>(i), mCurrentEffects[ i ].mRumbleTimeLeft );
             }

             mCurrentEffects[ i ].mRumbleTimeLeft = 0;
         }
         else
         {
             mCurrentEffects[ i ].mRumbleTimeLeft -= milliseconds;

             UpdateEffect( static_cast<Effect>(i), milliseconds );
         }
    }

    for ( i = 0; i < NUM_DYNA_EFFECTS; ++i )
    {
         if ( mCurrentDynaEffects[ i ].mRumbleTimeLeft <= milliseconds )
         {
             if ( mCurrentDynaEffects[ i ].mRumbleTimeLeft > 0 )
             {
                 UpdateDynaEffect( static_cast<DynaEffect>(i), mCurrentDynaEffects[ i ].mRumbleTimeLeft, mCurrentDynaEffects[ i ].mMaxGain );
             }

             mCurrentDynaEffects[ i ].mRumbleTimeLeft = 0;
         }
         else
         {
             mCurrentDynaEffects[ i ].mRumbleTimeLeft -= milliseconds;

             UpdateDynaEffect( static_cast<DynaEffect>(i), milliseconds, mCurrentDynaEffects[ i ].mMaxGain );
         }
    }

    for ( i = 0; i < Input::MaxOutputMotor; ++i )
    {
        //You only update a motor when you want to give it a positive value.
        if ( !mMotorUpdated[ i ] )
        {
            if ( mMotors[ i ]&& mMotors[ i ]->GetGain() > 0.0f )
            {
#ifdef RAD_GAMECUBE
                mMotors[ i ]->SetGain( -1.0f ); //This stops the motor HARD
#else
                mMotors[ i ]->SetGain( 0.0f );
#endif
            }
        }

        //Reset them all.
        mMotorUpdated[ i ] = false;
    }
}

//=============================================================================
// RumbleEffect::ShutDownEffects
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void RumbleEffect::ShutDownEffects()
{
    unsigned int i;
    for ( i = 0; i < NUM_EFFECTS; ++i )
    {
        mCurrentEffects[ i ].mRumbleTimeLeft = 0;
    }

    for ( i = 0; i < Input::MaxOutputMotor; ++i )
    {
        if ( mMotors[ i ] && mMotors[ i ]->GetGain() > 0.0f )
        {
#ifdef RAD_GAMECUBE
            mMotors[ i ]->SetGain( -1.0f ); //This stops the motor HARD
#else
            mMotors[ i ]->SetGain( 0.0f );
#endif
        }
    }

    OnShutDownEffects();
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************

//=============================================================================
// RumbleEffect::UpdateEffect
//=============================================================================
// Description: Comment
//
// Parameters:  ( Effect effect, unsigned int milliseconds )
//
// Return:      void 
//
//=============================================================================
void RumbleEffect::UpdateEffect( Effect effect, unsigned int milliseconds )
{
    if ( mCurrentEffects[ effect ].mRumbleTimeLeft % VALUES[effect].pulseTime < 32 ) //We use 32 because we hope to only do this once a frame (or twice)
    {
        if ( mMotors[ VALUES[effect].motor ] )
        {
            float currMotorGain = mMotors[ VALUES[effect].motor ]->GetGain();
            float desiredGain = VALUES[effect].gain;

            if ( currMotorGain < desiredGain )
            {
                mMotors[ VALUES[effect].motor ]->SetGain( VALUES[effect].gain );
                mMotorUpdated[ VALUES[effect].motor ] = true;
            }
#ifdef RAD_GAMECUBE
            // Michael Riegger - Gamecube is different from other platforms in rumble
            // control in that motor control is basically turn on / turn off commands
            // and that rumble just 'goes' in between those commands. No need to
            // toggle on and off manually
            // so set the updated flag to true always if its where we want. Otherwise
            // gain will be reset to 0 or -1 automatically
            else if ( currMotorGain == desiredGain )
            {
                mMotorUpdated[ VALUES[effect].motor ] = true;
            }
#endif
        }
    }
    /*    if ( mCurrentEffects[ effect ].mRumbleTimeLeft % VALUES[effect].pulseTime < 32 ) //We use 32 because we hope to only do this once a frame (or twice)
    {
        if ( mMotors[ VALUES[effect].motor ] )
        {


            if ( currMotorGain == desiredGain )
            {
                mMotorUpdated[ VALUES[effect].motor ] = true;
            }
            else if ( currMotorGain < desiredGain )
            {                
                mMotors[ VALUES[effect].motor ]->SetGain( VALUES[effect].gain );
                mMotorUpdated[ VALUES[effect].motor ] = true;
            }
        }
    }*/
}

//=============================================================================
// RumbleEffect::UpdateDynaEffect
//=============================================================================
// Description: Comment
//
// Parameters:  ( DynaEffect effect, unsigned int milliseconds, float gain )
//
// Return:      void 
//
//=============================================================================
void RumbleEffect::UpdateDynaEffect( DynaEffect effect, unsigned int milliseconds, float gain ) //This is a pcnt
{
    if ( mCurrentDynaEffects[ effect ].mRumbleTimeLeft % DYNA_VALUES[effect].pulseTime < 32 ) //We use 32 because we hope to only do this once a frame (or twice)
    {
        if ( mMotors[ DYNA_VALUES[effect].motor ] && 
             mMotors[ DYNA_VALUES[effect].motor ]->GetGain() < DYNA_VALUES[effect].gain * gain )
        {
            mMotors[ DYNA_VALUES[effect].motor ]->SetGain( DYNA_VALUES[effect].gain * gain );
            mMotorUpdated[ DYNA_VALUES[effect].motor ] = true;
        }
    }
}


//=============================================================================
// RumbleEffect::InitEffects
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void RumbleEffect::InitEffects()
{
#ifdef DEBUGWATCH
    if ( gRECount == 0 )
    {
        char name[256];
        unsigned int i;
        for ( i = 0; i < NUM_EFFECTS; ++i )
        {
            sprintf( name, "Controller\\%s Effect", VALUES[i].name );
            radDbgWatchAddUnsignedInt( &VALUES[i].pulseTime, "PulseTime", name, NULL, NULL, 1, 1000 );
            radDbgWatchAddFloat( &VALUES[i].gain, "Gain", name, NULL, NULL, 0.0f, 1.0f );
            radDbgWatchAddChar( &VALUES[i].motor, "Motor", name, NULL, NULL, 0, 1 );
        }

        for ( i = 0; i < NUM_DYNA_EFFECTS; ++i )
        {
            sprintf( name, "Controller\\%s Dyna Effect", VALUES[i].name );
            radDbgWatchAddUnsignedInt( &DYNA_VALUES[i].pulseTime, "PulseTime", name, NULL, NULL, 1, 1000 );
            radDbgWatchAddFloat( &DYNA_VALUES[i].gain, "Gain", name, NULL, NULL, 0.0f, 1.0f );
            radDbgWatchAddChar( &DYNA_VALUES[i].motor, "Motor", name, NULL, NULL, 0, 1 );
        }
    }
#endif
}

//=============================================================================
// RumbleEffect::OnShutDownEffects
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void RumbleEffect::OnShutDownEffects()
{
#ifdef DEBUGWATCH
    if ( gRECount == -1 )
    {
        unsigned int i;
        for ( i = 0; i < NUM_EFFECTS; ++i )
        {
            radDbgWatchDelete( &VALUES[i].pulseTime );
            radDbgWatchDelete( &VALUES[i].gain );
            radDbgWatchDelete( &VALUES[i].motor );
        }

        for ( i = 0; i < NUM_DYNA_EFFECTS; ++i )
        {
            radDbgWatchDelete( &DYNA_VALUES[i].pulseTime );
            radDbgWatchDelete( &DYNA_VALUES[i].gain );
            radDbgWatchDelete( &DYNA_VALUES[i].motor );
        }
    }
#endif
}