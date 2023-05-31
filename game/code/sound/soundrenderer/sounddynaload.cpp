//=============================================================================
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// File:        sounddynaload.cpp
//
// Subsystem:   Dark Angel - Dynamic Loading System
//
// Description: Implementation of the DA Dynamic Sound Loading System
//
// Revisions:
//  + Created: Novemeber 22, 2001 -- breimer
//
//=============================================================================

//=============================================================================
// Included Files
//=============================================================================

#include <radobject.hpp>
#include <raddebug.hpp>
#include <radkey.hpp>
#include <radsound.hpp>

#include <sound/soundrenderer/sounddynaload.h>
#include <sound/soundrenderer/soundallocatedresource.h>

//=============================================================================
// Static Variables (outside namespace)
//=============================================================================

Sound::daSoundDynaLoadRegion* radLinkedClass< Sound::daSoundDynaLoadRegion >::s_pLinkedClassHead = NULL;
Sound::daSoundDynaLoadRegion* radLinkedClass< Sound::daSoundDynaLoadRegion >::s_pLinkedClassTail = NULL;

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
#define DASOUNDDYNALOAD_DEBUG
#ifdef DASOUNDDYNALOAD_DEBUG

// Show the creation and destruction of dynamic loading regions
static bool sg_ShowDynaLoadRegionCreation = false;

#endif //DASOUNDDYNALOAD_DEBUG
#endif //NDEBUG
#endif //FINAL

//=============================================================================
// Static Variables
//=============================================================================

daSoundDynaLoadManager* daSoundDynaLoadManager::s_pSingleton = NULL;
daSoundDynaLoadRegion* daSoundDynaLoadRegion::s_pActiveRegion = NULL;
unsigned int daSoundDynaLoadRegion::s_ActiveSlot = 0;
unsigned int daSoundDynaLoadRegion::s_GlobalPendingSwapCount = 0;

//=============================================================================
// Prototypes
//=============================================================================

class daSoundFileInstance;
class daSoundDynaLoadRegion;

//=============================================================================
// Class Implementation
//=============================================================================

//=============================================================================
// daSoundDynaLoadRegion Implementation
//=============================================================================

//=============================================================================
// Function:    daSoundDynaLoadRegion::daSoundDynaLoadRegion
//=============================================================================
// Description: Constructor.
//
//-----------------------------------------------------------------------------

daSoundDynaLoadRegion::daSoundDynaLoadRegion( )
:
radRefCount( 0 ),
m_IsInitialized( false ),
m_NumSlots( 0 ),
m_SlotSize( 0 ),
m_ppSlot( NULL ),
m_ppSlotObjects( NULL ),
m_ppPendingSwapObjects( NULL ),
m_PendingSwapCount( 0 )
{
    //
}

//=============================================================================
// Function:    daSoundDynaLoadRegion::~daSoundDynaLoadRegion
//=============================================================================
// Description: Destructor.
//
//-----------------------------------------------------------------------------

daSoundDynaLoadRegion::~daSoundDynaLoadRegion( )
{
    // Destroy everything
    Destroy( );
}

//=============================================================================
// Function:    daSoundDynaLoadRegion::~daSoundDynaLoadRegion
//=============================================================================
// Description: Create the region.  If the size of slots is zero, do
//              not preallocate the region memory.
//
//-----------------------------------------------------------------------------

void daSoundDynaLoadRegion::Create
(
    IRadSoundHalMemoryRegion* pMemRegion,
    unsigned int sizeofslots,
    unsigned int numslots
)
{
    rAssert( pMemRegion != NULL );
    rAssert( !m_IsInitialized );
    rAssert( !ArePendingSwapsRegistered( ) );

    // Set the region information
    m_SlotSize = sizeofslots;
    m_NumSlots = numslots;

    // Create the slots
    m_ppSlot = reinterpret_cast< IRadSoundHalMemoryRegion** >
    (
        ::radMemoryAlloc
        (
            GetThisAllocator( ),
            sizeof( IRadSoundHalMemoryRegion* ) * numslots
        )
    );
    rAssert( m_ppSlot );

    unsigned int i = 0;
    for( i = 0; i < numslots; i++ )
    {
        // If the slot size is zero, just use the main memory
        if( SharedMemoryRegions( ) )
        {
            m_ppSlot[ i ] = pMemRegion;
            m_ppSlot[ i ]->AddRef( );
        }
        else
        {
            //
            // ESAN TODO: Investigate the magic number 32 below...
            //
            m_ppSlot[ i ] = pMemRegion->CreateChildRegion( m_SlotSize, 32, "Sound Memory Region object" );
            if( m_ppSlot[ i ] == NULL )
            {
                // If this occurs, there in the check for free space.  This may
                // occur if the slot size is not aligned in the same way as
                // sound memory.
                rDebugString( "Out of sound memory allocating region\n" );
                rAssert( m_ppSlot[ i ] != NULL );
            }
            else
            {
                m_ppSlot[ i ]->AddRef( );
            }
        }
        rAssert( m_ppSlot[ i ] != NULL );
        rAssert( m_ppSlot[ i ]->GetSize( ) >= m_SlotSize );
    }

    // Create the slot objects and pending slot objects
    m_ppSlotObjects = reinterpret_cast< daSoundFileInstance** >
    (
        ::radMemoryAlloc
        (
            GetThisAllocator( ),
            sizeof( daSoundFileInstance* ) * numslots
        )
    );
    m_ppPendingSwapObjects = reinterpret_cast< daSoundFileInstance** >
    (
        ::radMemoryAlloc
        (
            GetThisAllocator( ),
            sizeof( daSoundFileInstance* ) * numslots
        )
    );
    for( i = 0; i < numslots; i++ )
    {
        m_ppSlotObjects[ i ] = NULL;
        m_ppPendingSwapObjects[ i ] = NULL;
    }

    m_IsInitialized = true;
}

//=============================================================================
// Function:    daSoundDynaLoadRegion::Destroy
//=============================================================================
// Description: Destroy the region.
//
//-----------------------------------------------------------------------------

void daSoundDynaLoadRegion::Destroy( void )
{
    if( m_IsInitialized )
    {
        //
        // Swap out all the objects
        //
        unsigned int i = 0;
        for( i = 0; i < m_NumSlots; i++ )
        {
            SwapInObject( i, NULL );
        }

        // Destroy the pending swap objects
        if( m_ppPendingSwapObjects != NULL )
        {
            ::radMemoryFree( GetThisAllocator( ), m_ppPendingSwapObjects );
            m_ppPendingSwapObjects = NULL;
        }
        
        // Destroy the memory objects
        if( m_ppSlotObjects != NULL )
        {
            ::radMemoryFree( GetThisAllocator( ), m_ppSlotObjects );
            m_ppSlotObjects = NULL;
        }

        // Destroy the memory regions
        if( m_ppSlot != NULL )
        {
            for( i = 0; i < m_NumSlots; i++ )
            {
                rAssert( m_ppSlot[ i ] != NULL );
                m_ppSlot[ i ]->Release( );
                m_ppSlot[ i ] = NULL;
            }
            ::radMemoryFree( GetThisAllocator( ), m_ppSlot );
            m_ppSlot = NULL;
        }

        // Make sure we are not the active swap
        ClearActiveSwap( );
    }
}

//=============================================================================
// Function:    daSoundDynaLoadRegion::ServiceOncePerFrame
//=============================================================================
// Description: Service the sound system once per frame.
//
//-----------------------------------------------------------------------------

void daSoundDynaLoadRegion::ServiceOncePerFrame( void )
{
    // Service any active swaps
    if( s_pActiveRegion == this )
    {
        if( GetSlotState( s_ActiveSlot ) != Initializing )
        {
            ClearActiveSwap( );
        }
    }
    else
    {
        // Make sure that any pending swaps take place
        if( ArePendingSwapsRegistered( ) )
        {
            unsigned int i = 0;
            for( i = 0; i < GetNumSlots( ); i++ )
            {
                daSoundFileInstance* pObject = GetPendingSwapObject( i );
                if( pObject != NULL )
                {
                    daSoundDynaLoadRegion::SlotState state = GetSlotState( i );
                    if( state == Empty ) 
                    {
                        PerformSwap( i );
                    }
                }
            }
        }
    }
}

//=============================================================================
// Function:    daSoundDynaLoadRegion::SwapInObject
//=============================================================================
// Description: Swap in a sound object.
//
// Parameters:  slot - the slot number to swap a sound into
//              pObject - a dynamic loading object
//
//-----------------------------------------------------------------------------

void daSoundDynaLoadRegion::SwapInObject
(
    unsigned int slot,
    daSoundFileInstance* pObject
)
{
    // Destroy any pending swap object
    daSoundFileInstance* pOldObject = GetPendingSwapObject( slot );
    if( pOldObject != NULL )
    {
        SetPendingSwapObject( slot, NULL );
        ClearActiveSwap( );
    }

    // Destroy the old object
    pOldObject = GetSlotObject( slot );
    if( pOldObject != NULL )
    {
        pOldObject->UnLoad( );
        SetSlotObject( slot, NULL );
    }

    // Set the pending swap object
    SetPendingSwapObject( slot, pObject );

    // The swap will occur asynchronously in ServiceOncePerFrame( )
}

//=============================================================================
// Function:    daSoundDynaLoadRegion::GetSlotState
//=============================================================================
// Description: Get the state of a slot
//
// Parameters:  slot - the slot number to check
//
// Return:      Returns the state of a slot
//
//-----------------------------------------------------------------------------

daSoundDynaLoadRegion::SlotState daSoundDynaLoadRegion::GetSlotState
(
    unsigned int slot
)
{
    rAssert( m_IsInitialized );
    rAssert( m_ppSlot != NULL );
    rAssert( slot < m_NumSlots );
    rAssert( GetSlotMemoryRegion( slot ) != NULL );
    
    bool slotHasObject = true;
    bool slotHasAllocation = true;

    unsigned int numobjs = 0;
    GetSlotMemoryRegion( slot )->GetStats( NULL, &numobjs, NULL, false );
    slotHasAllocation = ( ( numobjs != 0 ) && ( !SharedMemoryRegions( ) ) );

    daSoundFileInstance* pObject = GetSlotObject( slot );
    // If we do not have our own memory regions, we assume that the object has already disapeared.
    slotHasObject = ( pObject != NULL );

    // Determine the state
    daSoundDynaLoadRegion::SlotState state = Empty;
    if( slotHasObject )
    {
        rAssert( pObject != NULL );

        if( pObject->UpdateLoading( ) )
        {
            state = Ready;
        }
        else
        {
           state = Initializing;
        }
    }
    else
    {
        if( slotHasAllocation )
        {
            state = Destroying;
        }
        else
        {
            state = Empty;
        }
    }

    return state;
}

//=============================================================================
// Function:    daSoundDynaLoadRegion::GetNumSlots
//=============================================================================
// Description: Get the number of slots in this region
//
// Return:      Returns the number of slots
//
//-----------------------------------------------------------------------------

unsigned int daSoundDynaLoadRegion::GetNumSlots( void )
{
    rAssert( m_IsInitialized );
    return m_NumSlots;
}

//=============================================================================
// Function:    daSoundDynaLoadRegion::GetSlotSize
//=============================================================================
// Description: Get the size of the slots in this dynaload region
//
// Return:      Returns the size of the slots
//
//-----------------------------------------------------------------------------

unsigned int daSoundDynaLoadRegion::GetSlotSize( void )
{
    rAssert( m_IsInitialized );
    return m_SlotSize;
}

//=============================================================================
// Function:    daSoundDynaLoadRegion::SharedMemoryRegions
//=============================================================================
// Description: Are the memory regions shared, or does each slot have its own?
//
// Return:      Returns true if the memory regions are shared.
//
//-----------------------------------------------------------------------------

bool daSoundDynaLoadRegion::SharedMemoryRegions( void )
{
    return( m_SlotSize == 0 );
}

//=============================================================================
// Function:    daSoundDynaLoadRegion::PerformSwap
//=============================================================================
// Description: Do an actual swap into a memory slot.  The slot
//              should be already verified as empty.
//
//-----------------------------------------------------------------------------

void daSoundDynaLoadRegion::PerformSwap
(
    unsigned int slot
)
{
    bool result = SetActiveSwap( slot );
    if( result )
    {
        rAssert( GetSlotState( slot ) == Empty );

        // Swap in the object
        daSoundFileInstance* pObject = GetPendingSwapObject( slot );
        rAssert( pObject != NULL );
        pObject->AddRef( );
        SetSlotObject( slot, pObject );
        SetPendingSwapObject( slot, NULL );

        // Tell it to create itself
        IRadSoundHalMemoryRegion* pRegion = GetSlotMemoryRegion( slot );
        rAssert( pRegion != NULL );
        pObject->Load( pRegion );

        pObject->Release( );
    }
}

//=============================================================================
// Function:    daSoundDynaLoadRegion::GetSlotMemoryRegion
//=============================================================================
// Description: Get the slot memory region
//
// Return:      Returns the size of the slots
//
//-----------------------------------------------------------------------------

IRadSoundHalMemoryRegion* daSoundDynaLoadRegion::GetSlotMemoryRegion
(
    unsigned int slot
)
{
    rAssert( m_IsInitialized );
    rAssert( m_ppSlot != NULL );
    rAssert( slot < m_NumSlots );

    return( m_ppSlot[ slot ] );
}

//=============================================================================
// Function:    daSoundDynaLoadRegion::SetSlotObject
//=============================================================================
// Description: Set a slot's object
//
// Parameters:  slot - the slot whose object is to be set
//              pObject - the object to place in the slot
//
//-----------------------------------------------------------------------------

void daSoundDynaLoadRegion::SetSlotObject
(
    unsigned int slot,
    daSoundFileInstance* pObject
)
{
    SetObject_Internal( m_ppSlotObjects, slot, pObject );
}

//=============================================================================
// Function:    daSoundDynaLoadRegion::GetSlotObject
//=============================================================================
// Description: Get a slot's object
//
// Return:      Returns the sound object
//
//-----------------------------------------------------------------------------

daSoundFileInstance* daSoundDynaLoadRegion::GetSlotObject
(
    unsigned int slot
)
{
    return GetObject_Internal( m_ppSlotObjects, slot );
}

//=============================================================================
// Function:    daSoundDynaLoadRegion::SetPendingSwapObject
//=============================================================================
// Description: Set a pending swap ovject for a given slot
//
// Parameters:  slot - the slot whose object is to be set
//              pObject - the object to place in the slot
//
//-----------------------------------------------------------------------------

void daSoundDynaLoadRegion::SetPendingSwapObject
(
    unsigned int slot,
    daSoundFileInstance* pObject
)
{
    if( GetPendingSwapObject( slot ) != NULL )
    {
        --m_PendingSwapCount;
        --s_GlobalPendingSwapCount;
    }
    if( pObject != NULL )
    {
        ++m_PendingSwapCount;
        ++s_GlobalPendingSwapCount;
    }
    SetObject_Internal( m_ppPendingSwapObjects, slot, pObject );
}

//=============================================================================
// Function:    daSoundDynaLoadRegion::ArePendingSwapsRegistered
//=============================================================================
// Description: Returns true if there are still pending swaps registered
//
//-----------------------------------------------------------------------------

bool daSoundDynaLoadRegion::ArePendingSwapsRegistered( void )
{
    return( m_PendingSwapCount > 0 );
}

//=============================================================================
// Function:    daSoundDynaLoadRegion::ArePendingSwapsRegistered
//=============================================================================
// Description: Returns true if there are still pending swaps registered
//
//-----------------------------------------------------------------------------

unsigned int  daSoundDynaLoadRegion::GetNumPendingSwaps( void )
{
    return( s_GlobalPendingSwapCount );
}

//=============================================================================
// Function:    daSoundDynaLoadRegion::GetPendingSwapObject
//=============================================================================
// Description: Get a pending swap object for a given slot
//
// Return:      Returns the sound object
//
//-----------------------------------------------------------------------------

daSoundFileInstance* daSoundDynaLoadRegion::GetPendingSwapObject
(
    unsigned int slot
)
{
    return GetObject_Internal( m_ppPendingSwapObjects, slot );
}

//=============================================================================
// Function:    daSoundDynaLoadRegion::SetObject_Internal
//=============================================================================
// Description: Set an object in an object array
//
// Parameters:  ppObjects - the object array
//              slot - the slot whose object is to be set
//              pObject - the object to place in the slot
//
//-----------------------------------------------------------------------------

void daSoundDynaLoadRegion::SetObject_Internal
(
    daSoundFileInstance** ppObjects,
    unsigned int slot,
    daSoundFileInstance* pObject
)
{
    rAssert( m_IsInitialized );
    rAssert( ppObjects != NULL );
    rAssert( slot < m_NumSlots );

    // Under the current usage of this class, we cannot set a slot object
    // to anything but NULL if an object already exists.
    daSoundFileInstance* pOldObject = GetObject_Internal
    (
        ppObjects,
        slot
    );
    rAssert( (pObject == NULL ) || ( pOldObject == NULL ) );
    if( pOldObject != pObject )
    {
        // Out with the old
        if( pOldObject != NULL )
        {
            pOldObject->Release( );
            pOldObject = NULL;
        }

        // In with the new
        ppObjects[ slot ] = pObject;
        rAssert( GetObject_Internal( ppObjects, slot ) == pObject );
        if( ppObjects[ slot ] != NULL )
        {
            ppObjects[ slot ]->AddRef( );
        }
    }
}

//=============================================================================
// Function:    daSoundDynaLoadRegion::GetObject_Internal
//=============================================================================
// Description: Get an object in an object array
//
// Return:      Returns the sound object
//
//-----------------------------------------------------------------------------

daSoundFileInstance* daSoundDynaLoadRegion::GetObject_Internal
(
    daSoundFileInstance** ppObjects,
    unsigned int slot
)
{
    rAssert( m_IsInitialized );
    rAssert( ppObjects != NULL );
    rAssert( slot < m_NumSlots );

    return( ppObjects[ slot ] );
}

//=============================================================================
// Function:    daSoundDynaLoadRegion::ClearActiveSwap
//=============================================================================
// Description: Clear the active swap region
//
//-----------------------------------------------------------------------------

void daSoundDynaLoadRegion::ClearActiveSwap( void )
{
    if( s_pActiveRegion == this )
    {
        s_pActiveRegion = NULL;
    }
}

//=============================================================================
// Function:    daSoundDynaLoadRegion::SetActiveSwap
//=============================================================================
// Description: Set the active swap to this object.  This is used
//              to help serialize dynamic loading.
//
// Returns:     Returns true if the active swap region has been set to
//              this.
//
//-----------------------------------------------------------------------------

bool daSoundDynaLoadRegion::SetActiveSwap( unsigned int slot )
{
    if( s_pActiveRegion == NULL )
    {
        s_pActiveRegion = this;
        s_ActiveSlot = slot;

        return true;
    }
    return false;
}


//=============================================================================
// daSoundDynaLoadManager Implementation
//=============================================================================

//=============================================================================
// Function:    daSoundDynaLoadManager::daSoundDynaLoadManager
//=============================================================================
// Description: Constructor
//
//-----------------------------------------------------------------------------

daSoundDynaLoadManager::daSoundDynaLoadManager( )
    :
    radRefCount( 0 ),
    m_pCompletionCallback( NULL ),
    m_pCompletionUserData( NULL )
{
    // Set the singleton
    rAssert( s_pSingleton == NULL );
    s_pSingleton = this;
}

//=============================================================================
// Function:    daSoundDynaLoadManager:~daSoundDynaLoadManager
//=============================================================================
// Description: Destructor
//
//-----------------------------------------------------------------------------

daSoundDynaLoadManager::~daSoundDynaLoadManager( )
{
    // Remove the singleton
    rAssert( s_pSingleton != NULL );
    s_pSingleton = NULL;

    // Assert that there is no pending completion callback
    rAssert( m_pCompletionCallback == NULL );
}

//=============================================================================
// Function:    daSoundDynaLoadManager::GetInstance
//=============================================================================
// Description: Get the singleton instance of the load manager
//
//-----------------------------------------------------------------------------

daSoundDynaLoadManager* daSoundDynaLoadManager::GetInstance( void )
{
    return s_pSingleton;
}

//=============================================================================
// Function:    daSoundDynaLoadManager::ServiceOncePerFrame
//=============================================================================
// Description: Service the load manager.
//
//-----------------------------------------------------------------------------

void daSoundDynaLoadManager::ServiceOncePerFrame( void )
{
    IDaSoundDynaLoadCompletionCallback* callback;

    // Service each of the regions
    daSoundDynaLoadRegion* pDynaLoadRegion =
        daSoundDynaLoadRegion::GetLinkedClassHead( );
    while( pDynaLoadRegion != NULL )
    {
        pDynaLoadRegion->ServiceOncePerFrame( );
        pDynaLoadRegion = pDynaLoadRegion->GetLinkedClassNext( );
    }

    // Call any completion callbacks
    if
    (
        ( m_pCompletionCallback != NULL ) &&
        ( daSoundDynaLoadRegion::GetNumPendingSwaps( ) == 0 )
    )
    {
        //
        // Store the callback separately before using, since the callback
        // may lead to another sound load
        //
        callback = m_pCompletionCallback;
        m_pCompletionCallback = NULL;
        m_pCompletionUserData = NULL;

        callback->OnDynaLoadOperationsComplete
        (
            m_pCompletionUserData
        );

        callback->Release();
    }
}

//=============================================================================
// Function:    daSoundDynaLoadManager::CreateRegion
//=============================================================================
// Description: Create a dynamic loading region
//
// Parameters:  pMemRegion - the sound region to divide up so that
//                           the given slots may be created.
//              sizeofslots - the size of the dynamic loading slots
//              numslots - the number of dynamic loading slots
//
//-----------------------------------------------------------------------------

daSoundDynaLoadRegion* daSoundDynaLoadManager::CreateRegion
(
    IRadSoundHalMemoryRegion* pMemRegion,
    unsigned int sizeofslots,
    unsigned int numslots
)
{
    rAssert( pMemRegion != NULL );
#ifdef DASOUNDDYNALOAD_DEBUG
    if( sg_ShowDynaLoadRegionCreation )
    {
        rReleasePrintf
        (
            "CreateRegion( %#x, sizeofslots=%u, numslots=%u )\n",
            pMemRegion,
            sizeofslots,
            numslots,
            0
        );
    }
#endif //DASOUNDDYNALOAD_DEBUG

    // Make sure the new region fits into memory
    unsigned int freeSpace = 0;
    pMemRegion->GetStats( NULL, NULL, &freeSpace, false );
    daSoundDynaLoadRegion* pDynaRegion = NULL;
    if( freeSpace >= sizeofslots * numslots )
    {
        // Create the memory region
        pDynaRegion = new( GetThisAllocator( ) ) daSoundDynaLoadRegion( );
        rAssert( pDynaRegion != NULL );
        pDynaRegion->Create( pMemRegion, sizeofslots, numslots );
    }
    else
    {
        // Out of memory!
        // If this occurs during the game, your loading stratagies must be
        // reconsidered.  If it happens while setting up sounds for
        // the game then the choices of resident sound files must
        // be looked at, or more sound memory should be allocated.
        rDebugString( "Out of sound memory trying to create sound region\n" );
        rAssert( 0 );
    }

    return pDynaRegion;
}

//=============================================================================
// Function:    daSoundDynaLoadManager::CreateRegionFromTotalSpace
//=============================================================================
// Description: Create a dynamic loading region when given a total amount of
//              space.
//
// Parameters:  pMemRegion - the sound region to divide up so that
//                           the given slots may be created.
//              sizeofslots - the size of the dynamic loading slots
//
// Note:        The actual amount of space used will be aligned down by the
//              size of slots.
//
//-----------------------------------------------------------------------------

daSoundDynaLoadRegion* daSoundDynaLoadManager::CreateRegionFromTotalSpace
(
    IRadSoundHalMemoryRegion* pMemRegion,
    unsigned int sizeofslots
)
{
    rAssert( pMemRegion != NULL );

    // How much free space is there?
    unsigned int freeSpace = 0;
    pMemRegion->GetStats( NULL, NULL, &freeSpace, false );

    // How many slots can we make
    unsigned int numslots = freeSpace / sizeofslots;

    // Make the slots
    daSoundDynaLoadRegion* pDynaRegion = NULL;
    if( numslots > 0 )
    {
        pDynaRegion = CreateRegion( pMemRegion, sizeofslots, numslots );
    }

    return pDynaRegion;
}

//=============================================================================
// Function:    daSoundDynaLoadManager::AddCompletionCallback
//=============================================================================
// Description: Add a completion callback
//
//-----------------------------------------------------------------------------

void daSoundDynaLoadManager::AddCompletionCallback
(
    IDaSoundDynaLoadCompletionCallback* pCallback,
    void* pUserData
)
{
    if( m_pCompletionCallback != NULL )
    {
        rDebugString( "Cannot add a completion callback while one is\n" );
        rDebugString( "pending using current sounddynamic loading manager.\n" );
        rAssert( 0 );

        m_pCompletionCallback->Release( );
        m_pCompletionCallback = NULL;
    }

    m_pCompletionCallback = pCallback;
    m_pCompletionUserData = pUserData;

    if( m_pCompletionCallback != NULL )
    {
        m_pCompletionCallback->AddRef( );
    }
}

//=============================================================================
// Function:    daSoundDynaLoadManager::GetNumPendingSwaps
//=============================================================================
// Description: Returns the number of load regions in line to 
//              be loaded with new data
//
//-----------------------------------------------------------------------------

unsigned int daSoundDynaLoadManager::GetNumPendingSwaps( void )
{
    return daSoundDynaLoadRegion::GetNumPendingSwaps( );
}

} // Sound Namespace
