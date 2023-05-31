//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        icarsoundparameters.h
//
// Description: Interface declaration for RadScript-created class for 
//              tunable car sound parameters
//
// History:     30/06/2002 + Created -- Darren
//
//=============================================================================

#ifndef ICARSOUNDPARAMETERS_H
#define ICARSOUNDPARAMETERS_H

//========================================
// Nested Includes
//========================================

#include <radobject.hpp>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    ICarSoundParameters
//
//=============================================================================

struct ICarSoundParameters : public IRefCount
{
    //
    // Vehicle RPM at which the engine sound clip should be played unchanged
    //
    virtual void SetClipRPM( float rpm ) = 0;

    //
    // Name of clip to use for engine sound
    //
    virtual void SetEngineClipName( const char* clipName ) = 0;

    //
    // Name of clip to use for engine idle sound
    //
    virtual void SetEngineIdleClipName( const char* clipName ) = 0;

    //
    // Name of clip to use for damaged engine sound
    //
    virtual void SetDamagedEngineClipName( const char* clipName ) = 0;

    //
    // Horn sound
    //
    virtual void SetHornClipName( const char* clipName ) = 0;

    //
    // Skid sounds
    //
    virtual void SetRoadSkidClipName( const char* clipName ) = 0;
    virtual void SetDirtSkidClipName( const char* clipName ) = 0;

    //
    // Backup sound
    //
    virtual void SetBackupClipName( const char* clipName ) = 0;

    //
    // Car door entry/exit sounds
    //
    virtual void SetCarDoorOpenClipName( const char* clipName ) = 0;
    virtual void SetCarDoorCloseClipName( const char* clipName ) = 0;

    //
    // Percentages of top speed for shift points
    //
    virtual void SetShiftPoint( unsigned int gear, float percent ) = 0;

    //
    // Percentage of top speed that we'll use as a range for dropping
    // below the shift point to avoid downshifting
    //
    virtual void SetDownshiftDamperSize( float percent ) = 0;

    //
    // Pitch range for a particular gear
    //
    virtual void SetGearPitchRange( unsigned int gear, float min, float max ) = 0;

    //
    // Number of gears
    //
    virtual void SetNumberOfGears( unsigned int gear ) = 0;

    //
    // Attack/delay/decay
    //
    virtual void SetAttackTimeMsecs( float msecs ) = 0;
    virtual void SetDelayTimeMsecs( unsigned int msecs ) = 0;
    virtual void SetDecayTimeMsecs( float msecs ) = 0;

    virtual void SetDecayFinishTrim( float trim ) = 0;

    //
    // Top speed for which we'll increase pitch in reverse
    //
    virtual void SetReversePitchCapKmh( float speed ) = 0;

    //
    // Reverse gear pitch range
    //
    virtual void SetReversePitchRange( float min, float max ) = 0;

    //
    // Amount of pitch drop and time to apply in gear shifts
    //
    virtual void SetGearShiftPitchDrop( unsigned int gear, float drop ) = 0;

    //
    // Percentage damage when we play damage sound
    //
    virtual void SetDamageStartPcnt( float damagePercent ) = 0;
    virtual void SetDamageMaxVolPcnt( float percent ) = 0;

    //
    // Volume control on damage sounds
    //
    virtual void SetDamageStartTrim( float trim ) = 0;
    virtual void SetDamageMaxTrim( float trim ) = 0;

    //
    // Pitch for idle engine clip (separate from sound resource in case
    // we want to share resource between cars.  Strictly a convenience,
    // since multiple sound resources with the same sound file could be
    // created instead).
    //
    virtual void SetIdleEnginePitch( float pitch ) = 0;

    //
    // In-air sound characteristics
    //
    virtual void SetInAirThrottlePitch( float pitch ) = 0;
    virtual void SetInAirIdlePitch( float pitch ) = 0;
    virtual void SetInAirThrottleResponseTimeMsecs( unsigned int msecs ) = 0;

    //
    // Burnout sound characteristics
    //
    virtual void SetBurnoutMinPitch( float pitch ) = 0;
    virtual void SetBurnoutMaxPitch( float pitch ) = 0;

    //
    // Powerslide sound characteristics
    //
    virtual void SetPowerslideMinPitch( float pitch ) = 0;
    virtual void SetPowerslideMaxPitch( float pitch ) = 0;

    //
    // Playing an overlay clip
    //
    virtual void SetOverlayClipName( const char* clipName ) = 0;
};


#endif // ICARSOUNDPARAMETERS_H

