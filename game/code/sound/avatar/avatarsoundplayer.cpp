//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        avatarsoundplayer.cpp
//
// Description: Implementation of AvatarSoundPlayer, which directs the
//              playing of avatar-related sounds
//
// History:     30/06/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================

#include <radfactory.hpp>

//========================================
// Project Includes
//========================================
#include <sound/avatar/avatarsoundplayer.h>

#include <mission/gameplaymanager.h>
#include <worldsim/avatarmanager.h>

#include <sound/avatar/soundavatar.h>
#include <sound/avatar/carsoundparameters.h>



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
// AvatarSoundPlayer::AvatarSoundPlayer
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
AvatarSoundPlayer::AvatarSoundPlayer()
{
    int i;

    for( i = 0; i < MAX_PLAYERS; i++ )
    {
        m_avatars[i] = NULL;
    }
}

//==============================================================================
// AvatarSoundPlayer::~AvatarSoundPlayer
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
AvatarSoundPlayer::~AvatarSoundPlayer()
{
    int i;

    for( i = 0; i < MAX_PLAYERS; i++ )
    {
        if( m_avatars[i] != NULL )
        {
            delete m_avatars[i];
        }
    }
}

//=============================================================================
// AvatarSoundPlayer::Initialize
//=============================================================================
// Description: Do the initialization stuff for the avatar sound subsystem
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void AvatarSoundPlayer::Initialize()
{
    //
    // Must register the carSoundParameters factory method with RadScript before
    // the script creating these objects gets run
    //
    ::radFactoryRegister( "carSoundParameters", (radFactoryOutParamProc*) ::CarSoundParameterObjCreate );
}

//=============================================================================
// AvatarSoundPlayer::UpdateOncePerFrame
//=============================================================================
// Description: Update function.  Used for stuff that either doesn't need to
//              be called often and/or uses expensive math.
//
// Parameters:  elapsedTime - time since last frame in msecs
//
// Return:      void 
//
//=============================================================================
void AvatarSoundPlayer::UpdateOncePerFrame( unsigned int elapsedTime )
{
    int i;

    for( i = 0; i < MAX_PLAYERS; i++ )
    {
        if( m_avatars[i] != NULL )
        {
            m_avatars[i]->UpdateOncePerFrame( elapsedTime );
        }
    }
}

//=============================================================================
// AvatarSoundPlayer::OnBeginGameplay
//=============================================================================
// Description: Create SoundAvatar objects when gameplay begins
//
// Parameters:  None
//
// Return:      true if player 0 in car, false otherwise 
//
//=============================================================================
bool AvatarSoundPlayer::OnBeginGameplay()
{
    unsigned int i;
    unsigned int numPlayers = GetGameplayManager()->GetNumPlayers();

    for( i = 0; i < numPlayers; i++ )
    {
        rAssert( m_avatars[i] == NULL );

        m_avatars[i] = new(GMA_LEVEL_AUDIO) SoundAvatar( GetAvatarManager()->GetAvatarForPlayer( i ) );
    }

    return( GetAvatarManager()->GetAvatarForPlayer( 0 )->IsInCar() );
}

//=============================================================================
// AvatarSoundPlayer::OnEndGameplay
//=============================================================================
// Description: Destroy SoundAvatar objects when gameplay ends
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void AvatarSoundPlayer::OnEndGameplay()
{
    int i;

    //
    // Destroy the avatar trackers, since we won't have avatars anymore until
    // gameplay restarts
    //
    for( i = 0; i < MAX_PLAYERS; i++ )
    {
        if( m_avatars[i] != NULL )
        {
            delete m_avatars[i];
            m_avatars[i] = NULL;
        }
    }
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
