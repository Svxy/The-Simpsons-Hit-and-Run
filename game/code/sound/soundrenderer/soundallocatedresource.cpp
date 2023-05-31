//=============================================================================
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundallocatedresource.cpp
//
// Subsystem:   Dark Angel - Sound Resource Management System
//
// Description: Implementation of an allocated sound resource
//
// Revisions:
//  + Created October 18, 2001 -- breimer
//
//=============================================================================

//=============================================================================
// Included Files
//=============================================================================

#include <radobject.hpp>
#include <raddebug.hpp>
#include <radsound.hpp>
#include <raddebugwatch.hpp>

#include <sound/soundrenderer/soundsystem.h>
#include <sound/soundrenderer/soundrenderingmanager.h>
#include <sound/soundrenderer/soundresourcemanager.h>
#include <sound/soundrenderer/soundallocatedresource.h>
#include <sound/soundrenderer/idasoundresource.h>
#include <sound/soundrenderer/idasoundtuner.h>
#include <sound/soundrenderer/soundnucleus.hpp>

#include <memory/srrmemory.h>

//=============================================================================
// Static Variables (outside namespace)
//=============================================================================
    
//=============================================================================
// Namespace
//=============================================================================

namespace Sound {

//=============================================================================
// Static Variables (inside namespace)
//=============================================================================

//=============================================================================
// Constants
//=============================================================================

#ifndef RAD_RELEASE

#ifdef RAD_DEBUG
static bool s_displayMemoryInfo = true;
#else
static bool s_displayMemoryInfo = false;
#endif

static bool s_isInitialized = false;
static int s_memoryRemaining = 0;

#endif // RAD_RELEASE

//=============================================================================
// Class Implementations
//=============================================================================

//=============================================================================
// daSoundAllocatedResource Implementation
//=============================================================================

//=============================================================================
// Function:    daSoundFileInstance::daSoundFileInstance
//=============================================================================
// Description: Constructor
//
//-----------------------------------------------------------------------------

daSoundFileInstance::daSoundFileInstance(
    IDaSoundResource* pResource,
    unsigned int fileIndex )
{
    rAssert( pResource != NULL );
    
    m_RefCount = 0;
    m_State = UnLoaded;
    m_pSoundClip = NULL;

    m_State = UnLoaded;    

    m_FileIndex = fileIndex;
    
    m_pResource = pResource;
    m_pResource->AddRef( );

    // Set the type of resource
    
    IDaSoundResource::Type type = pResource->GetType( );       

    if( type == IDaSoundResource::CLIP )
    {
        m_Type = IDaSoundResource::CLIP;
    }
    else
    {
        rAssert( type == IDaSoundResource::STREAM );
        m_Type = IDaSoundResource::STREAM;
    }
        
#ifndef RAD_RELEASE
    if( !s_isInitialized )
    {
        radDbgWatchAddBoolean( &s_displayMemoryInfo, "Show Loading Spew", "Sound Info", 0, 0 );

        s_isInitialized = true;
    }
#endif
}

//=============================================================================
// Function:    daSoundFileInstance::~daSoundFileInstance
//=============================================================================
// Description: Destructor
//
//-----------------------------------------------------------------------------

daSoundFileInstance::~daSoundFileInstance( )
{
    m_pResource->Release( );
    
    rAssert( m_State == UnLoaded );
    rAssert( NULL == m_pSoundClip );
  
}

//=============================================================================
// Function:    daSoundFileInstance::CreateFileDataSource
//=============================================================================
// Description: Get the sound stream.
//
// Returns:     Returns the sound stream if it is allocated for this resource,
//              or NULL if it is not.
//
//-----------------------------------------------------------------------------

void daSoundFileInstance::CreateFileDataSource(
    IRadSoundRsdFileDataSource** ppFds )
{
    rAssert( GetType( ) == IDaSoundResource::STREAM );
    rAssert( Loaded == m_State );    

    char fileName[ 256 ];
    m_pResource->GetFileKeyAt( m_FileIndex, fileName, 256 );
    
    *ppFds = radSoundRsdFileDataSourceCreate( GMA_AUDIO_PERSISTENT );
    (*ppFds)->AddRef( );
    
       (*ppFds)->InitializeFromFileName(
        fileName,
        true,
        0,
        IRadSoundHalAudioFormat::Milliseconds,
        SoundNucleusGetStreamFileAudioFormat( ) );
        
}

//=============================================================================
// Function:    daSoundFileInstance::OnDynaLoadObjectCreate
//=============================================================================
// Description: Called when this object is being created (by dynamic loading)
//
//-----------------------------------------------------------------------------

void daSoundFileInstance::Load( IRadSoundHalMemoryRegion* pRegion )
{
    rAssert( m_State == UnLoaded );

    rAssert( m_Type == IDaSoundResource::UNKNOWN || m_pResource != NULL );
                    
    // Create each type of object
    if( m_Type == IDaSoundResource::CLIP )
    {
        char fileName[ 256 ];
        m_pResource->GetFileKeyAt( m_FileIndex, fileName, 256 );
        
        SoundNucleusLoadClip( fileName, m_pResource->GetLooping( ) );
    }

    m_State = Loading;
}

//=============================================================================
// Function:    daSoundFileInstance::OnDynaLoadObjectCreate
//=============================================================================
// Description: Is this object ready and stable?
//
//-----------------------------------------------------------------------------

bool daSoundFileInstance::UpdateLoading( void )
{
    rAssert( Loading == m_State );
    
    if( m_Type == IDaSoundResource::CLIP )
    {
        if ( SoundNucleusIsClipLoaded( ) )
        {
            SoundNucleusFinishClipLoad( & m_pSoundClip );
            m_State = Loaded;
        }
    }
    else
    {
        m_State = Loaded;
    }
    
    return Loaded == m_State;
}

//=============================================================================
// Function:    daSoundFileInstance::OnDynaLoadObjectDestroy
//=============================================================================
// Description: Destroy this dynamically loading object
//
//-----------------------------------------------------------------------------

void daSoundFileInstance::UnLoad( void )
{
    rAssert( m_State == Loaded || Loading == m_State );
    
    if ( m_Type == IDaSoundResource::CLIP )
    {
        if ( Loading == m_State )
        {
            SoundNucleusCancelClipLoad( );
        }
        else if ( Loaded == m_State )
        {
            rAssert( m_pSoundClip != NULL );
            m_pSoundClip->Release( );
            m_pSoundClip = NULL;
        }
    }

    m_State = UnLoaded;
}
    
//=============================================================================
// daSoundAllocatedResource Implementation
//=============================================================================

//=============================================================================
// Function:    daSoundAllocatedResource::daSoundAllocatedResource
//=============================================================================
// Description: Constructor
//
//-----------------------------------------------------------------------------

daSoundAllocatedResource::daSoundAllocatedResource( )
    :
    m_RefCount( 0 )
{   
    unsigned int i = 0;
    
    for( i = 0; i < DASound_MaxNumSoundResourceFiles; i++ )
    {
        m_pFileInstance[ i ] = NULL;
        m_pDynaLoadRegion[ i ] = NULL;
    }
}

//=============================================================================
// Function:    daSoundAllocatedResource::~daSoundAllocatedResource
//=============================================================================
// Description: Destructor
//
//-----------------------------------------------------------------------------

daSoundAllocatedResource::~daSoundAllocatedResource( void )
{
    if( GetResource( ) != NULL )
    {
        unsigned int i = 0;
        unsigned int j = 0;
        for( i = 0; i < DASound_MaxNumSoundResourceFiles; i++ )
        {
            // Release the dynamic loading region
            if( m_pDynaLoadRegion[ i ] != NULL )
            {
                for( j = 0; j < m_pDynaLoadRegion[ i ]->GetNumSlots( ); j++ )
                {
                    m_pDynaLoadRegion[ i ]->SwapInObject( j, NULL );
                }
                m_pDynaLoadRegion[ i ]->Release( );
                m_pDynaLoadRegion[ i ] = NULL;
            }

            // Release file instance
            if( m_pFileInstance[ i ] != NULL )
            {
                m_pFileInstance[ i ]->Release( );
                m_pFileInstance[ i ] = NULL;
            }
        }

        // Release the resource
        m_pResource->Release( );
    }

}

//=============================================================================
// Function:    daSoundAllocatedResource::Initialize
//=============================================================================
// Description: Intialize the allocated resource by giving it a normal
//              resource.
//
// Parameters:  pResource - the resource for this instance to associate with
//              index - the index of the resource file to use
//
//-----------------------------------------------------------------------------

void daSoundAllocatedResource::Initialize
(
    IDaSoundResource* pResource
)
{
    rAssert( pResource != NULL );

    m_pResource = pResource;
    m_pResource->AddRef( );

    // Look in the resource manager's tree of allocated resources
    // If the file is already loaded, just addref the object
    Sound::daSoundResourceManager* pResManager = Sound::daSoundResourceManager::GetInstance( );

    // Load and initialize the files;
    
    unsigned int numFiles = m_pResource->GetNumFiles( );
    
    for( unsigned int i = 0; i < numFiles; i++ )
    {

#ifdef RAD_XBOX
        daSoundFileInstance* pFileInstance = new( GMA_XBOX_SOUND_MEMORY ) daSoundFileInstance( m_pResource, i );
#else
        daSoundFileInstance* pFileInstance = new( GMA_AUDIO_PERSISTENT ) daSoundFileInstance( m_pResource, i );
#endif
        pFileInstance->AddRef( );

        //
        // Generate a dyna load region for this object
        //
        
        unsigned int size = 0;
        daSoundDynaLoadRegion* pRegion = NULL;
        m_pDynaLoadRegion[ i ] = Sound::daSoundRenderingManagerGet( )->
            GetDynaLoadManager( )->
            CreateRegion
            (
                ::radSoundHalSystemGet( )->GetRootMemoryRegion( ),
                size,
                1
            );
        rAssert( m_pDynaLoadRegion[ i ] != NULL );
        m_pDynaLoadRegion[ i ]->AddRef( );
        m_pDynaLoadRegion[ i ]->SwapInObject( 0, pFileInstance );

        // Set the file instance internally
        // (The AddRef is just copied)
        
        m_pFileInstance[ i ] = pFileInstance;
    }
}

//=============================================================================
// Function:    daSoundAllocatedResource::ChooseNextInstance
//=============================================================================
// Description: Choose the next instance to play in this allocated resource
//
// Notes:
//  The resource automatically keeps track of what files
//  are being used.  It will try not to let anything repeat
//  based on the following rules:
//    (1) IF ( numfiles <= DASound_NumLastPlayedFilesToRemember ) THEN
//        Choose a file randomly
//    (2) IF ( numfiles == DASound_NumLastPlayedFilesToRemember + 1 ) THEN
//        Choose randomly out of all files except the last one
//        used.
//    (2) ELSE
//        Don't choose one of the DASound_NumLastPlayedFilesToRemember
//        last files, but choose randomly out of the rest
//
//-----------------------------------------------------------------------------

unsigned int daSoundAllocatedResource::ChooseNextInstance( void )
{
    rAssert( GetResource( ) != NULL );

    // Get the number of resource files
    unsigned int numResourceFiles = GetResource( )->GetNumFiles( );
    rAssert( numResourceFiles > 0 );

    // Return the chosen file
    return( rand( ) % numResourceFiles );
}

//=============================================================================
// Function:    daSoundAllocatedResource::GetFileInstance
//=============================================================================
// Description: Get the file instance at the given index
//
// Returns:     Returns a pointer to the file instance
//
//-----------------------------------------------------------------------------

daSoundFileInstance* daSoundAllocatedResource::GetFileInstance
(
    unsigned int index
)
{
    rAssert( GetResource( ) != NULL );
    rAssert( index < GetResource( )->GetNumFiles( ) );

    return m_pFileInstance[ index ];
}

} // Sound Namespace

