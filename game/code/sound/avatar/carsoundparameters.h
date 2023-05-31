//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        carsoundparameters.h
//
// Description: Declaration for RadScript-created class for tunable car sound
//              parameters
//
// History:     30/06/2002 + Created -- Darren
//
//=============================================================================

#ifndef CARSOUNDPARAMETERS_H
#define CARSOUNDPARAMETERS_H

//========================================
// Nested Includes
//========================================

#include <radobject.hpp>
#include <radlinkedclass.hpp>

#include <sound/avatar/icarsoundparameters.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    carSoundParameters
//
//=============================================================================

class carSoundParameters: public ICarSoundParameters,
                          public radLinkedClass< carSoundParameters >,
                          public radRefCount
{
    public:
        IMPLEMENT_REFCOUNTED( "carSoundParameters" );

        static const int REVERSE_GEAR = -1;
        
        carSoundParameters();
        virtual ~carSoundParameters();

        void SetWatcherName( const char* name );

        void SetClipRPM( float rpm ) { m_clipRPM = rpm; }
        float GetClipRPM() { return( m_clipRPM ); }

        void SetEngineClipName( const char* clipName );
        const char* GetEngineClipName() { return( m_engineClipName ); }

        void SetEngineIdleClipName( const char* clipName );
        const char* GetEngineIdleClipName() { return( m_idleClipName ); }

        void SetDamagedEngineClipName( const char* clipName );
        const char* GetDamagedEngineClipName() { return( m_damagedClipName ); }

        void SetHornClipName( const char* clipName );
        const char* GetHornClipName() { return( m_hornClipName ); }

        void SetCarDoorOpenClipName( const char* clipName );
        const char* GetCarDoorOpenClipName() { return( m_carOpenClipName ); }

        void SetCarDoorCloseClipName( const char* clipName );
        const char* GetCarDoorCloseClipName() { return( m_carCloseClipName ); }

        void SetOverlayClipName( const char* clipName );
        const char* GetOverlayClipName() { return( m_overlayClipName ); }

        void SetRoadSkidClipName( const char* clipName );
        const char* GetRoadSkidClipName() { return( m_roadSkidClipName ); }

        void SetDirtSkidClipName( const char* clipName );
        const char* GetDirtSkidClipName() { return( m_dirtSkidClipName ); }

        void SetBackupClipName( const char* clipName );
        const char* GetBackupClipName() { return( m_backupClipName ); }

        void SetShiftPoint( unsigned int gear, float percent );
        float GetShiftPoint( int gear );

        void SetDownshiftDamperSize( float percent );
        float GetDownshiftDamperSize() { return( m_downshiftDamper ); }

        void SetGearPitchRange( unsigned int gear, float min, float max );

        void SetNumberOfGears( unsigned int gear );

        float GetMsecsPerOctaveCap() { return( m_msecsPerOctave ); }

        //
        // Attack/delay/decay
        //
        void SetAttackTimeMsecs( float msecs );
        float GetAttackTimeMsecs() { return( m_attackTime ); }

        void SetDelayTimeMsecs( unsigned int msecs );
        unsigned int GetDelayTimeMsecs() { return( m_delayTime ); }

        void SetDecayTimeMsecs( float msecs );
        float GetDecayTimeMsecs() { return( m_decayTime ); }

        void SetDecayFinishTrim( float trim );
        float GetDecayFinishTrim() { return( m_decayFinishTrim ); }

        //
        // Reverse
        //
        void SetReversePitchCapKmh( float speed );
        float GetReversePitchCapKmh() { return( m_maxReverseKmh ); }

        void SetReversePitchRange( float min, float max );
        void GetReversePitchRange( float& min, float& max ) 
            { min = m_minReversePitch; max = m_maxReversePitch; }

        //
        // Gear shifts
        //
        void SetGearShiftPitchDrop( unsigned int gear, float drop );
        float GetGearShiftPitchDrop( unsigned int gear );

        //
        // Damage
        //
        void SetDamageStartPcnt( float damagePercent );
        float GetDamageStartPcnt() { return( m_damageStartPcnt ); }

        void SetDamageMaxVolPcnt( float percent );
        float GetDamageVolumeRange() { return( m_damageVolumeRange ); }

        void SetDamageStartTrim( float trim );
        float GetDamageStartTrim() { return( m_damageStartTrim ); }
        void SetDamageMaxTrim( float trim );
        float GetDamageMaxTrim() { return( m_damageMaxTrim ); }

        //
        // Idle
        //
        void SetIdleEnginePitch( float pitch );
        float GetIdleEnginePitch() { return( m_idleEnginePitch ); }

        //
        // In-air sound characteristics
        //
        void SetInAirThrottlePitch( float pitch );
        float GetInAirThrottlePitch();

        void SetInAirIdlePitch( float pitch );
        float GetInAirIdlePitch();

        void SetInAirThrottleResponseTimeMsecs( unsigned int msecs );
        unsigned int GetInAirThrottleResponseTimeMsecs() { return( m_inAirResponseMsecs ); }

        //
        // Burnout sound characteristics
        //
        void SetBurnoutMinPitch( float pitch );
        float GetBurnoutMinPitch() { return( m_burnoutMinPitch ); }

        void SetBurnoutMaxPitch( float pitch );
        float GetBurnoutMaxPitch() { return( m_burnoutMaxPitch ); }

        //
        // Powerslide sound characteristics
        //
        void SetPowerslideMinPitch( float pitch );
        float GetPowerslideMinPitch();

        void SetPowerslideMaxPitch( float pitch );
        float GetPowerslideMaxPitch();

        //
        // Given a vehicle's current speed and max speed, figure out the
        // gear it's in
        //
        int CalculateCurrentGear( float currentSpeed, float previousSpeed, 
                                  float topSpeed, int previousGear );

        //
        // Given a vehicle's current gear, speed, and max speed, figure out the
        // pitch for the engine clip
        //
        float CalculateEnginePitch( int gear, float currentSpeed, float topSpeed );

        float GetRevLimit();

    private:
        //Prevent wasteful constructor creation.
        carSoundParameters( const carSoundParameters& original );
        carSoundParameters& operator=( const carSoundParameters& rhs );

        static const unsigned int MAX_GEARS = 6;

        float m_clipRPM;

        float m_minPitch[MAX_GEARS];
        float m_maxPitch[MAX_GEARS];

        char* m_engineClipName;
        char* m_idleClipName;
        char* m_damagedClipName;
        char* m_hornClipName;
        char* m_carOpenClipName;
        char* m_carCloseClipName;
        char* m_overlayClipName;
        char* m_roadSkidClipName;
        char* m_dirtSkidClipName;
        char* m_backupClipName;

        //
        // Percentages of top speed at which we shift gears.
        // E.g. m_shiftPoints[0] == 0.01f means that we shift from idle
        // to first at 1% of top speed.  m_shiftPoints[MAX_GEARS+1] is
        // 1.0f to simplify math.
        //

        // Used for shifting
        float m_shiftPoints[MAX_GEARS+1];
        float m_downshiftDamper;

        //
        // Attack/delay/decay
        //
        float m_attackTime;
        unsigned int m_delayTime;
        float m_decayTime;

        float m_decayFinishTrim;

        //
        // Reverse
        //
        float m_maxReverseKmh;
        float m_minReversePitch;
        float m_maxReversePitch;

        //
        // Gear shifts
        //
        float m_gearShiftPitchDrop[MAX_GEARS];
        unsigned int m_gearShiftTimeMsecs[MAX_GEARS];

        //
        // Damage
        //
        float m_damageStartPcnt;
        float m_damageVolumeRange;

        float m_damageStartTrim;
        float m_damageMaxTrim;

        //
        // Idle
        //
        float m_idleEnginePitch;

        //
        // Airborne
        //
        float m_inAirThrottlePitch;
        float m_inAirIdlePitch;
        unsigned int m_inAirResponseMsecs;

        //
        // Burnout
        //
        float m_burnoutMinPitch;
        float m_burnoutMaxPitch;

        //
        // Powerslide
        //
        float m_powerslideMinPitch;
        float m_powerslideMaxPitch;

        //
        // Makes transitions a little less abrupt
        //
        float m_msecsPerOctave;
};

//=============================================================================
// Factory Functions
//=============================================================================

//
// Create a CarSoundParameters object
//
void CarSoundParameterObjCreate
(
    ICarSoundParameters** ppSoundResource,
    radMemoryAllocator allocator
);




#endif // CARSOUNDPARAMETERS_H

