//=============================================================================
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// File:        dasoundplayer.cpp
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

#include <radobject.hpp>
#include <raddebug.hpp>
#include <radlinkedclass.hpp>

#include <radsound.hpp>
#include <radsoundmath.hpp>

#include <sound/soundrenderer/soundsystem.h>

#include <sound/soundrenderer/soundrenderingmanager.h>
#include <sound/soundrenderer/idasoundresource.h>
#include <sound/soundrenderer/soundresourcemanager.h>
#include <sound/soundrenderer/soundplayer.h>
#include <sound/soundrenderer/idasoundtuner.h>
#include <sound/soundrenderer/soundnucleus.hpp>

#include <main/commandlineoptions.h>

//=============================================================================
// Static Variables (outside namespace)
//=============================================================================


//
// Initialially the player list is empty
//
Sound::daSoundPlayerBase* radLinkedClass< Sound::daSoundPlayerBase >::s_pLinkedClassHead = NULL;
Sound::daSoundPlayerBase* radLinkedClass< Sound::daSoundPlayerBase >::s_pLinkedClassTail = NULL;
Sound::daSoundClipStreamPlayer* radLinkedClass< Sound::daSoundClipStreamPlayer >::s_pLinkedClassHead = NULL;
Sound::daSoundClipStreamPlayer* radLinkedClass< Sound::daSoundClipStreamPlayer >::s_pLinkedClassTail = NULL;

//=============================================================================
// Namespace
//=============================================================================

namespace Sound {

//=============================================================================
// Debug Information
//=============================================================================

//
// Use this if you want to debug the sound player
//
#ifndef FINAL
#ifndef NDEBUG
#define DASOUNDPLAYER_DEBUG
#ifdef DASOUNDPLAYER_DEBUG

// Show sound state changes
static bool sg_ShowSoundStates = false;

#endif //DASOUNDPLAYER_DEBUG
#endif //NDEBUG
#endif //FINAL

//=============================================================================
// Definitions Macros and Constants
//=============================================================================

//
// These are the minimum ranges that the given variables must range
// over to warrant a randomization calculation
//
const float DASOUND_MINFLOATVARYINGRANGE = 0.0001f;

//=============================================================================
// Local functions
//=============================================================================

//=============================================================================
// Class Implementations
//=============================================================================

//=============================================================================
// daSoundClipStreamPlayer Implementation
//=============================================================================

//=============================================================================
// Function:    daSoundClipStreamPlayer::daSoundClipStreamPlayer
//=============================================================================
// Description: Constructor
//
//-----------------------------------------------------------------------------

daSoundClipStreamPlayer::daSoundClipStreamPlayer( )
    :
    m_Trim( 1.0f ),
    m_GroupTrim( 1.0f ),
    m_FaderGroupTrim( 1.0f ),
    m_MasterTrim( 1.0f ),
    m_State( State_DeCued ),
    m_CueingState( CueingState_Null ),
    m_CaptureCount( 0 ),
    m_PauseCount( 0 ),
    m_pAllocatedResource( NULL ),
    m_AllocResInstanceID( 0 ),
    m_pResource( NULL ),
    m_pStateChangeCallback( NULL ),
    m_pStateChangeCallbackUserData( NULL ),
    m_Type( IDaSoundResource::UNKNOWN )
{
    m_IsPositional = false;
    m_pPositionalGroup = radSoundHalPositionalGroupCreate( GetThisAllocator( ) );
    m_pPositionalGroup->AddRef( );
    
    m_CurrentTrim  = 0.0f;
    m_StoppingTrim = 1.0f;
    m_VaryingTrim = 1.0f;
    
    m_Pitch = 1.0f;
    m_VaryingPitch = 1.0f;
    m_CurrentPitch = 1.0f;    
}

//=============================================================================
// Function:    daSoundClipStreamPlayer::~daSoundClipStreamPlayer
//=============================================================================
// Description: Destructor
//
//-----------------------------------------------------------------------------

daSoundClipStreamPlayer::~daSoundClipStreamPlayer( )
{   
    if ( IDaSoundResource::CLIP == m_Type )
    {
        m_ClipInfo.m_pClipPlayer->Stop( );
        m_ClipInfo.m_pClipPlayer->SetClip( NULL );
        m_ClipInfo.m_pClipPlayer->Release( );
    }
    else
    {
        if ( m_StreamInfo.m_pResources != NULL )
        {
            Sound::SoundNucleusUnCaptureStreamerResources( m_StreamInfo.m_pResources );
            m_StreamInfo.m_pResources->m_pStitchedDataSource->SetStitchCallback( NULL, NULL );
        }
        
        if ( m_StreamInfo.m_pRsdFileDataSource != NULL )
        {
            m_StreamInfo.m_pRsdFileDataSource->Release( );
        }
    }

    if ( m_pResource != NULL )
    {
        m_pResource->Release( );
    }
    
    if ( m_pPositionalGroup )
    {                   
        m_pPositionalGroup->Release( );
    }

    if ( m_pAllocatedResource )
    {
        m_pAllocatedResource->Release( );
    }

    if ( m_pResource != NULL )
    {
        m_pResource->Release( );
    }
}

void daSoundClipStreamPlayer::UpdateClip( void )
{
    m_ClipInfo.m_pClipPlayer->SetTrim( m_CurrentTrim );
    m_ClipInfo.m_pClipPlayer->SetPitch( m_CurrentPitch );
    
    IRadSoundClipPlayer * pCp = m_ClipInfo.m_pClipPlayer;
    IRadSoundClipPlayer::State state = pCp->GetState( );
    
    switch( m_State )
    {
        case State_DeCued:
        {
            rAssert( IRadSoundClipPlayer::NoClip == state );
            break;
        }
        case State_CuedPlay:
        case State_Cueing:
        {
            switch( m_CueingState )
            {
                case CueingState_Null:
                {
                    rAssert( false );
                    break;
                }
                case CueingState_Resource:
                {
                    rAssert( IRadSoundClipPlayer::NoClip == state );  
                                  
                    daSoundFileInstance* pFileInstance = m_pAllocatedResource->GetFileInstance( m_AllocResInstanceID );
                    if ( pFileInstance->GetState( ) == daSoundFileInstance::Loaded )
                    {
                        HookUpAndCuePlayer( );
                    }
              
                    break;
                }
                case CueingState_Player:
                {
                    m_CueingState = CueingState_Cued;
                    break;
                }
                case CueingState_Cued:
                {
                    rAssert( IRadSoundClipPlayer::Stopped == state );
                                      
                    if ( State_CuedPlay == m_State )
                    {
                        if( 0 == m_PauseCount )
                        {
                            pCp->Play( );
                        }
                        
                        m_CaptureCount++;                        
                        m_State = State_Playing;
                    }
                    else
                    {
                        m_State = State_Cued;
                    }
                
                    if ( m_pStateChangeCallback )
                    {
                        m_pStateChangeCallback->OnSoundReady( m_pStateChangeCallbackUserData );
                    }
                                            
                    m_CueingState = CueingState_Null;
                    
                    break;
                }
            }
            break;
        }
        case State_Cued:
        {
            switch( state )
            {
                case IRadSoundClipPlayer::Stopped:
                    break;
                case IRadSoundClipPlayer::NoClip:
                case IRadSoundClipPlayer::Queueing:
                case IRadSoundClipPlayer::QueuedPlay:
                case IRadSoundClipPlayer::Playing:
                default:
                    rAssert( false );          
            }
            break;
        }
        case State_Playing:
        {
            if ( m_StoppingTrim <= 1.0f )
            {
                m_StoppingTrim += radSoundVolumeChangeThreshold;
                
                if ( m_StoppingTrim >= 1.0f )
                {
                    m_StoppingTrim = 1.0f;
                }
            }
            switch( state )
            {
                case IRadSoundClipPlayer::NoClip:
                case IRadSoundClipPlayer::Queueing:
                case IRadSoundClipPlayer::QueuedPlay:
                    rAssert( false );
                    break;
                case IRadSoundClipPlayer::Playing:
                    rAssert( 0 == m_PauseCount );
                    break;
                case IRadSoundClipPlayer::Stopped:
                    if( m_PauseCount == 0 )
                    {
                        m_State = State_Done;
                        UnCapture( );
                        
                        if ( m_pStateChangeCallback )
                        {
                            m_pStateChangeCallback->OnSoundDone( m_pStateChangeCallbackUserData );
                        }
                    }
                    break;                   
                default:
                    break;           
            }
            break;
        }
      case State_Stopping:
      {
            if ( m_CurrentTrim <= 0.0f )
            {
                m_ClipInfo.m_pClipPlayer->Stop( );
                m_State = State_Cued;
                m_StoppingTrim = 1.0f;
                UnCapture( );                      
            }
            else
            {
                m_StoppingTrim -= radSoundVolumeChangeThreshold;
                
                if ( m_StoppingTrim <= 0.0f )
                {
                    m_StoppingTrim = 0.0f;
                }
            }            

            break;
        }        
  
        case State_Done:
        {
            rAssert( IRadSoundClipPlayer::NoClip == state );
            break;
        }
        default:
        {
            rAssert( false );
        }
    }
}

void daSoundClipStreamPlayer::UpdateStream( void )
{
    IRadSoundStreamPlayer::State state;
    
    if ( NULL != m_StreamInfo.m_pResources )
    {
        m_StreamInfo.m_pResources->m_pStreamPlayer->SetTrim( m_CurrentTrim );
        m_StreamInfo.m_pResources->m_pStreamPlayer->SetPitch( m_CurrentPitch );
        state = m_StreamInfo.m_pResources->m_pStreamPlayer->GetState( );
    }
    else
    {
        state = IRadSoundStreamPlayer::NoSource;    
    }
    
    switch( m_State )
    {
        case State_DeCued:
        {
            rAssert( IRadSoundStreamPlayer::NoSource == state );
            break;
        }
        case State_CuedPlay:        
        case State_Cueing:
        {
            switch( m_CueingState )
            {
                case CueingState_Resource:
                {
                    daSoundFileInstance* pFileInstance = m_pAllocatedResource->GetFileInstance( m_AllocResInstanceID );
                    
                    if ( pFileInstance->GetState( ) == daSoundFileInstance::Loaded )
                    {
                        if ( NULL == m_StreamInfo.m_pRsdFileDataSource )
                        {
                            m_pAllocatedResource->GetFileInstance( m_AllocResInstanceID )->CreateFileDataSource( & m_StreamInfo.m_pRsdFileDataSource );
                        }
                        else if ( IRadSoundRsdFileDataSource::Initialized == m_StreamInfo.m_pRsdFileDataSource->GetState( ) )
                        {
                            /* rDebugPrintf( "DASOUND: Playing Streamed File: [%s], format: [%d], channels: [%d]\n",
                                pFileInstance->GetFileName( ),
                                m_StreamInfo.m_pRsdFileDataSource->GetFormat( )->GetEncoding( ),
                                m_StreamInfo.m_pRsdFileDataSource->GetFormat( )->GetNumberOfChannels( ) ); */
                                
                            HookUpAndCuePlayer( );
                        }
                    }
                    
                    break;              
                }
                case CueingState_Player:
                {
                    switch( state )
                    {
                        case IRadSoundStreamPlayer::Queueing:
                        {
                            break;
                        }
                        case IRadSoundStreamPlayer::Paused:
                        {

                            if ( m_pStateChangeCallback )
                            {
                                m_pStateChangeCallback->OnSoundReady( m_pStateChangeCallbackUserData );
                            }      
                                              
                            m_CueingState = CueingState_Cued;
                            break;
                        }
                        case IRadSoundStreamPlayer::NoSource:
                        case IRadSoundStreamPlayer::Playing:
                        case IRadSoundStreamPlayer::QueuedPlay:                        
                        default:
                            rAssert( false );                    
                    }
                    
                    break;
                }
                case CueingState_Cued:
                {
                    if ( State_Cueing == m_State )
                    {
                        m_State = State_Cued;
                    }
                    else
                    {
                        if ( 0 == m_PauseCount )
                        {
                            m_StreamInfo.m_pResources->m_pStreamPlayer->Play( );
                        }
                        m_CaptureCount++;
                        m_State = State_Playing;
                    }
                    
                    m_CueingState = CueingState_Null;                
                    break;
                }
                default:
                {
                    rAssert( false );
                }
            }
            break;
        }
        case State_Cued:
        {
            rAssert( IRadSoundStreamPlayer::Paused == state );
            break;
        }
        case State_Playing:
        {
            m_StoppingTrim += radSoundVolumeChangeThreshold;
            
            if ( m_StoppingTrim >= 1.0f )
            {
                m_StoppingTrim = 1.0f;
            }
                        
            switch( state )
            {
                case IRadSoundStreamPlayer::Queueing:
                    rAssert(false);
                    break;
                case IRadSoundStreamPlayer::Paused:
                    rAssert( 0 < m_PauseCount );
                    break;
                case IRadSoundStreamPlayer::NoSource:
                    m_State = State_Done;
                    UnCapture( ); // Internal
                    if ( m_pStateChangeCallback )
                    {
                        m_pStateChangeCallback->OnSoundDone( m_pStateChangeCallbackUserData );
                    }                    
                    break;
                case IRadSoundStreamPlayer::Playing:
                    rAssert( 0 == m_PauseCount );
                    break;
                case IRadSoundStreamPlayer::QueuedPlay:   
                default:
                    rAssert( false );                    
            }
            
            break;
          
        }
        case State_Stopping:
        {
            if ( m_CurrentTrim <= 0.0f )
            {
                m_StreamInfo.m_pResources->m_pStreamPlayer->Stop( );
                m_State = State_Cued;
                m_StoppingTrim = 1.0f;
                UnCapture( );                      
            }
            else
            {
                m_StoppingTrim -= radSoundVolumeChangeThreshold;
                
                if ( m_StoppingTrim <= 0.0f )
                {
                    m_StoppingTrim = 0.0f;
                }
            }
   
            break;
        }
        case State_Done:
        {
            rAssert( IRadSoundStreamPlayer::NoSource == state );
            break;
        }
        default:
        {
            rAssert( false );
        }     
    }
    
}

void daSoundClipStreamPlayer::ServiceOncePerFrame( void )
{
    //
    // Update the state information.  This now requires polling the player.
    //
    
    State prevState;
    CueingState prevCueingState;
    
    do
    {
    
        CalculateCurrentTrim( );
        CalculateCurrentPitch( );
        
        prevState = m_State;
        prevCueingState = m_CueingState;
        
        switch( m_Type )
        {
            case IDaSoundResource::CLIP:
                UpdateClip( );
                break;
            case IDaSoundResource::STREAM:
                UpdateStream( );
                break;
            default:
                rAssert( 0 );
                break;
        }
    }
    while( prevState != m_State || prevCueingState != m_CueingState );       
}

void daSoundClipStreamPlayer::InitializeAsClipPlayer( void )
{
    m_Type = IDaSoundResource::CLIP;
    
    m_ClipInfo.m_pClipPlayer = radSoundClipPlayerCreate( GetThisAllocator( ) );
    m_ClipInfo.m_pClipPlayer->AddRef( );

#ifndef RAD_WIN32
    // Prepare the player to listen for spacial effects
    for( unsigned int i = 0; i < ::radSoundHalSystemGet( )->GetNumAuxSends( ); i++ )
    {
        m_ClipInfo.m_pClipPlayer->SetAuxMode( i, radSoundAuxMode_PostFader );
        m_ClipInfo.m_pClipPlayer->SetAuxGain( i, 1.0f );
    }
#endif
}

void daSoundClipStreamPlayer::InitializeAsStreamPlayer( void )
{
    m_Type = IDaSoundResource::STREAM;
    
    m_StreamInfo.m_pResources = NULL;
    m_StreamInfo.m_pRsdFileDataSource = NULL;    
}

//=============================================================================
// Function:    daSoundClipStreamPlayer::CapturePlayer
//=============================================================================
// Description: Capture the sound player
//
//-----------------------------------------------------------------------------

void daSoundClipStreamPlayer::Capture(
    IDaSoundResource* pResource,
    bool isPositional )
{
    rAssert( pResource != NULL );
    rAssert( State_DeCued == m_State );
    
    m_IsPositional = isPositional;
    
    ++m_CaptureCount;
    
    // A bit of a hack, but the resource should be connected only at this time
    rAssert( m_CaptureCount == 1 );

    rAssert( m_pAllocatedResource == NULL );
    rAssert( m_pResource == NULL );

    m_pResource = pResource;
    m_pResource->CaptureResource( );

    //
    // Get an allocated resource
    //
    m_pAllocatedResource = daSoundResourceManager::GetInstance( )->FindAllocatedResource( m_pResource );

    rAssertMsg( m_pAllocatedResource != NULL, "Resource not properly allocated" );
    
    m_pAllocatedResource->AddRef( );

    // Choose an instance to play...
    m_AllocResInstanceID = m_pAllocatedResource->ChooseNextInstance( );

    //
    // Check the type of resource
    //

    rAssert( m_Type == m_pResource->GetType( ) );
    
    daSoundFileInstance* pFileInstance = m_pAllocatedResource->GetFileInstance( m_AllocResInstanceID );
    rAssert( pFileInstance != NULL );
    
    if( pFileInstance->GetType( ) == IDaSoundResource::UNKNOWN )
    {
        rAssert( false); // when does this happen ? -Th
        m_State = State_DeCued;
    } 
    else
    {
        //
        // Start initializing
        //
        m_State = State_Cueing;
        m_CueingState = CueingState_Resource;
    }
    
    // Reset state.
    
    m_Pitch = 1.0f;
    m_Trim = 1.0f;
    
    CalculateNewVaryingPitch( );
    CalculateNewVaryingTrim( );
}

//=============================================================================
// Function:    daSoundClipStreamPlayer::IsCaptured
//=============================================================================
// Description: Returns true if the player is already captured
//
//-----------------------------------------------------------------------------

bool daSoundClipStreamPlayer::IsCaptured( void )
{
    return( m_CaptureCount > 0 );
}

//=============================================================================
// Function:    daSoundClipStreamPlayer::ReleasePlayer
//=============================================================================
// Description: Releases a player.  Automattically detatches any
//              resources when it is no longer captured.
//
//-----------------------------------------------------------------------------

void daSoundClipStreamPlayer::UnCapture( void )
{
    unsigned int i = 0;

    //
    // I don't think this assertion is valid anymore.  If a player is cued but not
    // playing, then Stop() will release the resource, but the player isn't
    // captured yet and this call shouldn't do anything -- DE
    //
    //rAssert( IsCaptured( ) );
    if( m_CaptureCount > 0 )
    {
        --m_CaptureCount;
        
        if( 0 == m_CaptureCount )
        {
            rAssert( NULL == m_pStateChangeCallback );            
            rAssert( m_pResource != NULL );
            rAssert( m_pAllocatedResource->GetResource( ) == m_pResource );
            rAssert( m_Type == m_pResource->GetType( ) );

            // Detatch the real player
            switch( m_Type )
            {
                case IDaSoundResource::CLIP:
                {
                    rAssert( m_ClipInfo.m_pClipPlayer != NULL );

                    // Detach the clip
                    m_ClipInfo.m_pClipPlayer->Stop( );
                    m_ClipInfo.m_pClipPlayer->SetClip( NULL );

                    break;
                }
                case IDaSoundResource::STREAM:
                {
                    if ( m_StreamInfo.m_pResources != NULL )
                    {
                        m_StreamInfo.m_pResources->m_pStitchedDataSource->SetStitchCallback( NULL, NULL );
                        
                        // Detach the stream
                        m_StreamInfo.m_pResources->m_pStreamPlayer->Stop( );
                        m_StreamInfo.m_pResources->m_pStreamPlayer->SetDataSource( NULL );
                        
                        if ( m_StreamInfo.m_pResources->m_pBufferedDataSource != NULL )
                        {
                            m_StreamInfo.m_pResources->m_pBufferedDataSource->SetInputDataSource( NULL );
                        }
                        
                        SoundNucleusUnCaptureStreamerResources( m_StreamInfo.m_pResources );
                        m_StreamInfo.m_pResources = NULL;
                    }
                
                    if ( m_StreamInfo.m_pRsdFileDataSource != NULL )
                    {
                        m_StreamInfo.m_pRsdFileDataSource->Release( );
                        m_StreamInfo.m_pRsdFileDataSource = NULL;
                    }                    

                    break;
                }
                default:
                    rAssert( 0 );
                    break;
            }
        
            // Release the allocated resource
            m_pAllocatedResource->Release( );
            m_pAllocatedResource = NULL;
            m_pResource->ReleaseResource( );
            m_pResource = NULL;
                        
            m_State = State_DeCued;
            m_CueingState = CueingState_Null;
        }                      
    }
}

//=============================================================================
// daSoundClipStreamPlayer::ChangeTrim
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
void daSoundClipStreamPlayer::ChangeTrim( daSoundGroup groupName, float newTrim )
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
// daSoundClipStreamPlayer::ChangeFaderTrim
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
void daSoundClipStreamPlayer::ChangeFaderTrim( daSoundGroup groupName, float newTrim )
{
    daSoundGroup myGroup = GetSoundGroup();

    //
    // ChangeTrim should be used for master volume settings
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
// Function:    daSoundClipStreamPlayer::GetSoundGroup
//=============================================================================
// Description: Get the sound group that this player belongs to.  This is
//              the sound group that its currently active sound is a part of.
//
// Returns:     Returns the appropriate sound group.
//
//-----------------------------------------------------------------------------

daSoundGroup daSoundClipStreamPlayer::GetSoundGroup( void )
{
    daSoundGroup soundGroup = Sound::MASTER;
    if( m_pAllocatedResource != NULL )
    {
        rAssert( m_pAllocatedResource->GetResource( ) != NULL );
        soundGroup = m_pAllocatedResource->GetResource( )->GetSoundGroup( );
    }
    return soundGroup;
}

//=============================================================================
// Function:    daSoundClipStreamPlayer::RegisterSoundPlayerStateCallback
//=============================================================================
// Description: Register a sound player state callback
//
// Notes:       Currently, only one callback is supported.
//
//-----------------------------------------------------------------------------

void daSoundClipStreamPlayer::RegisterSoundPlayerStateCallback
(
    IDaSoundPlayerState* pCallback,
    void* pUserData
)
{
    rAssertMsg( m_pStateChangeCallback == NULL, "Currently, only one callback is allowed" );
    m_pStateChangeCallback = pCallback;
    m_pStateChangeCallbackUserData = pUserData;
    if( m_pStateChangeCallback != NULL )
    {
        m_pStateChangeCallback->AddRef( );
    }
}

//=============================================================================
// Function:    daSoundClipStreamPlayer::UnregisterSoundPlayerStateCallback
//=============================================================================
// Description: Unregister a sound player state callback
//
// Notes:       Currently, only one callback is supported.
//
//-----------------------------------------------------------------------------

void daSoundClipStreamPlayer::UnregisterSoundPlayerStateCallback
(
    IDaSoundPlayerState* pCallback,
    void* pUserData
)
{
    rAssert( pCallback == m_pStateChangeCallback );
    if( m_pStateChangeCallback != NULL )
    {
        m_pStateChangeCallback->Release( );
        m_pStateChangeCallback = NULL;
        m_pStateChangeCallbackUserData = NULL;
    }
}

//=============================================================================
// Function:    daSoundClipStreamPlayer::GetPlaybackTimeInSamples
//=============================================================================
// Description: Get the samples that have elapsed since playback started.
//
//-----------------------------------------------------------------------------

unsigned int daSoundClipStreamPlayer::GetPlaybackTimeInSamples( void )
{
    // Is this synchronized with play / pause?
    unsigned int elapsedtime = 0;
    if( IsCaptured( ) )
    {
        // Update the positional information
        IRadSoundPlayer* pPlayer = NULL;
        switch( m_Type )
        {
        case IDaSoundResource::CLIP:
            {
                pPlayer = static_cast< IRadSoundPlayer* >
                (
                    m_ClipInfo.m_pClipPlayer
                );
                break;
            }
        case IDaSoundResource::STREAM:
            {
                pPlayer = static_cast< IRadSoundPlayer* >
                (
                    m_StreamInfo.m_pResources->m_pStreamPlayer
                );
                break;
            }
        default:
            rAssert( 0 );
            break;
        }
        if( pPlayer != NULL )
        {
            elapsedtime = pPlayer->GetPlaybackTimeInSamples( );
            //elapsedtime = ::radSoundHalSystemGet( )->GetReferenceClock( );
        }
    }

    // Return the time
    return( elapsedtime );
}

//=============================================================================
// Function:    daSoundClipStreamPlayer::Play
//=============================================================================
// Description: Play the active sound resource.
//
//-----------------------------------------------------------------------------
        
void daSoundClipStreamPlayer::Play( void )
{    
    daSoundFileInstance* pFileInstance = m_pAllocatedResource->GetFileInstance( m_AllocResInstanceID );
    rAssert( pFileInstance != NULL );
    
    #ifndef RAD_RELEASE    
        if ( daSoundFileInstance::Loaded != pFileInstance->GetState( ) )
        {
            char fileName[ 256 ];
            pFileInstance->GetFileName( fileName, 256 );      
            
            rTunePrintf(
                "\nAUDIO: Controlling code is not paying attention to latency: [%s]\n\n", fileName );
        }
    #endif        
    
    switch ( m_State )
    {
        case State_Cueing:
        {
            m_State = State_CuedPlay;
            break;
        }
        case State_Cued:
        {
            m_StoppingTrim = 1.0f;
            
            // fall through
        }
        case State_Stopping:        
        {
            m_State = State_CuedPlay;
            m_CueingState = CueingState_Cued;
            break;
        }
        default:
        {
            rAssert( false );
            break;
        }
    }
}

    
void daSoundClipStreamPlayer::OnStitch( IRadSoundHalDataSource ** ppHds , unsigned int frameCount, void * pUserData )
{    
    bool loop = m_pAllocatedResource->GetResource( )->GetLooping( );
    
    if ( loop || NULL != m_StreamInfo.m_pRsdFileDataSource )
    {
        if ( NULL != m_StreamInfo.m_pRsdFileDataSource )
        {
            // pass on ref count
            
            *ppHds = m_StreamInfo.m_pRsdFileDataSource;
            m_StreamInfo.m_pRsdFileDataSource = NULL;
        }
        else
        {            
            ref< IRadSoundRsdFileDataSource > refFds;
            m_pAllocatedResource->GetFileInstance( m_AllocResInstanceID )->CreateFileDataSource( & refFds );
        
             *ppHds = refFds;
            (*ppHds)->AddRef( );
        }
        //rDebugPrintf( "DASOUND: Player: [0x%x] Stitching: [%s]\n", this, refFds->GetName( ) );
    }
}
    
//=============================================================================
// Function:    daSoundClipStreamPlayer::Pause
//=============================================================================
// Description: Pause the active sound resource.
//
//-----------------------------------------------------------------------------

void daSoundClipStreamPlayer::Pause( void )
{
    m_PauseCount++;
    
    if ( State_Playing == m_State )
    {
        switch( m_Type )
        {
            case IDaSoundResource::CLIP:
            {
                m_ClipInfo.m_pClipPlayer->Stop( );
                break;
            }
            case IDaSoundResource::STREAM:
            {
                m_StreamInfo.m_pResources->m_pStreamPlayer->Stop( );
                break;
            }
            default:
            {
                rAssert( false );
            }        
        }
    }
}

//=============================================================================
// Function:    daSoundClipStreamPlayer::Continue
//=============================================================================
// Description: Continue the active sound resource.
//
//-----------------------------------------------------------------------------

void daSoundClipStreamPlayer::Continue( void )
{
    rAssert( m_PauseCount > 0 );
    
    m_PauseCount--;
    
    if ( 0 == m_PauseCount )
    {
        if ( State_Playing == m_State )
        {
            switch( m_Type )
            {
                case IDaSoundResource::CLIP:
                {
                    m_ClipInfo.m_pClipPlayer->Play( );
                    break;
                }
                case IDaSoundResource::STREAM:
                {
                    m_StreamInfo.m_pResources->m_pStreamPlayer->Play( );
                    break;
                }
                default:
                {
                    rAssert( false );
                }
            }        
        }
    }
}

//=============================================================================
// Function:    daSoundClipStreamPlayer::UberContinue
//=============================================================================
// Description: Continue the active sound resource, no matter how many times
//              it was previously paused
//
//-----------------------------------------------------------------------------

void daSoundClipStreamPlayer::UberContinue( void )
{
    if( IsPaused() )
    {
        if( m_PauseCount > 1 )
        {
            m_PauseCount = 1;
        }
        Continue();
    }
}

//=============================================================================
// Function:    daSoundClipStreamPlayer::Stop
//=============================================================================
// Description: Stop the active sound resource.
//
//-----------------------------------------------------------------------------

void daSoundClipStreamPlayer::Stop( void )
{
    if ( State_CuedPlay == m_State )
    {
        m_State = State_Cueing;
    }
    else if ( State_Playing == m_State )
    {
        m_State = State_Stopping;
    }
}

void daSoundClipStreamPlayer::HookUpAndCuePlayer( void )
{
    rAssert( State_Cueing == m_State || State_CuedPlay == m_State );
    rAssert( CueingState_Resource == m_CueingState );

    daSoundFileInstance* pFileInstance = m_pAllocatedResource->GetFileInstance( m_AllocResInstanceID );
    rAssert( pFileInstance != NULL );
    
    //
    // Initialize the group trims
    //
    daSoundGroup myGroup = m_pResource->GetSoundGroup();
    SetMasterTrim( daSoundRenderingManagerGet()->GetTuner()->GetMasterVolume() );
    SetGroupTrim( daSoundRenderingManagerGet()->GetTuner()->GetGroupTrim( myGroup ) );
    SetFaderGroupTrim( daSoundRenderingManagerGet()->GetTuner()->GetFaderGroupTrim( myGroup ) );
    
    switch( m_Type )
    {
        case IDaSoundResource::CLIP:
        {
            IRadSoundClip * pClip = pFileInstance->GetSoundClip( );
            rAssert( NULL != pClip );
            
            // Attach the clip to the player
            m_ClipInfo.m_pClipPlayer->SetClip( pClip );
            m_ClipInfo.m_pClipPlayer->SetPositionalGroup( m_IsPositional ? m_pPositionalGroup : NULL );
            
            break;
        }
        
        case IDaSoundResource::STREAM:
        {
            m_StreamInfo.m_pResources = SoundNucleusCaptureStreamerResources(
                m_StreamInfo.m_pRsdFileDataSource->GetFormat( ) );
            
            rAssert( m_StreamInfo.m_pResources != NULL ); 

            m_StreamInfo.m_pResources->m_pStreamPlayer->SetPositionalGroup( m_IsPositional ? m_pPositionalGroup : NULL );
                                  
            m_StreamInfo.m_pResources->m_pStitchedDataSource->SetStitchCallback( this, NULL );
            m_StreamInfo.m_pResources->m_pStitchedDataSource->ResetAudioFormat( m_StreamInfo.m_pRsdFileDataSource->GetFormat( ) ); // GCN ADPCM HACK
            m_StreamInfo.m_pResources->m_pStitchedDataSource->Reset( );
                                                                        
            if ( NULL != m_StreamInfo.m_pResources->m_pBufferedDataSource  )
            {
                m_StreamInfo.m_pResources->m_pBufferedDataSource->SetInputDataSource( m_StreamInfo.m_pResources->m_pStitchedDataSource );
                m_StreamInfo.m_pResources->m_pStreamPlayer->SetDataSource( m_StreamInfo.m_pResources->m_pBufferedDataSource );
            }
            else
            {                                                            
                m_StreamInfo.m_pResources->m_pStreamPlayer->SetDataSource(
                    m_StreamInfo.m_pResources->m_pStitchedDataSource );
            }


            break;
        }
    default:
        rAssert( 0 );
        break;
    }
    
    m_CueingState = CueingState_Player;
}

const void daSoundClipStreamPlayer::GetFileName( char * pBuf, unsigned int max )
{
    rAssert( m_pAllocatedResource );
    
    daSoundFileInstance* pFileInstance = m_pAllocatedResource->GetFileInstance( m_AllocResInstanceID );
    
    return pFileInstance->GetFileName( pBuf, max );
    
}

} // Sound Namespace
    