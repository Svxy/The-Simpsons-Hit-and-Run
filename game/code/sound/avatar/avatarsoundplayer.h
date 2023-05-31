//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        avatarsoundplayer.h
//
// Description: Declaration of AvatarSoundPlayer class, which directs the
//              playing of avatar-related sounds
//
// History:     30/06/2002 + Created -- Darren
//
//=============================================================================

#ifndef AVATARSOUNDPLAYER_H
#define AVATARSOUNDPLAYER_H

//========================================
// Nested Includes
//========================================
#include <constants/maxplayers.h>

//========================================
// Forward References
//========================================

class SoundAvatar;

//=============================================================================
//
// Synopsis:    AvatarSoundPlayer
//
//=============================================================================

class AvatarSoundPlayer
{
    public:
        AvatarSoundPlayer();
        virtual ~AvatarSoundPlayer();

        void Initialize();

        void UpdateOncePerFrame( unsigned int elapsedTime );
        
        //
        // Returns true if first player in car, false otherwise
        //
        bool OnBeginGameplay();

        void OnEndGameplay();

    private:
        //Prevent wasteful constructor creation.
        AvatarSoundPlayer( const AvatarSoundPlayer& original );
        AvatarSoundPlayer& operator=( const AvatarSoundPlayer& rhs );

        //
        // One SoundAvatar to track activity of each Avatar object
        //
        SoundAvatar* m_avatars[MAX_PLAYERS];
};


#endif // AVATARSOUNDPLAYER_H

