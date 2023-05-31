//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   animentitydsgmanager
//
// Description: Single place for updating all loaded animentitydsg's in the world
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <render/animentitydsgmanager/animentitydsgmanager.h>
#include <p3d/anim/multicontroller.hpp>
#include <p3d/utility.hpp>
#include <memory/srrmemory.h>
#include <meta/locatorevents.h>
#include <events/eventmanager.h>
#include <render/DSG/IEntityDSG.h>
#include <render/DSG/animentitydsg.h>
#include <render/DSG/animcollisionentitydsg.h>
#include <render/DSG/StatePropDSG.h>
#include <mission/gameplaymanager.h>
#include <camera/supercammanager.h>
#include <radtime.hpp>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================


// Maximum number of animentitydsgs that can be inserted into the manager
const int MAX_NUM_ANIM_ENTITY_DSGS = 100;
// Maximum number of multicontrollers that can be inserted into the manager
const int MAX_NUM_MULTICONTROLLERS = 50;
// Maximum number of stateprops that can be inserted into the manager
#ifdef RAD_WIN32
const int MAX_NUM_STATEPROPS = 300;
#else
const int MAX_NUM_STATEPROPS = 250;
#endif

const char* PIGEON_IDLE_ANIM_DSG = "pidgeon_A_group";
const char* PIGEON_FLY_ANIM_DSG = "pidgeon_B_group";

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

// Singleton instance pointer
AnimEntityDSGManager* AnimEntityDSGManager::spInstance = NULL;

//===========================================================================
// Member Functions
//===========================================================================

//===========================================================================
// AnimEntityDSGManager::CreateInstance
//===========================================================================
// Description:
//				Calls AnimEntityDSGManager ctor
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================

AnimEntityDSGManager* AnimEntityDSGManager::CreateInstance()
{
MEMTRACK_PUSH_GROUP( "AnimEntityDSGManager" );
    rAssert( spInstance == NULL );

    spInstance = new(GMA_PERSISTENT) AnimEntityDSGManager;
    rAssert( spInstance != NULL );
    
MEMTRACK_POP_GROUP( "AnimEntityDSGManager" );
    return spInstance;
}

//===========================================================================
// AnimEntityDSGManager::GetInstance
//===========================================================================
// Description:
//					Returns a non-const pointer to the singleton object
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================

AnimEntityDSGManager* AnimEntityDSGManager::GetInstance()
{
    rAssert ( spInstance != NULL);

    return spInstance; 
}
//===========================================================================
// AnimEntityDSGManager::DestroyInstance
//===========================================================================
// Description:
//					Calls the private manager singleton dtor
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================
void AnimEntityDSGManager::DestroyInstance()
{
    rAssert( spInstance != NULL );

    delete( GMA_PERSISTENT, spInstance );
    spInstance = NULL; 
}
//===========================================================================
// AnimEntityDSGManager::AnimEntityDSGManager
//===========================================================================
// Description:
//			AnimEntityDSGManager ctor
//
// Constraints:
//			Allocates memory for a specific number of elements
//			if exceeded, assertion error
//
// Parameters:
//			None
//
// Return:
//
//			None
//===========================================================================

AnimEntityDSGManager::AnimEntityDSGManager()
{
	// Allocate the array of pointers
	// Don't worry too much about allocating too many since there it will
	// be the only array in existence

    mpFloatingRightWayArrows.Allocate(6);
    mpFloatingWrongWayArrows.Allocate(6);

	mEntityList.Allocate( MAX_NUM_ANIM_ENTITY_DSGS );
    mCollEntityList.Allocate( MAX_NUM_ANIM_ENTITY_DSGS );
	mMultiControllerlist.Allocate( MAX_NUM_MULTICONTROLLERS );
    mStatePropList.Allocate( MAX_NUM_STATEPROPS );
	GetEventManager()->AddListener( this, (EventEnum)( EVENT_LOCATOR + LocatorEvent::PARKED_BIRDS ) );
}
//===========================================================================
// AnimEntityDSGManager::~AnimEntityDSGManager
//===========================================================================
// Description:
//		AnimEntityDSGManager dtor
//
// Constraints:
//
// Parameters:
//		None
//
// Return:
//		None
//
//===========================================================================
AnimEntityDSGManager::~AnimEntityDSGManager()
{
	RemoveAll();
	mEntityList.Clear();
	mCollEntityList.Clear();
	mMultiControllerlist.Clear();
    mStatePropList.Clear();
	mpFloatingRightWayArrows.Clear();
	mpFloatingWrongWayArrows.Clear();
	
    GetEventManager()->RemoveListener( this, (EventEnum)( EVENT_LOCATOR + LocatorEvent::PARKED_BIRDS ) );
}
//===========================================================================
// AnimEntityDSGManager::Add
//===========================================================================
// Description:
//			Adds a new AnimCollisionEntityDSG to the manager	
//
// Constraints:
//			Increases refcount 
//	
// Parameters:
//			AnimCollisionEntityDSG* pointer
//
// Return:
//			None.
//
//===========================================================================
void AnimEntityDSGManager::Add( AnimCollisionEntityDSG* pDSG )
{
    rAssert( pDSG != NULL );
    pDSG->AddRef();
    mCollEntityList.Add( pDSG );
}


//===========================================================================
// AnimEntityDSGManager::Add
//===========================================================================
// Description:
//			Adds a new AnimEntityDSG to the manager	
//
// Constraints:
//			Increases refcount 
//	
// Parameters:
//			AnimEntityDSG* pointer
//
// Return:
//			None.
//
//===========================================================================
void AnimEntityDSGManager::Add( AnimEntityDSG* pEntity )
{
	rAssert( pEntity != NULL );

    if( pEntity->AddToUpdateList() )
    {
        pEntity->AddRef();
	    mEntityList.Add( pEntity );
        return;
    }
    if( pEntity->TrackSeparately() )
    {
        if(pEntity->TrackSeparately()%2==0)
        {
            mpFloatingWrongWayArrows.Add(pEntity);//[pEntity->TrackSeparately()-1] = pEntity;
        }
        else
        {
            mpFloatingRightWayArrows.Add(pEntity);//[pEntity->TrackSeparately()-1] = pEntity;
        }

        pEntity->AddRef();
        return;
    }
}

void AnimEntityDSGManager::Add( StatePropDSG* prop )
{
    for (int i=0;i<mStatePropList.mUseSize;i++)
    {
        assert( mStatePropList[i] != prop );
    }

    mStatePropList.Add( prop );
}


//===========================================================================
// AnimEntityDSGManager::Remove
//===========================================================================
// Description:
//
// Constraints:
//			O(N) time to remove. But array is so small who care?
//			No message if pointer not found 
//
// Parameters:
//			AnimEntityDSG* indicating the object to remove
//	
// Return:
//			None
//
//===========================================================================
void AnimEntityDSGManager::Remove( AnimEntityDSG* pEntity )
{
	// Is it possible to insert multiple copies of the same pointer?
	// Lets assume it is, and if it isn't we aren't really saving
	// much by breaking early
	for( int i = 0 ; i < mEntityList.mUseSize ; ++i)
	{
		if( mEntityList[i] == pEntity )
		{
			mEntityList[ i ]->Release();
			mEntityList.Remove( i );
		}
	}
}
//===========================================================================
// AnimEntityDSGManager::Remove
//===========================================================================
// Description:
//
// Constraints:
//			O(N) time to remove. But array is so small who care?
//			No message if pointer not found 
//
// Parameters:
//			AnimCollisionEntityDSG* indicating the object to remove
//	
// Return:
//			None
//
//===========================================================================
void AnimEntityDSGManager::Remove( AnimCollisionEntityDSG* pEntity )
{
	// Is it possible to insert multiple copies of the same pointer?
	// Lets assume it is, and if it isn't we aren't really saving
	// much by breaking early
	for( int i = 0 ; i < mCollEntityList.mUseSize ; ++i)
	{
		if( mCollEntityList[i] == pEntity )
		{
			mCollEntityList[ i ]->Release();
			mCollEntityList.Remove( i );
		}
	}
}

void AnimEntityDSGManager::Remove( StatePropDSG* pProp )
{
    for ( int i = 0 ; i < mStatePropList.mUseSize ; i++ )
    {
        if ( mStatePropList[ i ] == pProp )
        {
            mStatePropList.Remove( i );
            break;
        }
    }
}

//===========================================================================
// AnimEntityDSGManager::RemoveAll
//===========================================================================
// Description:
//		Removes all dsg entitys from the manager
//		
// Constraints:
//		Does not remove them from the scenegraph
//
// Parameters:
//		None
//
// Return:
//		None
//
//===========================================================================
void AnimEntityDSGManager::RemoveAll()
{
    for(int i=mpFloatingRightWayArrows.mUseSize-1; i>-1; i--)
	{
		mpFloatingRightWayArrows[ i ]->Release();
	}

    for(int i=mpFloatingWrongWayArrows.mUseSize-1; i>-1; i--)
	{
		mpFloatingWrongWayArrows[ i ]->Release();
	}

    for (int i = 0 ; i < mEntityList.mUseSize ; ++i )
	{
		mEntityList[ i ]->Release();
	}
	for (int i = 0 ; i < mCollEntityList.mUseSize ; ++i )
	{
		mCollEntityList[ i ]->Release();
	}
	for (int i = 0 ; i < mMultiControllerlist.mUseSize ; ++i )
	{
		mMultiControllerlist[ i ]->Release();
	}
	mpFloatingRightWayArrows.ClearUse();
	mpFloatingWrongWayArrows.ClearUse();
	mEntityList.ClearUse();
	mCollEntityList.ClearUse();
	mMultiControllerlist.ClearUse();
    mStatePropList.ClearUse();
}
//===========================================================================
// AnimEntityDSGManager::Update
//===========================================================================
// Description:
//		Advances all animations 		
//	
// Constraints:
//
// Parameters:
//		time to advance in milliseconds
//
// Return:
//		none.
//
//===========================================================================
#ifdef BREAK_DOWN_PROFILE
    #define ANIM_PROFILE_BEGIN(atime)  atime = radTimeGetMicroseconds(); 
    #define ANIM_PROFILE_END(atime, aname)    atime = radTimeGetMicroseconds() - atime; if(atime>90) rReleasePrintf("Advance on %s takes %u us.\n", aname, atime); 
#else
    #define ANIM_PROFILE_BEGIN(atime) 
    #define ANIM_PROFILE_END(atime, aname) 
#endif

void AnimEntityDSGManager::Update( unsigned int elapsedTime )
{
	float fElapsedTime = static_cast<float>(elapsedTime);
	float elapsedTimeSeconds = fElapsedTime * 0.001f;

    #ifdef BREAK_DOWN_PROFILE
        unsigned int updateTime;
    #endif //BREAK_DOWN_PROFILE

    int i;

	for (i = 0 ; i < mEntityList.mUseSize ; ++i)
	{
        ANIM_PROFILE_BEGIN(updateTime)
		mEntityList[ i ]->Update( elapsedTimeSeconds );
        ANIM_PROFILE_END(updateTime, mEntityList[i]->GetName())
	}
	for (i = 0 ; i < mCollEntityList.mUseSize ; ++i)
	{
        ANIM_PROFILE_BEGIN(updateTime)
        mCollEntityList[ i ]->AdvanceAnimation( elapsedTimeSeconds );            
        ANIM_PROFILE_END(updateTime, mCollEntityList[i]->GetName())
	}
    for (i = 0 ; i < mStatePropList.mUseSize ; ++i)
    {
        ANIM_PROFILE_BEGIN(updateTime)
        mStatePropList[ i ]->AdvanceAnimation( fElapsedTime );
        ANIM_PROFILE_END(updateTime, mStatePropList[i]->GetName())
    }

	for (i = 0 ; i < mMultiControllerlist.mUseSize ; ++i)
	{
        ANIM_PROFILE_BEGIN(updateTime)
		mMultiControllerlist[ i ]->Advance( fElapsedTime );
        ANIM_PROFILE_END(updateTime, mMultiControllerlist[i]->GetName())
	}
}

//===========================================================================
// AnimEntityDSGManager::Add
//===========================================================================
// Description:
//		Adds the given multicontroller to the list, but only if it is not present
//		already		
//	
// Constraints:
//
// Parameters:
//		tMultiController pointer
//
// Return:
//		none.
//
//===========================================================================
void AnimEntityDSGManager::Add( tMultiController* pController )
{
	bool found = false;
	for (int i = 0 ; i < mMultiControllerlist.mUseSize ; i++)
	{
		if ( mMultiControllerlist[ i ] == pController )
		{
			found = true;
			break;
		}
	}


	if ( found == false )
	{
		mMultiControllerlist.Add( pController );
		pController->AddRef();
	}
}
//===========================================================================
// AnimEntityDSGManager::Remove
//===========================================================================
// Description:
//		Removes the given multicontroller from the list		
//	
// Constraints:
//
// Parameters:
//		tMultiController pointer
//
// Return:
//		none.
//
//===========================================================================
void AnimEntityDSGManager::Remove( tMultiController* pController )
{
	for (int i = 0 ; i < mMultiControllerlist.mUseSize ; i++)
	{
		if ( mMultiControllerlist[ i ] == pController )
		{
			pController->Release();
			mMultiControllerlist.Remove( i );
			break;
		}
	}
}
//===========================================================================
// AnimEntityDSGManager::HandleEvent
//===========================================================================
// Description:
//		Handles trigger volume locators being triggered	
//	
// Constraints:
//
// Parameters:
//		Event ID and pointer to user data
//
// Return:
//		none.
//
//===========================================================================

void AnimEntityDSGManager::HandleEvent( EventEnum id, void* pEventData )
{


	switch( id )
	{
	case EVENT_LOCATOR + LocatorEvent::PARKED_BIRDS:
		{
			// The character has walked into the zone of range of the birds.
			// We must make them fly away, but only if they are on the ground actually playing
			// Stop the original idle animation. Remove it from the DSG.
			// Find the second animation of the birds flying away. Insert it into the DSG and start it off
			
			AnimEntityDSG* pIdleAnim = p3d::find< AnimEntityDSG >( PIGEON_IDLE_ANIM_DSG );
			rAssertMsg( pIdleAnim != NULL, "Pigeon idle animation not found" );
			
            //chuck:if the animation isn't loaded yet don't play it
            if(pIdleAnim != NULL)
            {
			    if ( pIdleAnim && pIdleAnim->GetVisibility() )
			    {

				    AnimEntityDSG* pTriggerAnim = p3d::find< AnimEntityDSG >( PIGEON_FLY_ANIM_DSG );
				    rAssertMsg( pTriggerAnim != NULL, "Pigeons flying animation not found" );

				    pIdleAnim->PlayAnimation( false );
				    pIdleAnim->SetVisibility( false );

                    pTriggerAnim->Reset();
				    pTriggerAnim->PlayAnimation( true );
				    pTriggerAnim->SetVisibility( true );

                    GetEventManager()->TriggerEvent( EVENT_PLAY_BIRD_SOUND );
			    }
            }

		}
		break;

	default:
		rAssert(false);
		break;
	}
}

