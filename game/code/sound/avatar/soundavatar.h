//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundavatar.h
//
// Description: Declaration of SoundAvatar class, which maintains a reference
//              to an avatar for which sounds are to be created, and directs
//              the playing of sound as either vehicle- or character-based,
//              whichever is appropriate at the time.
//
// History:     30/06/2002 + Created -- Darren
//
//=============================================================================

#ifndef SOUNDAVATAR_H
#define SOUNDAVATAR_H

//========================================
// Nested Includes
//========================================

#include <sound/avatar/vehiclesoundplayer.h>

#include <events/eventlistener.h>

//========================================
// Forward References
//========================================

class Avatar;

//=============================================================================
//
// Synopsis:    SoundAvatar
//
//=============================================================================

class SoundAvatar : public EventListener
{
    public:
        SoundAvatar( Avatar* avatarObj );
        virtual ~SoundAvatar();

        void HandleEvent( EventEnum id, void* pEventData );

        void UpdateOncePerFrame( unsigned int elapsedTime );

    private:
        //Prevent wasteful constructor creation.
        SoundAvatar();
        SoundAvatar( const SoundAvatar& original );
        SoundAvatar& operator=( const SoundAvatar& rhs );
        
        void syncCarSoundState();

        //
        // Avatar object that we're going to keep an eye on
        //
        Avatar* m_avatar;

        //
        // Is the player in the car?
        //
        bool m_isInCar;

        //
        // Object for playing vehicle sounds
        VehicleSoundPlayer m_vehicleSoundPlayer;

        //
        // Timer for character sprinting
        //
        unsigned int m_turboTimer;
};


#endif // SOUNDAVATAR_H

