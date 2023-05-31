//=============================================================================
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// File:        playermagaer.cpp
//
// Subsystem:   Dark Angel - Player Manager System
//
// Description: Implementation of the DA sound player manager
//
// Revisions:
//  + Created October 16, 2001 -- breimer
//
//=============================================================================

//=============================================================================
// Included Files
//=============================================================================

#include <radobject.hpp>
#include <radobjectlist.hpp>
#include <raddebug.hpp>
#include <radlinkedclass.hpp>
#include <radnamespace.hpp>
#include <radsound_hal.hpp>

#include <memory/srrmemory.h>

#include <sound/soundrenderer/soundsystem.h>
#include <sound/soundrenderer/dasoundgroup.h>
#include <sound/soundrenderer/soundrenderingmanager.h>
#include <sound/soundrenderer/playermanager.h>
#include <sound/soundrenderer/soundplayer.h>
#include <sound/soundrenderer/musicsoundplayer.h>
#include <sound/soundrenderer/idasoundtuner.h>
#include <sound/soundrenderer/fader.h>
#include <sound/soundrenderer/soundconstants.h>
#include <sound/soundrenderer/soundnucleus.hpp>

#include <pddi/pddi.hpp>
#include <p3d/utility.hpp>
#include <p3d/p3dtypes.hpp>

#include <radobjectbtree.hpp>


//=============================================================================
// Namespace
//=============================================================================

namespace Sound {

//=============================================================================
// Debug Information
//=============================================================================

daSoundPlayerManager * daSoundPlayerManager::s_pInstance = 0;

//=============================================================================
// Class Implementations
//=============================================================================

//=============================================================================
// daSoundAsyncFadeCallback Implementation
//=============================================================================

//=============================================================================
// Function:    daSoundAsyncFadeCallback::daSoundAsyncFadeCallback
//=============================================================================
// Description: Constructor
//
//-----------------------------------------------------------------------------

daSoundAsyncFadeCallback::daSoundAsyncFadeCallback( )
:
radObject( ),
m_Action( 0 ),
m_pPlayerManager( NULL ),
m_pCallback( NULL ),
m_pUserData( NULL )
{
     
}

//=============================================================================
// Function:    daSoundAsyncFadeCallback::~daSoundAsyncFadeCallback
//=============================================================================
// Description: Destructor
//
//-----------------------------------------------------------------------------

daSoundAsyncFadeCallback::~daSoundAsyncFadeCallback( )
{
    
    if( m_pCallback != NULL )
    {
        m_pCallback->Release( );
    }
    if( m_pPlayerManager != NULL )
    {
        m_pPlayerManager->Release( );
    }
}

//=============================================================================
// Function:    daSoundAsyncFadeCallback::SetPlayerManager
//=============================================================================
// Description: Set the player manager
//
//-----------------------------------------------------------------------------

void daSoundAsyncFadeCallback::SetPlayerManager
(
    daSoundPlayerManager* pPlayerManager
)
{
    rAssert( m_pPlayerManager == NULL );
    rAssert( pPlayerManager != NULL );

    m_pPlayerManager = pPlayerManager;
    m_pPlayerManager->AddRef( );
}

//=============================================================================
// Function:    daSoundAsyncFadeCallback::GetPlayerManager
//=============================================================================
// Description: Get the player manager
//
//-----------------------------------------------------------------------------

daSoundPlayerManager* daSoundAsyncFadeCallback::GetPlayerManager( void )
{
    return m_pPlayerManager;
}

//=============================================================================
// Function:    daSoundAsyncFadeCallback::SetCallback
//=============================================================================
// Description: Set the fade callback
//
//-----------------------------------------------------------------------------

void daSoundAsyncFadeCallback::SetCallback
(
    IDaSoundFadeState* pCallback,
    void* pUserData
)
{
    rAssert( m_pCallback == NULL );
    m_pCallback = pCallback;
    m_pUserData = pUserData;

    if( m_pCallback != NULL )
    {
        m_pCallback->AddRef( );
    }    
}

//=============================================================================
// Function:    daSoundAsyncFadeCallback::GetCallback
//=============================================================================
// Description: Get the fade callback
//
//-----------------------------------------------------------------------------

void daSoundAsyncFadeCallback::GetCallback( IDaSoundFadeState** ppCallback, void** ppUserData )
{
    rAssert( ppCallback != NULL );
    rAssert( ppUserData != NULL );

    *ppCallback = m_pCallback;
    *ppUserData = m_pUserData;
}


//=============================================================================
// daSoundPlayerManager Implementation
//=============================================================================

//=============================================================================
// Function:    daSoundPlayerManager::daSoundPlayerManager
//=============================================================================
// Description: Constructor
//
//-----------------------------------------------------------------------------

daSoundClipStreamPlayer * gClipPlayerArray[ SOUND_NUM_CLIP_PLAYERS ];
daSoundClipStreamPlayer * gStreamPlayerArray [ SOUND_NUM_STREAM_PLAYERS ];

daSoundPlayerManager::daSoundPlayerManager( )
    :
    radRefCount( 0 )
{
    m_pIngameFadeIn = NULL;
    m_pIngameFadeOut = NULL;
    m_pMusicPlayer = NULL;
    m_pAmbiencePlayer = NULL;
    m_Initialized = false;    
    
    s_pInstance = this;
}

//=============================================================================
// Function:    daSoundPlayerManager::~daSoundPlayerManager
//=============================================================================
// Description: Destructor
//
//-----------------------------------------------------------------------------

daSoundPlayerManager::~daSoundPlayerManager( )
{
    s_pInstance = NULL;
    // Release our faders
    if( m_pIngameFadeOut != NULL )
    {
        m_pIngameFadeOut->Release();
        m_pIngameFadeOut = NULL;
    }
    if( m_pIngameFadeIn != NULL )
    {
        m_pIngameFadeIn->Release();
        m_pIngameFadeIn = NULL;
    }

    // Release music players
    if( m_pMusicPlayer != NULL )
    {
        m_pMusicPlayer->Release();
        m_pMusicPlayer = NULL;
    }
    if( m_pAmbiencePlayer != NULL )
    {
        m_pAmbiencePlayer->Release();
        m_pAmbiencePlayer = NULL;
    }

    for( unsigned int c = 0; c < SOUND_NUM_CLIP_PLAYERS; c ++ )
    {
        gClipPlayerArray[ c ]->Release( );
    }
    
    for( unsigned int s = 0; s < SOUND_NUM_STREAM_PLAYERS; s ++ )
    {
        gStreamPlayerArray[ s ]->Release( );
    } 
}

//=============================================================================
// daSoundPlayerManager::UglyHackPostInitialize
//=============================================================================
// Description: Comment
//
// Parameters:  ( IDaSoundTuner* pTuner )
//
// Return:      void 
//
//=============================================================================
void daSoundPlayerManager::UglyHackPostInitialize( IDaSoundTuner* pTuner )
{
    // Ingame faders
    m_pIngameFadeIn = new( GMA_PERSISTENT ) Fader( NULL, DUCK_FULL_FADE, *this, *pTuner );
    rAssert( m_pIngameFadeIn != NULL );

    m_pIngameFadeOut = new( GMA_PERSISTENT ) Fader( NULL, DUCK_FULL_FADE, *this, *pTuner );
    rAssert( m_pIngameFadeOut != NULL );
}

//=============================================================================
// Function:    daSoundPlayerManager::GetObjectSize
//=============================================================================
// Description: Get the size of the sound player object
//-----------------------------------------------------------------------------

unsigned int daSoundPlayerManager::GetObjectSize( void )
{
    unsigned int thisSize = sizeof( *this );
    return thisSize;
}

unsigned int daSoundPlayerManager::GetNumUsedClipPlayers()
{
    unsigned int playerCount = 0;

    for( unsigned int c = 0; c < SOUND_NUM_CLIP_PLAYERS; c ++ )
    {
        if ( gClipPlayerArray[ c ]->IsCaptured( ) )
        {
            playerCount++;
        }
    }

    return( playerCount );
}

unsigned int daSoundPlayerManager::GetNumUsedStreamPlayers()
{
    unsigned int playerCount = 0;

    for( unsigned int s = 0; s < SOUND_NUM_STREAM_PLAYERS; s ++ )
    {
        if ( gStreamPlayerArray[ s ]->IsCaptured( ) )
        {
            playerCount++;
        }
    }
    
    return playerCount;
}

//=============================================================================
// Function:    daSoundPlayerManager::Initialize
//=============================================================================
// Description: Initialize the player manager
//
//-----------------------------------------------------------------------------

void daSoundPlayerManager::Initialize( void )
{
    // The stream players are tuned from associated radscripts

    IRadSoundClipPlayer* pClipPlayer = NULL;
    IRadSoundStreamPlayer* pStreamPlayer = NULL;
    IRadSoundStitchedDataSource* pStitchedDataSource = NULL;

    // CLIP PLAYERS ///////////////////////////////////////////////////////////

    for( unsigned int c = 0; c < SOUND_NUM_CLIP_PLAYERS; c++ )
    {
        gClipPlayerArray[ c ] = new (GetThisAllocator( ) ) daSoundClipStreamPlayer( );
        gClipPlayerArray[ c ]->AddRef( );
        gClipPlayerArray[ c ]->InitializeAsClipPlayer( );
    }

    // STREAM PLAYERS /////////////////////////////////////////////////////////

    for( unsigned int s = 0; s < SOUND_NUM_STREAM_PLAYERS; s++ )
    {
    
        gStreamPlayerArray[ s ] = new ( GetThisAllocator() ) daSoundClipStreamPlayer;
        gStreamPlayerArray[ s ]->AddRef( );
        gStreamPlayerArray[ s ]->InitializeAsStreamPlayer( );
    }

    //
    // Make a couple of fake players for controlling music and
    // ambience trim
    //
    
    // Create a music and an ambience sound player
    m_pMusicPlayer = new( GetThisAllocator() ) MusicSoundPlayer;
    m_pMusicPlayer->AddRef( );
    m_pMusicPlayer->Initialize( true );

    m_pAmbiencePlayer = new( GetThisAllocator() ) MusicSoundPlayer;
    m_pAmbiencePlayer->AddRef( );
    m_pAmbiencePlayer->Initialize( false );
    
    m_Initialized = true;
    
}

//=============================================================================
// Function:    daSoundPlayerManager::ServiceOncePerFrame
//=============================================================================
// Description: Service once per frame
//
//-----------------------------------------------------------------------------

void daSoundPlayerManager::ServiceOncePerFrame( void )
{
    daSoundPlayerBase* pPlayer = daSoundPlayerBase::GetLinkedClassHead( );
    while( pPlayer != NULL )
    {
        pPlayer->ServiceOncePerFrame( );
        pPlayer = pPlayer->GetLinkedClassNext( );
    }
}

//=============================================================================
// Function:    daSoundPlayerManager::CaptureFreePlayer
//=============================================================================
// Description: This function finds and captures a sound player
//              and connects the given resource to it.
//
// Parameters:  ppPlayer - (out) the captured player or NULL if one can't be
//                         found
//              pResource - a pointer to a sound resource
//
//-----------------------------------------------------------------------------

void daSoundPlayerManager::CaptureFreePlayer
(
    daSoundClipStreamPlayer** ppPlayer,
    IDaSoundResource* pResource,
    bool positional
)
{
    rAssert( ppPlayer != NULL );
    rAssert( pResource != NULL );

    // Find out where to look
    unsigned int i = 0;
    bool playerFound = false;
    daSoundGroup soundGroup = pResource->GetSoundGroup( );
    IDaSoundResource::Type resourceType = pResource->GetType( );

    switch( pResource->GetType( ) )
    {
    case IDaSoundResource::CLIP:
        {
            // Look in the clip player list
            playerFound = FindFreeClipPlayer( ppPlayer, pResource );
            break;
        }
    case IDaSoundResource::STREAM:
        {
            // Look in the stream player list
            playerFound = FindFreeStreamPlayer( ppPlayer, pResource );
            break;
        }
    default:
        {
            rAssert( 0 );
            break;
        }
    }
    if( playerFound )
    {
        // Capture it
        (*ppPlayer)->Capture( pResource, positional  );
    }
    else
    {
        (*ppPlayer) = NULL;
    }

}

bool daSoundPlayerManager::FindFreePlayer(
    daSoundClipStreamPlayer** ppPlayerArray,
    unsigned int numPlayers,
    daSoundClipStreamPlayer ** ppPlayer )
{
    *ppPlayer = 0;
    
    for( unsigned int c = 0; c < numPlayers; c ++ )
    {
        if ( false == ppPlayerArray[ c ]->IsCaptured( ) )
        {
            *ppPlayer = ppPlayerArray[ c ];
            break;       
        }
    }
    
    return NULL != *ppPlayer;
}


//=============================================================================
// Function:    daSoundPlayerManager::FindFreeClipPlayer
//=============================================================================
// Description: Find a free clip player
//
//-----------------------------------------------------------------------------

bool daSoundPlayerManager::FindFreeClipPlayer(
    daSoundClipStreamPlayer** ppPlayer,
    IDaSoundResource* pResource
)
{
    return FindFreePlayer( gClipPlayerArray, SOUND_NUM_CLIP_PLAYERS, ppPlayer );
}

//=============================================================================
// Function:    daSoundPlayerManager::FindFreeStreamPlayer
//=============================================================================
// Description: Find a free stream player
//
//-----------------------------------------------------------------------------

bool daSoundPlayerManager::FindFreeStreamPlayer(
    daSoundClipStreamPlayer** ppPlayer,
    IDaSoundResource* pResource )
{
    return FindFreePlayer( gStreamPlayerArray, SOUND_NUM_STREAM_PLAYERS, ppPlayer );
}

//=============================================================================
// Function:    daSoundPlayerManager::PausePlayers
//=============================================================================
// Description: Pause the sound players
//
// Parameters:  stackFrame - the stack frame that a player is associated with.
//                           If NULL, all players pause.
//
// Returns:     n/a
//
//-----------------------------------------------------------------------------

void daSoundPlayerManager::PausePlayers
(
)
{
    //
    // Pause all players
    //
    daSoundPlayerBase* pPlayer = daSoundPlayerBase::GetLinkedClassHead( );
    while( pPlayer != NULL )
    {
        pPlayer->Pause( );

        // Move on
        pPlayer = pPlayer->GetLinkedClassNext( );
    }
}

//=============================================================================
// Function:    daSoundPlayerManager::PausePlayersWithFade
//=============================================================================
// Description: Fade out the players and then pause
//
// Parameters:  see PausePlayers
//              pCallback - the asynchronous callback to call when done
//              pUserData - the user data to pass into the callback
//
// Returns:     n/a
//
//-----------------------------------------------------------------------------

void daSoundPlayerManager::PausePlayersWithFade
(
    IDaSoundFadeState* pCallback,
    void* pUserData
)
{
    // Create the fade info
    daSoundAsyncFadeCallback* pFadeInfo = new( GMA_TEMP ) daSoundAsyncFadeCallback( );
    rAssert( pFadeInfo != NULL );
    pFadeInfo->SetPlayerManager( this );
    pFadeInfo->SetAction( OnFade_PausePlayers );
    pFadeInfo->SetCallback( pCallback, pUserData );

    // Start fading the sounds
    rAssert( m_pIngameFadeOut != NULL );
    Sound::daSoundRenderingManagerGet( )->GetTuner( )->FadeSounds
    (
        this,
        pFadeInfo,
        m_pIngameFadeOut,
        false
    );
}

//=============================================================================
// Function:    daSoundPlayerManager::ContinuePlayers
//=============================================================================
// Description: Continue the sound players
//
// Parameters:  stackFrame - the stack frame that a player is associated with.
//                           If NULL, all players continue.
//
// Returns:     n/a
//
//-----------------------------------------------------------------------------

void daSoundPlayerManager::ContinuePlayers
(
)
{
    //
    // Continue all players
    //
    daSoundPlayerBase* pPlayer = daSoundPlayerBase::GetLinkedClassHead( );
    while( pPlayer != NULL )
    {
        if( pPlayer->IsPaused() )
        {
            pPlayer->Continue( );
        }

        // Move on
        pPlayer = pPlayer->GetLinkedClassNext( );
    }
}

//=============================================================================
// Function:    daSoundPlayerManager::ContinuePlayersWithFade
//=============================================================================
// Description: Continue the players and then fade in.
//
// Parameters:  see PausePlayers
//              pCallback - the asynchronous callback to call when done
//              pUserData - the user data to pass into the callback
//
// Returns:     n/a
//
//-----------------------------------------------------------------------------

void daSoundPlayerManager::ContinuePlayersWithFade
(
    IDaSoundFadeState* pCallback,
    void* pUserData
)
{
    // Create the fade info
    daSoundAsyncFadeCallback* pFadeInfo =
        new( GMA_TEMP ) daSoundAsyncFadeCallback( );
    rAssert( pFadeInfo != NULL );
    pFadeInfo->SetPlayerManager( this );
    pFadeInfo->SetAction( OnFade_ContinuePlayers );
    pFadeInfo->SetCallback( pCallback, pUserData );

    // Continue the players
    ContinuePlayers();

    // Start fading the sounds
    rAssert( m_pIngameFadeIn != NULL );
    Sound::daSoundRenderingManagerGet( )->GetTuner( )->FadeSounds
    (
        this,
        pFadeInfo,
        m_pIngameFadeIn,
        true
    );
}

//=============================================================================
// Function:    daSoundPlayerManager::CancelPlayers
//=============================================================================
// Description: Stop the sound players
//
// Parameters:  stackFrame - the stack frame that a player is associated with.
//                           If NULL, all players stop.
//
// Returns:     n/a
//
//-----------------------------------------------------------------------------

void daSoundPlayerManager::CancelPlayers
(
)
{
    //
    // Stop all players
    //
    daSoundPlayerBase* pPlayer = daSoundPlayerBase::GetLinkedClassHead( );
    while( pPlayer != NULL )
    {
        if( pPlayer->IsCaptured( ) )
        {
            // Stop the player
            pPlayer->Stop( );
        }

        // Move on
        pPlayer = pPlayer->GetLinkedClassNext( );
    }

    //
    // Since this command may be called by the sound manager destructor,
    // we must make sure that we service the sound system at least one
    // more time so that we can actually stop all the sounds.
    //
    ::radSoundHalSystemGet( )->Service( );
    ::radSoundHalSystemGet( )->ServiceOncePerFrame( );
}

//=============================================================================
// daSoundPlayerManager::AreAllPlayersStopped
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool daSoundPlayerManager::AreAllPlayersStopped()
{
    unsigned int c;
    daSoundClipStreamPlayer::State playerState;

    for( c = 0; c < SOUND_NUM_CLIP_PLAYERS; c++ )
    {
        if( gClipPlayerArray[c] != NULL )
        {
            playerState = gClipPlayerArray[c]->GetState();

            if( ( playerState == daSoundClipStreamPlayer::State_Cueing )
                || ( playerState == daSoundClipStreamPlayer::State_CuedPlay )
                || ( playerState == daSoundClipStreamPlayer::State_Playing ) )
            {
                if( !(gClipPlayerArray[c]->IsPaused()) )
                {
                    return( false );
                }
            }
        }
    }

    for( c = 0; c < SOUND_NUM_STREAM_PLAYERS; c ++ )
    {
        if( gStreamPlayerArray[c] != NULL )
        {
            playerState = gStreamPlayerArray[c]->GetState();

            if( ( playerState == daSoundClipStreamPlayer::State_Cueing )
                || ( playerState == daSoundClipStreamPlayer::State_CuedPlay )
                || ( playerState == daSoundClipStreamPlayer::State_Playing ) 
                || ( playerState == daSoundClipStreamPlayer::State_Stopping ) )
            {
                if( !(gStreamPlayerArray[c]->IsPaused()) )
                {
                    return( false );
                }
            }
        }
    }

    return( true );
}

//=============================================================================
// Function:    daSoundPlayerManager::PlayerVolumeChange
//=============================================================================
// Description: updates all of the players with the new volume value
//
//-----------------------------------------------------------------------------
void daSoundPlayerManager::PlayerVolumeChange( daSoundGroup groupName, float trim )
{
    daSoundPlayerBase* pPlayer = daSoundPlayerBase::GetLinkedClassHead( );
    while( pPlayer != NULL )
    {
        pPlayer->ChangeTrim(groupName,trim);

        // Move on
        pPlayer = pPlayer->GetLinkedClassNext( );
    }
}

//=============================================================================
// Function:    daSoundPlayerManager::PlayerFaderVolumeChange
//=============================================================================
// Description: updates all of the players with the new fader volume value
//
//-----------------------------------------------------------------------------
void daSoundPlayerManager::PlayerFaderVolumeChange( daSoundGroup groupName, float trim )
{
    daSoundPlayerBase* pPlayer = daSoundPlayerBase::GetLinkedClassHead( );
    while( pPlayer != NULL )
    {
        pPlayer->ChangeFaderTrim(groupName,trim);

        // Move on
        pPlayer = pPlayer->GetLinkedClassNext( );
    }
}

//=============================================================================
// Function:    daSoundPlayerManager::OnFadeDone
//=============================================================================
// Description: React when the fade is done
//
// Parameters:  pUserData - a void* to a daSoundAsyncFadeCallback
//
//-----------------------------------------------------------------------------

void daSoundPlayerManager::OnFadeDone( void* pUserData )
{
    daSoundAsyncFadeCallback* pFadeInfo =
        reinterpret_cast< daSoundAsyncFadeCallback* >( pUserData );
    rAssert( pFadeInfo != NULL );

    // Perform the appropriate action
    switch( pFadeInfo->GetAction( ) )
    {
    case OnFade_PausePlayers:
        {
            PausePlayers();
            break;
        }
    case OnFade_ContinuePlayers:
        {
            break;
        }
    case OnFade_CancelPlayers:
        {
            CancelPlayers();
            Sound::daSoundRenderingManagerGet( )->
                GetTuner( )->
                SetMasterVolume( 1.0f );
            break;
        }
    default:
        {
            rAssert( 0 );
            break;
        }
    }

    // Call the callback
    IDaSoundFadeState* pCallback = NULL;
    void* pData = NULL;
    pFadeInfo->GetCallback( &pCallback, &pData );

    if( pCallback != NULL )
    {
        pCallback->OnFadeDone( pData );
    }

    // Delete the fade info
    delete pFadeInfo;
}

void TrimFileName( char * pS, int len )
{
    int sl = strlen( pS );
    
    char * pStart;
    char * pEnd;
    
    pEnd = pS + sl - 4;
    pStart = pEnd - len;
    
    if ( pStart < pS )
    {
        pStart = pS;
    }
    
    if ( pEnd < pStart )
    {
        pEnd = pS + 1;
    }
    
    int chars = pEnd - pStart;
    
    ::memcpy( pS, pStart, chars);
    pS[ chars ] = 0;
}

void RenderPlayer( daSoundClipStreamPlayer * pPlayer, int row, int col )
{
    char buf[ 256 ];
    
    if ( false == pPlayer->IsCaptured( ) )
    {
        sprintf( buf, "free" );
    }
    else
    {
        float fDistToListener;
        char sDistToListener[ 64 ];
        char sFileName[ 64 ];
        char sMaxDistance[ 64 ];
        
        pPlayer->GetFileName( sFileName, 64 );

        TrimFileName( sFileName, 8 );
        
        IRadSoundHalPositionalGroup * pPosGroup = pPlayer->GetPositionalGroup( );
        
        if ( pPosGroup )
        {
            radSoundVector listenerPos;
            radSoundVector position;
            
            float minDist;
            float maxDist;            
            
            radSoundHalListenerGet( )->GetPosition( & listenerPos );        
            
            pPosGroup->GetPosition( & position );
            pPosGroup->GetMinMaxDistance( & minDist, & maxDist );
            
            fDistToListener = listenerPos.GetDistanceBetween( position );
            
            sprintf( sDistToListener, "%.2f", fDistToListener );
            sprintf( sMaxDistance, "%.2f", maxDist );
        }
        else
        {
            strcpy( sDistToListener, "--" );
            strcpy( sMaxDistance, "--" );
        }
        
        
        // gClipPlayerArray[ c ]->
        sprintf( buf, "[%s](%s)[%d][%s][%s]",
            sFileName,
            pPlayer->IsPaused( ) ? "-" : "*",
            pPlayer->GetState( ),
            sDistToListener,
            sMaxDistance );
    }
    
    p3d::pddi->DrawString( buf, 40 + col * 320
    , 36 + row * 16, pddiColour( 255, 255, 0 ) ); 

}

void daSoundPlayerManager::Render( void )
{
    if( m_Initialized )
    {
        int col = 0;
        int row = 0;

        unsigned int freeMem;
        unsigned int numObjects;
        unsigned int largestBlock;
        unsigned int size;
        
        radSoundHalSystemGet( )->GetRootMemoryRegion( )->GetStats(
            & freeMem,
            & numObjects,
            & largestBlock,
            true );
            
        size = radSoundHalSystemGet( )->GetRootMemoryRegion( )->GetSize( );
        
        char memStr[ 256 ];
        sprintf(
            memStr,
            "Usd %dK Fre %dK Lrg %dK Objs: %d",
            ( size - freeMem ) / 1024,
            freeMem / 1024,
            largestBlock / 1024,
            numObjects );
        
        p3d::pddi->DrawString( memStr, 40 + col * 320, 36 + row * 16, pddiColour( 255, 255, 0 ) );

        row++;
                
        unsigned int usedBTreeNodes = radObjectBTree::GetNumAllocatedNodes( );
        unsigned int nodeSize = sizeof( radObjectBTreeNode );
        
        sprintf( memStr,
            "BTree Nodes: [0x%x], size: [0x%x]",
            usedBTreeNodes,
            nodeSize );

        p3d::pddi->DrawString( memStr, 40 + col * 320, 36 + row * 16, pddiColour( 255, 255, 0 ) );
                    
        row++;
            
        char listenerStr[ 128 ];
        radSoundVector lp;
        radSoundVector lv;
        radSoundHalListenerGet( )->GetPosition( & lp );
        radSoundHalListenerGet( )->GetVelocity( & lv );
        
        sprintf( listenerStr, "Pos:[%.2f][%.2f][%.2f] Vel:[%.2f][%.2f][%.2f]\n",
            lp.m_x, lp.m_y, lp.m_z, lv.m_x, lv.m_y, lv.m_z );
            
        p3d::pddi->DrawString( listenerStr, 40 + col * 320, 36 + row * 16, pddiColour( 255, 255, 0 ) );

        row++;
        
        for( unsigned int c = 0; c < SOUND_NUM_CLIP_PLAYERS / 2; c ++ )
        {
            RenderPlayer( gClipPlayerArray[ c ], row, col );
                         
            col++;
            
            if ( col >= 2 )
            {
                row++;
                col = 0;
            }         
        }
        
        row++;
        
        for( unsigned int c = 0; c < SOUND_NUM_STREAM_PLAYERS; c ++ )
        {
            RenderPlayer( gStreamPlayerArray[ c ], row, col );
                         
            col++;
            
            if ( col >= 2 )
            {
                row++;
                col = 0;
            }         
        }        
    }        
}

} // Sound Namespace

