//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundcluster.cpp
//
// Description: Implement SoundCluster
//
// History:     26/06/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <radnamespace.hpp>

//========================================
// Project Includes
//========================================
#include <sound/soundcluster.h>

#include <sound/soundrenderer/idasoundresource.h>
#include <sound/soundrenderer/soundrenderingmanager.h>
#include <sound/soundrenderer/soundresourcemanager.h>

#include <loading/soundfilehandler.h>
#include <memory/srrmemory.h>


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

static const radKey32 NULL_SOUND_KEY = 0;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// SoundCluster::SoundCluster
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SoundCluster::SoundCluster( int clusterIndex, 
                            IRadNameSpace* soundNamespace ) :
    m_isLoaded( false ),
    m_namespace( soundNamespace ),
    m_loadCompleteCallbackObj( NULL )
{
    int i;
    
    //
    // Initialize the sound list to zeroes, which is hopefully an unlikely radKey32 value
    //
    for( i = 0; i < MAX_RESOURCES; i++ )
    {
        m_soundList[i] = NULL_SOUND_KEY;
    }

#ifdef RAD_DEBUG
    m_clusterIndex = clusterIndex;
#endif
}

//==============================================================================
// SoundCluster::~SoundCluster
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SoundCluster::~SoundCluster()
{
}

//=============================================================================
// SoundCluster::LoadSounds
//=============================================================================
// Description: Loads the sounds listed in m_soundList
//
// Parameters:  none
//
// Return:      void 
//
//=============================================================================
void SoundCluster::LoadSounds( SoundFileHandler* callbackObj /* = NULL  */ )
{
    IDaSoundResource* resource;
    int i;
    
    MEMTRACK_PUSH_GROUP( "Sound" );

    for( i = 0; i < MAX_RESOURCES; i++ )
    {
        if( m_soundList[i] != NULL_SOUND_KEY )
        {
            resource = reinterpret_cast< IDaSoundResource* >
            (
                m_namespace->GetInstance( m_soundList[i] )
            );

            if( resource != NULL )
            {
                resource->CaptureResource();
            }
#ifdef RAD_DEBUG
            else
            {
                //
                // Sound not found, spew debug message
                //
                rDebugPrintf( "Sound resource #%d couldn't be found in cluster #%d\n", i, m_clusterIndex );
            }
#endif
        }
    }

    //
    // Register for notification on load completion
    //
    Sound::daSoundRenderingManagerGet()->GetDynaLoadManager()->AddCompletionCallback( this, NULL );

    m_loadCompleteCallbackObj = callbackObj;

    //
    // Tell the resource manager that we're using this namespace
    //
    Sound::daSoundRenderingManagerGet()->GetResourceManager()->SetActiveResource( m_namespace );

    MEMTRACK_POP_GROUP( "Sound" );
}

//=============================================================================
// SoundCluster::UnloadSounds
//=============================================================================
// Description: Unloads the sounds listed in m_soundList
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundCluster::UnloadSounds()
{
    IDaSoundResource* resource;
    int i;

    for( i = 0; i < MAX_RESOURCES; i++ )
    {
        if( m_soundList[i] != NULL_SOUND_KEY )
        {
            resource = reinterpret_cast< IDaSoundResource* >
            (
                m_namespace->GetInstance( m_soundList[i] )
            );

            if( resource != NULL )
            {
                resource->ReleaseResource();
            }
#ifdef RAD_DEBUG
            else
            {
                //
                // Sound not found, spew debug message
                //
                rDebugPrintf( "Tried to free unloaded sound resource #%d in cluster #%d\n", i, m_clusterIndex );
            }
#endif
        }
    }

    //
    // Tell the resource manager that we're no longer using this namespace
    //
    Sound::daSoundRenderingManagerGet()->GetResourceManager()->ReleaseActiveResource( m_namespace );


    m_isLoaded = false;
}

//=============================================================================
// SoundCluster::AddResource
//=============================================================================
// Description: Add given sound resource to the list of resources associated
//              with this cluster
//
// Parameters:  resourceKey - radKey32 crunched from sound resource name
//
// Return:      true if we could add it to the list, false otherwise
//
//=============================================================================
bool SoundCluster::AddResource( Sound::daResourceKey resourceKey )
{
    int i;
    bool added = false;

    for( i = 0; i < MAX_RESOURCES; i++ )
    {
        if( m_soundList[i] == NULL_SOUND_KEY )
        {
            m_soundList[i] = resourceKey;
            added = true;
            break;
        }
    }

    return( added );
}

//=============================================================================
// SoundCluster::ContainsResource
//=============================================================================
// Description: Search the sound list and return a bool indicating whether
//              this sound cluster contains the indicated resource
//
// Parameters:  resourceKey - hashed name of resource to search for
//
// Return:      true if resource present, false otherwise
//
//=============================================================================
bool SoundCluster::ContainsResource( Sound::daResourceKey resourceKey )
{
    int i;

    rAssert( resourceKey != NULL_SOUND_KEY );

    for( i = 0; i < MAX_RESOURCES; i++ )
    {
        if( m_soundList[i] == resourceKey )
        {
            return( true );
        }
    }

    return( false );
}

//=============================================================================
// SoundCluster::OnDynaLoadOperationsComplete
//=============================================================================
// Description: Called when the sound renderer is finished loading the cluster
//
// Parameters:  pUserData - user data, unused
//
// Return:      void 
//
//=============================================================================
void SoundCluster::OnDynaLoadOperationsComplete( void* pUserData )
{
    m_isLoaded = true;
    if( m_loadCompleteCallbackObj != NULL )
    {
        m_loadCompleteCallbackObj->LoadCompleted();
        m_loadCompleteCallbackObj = NULL;
    }
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
