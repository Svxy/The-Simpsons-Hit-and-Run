//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        actorplayer.h
//
// Description: Abstract base class for playing sounds associated with moving 
//              state props (e.g. wasps)
//
// History:     3/10/2003 + Created -- Darren
//
//=============================================================================

#ifndef ACTORPLAYER_H
#define ACTORPLAYER_H

//========================================
// Nested Includes
//========================================
#include <sound/positionalsoundplayer.h>

//========================================
// Forward References
//========================================
class Actor;

//=============================================================================
//
// Synopsis:    ActorPlayer
//
//=============================================================================

class ActorPlayer : public PositionCarrier,
                    public SimpsonsSoundPlayerCallback
{
public:
    ActorPlayer();
    virtual ~ActorPlayer();

    bool IsActive() { return( m_actor != NULL ); }
    virtual void Activate( Actor* theActor ) = 0;

    virtual void ServiceOncePerFrame();

    //
    // PositionCarrier functions
    //
    void GetPosition( radSoundVector& position );
    void GetVelocity( radSoundVector& velocity );

    //
    // SimpsonsSoundPlayerCallback functions
    //
    virtual void OnSoundReady();
    virtual void OnPlaybackComplete();

protected:
    void playSound( positionalSoundSettings* settings, const char* resourceName, Actor* theActor );

    virtual void deactivate();

    Actor* m_actor;

    PositionalSoundPlayer m_player;

private:
    //Prevent wasteful constructor creation.
    ActorPlayer( const ActorPlayer& actorplayer );
    ActorPlayer& operator=( const ActorPlayer& actorplayer );
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

#endif //ACTORPLAYER_H
