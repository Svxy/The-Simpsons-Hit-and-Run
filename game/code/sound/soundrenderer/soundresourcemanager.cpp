//=============================================================================
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundresourcemanager.cpp
//
// Subsystem:   Dark Angel - Sound Resource Manager
//
// Description: Implementation of the sound resource manager
//
// Revisions:
//  + Created October 11, 2001 -- breimer
//
//=============================================================================

//=============================================================================
// Included Files
//=============================================================================

#include <radobject.hpp>
#include <raddebug.hpp>
#include <radkey.hpp>
#include <radobjectbtree.hpp>
#include <radnamespace.hpp>
#include <radsound.hpp>

#include <sound/soundrenderer/soundsystem.h>
#include <sound/soundrenderer/soundresource.h>
#include <sound/soundrenderer/soundallocatedresource.h>
#include <sound/soundrenderer/soundresourcemanager.h>
#include <sound/soundrenderer/soundrenderingmanager.h>

#include <memory/srrmemory.h>

//=============================================================================
// Namespace
//=============================================================================

namespace Sound {

//=============================================================================
// Static Variables
//=============================================================================

daSoundResourceManager* daSoundResourceManager::s_pSingleton = NULL;

//=============================================================================
// Constants
//=============================================================================

//
// The maximum number of sound resource files (not the resources, but the
// the actual files)
//
const unsigned int MaxNumResourceFiles = 512;

#if defined( RAD_XBOX ) || defined( RAD_WIN32 )
    const IRadSoundHalAudioFormat::Encoding DEFAULT_ENCODING = IRadSoundHalAudioFormat::PCM;
#elif defined RAD_PS2
    const IRadSoundHalAudioFormat::Encoding DEFAULT_ENCODING = IRadSoundHalAudioFormat::VAG;
#elif defined RAD_GAMECUBE
    const IRadSoundHalAudioFormat::Encoding DEFAULT_ENCODING = IRadSoundHalAudioFormat::PCM_BIGENDIAN;
#else
    Uh oh.
#endif

//=============================================================================
// Class Implementations
//=============================================================================

//=============================================================================
// daSoundResourceManager Implementation
//=============================================================================


//=============================================================================
// Function:    daSoundResourceManager::daSoundResourceManager
//=============================================================================
// Description: Constructor
//
//-----------------------------------------------------------------------------

daSoundResourceManager::daSoundResourceManager( )
    :
    radRefCount( 0 ),
    m_pSoundNamespace( NULL ),
    m_ResourceLockdown( false ),
    m_secondaryNamespace( NULL )
    
{
    m_pFileIdMemory = 0;

    m_NumResourceDatas = 0;
    
    // Create the singleton
    rAssert( s_pSingleton == NULL );
    s_pSingleton = this;

    // Remember the sound namespace
    m_pSoundNamespace = Sound::daSoundRenderingManagerGet( )->GetSoundNamespace( );
    if( m_pSoundNamespace != NULL )
    {
        m_pSoundNamespace->AddRef( );
    }

    m_xIOL_AllocatedResources = new ( GetThisAllocator( )) radObjectBTree( );
}

//=============================================================================
// Function:    daSoundResourceManager::~daSoundResourceManager
//=============================================================================
// Description: Destructor
//
//-----------------------------------------------------------------------------

daSoundResourceManager::~daSoundResourceManager( )
{
    // Release the allocated sound resources
    m_xIOL_AllocatedResources = NULL;

    // Release the sound namespace
    if( m_pSoundNamespace != NULL )
    {
        m_pSoundNamespace->Release( );
        m_pSoundNamespace = NULL;
    }

    // Remove the singleton
    rAssert( s_pSingleton != NULL );
    s_pSingleton = NULL;
}

//=============================================================================
// Function:    daSoundResourceManager::AllocateResource
//=============================================================================
// Description: Allocate a resource
//
// Parameters:  pResource - the resource to allocate
//
//-----------------------------------------------------------------------------

void daSoundResourceManager::AllocateResource
(
    IDaSoundResource* pResource
)
{
    rAssert( pResource != NULL );

    //
    // Allocate this resource
    //
    radKey32 soundKey = (radKey32) pResource;

    // Find out if it already exists...
    daSoundAllocatedResource* pAllocatedResource =
        FindAllocatedResource( pResource );
    if( pAllocatedResource == NULL )
    {
        pAllocatedResource = new ( GetThisAllocator( ) ) daSoundAllocatedResource;
        pAllocatedResource->AddRef( );
        pAllocatedResource->Initialize( pResource );

        // Add it to our allocated resource manager
        m_xIOL_AllocatedResources->AddObject (
            soundKey,
            pAllocatedResource
        );

        // Release our version
        pAllocatedResource->Release( );
    }

    rAssert( pAllocatedResource != NULL );
}

//=============================================================================
// Function:    daSoundResourceManager::DeallocateResource
//=============================================================================
// Description: Allocate a resource
//
// Parameters:  pResource - the resource to allocate
//
//-----------------------------------------------------------------------------

void daSoundResourceManager::DeallocateResource
(
    IDaSoundResource* pResource
)
{
    rAssert( pResource != NULL );

    //
    // Deallocate the resource
    //
    radKey32 soundKey = (radKey32) pResource;

    bool result = m_xIOL_AllocatedResources->RemoveObject( soundKey );
    rAssertMsg( result, "Resource deallocated prematurely?" );
}

//=============================================================================
// Function:    daSoundResourceManager::FindResource
//=============================================================================
// Description: Find a resource.
//
// Parameters:  resourceName - the name of the resource to find
//
// Returns:     Returns a pointer to the appropriate resource or NULL if none
//              found.
//
//-----------------------------------------------------------------------------

IDaSoundResource* daSoundResourceManager::FindResource
(
    daResourceName resourceName
)
{
    IDaSoundResource* pResource;
        
    pResource = reinterpret_cast< IDaSoundResource* >(
        m_pSoundNamespace->GetInstance( resourceName ) );
        
    if( ( pResource == NULL ) && ( m_secondaryNamespace != NULL ) )
    {
        // Search secondary namespace
        pResource = reinterpret_cast< IDaSoundResource* >
        (
            m_secondaryNamespace->GetInstance( resourceName )
        );
    }

    return pResource;
}

//=============================================================================
// Function:    daSoundResourceManager::FindResource
//=============================================================================
// Description: Find a resource by its key.
//
// Parameters:  resourceKey - the key for the resource to find
//
// Returns:     Returns a pointer to the appropriate resource or NULL if none
//              found.
//
//-----------------------------------------------------------------------------

IDaSoundResource* daSoundResourceManager::FindResource
(
    daResourceKey resourceKey
)
{
    IDaSoundResource* pResource;

    pResource = reinterpret_cast< IDaSoundResource* >
    (
        m_pSoundNamespace->GetInstance( resourceKey )
    );
    if( ( pResource == NULL ) && ( m_secondaryNamespace != NULL ) )
    {
        // Search secondary namespace
        pResource = reinterpret_cast< IDaSoundResource* >
        (
            m_secondaryNamespace->GetInstance( resourceKey )
        );
    }

    return pResource;
}

//=============================================================================
// Function:    daSoundResourceManager::FindAllocatedResource
//=============================================================================
// Description: Find an allocated resource
//
// Parameters:  pResource - the resource associated with an allocated resource
//
// Note:        Calling this command on a given resource is not guarenteed
//              to always return the same allocated resource.  In fact,
//              it randomly choose which of the possible file variations
//              it should use.
//
//-----------------------------------------------------------------------------

daSoundAllocatedResource* daSoundResourceManager::FindAllocatedResource
(
    IDaSoundResource* pResource
)
{
    rAssert( pResource != NULL );

    // Generate a key based on the resource
    radKey32 soundKey = (radKey32) pResource;

    // Find the allocated resource for the resource
    daSoundAllocatedResource* pAllocatedResource =
        reinterpret_cast< daSoundAllocatedResource* >
        (
            m_xIOL_AllocatedResources->FindObject( soundKey )
        );

    return pAllocatedResource;
}

//=============================================================================
// Function:    daSoundResourceManager::SetResourceLockdown
//=============================================================================
// Description: Set the lockdown state for resources
//
// Notes:       When the resources are locked down, no new resources
//              can be created.  This is so that the tuner's wiring
//              of the sound system can successively map all available
//              resources to sound groups.
//
//-----------------------------------------------------------------------------

void FlipSlashes( char * pString )
{
    while (*pString != 0 )
    {
        if ( *pString == '/' )
        {
            *pString = '\\';
        }
        pString++;
    }
}

void daSoundResourceManager::SetResourceLockdown( bool lockdown )
{
    // Currently we may only lock down the resources
    rAssert( lockdown == true );
    m_ResourceLockdown = lockdown;

    unsigned int totalFiles = 0;
                        
    for( unsigned int r = 0; r < m_NumResourceDatas; r ++ )
    {
        daSoundResourceData * pRd = this->m_ResourceData + r;
        
        totalFiles += pRd->m_NumFiles;
    }
    
    m_pFileIdMemory = (radKey32*) radMemoryAlloc( GMA_PERSISTENT, sizeof( radKey32 ) * totalFiles );
   
    radKey32* pCurrent = m_pFileIdMemory;
    
    for( unsigned int r = 0; r < MAX_SOUND_DATA_RESOURCES; r ++ )
    {
        daSoundResourceData * pRd = this->m_ResourceData + r;
        
        for( unsigned int f = 0; f < pRd->m_NumFiles; f ++ )
        {
            FlipSlashes( pRd->m_pFileIds[ f ].m_pName );
            
            pCurrent[ f ] = ::radMakeCaseInsensitiveKey32( pRd->m_pFileIds[ f ].m_pName );
            radMemoryFree( pRd->m_pFileIds[ f ].m_pName );
        }

        radMemoryFree( pRd->m_pFileIds );
        pRd->m_pFileIds = (daSoundResourceData::FileId*)pCurrent;
        pCurrent += pRd->m_NumFiles;
    } 
}

//=============================================================================
// Function:    daSoundResourceManager::GetResourceLockdown
//=============================================================================
// Description: Get the lockdown state for resources
//-----------------------------------------------------------------------------

bool daSoundResourceManager::GetResourceLockdown( void )
{
    return m_ResourceLockdown;
}

//=============================================================================
// Function:    daSoundResourceManager::GetLargestFileSize
//=============================================================================
// Description: Get the state of a resource
//
// Parameters:  pResource - the resource to get the state of
//
// Returns:     Returns the state of the resource
//
//-----------------------------------------------------------------------------

unsigned int daSoundResourceManager::GetLargestFileSize
(
    IDaSoundResource* pResource
)
{
    return 0;
}

//=============================================================================
// Function:    daSoundResourceManager::GetTotalSize
//=============================================================================
// Description: Get the state of a resource
//
// Parameters:  pResource - the resource to get the state of
//
// Returns:     Returns the state of the resource
//
//-----------------------------------------------------------------------------

unsigned int daSoundResourceManager::GetTotalSize
(
    IDaSoundResource* pResource
)
{
    return 0;
}

//=============================================================================
// Function:    daSoundResourceManager::GetSoundFileSize
//=============================================================================
// Description: Get debug information from resource manager
//
// Parameters:  pFile - the file to find the size of
//
//-----------------------------------------------------------------------------

unsigned int daSoundResourceManager::GetSoundFileSize
(
    const char* filename
)
{
    // TODO : Don't use this unless it is not synchronous
    rDebugString( "BAD SOUND FUNCTION BEING USED\n" );
    unsigned int fileSize = 0;
    IRadFile* pMyFile = NULL;
    ::radFileOpen( &pMyFile, filename );
    rAssert( pMyFile != NULL );
    pMyFile->GetSizeAsync( &fileSize );
    pMyFile->WaitForCompletion( );
    pMyFile->Release( );
    return fileSize;
}

//=============================================================================
// Function:    daSoundResourceManager::GetNumResources
//=============================================================================
// Description: Get the number of resources
//
//-----------------------------------------------------------------------------

unsigned int daSoundResourceManager::GetNumResourceDatas( void )
{
    return m_NumResourceDatas;
}

//=============================================================================
// Function:    daSoundResourceManager::GetNumAllocatedResources
//=============================================================================
// Description: Get the number of allocated resources
//
//-----------------------------------------------------------------------------

unsigned int daSoundResourceManager::GetNumAllocatedResources( void )
{
    return m_xIOL_AllocatedResources->GetSize( );
}

//=============================================================================
// daSoundResourceManager::SetActiveResource
//=============================================================================
// Description: Set a secondary namespace for us to search for resources.
//              NOTE: a more robust implementation would have us storing a list
//              of these things, but I know for Simpsons that we'll only need
//              one and I want to keep the searches fast
//
// Parameters:  activeNamespace - secondary namespace
//
// Return:      void 
//
//=============================================================================
void daSoundResourceManager::SetActiveResource( IRadNameSpace* activeNamespace )
{
    if( activeNamespace != m_pSoundNamespace )
    {
        m_secondaryNamespace = activeNamespace;
    }
}

//=============================================================================
// daSoundResourceManager::ReleaseActiveResource
//=============================================================================
// Description: Lose our reference to the secondary namespace, probably because
//              the in-game resources are being released.
//
// Parameters:  inactiveNamespace - only used to make sure we aren't releasing
//                                  the main sound namespace
//
// Return:      void 
//
//=============================================================================
void daSoundResourceManager::ReleaseActiveResource( IRadNameSpace* inactiveNamespace )
{
    if( inactiveNamespace != m_pSoundNamespace )
    {
        m_secondaryNamespace = NULL;
    }
}

daSoundResourceData * daSoundResourceManager::GetResourceDataAt( unsigned int i )
{
    rAssert( i < m_NumResourceDatas );
    
    return m_ResourceData + i;
}

daSoundResourceData * daSoundResourceManager::CreateResourceData( void )
{
    daSoundResourceManager * pThis = daSoundResourceManager::GetInstance( );
    
    rAssert( false == pThis->m_ResourceLockdown );
    
    rAssert( pThis->m_NumResourceDatas < MAX_SOUND_DATA_RESOURCES );
     
    daSoundResourceData * pRd = pThis->m_ResourceData + pThis->m_NumResourceDatas;
        
    pThis->m_NumResourceDatas++;
    
    return pRd;
}

} // Sound Namespace

