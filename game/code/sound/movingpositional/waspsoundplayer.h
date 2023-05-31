//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        waspsoundplayer.h
//
// Description: Plays sound for those flying bugs
//
// History:     3/10/2003 + Created -- Esan
//
//=============================================================================

#ifndef WASPSOUNDPLAYER_H
#define WASPSOUNDPLAYER_H

//========================================
// Nested Includes
//========================================
#include <sound/movingpositional/actorplayer.h>

#include <events/eventlistener.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    WaspSoundPlayer
//
//=============================================================================

class WaspSoundPlayer : public ActorPlayer,
                        public EventListener
{
    public:
        WaspSoundPlayer();
        virtual ~WaspSoundPlayer();

        //
        // ActorPlayer functions
        //
        void Activate( Actor* theActor );
        void OnPlaybackComplete();

        //
        // EventListener functions
        //
        void HandleEvent( EventEnum id, void* pEventData );

    private:
        //Prevent wasteful constructor creation.
        WaspSoundPlayer( const WaspSoundPlayer& waspsoundplayer );
        WaspSoundPlayer& operator=( const WaspSoundPlayer& waspsoundplayer );

        void playWaspSound( const char* soundName, Actor* theActor );

        void deactivate();
        void safeStop();

        bool m_isFadingIn;
        bool m_attacking;
        bool m_blowingUp;
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

#endif //WASPSOUNDPLAYER_H
