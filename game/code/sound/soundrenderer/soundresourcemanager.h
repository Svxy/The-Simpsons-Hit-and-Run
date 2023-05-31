//=============================================================================
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundresourcemanager.hpp
//
// Subsystem:   Dark Angel - Sound Resource Management System
//
// Description: Description of the DA sound resource manager
//
// Revisions:
//  + Created October 11, 2001 -- breimer
//
//=============================================================================

#ifndef _SOUNDRESOURCEMANAGER_HPP
#define _SOUNDRESOURCEMANAGER_HPP

//=============================================================================
// Included Files
//=============================================================================

#include <radobject.hpp>

#include <sound/soundrenderer/idasoundresource.h>
#include <sound/soundrenderer/soundresource.h>

//=============================================================================
// Global namespace forward declarations
//=============================================================================

class radObjectBTree;

//=============================================================================
// Namespace
//=============================================================================

namespace Sound {

const unsigned int MAX_SOUND_DATA_RESOURCES = 5000;

//=============================================================================
// Prototypes
//=============================================================================

class daSoundResourceManager;

//=============================================================================
// Forward declarations
//=============================================================================

class daSoundAllocatedResource;

//=============================================================================
// Class Declarations
//=============================================================================

//
// The sound player manager passes information into the various sound
// players.  It also keeps track of these players, gives them to people
// who ask for them, and makes sure they do not cause to much trouble :)
//
class daSoundResourceManager : public IRefCount,
                               public radRefCount
{
public:
    IMPLEMENT_REFCOUNTED( "daSoundResourceManager" );

    //
    // Constructor and destructor
    //
    daSoundResourceManager( void );
    
    virtual ~daSoundResourceManager( );

    inline static daSoundResourceManager* GetInstance( void );

    // Controlled by the resource data
    void AllocateResource( IDaSoundResource* pResource );
    void DeallocateResource( IDaSoundResource* pResource );

    // Resource lockdown
    void SetResourceLockdown( bool lockdown );

    // Allocated resources
    daSoundAllocatedResource* FindAllocatedResource
    (
        IDaSoundResource* pResource
    );

    // Get a sound file's size
    unsigned int GetSoundFileSize
    (
        const char* filename
    );

    //
    // IDaSoundResourceManager
    //
    IDaSoundResource* FindResource(
        daResourceName resourceName );
        
    IDaSoundResource* FindResource(
        daResourceKey resourceKey );

    unsigned int GetLargestFileSize( IDaSoundResource* pResource );
    unsigned int GetTotalSize( IDaSoundResource* pResource );


    bool GetResourceLockdown( void );

    void SetActiveResource( IRadNameSpace* activeNamespace );
    void ReleaseActiveResource( IRadNameSpace* inactiveNamespace );
    
    unsigned int GetNumResourceDatas( void );
    daSoundResourceData* GetResourceDataAt( unsigned int );
    static daSoundResourceData* CreateResourceData( void );
    
protected:
    //
    // Calculate some debug info
    //

    unsigned int GetNumAllocatedResources( );

private:
    // This is a singleton
    static daSoundResourceManager*          s_pSingleton;

    //
    // Store the sound namespace
    //
    IRadNameSpace*                          m_pSoundNamespace;

    //
    // Store all allocated resources (referenced by the resource's
    // address cast to a radkey)
    //
    ref< radObjectBTree >                  m_xIOL_AllocatedResources;

    //
    // Are the resources locked down?
    //
    bool                                    m_ResourceLockdown;

    //
    // Store active secondary namespace
    //
    IRadNameSpace* m_secondaryNamespace;
    
    daSoundResourceData m_ResourceData[ MAX_SOUND_DATA_RESOURCES ];
    unsigned int m_NumResourceDatas;
    
    radKey32 * m_pFileIdMemory;
};

inline daSoundResourceManager* daSoundResourceManager::GetInstance( void )
{
    return s_pSingleton;
}

} // Sound Namespace
#endif //_SOUNDRESOURCEMANAGER_HPP


