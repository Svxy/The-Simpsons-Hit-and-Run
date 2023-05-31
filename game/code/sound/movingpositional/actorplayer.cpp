//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        actorplayer.cpp
//
// Description: Implement ActorPlayer
//
// History:     3/10/2003 + Created -- Esan
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <sound/movingpositional/actorplayer.h>

#include <ai/actor/actor.h>

//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// ActorPlayer::ActorPlayer
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
ActorPlayer::ActorPlayer() :
    m_actor( NULL )
{
}

//=============================================================================
// ActorPlayer::~ActorPlayer
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
ActorPlayer::~ActorPlayer()
{
}

//=============================================================================
// ActorPlayer::GetPosition
//=============================================================================
// Description: Get position of actor for moving positional sound
//
// Parameters:  position - output parameter, to be filled with actor position
//
// Return:      void 
//
//=============================================================================
void ActorPlayer::GetPosition( radSoundVector& position )
{
    rmt::Vector actorPosn;

    m_actor->GetPosition( &actorPosn );
    position.SetElements( actorPosn.x, actorPosn.y, actorPosn.z );
}

//=============================================================================
// ActorPlayer::GetVelocity
//=============================================================================
// Description: Get velocity of actor for moving positional sound
//
// Parameters:  velocity - output parameter, to be filled with actor velocity
//
// Return:      void 
//
//=============================================================================
void ActorPlayer::GetVelocity( radSoundVector& velocity )
{
    //
    // No doppler for actors, therefore make velocity zero
    //
    velocity.SetElements( 0.0f, 0.0f, 0.0f );
}

//=============================================================================
// ActorPlayer::ServiceOncePerFrame
//=============================================================================
// Description: Service stuff
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void ActorPlayer::ServiceOncePerFrame()
{
    m_player.ServiceOncePerFrame();
}

//=============================================================================
// ActorPlayer::OnPlaybackComplete
//=============================================================================
// Description: Sound player callback.  Do nothing, leave for subclasses
//              to override
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void ActorPlayer::OnPlaybackComplete()
{
}

//=============================================================================
// ActorPlayer::OnSoundReady
//=============================================================================
// Description: Sound player callback.  Do nothing, leave for subclasses
//              to override
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void ActorPlayer::OnSoundReady()
{
}

//=============================================================================
// ActorPlayer::playSound
//=============================================================================
// Description: Comment
//
// Parameters:  settings - positional sound attributes
//              resourceName - name of sound resource to play
//              theActor - the moving object that this sound is associated with
//
// Return:      void 
//
//=============================================================================
void ActorPlayer::playSound( positionalSoundSettings* settings, 
                             const char* resourceName,
                             Actor* theActor )
{
    rmt::Vector posn;

    m_actor = theActor;

    m_player.SetPositionCarrier( *this );
    m_player.SetParameters( settings );

    theActor->GetPosition( &posn );
    m_player.SetPosition( posn.x, posn.y, posn.z );

    m_player.PlaySound( resourceName, this );
}

//=============================================================================
// ActorPlayer::deactivate
//=============================================================================
// Description: Shut down the sound and disassociate from the actor
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void ActorPlayer::deactivate()
{
    m_actor = NULL;
    m_player.Stop();
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************
