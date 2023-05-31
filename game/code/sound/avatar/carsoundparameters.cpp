//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        carsoundparameters.cpp
//
// Description: Implement carSoundParameters
//
// History:     01/07/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <string.h>

#include <raddebugwatch.hpp>

//========================================
// Project Includes
//========================================
#include <sound/avatar/carsoundparameters.h>

#include <memory/srrmemory.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//
// Initialially the list is empty
//
carSoundParameters* radLinkedClass< carSoundParameters >::s_pLinkedClassHead = NULL;
carSoundParameters* radLinkedClass< carSoundParameters >::s_pLinkedClassTail = NULL;

static float s_maxPitchDefault = 5.0f;
static float s_inAirIdleDefault = 0.2f;
static float s_inAirThrottleDefault = 2.0f;
static float s_powerslideMinDefault = 0.2f;
static float s_powerslideMaxDefault = 2.0f;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// carSoundParameters::carSoundParameters
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
carSoundParameters::carSoundParameters() :
    radRefCount( 0 ),
    m_clipRPM( 3000 ),
    m_engineClipName( NULL ),
    m_idleClipName( NULL ),
    m_damagedClipName( NULL ),
    m_hornClipName( NULL ),
    m_carOpenClipName( NULL ),
    m_carCloseClipName( NULL ),
    m_overlayClipName( NULL ),
    m_roadSkidClipName( NULL ),
    m_dirtSkidClipName( NULL ),
    m_backupClipName( NULL ),
    m_downshiftDamper( 0.05f ),
    m_attackTime( 100 ),
    m_delayTime( 50 ),
    m_decayTime( 200 ),
    m_decayFinishTrim( 0.75f ),
    m_maxReverseKmh( 50.0f ),
    m_minReversePitch( 1.5f ),
    m_maxReversePitch( 3.0f ),
    m_damageStartPcnt( 0.4f ),
    m_damageVolumeRange( m_damageStartPcnt ),
    m_damageStartTrim( 0.0f ),
    m_damageMaxTrim( 1.0f ),
    m_idleEnginePitch( 1.0f ),
    m_inAirThrottlePitch( s_inAirThrottleDefault ),
    m_inAirIdlePitch( s_inAirIdleDefault ),
    m_inAirResponseMsecs( 500 ),
    m_burnoutMinPitch( 0.5f ),
    m_burnoutMaxPitch( 1.3f ),
    m_powerslideMinPitch( s_powerslideMinDefault ),
    m_powerslideMaxPitch( s_powerslideMaxDefault ),
    m_msecsPerOctave( 500 )
{
    unsigned int i;

    //
    // Shift point defaults
    //
    m_shiftPoints[0] = 0.01f;
    m_shiftPoints[1] = 0.30f;
    m_shiftPoints[2] = 0.45f;
    m_shiftPoints[3] = 0.70f;
    m_shiftPoints[4] = 0.85f;
    m_shiftPoints[5] = 0.95f;

    m_shiftPoints[MAX_GEARS] = 1.0f;

    //
    // Pitch range and gear shift defaults
    //
    for( i = 0; i < MAX_GEARS; i++ )
    {
        m_minPitch[i] = 0.5f;
        m_maxPitch[i] = s_maxPitchDefault;

        m_gearShiftPitchDrop[i] = 0.2f;
    }
}

//==============================================================================
// carSoundParameters::~carSoundParameters
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
carSoundParameters::~carSoundParameters()
{
    if( m_engineClipName != NULL )
    {
        delete [] m_engineClipName;
    }
    if( m_idleClipName != NULL )
    {
        delete [] m_idleClipName;
    }
    if( m_damagedClipName != NULL )
    {
        delete [] m_damagedClipName;
    }
    if( m_hornClipName != NULL )
    {
        delete [] m_hornClipName;
    }
    if( m_carOpenClipName != NULL )
    {
        delete [] m_carOpenClipName;
    }
    if( m_carCloseClipName != NULL )
    {
        delete [] m_carCloseClipName;
    }
    if( m_overlayClipName != NULL )
    {
        delete [] m_overlayClipName;
    }
    if( m_roadSkidClipName != NULL )
    {
        delete [] m_roadSkidClipName;
    }
    if( m_dirtSkidClipName != NULL )
    {
        delete [] m_dirtSkidClipName;
    }
    if( m_backupClipName != NULL )
    {
        delete [] m_backupClipName;
    }
}

//=============================================================================
// carSoundParameters::SetWatcherName
//=============================================================================
// Description: Set the name of this object, since it's usually stored
//              in radScript's data structures.  FOR DEBUGGING ONLY.  We use
//              this to give the car a name for Watcher tuning.
//
// Parameters:  name - name of car
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetWatcherName( const char* name )
{
#ifndef RAD_RELEASE
    unsigned int i;
    char label[50];
    char watcherGroup[50];
    
    sprintf( watcherGroup, "Sound::%s", name );
    
    //
    // Register members with Watcher for tweakage
    //
    for( i = 1; i <= MAX_GEARS; i++ )
    {
        sprintf( label, "Min pitch: gear %d", i );
        radDbgWatchAddFloat( &m_minPitch[i-1], label, watcherGroup, NULL, NULL, 0.0f, 10.0f );
        
        sprintf( label, "Max pitch: gear %d", i );
        radDbgWatchAddFloat( &m_maxPitch[i-1], label, watcherGroup, NULL, NULL, 0.0f, 10.0f );
        
        sprintf( label, "Pitch drop: gear %d", i );
        radDbgWatchAddFloat( &m_gearShiftPitchDrop[i-1], label, watcherGroup, NULL, NULL, 0.0f, 10.0f );
    }
        
    for( i = 1; i <= MAX_GEARS + 1; i++ )
    {
        sprintf( label, "Shift point: gear %d", i );
        radDbgWatchAddFloat( &m_shiftPoints[i-1], label, watcherGroup );
    }
    
    radDbgWatchAddFloat( &m_downshiftDamper, "Downshift damper", watcherGroup );
    radDbgWatchAddFloat( &m_attackTime, "Attack time (msecs)", watcherGroup, NULL, NULL, 0, 2000.0f );
    radDbgWatchAddUnsignedInt( &m_delayTime, "Delay time (msecs)", watcherGroup, NULL, NULL, 0, 2000 );
    radDbgWatchAddFloat( &m_decayTime, "Decay time (msecs)", watcherGroup, NULL, NULL, 0, 2000.0f );
    radDbgWatchAddFloat( &m_decayFinishTrim, "Decay finish trim", watcherGroup );
    radDbgWatchAddFloat( &m_maxReverseKmh, "Max reverse kmh", watcherGroup, NULL, NULL, 0.0f, 200.0f );
    radDbgWatchAddFloat( &m_minReversePitch, "Min reverse pitch", watcherGroup, NULL, NULL, 0.0f, 10.0f );
    radDbgWatchAddFloat( &m_maxReversePitch, "Max reverse pitch", watcherGroup, NULL, NULL, 0.0f, 10.0f );
    radDbgWatchAddFloat( &m_damageStartPcnt, "Damage start", watcherGroup );
    radDbgWatchAddFloat( &m_damageVolumeRange, "Damage max volume life %", watcherGroup );
    radDbgWatchAddFloat( &m_damageStartTrim, "Damage start trim", watcherGroup );
    radDbgWatchAddFloat( &m_damageMaxTrim, "Damage max trim", watcherGroup );
    radDbgWatchAddFloat( &m_idleEnginePitch, "Idle engine pitch", watcherGroup, NULL, NULL, 0.0f, 10.0f );
    radDbgWatchAddFloat( &m_inAirThrottlePitch, "In air throttle pitch", watcherGroup, NULL, NULL, 0.0f, 10.0f );
    radDbgWatchAddFloat( &m_inAirIdlePitch, "In air idle pitch", watcherGroup, NULL, NULL, 0.0f, 10.0f );
    radDbgWatchAddUnsignedInt( &m_inAirResponseMsecs, "In air response time (msecs)", watcherGroup, NULL, NULL, 0, 2000 );
#endif
}

//=============================================================================
// carSoundParameters::SetShiftPoint
//=============================================================================
// Description: Set the percentage of top speed at which we shift to a
//              given gear
//
// Parameters:  gear - gear to be shifted to
//              lowPercent - percentage of top speed at which we downshift
//              highPercent - percentage of top speed at which we upshift
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetShiftPoint( unsigned int gear, float percent )
{
    rAssert( gear > 0 );
    rAssert( gear <= MAX_GEARS );
    rAssert( percent > 0.0f );
    rAssert( percent <= 1.0f );

    m_shiftPoints[gear-1] = percent;
}

//=============================================================================
// carSoundParameters::GetShiftPoint
//=============================================================================
// Description: Get the percentage of top speed at which we shift up from a
//              given gear
//
// Parameters:  gear - gear we're currently in
//
// Return:      shift point as percentage of top speed 
//
//=============================================================================
float carSoundParameters::GetShiftPoint( int gear )
{
    if( gear > 0 )
    {
        return( m_shiftPoints[gear-1] );
    }
    else
    {
        return( 0.0f );
    }
}

//=============================================================================
// carSoundParameters::SetAttackTimeMsecs
//=============================================================================
// Description: Comment
//
// Parameters:  ( float msecs )
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetAttackTimeMsecs( float msecs )
{
    m_attackTime = msecs;
}

//=============================================================================
// carSoundParameters::SetDelayTimeMsecs
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int msecs )
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetDelayTimeMsecs( unsigned int msecs )
{
    m_delayTime = msecs;
}

//=============================================================================
// carSoundParameters::SetDecayTimeMsecs
//=============================================================================
// Description: Comment
//
// Parameters:  ( float msecs )
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetDecayTimeMsecs( float msecs )
{
    m_decayTime = msecs;
}

//=============================================================================
// carSoundParameters::SetDecayFinishTrim
//=============================================================================
// Description: Comment
//
// Parameters:  ( float trim )
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetDecayFinishTrim( float trim )
{
    m_decayFinishTrim = trim;
}

//=============================================================================
// carSoundParameters::SetDownshiftDamperSize
//=============================================================================
// Description: Amount that we can drop below the shift point to avoid
//              downshifting
//
// Parameters:  percent - percentage of top speed that acts as the buffer
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetDownshiftDamperSize( float percent )
{
    rAssert( percent >= 0.0f );
    rAssert( percent <= 1.0f );

    m_downshiftDamper = percent;
}

//=============================================================================
// carSoundParameters::SetEngineClipName
//=============================================================================
// Description: Store the name of the sound resource we're using for engine
//              sounds
//
// Parameters:  clipName - name of sound resource
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetEngineClipName( const char* clipName )
{
    rAssert( clipName != NULL );

    HeapMgr()->PushHeap( GMA_PERSISTENT );

    m_engineClipName = new char[strlen(clipName)+1];
    strcpy( m_engineClipName, clipName );

    HeapMgr()->PopHeap(GMA_PERSISTENT);
}

//=============================================================================
// carSoundParameters::SetEngineIdleClipName
//=============================================================================
// Description: Store the name of the sound resource we're using for engine
//              idle sounds
//
// Parameters:  clipName - name of sound resource
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetEngineIdleClipName( const char* clipName )
{
    rAssert( clipName != NULL );

    HeapMgr()->PushHeap( GMA_PERSISTENT );

    m_idleClipName = new char[strlen(clipName)+1];
    strcpy( m_idleClipName, clipName );

    HeapMgr()->PopHeap(GMA_PERSISTENT);
}

//=============================================================================
// carSoundParameters::SetDamagedEngineClipName
//=============================================================================
// Description: Store the name of the sound resource we're using for damaged
//              engine sounds
//
// Parameters:  clipName - name of sound resource
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetDamagedEngineClipName( const char* clipName )
{
    rAssert( clipName != NULL );

    HeapMgr()->PushHeap( GMA_PERSISTENT );

    m_damagedClipName = new char[strlen(clipName)+1];
    strcpy( m_damagedClipName, clipName );

    HeapMgr()->PopHeap(GMA_PERSISTENT);
}

//=============================================================================
// carSoundParameters::SetHornClipName
//=============================================================================
// Description: Store the name of the sound resource we're using for horn
//              sounds
//
// Parameters:  clipName - name of sound resource
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetHornClipName( const char* clipName )
{
    rAssert( clipName != NULL );

    HeapMgr()->PushHeap( GMA_PERSISTENT );

    m_hornClipName = new char[strlen(clipName)+1];
    strcpy( m_hornClipName, clipName );

    HeapMgr()->PopHeap(GMA_PERSISTENT);
}

//=============================================================================
// carSoundParameters::SetCarDoorOpenClipName
//=============================================================================
// Description: Store the name of the sound resource we're using for car
//              door opening sounds
//
// Parameters:  clipName - name of sound resource
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetCarDoorOpenClipName( const char* clipName )
{
    rAssert( clipName != NULL );

    HeapMgr()->PushHeap( GMA_PERSISTENT );

    m_carOpenClipName = new char[strlen(clipName)+1];
    strcpy( m_carOpenClipName, clipName );

    HeapMgr()->PopHeap(GMA_PERSISTENT);
}

//=============================================================================
// carSoundParameters::SetCarDoorCloseClipName
//=============================================================================
// Description: Store the name of the sound resource we're using for car
//              door closing sounds
//
// Parameters:  clipName - name of sound resource
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetCarDoorCloseClipName( const char* clipName )
{
    rAssert( clipName != NULL );

    HeapMgr()->PushHeap( GMA_PERSISTENT );

    m_carCloseClipName = new char[strlen(clipName)+1];
    strcpy( m_carCloseClipName, clipName );

    HeapMgr()->PopHeap(GMA_PERSISTENT);
}

//=============================================================================
// carSoundParameters::SetOverlayClipName
//=============================================================================
// Description: Store the name of the sound resource we want to play along
//              with the specified engine sound.  Is optional.
//
// Parameters:  clipName - name of sound resource
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetOverlayClipName( const char* clipName )
{
    rAssert( clipName != NULL );

    HeapMgr()->PushHeap( GMA_PERSISTENT );

    m_overlayClipName = new char[strlen(clipName)+1];
    strcpy( m_overlayClipName, clipName );

    HeapMgr()->PopHeap(GMA_PERSISTENT);
}

//=============================================================================
// carSoundParameters::SetRoadSkidClipName
//=============================================================================
// Description: Store the name of the sound resource we want to play when
//              the car skids on the road.  Is optional.
//
// Parameters:  clipName - name of sound resource
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetRoadSkidClipName( const char* clipName )
{
    rAssert( clipName != NULL );

    HeapMgr()->PushHeap( GMA_PERSISTENT );

    m_roadSkidClipName = new char[strlen(clipName)+1];
    strcpy( m_roadSkidClipName, clipName );

    HeapMgr()->PopHeap(GMA_PERSISTENT);
}

//=============================================================================
// carSoundParameters::SetDirtSkidClipName
//=============================================================================
// Description: Store the name of the sound resource we want to play when
//              the car skids.  Is optional.
//
// Parameters:  clipName - name of sound resource
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetDirtSkidClipName( const char* clipName )
{
    rAssert( clipName != NULL );

    HeapMgr()->PushHeap( GMA_PERSISTENT );

    m_dirtSkidClipName = new char[strlen(clipName)+1];
    strcpy( m_dirtSkidClipName, clipName );

    HeapMgr()->PopHeap(GMA_PERSISTENT);
}

//=============================================================================
// carSoundParameters::SetBackupClipName
//=============================================================================
// Description: Store the name of the sound resource we want to play when
//              the car goes into reverse.  Is optional.
//
// Parameters:  clipName - name of sound resource
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetBackupClipName( const char* clipName )
{
    rAssert( clipName != NULL );

    HeapMgr()->PushHeap( GMA_PERSISTENT );

    m_backupClipName = new char[strlen(clipName)+1];
    strcpy( m_backupClipName, clipName );

    HeapMgr()->PopHeap(GMA_PERSISTENT);
}

//=============================================================================
// carSoundParameters::SetGearPitchRange
//=============================================================================
// Description: Set pitch range for engine clip for given gear
//
// Parameters:  gear - gear to apply range to
//              min - pitch at lowest gear RPMs
//              max - pitch at highest gear RPMs
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetGearPitchRange( unsigned int gear, float min, float max )
{
    rAssert( gear > 0 );
    rAssert( gear <= MAX_GEARS );
    rAssert( max >= min );

    m_minPitch[gear-1] = min;
    m_maxPitch[gear-1] = max;
}

//=============================================================================
// carSoundParameters::SetNumberOfGears
//=============================================================================
// Description: Like the name says
//
// Parameters:  gear - number of gears we want, must be <= MAX_GEARS
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetNumberOfGears( unsigned int gear )
{
    rAssert( gear <= MAX_GEARS );

    //
    // If we set the shift point for the given gear to 1.0f, we're 
    // effectively cutting the number of gears
    //
    m_shiftPoints[gear] = 1.0f;
}

//=============================================================================
// carSoundParameters::SetReversePitchCapKmh
//=============================================================================
// Description: Set maximum reverse speed at which we'll increase engine
//              pitch
//
// Parameters:  speed - max reverse speed
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetReversePitchCapKmh( float speed )
{
    rAssert( speed > 0.0f );

    m_maxReverseKmh = speed;
}

//=============================================================================
// carSoundParameters::SetReversePitchRange
//=============================================================================
// Description: Set range of engine pitch for reverse gear
//
// Parameters:  min - pitch at rest
//              max - pitch at m_maxReverseKmh
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetReversePitchRange( float min, float max )
{
    rAssert( min < max );

    m_minReversePitch = min;
    m_maxReversePitch = max;
}

//=============================================================================
// carSoundParameters::SetGearShiftPitchDrop
//=============================================================================
// Description: Set amount of engine clip pitch drop we'll do on gear shifts
//
// Parameters:  gear - gear that we want to set drop for
//              drop - amount of pitch drop
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetGearShiftPitchDrop( unsigned int gear, float drop )
{
    rAssert( gear > 0 );
    rAssert( gear <= MAX_GEARS );
    rAssert( drop >= 0.0f );

    m_gearShiftPitchDrop[gear-1] = drop;
}

//=============================================================================
// carSoundParameters::GetGearShiftPitchDrop
//=============================================================================
// Description: Get amount of engine clip pitch drop we'll do on gear shifts
//
// Parameters:  gear - gear that we want drop for
//
// Return:      value of pitch drop 
//
//=============================================================================
float carSoundParameters::GetGearShiftPitchDrop( unsigned int gear )
{
    rAssert( gear > 0 );
    rAssert( gear <= MAX_GEARS );

    return( m_gearShiftPitchDrop[gear-1] );
}

//=============================================================================
// carSoundParameters::SetDamageStartPcnt
//=============================================================================
// Description: Set vehicle life percentage at which we'll start playing
//              damage sounds
//
// Parameters:  damagePercent - vehicle life percentage for damage sound
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetDamageStartPcnt( float damagePercent )
{
    rAssert( damagePercent >= 0.0f );
    rAssert( damagePercent <= 1.0f );

    // Also need to adjust damage volume range when start pcnt changes
    m_damageVolumeRange += damagePercent - m_damageStartPcnt;

    m_damageStartPcnt = damagePercent;
}

//=============================================================================
// carSoundParameters::SetDamageMaxVolPcnt
//=============================================================================
// Description: Set vehicle life percentage at which the damage clip will
//              be played at maximum volume (linear interpolation between
//              m_damageStartPcnt and m_damageMaxVolPcnt).
//
// Parameters:  percent - vehicle life percentage for maximum volume
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetDamageMaxVolPcnt( float percent )
{
    rAssert( percent >= 0.0f );
    rAssert( percent <= 1.0f );

    //
    // We don't actually store the percentage, since the trim setting code
    // only cares about the min/max range; might as well precalculate it
    m_damageVolumeRange = m_damageStartPcnt - percent;
}

//=============================================================================
// carSoundParameters::SetDamageStartTrim
//=============================================================================
// Description: Set volume for damage clip when vehicle life percentage
//              reaches damage start percentage
//
// Parameters:  trim - initial volume
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetDamageStartTrim( float trim )
{
    rAssert( trim >= 0.0f );
    rAssert( trim <= 1.0f );

    m_damageStartTrim = trim;
}

//=============================================================================
// carSoundParameters::SetDamageMaxTrim
//=============================================================================
// Description: Set volume for damage clip when vehicle life percentage
//              reaches damage max percentage
//
// Parameters:  trim - initial volume
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetDamageMaxTrim( float trim )
{
    rAssert( trim >= 0.0f );
    rAssert( trim <= 1.0f );

    m_damageMaxTrim = trim;
}

//=============================================================================
// carSoundParameters::SetIdleEnginePitch
//=============================================================================
// Description: Set pitch value for idle engine clip
//
// Parameters:  pitch - obvious
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetIdleEnginePitch( float pitch )
{
    m_idleEnginePitch = pitch;
}

//=============================================================================
// carSoundParameters::SetInAirThrottlePitch
//=============================================================================
// Description: Set pitch that engine goes to when gas applied in air
//
// Parameters:  pitch - engine pitch
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetInAirThrottlePitch( float pitch )
{
    m_inAirThrottlePitch = pitch;
}

//=============================================================================
// carSoundParameters::GetInAirThrottlePitch
//=============================================================================
// Description: Get pitch that engine goes to when gas applied in air
//
// Parameters:  None
//
// Return:      full-throttle pitch 
//
//=============================================================================
float carSoundParameters::GetInAirThrottlePitch()
{
    //
    // POST-BETA HACK!!
    //
    if( m_inAirIdlePitch == s_inAirIdleDefault )
    {
        //
        // Untuned, d'oh.  Make something up.
        //
        m_inAirThrottlePitch = GetRevLimit();
    }

    return( m_inAirThrottlePitch );
}

//=============================================================================
// carSoundParameters::SetInAirIdlePitch
//=============================================================================
// Description: Set pitch that engine goes to when engine idling in air
//
// Parameters:  pitch - engine pitch
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetInAirIdlePitch( float pitch )
{
    m_inAirIdlePitch = pitch;
}

//=============================================================================
// carSoundParameters::GetInAirIdlePitch
//=============================================================================
// Description: Get pitch that engine goes to when engine idling in air
//
// Parameters:  None
//
// Return:      idle pitch 
//
//=============================================================================
float carSoundParameters::GetInAirIdlePitch()
{
    //
    // POST-BETA HACK!!
    //
    if( m_inAirIdlePitch == s_inAirIdleDefault )
    {
        //
        // Untuned, d'oh.  Make something up as percentage of rev limit
        //
        m_inAirIdlePitch = 0.5f * GetRevLimit();
    }

    return( m_inAirIdlePitch );
}

//=============================================================================
// carSoundParameters::SetInAirThrottleResponseTimeMsecs
//=============================================================================
// Description: Set amount of time to go from throttle pitch to idle pitch
//              in midair and vice versa
//
// Parameters:  msecs - time in milliseconds
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetInAirThrottleResponseTimeMsecs( unsigned int msecs )
{
    m_inAirResponseMsecs = msecs;
}

//=============================================================================
// carSoundParameters::SetBurnoutMinPitch
//=============================================================================
// Description: Set pitch for engine when burnout factor is at minimum
//              value
//
// Parameters:  pitch - pitch to apply to sound clip
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetBurnoutMinPitch( float pitch )
{
    m_burnoutMinPitch = pitch;
}

//=============================================================================
// carSoundParameters::SetBurnoutMaxPitch
//=============================================================================
// Description: Set pitch for engine when burnout factor is at maximum
//              value
//
// Parameters:  pitch - pitch to apply to sound clip
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetBurnoutMaxPitch( float pitch )
{
    m_burnoutMaxPitch = pitch;
}

//=============================================================================
// carSoundParameters::SetPowerslideMinPitch
//=============================================================================
// Description: Set pitch for engine when powerslide factor is at minimum
//              value
//
// Parameters:  pitch - pitch to apply to sound clip
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetPowerslideMinPitch( float pitch )
{
    m_powerslideMinPitch = pitch;
}

//=============================================================================
// carSoundParameters::GetPowerslideMinPitch
//=============================================================================
// Description: Get pitch for engine when powerslide factor is at minimum
//              value
//
// Parameters:  None
//
// Return:      min pitch 
//
//=============================================================================
float carSoundParameters::GetPowerslideMinPitch()
{
    if( m_powerslideMinPitch == s_powerslideMinDefault )
    {
        m_powerslideMinPitch = 0.5f * GetRevLimit();
    }

    return( m_powerslideMinPitch );
}

//=============================================================================
// carSoundParameters::SetPowerslideMaxPitch
//=============================================================================
// Description: Set pitch for engine when powerslide factor is at maximum
//              value
//
// Parameters:  pitch - pitch to apply to sound clip
//
// Return:      void 
//
//=============================================================================
void carSoundParameters::SetPowerslideMaxPitch( float pitch )
{
    m_powerslideMaxPitch = pitch;
}

//=============================================================================
// carSoundParameters::GetPowerslideMaxPitch
//=============================================================================
// Description: Get pitch for engine when powerslide factor is at maximum
//              value
//
// Parameters:  None
//
// Return:      max pitch 
//
//=============================================================================
float carSoundParameters::GetPowerslideMaxPitch()
{
    if( m_powerslideMaxPitch == s_powerslideMaxDefault )
    {
        m_powerslideMaxPitch = 0.9f * GetRevLimit();
    }

    return( m_powerslideMaxPitch );
}

//=============================================================================
// carSoundParameters::CalculateEnginePitch
//=============================================================================
// Description: Given some car data, figure out the pitch we should be playing
//              the clip at
//
// Parameters:  gear - gear we're currently in
//              currentSpeed - how fast the car is travelling
//              topSpeed - top end for this car
//
// Return:      pitch as a value from 0.0f to 1.0f
//
//=============================================================================
float carSoundParameters::CalculateEnginePitch( int gear, float currentSpeed, float topSpeed )
{
    float enginePitch;
    float percentageOfGear;
    float bottomEndSpeed;

    //rAssert( currentSpeed <= topSpeed );
    rAssert( ( gear > 0 ) || ( gear == REVERSE_GEAR ) );
    rAssert( gear <= static_cast<int>(MAX_GEARS) );

    if( gear == REVERSE_GEAR )
    {
        percentageOfGear = currentSpeed / m_maxReverseKmh;
        if( percentageOfGear > 1.0f )
        {
            percentageOfGear = 1.0f;
        }

        enginePitch = m_minReversePitch + ( percentageOfGear * ( m_maxReversePitch - m_minReversePitch ) );
    }
    else
    {
        bottomEndSpeed = m_shiftPoints[gear-1] * topSpeed;

        //
        // m_shiftPoints[gear+1] is allowed because we set m_shiftPoints[MAX_GEARS] to 1.0f
        //
        percentageOfGear = ( ( currentSpeed - bottomEndSpeed )
                             / ( ( m_shiftPoints[gear] * topSpeed ) - bottomEndSpeed ) );

        //
        // Based on our speed position within the range of speed used for this gear, apply that
        // same percentage to the pitch range for our final pitch calculation
        //
        enginePitch = m_minPitch[gear-1] + ( ( m_maxPitch[gear-1] - m_minPitch[gear-1] ) * percentageOfGear );
    }

    return( enginePitch );
}

//=============================================================================
// carSoundParameters::CalculateCurrentGear
//=============================================================================
// Description: Given our speed, figure out what gear we're in
//
// Parameters:  currentSpeed - how fast the car is travelling
//              previousSpeed - how fast the car was going in the previous frame
//                              (to see if we're upshifting or downshifting)
//              topSpeed - top end for this car
//              previousGear - which gear we were in before (used to cut down
//                             on spurious gearshifts).  If unknown, use -1.
//
// Return:      0 for idle, 1 to MAX_GEARS for forward gears
//
//=============================================================================
int carSoundParameters::CalculateCurrentGear( float currentSpeed,
                                              float previousSpeed,
                                              float topSpeed,
                                              int previousGear )
{
    unsigned int currentGear;
    unsigned int uiPreviousGear;

    //
    // Find our current gear
    //
    for( currentGear = 0; currentGear < MAX_GEARS; currentGear++ )
    {
        if( currentSpeed <= ( topSpeed * m_shiftPoints[currentGear] ) )
        {
            break;
        }
    }

    //
    // Don't downshift if we're accelerating, and vice versa.  If
    // previousGear is -1, we'll accept the gearshift anyway
    //
    if( previousGear >= 0 )
    {
        uiPreviousGear = static_cast<unsigned int>(previousGear);

        if( ( currentSpeed > previousSpeed )
            && ( currentGear < uiPreviousGear ) )
        {
            currentGear = uiPreviousGear;
        }
        else if( ( currentSpeed <= previousSpeed )
                 && ( currentGear > uiPreviousGear ) )
        {
            currentGear = uiPreviousGear;
        }
    }

    return( static_cast<int>(currentGear) );
}

//=============================================================================
// carSoundParameters::GetRevLimit
//=============================================================================
// Description: Try to calculate a sensible maximum pitch
//
// Parameters:  None
//
// Return:      Rev limit as pitch value  
//
//=============================================================================
float carSoundParameters::GetRevLimit()
{
    unsigned int i;
    float revLimit = 0.0f;

    for( i = 0; i < MAX_GEARS; i++ )
    {
        //
        // Ignore the default values of 2.0f, probably too high
        //
        if( ( m_maxPitch[i] > revLimit )
            && ( m_maxPitch[i] < s_maxPitchDefault ) )
        {
            revLimit = m_maxPitch[i];
        }
    }

    return( revLimit );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************


//******************************************************************************
// Factory functions
//******************************************************************************

//==============================================================================
// CarSoundParameterObjCreate
//==============================================================================
// Description: Factory function for creating carSoundParameters objects.
//              Called by RadScript.
//
// Parameters:	ppParametersObj - Address of ptr to new object
//              allocator - FTT pool to allocate object within
//
// Return:      N/A.
//
//==============================================================================
void CarSoundParameterObjCreate
(
    ICarSoundParameters** ppParametersObj,
    radMemoryAllocator allocator
)
{
    rAssert( ppParametersObj != NULL );
    (*ppParametersObj) = new ( allocator ) carSoundParameters( );
    (*ppParametersObj)->AddRef( );
}

