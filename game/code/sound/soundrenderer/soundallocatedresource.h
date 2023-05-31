//=============================================================================
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundallocatedresource.hpp
//
// Subsystem:   Dark Angel - Sound Resource Management System
//
// Description: Description of an allocated sound resource
//
// Revisions:
//  + Created October 19, 2001 -- breimer
//
//=============================================================================

#ifndef _SOUNDALLOCATEDRESOURCE_HPP
#define _SOUNDALLOCATEDRESOURCE_HPP

//=============================================================================
// Included Files
//=============================================================================

#include <radobject.hpp>
#include <radlinkedclass.hpp>
#include <radsound_hal.hpp>
#include <radsound.hpp>

#include <sound/soundrenderer/idasoundresource.h>
#include <sound/soundrenderer/sounddynaload.h>

//=============================================================================
// Namespace
//=============================================================================

namespace Sound {

//=============================================================================
// Prototypes
//=============================================================================

class daSoundAllocatedResource;

class daSoundFileInstance
{
public:

    inline void * operator new ( size_t size, radMemoryAllocator allocator );
    inline void operator delete( void * pMem );
    
    inline void AddRef( void );
    inline void Release( void );

    // Constructor and destructor
    daSoundFileInstance( IDaSoundResource* pResource, unsigned int fileIndex );
    ~daSoundFileInstance( );

    void Load( IRadSoundHalMemoryRegion* pRegion );
    bool UpdateLoading( void );
    void UnLoad( void );

    inline IDaSoundResource::Type GetType( void );

    inline IRadSoundClip* GetSoundClip( void );
    void CreateFileDataSource( IRadSoundRsdFileDataSource** );

    // Internal state
    enum State { Loading, Loaded, UnLoaded };
    
    inline State GetState( void );
    
    inline void GetFileName( char * pFileName, unsigned int maxChars );
    
protected:

private:

    unsigned int m_FileIndex;
    unsigned int m_RefCount;

    // Store the resource
    IDaSoundResource * m_pResource;

    // Store the actual allocated resource based on the attached
    IDaSoundResource::Type m_Type;

    IRadSoundClip*  m_pSoundClip;

    State m_State;
};

//
// The resource library stores a giant array of resources, and provides helpfull
// ways to get at the information.  Notices that there is no
// way to remove a resource file from this library.
//
class daSoundAllocatedResource : public IRefCount
{

public:
    
    inline void AddRef( void );
    inline void Release( void );
    
    inline void * operator new ( size_t size, radMemoryAllocator allocator );
    inline void operator delete( void * pMem );    

    //
    // Constructor and destructor
    //
    daSoundAllocatedResource( );
    ~daSoundAllocatedResource( );

    //
    // IDaSoundAllocatedResource
    //
    void Initialize( IDaSoundResource* pResource );
    inline IDaSoundResource* GetResource( void );
    unsigned int ChooseNextInstance( void );

    daSoundFileInstance* GetFileInstance( unsigned int index );

protected:
    void ApplyResourceSettings_Internal( unsigned int index );

private:
    // Store the attached resource
    IDaSoundResource*               m_pResource;
    
    unsigned int m_RefCount;

    // Store the file instances
    daSoundFileInstance*            m_pFileInstance[ DASound_MaxNumSoundResourceFiles ];

    // Dynamic loading region
    daSoundDynaLoadRegion*         m_pDynaLoadRegion[ DASound_MaxNumSoundResourceFiles ];
};

//=============================================================================
// Function:    daSoundFileInstance::GetType
//=============================================================================
// Description: Get the type of the allocated res
//
//-----------------------------------------------------------------------------

inline IDaSoundResource::Type daSoundFileInstance::GetType( void )
{
    return m_Type;
}


//=============================================================================
// Function:    daSoundFileInstance::GetSoundClip
//=============================================================================
// Description: Get the sound clip.
//
// Returns:     Returns the sound clip if it is allocated for this resource,
//              or NULL if it is not.
//
//-----------------------------------------------------------------------------

inline IRadSoundClip* daSoundFileInstance::GetSoundClip( void )
{
    rAssert( IDaSoundResource::CLIP == GetType( ) );
    rAssert( Loaded == m_State );

    return m_pSoundClip;
}


//=============================================================================
// Function:    daSoundAllocatedResource::GetResource
//=============================================================================
// Description: Get the resource data from the allocated resource
//
//-----------------------------------------------------------------------------

IDaSoundResource* daSoundAllocatedResource::GetResource( void )
{
    return m_pResource;
}

inline daSoundFileInstance::State daSoundFileInstance::GetState( void )
{
    return m_State;
}

inline void daSoundFileInstance::GetFileName( char * pBuffer, unsigned int max )
{
    m_pResource->GetFileKeyAt( m_FileIndex, pBuffer, max );
}


inline void daSoundFileInstance::AddRef( void )
{
    m_RefCount++;
}

inline void daSoundFileInstance::Release( void )
{
    rAssert( m_RefCount > 0 );
    
    m_RefCount--;
    
    if( 0 == m_RefCount )
    {
        delete this;
    }
}


inline void * daSoundFileInstance::operator new ( size_t size, radMemoryAllocator allocator )
{
    return radMemoryAlloc( allocator, size );
}

inline void daSoundFileInstance::operator delete( void * pMem )
{
    return radMemoryFree( pMem );
}

inline void daSoundAllocatedResource::AddRef( void )
{
    m_RefCount++;
}

inline void daSoundAllocatedResource::Release( void )
{   
    rAssert( m_RefCount > 0 );
    
    m_RefCount--;
    
    if( 0 == m_RefCount )
    {
        delete this;
    }
}

inline void * daSoundAllocatedResource::operator new ( size_t size, radMemoryAllocator allocator )
{
    return radMemoryAlloc( allocator, size );
}

inline void daSoundAllocatedResource::operator delete( void * pMem )
{
    return radMemoryFree( pMem );
}
                
} // Sound Namespace
#endif //_SOUNDALLOCATEDRESOURCE_HPP

