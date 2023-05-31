//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   breakablesmanager
//
// Description: Contains and manages all types of breakable animations in the game
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <render/particles/particlemanager.h>
#include <render/Breakables/breakablesmanager.h>

#include <render/RenderManager/RenderManager.h>
#include <render/culling/worldscene.h>
#include <render/RenderManager/WorldRenderLayer.h>
#include <events/eventmanager.h>

#include <memory/srrmemory.h>
#include <algorithm>
#include <worldsim/coins/coinmanager.h>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================


//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

// Static instance of our singleton pointer.
BreakablesManager* BreakablesManager::spInstance = NULL;
const char* BreakablesManager::sInventorySectionName = "Breakables Inventory Section";


//===========================================================================
// Member Functions
//===========================================================================

//==============================================================================
// BreakablesManager::CreateInstance
//==============================================================================
//
// Description: Creates the BreakablesManager.
//
// Parameters:	None.
//
// Return:      Pointer to the BreakablesManager.
//
// Constraints: Multiple calls to CreateInstance without a DestroyInstance call in between
//              will result in an assertion (or lost memory if assertions not enabled)
//
//==============================================================================
BreakablesManager* BreakablesManager::CreateInstance()
{
MEMTRACK_PUSH_GROUP( "BreakablesManager" );
    rAssert( spInstance == NULL );

    spInstance = new(GMA_PERSISTENT) BreakablesManager;
    rAssert( spInstance != NULL );
MEMTRACK_POP_GROUP( "BreakablesManager" );
    
    return spInstance;
}
//==============================================================================
// BreakablesManager::DestroyInstance
//==============================================================================
//
// Description: Frees the BreakablesManager singleton.
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: Assertion failure if CreateInstance was not called first.
//
//
//==============================================================================
void BreakablesManager::DestroyInstance()
{
    rAssert( spInstance != NULL );

    delete( GMA_PERSISTENT, spInstance );
    spInstance = NULL;
}
//==============================================================================
// BreakablesManager::GetInstance
//==============================================================================
//
// Description: Returns a BreakablesManager singleton object.
//
// Parameters:	None.
//
// Return:      Pointer to a BreakablesManager object.
//
// Constraints: Assertion failure if CreateInstance was not called first.
//
//
//==============================================================================
BreakablesManager* BreakablesManager::GetInstance()
{
    if ( spInstance == NULL )
    {
        CreateInstance();
    }
    rAssert ( spInstance != NULL);

    return spInstance;
}

//==============================================================================
// BreakablesManager::BreakablesManager
//==============================================================================
//
// Description: BreakablesManager constructor.
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: 
//
//
//==============================================================================
BreakablesManager::BreakablesManager()
: mInventorySectionUID( tName::MakeUID( GetInvSectionName() ) ),
mBreakablesList( BreakablesEnum::eNumBreakables ),
mZoneList( BreakablesEnum::eNumBreakables )
{
    GetEventManager()->AddListener( this, EVENT_DUMP_DYNA_SECTION );
	mBreakableRemoveQueue.Allocate( BREAKABLE_QUEUE_SIZE );
	
	unsigned int i;
    mZoneList.AddUse( BreakablesEnum::eNumBreakables );
	for ( i = 0; i < BreakablesEnum::eNumBreakables; ++i )
	{
       // mZoneList[i].AddUse( BreakablesEnum::eMaxBreakableNames );
        mZoneList[i].Allocate( BreakablesEnum::eMaxBreakableNames );
	}
}
//==============================================================================
// BreakablesManager::~BreakablesManager
//==============================================================================
//
// Description: BreakablesManager destructor
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: 
//
//
//==============================================================================
BreakablesManager::~BreakablesManager()
{
    mBreakablesList.clear();
	p3d::inventory->DeleteSection( mInventorySectionUID );
    GetEventManager()->RemoveListener( this, EVENT_DUMP_DYNA_SECTION );
}
//==============================================================================
// BreakablesManager::Update
//==============================================================================
//
// Description: Updates all breakables in the game 
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: 
//
//
//==============================================================================

//==============================================================================
// BreakablesManager::ManagedBreakable::ManagedBreakable
//==============================================================================
//
// Description: ManagedBreakable ctor.
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: None.
//
//
//==============================================================================
BreakablesManager::ManagedBreakable::ManagedBreakable()
: mIsActive( false )
{
    mpBreakableDSG = new (GMA_LEVEL_OTHER) BreakableObjectDSG;    
    mpBreakableDSG->AddRef();
}
//==============================================================================
// BreakablesManager::ManagedBreakable::~ManagedBreakable
//==============================================================================
//
// Description: ManagedBreakable dtor.
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: None.
//
//
//==============================================================================
BreakablesManager::ManagedBreakable::~ManagedBreakable()
{
    if ( mpBreakableDSG != NULL )   
    {
        mpBreakableDSG->ReleaseVerified();
        mpBreakableDSG = NULL;
    }
}
void BreakablesManager::ManagedBreakable::AddToDSG()
{
    // Get the renderlayer that the breakable object is located in, store it in mLayer
    mLayer = static_cast< RenderEnums::LayerEnum >( GetRenderManager()->rCurWorldRenderLayer() );
    // Add the object
    GetRenderManager()->pWorldRenderLayer()->pWorldScene()->Add( mpBreakableDSG );   
}

void BreakablesManager::ManagedBreakable::RemoveFromDSG()
{
    // Get the renderlayer that the breakable object is located in
    WorldRenderLayer* pWorldRenderLayer = static_cast< WorldRenderLayer* > (GetRenderManager()->mpLayer( mLayer ));              
    // Sanity check
    rAssert( dynamic_cast<WorldRenderLayer*>(pWorldRenderLayer) != NULL );
    // Remove the object
    pWorldRenderLayer->pWorldScene()->Remove( mpBreakableDSG );   

}


//==============================================================================
// BreakablesManager::DebugRender
//==============================================================================
//
// Description: Draw all breakable objects in one ugly loop.
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: Get rid of this once Devin's Z sorting is finished.
//
//
//==============================================================================
void BreakablesManager::DebugRender()const
{
	return;
    for( unsigned int i = 0 ; i < mBreakablesList.size() ; ++i )
    {
        for( unsigned int j = 0 ; j < mBreakablesList[ i ].size ; ++j)
        {
            if (mBreakablesList[ i ].list[ j ]->mIsActive)
                mBreakablesList[ i ].list[ j ]->mpBreakableDSG->Display();
        }
    }
}
//==============================================================================
// BreakablesManager::Update
//==============================================================================
//
// Description: Advance all animations of currently playing breakable animations
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: Is float the correct parameter? Is int or unsigned int better?
//
//
//==============================================================================
void BreakablesManager::Update( unsigned int deltaTime )
{
	float fDeltaTime = static_cast< float >( deltaTime );

    for( unsigned int i = 0 ; i < mBreakablesList.size() ; ++i )
    {
        for( unsigned int j = 0 ; j < mBreakablesList[ i ].size ; ++j)
        {
            if ( mBreakablesList[ i ].list[ j ]->mIsActive )
            {
                if (mBreakablesList[ i ].list[ j ]->mpBreakableDSG->LastFrameReached())
                {
                    mBreakablesList[ i ].list[ j ]->mIsActive = false;
                    // Remove it from the DSG
                    mBreakablesList[ i ].list[ j ]->RemoveFromDSG();
                    // Lets check the type that we just finished playing
                    if ( BreakablesEnum::BreakableID(i) == BreakablesEnum::eCarExplosion )
                    {
                        // Trigger a car explosion event
                        GetEventManager()->TriggerEvent( EVENT_CAR_EXPLOSION_DONE, NULL );
                    }
                }   
                else
                {
                    mBreakablesList[ i ].list[ j ]->mpBreakableDSG->Update( fDeltaTime );
                }
            }
        }
    }
	// Iterate through the list of broken objects (NOT the breakable animation, the original
	// object that was broken and replaced by the breakable) and remove them from the DSG tree
	
	for( int i = 0 ; i < mBreakableRemoveQueue.mUseSize ; ++i )
	{
        // Get the renderlayer that the broken object is located in
        WorldRenderLayer* pWorldRenderLayer = static_cast< WorldRenderLayer* > (GetRenderManager()->mpLayer( mBreakableRemoveQueue[ i ].layer ));              
        // Sanity check
        rAssert( dynamic_cast<WorldRenderLayer*>(pWorldRenderLayer) != NULL );
        // Remove the object
        if ( mBreakableRemoveQueue[ i ].useRemoveGuts )
        {
            pWorldRenderLayer->RemoveGuts( mBreakableRemoveQueue[ i ].pDSG );
        }
        else
        {
            pWorldRenderLayer->pWorldScene()->Remove( mBreakableRemoveQueue[ i ].pDSG );
        } 
	}
	mBreakableRemoveQueue.ClearUse();

}
//==============================================================================
// BreakablesManager::AllocateBreakables
//==============================================================================
//
// Description: Allocate the breakable objects of a certain type
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: None.
//
//
//==============================================================================
void BreakablesManager::AllocateBreakables( BreakablesEnum::BreakableID type, 
                                            tAnimatedObjectFactory* pFactory, 
                                            tAnimatedObjectFrameController* pController, 
                                            int numInstances )
{
    rAssert( type >= 0 && type < (int)mBreakablesList.size());
        
    // Set the tName of the zone that we are loading
    // This could cause a brief alloc, which will go away when we go out of scope.  So push GMA_TEMP here.
    //
    HeapMgr()->PushHeap(GMA_TEMP);
    tName zoneBeingLoaded = GetRenderManager()->pWorldRenderLayer()->GetCurSectionName().GetUID();
    HeapMgr()->PopHeap(GMA_TEMP);
 
    mZoneList[ type ].Add( zoneBeingLoaded );

    if ( mBreakablesList[type].size == 0 )
    {
        #ifdef RAD_GAMECUBE
            HeapMgr()->PushHeap( GMA_GC_VMM );
        #else
            HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
        #endif
        mBreakablesList[ type ].size = numInstances;
        mBreakablesList[ type ].list = new ManagedBreakable*[ numInstances ];
        for( int i = 0 ; i < numInstances ; ++i )
        {
            mBreakablesList[ type ].list[ i ] = new ManagedBreakable;
            mBreakablesList[ type ].list[ i ]->mpBreakableDSG->Init( pFactory, pController );
        }
        #ifdef RAD_GAMECUBE
            HeapMgr()->PopHeap( GMA_GC_VMM );
        #else
            HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
        #endif
    }
}
//==============================================================================
// BreakablesManager::FreeBreakables
//==============================================================================
//
// Description: Frees all breakables of the given type
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: Not refcount tested yet.
//
//
//==============================================================================
void BreakablesManager::FreeBreakables( BreakablesEnum::BreakableID type )
{
	RemoveFromDSG( type );

    rAssert( type >= 0 && type < (int)mBreakablesList.size());

    for (unsigned int i = 0 ; i < mBreakablesList[ type ].size ; ++i)
    {
        delete mBreakablesList[ type ].list[ i ];
    }
    delete[] mBreakablesList[ type ].list;
    mBreakablesList[ type ].list = 0;
    mBreakablesList[ type ].size = 0;

    mZoneList[ type ].ClearUse();
}
//==============================================================================
// BreakablesManager::RemoveAllFromDSG
//==============================================================================
//
// Description: Removes all playing breakables from the DSG
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: 
//
//
//==============================================================================
void BreakablesManager::RemoveAllFromDSG()
{
	// For each type
	// for each allocated instance
	// if its in the DSG
	// remove it and set active to false
	for ( unsigned int i = 0 ; i < mBreakablesList.size() ; i++)
	{
		for ( unsigned int j = 0 ; j < mBreakablesList[ i ].size ; j++ )
		{
			if ( mBreakablesList[ i ].list[ j ]->mIsActive )
			{
				mBreakablesList[ i ].list[ j ]->mIsActive = false;
                mBreakablesList[ i ].list[ j ]->RemoveFromDSG();
			}
		}
	}
}
void BreakablesManager::RemoveFromDSG( BreakablesEnum::BreakableID type )
{
	for ( unsigned int j = 0 ; j < mBreakablesList[ type ].size ; j++ )
	{
		if ( mBreakablesList[ type ].list[ j ]->mIsActive )
		{
			mBreakablesList[ type ].list[ j ]->mIsActive = false;
            mBreakablesList[ type ].list[ j ]->RemoveFromDSG();
		}
	}
}


//==============================================================================
// BreakablesManager::FreeAllBreakables
//==============================================================================
//
// Description: Frees all breakables
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: None.
//
//
//==============================================================================
void BreakablesManager::FreeAllBreakables()
{
    for ( unsigned int i = 0 ; i < mBreakablesList.size() ; ++i )
    {
        FreeBreakables( BreakablesEnum::BreakableID( i ) );
    }
}
//==============================================================================
// BreakablesManager::RemoveBrokenObjectFromWorld
//==============================================================================
//
// Description: Adds a broken object to the manager's internal list of intact objects	
//				that have to be removed since they got broken. We don't immediately remove the broken
//				object because they may still reside in internal collision states until collision
//				resolution is completed
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: Will not add duplicate entries. O(N) performance (but array is so small who cares?)
//
//
//==============================================================================
void BreakablesManager::RemoveBrokenObjectFromWorld( IEntityDSG* pObjectToBeRemoved, RenderEnums::LayerEnum layer, bool useRemoveGuts )
{
	bool wasDuplicateFound = false;
	for( int  i = 0 ; i < mBreakableRemoveQueue.mUseSize ; ++i)
	{
        if( mBreakableRemoveQueue[ i ].pDSG == pObjectToBeRemoved )
		{
			wasDuplicateFound = true;
			break;
		}
	}
	if( wasDuplicateFound == false )
	{
        BrokenObject brokenObject;
        brokenObject.pDSG = pObjectToBeRemoved;
        brokenObject.layer = layer;
        brokenObject.useRemoveGuts = useRemoveGuts;

		mBreakableRemoveQueue.Add( brokenObject );
	}
}

//==============================================================================
// BreakablesManager::Play
//==============================================================================
//
// Description: Inserts a new breakable object into the DSG and starts animation playback
//              Animation stops when LastFrameReached() is true
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: 
//
//
//==============================================================================
void BreakablesManager::Play( BreakablesEnum::BreakableID type, const rmt::Matrix& transform )
{
    rAssert( type >= 0 && type < (int)mBreakablesList.size());

    if ( mBreakablesList[ type ].size > 0 )
    {
        ManagedBreakable* pMB = mBreakablesList[ type ].Next();
        if ( pMB->mIsActive )
        {
            pMB->RemoveFromDSG();
        }
        pMB->mIsActive = true;
        pMB->mpBreakableDSG->Reset();
        pMB->mpBreakableDSG->SetTransform( transform );
        pMB->AddToDSG();

        //passing pointer to local variable here!!
        GetEventManager()->TriggerEvent( EVENT_HIT_BREAKABLE, (void*)&type );
    }
}
 
void BreakablesManager::HandleEvent( EventEnum id, void* pEventData )
{
    int i,j;
    switch (id)
    {
    case EVENT_DUMP_DYNA_SECTION:
        {
            // This could cause a brief alloc, which will go away when we go out of scope.  So push GMA_TEMP here.
            //
            HeapMgr()->PushHeap(GMA_TEMP);
            tName zoneBeingDumped = *( static_cast< tName* >( pEventData ) );
            HeapMgr()->PopHeap(GMA_TEMP);

            // Iterate through all the breakables and find those that have the 
            // same name as the one given
            for ( i = 0 ; i < mZoneList.mUseSize ; i++)
            {
                //tNameList::iterator it = std::find( mZoneList[i].begin(), mZoneList[i].end(), zoneBeingDumped );
                for ( j = 0 ; j < mZoneList[ i ].mUseSize ; j++)
                {
                    if ( mZoneList[ i ][j] == zoneBeingDumped )
                    {
                        break;
                    }
                }

                if ( j != mZoneList[ i ].mUseSize )
                {
                    mZoneList[i].Remove( j );
                }
                // Kill the zone if all zones that reference this breakable are gone
                if ( mZoneList[i].mUseSize == 0 )
                {
                    FreeBreakables( static_cast<BreakablesEnum::BreakableID>(i) );
                }
            }            
        }
        break; 
    default:
        rAssertMsg(true,"Unhandled case in Breakables Manager!");
        break;
    };
}


BreakablesManager::ManagedBreakable*
BreakablesManager::BreakableInstances::Next()
{
    ++currElement;

    if ( currElement >= size )
    {
        currElement = 0;
    }
    BreakablesManager::ManagedBreakable* pRetVal = list[ currElement ];
    return pRetVal;
}

bool BreakablesManager::IsLoaded( BreakablesEnum::BreakableID type )
{
    bool isLoaded;
    if ( type < 0 || type >= static_cast< int > ( mBreakablesList.size() ) )
    {
        isLoaded = false;
    }
    else if ( mBreakablesList[ type ].size == 0 )
    {
        isLoaded = false;
    }
    else
    {
        isLoaded = true;
    }   

    return isLoaded;
}

void BreakablesManager::AddToZoneList( BreakablesEnum::BreakableID type )
{
    if ( type < 0 || type >= static_cast< int >( mBreakablesList.size() ))
        return;
    
    // Check to see that we aren't already adding it        
    tName zoneBeingLoaded = GetRenderManager()->pWorldRenderLayer()->GetCurSectionName().GetUID();
    for ( int i = 0 ; i < mZoneList[ type ].mUseSize ; i++ )
    {
        if ( zoneBeingLoaded == mZoneList[ type ][ i ] )
            return;
    }

    mZoneList[ type ].Add( zoneBeingLoaded );
}

