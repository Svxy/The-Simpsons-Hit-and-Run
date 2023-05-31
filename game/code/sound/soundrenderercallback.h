//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundrenderercallback.h
//
// Description: Declaration of SoundRenderingPlayerCallback class.  Used from
//              sound renderer for player-related callbacks.
//
// History:     06/07/2002 + Created -- Darren
//
//=============================================================================

#ifndef SOUNDRENDERERCALLBACK_H
#define SOUNDRENDERERCALLBACK_H

//========================================
// Nested Includes
//========================================
#include <radobject.hpp>
#include <radlinkedclass.hpp>

#include <sound/soundrenderer/soundsystem.h>

//========================================
// Forward References
//========================================

struct SimpsonsSoundPlayerCallback;
class SimpsonsSoundPlayer;

//=============================================================================
//
// Synopsis:    SoundRenderingPlayerCallback
//
//=============================================================================

class SoundRenderingPlayerCallback : public Sound::IDaSoundPlayerState,
                                     public radLinkedClass< SoundRenderingPlayerCallback >,
                                     public radRefCount
{
    public:
        IMPLEMENT_REFCOUNTED( "SoundRenderingPlayerCallback" );

        SoundRenderingPlayerCallback( SimpsonsSoundPlayer& playerObj,
                                      SimpsonsSoundPlayerCallback* callbackObj );
        virtual ~SoundRenderingPlayerCallback();

        void CancelGameCallbackAndRelease();

        static void CompletionCheck();

        // Currently unused
        void OnSoundReady( void* pData );

        // Called when sound renderer player has completed playback
        void OnSoundDone( void* pData );

    private:
        //Prevent wasteful constructor creation.
        SoundRenderingPlayerCallback();
        SoundRenderingPlayerCallback( const SoundRenderingPlayerCallback& original );
        SoundRenderingPlayerCallback& operator=( const SoundRenderingPlayerCallback& rhs );

        SimpsonsSoundPlayerCallback* m_callbackObj;
        SimpsonsSoundPlayer* m_playerObj;
};


#endif // SOUNDRENDERERCALLBACK_H

