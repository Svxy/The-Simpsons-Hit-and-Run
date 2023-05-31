//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        vehiclesoundplayer.cpp
//
// Description: Implement VehicleSoundPlayer
//
// History:     30/06/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <float.h>

#include <radnamespace.hpp>

//========================================
// Project Includes
//========================================
#include <sound/avatar/vehiclesoundplayer.h>

#include <sound/avatar/carsoundparameters.h>
#include <sound/tuning/globalsettings.h>
#include <sound/soundrenderer/soundrenderingmanager.h>
#include <sound/soundrenderer/idasoundresource.h>
#include <sound/soundrenderer/soundresourcemanager.h>
#include <sound/soundmanager.h>

#include <worldsim/redbrick/vehicle.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/redbrick/vehiclecontroller/vehiclecontroller.h>

#include <input/button.h>
#include <events/eventmanager.h>
#include <mission/gameplaymanager.h>



//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//
// For tuning
//
static bool s_resetEngineOnHonk = false;
static bool s_resetEngineInitialized = false;

static const char* s_skidResource = "skid";
static const char* s_gearResource = "gearshift";

static const float GAME_POWERSLIDE_TRIM_MIN = 0.6f;
static const float GAME_POWERSLIDE_TRIM_MAX = 0.9f;

static const float SUPERSPRINT_POWERSLIDE_TRIM_MIN = 0.4f;
static const float SUPERSPRINT_POWERSLIDE_TRIM_MAX = 0.7f;

//
// MACROS FOR DEBUG INFO
//
#ifdef SOUND_DEBUG_INFO_ENABLED

#define SET_SHIFT_IN_PROGRESS(inProgress) m_debugInfo->SetShiftInProgress(inProgress)
#define SET_CURRENT_GEAR(gear) m_debugInfo->SetCurrentGear(gear)
#define SET_CURRENT_SPEED(speed) m_debugInfo->SetCurrentSpeed(speed)
#define SET_SHIFT_TIME_REMAINING(time) m_debugInfo->SetShiftTime(time)
#define SET_DOWNSHIFT_SPEED(speed) m_debugInfo->SetDownshiftSpeed(speed)
#define SET_UPSHIFT_SPEED(speed) m_debugInfo->SetUpshiftSpeed(speed)
#define SET_CURRENT_PITCH(pitch) m_debugInfo->SetCurrentPitch(pitch)
#define SET_IS_DAMAGED(isDamaged) m_debugInfo->SetDamageEnabled(isDamaged)
#define SET_VEHICLE_LIFE(life) m_debugInfo->SetVehicleLife(life)
#define SET_DAMAGE_THRESHOLD(threshold) m_debugInfo->SetDamageThreshold(threshold)

//
// This is annoying.  Stinky bad design.
//
#define SET_LOCAL_SHIFT_IN_PROGRESS(inProgress) m_debugInfo.SetShiftInProgress(inProgress)
#define SET_LOCAL_CURRENT_GEAR(gear) m_debugInfo.SetCurrentGear(gear)
#define SET_LOCAL_CURRENT_SPEED(speed) m_debugInfo.SetCurrentSpeed(speed)
#define SET_LOCAL_SHIFT_TIME_REMAINING(time) m_debugInfo.SetShiftTime(time)
#define SET_LOCAL_DOWNSHIFT_SPEED(speed) m_debugInfo.SetDownshiftSpeed(speed)
#define SET_LOCAL_UPSHIFT_SPEED(speed) m_debugInfo.SetUpshiftSpeed(speed)
#define SET_LOCAL_CURRENT_PITCH(pitch) m_debugInfo.SetCurrentPitch(pitch)
#define SET_LOCAL_IS_DAMAGED(isDamaged) m_debugInfo.SetDamageEnabled(isDamaged)
#define SET_LOCAL_VEHICLE_LIFE(life) m_debugInfo.SetVehicleLife(life)
#define SET_LOCAL_DAMAGE_THRESHOLD(threshold) m_debugInfo.SetDamageThreshold(threshold)

#else

#define SET_SHIFT_IN_PROGRESS(inProgress)
#define SET_CURRENT_GEAR(gear)
#define SET_CURRENT_SPEED(speed)
#define SET_SHIFT_TIME_REMAINING(time)
#define SET_DOWNSHIFT_SPEED(speed)
#define SET_UPSHIFT_SPEED(speed)
#define SET_CURRENT_PITCH(pitch)
#define SET_IS_DAMAGED(isDamaged)
#define SET_VEHICLE_LIFE(life)
#define SET_DAMAGE_THRESHOLD(threshold)

#define SET_LOCAL_SHIFT_IN_PROGRESS(inProgress)
#define SET_LOCAL_CURRENT_GEAR(gear)
#define SET_LOCAL_CURRENT_SPEED(speed)
#define SET_LOCAL_SHIFT_TIME_REMAINING(time)
#define SET_LOCAL_DOWNSHIFT_SPEED(speed)
#define SET_LOCAL_UPSHIFT_SPEED(speed)
#define SET_LOCAL_CURRENT_PITCH(pitch)
#define SET_LOCAL_IS_DAMAGED(isDamaged)
#define SET_LOCAL_VEHICLE_LIFE(life)
#define SET_LOCAL_DAMAGE_THRESHOLD(threshold)

#endif

enum GearShiftResult
{
    GEARSHIFT_NONE,
    GEARSHIFT_DAMPED,
    GEARSHIFT_UP,
    GEARSHIFT_DOWN
};


//
// Classes for engine states.  Each one represents a state 
// in which the engine will have different sound-speed characteristics
// (e.g. car in air, gearshift in progress).
//

//
// Base class for engine states
//
class EngineState
{
    public:
        EngineState();
        virtual ~EngineState();

        void Initialize( Vehicle* vehicle, carSoundParameters* parameters, 
                         SimpsonsSoundPlayer* player, radKey32 resourceKey,
                         VehicleSoundDebugPage* debugPtr );
        bool StartNewState() { return( m_nextState != ENGINE_STATE_INVALID ); }
        EngineStateEnum GetNewState() { return( m_nextState ); }
        void Reset() { m_nextState = ENGINE_STATE_INVALID; }

        virtual void SetDebugInfo();

        virtual void Service( unsigned int elapsedTime ) = 0;

        virtual float GetCurrentPitch();

        bool IsActive() { return( m_isActive ); }
        void SetActive( bool isActive, float prevPitch = 0.0f );

        void StartFade( float initialTrim, bool stopAfterFade = true );
        bool IsFading();
        void ServiceFade( unsigned int elapsedTime );

        void SetTrim( float trim );

    protected:

        virtual void startup( float prevPitch ) = 0;
        bool isAtIdleSpeed();
        bool isSkidding();

        //
        // True if this state is running and producing sound
        //
        bool m_isActive;

        //
        // When we want to kick off a new state, set this to something
        // other than ENGINE_STATE_INVALID
        //
        EngineStateEnum m_nextState;

        //
        // Stuff for getting vehicle and sound parameter info
        //
        Vehicle* m_vehicle;
        carSoundParameters* m_parameters;

        //
        // Sound player stuff
        //
        SimpsonsSoundPlayer* m_player;
        radKey32 m_resourceKey;

        //
        // Rev limiter, calculated from pitch ranges
        //
        float m_revLimit;

        //
        // Debug stuff
        //
#ifdef SOUND_DEBUG_INFO_ENABLED
        VehicleSoundDebugPage* m_debugInfo;
#endif

    private:
        //Prevent wasteful constructor creation.
        EngineState( const EngineState& original );
        EngineState& operator=( const EngineState& rhs );

        float m_fadeTrim;
        bool m_stopAfterFade;
};

//
// State for normal, on-ground, no-shift engine sound
//
class NormalEngineState : public EngineState
{
    public:
        NormalEngineState();
        virtual ~NormalEngineState();

        void Service( unsigned int elapsedTime );

        void SetDebugInfo();

        float GetCurrentPitch();

    protected:
        void startup( float prevPitch );
    private:
        //Prevent wasteful constructor creation.
        NormalEngineState( const NormalEngineState& original );
        NormalEngineState& operator=( const NormalEngineState& rhs );

        GearShiftResult updateCurrentGearFromVehicle( bool dampenShifts = false );

        int m_currentGear;
        float m_currentSpeed;
        float m_currentPitch;
        bool m_isAttacking;
        float m_currentTrim;
        unsigned int m_interpolateTime;
};

//
// State for upshifts (e.g. 2nd to 3rd gear)
//
class UpshiftEngineState : public EngineState
{
    public:
        UpshiftEngineState();
        virtual ~UpshiftEngineState();

        void Service( unsigned int elapsedTime );

        void SetDebugInfo();

    protected:
        void startup( float prevPitch );
    private:
        //Prevent wasteful constructor creation.
        UpshiftEngineState( const UpshiftEngineState& original );
        UpshiftEngineState& operator=( const UpshiftEngineState& rhs );

        float m_startPitch;
        float m_pitchDrop;
        float m_shiftTimeMsecs;
        unsigned int m_remainingTime;
};

//
// State for downshifts (e.g. 3rd to 2nd gear)
//
class DownshiftEngineState : public EngineState
{
    public:
        DownshiftEngineState();
        virtual ~DownshiftEngineState();

        void Service( unsigned int elapsedTime );

        void SetDebugInfo();

        float GetCurrentPitch();

    protected:
        void startup( float prevPitch );
    private:
        //Prevent wasteful constructor creation.
        DownshiftEngineState( const DownshiftEngineState& original );
        DownshiftEngineState& operator=( const DownshiftEngineState& rhs );

        float m_startSpeed;
        float m_startPitch;
        float m_lastSpeed;
        float m_currentPitch;
};

//
// State for car in mid-air
//
class InAirEngineState : public EngineState
{
    public:
        InAirEngineState();
        virtual ~InAirEngineState();

        void Service( unsigned int elapsedTime );

        void SetDebugInfo();

    protected:
        void startup( float prevPitch );
    private:
        //Prevent wasteful constructor creation.
        InAirEngineState( const InAirEngineState& original );
        InAirEngineState& operator=( const InAirEngineState& rhs );

        float m_currentPitch;
};

//
// State for car in reverse
//
class ReverseEngineState : public EngineState
{
    public:
        ReverseEngineState( SimpsonsSoundPlayer* beepPlayer );
        virtual ~ReverseEngineState();

        void Service( unsigned int elapsedTime );

        void SetDebugInfo();

    protected:
        void startup( float prevPitch );
    private:
        //Prevent wasteful constructor creation.
        ReverseEngineState( const ReverseEngineState& original );
        ReverseEngineState& operator=( const ReverseEngineState& rhs );

        SimpsonsSoundPlayer* m_beepPlayer;
};

//
// State for car in idle
//
class IdleEngineState : public EngineState
{
    public:
        IdleEngineState();
        virtual ~IdleEngineState();

        void Service( unsigned int elapsedTime );

        void SetDebugInfo();

        float GetCurrentPitch();

    protected:
        void startup( float prevPitch );
    private:
        //Prevent wasteful constructor creation.
        IdleEngineState( const IdleEngineState& original );
        IdleEngineState& operator=( const IdleEngineState& rhs );

        float m_currentPitch;
};

//
// State for car when skidding
//
class SkidEngineState : public EngineState
{
    public:
        SkidEngineState();
        virtual ~SkidEngineState();

        void Service( unsigned int elapsedTime );

        void SetDebugInfo();

        float GetCurrentPitch();

    protected:
        void startup( float prevPitch );

    private:
        //Prevent wasteful constructor creation.
        SkidEngineState( const SkidEngineState& original );
        SkidEngineState& operator=( const SkidEngineState& rhs );

        float m_currentPitch;
};

//******************************************************************************
//
// EngineState
//
//******************************************************************************

EngineState::EngineState() :
    m_isActive( false ),
    m_nextState( ENGINE_STATE_INVALID ),
    m_vehicle( NULL ),
    m_parameters( NULL ),
    m_player( NULL ),
    m_resourceKey( 0 ),
    m_fadeTrim( 0.0f ),
    m_stopAfterFade( true )
{
}

EngineState::~EngineState()
{
}

void EngineState::Initialize( Vehicle* vehicle, carSoundParameters* parameters, 
                              SimpsonsSoundPlayer* player, radKey32 resourceKey,
                              VehicleSoundDebugPage* debugPtr )
{
    rAssert( vehicle != NULL );
    rAssert( parameters != NULL );
    rAssert( player != NULL );

    m_vehicle = vehicle;
    m_parameters = parameters;
    m_player = player;
    m_resourceKey = resourceKey;

    //
    // Figure out a sensible rev limit
    //
    m_revLimit = parameters->GetRevLimit();

#ifdef SOUND_DEBUG_INFO_ENABLED
    m_debugInfo = debugPtr;
#endif
}

void EngineState::SetActive( bool isActive, float prevPitch )
{
    bool wasActive = m_isActive;

    m_isActive = isActive;
    if( m_isActive && !wasActive )
    {
        startup( prevPitch );
    }
}

void EngineState::SetDebugInfo()
{
    // Do nothing by default
}

bool EngineState::isAtIdleSpeed()
{
    return( m_vehicle->GetSpeedKmh() 
            <= ( m_parameters->GetShiftPoint( 1 ) 
                 * m_vehicle->mDesignerParams.mDpTopSpeedKmh ) );
}

bool EngineState::isSkidding()
{
    return( m_vehicle->GetSkidLevel() > 0.0f );
}

float EngineState::GetCurrentPitch()
{
    //
    // Not needed by all states, so return zero by default.
    //
    return( 0.0f );
}

bool EngineState::IsFading()
{
    return( m_fadeTrim > 0.0f );
}

void EngineState::StartFade( float initialTrim, bool stopAfterFade )
{
    m_fadeTrim = initialTrim;
    m_stopAfterFade = stopAfterFade;

    //
    // Check whether we're already faded
    //
    if( ( m_fadeTrim <= 0.0f )
        && stopAfterFade )
    {
        m_player->Stop();
    }
    else
    {
        //
        // Set the trim, in case the parent hasn't done it already
        //
        m_player->SetTrim( initialTrim );
    }
}

void EngineState::ServiceFade( unsigned int elapsedTime )
{
    //
    // Fade from 1.0f to 0.0f in 1/10th of a second, time picked arbitrarily
    //
    m_fadeTrim -= static_cast<float>( elapsedTime ) / 100.0f;
    if( m_fadeTrim < 0.0f )
    {
        m_fadeTrim = 0.0f;
    }

    m_player->SetTrim( m_fadeTrim );

    if( ( m_fadeTrim == 0.0f )
        && m_stopAfterFade )
    {
        m_player->Stop();
    }
}

void EngineState::SetTrim( float trim )
{
    //
    // Cancel fades in effect
    //
    m_fadeTrim = 0.0f;

    m_player->SetTrim( trim );
}

//******************************************************************************
//
// NormalEngineState
//
//******************************************************************************

NormalEngineState::NormalEngineState() :
    m_currentGear( 0 ),
    m_currentSpeed( 0.0f ),
    m_isAttacking( false ),
    m_currentTrim( 1.0f ),
    m_interpolateTime( 0 )
{
}

NormalEngineState::~NormalEngineState()
{
}

void NormalEngineState::Service( unsigned int elapsedTime )
{
    GearShiftResult startedShift;
    float newPitch;
    float trimDiff;
    float pitchDiff;
    float maxDiff;
    float speedPcnt;
    float shiftPoint;
    float shiftSpeed;
    float topSpeed;

    if( ( m_interpolateTime > 0 ) && !m_isAttacking )
    {
        //
        // Delaying
        //
        if( elapsedTime > m_interpolateTime )
        {
            //
            // Delay done, start attack
            //
            m_interpolateTime = static_cast<unsigned int>(m_parameters->GetAttackTimeMsecs());
            m_isAttacking = true;
        }
        else
        {
            //
            // Delay not done
            //
            m_interpolateTime -= elapsedTime;

            if( m_interpolateTime <= m_parameters->GetDelayTimeMsecs() )
            {
                m_currentTrim = m_parameters->GetDecayFinishTrim();
                m_player->SetPitch( 1.0f );
            }

            //
            // Since we're delaying, don't do anything else
            //
            return;
        }
    }
    else if( m_interpolateTime > 0 )
    {
        //
        // Attacking
        //
        if( elapsedTime > m_interpolateTime )
        {
            m_currentTrim = 1.0f;
            m_interpolateTime = 0;
            m_isAttacking = false;
        }
        else
        {
            m_interpolateTime -= elapsedTime;
            trimDiff = static_cast<float>(elapsedTime) / m_parameters->GetAttackTimeMsecs();
            m_currentTrim += trimDiff * ( 1.0f - m_parameters->GetDecayFinishTrim() );  // Increase at half-speed because we start at 0.5 above
            if( m_currentTrim > 1.0f )
            {
                //
                // Interpolation error
                //
                m_currentTrim = 1.0f;
            }
        }
    }
    else
    {
        //
        // Regular engine playback, make sure the attack is off and the trim is maxed.
        //
        m_isAttacking = false;

        if( m_currentTrim != 1.0f )
        {
            m_currentTrim = 1.0f;
        }
    }
    
    m_currentSpeed = m_vehicle->GetSpeedKmh();

    if( m_vehicle->IsMovingBackward() )
    {
        m_nextState = ENGINE_STATE_REVERSE;
    }
    else if( m_vehicle->IsAirborn() )
    {
        m_nextState = ENGINE_STATE_IN_AIR;
    }
    else if( isSkidding() )
    {
        m_nextState = ENGINE_STATE_SKIDDING;
    }
    else
    {
        startedShift = updateCurrentGearFromVehicle();
        if( ( startedShift == GEARSHIFT_UP )
            || ( startedShift == GEARSHIFT_DOWN ) )
        {
            if( startedShift == GEARSHIFT_UP )
            {
                m_nextState = ENGINE_STATE_UPSHIFTING;
                //
                // Don't stop the player, start delay
                //
                m_interpolateTime = m_parameters->GetDelayTimeMsecs() 
                                    + static_cast<unsigned int>(m_parameters->GetDecayTimeMsecs());
                if( m_interpolateTime > 0 )
                {
                    m_currentTrim = 0.0f;
                }
            }
            else if( m_currentGear > 0 )
            {
                m_nextState = ENGINE_STATE_DOWNSHIFTING;
            }
            else
            {
                m_nextState = ENGINE_STATE_IDLING;
            }
        }
        else
        {
            if( startedShift == GEARSHIFT_DAMPED )
            {
                //
                // If we're going slower than the speed appropriate for the gear,
                // don't let the pitch drop too low
                //
                shiftPoint = m_parameters->GetShiftPoint( m_currentGear );
                topSpeed = m_vehicle->mDesignerParams.mDpTopSpeedKmh;
                shiftSpeed = shiftPoint * topSpeed;
                speedPcnt = m_currentSpeed / shiftSpeed;
                newPitch = m_parameters->CalculateEnginePitch( m_currentGear, shiftSpeed, topSpeed );
                newPitch *= speedPcnt;
            }
            else
            {
                //
                // No shift, just set the pitch to value appropriate to the vehicle speed
                //
                newPitch = m_parameters->CalculateEnginePitch( m_currentGear, m_currentSpeed,
                    m_vehicle->mDesignerParams.mDpTopSpeedKmh );
            }

            pitchDiff = newPitch - m_currentPitch;
            maxDiff = static_cast<float>(elapsedTime) / m_parameters->GetMsecsPerOctaveCap();

            //
            // Ignore the calculated pitch if it means we're going to jump around
            // too suddenly
            //
            if( pitchDiff >= 0.0f )
            {
                // Pitch rising
                m_currentPitch += ( pitchDiff > maxDiff ) ? maxDiff : pitchDiff;
            }
            else
            {
                // Pitch dropping
                m_currentPitch += ( pitchDiff < -maxDiff ) ? -maxDiff : pitchDiff;
            }

            if( m_currentPitch > m_revLimit )
            {
                m_currentPitch = m_revLimit;
            }

            m_player->SetPitch( m_currentPitch );
            SET_CURRENT_PITCH( m_currentPitch );

            if( !m_isAttacking )
            {
                m_currentTrim = 1.0f;
            }
        }
    }

    if( m_nextState != ENGINE_STATE_INVALID )
    {
        if( ( m_nextState == ENGINE_STATE_DOWNSHIFTING )
            || ( m_nextState == ENGINE_STATE_UPSHIFTING ) )
        {
            StartFade( m_currentTrim, false );
        }
        else
        {
            SetTrim( m_currentTrim );
        }

        if( m_nextState != ENGINE_STATE_UPSHIFTING )
        {
            SetActive( false );
        }
    }
    else
    {
        SetTrim( m_currentTrim );
    }

    rAssertMsg( m_currentPitch >= 0.0f, "If you're running a debugger, tell Esan about this" );
}

void NormalEngineState::SetDebugInfo()
{
    if( IsActive() )
    {
        SET_CURRENT_GEAR( m_currentGear );
        SET_CURRENT_SPEED( m_currentSpeed );
    }
}

float NormalEngineState::GetCurrentPitch()
{
    return( m_currentPitch );
}

void NormalEngineState::startup( float prevPitch )
{
    m_currentSpeed = m_vehicle->GetSpeedKmh();
    m_currentGear = m_parameters->CalculateCurrentGear( m_vehicle->GetSpeedKmh(), m_currentSpeed,
                                                        m_vehicle->mDesignerParams.mDpTopSpeedKmh,
                                                        -1 );

    //
    // Start the sound
    //
    if( !(m_player->IsInUse()) )
    {
        m_player->PlaySound( m_resourceKey );
    }

    m_currentTrim = 1.0f;
    m_interpolateTime = 0;
    if( prevPitch > 0.0f )
    {
        m_currentPitch = prevPitch;
    }
    else
    {
        m_currentPitch = m_parameters->CalculateEnginePitch( m_currentGear, m_currentSpeed,
                                                             m_vehicle->mDesignerParams.mDpTopSpeedKmh );
    }
    SetTrim( m_currentTrim );

    if( m_currentPitch > m_revLimit )
    {
        m_currentPitch = m_revLimit;
    }
    m_player->SetPitch( m_currentPitch );
}

//=============================================================================
// NormalEngineState::updateCurrentGearFromVehicle
//=============================================================================
// Description: Update m_currentGear from the current state of m_vehicle
//
// Parameters:  dampenShifts - indicates whether we want to eliminate unnecessary
//                             gear shifts.  Not needed when we get in the car.
//
// Return:      enumeration indicating if we shifted and if so, which direction
//
//=============================================================================
GearShiftResult NormalEngineState::updateCurrentGearFromVehicle( bool dampenShifts /* = false */ )
{
    int currentGear;
    GearShiftResult shiftResult;
    float damperSize;
    float upperShiftSpeed;
    int oldGear = m_currentGear;
    float topSpeed = m_vehicle->mDesignerParams.mDpTopSpeedKmh;

    if( dampenShifts )
    {
        currentGear = m_currentGear;
    }
    else
    {
        currentGear = -1;
    }

    m_currentGear = m_parameters->CalculateCurrentGear( m_vehicle->GetSpeedKmh(), m_currentSpeed,
                                                        topSpeed, -1 );

    if( m_currentGear == oldGear )
    {
        shiftResult = GEARSHIFT_NONE;
    }
    else
    {
        if( m_currentGear > oldGear )
        {
            if( m_vehicle->IsSafeToUpShift() )
            {
                shiftResult = GEARSHIFT_UP;
            }
            else
            {
                //
                // Don't upshift when we're climbing hills, because the engine
                // tends to stay bogged down at low revs
                //
                m_currentGear = oldGear;
                shiftResult = GEARSHIFT_NONE;
            }
        }
        else if( m_currentGear == 0 )
        {
            //
            // Don't damp going into idle
            //
            shiftResult = GEARSHIFT_DOWN;
        }
        else
        {
            //
            // Apply some gearshift damping
            //
            damperSize = m_parameters->GetDownshiftDamperSize() * topSpeed;
            upperShiftSpeed = m_parameters->GetShiftPoint( m_currentGear + 1 ) * topSpeed;
            if( ( upperShiftSpeed - m_currentSpeed ) <= damperSize )
            {
                shiftResult = GEARSHIFT_DAMPED;

                //
                // Wah.  I think there's a problem where having the top gear too narrow can
                // cause us to bump up into a non-existent gear.  Don't do that.
                //
                if( upperShiftSpeed < topSpeed )
                {
                    ++m_currentGear;
                }
                //else
                //{
                //    rDebugString("Gotcha!");
                //}
            }
            else
            {
                shiftResult = GEARSHIFT_DOWN;
            }
        }
    }

    return( shiftResult );
}

//******************************************************************************
//
// UpshiftEngineState
//
//******************************************************************************

UpshiftEngineState::UpshiftEngineState()
{
}

UpshiftEngineState::~UpshiftEngineState()
{
}

void UpshiftEngineState::Service( unsigned int elapsedTime )
{
    float newPitch;
    float timePercent;
    float newTrim;
    float targetTrim;

    if( elapsedTime > m_remainingTime )
    {
        //
        // Upshift is done, return to normal engine sound
        //

        //
        // No need for state change, normal doesn't shut off on
        // upshifts anymore
        //
        //m_nextState = ENGINE_STATE_NORMAL;

        SetTrim( 1.0f );
        m_player->Stop();
        SetActive( false );
    }
    else
    {
        //
        // Calculate the amount we need to drop the pitch in this frame
        //
        m_remainingTime -= elapsedTime;
        timePercent = static_cast<float>(m_remainingTime) / static_cast<float>(m_shiftTimeMsecs);
        newPitch = m_startPitch - ( ( 1.0f - timePercent ) * m_pitchDrop );
        targetTrim = m_parameters->GetDecayFinishTrim();
        newTrim = targetTrim + ( ( 1.0f - targetTrim ) * timePercent );

        m_player->SetPitch( newPitch );
        SET_CURRENT_PITCH( newPitch );
        SetTrim( newTrim );
    }
}

void UpshiftEngineState::SetDebugInfo()
{
    if( IsActive() )
    {
        SET_SHIFT_IN_PROGRESS( true );
        SET_SHIFT_TIME_REMAINING( m_remainingTime );
        SET_CURRENT_SPEED( m_vehicle->GetSpeedKmh() );
    }
}

void UpshiftEngineState::startup( float prevPitch )
{
    int currentGear;
    float currentSpeed;

    //
    // Initialize the current pitch, the amount we're going to drop it by,
    // and the length of time we'll take to drop it
    //
    currentSpeed = m_vehicle->GetSpeedKmh();
    currentGear = m_parameters->CalculateCurrentGear( currentSpeed, currentSpeed,
                                                      m_vehicle->mDesignerParams.mDpTopSpeedKmh,
                                                      -1 );

    //
    // Don't forget to subtract one from the gear because the parameter calculation
    // will already have signalled the upshift, and we still want the old gear's pitch
    //
    m_startPitch = m_parameters->CalculateEnginePitch( currentGear - 1, currentSpeed,
                                                       m_vehicle->mDesignerParams.mDpTopSpeedKmh );
    m_pitchDrop = m_parameters->GetGearShiftPitchDrop( currentGear );
    m_shiftTimeMsecs = m_parameters->GetDecayTimeMsecs();
    m_remainingTime = static_cast<unsigned int>(m_shiftTimeMsecs);

    //
    // Start the player
    //
    if( !(m_player->IsInUse()) )
    {
        m_player->PlaySound( m_resourceKey );
    }
    m_player->SetPitch( m_startPitch );
    SetTrim( 1.0f );
}

//******************************************************************************
//
// DownshiftEngineState
//
//******************************************************************************

DownshiftEngineState::DownshiftEngineState()
{
}

DownshiftEngineState::~DownshiftEngineState()
{
}

void DownshiftEngineState::Service( unsigned int elapsedTime )
{
    float speedPercent;
    float idlePitch;
    float currentSpeed = m_vehicle->GetSpeedKmh();

    if( isAtIdleSpeed() )
    {
        //
        // Switch to idle sound
        //
        m_nextState = ENGINE_STATE_IDLING;

        m_player->Stop();
        SetActive( false );
    }
    else if( currentSpeed > m_lastSpeed )
    {
        //
        // We've started accelerating again, switch to normal engine sound
        //
        m_nextState = ENGINE_STATE_NORMAL;

        StartFade( 1.0f );
        SetActive( false );
    }
    else
    {
        //
        // Calculate new engine pitch
        //
        speedPercent = m_vehicle->GetSpeedKmh() / m_startSpeed;
        // For now, use idle engine pitch for bottom of pitch range
        idlePitch = m_parameters->GetIdleEnginePitch();
        m_currentPitch = idlePitch + ( speedPercent * ( m_startPitch - idlePitch ) );

        m_player->SetPitch( m_currentPitch );
        SET_CURRENT_PITCH( m_currentPitch );

        m_lastSpeed = currentSpeed;
    }
}

void DownshiftEngineState::SetDebugInfo()
{
    if( IsActive() )
    {
        SET_SHIFT_IN_PROGRESS( true );
        SET_SHIFT_TIME_REMAINING( 0 );
        SET_CURRENT_SPEED( m_vehicle->GetSpeedKmh() );
    }
}

float DownshiftEngineState::GetCurrentPitch()
{
    return( m_currentPitch );
}

void DownshiftEngineState::startup( float prevPitch )
{
    //int currentGear;

    m_startSpeed = m_vehicle->GetSpeedKmh();
    m_lastSpeed = m_startSpeed;

    //
    // Find the speed that we need to interpolate over
    //

    // "+ 1" is because we're already in the new gear, and we want to interpolate
    // from the last one
    //currentGear = m_parameters->CalculateCurrentGear( m_startSpeed, m_startSpeed,
    //                                                  m_vehicle->mDesignerParams.mDpTopSpeedKmh,
    //                                                  -1 ) + 1;

    //m_startPitch = m_parameters->CalculateEnginePitch( currentGear, m_startSpeed,
    //                                                   m_vehicle->mDesignerParams.mDpTopSpeedKmh );
    m_startPitch = prevPitch;
    m_currentPitch = m_startPitch;

    //
    // Start the sound
    //
    if( !(m_player->IsInUse()) )
    {
        m_player->PlaySound( m_resourceKey );
    }
    m_player->SetPitch( m_startPitch );
}

//******************************************************************************
//
// InAirEngineState
//
//******************************************************************************

InAirEngineState::InAirEngineState()
{
}

InAirEngineState::~InAirEngineState()
{
}

void InAirEngineState::Service( unsigned int elapsedTime )
{
    float maxPitchChange;
    float idlePitch;
    float throttlePitch;
    unsigned int responseTime;
    float targetPitch;
    float newPitch;

    if( !(m_vehicle->IsAirborn()) )
    {
        if( isAtIdleSpeed() )
        {
            m_nextState = ENGINE_STATE_IDLING;
        }
        else if( isSkidding() )
        {
            m_nextState = ENGINE_STATE_SKIDDING;
        }
        else
        {
            m_nextState = ENGINE_STATE_NORMAL;
        }

        m_player->Stop();
        SetActive( false );
    }
    else
    {
        //
        // Calculate the maximum pitch change given the car's responsiveness
        idlePitch = m_parameters->GetInAirIdlePitch();
        throttlePitch = m_parameters->GetInAirThrottlePitch();
        responseTime = m_parameters->GetInAirThrottleResponseTimeMsecs();
        maxPitchChange = ( throttlePitch - idlePitch ) 
                         * ( static_cast<float>(elapsedTime) / static_cast<float>(responseTime) );

        //
        // Get target pitch
        //
        targetPitch = idlePitch + ( m_vehicle->mGas * ( throttlePitch - idlePitch ) );
        if( targetPitch > m_currentPitch )
        {
            if( m_currentPitch + maxPitchChange > targetPitch )
            {
                newPitch = targetPitch;
            }
            else
            {
                newPitch = m_currentPitch + maxPitchChange;
            }
        }
        else
        {
            if( m_currentPitch - maxPitchChange < targetPitch )
            {
                newPitch = targetPitch;
            }
            else
            {
                newPitch = m_currentPitch - maxPitchChange;
            }
        }

        m_player->SetPitch( newPitch );
        m_currentPitch = newPitch;
    }
}

void InAirEngineState::SetDebugInfo()
{
    if( IsActive() )
    {
        SET_CURRENT_PITCH( m_currentPitch );
        SET_CURRENT_SPEED( m_vehicle->GetSpeedKmh() );
    }
}

void InAirEngineState::startup( float prevPitch )
{
    if( !(m_player->IsInUse()) )
    {
        m_player->PlaySound( m_resourceKey );
    }

    m_currentPitch = prevPitch;

    m_player->SetPitch( m_currentPitch );
}

//******************************************************************************
//
// ReverseEngineState
//
//******************************************************************************

ReverseEngineState::ReverseEngineState( SimpsonsSoundPlayer* beepPlayer ) :
    m_beepPlayer( beepPlayer )
{
}

ReverseEngineState::~ReverseEngineState()
{
}

void ReverseEngineState::Service( unsigned int elapsedTime )
{
    float speed;
    float maxReverseSpeed;
    float minPitch;
    float maxPitch;
    float newPitch;

    speed = m_vehicle->GetSpeedKmh();

    if( !(m_vehicle->IsMovingBackward()) )
    {
        if( m_vehicle->IsAirborn() )
        {
            m_nextState = ENGINE_STATE_IN_AIR;
        }
        else if( isAtIdleSpeed() )
        {
            m_nextState = ENGINE_STATE_IDLING;
        }
        else if( isSkidding() )
        {
            m_nextState = ENGINE_STATE_SKIDDING;
        }
        else
        {
            m_nextState = ENGINE_STATE_NORMAL;
        }

        m_beepPlayer->Stop();
        SetActive( false );
    }
    else
    {
        //
        // Set pitch based on speed
        //
        maxReverseSpeed = m_parameters->GetReversePitchCapKmh();
        m_parameters->GetReversePitchRange( minPitch, maxPitch );
        if( speed > maxReverseSpeed )
        {
            newPitch = maxPitch;
        }
        else
        {
            newPitch = minPitch + ( ( speed / maxReverseSpeed ) * ( maxPitch - minPitch ) );
        }

        m_player->SetPitch( newPitch );
        SET_CURRENT_PITCH( newPitch );
    }
}

void ReverseEngineState::SetDebugInfo()
{
    if( IsActive() )
    {
        SET_CURRENT_SPEED( m_vehicle->GetSpeedKmh() );
        SET_CURRENT_GEAR( -1 );
    }
}

void ReverseEngineState::startup( float prevPitch )
{
    const char* backupClipName;

    if( !(m_player->IsInUse()) )
    {
        m_player->PlaySound( m_resourceKey );
    }

    //
    // Call Service() to set the pitch
    //
    Service( 0 );

    //
    // If we have a backup beep sound, play it
    //
    if( ( backupClipName = m_parameters->GetBackupClipName() ) != NULL )
    {
        m_beepPlayer->PlaySound( backupClipName );
    }
}

//******************************************************************************
//
// IdleEngineState
//
//******************************************************************************

IdleEngineState::IdleEngineState() :
    m_currentPitch( 0.0f )
{
}

IdleEngineState::~IdleEngineState()
{
}

void IdleEngineState::Service( unsigned int elapsedTime )
{
    float idlePitch;
    float pitchDiff;
    float maxDiff;

    //
    // Check for car motion, indicating we're leaving the idle state
    //
    if( m_vehicle->IsMovingBackward() )
    {
        m_nextState = ENGINE_STATE_REVERSE;
    }
    else if( isSkidding() )
    {
        m_nextState = ENGINE_STATE_SKIDDING;
    }
    else if( !isAtIdleSpeed() )
    {
        m_nextState = ENGINE_STATE_NORMAL;
    }

    if( m_nextState != ENGINE_STATE_INVALID )
    {
        //
        // Deactivate self
        //
        m_player->Stop();
        SetActive( false );
    }
    else
    {
        //
        // Keep playing idle.  Interpolate to it if we're not playing
        // the idle pitch already
        //
        idlePitch = m_parameters->GetIdleEnginePitch();
        if( m_currentPitch != idlePitch )
        {
            pitchDiff = idlePitch - m_currentPitch;
            maxDiff = static_cast<float>(elapsedTime) / m_parameters->GetMsecsPerOctaveCap();

            //
            // Ignore the calculated pitch if it means we're going to jump around
            // too suddenly
            //
            if( pitchDiff >= 0.0f )
            {
                // Pitch rising
                m_currentPitch += ( pitchDiff > maxDiff ) ? maxDiff : pitchDiff;
            }
            else
            {
                // Pitch dropping
                m_currentPitch += ( pitchDiff < -maxDiff ) ? -maxDiff : pitchDiff;
            }

            m_player->SetPitch( m_currentPitch );
            SET_CURRENT_PITCH( m_currentPitch );
        }
    }
}

void IdleEngineState::SetDebugInfo()
{
    if( IsActive() )
    {
        SET_CURRENT_SPEED( m_vehicle->GetSpeedKmh() );
        SET_CURRENT_GEAR( 0 );
    }
}

float IdleEngineState::GetCurrentPitch()
{
    return( m_currentPitch );
}

void IdleEngineState::startup( float prevPitch )
{
    rAssert( m_parameters != NULL );

    if( !(m_player->IsInUse()) )
    {
        m_player->PlaySound( m_resourceKey );
    }

    m_currentPitch = m_parameters->GetIdleEnginePitch();
    if( prevPitch != m_currentPitch )
    {
        m_currentPitch = prevPitch;
    }

    m_player->SetPitch( m_currentPitch );
    SET_CURRENT_PITCH( m_currentPitch );
}

//******************************************************************************
//
// SkidEngineState
//
//******************************************************************************

SkidEngineState::SkidEngineState() :
    m_currentPitch( 0.0f )
{
}

SkidEngineState::~SkidEngineState()
{
}

void SkidEngineState::Service( unsigned int elapsedTime )
{
    float burnoutLevel;
    float desiredPitch;
    float minBurnoutPitch;
    float minPowerslidePitch;
    float pitchDiff;
    float maxDiff;

    //
    // Get a desired pitch based on the burnout level that the
    // vehicle object gives us
    //
    if( isSkidding() )
    {
        //
        // Sometimes we're skidding, sometimes we're burning out.  Try to figure out
        // which value makes more sense.
        //
        if( m_vehicle->mBurnoutLevel > 0.0f )
        {
            burnoutLevel = m_vehicle->mBurnoutLevel;

            minBurnoutPitch = m_parameters->GetBurnoutMinPitch();
            desiredPitch = minBurnoutPitch +
                ( ( m_parameters->GetBurnoutMaxPitch() - minBurnoutPitch ) 
                * burnoutLevel );
        }
        else
        {
            //burnoutLevel = m_vehicle->GetSkidLevel();

            //
            // The skid level appears to be a constant value of 0.5.  Try using the
            // vehicle gas value instead, since the tires would be spinning kinda 
            // freely in the real world anyway.
            //
            // HACK: Since sliding skids seem to be quite different from burnouts,
            // bump the burnout level beyond 1.0f at will to make this sound right.
            // This should be a separate set of tuning parameters next time.
            //
            burnoutLevel = m_vehicle->mGas;

            minPowerslidePitch = m_parameters->GetPowerslideMinPitch();
            desiredPitch = minPowerslidePitch +
                ( ( m_parameters->GetPowerslideMaxPitch() - minPowerslidePitch ) 
                * burnoutLevel );
        }

        //
        // Ease toward desired pitch
        //
        pitchDiff = desiredPitch - m_currentPitch;
        maxDiff = static_cast<float>(elapsedTime) / ( m_parameters->GetMsecsPerOctaveCap() * 4 );

        //
        // Ignore the calculated pitch if it means we're going to jump around
        // too suddenly
        //
        if( pitchDiff >= 0.0f )
        {
            // Pitch rising
            m_currentPitch += ( pitchDiff > maxDiff ) ? maxDiff : pitchDiff;
        }
        else
        {
            // Pitch dropping
            m_currentPitch += ( pitchDiff < -maxDiff ) ? -maxDiff : pitchDiff;
        }

        m_player->SetPitch( m_currentPitch );
        SET_CURRENT_PITCH( m_currentPitch );
    }
    else
    {
        if( isAtIdleSpeed() )
        {
            m_nextState = ENGINE_STATE_IDLING;
        }
        else if( m_vehicle->IsMovingBackward() )
        {
            m_nextState = ENGINE_STATE_REVERSE;
        }
        else
        {
            m_nextState = ENGINE_STATE_NORMAL;
        }

        SetActive( false );
    }
}

void SkidEngineState::SetDebugInfo()
{
    if( IsActive() )
    {
        SET_CURRENT_SPEED( m_vehicle->GetSpeedKmh() );
    }
}

float SkidEngineState::GetCurrentPitch()
{
    return( m_currentPitch );
}

void SkidEngineState::startup( float prevPitch )
{
    rAssert( m_parameters != NULL );

    if( !(m_player->IsInUse()) )
    {
        m_player->PlaySound( m_resourceKey );
    }

    m_currentPitch = prevPitch;

    m_player->SetPitch( m_currentPitch );
    SET_CURRENT_PITCH( m_currentPitch );
}

//******************************************************************************
//
// VehicleSoundPlayer
//
//******************************************************************************

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// VehicleSoundPlayer::VehicleSoundPlayer
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
VehicleSoundPlayer::VehicleSoundPlayer() :
#ifdef SOUND_DEBUG_INFO_ENABLED
    m_debugInfo( 2, GetSoundManager()->GetDebugDisplay() ),
#endif
    m_vehicle( NULL ),
    m_parameters( NULL ),
    m_peeloutSettings( NULL ),
    m_isSkidding( false ),
    m_hornPlaying( false ),
    m_oneTimeHorn( false ),
    m_powerslideTrim( 0.0f ),
    m_playingDamage( false ),
    m_proximityAIVehicle( NULL ),
    m_terrainType( TT_Road )
{
    m_engineStates[ENGINE_STATE_NORMAL] = new NormalEngineState();
    m_engineStates[ENGINE_STATE_UPSHIFTING] = new UpshiftEngineState();
    m_engineStates[ENGINE_STATE_DOWNSHIFTING] = new DownshiftEngineState();
    m_engineStates[ENGINE_STATE_IN_AIR] = new InAirEngineState();
    m_engineStates[ENGINE_STATE_REVERSE] = new ReverseEngineState( &(m_soundPlayers[CARPLAYER_BACKUP_BEEP]) );
    m_engineStates[ENGINE_STATE_IDLING] = new IdleEngineState();
    m_engineStates[ENGINE_STATE_SKIDDING] = new SkidEngineState();

    if( !s_resetEngineInitialized )
    {
        radDbgWatchAddBoolean( &s_resetEngineOnHonk, "Reset engine on honk", "Sound Info" );
    }
}

//==============================================================================
// VehicleSoundPlayer::~VehicleSoundPlayer
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
VehicleSoundPlayer::~VehicleSoundPlayer()
{
    int i;

    for( i = 0; i < NUM_ENGINE_STATES; i++ )
    {
        delete m_engineStates[i];
    }
}

//=============================================================================
// VehicleSoundPlayer::StartCarSounds
//=============================================================================
// Description: Begin playing car-related sounds
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void VehicleSoundPlayer::StartCarSounds( Vehicle* newVehicle )
{
    IRadNameSpace* nameSpace;
    IRefCount* nameSpaceObj;
    VehicleSoundDebugPage* debugPtr;
    const char* overlayName;

    rAssert( newVehicle != NULL );

    GetEventManager()->TriggerEvent( EVENT_AVATAR_VEHICLE_TOGGLE, newVehicle );

    //
    // Don't bother switching sounds if we're already playing this car
    //
    if( ( newVehicle == m_vehicle ) && carSoundIsActive() )
    {
        return;
    }

    //
    // Remember which vehicle we're playing
    //
    m_vehicle = newVehicle;

    //
    // Get the car sound parameter object for this vehicle.
    //
    // IMPORTANT: We assume that the object in the namespace has the same
    // name as the one in the vehicle object, which comes from the loading
    // script for that car, I think.
    //
    nameSpace = Sound::daSoundRenderingManagerGet()->GetTuningNamespace();
    rAssert( nameSpace != NULL );

    nameSpaceObj = nameSpace->GetInstance( m_vehicle->GetName() );
    if( nameSpaceObj != NULL )
    {
        m_parameters = reinterpret_cast<carSoundParameters*>( nameSpaceObj );

        //
        // Set up the Watcher tuning
        //
        m_parameters->SetWatcherName( m_vehicle->GetName() );

        //
        // Get global car values
        //
        nameSpaceObj = nameSpace->GetInstance( "tuner" );
        rAssert( nameSpaceObj != NULL );
        m_peeloutSettings = reinterpret_cast<globalSettings*>( nameSpaceObj );

        const char* clipName = m_parameters->GetEngineClipName();

        const char* idleClipName = m_parameters->GetEngineIdleClipName();
        if( idleClipName == NULL )
        {
            //
            // No idle clip, use the engine clip by default
            //
            idleClipName = clipName;
        }

        if( ( clipName != NULL ) && ( idleClipName != NULL ) )
        {
#ifdef SOUND_DEBUG_INFO_ENABLED
            debugPtr = &m_debugInfo;
#else
            debugPtr = NULL;
#endif

            //
            // Initialize the engine sound states
            //
            m_engineStates[ENGINE_STATE_NORMAL]->Initialize( newVehicle, m_parameters,
                                                             &m_soundPlayers[CARPLAYER_ENGINE],
                                                             ::radMakeKey32( clipName ),
                                                             debugPtr );
            m_engineStates[ENGINE_STATE_UPSHIFTING]->Initialize( newVehicle, m_parameters,
                                                                 &m_soundPlayers[CARPLAYER_SHIFT],
                                                                 ::radMakeKey32( clipName ),
                                                                 debugPtr );
            m_engineStates[ENGINE_STATE_DOWNSHIFTING]->Initialize( newVehicle, m_parameters,
                                                                   &m_soundPlayers[CARPLAYER_SHIFT],
                                                                   ::radMakeKey32( clipName ),
                                                                   debugPtr );
            m_engineStates[ENGINE_STATE_IN_AIR]->Initialize( newVehicle, m_parameters,
                                                             &m_soundPlayers[CARPLAYER_ENGINE],
                                                             ::radMakeKey32( clipName ),
                                                             debugPtr );
            m_engineStates[ENGINE_STATE_REVERSE]->Initialize( newVehicle, m_parameters,
                                                              &m_soundPlayers[CARPLAYER_ENGINE],
                                                              ::radMakeKey32( clipName ),
                                                              debugPtr );
            m_engineStates[ENGINE_STATE_IDLING]->Initialize( newVehicle, m_parameters,
                                                             &m_soundPlayers[CARPLAYER_ENGINE],
                                                             ::radMakeKey32( idleClipName ),
                                                             debugPtr );
            m_engineStates[ENGINE_STATE_SKIDDING]->Initialize( newVehicle, m_parameters,
                                                               &m_soundPlayers[CARPLAYER_ENGINE],
                                                               ::radMakeKey32( clipName ),
                                                               debugPtr );

            //
            // Assume that we're getting into an idle car
            //
            m_engineStates[ENGINE_STATE_IDLING]->SetActive( true );
        }
        else
        {
            //
            // What the...?  No engine clip?  That's not right.
            //
            rTuneString( "WARNING: No engine clip found.  No engine sounds will be played.\n" );
            m_vehicle = NULL;
        }

        //
        // If we have an overlay clip, start it
        //
        overlayName = m_parameters->GetOverlayClipName();
        if( overlayName != NULL )
        {
            m_soundPlayers[CARPLAYER_OVERLAY].PlaySound( overlayName );
        }
    }
    else
    {
        rTunePrintf( "Couldn't find carSoundParameters object named %s, no sound played\n",
                     m_vehicle->GetName() );
        m_vehicle = NULL;
    }
}

//=============================================================================
// VehicleSoundPlayer::StopCarSounds
//=============================================================================
// Description: Stop playing car-related sounds
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void VehicleSoundPlayer::StopCarSounds()
{
    unsigned int i;

    for( i = 0; i < CARPLAYER_NUMPLAYERS; i++ )
    {
        m_soundPlayers[i].Stop();
    }

    for( i = 0; i < NUM_ENGINE_STATES; i++ )
    {
        m_engineStates[i]->SetActive( false );
        m_engineStates[i]->Reset();
    }

    m_proximityAIVehicle = NULL;

    GetEventManager()->TriggerEvent( EVENT_AVATAR_VEHICLE_TOGGLE, m_vehicle );
}

//=============================================================================
// VehicleSoundPlayer::UpdateSoundParameters
//=============================================================================
// Description: Look at the car sound parameter object, and make sure that
//              the engine sound is being played at a pitch appropriate for
//              the vehicle RPMs.
//
// Parameters:  elapsedTime - time since last frame in msecs
//
// Return:      void 
//
//=============================================================================
void VehicleSoundPlayer::UpdateSoundParameters( unsigned int elapsedTime )
{
    //float newPitch;
    //unsigned int shiftTime;
    int i;
    static bool warningPrinted = false;
    bool activateState[NUM_ENGINE_STATES];
    float prevStatePitch[NUM_ENGINE_STATES];
    EngineStateEnum newState;

    //
    // Before updating, check to make sure we have a vehicle.  If we don't,
    // then we got into a car that didn't have a carSoundParameters object
    // associated with it.  Handle it gracefully, but we really should fix it
    //
    if( m_vehicle != NULL )
    {
        for( i = 0; i < NUM_ENGINE_STATES; i++ )
        {
            activateState[i] = false;
            prevStatePitch[i] = 0.0f;
        }

        //
        // Service the active engine states
        //
        for( i = 0; i < NUM_ENGINE_STATES; i++ )
        {
            if( m_engineStates[i]->IsActive() )
            {
                m_engineStates[i]->Service( elapsedTime );
                if( m_engineStates[i]->StartNewState() )
                {
                    newState = m_engineStates[i]->GetNewState();
                    activateState[newState] = true;
                    prevStatePitch[newState] = m_engineStates[i]->GetCurrentPitch();
                }
            }
        }

        //
        // Service fades
        //
        for( i = 0; i < NUM_ENGINE_STATES; i++ )
        {
            if( m_engineStates[i]->IsFading() )
            {
                m_engineStates[i]->ServiceFade( elapsedTime );
            }
        }

        //
        // Initialize debug info
        //
        SET_LOCAL_SHIFT_IN_PROGRESS( false );

        //
        // Start up any states that were marked for activation during servicing
        //
        for( i = 0; i < NUM_ENGINE_STATES; i++ )
        {
            if( activateState[i] )
            {
                m_engineStates[i]->SetActive( true, prevStatePitch[i] );
            }

            m_engineStates[i]->Reset();

            m_engineStates[i]->SetDebugInfo();
        }
    }
    else
    {
        if( !warningPrinted )
        {
            rTuneString("Warning: Vehicle doesn't have a sound associated with it (CarSoundParameters)\n");
            warningPrinted = true;
        }
    }
}

//=============================================================================
// VehicleSoundPlayer::UpdateOncePerFrame
//=============================================================================
// Description: Update routine, used to set the engine RPMs correctly and
//              check for skids, gear changes, and horns
//
// Parameters:  elapsedTime - time since last frame in msecs
//
// Return:      void 
//
//=============================================================================
void VehicleSoundPlayer::UpdateOncePerFrame( unsigned int elapsedTime )
{
    UpdateSoundParameters( elapsedTime );
    checkDamage();
    CheckForSkid( elapsedTime );
    CheckHorn();
    checkProximity();

    //
    // Debug stuff
    //
    if( ( m_parameters != NULL ) && ( m_vehicle != NULL ) )
    {
        /*SET_SHIFT_IN_PROGRESS( m_gearChangeInProgress );
        SET_CURRENT_GEAR( m_currentGear );
        SET_CURRENT_SPEED( m_currentSpeed );
        SET_SHIFT_TIME_REMAINING( m_gearChangeTimeRemaining );
        SET_DOWNSHIFT_SPEED( m_parameters->GetLowShiftPoint( m_currentGear ) * m_vehicle->mDesignerParams.mDpTopSpeedKmh );
        SET_UPSHIFT_SPEED( m_parameters->GetHighShiftPoint( m_currentGear ) * m_vehicle->mDesignerParams.mDpTopSpeedKmh );*/
        SET_LOCAL_IS_DAMAGED( m_playingDamage );
        SET_LOCAL_VEHICLE_LIFE( m_vehicle->GetVehicleLifePercentage(m_vehicle->mHitPoints) );
        SET_LOCAL_DAMAGE_THRESHOLD( m_parameters->GetDamageStartPcnt() );
    }
}

//=============================================================================
// VehicleSoundPlayer::CheckForSkid
//=============================================================================
// Description: Check the vehicle object to see if we're skidding, and make
//              the appropriate noise
//
// Parameters:  elapsedTime - time elapsed in the last frame
//
// Return:      void 
//
//=============================================================================
void VehicleSoundPlayer::CheckForSkid( unsigned int elapsedTime )
{
    bool skidPlaySuccessful;
    float trim;
    float skidLevel;
    float peeloutMin;
    float peeloutMax;
    const char* skidName;
    bool isSupersprint;
    float trimMax;

    //
    // If we don't have a car, don't do anything
    //
    if( m_vehicle == NULL )
    {
        return;
    }

    //
    // Find out if the car is skidding
    //
    skidLevel = m_vehicle->GetSkidLevel();
    peeloutMin = m_peeloutSettings->GetPeeloutMin();
    peeloutMax = m_peeloutSettings->GetPeeloutMax();
    if( skidLevel > peeloutMin )
    {
        //
        // Vehicle skidding.  Start making tire squeal if we're not doing it yet
        //
        if( !m_isSkidding )
        {
            //
            // Get the terrain type underneath the car so we know which sound
            // resource to play
            //
            m_terrainType = m_vehicle->mTerrainType;
            skidName = getSkidResourceForTerrain( m_terrainType );

            //
            // Squeal like a pig, boy
            //
            skidPlaySuccessful =
                m_soundPlayers[CARPLAYER_SKID].PlaySound( skidName );
            rAssert( skidPlaySuccessful );

            m_isSkidding = true;
            m_powerslideTrim = 0.0f;

            //
            // If we're starting from a slow speed, assume this is a burnout
            //
            
            // move the triggering, and triggering of the END of this event, to 
            // Vehicle since Cary wants it also, to play with the camera

            //if( m_vehicle->GetSpeedKmh() < 10.0f )
            //{
            //    GetEventManager()->TriggerEvent( EVENT_BURNOUT );
            //}
        }
        else
        {
            //
            // Check for whether we've changed terrain types
            //
            if( m_vehicle->mTerrainType != m_terrainType )
            {
                m_terrainType = m_vehicle->mTerrainType;
                skidName = getSkidResourceForTerrain( m_terrainType );

                m_soundPlayers[CARPLAYER_SKID].Stop();

                skidPlaySuccessful =
                    m_soundPlayers[CARPLAYER_SKID].PlaySound( skidName );
                rAssert( skidPlaySuccessful );
            }
        }

        if( m_vehicle->mBurnoutLevel > 0.0f )
        {
            //
            // Burnout
            //
            if( skidLevel >= peeloutMax )
            {
                trim = m_peeloutSettings->GetPeeloutMaxTrim();
            }
            else
            {
                trim = ( ( skidLevel - peeloutMin ) / ( peeloutMax - peeloutMin ) )
                    * m_peeloutSettings->GetPeeloutMaxTrim();
            }
        }
        else
        {
            //
            // Powerslide
            //
            if( ( GetGameplayManager() != NULL )
                && ( GetGameplayManager()->IsSuperSprint() ) )
            {
                isSupersprint = true;
                trimMax = SUPERSPRINT_POWERSLIDE_TRIM_MAX;
            }
            else
            {
                isSupersprint = false;
                trimMax = GAME_POWERSLIDE_TRIM_MAX;
            }

            if( m_powerslideTrim <= 0.0f )
            {
                if( isSupersprint )
                {
                    m_powerslideTrim = SUPERSPRINT_POWERSLIDE_TRIM_MIN;
                }
                else
                {
                    m_powerslideTrim = GAME_POWERSLIDE_TRIM_MIN;
                }
            }
            else
            {
                m_powerslideTrim += static_cast<float>(elapsedTime) * 0.0003f;
            }

            if( m_powerslideTrim > trimMax )
            {
                m_powerslideTrim = trimMax;
            }

            trim = m_powerslideTrim;
        }
        m_soundPlayers[CARPLAYER_SKID].SetTrim( trim );
    }
    else
    {
        //
        // Not skidding.  Stop the noise if it's playing
        //
        if( m_isSkidding )
        {
            m_soundPlayers[CARPLAYER_SKID].Stop();
            m_isSkidding = false;
        }
    }
}

//=============================================================================
// VehicleSoundPlayer::CheckHorn
//=============================================================================
// Description: Find the vehicle controller, and check to see whether the horn
//              button is being pressed and play/stop/do nothing with the
//              horn clip appropriately
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void VehicleSoundPlayer::CheckHorn()
{
    int vehicleId;
    VehicleController* controller;
    bool hornPlaySuccessful;
    float buttonVal = 0.0f;
    const char* hornClipName;
    IDaSoundResource* resource;

    if( m_vehicle == NULL )
    {
        return;
    }

    //
    // Find the horn button for this vehicle and get its state
    //
    vehicleId = GetVehicleCentral()->GetVehicleId( m_vehicle, false );
    controller = GetVehicleCentral()->GetVehicleController( vehicleId );

    //
    // Controller might already be gone, if we're playing car sound while
    // the player is getting out of the car.
    //
    if( controller != NULL )
    {
        buttonVal = controller->GetHorn();
    }
    
    //
    // Look for whether we need to start/stop a sound
    //
    if( buttonVal > 0.05f )
    {
        if( !m_hornPlaying )
        {
            //
            // Horn button has just been pressed, find the correct clip and play it
            //
            rAssert( m_parameters != NULL );
            hornClipName = m_parameters->GetHornClipName();

            if( hornClipName == NULL )
            {
                hornClipName = "horn";
            }

            //
            // Just in case we were still playing a non-looping horn sound
            //
            m_soundPlayers[CARPLAYER_HORNPLAYER].Stop();

            hornPlaySuccessful =
                m_soundPlayers[CARPLAYER_HORNPLAYER].PlaySound( hornClipName );
            rAssert( hornPlaySuccessful );

            m_hornPlaying = true;

            //
            // Find out if this is looping horn sound
            //
            resource = Sound::daSoundRenderingManagerGet()->GetResourceManager()->FindResource( hornClipName );
            rAssert( resource != NULL );

            // Sanity check
            if( resource == NULL )
            {
                rTunePrintf( "No horn named %s found\n", hornClipName );
                m_oneTimeHorn = true;
            }
            else
            {
                m_oneTimeHorn = !( resource->GetLooping() );
            }

#ifndef RAD_RELEASE
            //
            // Tuning hack.  Stop and restart the engine to pick up any trim changes
            //
            if( s_resetEngineOnHonk )
            {
                Vehicle* theCar = m_vehicle;
                StopCarSounds();
                StartCarSounds( theCar );
            }
#endif
        }
    }
    else
    {
        if( m_hornPlaying )
        {
            //
            // Horn button has just been released, stop the clip
            //
            if( !m_oneTimeHorn )
            {
                m_soundPlayers[CARPLAYER_HORNPLAYER].Stop();
            }
            m_hornPlaying = false;
        }
    }
}

//=============================================================================
// VehicleSoundPlayer::OnPlaybackComplete
//=============================================================================
// Description: SimpsonsSoundPlayer callback, used when gearshift is done
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void VehicleSoundPlayer::OnPlaybackComplete()
{
}

//=============================================================================
// VehicleSoundPlayer::OnSoundReady
//=============================================================================
// Description: Unused, required for SimpsonsSoundPlayerCallback interface
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void VehicleSoundPlayer::OnSoundReady()
{
}

//=============================================================================
// VehicleSoundPlayer::AddAIVehicleProximityTest
//=============================================================================
// Description: Add check for proximity to newly spawned AI vehicle
//
// Parameters:  aiVehicle - vehicle to test
//
// Return:      void 
//
//=============================================================================
void VehicleSoundPlayer::AddAIVehicleProximityTest( Vehicle* aiVehicle )
{
    if( m_proximityAIVehicle == NULL )
    {
        m_proximityAIVehicle = aiVehicle;
    }
}

//=============================================================================
// VehicleSoundPlayer::DeleteAIVehicleProximityTest
//=============================================================================
// Description: Stop proximity testing for given vehicle, if that's the one
//              we're currently testing.
//
// Parameters:  aiVehicle - vehicle to stop testing for
//
// Return:      void 
//
//=============================================================================
void VehicleSoundPlayer::DeleteAIVehicleProximityTest( Vehicle* aiVehicle )
{
    if( m_proximityAIVehicle == aiVehicle )
    {
        m_proximityAIVehicle = NULL;
    }
}

//=============================================================================
// VehicleSoundPlayer::PlayDoorOpen
//=============================================================================
// Description: Play a door opening sound
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void VehicleSoundPlayer::PlayDoorOpen()
{
}

//=============================================================================
// VehicleSoundPlayer::PlayDoorClose
//=============================================================================
// Description: Play a door closing sound
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void VehicleSoundPlayer::PlayDoorClose()
{
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// VehicleSoundPlayer::checkDamage
//=============================================================================
// Description: Start or stop the damage sound for this vehicle as appropriate
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void VehicleSoundPlayer::checkDamage()
{
    float damagePercentage;
    float lifePercentage;
    float damageTrimPercentage;
    float damageTrim;
    float trimRange;
    float minTrim;

    if( m_vehicle == NULL )
    {
        return;
    }

    damagePercentage = m_parameters->GetDamageStartPcnt();
    lifePercentage = m_vehicle->GetVehicleLifePercentage(m_vehicle->mHitPoints);
    minTrim = m_parameters->GetDamageStartTrim();
    trimRange = m_parameters->GetDamageMaxTrim() - m_parameters->GetDamageStartTrim();

    if( ( lifePercentage <= damagePercentage ) && ( !m_playingDamage ) )
    {
        m_playingDamage = true;
        m_soundPlayers[CARPLAYER_DAMAGE].PlaySound( m_parameters->GetDamagedEngineClipName() );

        //
        // Scale volume by amount of damage
        //
        damageTrimPercentage = ( damagePercentage - lifePercentage ) / m_parameters->GetDamageVolumeRange();
        if( damageTrimPercentage > 1.0f )
        {
            damageTrimPercentage = 1.0f;
        }
        damageTrim = minTrim + ( damageTrimPercentage * trimRange );
        m_soundPlayers[CARPLAYER_DAMAGE].SetTrim( damageTrim );
    }
    else if( m_playingDamage )
    {
        if( lifePercentage > damagePercentage )
        {
            //
            // Must've reset or something
            //
            m_playingDamage = false;
            m_soundPlayers[CARPLAYER_DAMAGE].Stop();
        }
        else
        {
            //
            // Adjust volume
            //
            damageTrimPercentage = ( damagePercentage - lifePercentage ) / m_parameters->GetDamageVolumeRange();
            if( damageTrimPercentage > 1.0f )
            {
                damageTrimPercentage = 1.0f;
            }
            damageTrim = minTrim + ( damageTrimPercentage * trimRange );
            m_soundPlayers[CARPLAYER_DAMAGE].SetTrim( damageTrim );
        }
    }
}

//=============================================================================
// VehicleSoundPlayer::carSoundIsActive
//=============================================================================
// Description: Indicate whether any of the engine sound states are going
//
// Parameters:  None
//
// Return:      true if a state is active, false otherwise 
//
//=============================================================================
bool VehicleSoundPlayer::carSoundIsActive()
{
    int i;
    bool isActive = false;

    for( i = 0; i < NUM_ENGINE_STATES; i++ )
    {
        if( m_engineStates[i]->IsActive() )
        {
            isActive = true;
            break;
        }
    }

    return( isActive );
}

//=============================================================================
// VehicleSoundPlayer::checkProximity
//=============================================================================
// Description: Determine whether AI vehicle has gotten close enough to
//              trigger an event
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void VehicleSoundPlayer::checkProximity()
{
    rmt::Vector position1;
    rmt::Vector position2;

    //rAssert( m_vehicle != NULL );

    if( m_proximityAIVehicle != NULL && m_vehicle != NULL )
    {
        m_vehicle->GetPosition( &position1 );
        m_proximityAIVehicle->GetPosition( &position2 );

        //
        // Trigger event at 20 (use 20^2 for efficiency) metres
        //
        position1.Sub( position2 );
        if( position1.MagnitudeSqr() < 400.0f )
        {
            GetEventManager()->TriggerEvent( EVENT_CHASE_VEHICLE_PROXIMITY );
            m_proximityAIVehicle = NULL;
        }
    }
}

//=============================================================================
// VehicleSoundPlayer::getSkidResourceForTerrain
//=============================================================================
// Description: Find the name of the sound resource for a skid appropriate
//              for the kind of ground we're driving over
//
// Parameters:  terrain - type of terrain the car is over right now
//
// Return:      name of sound resource to use
//
//=============================================================================
const char* VehicleSoundPlayer::getSkidResourceForTerrain( eTerrainType terrain )
{
    IRadNameSpace* nameSpace;
    IRefCount* nameSpaceObj;
    globalSettings* clipNameObj;
    const char* name;
    bool terrainIsDirt = ( terrain == TT_Dirt )
                         || ( terrain == TT_Sand )
                         || ( terrain == TT_Gravel )
                         || ( terrain == TT_Grass );

    //
    // First, see if we have something specific for this car
    //
    rAssert( m_parameters != NULL );
    if( terrainIsDirt )
    {
        name = m_parameters->GetDirtSkidClipName();
    }
    else
    {
        name = m_parameters->GetRoadSkidClipName();
    }
    
    if( name == NULL )
    {
        //
        // No car-specific skid, get the global one from the globalSettings
        // object
        //
        nameSpace = Sound::daSoundRenderingManagerGet()->GetTuningNamespace();
        rAssert( nameSpace != NULL );
        nameSpaceObj = nameSpace->GetInstance( "tuner" );
        rAssert( nameSpaceObj != NULL );

        clipNameObj = static_cast<globalSettings*>( nameSpaceObj );

        if( terrainIsDirt )
        {
            name = clipNameObj->GetSkidDirtClipName();
        }
        else
        {
            name = clipNameObj->GetSkidRoadClipName();
        }
    }

    rAssert( name != NULL );

    return( name );
}
