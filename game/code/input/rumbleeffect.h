//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        rumbleeffect.h
//
// Description: Blahblahblah
//
// History:     12/19/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef RUMBLEEFFECT_H
#define RUMBLEEFFECT_H

//========================================
// Nested Includes
//========================================
#include <radcontroller.hpp>

#include <input/controller.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

struct EffectValue
{
    unsigned int pulseTime;
    float gain;
    char motor;
    const char* name;
};

class RumbleEffect
{
public:
    enum Effect
    {
        LIGHT,
        MEDIUM,
        HARD1,
        HARD2,
        GROUND1,
        GROUND2,
        GROUND3,
        GROUND4,
        PULSE,
        NUM_EFFECTS
    };

    enum DynaEffect
    {
        COLLISION1,
        COLLISION2,
        NUM_DYNA_EFFECTS
    };

    RumbleEffect();
    virtual ~RumbleEffect();

    void SetWheelEffect( IRadControllerOutputPoint* wheelEffect );
    void SetMotor( unsigned int whichMotor, IRadControllerOutputPoint* motor );
    void SetEffect( Effect effect, unsigned int milliseconds );
    void SetDynaEffect( DynaEffect effect, unsigned int milliseconds, float gain );

    void Update( unsigned int milliseconds );
    void ShutDownEffects();

private:
    IRadControllerOutputPoint* mWheelEffect;
    IRadControllerOutputPoint* mMotors[ Input::MaxOutputMotor ];
    bool mMotorUpdated[ Input::MaxOutputMotor ];

    struct EffectInfo
    {
        EffectInfo() : mRumbleTimeLeft( 0 ) {};
        unsigned int mRumbleTimeLeft;
    };

    EffectInfo mCurrentEffects[ NUM_EFFECTS ];

    struct DynaEffectInfo : public EffectInfo
    {
        DynaEffectInfo() : mMaxGain( 1.0f ) {};
        float mMaxGain;
    };

    DynaEffectInfo mCurrentDynaEffects[ NUM_DYNA_EFFECTS ];

#ifdef DEBUGWATCH
    int mEffectNum;
#endif

    void InitEffects();
    void UpdateEffect( Effect effect, unsigned int milliseconds );
    void UpdateDynaEffect( DynaEffect effect, unsigned int milliseconds, float gain );
    void OnShutDownEffects();

    //Prevent wasteful constructor creation.
    RumbleEffect( const RumbleEffect& rumbleeffect );
    RumbleEffect& operator=( const RumbleEffect& rumbleeffect );
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

#endif //RUMBLEEFFECT_H
