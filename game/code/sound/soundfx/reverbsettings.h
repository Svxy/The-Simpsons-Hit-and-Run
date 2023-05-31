//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        reverbsettings.h
//
// Description: Declaration for the reverbSettings class, which stores sets
//              of reverb parameters to be applied whenever we want that
//              reverby sound thing happening.  NOTE: lower-case "r" needed
//              to make RadScript happy.
//
// History:     11/5/2002 + Created -- Darren
//
//=============================================================================

#ifndef REVERBSETTINGS_H
#define REVERBSETTINGS_H

//========================================
// Nested Includes
//========================================
#include <radobject.hpp>
#include <radlinkedclass.hpp>

#include <sound/soundfx/ireverbsettings.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    reverbSettings
//
//=============================================================================

class reverbSettings : public IReverbSettings,
                       public radLinkedClass< reverbSettings >,
                       public radRefCount
{
    public:
        IMPLEMENT_REFCOUNTED( "reverbSettings" );

        reverbSettings();
        virtual ~reverbSettings();

        void SetGain( float gain ) { m_gain = gain; }
        float GetGain() { return( m_gain ); }
        void SetFadeInTime( float milliseconds ) { m_fadeInTime = milliseconds; }
        float GetFadeInTime() { return( m_fadeInTime ); }
        void SetFadeOutTime( float milliseconds ) { m_fadeOutTime = milliseconds; }
        float GetFadeOutTime() { return( m_fadeOutTime ); }

        //
        // See radsound_<platform name>.hpp for details on this stuff
        //
        void SetXboxRoom( int mBvalue ) { m_xboxRoom = mBvalue; }
        int GetXboxRoom() { return( m_xboxRoom ); }
        void SetXboxRoomHF( int mBvalue ) { m_xboxRoomHF = mBvalue; }
        int GetXboxRoomHF() { return( m_xboxRoomHF ); }
        void SetXboxRoomRolloffFactor( float value ) { m_xboxRoomRolloff = value; }
        float GetXboxRoomRolloffFactor() { return( m_xboxRoomRolloff ); }
        void SetXboxDecayTime( float value ) { m_xboxDecay = value; }
        float GetXboxDecayTime() { return( m_xboxDecay ); }
        void SetXboxDecayHFRatio( float value ) { m_xboxDecayHFRatio = value; }
        float GetXboxDecayHFRatio() { return( m_xboxDecayHFRatio ); }
        void SetXboxReflections( int mBvalue ) { m_xboxReflections = mBvalue; }
        int GetXboxReflections() { return( m_xboxReflections ); }
        void SetXboxReflectionsDelay( float value ) { m_xboxReflectionsDelay = value; }
        float GetXboxReflectionsDelay() { return( m_xboxReflectionsDelay ); }
        void SetXboxReverb( int mBvalue ) { m_xboxReverb = mBvalue; }
        int GetXboxReverb() { return( m_xboxReverb ); }
        void SetXboxReverbDelay( float value ) { m_xboxReverbDelay = value; }
        float GetXboxReverbDelay() { return( m_xboxReverbDelay ); }
        void SetXboxDiffusion( float value ) { m_xboxDiffusion = value; }
        float GetXboxDiffusion() { return( m_xboxDiffusion ); }
        void SetXboxDensity( float value ) { m_xboxDensity = value; }
        float GetXboxDensity() { return( m_xboxDensity ); }
        void SetXboxHFReference( float value ) { m_xboxHFReference = value; }
        float GetXboxHFReference() { return( m_xboxHFReference ); }

        // No RadTuner interface for enumerations as far as I know, so
        // we'll have to cast whatever integer we get here
        void SetPS2ReverbMode( int mode ) { m_ps2ReverbMode = mode; }
        int GetPS2ReverbMode() { return( m_ps2ReverbMode ); }

        void SetPS2Delay( float delayTime ) { m_ps2Delay = delayTime; }
        float GetPS2Delay() { return( m_ps2Delay ); }
        void SetPS2Feedback( float feedback ) { m_ps2Feedback = feedback; }
        float GetPS2Feedback() { return( m_ps2Feedback ); }

        void SetGCPreDelay( float milliseconds ) { m_gcPreDelay = milliseconds; }
        float GetGCPreDelay() { return( m_gcPreDelay ); }
        void SetGCReverbTime( float milliseconds ) { m_gcReverbTime = milliseconds; }
        float GetGCReverbTime() { return( m_gcReverbTime ); }
        void SetGCColoration( float coloration ) { m_gcColoration = coloration; }
        float GetGCColoration() { return( m_gcColoration ); }
        void SetGCDamping( float damping ) { m_gcDamping = damping; }
        float GetGCDamping() { return( m_gcDamping ); }

        // Must be defined for all platforms cause of the script.
        void SetWinEnvironmentDiffusion( float diffusion ) { m_winEnvironmentDiffusion = diffusion; }
        float GetWinEnvironmentDiffusion() const { return m_winEnvironmentDiffusion; }
        void SetWinAirAbsorptionHF( float value ) { m_winAirAbsorptionHF = value; }
        float GetWinAirAbsorptionHF() const { return m_winAirAbsorptionHF; }

    private:
        //Prevent wasteful constructor creation.
        reverbSettings( const reverbSettings& original );
        reverbSettings& operator=( const reverbSettings& rhs );

        //
        // Reverb parameters
        //
        float m_gain;
        float m_fadeInTime;
        float m_fadeOutTime;

        int m_xboxRoom;
        int m_xboxRoomHF;
        float m_xboxRoomRolloff;
        float m_xboxDecay;
        float m_xboxDecayHFRatio;
        int m_xboxReflections;
        float m_xboxReflectionsDelay;
        int m_xboxReverb;
        float m_xboxReverbDelay;
        float m_xboxDiffusion;
        float m_xboxDensity;
        float m_xboxHFReference;

        int m_ps2ReverbMode;
        float m_ps2Delay;
        float m_ps2Feedback;

        float m_gcPreDelay;
        float m_gcReverbTime;
        float m_gcColoration;
        float m_gcDamping;

        float m_winEnvironmentDiffusion;
        float m_winAirAbsorptionHF;
};

//=============================================================================
// Factory Functions
//=============================================================================

//
// Create a reverbSettings object
//
void ReverbSettingsObjCreate
(
    IReverbSettings** ppSettings,
    radMemoryAllocator allocator
);

#endif // REVERBSETTINGS_H

