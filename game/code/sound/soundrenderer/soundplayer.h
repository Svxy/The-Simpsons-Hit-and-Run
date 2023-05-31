//=============================================================================
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// File:        dasoundplayer.hpp
//
// Subsystem:   Dark Angel - Sound players
//
// Description: Defines the a Dark Angel sound player
//
// Revisions:
//  + Created October 16, 2001 -- breimer
//
//=============================================================================

#ifndef _DASOUNDPLAYER_HPP
#define _DASOUNDPLAYER_HPP

//=============================================================================
// Included Files
//=============================================================================

#include <radobject.hpp>
#include <radlinkedclass.hpp>

#include <sound/soundrenderer/idasoundresource.h>
#include <sound/soundrenderer/soundsystem.h>
#include <sound/soundrenderer/soundallocatedresource.h>
#include <radsound.hpp>

//=============================================================================
// Forward declarations
//=============================================================================

struct IRadObjectBTree;

//=============================================================================
// Define Owning Namespace
//=============================================================================

namespace Sound {

//=============================================================================
// Prototypes
//=============================================================================

class daSoundClipStreamPlayer;
class daSoundTuner;
struct IDaSoundPlayerState;
struct StreamerResources;

//=============================================================================
// Forward declarations
//=============================================================================

class daSoundAllocatedResource;

//=============================================================================
// Typdefs and enumerations
//=============================================================================

const float radSoundPitchChangeThreshold = 0.1f;

//=============================================================================
// Class Declarations
//=============================================================================

class daSoundPlayerBase :
    public IRefCount, 
    public radLinkedClass< daSoundPlayerBase >,
    public radRefCount
{
    public:
    
        inline daSoundPlayerBase( ) : radRefCount( 0 ) { }
        virtual ~daSoundPlayerBase( ) { }
    
        IMPLEMENT_REFCOUNTED( "daSoundPlayerBase" );
        
        virtual void ServiceOncePerFrame ( void ) = 0;
        virtual bool IsCaptured ( void ) = 0;
        virtual void Pause ( void ) = 0;
        virtual void Continue ( void ) = 0;
        virtual void UberContinue( void ) = 0;
        virtual void Stop ( void ) = 0;
        virtual void SetPitch( float pitch ) = 0; 
        virtual bool IsPaused( void ) = 0;
        
        virtual void ChangeTrim( daSoundGroup groupName, float newTrim ) = 0;  
        virtual void ChangeFaderTrim( daSoundGroup groupName, float newTrim ) = 0;
};

//
// This contains a Dark Angel player instance.
//
class daSoundClipStreamPlayer
    :
    public daSoundPlayerBase,
    public IRadSoundStitchCallback
                    
{
public:

    //
    // Constructor and destructor
    //
    daSoundClipStreamPlayer( void );
    virtual ~daSoundClipStreamPlayer ( void );

    // daSoundPlayerBase
    
    virtual void ServiceOncePerFrame ( void );
    virtual bool IsCaptured ( void );
    virtual void Pause ( void );
    virtual void Continue ( void );
    virtual void UberContinue( void );
    virtual void Stop ( void );
    virtual void SetPitch( float pitch );
    
    virtual void ChangeTrim( daSoundGroup groupName, float newTrim ); 
    virtual void ChangeFaderTrim( daSoundGroup groupName, float newTrim );

    bool IsPaused( void );
    
    //
    // Sound player states
    //
    
    enum State {
        State_DeCued,
        State_Cueing,
        State_Cued,
        State_CuedPlay,
        State_Playing,
        State_Stopping,
        State_Done
    };

    //
    // Get and state this player's state
    //
    
    inline State GetState ( void );
    
    //
    // Initialize with some form of player
    //
    void InitializeAsClipPlayer( void );
    void InitializeAsStreamPlayer( void );

    void Capture(
        IDaSoundResource* pResource,
        bool isPositional );
        
    inline IRadSoundHalPositionalGroup* GetPositionalGroup( void );
        
    void Play( void );
    void UnCapture ( void );

    inline void SetPositionAndVelocity(
        const radSoundVector * pPosition,
        const radSoundVector * pVelocity );

    inline void SetMinMaxDistance( float min, float max );
    
    inline void SetExternalTrim( float newTrim );
    inline void SetGroupTrim( float newTrim );
    inline void SetFaderGroupTrim( float newTrim );
    inline void SetMasterTrim( float newTrim );

    inline IDaSoundResource::Type GetPlayerType ( void );
    daSoundGroup GetSoundGroup ( void );

    void RegisterSoundPlayerStateCallback
    (
        IDaSoundPlayerState* pCallback,
        void* pUserData
    );
    void UnregisterSoundPlayerStateCallback
    (
        IDaSoundPlayerState* pCallback,
        void* pUserData
    );

    unsigned int GetPlaybackTimeInSamples ( void );
    
    void OnStitch( IRadSoundHalDataSource **, unsigned int frameCount, void * pUserData );

    const void GetFileName( char * pBuf, unsigned int max );
      
private:
    
    enum CueingState
    {
        CueingState_Null,
        CueingState_Resource,
        CueingState_Player,
        CueingState_Cued
    };
        
    inline void CalculateCurrentTrim( void );
    inline void CalculateCurrentPitch( void );
    void HookUpAndCuePlayer( void );
    
    void UpdateClip( void );
    void UpdateStream( void );    
    
    //
    // Get the randomized trim and pitch settings based on a
    // min/max variance of the current resource
    //
    inline void CalculateNewVaryingTrim ( void );
    inline void CalculateNewVaryingPitch ( void );

    //
    // Trim values
    //
    float m_Trim;
    float m_GroupTrim;
    float m_FaderGroupTrim;
    float m_MasterTrim;
    float m_StoppingTrim;
    float m_VaryingTrim;
    float m_CurrentTrim; // floating target
        
    float m_VaryingPitch;
    float m_Pitch;    
    float m_CurrentPitch; // floating target
    
    //
    // Sound player state
    //
    State m_State;
    CueingState m_CueingState;
    
    //
    // Hold a capture counter
    //
    unsigned int                        m_CaptureCount;

    //
    // Hold a pause counter
    //
    unsigned int                        m_PauseCount;

    //
    // Store the allocated and normal resource
    //
    daSoundAllocatedResource*           m_pAllocatedResource;
    unsigned int                        m_AllocResInstanceID;
    IDaSoundResource*                   m_pResource;

    //
    // The positional group that this player uses
    //
    IRadSoundHalPositionalGroup * m_pPositionalGroup;
    bool m_IsPositional;

    //
    // Currently, only one callback is allowed at a time
    //
    IDaSoundPlayerState*                m_pStateChangeCallback;
    void*                               m_pStateChangeCallbackUserData;
    
    //
    // Store the various types of players based on the connected resource
    //
    union
    {
        struct
        {
            IRadSoundClipPlayer*            m_pClipPlayer;
        } m_ClipInfo;
        
        struct
        {
            StreamerResources* m_pResources;
            IRadSoundRsdFileDataSource*     m_pRsdFileDataSource;
        } m_StreamInfo;
    };
    
    IDaSoundResource::Type m_Type;
};


inline IDaSoundResource::Type daSoundClipStreamPlayer::GetPlayerType( void )
{
    return m_Type;
}

inline void daSoundClipStreamPlayer::SetExternalTrim( float newTrim )
{
    radSoundVerifyAnalogVolume( newTrim );
    
    m_Trim = newTrim;
    
}

inline void daSoundClipStreamPlayer::SetGroupTrim( float newTrim )
{
    radSoundVerifyAnalogVolume( newTrim );
    
    m_GroupTrim = newTrim;

}

inline void daSoundClipStreamPlayer::SetFaderGroupTrim( float newTrim )
{
    radSoundVerifyAnalogVolume( newTrim );
    
    m_FaderGroupTrim = newTrim;
}

inline void daSoundClipStreamPlayer::SetMasterTrim( float newTrim )
{
    radSoundVerifyAnalogVolume( newTrim );
    
    m_MasterTrim = newTrim;
}

inline void daSoundClipStreamPlayer::CalculateCurrentTrim( void)
{
    float oldTrim = m_CurrentTrim;
    
    float newTrim =
        m_VaryingTrim *
        m_StoppingTrim *
        m_Trim *
        m_GroupTrim *
        m_FaderGroupTrim *
        m_MasterTrim;
 
    if ( State_Playing == m_State || State_Stopping == m_State )
    {
        if ( newTrim >= oldTrim )
        {
            float dif = newTrim - oldTrim;
            
            if ( dif >= radSoundVolumeChangeThreshold )
            {
                newTrim = oldTrim + radSoundVolumeChangeThreshold;
            }
        }
        else
        {
            float dif = oldTrim - newTrim;
            
            if ( dif >= radSoundVolumeChangeThreshold )
            {
                newTrim = oldTrim - radSoundVolumeChangeThreshold;
            }
        }
    }       
    
    m_CurrentTrim = newTrim;
    
    radSoundVerifyAnalogVolume( m_CurrentTrim );    
}

inline void daSoundClipStreamPlayer::CalculateCurrentPitch( void )
{
    float oldPitch = m_CurrentPitch;
    
    float newPitch = m_VaryingPitch * m_Pitch;
    
    if ( m_State == State_Playing || m_State == State_Stopping )
    {        
        if ( newPitch > oldPitch )
        {
            float dif = newPitch - oldPitch;
            
            if(  dif > radSoundPitchChangeThreshold )
            {
                newPitch = oldPitch + radSoundVolumeChangeThreshold;
            }
        }
        else
        {
            float dif = oldPitch - newPitch;
            
            if(  dif > radSoundPitchChangeThreshold )
            {
                newPitch = oldPitch - radSoundVolumeChangeThreshold;
            }
        }
    }
    
    m_CurrentPitch = newPitch;
    
    radSoundVerifyAnalogPitch( m_CurrentPitch );     
}

inline void daSoundClipStreamPlayer::SetPitch( float pitch )
{
    if ( pitch <= 0.0f )
    {
        rDebugPrintf( "AUDIO: Error, pitch set to: [%f]\n", pitch );
                
        pitch = 0.0f;
    }
    
    // radSoundVerifyAnalogPitch( pitch );     
    
    m_Pitch = pitch;
}

inline void daSoundClipStreamPlayer::CalculateNewVaryingTrim( void )
{    
    float minTrim;
    float maxTrim;

    m_pResource->GetTrimRange( &minTrim, &maxTrim );
    
    m_VaryingTrim = ::radSoundRandMinMax( minTrim, maxTrim );
    
    radSoundVerifyAnalogVolume( m_VaryingTrim );
}


inline void daSoundClipStreamPlayer::CalculateNewVaryingPitch( void )
{
    float minPitch;
    float maxPitch;
    
    m_pResource->GetPitchRange( &minPitch, &maxPitch );

    m_VaryingPitch = ::radSoundRandMinMax( minPitch, maxPitch );
    
    radSoundVerifyAnalogPitch( m_VaryingPitch );    
}

inline daSoundClipStreamPlayer::State daSoundClipStreamPlayer::GetState( void )
{
    return m_State;
}

inline void daSoundClipStreamPlayer::SetPositionAndVelocity(
    const radSoundVector * pPosition,
    const radSoundVector * pVelocity )
{
    m_pPositionalGroup->SetPosition( (radSoundVector*) pPosition );
    m_pPositionalGroup->SetVelocity( (radSoundVector*) pVelocity );
}

inline void daSoundClipStreamPlayer::SetMinMaxDistance( float min, float max )
{
    m_pPositionalGroup->SetMinMaxDistance( min, max );
}

inline bool daSoundClipStreamPlayer::IsPaused( void )
{
    return m_PauseCount != 0;
}
    

inline IRadSoundHalPositionalGroup * daSoundClipStreamPlayer::GetPositionalGroup( void )
{
    if ( m_IsPositional )
    {
        return m_pPositionalGroup;
    }

    return NULL;
}    


} // Sound Namespace
#endif //_DASOUNDPLAYER_HPP

