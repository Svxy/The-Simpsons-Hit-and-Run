//=============================================================================
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundresource.cpp
//
// Subsystem:   Dark Angel - Sound resources
//
// Description: Implements sound resources
//
// Modification History:
//  + Created October 11, 2001 -- aking
//
//=============================================================================

//=============================================================================
// Included Files
//=============================================================================

#include <radobject.hpp>
#include <raddebug.hpp>

#include <sound/soundrenderer/soundsystem.h>
#include <sound/soundrenderer/soundplayer.h>
#include <sound/soundrenderer/soundrenderingmanager.h>
#include <sound/soundrenderer/playermanager.h>

#include <sound/soundrenderer/soundresourcemanager.h>

#include <sound/soundrenderer/idasoundresource.h>
#include <sound/soundrenderer/soundresource.h>

#include <sound/soundmanager.h>
#include <memory/srrmemory.h>

//=============================================================================
// Static Variables
//=============================================================================

//=============================================================================
// daSoundResourceData Implementation
//=============================================================================

//=============================================================================
// Function:    daSoundResourceData::daSoundResourceData
//=============================================================================
// Description: Constructor
//
//-----------------------------------------------------------------------------


inline unsigned char vol_f_to_c( float f )
{
    return (unsigned char) radSoundFloatToUInt( f * 255.0f );
}

inline float vol_c_to_f( unsigned char c )
{
    return radSoundUIntToFloat( (unsigned int) c ) / 255.0f;
}

inline unsigned short pitch_f_to_s( float f )
{
    return (unsigned short) radSoundFloatToUInt( f * 6553.0f );
}

inline float pitch_s_to_f( unsigned short c )
{
    return radSoundUIntToFloat( (unsigned short) c ) / 6553.0f;
}

const int FLAG_LOOPING = 1 << 0;
const int FLAG_STREAMING = 1 << 1;

daSoundResourceData::daSoundResourceData( )
{
    m_NumFiles = 0;
    
    m_MinTrim = vol_f_to_c( 1.0f );
    m_MaxTrim = vol_f_to_c( 1.0f );
    m_MinPitch = pitch_f_to_s( 1.0f );
    m_MaxPitch = pitch_f_to_s( 1.0f );

    m_SoundGroup = Sound::MASTER;
    m_CaptureCount = 0;
    
    m_Flags = 0;

    m_pFileIds = 0;
}

//=============================================================================
// Function:    daSoundResourceData::~daSoundResourceData
//=============================================================================
// Description: Destructor
//
//-----------------------------------------------------------------------------

daSoundResourceData::~daSoundResourceData( )
{

}

//=============================================================================
// Function:    daSoundResourceData::AddFilename
//=============================================================================
// Description: Add a file to this resource
//
// Parameters:  newFileName - the new file name
//              trim - the trim for this file
//
// Returns:     n/a
//
//-----------------------------------------------------------------------------

void daSoundResourceData::AddFilename
(
    const char* newFileName,
    float trim
)
{
    rAssert( false == Sound::daSoundResourceManager::GetInstance( )->GetResourceLockdown( ) );

    // very lazy indeed.
    
    if ( m_pFileIds == 0 )
    {    
        m_pFileIds = (FileId*) radMemoryAlloc( GMA_DEFAULT, sizeof( FileId ) * DASound_MaxNumSoundResourceFiles );
    }

    rAssert( Sound::daSoundResourceManager::GetInstance( ) != NULL );
    rAssert( m_NumFiles < DASound_MaxNumSoundResourceFiles );
    
    m_pFileIds[ m_NumFiles ].m_pName = (char*) radMemoryAlloc( GMA_DEFAULT, strlen( newFileName ) + 1 );
    strcpy( m_pFileIds[ m_NumFiles ].m_pName, newFileName );
    m_NumFiles++;
}


//=============================================================================
// Function:    daSoundResourceData::GetFilenameAt
//=============================================================================
// Description: Get the resource file at the given index
//
//-----------------------------------------------------------------------------

void daSoundResourceData::GetFileNameAt( unsigned int index, char* buffer, unsigned int max )
{
    rTuneAssert( false == Sound::daSoundResourceManager::GetInstance( )->GetResourceLockdown( ) );
    rTuneAssert( index < m_NumFiles );
    
    strcpy( buffer, m_pFileIds[ index ].m_pName );
}

void daSoundResourceData::GetFileKeyAt( unsigned int index, char* buffer, unsigned int max )
{
    rTuneAssert( index < m_NumFiles );
    KeyToStringKey32( buffer, m_pFileIds[ index ].m_Key );
}

//=============================================================================
// Function:    daSoundResourceData::SetPitchRange
//=============================================================================
// Description: Set the pitch range for this resource
//
// Parameters:  minPitch - the minimum pitch value
//              maxPitch - the maximum pitch value
//
// Returns:     n/a
//
//-----------------------------------------------------------------------------

void daSoundResourceData::SetPitchRange
(
    Sound::daPitchValue minPitch,
    Sound::daPitchValue maxPitch
)
{
    m_MinPitch = pitch_f_to_s( minPitch );
    m_MaxPitch = pitch_f_to_s( maxPitch );
}

//=============================================================================
// Function:    daSoundResourceData::GetPitchRange
//=============================================================================
// Description: Get the pitch range for this resource
//
// Parameters:  *MinPitch - (out )the minimum pitch value
//              *MaxPitch - (out) the maximum pitch value
//
// Returns:     n/a
//
//-----------------------------------------------------------------------------

void daSoundResourceData::GetPitchRange
(
    Sound::daPitchValue* pMinPitch,
    Sound::daPitchValue* pMaxPitch
)
{
    if( pMinPitch != NULL )
    {
        *pMinPitch = pitch_s_to_f( m_MinPitch );
    }
    if( pMaxPitch != NULL )
    {
        *pMaxPitch = pitch_s_to_f( m_MaxPitch );
    }
}

//=============================================================================
// Function:    daSoundResourceData::SetTrimRange
//=============================================================================
// Description: Set the trim range for this resource
//
// Parameters:  minTrim - the minimum trim value
//              maxTrim - the maximum trim value
//
// Returns:     n/a
//
//-----------------------------------------------------------------------------

void daSoundResourceData::SetTrimRange
(
    float minTrim,
    float maxTrim
)
{
    m_MinTrim = vol_f_to_c( minTrim );
    m_MaxTrim = vol_f_to_c( maxTrim );
}

//=============================================================================
// Function:    daSoundResourceData::SetTrim
//=============================================================================
// Description: Set the trim for this resource
//
// Parameters:  trim - the trim value
//
// Returns:     n/a
//
//-----------------------------------------------------------------------------

void daSoundResourceData::SetTrim
(
    float trim
)
{
    m_MinTrim = vol_f_to_c( trim );
    m_MaxTrim = vol_f_to_c( trim );
}

//=============================================================================
// Function:    daSoundResourceData::GetTrimRange
//=============================================================================
// Description: Get the trim range for this resource
//
// Parameters:  *MinTrim - (out )the minimum trim value
//              *MaxTrim - (out) the maximum trim value
//
// Returns:     n/a
//
//-----------------------------------------------------------------------------

void daSoundResourceData::GetTrimRange
(
    float* pMinTrim,
    float* pMaxTrim
)
{
    if( pMinTrim != NULL )
    {
        *pMinTrim = vol_c_to_f( m_MinTrim );
    }
    if( pMaxTrim != NULL )
    {
        *pMaxTrim = vol_c_to_f( m_MaxTrim );
    }
}

//=============================================================================
// Function:    daSoundResourceData::SetStreaming
//=============================================================================
// Description: Set this as a streaming resource
//
// Parameters:  streaming - should streaming be on or off
//
// Returns:     n/a
//
//-----------------------------------------------------------------------------

void daSoundResourceData::SetStreaming( bool streaming )
{
    if( streaming )
    {
        m_Flags |= FLAG_STREAMING;
    }
    else
    {
        m_Flags &= ~FLAG_STREAMING;
    }
    
    if( IsCaptured( ) )
    {
        rReleaseString
        (
            "Streaming will not immediately "
            "affect a captured resource\n"
        );
    }
}

//=============================================================================
// Function:    daSoundResourceData::GetStreaming
//=============================================================================
// Description: Get whether or not this is a streaming resource
//
// Parameters:  none
//
// Returns:     Returns true if this resource streams, otherwise false
//
//-----------------------------------------------------------------------------

bool daSoundResourceData::GetStreaming( void )
{
    return ( m_Flags & FLAG_STREAMING ) != 0;
}

//=============================================================================
// Function:    daSoundResourceData::SetLooping
//=============================================================================
// Description: Set this as a looping resource
//
// Parameters:  looping - should looping be on or off
//
// Returns:     n/a
//
//-----------------------------------------------------------------------------

void daSoundResourceData::SetLooping( bool looping )
{
    if( looping )
    {
        m_Flags |= FLAG_LOOPING;
    }
    else
    {
        m_Flags &= ~FLAG_LOOPING;
    }
    
    if( IsCaptured( ) )
    {
        rReleaseString
        (
            "Looping will not immediately "
            "affect a captured resource\n"
        );
    }
}

//=============================================================================
// Function:    daSoundResourceData::GetLooping
//=============================================================================
// Description: Get whether or not this is a looping resource
//
// Parameters:  none
//
// Returns:     Returns true if this resource loops, otherwise false
//
//-----------------------------------------------------------------------------

bool daSoundResourceData::GetLooping( void )
{
    return ( m_Flags & FLAG_LOOPING ) != 0;
}

//=============================================================================
// Function:    daSoundResourceData::GetType
//=============================================================================
// Description: Find out what kind of resource this is
//
// Parameters:  none
//
// Returns:     Returns the type of resource
//
//-----------------------------------------------------------------------------

IDaSoundResource::Type daSoundResourceData::GetType
(
    void
)
{
    IDaSoundResource::Type resourceType = UNKNOWN;

    if( GetStreaming( ) )
    {
        resourceType = STREAM;
    }
    else
    {
        resourceType = CLIP;
    }

    return resourceType;
}

//=============================================================================
// Function:    daSoundResourceData::SetSoundGroup
//=============================================================================
// Description: Set the sound group that this resource belongs to
//
// Parameters:  soundGroup - the sound group to associate with this resource
//
//-----------------------------------------------------------------------------

void daSoundResourceData::SetSoundGroup
(
    Sound::daSoundGroup soundGroup
)
{
    m_SoundGroup = soundGroup;
}

//=============================================================================
// Function:    daSoundResourceData::GetSoundGroup
//=============================================================================
// Description: Get the sound group that this resource belongs to
//
// Parameters:  none
//
// Returns:     Returns the sound group that this resource belongs to.
//
//-----------------------------------------------------------------------------

Sound::daSoundGroup daSoundResourceData::GetSoundGroup( void )
{
    return static_cast<Sound::daSoundGroup>(m_SoundGroup);
}

//=============================================================================
// Function:    daSoundResourceData::CaptureResource
//=============================================================================
// Description: Capture this resource
//
// Parameters:  none
//
// Returns:     n/a
//
//-----------------------------------------------------------------------------

void daSoundResourceData::CaptureResource( void )
{
    rAssert( Sound::daSoundResourceManager::GetInstance( ) != NULL );
    bool wasCaptured = IsCaptured( );
    ++m_CaptureCount;
    if( !wasCaptured )
    {
        // Inform the resource manager
        Sound::daSoundResourceManager::GetInstance( )->
            AllocateResource( this );
    }
}

//=============================================================================
// Function:    daSoundResourceData::IsCaptured
//=============================================================================
// Description: Is this resource captured?
//
// Parameters:  none
//
// Returns:     Returns true if the resource is captured, otherwise false.
//
//-----------------------------------------------------------------------------

bool daSoundResourceData::IsCaptured( void )
{
    return( m_CaptureCount > 0 );
}

//=============================================================================
// Function:    daSoundResourceData::ReleaseResource
//=============================================================================
// Description: Release this resource
//
// Parameters:  none
//
// Returns:     n/a
//
//-----------------------------------------------------------------------------

void daSoundResourceData::ReleaseResource( void )
{
    rAssertMsg( m_CaptureCount > 0, "Cannot release uncaptured resource" );
    rAssert( Sound::daSoundResourceManager::GetInstance( ) != NULL );

    --m_CaptureCount;
    if( m_CaptureCount == 0 )
    {
        // Inform the resource manager
        Sound::daSoundResourceManager::GetInstance( )->
            DeallocateResource( this );
    }
}

//=============================================================================
// Function:    daSoundResourceData::ReleaseResource
//=============================================================================
// Description: Plays this resource
//
// Notes:       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//              THIS IS FOR COMPOSERS ONLY.  DO NOT USE IT.
//              !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
//-----------------------------------------------------------------------------

void daSoundResourceData::Play( void )
{
    if( Sound::daSoundRenderingManagerGet( )->GetResourceManager( )->GetResourceLockdown( ) )
    {
        // Play it
        // DO NOT USE THIS COMMAND IF YOU ARE NOT IN RADTUNER / SOUNDTUNER / etc.
        Sound::daSoundClipStreamPlayer* pPlayer = NULL;
        Sound::daSoundRenderingManagerGet( )->GetPlayerManager( )->CaptureFreePlayer
        (
            &pPlayer,
            this,
            false );
            
        if( pPlayer != NULL )
        {
            pPlayer->Play(  );
        }
    }
    else
    {
        // You shouldn't have left play commands in the composer script
        rDebugString( "Can't play sounds before resources are locked down.\n" );
    }
}

//=============================================================================
// Public functions
//=============================================================================

void daSoundResourceData::AddRef( void )
{
    //Sound::daSoundResourceManager::GetInstance( )->AddRef( );
}

void daSoundResourceData::Release( void )
{
    //Sound::daSoundResourceManager::GetInstance( )->Release( );
}



