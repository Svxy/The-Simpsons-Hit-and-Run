//=============================================================================
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// File:        playermanager.h
//
// Subsystem:   Dark Angel - Player Manager System
//
// Description: Description of the DA sound player manager
//
// Revisions:
//  + Created October 16, 2001 -- breimer
//
//=============================================================================

#ifndef _PLAYERMANAGER_HPP
#define _PLAYERMANAGER_HPP

//=============================================================================
// Included Files
//=============================================================================

#include <radobject.hpp>

#include <sound/soundrenderer/soundsystem.h>
#include <sound/soundrenderer/idasoundtuner.h>
#include <sound/soundrenderer/idasoundresource.h>
#include <sound/soundrenderer/musicsoundplayer.h>
#include <radsound.hpp>

//=============================================================================
// Global namespace forward declarations
//=============================================================================

struct IRadObjectList;

class Fader;

//=============================================================================
// Namespace
//=============================================================================

namespace Sound {

//=============================================================================
// Prototypes
//=============================================================================

class daSoundPlayerGroupWiring;
class daSoundPlayerManager;

//=============================================================================
// Forward declarations
//=============================================================================

class daSoundClipStreamPlayer;

//=============================================================================
// Class Declarations
//=============================================================================

//
// This class is created for our asynchronous fades
//

class daSoundAsyncFadeCallback : public radObject
{
public:
    IMPLEMENT_BASEOBJECT( "daSoundAsyncFadeCallback" )

    daSoundAsyncFadeCallback( );
    virtual ~daSoundAsyncFadeCallback( );

    void SetAction( int action ) { m_Action = action; }
    int GetAction( void ) { return m_Action; }

    void SetPlayerManager( daSoundPlayerManager* pPlayerManager );
    daSoundPlayerManager* GetPlayerManager( void );

    void SetCallback( IDaSoundFadeState* pCallback, void* pUserData );
    void GetCallback( IDaSoundFadeState** ppCallback, void** ppUserData );

private:
    
    int                         m_Action;
    daSoundPlayerManager*       m_pPlayerManager;
    IDaSoundFadeState*          m_pCallback;
    void*                       m_pUserData;
};

//
// The player manager is responsible for creating and managing daSoundClipStreamPlayer
// objects.  These objects allow the user to play sound resources.
//
class daSoundPlayerManager :  public IDaSoundFadeState,
                              public radRefCount
{
public:
    IMPLEMENT_REFCOUNTED_NOSIZE( "daSoundPlayerManager" );

    //
    // Constructor and destructor
    //
    daSoundPlayerManager( );
    virtual ~daSoundPlayerManager( );
    
    inline daSoundPlayerManager * GetInstance( void );

    bool FindFreeClipPlayer(
        daSoundClipStreamPlayer** ppPlayer,
        IDaSoundResource* pResource );
    bool FindFreeStreamPlayer(
        daSoundClipStreamPlayer** ppPlayer,
        IDaSoundResource* pResource );

    unsigned int GetNumUsedClipPlayers();
    unsigned int GetNumUsedStreamPlayers();
    void Initialize( void );
    void UglyHackPostInitialize( IDaSoundTuner* pTuner );
    void ServiceOncePerFrame( void );
    unsigned int GetObjectSize( void );
    void CaptureFreePlayer(
        daSoundClipStreamPlayer** ppPlayer,
        IDaSoundResource* pResource,
        bool positional );

    void PausePlayers ( );
    void PausePlayersWithFade(
        IDaSoundFadeState* pCallback,
        void* pUserData );
        
    void ContinuePlayers ( );
    
    void ContinuePlayersWithFade(
        IDaSoundFadeState* pCallback,
        void* pUserData );
        
    void CancelPlayers ( );

    bool AreAllPlayersStopped();

    //
    // Volume controls
    //
    void PlayerVolumeChange( daSoundGroup soundGroup, daTrimValue trim );
    void PlayerFaderVolumeChange( daSoundGroup soundGroup, daTrimValue trim );
    
    void Render( void );

protected:
    // When a fade is done go here and call our callback
    enum FadeTypesEnum {
        OnFade_PausePlayers,
        OnFade_ContinuePlayers,
        OnFade_CancelPlayers
    };
    void OnFadeDone( void* pUserData );

private:

    bool FindFreePlayer( daSoundClipStreamPlayer** ppPlayerArray, unsigned int numPlayers, daSoundClipStreamPlayer ** ppPlayer );
    
    MusicSoundPlayer* m_pMusicPlayer;
    MusicSoundPlayer* m_pAmbiencePlayer;
    
    //
    // The ingame faders
    //
    Fader*                      m_pIngameFadeIn;
    Fader*                      m_pIngameFadeOut;
    
    bool m_Initialized;
    
    static daSoundPlayerManager * s_pInstance;

};

inline daSoundPlayerManager * daSoundPlayerManager::GetInstance( void )
{
    return s_pInstance;
}
    
} // Sound Namespace
#endif //_PLAYERMANAGER_HPP


