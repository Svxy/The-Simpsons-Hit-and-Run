//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundavatar.cpp
//
// Description: Implements SoundAvatar, which maintains a reference
//              to an avatar for which sounds are to be created, and directs
//              the playing of sound as either vehicle- or character-based,
//              whichever is appropriate at the time.
//
// History:     30/06/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <sound/avatar/soundavatar.h>

#include <sound/avatar/vehiclesoundplayer.h>
#include <worldsim/avatar.h>
#include <worldsim/character/character.h>
#include <events/eventmanager.h>


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// SoundAvatar::SoundAvatar
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SoundAvatar::SoundAvatar( Avatar* avatarObj ) :
    m_avatar( avatarObj ),
    m_isInCar( false ),
    m_turboTimer( 0 )
{
    rAssert( avatarObj != NULL );

    //
    // Register as an event listener
    //
    GetEventManager()->AddListener( this, EVENT_GETINTOVEHICLE_END );
    GetEventManager()->AddListener( this, EVENT_GETOUTOFVEHICLE_END );
    GetEventManager()->AddListener( this, EVENT_MISSION_RESET );
    GetEventManager()->AddListener( this, EVENT_VEHICLE_DESTROYED_SYNC_SOUND );
    GetEventManager()->AddListener( this, EVENT_CHARACTER_POS_RESET );   
    GetEventManager()->AddListener( this, EVENT_CHASE_VEHICLE_SPAWNED );
    GetEventManager()->AddListener( this, EVENT_CHASE_VEHICLE_DESTROYED );
    GetEventManager()->AddListener( this, EVENT_TURBO_START );

    syncCarSoundState();
}

//==============================================================================
// SoundAvatar::~SoundAvatar
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SoundAvatar::~SoundAvatar()
{
    if( m_isInCar )
    {
        m_vehicleSoundPlayer.StopCarSounds();
    }

    //
    // Deregister from EventManager
    //
    GetEventManager()->RemoveAll( this );
}

//=============================================================================
// SoundAvatar::HandleEvent
//=============================================================================
// Description: Listen for events that tell us if we're swapping between
//              walking and driving
//
// Parameters:  id - indicates which event has occurred
//              pEventData - ptr to Character class, must match to our avatar
//
// Return:      void 
//
//=============================================================================
void SoundAvatar::HandleEvent( EventEnum id, void* pEventData )
{
    switch( id )
    {
        case EVENT_GETINTOVEHICLE_END:
            if( static_cast<Character*>(pEventData) == m_avatar->GetCharacter() )
            {
                m_isInCar = true;

                rAssert( m_avatar->GetVehicle() );
                m_vehicleSoundPlayer.StartCarSounds( m_avatar->GetVehicle() );
            }
            break;

        case EVENT_GETOUTOFVEHICLE_END:
            if( static_cast<Character*>(pEventData) == m_avatar->GetCharacter() )
            {
                m_isInCar = false;

                m_vehicleSoundPlayer.StopCarSounds();
            }
            break;

        case EVENT_MISSION_RESET:
        case EVENT_CHARACTER_POS_RESET:
        case EVENT_VEHICLE_DESTROYED_SYNC_SOUND:
            syncCarSoundState();
            break;

        case EVENT_CHASE_VEHICLE_SPAWNED:
            m_vehicleSoundPlayer.AddAIVehicleProximityTest( static_cast<Vehicle*>(pEventData) );
            break;

        case EVENT_CHASE_VEHICLE_DESTROYED:
            m_vehicleSoundPlayer.DeleteAIVehicleProximityTest( static_cast<Vehicle*>(pEventData) );
            break;

        case EVENT_TURBO_START:
            //
            // If event applies to this character, start a timer.  When it expires,
            // say something funny
            //
            if( ( m_turboTimer == 0 ) &&
                ( static_cast<Character*>(pEventData) == m_avatar->GetCharacter() ) )
            {
                m_turboTimer = 5000;
            }

        default:
            break;
    }
}

//=============================================================================
// SoundAvatar::UpdateOncePerFrame
//=============================================================================
// Description: Update function.  Used for stuff that either doesn't need to
//              be called often and/or uses expensive math.
//
// Parameters:  elapsedTime - time since last frame in msecs
//
// Return:      void 
//
//=============================================================================
void SoundAvatar::UpdateOncePerFrame( unsigned int elapsedTime )
{
    if( m_isInCar )
    {
        m_vehicleSoundPlayer.UpdateOncePerFrame( elapsedTime );
    }

    if( m_turboTimer > 0 )
    {
        if( !(m_avatar->GetCharacter()->IsTurbo()) )
        {
            //
            // Player no longer sprinting, stop the countdown
            //
            m_turboTimer = 0;
        }
        else if( elapsedTime >= m_turboTimer )
        {
            //
            // Timer expired.  Time to make with the funny.
            //
            GetEventManager()->TriggerEvent( EVENT_CHARACTER_TIRED_NOW );

            //
            // Set up for another line, but if it's a repeat, let's make
            // it take a little longer
            //m_turboTimer = 10000;

            //
            // Okay, the repeating line is unpopular.  Just play it once
            // and be done.
            //
            m_turboTimer = 0;
        }
        else
        {
            m_turboTimer -= elapsedTime;
        }
    }
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// SoundAvatar::syncCarSoundState
//=============================================================================
// Description: Determine whether the avatar is in the car, and start or 
//              stop engine sounds accordingly.
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundAvatar::syncCarSoundState()
{
    m_isInCar = m_avatar->IsInCar();

    if( m_isInCar )
    {
        rAssert( m_avatar->GetVehicle() );

        m_vehicleSoundPlayer.StartCarSounds( m_avatar->GetVehicle() );
    }
    else
    {
        m_vehicleSoundPlayer.StopCarSounds();
    }
}