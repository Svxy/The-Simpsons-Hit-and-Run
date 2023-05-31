//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundfxlogic.cpp
//
// Description: Implement the SoundFXLogic class, which is an abstract
//              base class for objects that translate events into sound effects
//              in the different game states
//
// History:     31/07/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <sound/soundfx/soundfxlogic.h>

#include <events/eventmanager.h>
#include <events/eventdata.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

struct CreditInfo
{
    int lineNumber;
    radKey32 dialogName;
};

#ifdef PAL
    const int PAL_OFFSET = +16; // due to additional VUG localization team
#else
    const int PAL_OFFSET = 0;
#endif

static CreditInfo s_creditDialogTable[] =
{
    { 7, ::radMakeKey32( "pubcredits" ) },
    { 55, ::radMakeKey32( "foxcredits" ) },
    { 178 + PAL_OFFSET, ::radMakeKey32( "radproducer" ) },
    { 190 + PAL_OFFSET, ::radMakeKey32( "radlead" ) },
    { 204 + PAL_OFFSET, ::radMakeKey32( "raddesign" ) },
    { 221 + PAL_OFFSET, ::radMakeKey32( "radworld" ) },
    { 230 + PAL_OFFSET, ::radMakeKey32( "radmodel" ) },
    { 235 + PAL_OFFSET, ::radMakeKey32( "radfx" ) },
    { 242 + PAL_OFFSET, ::radMakeKey32( "radfmvart" ) },
    { 251 + PAL_OFFSET, ::radMakeKey32( "radfeart" ) },
    { 260 + PAL_OFFSET, ::radMakeKey32( "radprog" ) },
    { 279 + PAL_OFFSET, ::radMakeKey32( "radtest" ) },
    { 289 + PAL_OFFSET, ::radMakeKey32( "radsound" ) }
};

static int s_creditDialogTableSize = sizeof( s_creditDialogTable ) / sizeof( CreditInfo );

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// SoundFXLogic::SoundFXLogic
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SoundFXLogic::SoundFXLogic()
{
    unsigned int i;

    for( i = 0; i < s_numSFXPlayers; i++ )
    {
        m_soundPlayers[i].isKillable = true;
    }
}

//==============================================================================
// SoundFXLogic::~SoundFXLogic
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SoundFXLogic::~SoundFXLogic()
{
}

//=============================================================================
// SoundFXLogic::UnregisterEventListeners
//=============================================================================
// Description: Unregister all events with the Event Manager
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundFXLogic::UnregisterEventListeners()
{
    GetEventManager()->RemoveAll( this );
}

//=============================================================================
// SoundFXLogic::GetAvailableSFXPlayer
//=============================================================================
// Description: Find an unused SFXPlayer.  Failing that, find a killable
//              SFXPlayer.  Failing that, well, just fail.
//
// Parameters:  index - Address of unsigned int, filled in with index of
//                      SFXPlayer if one is available and address is non-NULL, 
//                      untouched otherwise
//
// Return:      SFXPlayer pointer if one available, NULL otherwise
//
//=============================================================================
SFXPlayer* SoundFXLogic::GetAvailableSFXPlayer( unsigned int* index )
{
    unsigned int i;
    int lastKillable = -1;

    //
    // First, look for free players
    //
    for( i = 0; i < s_numSFXPlayers; i++ )
    {
        if( !m_soundPlayers[i].soundPlayer.IsInUse() )
        {
            if( index != NULL )
            {
                *index = i;
            }
            return( &(m_soundPlayers[i]) );
        }
        else if( m_soundPlayers[i].isKillable )
        {
            lastKillable = i;
        }
    }

    //
    // If we get this far, all players are in use.  Kill a player if we can
    //
    if( lastKillable != -1 )
    {
        if( index != NULL )
        {
            *index = lastKillable;
        }
        return( &(m_soundPlayers[lastKillable]) );
    }
    else
    {
        return( NULL );
    }
}

//=============================================================================
// SoundFXLogic::playSFXSound
//=============================================================================
// Description: Searches through the list of SFXPlayers for one that's free.
//              If one isn't found, find one that's killable.  If none of them
//              are killable, eh, no sound.
//
// Parameters:  resource - name of sound resource to play
//              killable - true if sound can be killed prematurely, false otherwise
//              useCallback - true if we set callback on playback completion,
//                            false otherwise
//              index - to be filled in with index of SFXPlayer used if sound
//                      played and index is non-NULL
//              trim - volume to play sound at
//              pitch - pitch to play sound at
//
// Return:      true if sound played, false otherwise
//
//=============================================================================
bool SoundFXLogic::playSFXSound( const char* resource, bool killable, 
                                 bool useCallback, unsigned int* index,
                                 float trim, float pitch )
{
    SFXPlayer* player;
    bool success = false;
    SimpsonsSoundPlayerCallback* callbackObj = NULL;

    //
    // Get a player if possible
    //
    player = GetAvailableSFXPlayer( index );

    if( player != NULL )
    {
        if( player->soundPlayer.IsInUse() )
        {
            player->soundPlayer.Stop();
        }

        if( useCallback )
        {
            callbackObj = this;
        }
        player->soundPlayer.PlaySound( resource, callbackObj );
        player->soundPlayer.SetTrim( trim );
        player->soundPlayer.SetPitch( pitch );
        player->isKillable = killable;

        success = true;
    }
    else
    {
        rDebugString( "Dropped sound effect, no player available\n" );
    }

    return( success );
}

//=============================================================================
// SoundFXLogic::ServiceOncePerFrame
//=============================================================================
// Description: Does nothing.  Subclasses with servicing requirements need
//              to override this function
//
// Parameters:  elapsedTime - time elapsed since last frame
//
// Return:      void 
//
//=============================================================================
void SoundFXLogic::ServiceOncePerFrame( unsigned int elapsedTime )
{
}

//=============================================================================
// SoundFXLogic::OnSoundReady
//=============================================================================
// Description: Does nothing.  Needed to pull this in to get OnPlaybackComplete
//              from SimpsonsSoundPlayer.
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundFXLogic::OnSoundReady()
{
}

//=============================================================================
// SoundFXLogic::OnPlaybackComplete
//=============================================================================
// Description: Does nothing.  Subclasses with callback requirements need
//              to override this virtual function.
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundFXLogic::OnPlaybackComplete()
{
}

//=============================================================================
// SoundFXLogic::Cleanup
//=============================================================================
// Description: Does nothing.  Subclasses with stuff to clean up override
//              this virtual function.
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundFXLogic::Cleanup()
{
}

//******************************************************************************
//
// Protected Member Functions
//
//******************************************************************************

//=============================================================================
// SoundFXLogic::playCreditLine
//=============================================================================
// Description: Play a credits conversation
//
// Parameters:  lineNumber - last scrolled line number in credits text
//
// Return:      void 
//
//=============================================================================
void SoundFXLogic::playCreditLine( int lineNumber )
{
    int i;
    DialogEventData data;

    for( i = 0; i < s_creditDialogTableSize; i++ )
    {
        if( lineNumber == s_creditDialogTable[i].lineNumber )
        {
            data.charUID1 = tEntity::MakeUID( "kang" );
            data.charUID2 = tEntity::MakeUID( "kodos" );
            data.dialogName = s_creditDialogTable[i].dialogName;

            GetEventManager()->TriggerEvent( EVENT_IN_GAMEPLAY_CONVERSATION, static_cast<void*>(&data) );
        }
    }
}
