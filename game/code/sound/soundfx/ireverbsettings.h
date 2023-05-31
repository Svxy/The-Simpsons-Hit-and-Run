//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        ireverbsettings.h
//
// Description: Declaration for IReverbSettings class, which is the interface
//              visible in RadTuner for creating and storing a set of reverb 
//              parameters.
//
// History:     11/3/2002 + Created -- Darren
//
//=============================================================================

#ifndef IREVERBSETTINGS_H
#define IREVERBSETTINGS_H

//========================================
// Nested Includes
//========================================
#include <radobject.hpp>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    IReverbSettings
//
//=============================================================================

struct IReverbSettings : public IRefCount
{
    virtual void SetGain( float gain ) = 0;
    virtual void SetFadeInTime( float milliseconds ) = 0;
    virtual void SetFadeOutTime( float milliseconds ) = 0;

    //
    // See radsound_<platform name>.hpp for details on this stuff
    //
    virtual void SetXboxRoom( int mBvalue ) = 0;
    virtual void SetXboxRoomHF( int mBvalue ) = 0;
    virtual void SetXboxRoomRolloffFactor( float value ) = 0;
    virtual void SetXboxDecayTime( float value ) = 0;
    virtual void SetXboxDecayHFRatio( float value ) = 0;
    virtual void SetXboxReflections( int mBvalue ) = 0;
    virtual void SetXboxReflectionsDelay( float value ) = 0;
    virtual void SetXboxReverb( int mBvalue ) = 0;
    virtual void SetXboxReverbDelay( float value ) = 0;
    virtual void SetXboxDiffusion( float value ) = 0;
    virtual void SetXboxDensity( float value ) = 0;
    virtual void SetXboxHFReference( float value ) = 0;

    // No RadTuner interface for enumerations as far as I know, so
    // we'll have to cast whatever integer we get here
    virtual void SetPS2ReverbMode( int mode ) = 0;

    virtual void SetPS2Delay( float delayTime ) = 0;
    virtual void SetPS2Feedback( float feedback ) = 0;

    virtual void SetGCPreDelay( float milliseconds ) = 0;
    virtual void SetGCReverbTime( float milliseconds ) = 0;
    virtual void SetGCColoration( float coloration ) = 0;
    virtual void SetGCDamping( float damping ) = 0;

    // Must be defined for all platforms cause of the script.
    virtual void SetWinEnvironmentDiffusion( float diffusion ) = 0;
    virtual void SetWinAirAbsorptionHF( float value ) = 0;
};


#endif // IREVERBSETTINGS_H

