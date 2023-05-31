//=============================================================================
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// File:        musicsoundplayer.cpp
//
// Subsystem:   Dark Angel - Sound players
//
// Description: Implements the a Dark Angel sound player
//
// Revisions:
//  + Created October 16, 2001 -- breimer
//
//=============================================================================

//=============================================================================
// Included Files
//=============================================================================

#include <sound/soundrenderer/musicsoundplayer.h>

#include <sound/soundrenderer/soundrenderingmanager.h>
#include <sound/soundrenderer/idasoundtuner.h>

#include <sound/soundmanager.h>

//=============================================================================
// Static Variables (outside namespace)
//=============================================================================

//=============================================================================
// Namespace
//=============================================================================

namespace Sound {

//=============================================================================
// Debug Information
//=============================================================================

//=============================================================================
// Definitions Macros and Constants
//=============================================================================

//=============================================================================
// Local functions
//=============================================================================

//=============================================================================
// Class Implementations
//=============================================================================

//=============================================================================
// MusicSoundPlayer Implementation
//=============================================================================

//=============================================================================
// Function:    MusicSoundPlayer::MusicSoundPlayer
//=============================================================================
// Description: Constructor
//
//-----------------------------------------------------------------------------

MusicSoundPlayer::MusicSoundPlayer( )
    :
    m_isMusic( true ),
    m_PlayerTrim( 1.0f ),
    m_ResourceTrim( 1.0f ),
    m_ExternalTrim( 1.0f ),
    m_GroupTrim( 1.0f ),
    m_FaderGroupTrim( 1.0f ),
    m_MasterTrim( 1.0f )
{
}

//=============================================================================
// Function:    MusicSoundPlayer::~MusicSoundPlayer
//=============================================================================
// Description: Destructor
//
//-----------------------------------------------------------------------------

MusicSoundPlayer::~MusicSoundPlayer( )
{
}

//=============================================================================
// Function:    MusicSoundPlayer::Initialize
//=============================================================================
// Description: Initialize the sound player to determine whether it will
//              set the trim for music or ambience
//
//-----------------------------------------------------------------------------
void MusicSoundPlayer::Initialize( bool isMusic )
{
    m_isMusic = isMusic;

    //
    // Set the group trim
    //
    if( m_isMusic )
    {
        SetGroupTrim( daSoundRenderingManagerGet()->GetTuner()->GetGroupTrim( Sound::MUSIC ) );
    }
    else
    {
        SetGroupTrim( daSoundRenderingManagerGet()->GetTuner()->GetGroupTrim( Sound::AMBIENCE ) );
    }
}

//=============================================================================
// MusicSoundPlayer::GetSoundGroup
//=============================================================================
// Description: Unlike regular sound players, our group is determined by
//              the music/ambience specification from initialization, not
//              by an associated sound resource
//
// Parameters:  None
//
// Return:      sound group (either MUSIC or AMBIENCE) 
//
//=============================================================================
daSoundGroup MusicSoundPlayer::GetSoundGroup()
{
    if( m_isMusic )
    {
        return( Sound::MUSIC );
    }
    else
    {
        return( Sound::AMBIENCE );
    }
}

//=============================================================================
// MusicSoundPlayer::SetExternalTrim
//=============================================================================
// Description: Set arbitrary player trim.  May not be needed.
//
// Parameters:  newTrim - new trim value
//
// Return:      void 
//
//=============================================================================
void MusicSoundPlayer::SetExternalTrim( float newTrim )
{
    m_ExternalTrim = newTrim;
    m_PlayerTrim = m_ResourceTrim * m_ExternalTrim * m_GroupTrim * m_FaderGroupTrim * m_MasterTrim;

    ResetMusicTrim();
}

//=============================================================================
// MusicSoundPlayer::SetGroupTrim
//=============================================================================
// Description: Set sound group trim
//
// Parameters:  newTrim - new trim setting
//
// Return:      void 
//
//=============================================================================
void MusicSoundPlayer::SetGroupTrim( float newTrim )
{
    m_GroupTrim = newTrim;
    m_PlayerTrim = m_ResourceTrim * m_ExternalTrim * m_GroupTrim * m_FaderGroupTrim * m_MasterTrim;

    ResetMusicTrim();
}

//=============================================================================
// MusicSoundPlayer::SetFaderGroupTrim
//=============================================================================
// Description: Set sound group fader trim
//
// Parameters:  newTrim - new trim setting
//
// Return:      void 
//
//=============================================================================
void MusicSoundPlayer::SetFaderGroupTrim( float newTrim )
{
    m_FaderGroupTrim = newTrim;
    m_PlayerTrim = m_ResourceTrim * m_ExternalTrim * m_GroupTrim * m_FaderGroupTrim * m_MasterTrim;

    ResetMusicTrim();
}

//=============================================================================
// MusicSoundPlayer::SetMasterTrim
//=============================================================================
// Description: Set master trim.  Duh.
//
// Parameters:  newTrim - new trim setting
//
// Return:      void 
//
//=============================================================================
void MusicSoundPlayer::SetMasterTrim( float newTrim )
{
    m_MasterTrim = newTrim;
    m_PlayerTrim = m_ResourceTrim * m_ExternalTrim * m_GroupTrim * m_FaderGroupTrim * m_MasterTrim;

    ResetMusicTrim();
}

//
// TODO: revisit trim functions below.  They're identical to the one in the base
// class, should be virtual.
//

//=============================================================================
// MusicSoundPlayer::ChangeTrim
//=============================================================================
// Description: Change the trim setting for a sound group, or for the master
//              volume
//
// Parameters:  groupName - name of group to change
//              newTrim - trim value
//
// Return:      void 
//
//=============================================================================
void MusicSoundPlayer::ChangeTrim( daSoundGroup groupName, float newTrim )
{
    daSoundGroup myGroup = GetSoundGroup();

    if ( myGroup == groupName )
    {
        SetGroupTrim(newTrim);
    }
    else if ( groupName == MASTER )
    {
        SetMasterTrim(newTrim);
    }
    else if ( daSoundRenderingManagerGet()->GetTuner()->IsSlaveGroup( myGroup, groupName ) )
    {
        SetGroupTrim(newTrim);
    }
}

//=============================================================================
// MusicSoundPlayer::ChangeFaderTrim
//=============================================================================
// Description: Change the trim setting for a sound group, or for the master
//              volume
//
// Parameters:  groupName - name of group to change
//              newTrim - trim value
//
// Return:      void 
//
//=============================================================================
void MusicSoundPlayer::ChangeFaderTrim( daSoundGroup groupName, float newTrim )
{
    daSoundGroup myGroup = GetSoundGroup();

    //
    // Shouldn't change master volume here, use ChangeTrim
    //
    rAssert( groupName != MASTER );

    if ( myGroup == groupName )
    {
        SetFaderGroupTrim(newTrim);
    }
    else if ( daSoundRenderingManagerGet()->GetTuner()->IsSlaveGroup( myGroup, groupName ) )
    {
        SetFaderGroupTrim(newTrim);
    }
}


//=============================================================================
// Private functions
//=============================================================================

//=============================================================================
// MusicSoundPlayer::resetMusicTrim
//=============================================================================
// Description: Update the music player with the given trim setting
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void MusicSoundPlayer::ResetMusicTrim()
{
    if( m_isMusic )
    {
        GetSoundManager()->SetMusicVolumeWithoutTuner( m_PlayerTrim );
    }
    else
    {
        GetSoundManager()->SetAmbienceVolumeWithoutTuner( m_PlayerTrim );
    }
}

} // Sound Namespace

