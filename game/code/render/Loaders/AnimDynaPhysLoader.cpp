//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   AnimDynaPhysLoader
//
// Description: Loader for instanced, animated, dynaphys DSG objects
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================
#include <constants/chunks.h>
#include <constants/chunkids.hpp>
#include <render/Loaders/AnimDynaPhysLoader.h>
#include <render/DSG/InstAnimDynaPhysDSG.h>
#include <atc/atcmanager.h>
#include <constants/srrchunks.h>
#include <memory/srrmemory.h>
#include <simcollision/collisionobject.hpp>
#include <simphysics/physicsobject.hpp>
#include <render/Loaders/GeometryWrappedLoader.h>
#include <p3d/anim/multicontroller.hpp>
#include <render/Loaders/AllWrappers.h>
#include <render/Loaders/BillboardWrappedLoader.h>
#include <p3d/anim/skeleton.hpp>
#include <p3d/anim/animate.hpp>
#include <p3d/anim/compositedrawable.hpp>
#include <p3d/chunkfile.hpp>
#include <p3d/inventory.hpp>
#include <render/AnimEntityDSGManager/AnimEntityDSGManager.h>
#include <render/DSG/StatePropDSG.h>
#include <stateprop/statepropdata.hpp>
#include <p3d/anim/animatedobject.hpp>
#include <constants/physprop.h>
#include <data/persistentworldmanager.h>
#include <main/game.h>
#include <radmath/random.hpp>
#ifndef RAD_RELEASE
#include <memory/propstats.h>
#endif

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

Map< tUID, tUID > AnimDynaPhysLoader::s_ShadowList;


//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================



//===========================================================================
// Member Functions
//===========================================================================

//===========================================================================
// AnimDynaPhysLoader::AnimDynaPhysLoader
//===========================================================================
// Description:
//		AnimDynaPhysLoader ctor
//
// Constraints:
//
// Parameters:
//	
//
// Return:
//
//===========================================================================

AnimDynaPhysLoader::AnimDynaPhysLoader() : tSimpleChunkHandler(SRR2::ChunkID::INSTA_ANIM_DYNA_PHYS_DSG)
{

    mpListenerCB  = NULL;
    mUserData     = -1;

    if ( s_ShadowList.capacity() == 0 )
    {
        s_ShadowList.reserve( 20 );
    }
}

//===========================================================================
// AnimDynaPhysLoader::~AnimDynaPhysLoader
//===========================================================================
// Description:
//		AnimDynaPhysLoader dtor
//
// Constraints:
//
// Parameters:
//	
//
// Return:
//
//===========================================================================

AnimDynaPhysLoader::~AnimDynaPhysLoader()
{

}

//===========================================================================
// AnimDynaPhysLoader::LoadObject
//===========================================================================
// Description:
//		Loads an InstAnimDynaPhysDSG chunk and creates all the instances
//
// Constraints:
//		Expecting file format to be:
//		string name (max size 255)
//		long version
//		long hasalpha flag	
//			
//
// Parameters:
//		Chunk file, and store
//
// Return:
//		NULL, no final chunk going specifically into the inventory via return
//
//===========================================================================

tEntity* AnimDynaPhysLoader::LoadObject(tChunkFile* file, tEntityStore* store)
{
    IEntityDSG::msDeletionsSafe=true;
	// Grab the name
    char typeName[255];
    file->GetPString(typeName);

    // Quick test to see if we are dealing with an animated BV object
    int simAnimJoint = -1;
    if ( strstr ( typeName, "crate" ) != NULL )
        simAnimJoint = 3;
    else if ( strstr ( typeName, "vending" ) != NULL )
        simAnimJoint = 1;   
    else if ( strstr ( typeName, "l2_monkiesgag" ) != NULL )
        simAnimJoint = 1;
    else if ( strstr ( typeName, "l7_spider" ) != NULL )
        simAnimJoint = 1;


#ifndef RAD_RELEASE
    PropStats::StartTracking(typeName);
#endif

    int instanceCount = 0;

	// Grab version info
    int version = file->GetLong();
	// Grab the flag that tell us whether or not the object has alpha or not
    int hasAlpha = file->GetLong();

    // Does this chunk contain a subchunk for instances
    bool foundInstances = false;
    bool persistance = true;

    // Is this object a global entity (i.e. is it loaded once at level startup and
    // all subsequent instances are cloned off this thing?)
    StatePropDSG* globalStatepropEntity = static_cast<StatePropDSG*>(GetAllWrappers()->GetGlobalEntity(tName::MakeUID(typeName)));
    if(strstr(typeName, "phonestop_Shape"))
    {
        persistance = false;
    }
    bool haveGlobal = globalStatepropEntity != 0;
#ifndef RAD_RELEASE
    if ( haveGlobal )
    {
        rAssert( dynamic_cast< StatePropDSG* >( globalStatepropEntity ) != NULL );
    }
#endif
    CollisionAttributes*    pCollAttr   = NULL;
	AnimDynaPhysWrapper*	pWrappedObject = NULL;

    // Is this a mission prop?
    
    bool isMissionProp = IsMissionProp( typeName );


    while( file->ChunksRemaining() )
	{
		file->BeginChunk();
		switch(file->GetCurrentID())
		{
        case SRR2::ChunkID::ANIM_OBJ_DSG_WRAPPER:
            // We are loading a stateprop
            {
                if(!haveGlobal)
                {
                    HeapMgr()->PushHeap( GMA_TEMP );
                    AnimObjDSGWrapperLoader* pObjWrapperLoader = new AnimObjDSGWrapperLoader;
                    HeapMgr()->PopHeap( GMA_TEMP );
                    pWrappedObject = (AnimDynaPhysWrapper*) pObjWrapperLoader->LoadObject( file, store );
                    pWrappedObject->AddRef ();
                    pObjWrapperLoader->ReleaseVerified();
                }
                m_IsStateProp = true;
            }
            break;

		case SRR2::ChunkID::ANIM_DSG_WRAPPER:
			{
                if(!haveGlobal)
                {
				    // This is the wrapped object and contains a whole load of crap, meshes, comp drawables, billboards
				    // load them all
				    // Format of the wrapped object is 
				    // string name
				    // ULONG version
				    // ULONG hasalpha
			    //	pWrappedObject = new (GMA_TEMP ) WrappedObject;
			    //	LoadAnimWrapper( file, store, pWrappedObject );				
    				
                    HeapMgr()->PushHeap( GMA_TEMP );
				    AnimDynaPhysWrapperLoader* pWrapperLoader = new AnimDynaPhysWrapperLoader;
                    HeapMgr()->PopHeap( GMA_TEMP );

				    pWrappedObject = (AnimDynaPhysWrapper*) pWrapperLoader->LoadObject( file, store );
                    pWrappedObject->AddRef ();
				    pWrapperLoader->Release();
                }
                m_IsStateProp = false;
			}
			break;
			// Relies on ObjectAttributes and the Wrapped DSG object being loaded first

   		case SRR2::ChunkID::OBJECT_ATTRIBUTES:
			{
                if(!haveGlobal)
                {
                    rAssert( pWrappedObject != NULL );
                    // Relies on the Wrapped Object being loaded first
                    int classType = file->GetLong();
                    int physPropID = file->GetLong();
                    char tempsound [64];

                    file->GetString(tempsound); //Chuck: Reading the new sound properties since we dont use the sound returned from the ATC manager.

                    float volume;
                    if ( classType == PROP_MOVEABLE || 
                        classType == PROP_BREAKABLE ||
                        classType == PROP_ONETIME_MOVEABLE ||
                        classType == 8 )
                    {
                        volume = pWrappedObject->GetVolume();
                    }
                    else 
                    {
                        volume = 0.0f;
                    }
                    pCollAttr = GetATCManager()->CreateCollisionAttributes(classType, physPropID, volume);
                    pCollAttr->AddRef ();
                    pCollAttr->SetSound(tempsound); //Chuck: Setting the CollAttr to use the correct sound from the OTC chunk.
                }
			}
			break;

		case SRR2::ChunkID::INSTANCES:
            {
                foundInstances = true;

                //Instances >> Scenegraph
                file->BeginChunk();
                //Scenegraph >> ScenegraphRoot
                file->BeginChunk();
                //ScenegraphRoot >> ScenegraphBranch
                file->BeginChunk();
                //ScenegraphBranch >> ScenegraphTransform
                file->BeginChunk();

                //ScenegraphTransform >> real ScenegraphTransform
                //f->BeginChunk();

                while( file->ChunksRemaining() )
                {
                    instanceCount++;


                    file->BeginChunk();
                    char name[256];
                    file->GetPString(name);
                    int numChild = file->GetLong();

					rmt::Matrix matrix;
                    file->GetData( &matrix,16, tFile::DWORD );

                    if ( m_IsStateProp ) 
                    {
                        short persistentID = -1;
                        if(persistance && ((haveGlobal && globalStatepropEntity->GetCollisionAttributes()->GetClasstypeid()) || (!haveGlobal && ((pCollAttr->GetClasstypeid() == PROP_BREAKABLE) || (pCollAttr->GetClasstypeid() == PROP_MOVEABLE)))))
                        {
                            if ( isMissionProp == false )
                                persistentID = GetPersistentWorldManager()->GetPersistentObjectID( tEntity::MakeUID( file->GetFilename() ),tEntity::MakeUID(typeName) );
                            else
                                persistentID = -1;
                        }
                        if( persistentID >= -1 )
                        {
                            StatePropDSG* pStatePropDSG = NULL;

                            // Should we clone off the global entity (if it exists?)
                            if (haveGlobal)
                            {
                                pStatePropDSG = globalStatepropEntity->Clone( name, matrix );
                            }
                            else
                            {
                                rAssert( pCollAttr != NULL );
                                rAssert( pWrappedObject != NULL );
                                bool useSharedtPose = pWrappedObject->HasAnimation() ? false : true;                                                           
                                pStatePropDSG = new StatePropDSG(); 
                                pStatePropDSG->LoadSetup( pWrappedObject->GetStatePropData(), 0, matrix, pCollAttr, true, store, useSharedtPose, pWrappedObject->GetCollisionObject(), pWrappedObject->GetPhysicsObject() );
                                pStatePropDSG->SetName( name );
                                pStatePropDSG->SetSimJoint( simAnimJoint );
                            }

                

                            #ifndef FINAL
                            if( !haveGlobal && (pWrappedObject->GetStatePropData() == NULL))
                                {
                                    char outbuffer [255];
                                    sprintf(outbuffer,"Error: %s is missing a StateProp Chunk \n",pWrappedObject->GetName()); 
                                    rTuneAssertMsg( 0,outbuffer );
                                }
                            #endif


                            mpListenerCB->OnChunkLoaded( pStatePropDSG, mUserData, _id );
                            pStatePropDSG->mPersistentObjectID = persistentID;

                            // Check to see if there is a shadow associated with this object                           
                            tUID compDrawUID = pStatePropDSG->GetDrawableUID();

                            tUID shadowElementName = GetShadowElement( compDrawUID );
                            if ( shadowElementName != static_cast< tUID >( 0 ) )
                            {
                                pStatePropDSG->SetShadowElement( shadowElementName );
                            }

                            // place it into the inventory so that locators can access it by name
                            bool collision = store->TestCollision( pStatePropDSG->GetUID(), pStatePropDSG );
                            if( !collision )
                            {
                                store->Store( pStatePropDSG );
                            }
                            else
                            {
                                HandleCollision( pStatePropDSG );
                            }
                            // Lets offset the animation so that every crate or vending machine
                            // instance isn't jumping together
                            // Make the stateprop instances each start at a different point in time
                            static rmt::Randomizer randomizer( Game::GetRandomSeed () );
                            const float RANDOM_TIME_MS = 10000.0f;
                            // Advance animations by 0 - 10 seconds
                            float randomUpdateTime = randomizer.Float() * RANDOM_TIME_MS;
                            pStatePropDSG->AdvanceAnimation( randomUpdateTime );

                        }
                    }
                    else
                    {
                        InstAnimDynaPhysDSG* pAnimDSG = new InstAnimDynaPhysDSG();
                        // Make sure that each name is unique
                        // things like the powerboxes are set via lookup
                        pAnimDSG->SetName( name );

                        if( hasAlpha || pWrappedObject->HasAlpha() )
                        {
                            pAnimDSG->mTranslucent = true;
                        }
					    else
					    {
						    pAnimDSG->mTranslucent = false;
					    }
    
                        rAssert( pCollAttr != NULL );
                        pAnimDSG->LoadSetUp( pCollAttr, matrix, pWrappedObject->GetDrawable(), pWrappedObject->GetController(), store );
                        mpListenerCB->OnChunkLoaded( static_cast< InstDynaPhysDSG*> (pAnimDSG), mUserData, _id );
                            
                        // place it into the inventory so that locators can access it by name
                        store->Store( pAnimDSG );
                    }
                    file->EndChunk();  

#ifndef RAD_RELEASE
                    if ( instanceCount == 1 )
                    {
                        PropStats::StopTracking( typeName, 1 );
                        PropStats::StartTracking( typeName );
                        instanceCount = 0;
                    }
#endif


                }
                //ScenegraphBranch >> ScenegraphTransform
                file->EndChunk();
                //ScenegraphRoot >> ScenegraphBranch
                file->EndChunk();
                //Scenegraph >> ScenegraphRoot
                file->EndChunk();
                //Instances >> Scenegraph
                file->EndChunk();
            }
            break;


		default:
			rAssertMsg( false, "Unknown chunk in AnimatedDynaPhysDSG file");
			break;
		};
        file->EndChunk();
	}

    if(0&&!foundInstances && !haveGlobal )
    {
        // No instance chunk!
        // Therefore this MUST be a global entity
        // Dump it into the global entity list
        // so that subsequent instances of this type
        // will get cloned off the global entity instead of loading all this crap again
        //This must be a global entity.
        StatePropDSG* globalEntityStateProp = new StatePropDSG();
        rAssert(globalEntityStateProp);
        // Give this thing the name of the type

        bool useSharedtPose = pWrappedObject->HasAnimation() ? false : true;  
        rmt::Matrix identity;
        identity.Identity();
        globalEntityStateProp->LoadSetup( pWrappedObject->GetStatePropData(), 
                                          0, 
                                          identity, 
                                          pCollAttr, 
                                          false, 
                                          NULL, 
                                          useSharedtPose, 
                                          pWrappedObject->GetCollisionObject(), 
                                          pWrappedObject->GetPhysicsObject() );
        globalEntityStateProp->SetName( typeName );
        GetAllWrappers()->AddGlobalEntity( globalEntityStateProp );
        globalEntityStateProp->SetSimJoint( simAnimJoint );
        instanceCount++;
    }

    if( pWrappedObject != NULL)
    {
        pWrappedObject->ReleaseVerified ();
    }
    if ( pCollAttr != NULL )
    {
        pCollAttr->Release ();
    }

#ifndef RAD_RELEASE
    PropStats::StopTracking( typeName, instanceCount );
#endif


    IEntityDSG::msDeletionsSafe=false;
	return NULL;
}



//===========================================================================
// AnimDynaPhysLoader::SetRegdListener
//===========================================================================
// Description:
//		Informs the loader its listener is.
//
// Constraints:
//
// Parameters:
//		Pointer to the new listener. Integer for the data it sends to it OnChunkLoaded()
//
// Return:
//
//===========================================================================
void AnimDynaPhysLoader::SetRegdListener( ChunkListenerCallback* pListenerCB,
											int iUserData )
{
	//
	// Follow protocol; notify old Listener, that it has been 
	// "disconnected".
	//
	if( mpListenerCB != NULL )
	{
	  mpListenerCB->OnChunkLoaded( NULL, iUserData, 0 );
	}
	mpListenerCB  = pListenerCB;
	mUserData     = iUserData;
}

//===========================================================================
// AnimDynaPhysLoader::ModRegdListener
//===========================================================================
// Description:
//		Changes the data that gets sent to the listener
//
// Constraints:
//		Listener must have been set already via SetRegdListener
//
// Parameters:
//		Pointer to the listener. Integer holding the new data
//
// Return:
//
//===========================================================================
void AnimDynaPhysLoader::ModRegdListener( ChunkListenerCallback* pListenerCB, int iUserData )
{
   rAssert( pListenerCB == mpListenerCB );

   mUserData = iUserData;
}

void AnimDynaPhysLoader::SetShadowElement( const char* compDrawName, 
                                    const char* drawableElementName )
{
    s_ShadowList.insert( tName::MakeUID( compDrawName ), tName::MakeUID( drawableElementName ) );
}

void AnimDynaPhysLoader::ClearShadowList()
{
    s_ShadowList.clear();
}

tUID AnimDynaPhysLoader::GetShadowElement( tUID compDrawName )
{
    tUID elementName;
    Map< tUID, tUID >::const_iterator it;
    it = s_ShadowList.find( compDrawName );
    if ( it != s_ShadowList.end() )
        elementName = it->second;
    else
        elementName = 0;

    return elementName;
}



///////////////////////////////////////////////////////////
// AnimDyaPhysWrapperLoader methods
///////////////////////////////////////////////////////////

AnimDynaPhysWrapperLoader::AnimDynaPhysWrapperLoader() : tSimpleChunkHandler(SRR2::ChunkID::ANIM_DSG_WRAPPER)
{

    mpListenerCB  = NULL;
    mUserData     = -1;

	mpCompDLoader = new tCompositeDrawableLoader;
	mpCompDLoader->AddRef();
   
	mpMCLoader    = new tMultiControllerLoader;
    mpMCLoader->AddRef();

	mpSkelLoader = new tSkeletonLoader;
	mpSkelLoader->AddRef();
    
	mpAnimationLoader = new tAnimationLoader;
	mpAnimationLoader->AddRef();

	mpCollObjLoader = new sim::CollisionObjectLoader;
	mpCollObjLoader->AddRef();

	mpPhysObjLoader = new sim::PhysicsObjectLoader;
	mpPhysObjLoader->AddRef();

    mpFCLoader    = new tFrameControllerLoader;
    mpFCLoader->AddRef();
}

AnimDynaPhysWrapperLoader::~AnimDynaPhysWrapperLoader()
{
	mpCollObjLoader->ReleaseVerified();
	mpPhysObjLoader->ReleaseVerified();
	mpCompDLoader->ReleaseVerified();
	mpMCLoader->ReleaseVerified();
	mpSkelLoader->ReleaseVerified();
	mpAnimationLoader->ReleaseVerified();
	mpFCLoader->ReleaseVerified();
}



tEntity* AnimDynaPhysWrapperLoader::LoadObject( tChunkFile* file, tEntityStore* store )
{
    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
	AnimDynaPhysWrapper* wrapper = new AnimDynaPhysWrapper;
    HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
	rAssert( file != NULL );
	rAssert( store != NULL );

	// Grab the name
	char buffer[256];
	file->GetPString( buffer );

	wrapper->SetName( buffer );

	// Grab version info
	int version = file->GetLong();
	// Grab the flag that tell us whether or not the object has alpha or not
	int hasAlpha = file->GetLong();
	
	wrapper->mHasAlpha = ( hasAlpha != 0x00000000 );
    bool collisionObjectFound = false;

	while( file->ChunksRemaining() )
	{
		file->BeginChunk();
		switch( file->GetCurrentID() )
		{
		case P3D_COMPOSITE_DRAWABLE:
			{
				// We will save the composite drawable since we need it 
				// for our DSG objects
				rAssertMsg( wrapper->mCompDraw == NULL, "There must only be one composite drawable in the wrapper!" );
				wrapper->mCompDraw = (tCompositeDrawable*)mpCompDLoader->LoadObject( file, store );
				wrapper->mCompDraw->AddRef();
				store->Store( wrapper->mCompDraw );
				rAssert( wrapper->mCompDraw != NULL );
			}	
			break;
		case P3D_MULTI_CONTROLLER:
			{
				rAssert( wrapper->mMultiController == NULL );
				wrapper->mMultiController = (tMultiController*)mpMCLoader->LoadObject( file, store );
				rAssert( wrapper->mMultiController != NULL );
				// This should be inserted into the list of world render multicontrollers that
				// get advanced every frame
				wrapper->mMultiController->SetCycleMode( FORCE_CYCLIC );
				wrapper->mMultiController->AddRef();

                bool collision = store->TestCollision( wrapper->mMultiController->GetUID(), wrapper->mMultiController );
                if( !collision )
                {
                    store->Store( wrapper->mMultiController );
                }
                else
                {
                    HandleCollision( wrapper->mMultiController );
                }
			}
			break;
		case Pure3D::Animation::FrameControllerData::FRAME_CONTROLLER:
			{
				tFrameController* pFC = static_cast< tFrameController* >( mpFCLoader->LoadObject( file, store ) );
				rAssert( pFC != NULL );
				store->Store( pFC );
			}
			break;
		case Simulation::Collision::OBJECT:
			{
                collisionObjectFound = true;
                rAssert( wrapper->mCollisionObject == NULL );
				wrapper->mCollisionObject = static_cast< sim::CollisionObject* > (mpCollObjLoader->LoadObject( file, store ));
                rAssert( wrapper->mCollisionObject != NULL );
                wrapper->mCollisionObject->AddRef();
				store->Store( wrapper->mCollisionObject );
			}
			break;
		case Pure3D::BillboardObject::QUAD_GROUP:
			{
				// Remember that we have our own billboard loader
				// make sure that these don't get dumped into the DSG, override first

				// Grab the loader
				BillboardWrappedLoader::OverrideLoader( true );
                BillboardWrappedLoader* pBBQLoader = static_cast<BillboardWrappedLoader*>(AllWrappers::GetInstance()->mpLoader(AllWrappers::msBillboard));

                tBillboardQuadGroup* pGroup = static_cast<tBillboardQuadGroup*>( pBBQLoader->LoadObject( file, store ) );
				rAssert( pGroup != NULL );
				store->Store( pGroup );	

				// Set the loader back to its normal state
				BillboardWrappedLoader::OverrideLoader( false );
			}
			break;
		case Pure3D::Mesh::MESH:
			{
				GeometryWrappedLoader* pGeoLoader = (GeometryWrappedLoader*)AllWrappers::GetInstance()->mpLoader( AllWrappers::msGeometry );
				tGeometry* pGeo = static_cast<tGeometry*>(pGeoLoader->LoadObject( file, store ) );			
				rAssert( pGeo != NULL );
				store->Store( pGeo );
			}
			break;
		case Pure3D::Animation::AnimationData::ANIMATION:
			{
				tAnimation* pAnimation = static_cast< tAnimation*> ( mpAnimationLoader->LoadObject( file, store ) );
				rAssert( pAnimation != NULL );
				store->Store( pAnimation );
                wrapper->SetHasAnimation( true );
			}
			break;
		case Simulation::Physics::OBJECT:

			rAssert( wrapper->mPhysicsObject == NULL );
			wrapper->mPhysicsObject = (sim::PhysicsObject*)mpPhysObjLoader->LoadObject( file,store );
			rAssert( wrapper->mPhysicsObject != NULL );
			store->Store( wrapper->mPhysicsObject );
            wrapper->mPhysicsObject->AddRef ();

			break;
		case P3D_SKELETON:		
			{

				tSkeleton* pSkeleton = static_cast< tSkeleton* > (mpSkelLoader->LoadObject( file, store ) );
				rAssert( pSkeleton != NULL );
				store->Store( pSkeleton );
			}
			break;
		default:
			rAssertMsg( false, "Unknown chunk in animated wrapper chunk file");
			break;
		};
        file->EndChunk();
	}
    if ( collisionObjectFound == false )
    {
        rReleasePrintf("WARNING - Stateprop %s is missing a collision volume!\n", buffer );
    }
	return wrapper;
}
//===========================================================================
// AnimDynaPhysWrapperLoader::SetRegdListener
//===========================================================================
// Description:
//		Informs the loader its listener is.
//
// Constraints:
//
// Parameters:
//		Pointer to the new listener. Integer for the data it sends to it OnChunkLoaded()
//
// Return:
//
//===========================================================================
void AnimDynaPhysWrapperLoader::SetRegdListener( ChunkListenerCallback* pListenerCB,
											int iUserData )
{
	//
	// Follow protocol; notify old Listener, that it has been 
	// "disconnected".
	//
	if( mpListenerCB != NULL )
	{
	  mpListenerCB->OnChunkLoaded( NULL, iUserData, 0 );
	}
	mpListenerCB  = pListenerCB;
	mUserData     = iUserData;
}

//===========================================================================
// AnimDynaPhysWrapperLoader::ModRegdListener
//===========================================================================
// Description:
//		Changes the data that gets sent to the listener
//
// Constraints:
//		Listener must have been set already via SetRegdListener
//
// Parameters:
//		Pointer to the listener. Integer holding the new data
//
// Return:
//
//===========================================================================
void AnimDynaPhysWrapperLoader::ModRegdListener( ChunkListenerCallback* pListenerCB, int iUserData )
{
   rAssert( pListenerCB == mpListenerCB );

   mUserData = iUserData;
}


///////////////////////////////////////////////////////////
// ANIM_OBJ_DSG_WRAPPER methods
///////////////////////////////////////////////////////////

AnimObjDSGWrapperLoader::AnimObjDSGWrapperLoader() : tSimpleChunkHandler(SRR2::ChunkID::ANIM_OBJ_DSG_WRAPPER)
{

    mpListenerCB  = NULL;
    mUserData     = -1;

	mpCompDLoader = new tCompositeDrawableLoader;
	mpCompDLoader->AddRef();
   
	mpMCLoader    = new tMultiControllerLoader;
    mpMCLoader->AddRef();

	mpSkelLoader = new tSkeletonLoader;
	mpSkelLoader->AddRef();
    
	mpAnimationLoader = new tAnimationLoader;
	mpAnimationLoader->AddRef();

	mpCollObjLoader = new sim::CollisionObjectLoader;
	mpCollObjLoader->AddRef();

	mpPhysObjLoader = new sim::PhysicsObjectLoader;
	mpPhysObjLoader->AddRef();

    mpFCLoader    = new tFrameControllerLoader;
    mpFCLoader->AddRef();

    mpStatePropLoader = new CStatePropDataLoader;
    mpStatePropLoader->AddRef();

    mpFactoryLoader = new tAnimatedObjectFactoryLoader;
 	mpAnimObjectLoader = new tAnimatedObjectLoader;

}

AnimObjDSGWrapperLoader::~AnimObjDSGWrapperLoader()
{
	mpCollObjLoader->ReleaseVerified();
	mpPhysObjLoader->ReleaseVerified();
	mpCompDLoader->ReleaseVerified();
	mpMCLoader->ReleaseVerified();
	mpSkelLoader->ReleaseVerified();
	mpAnimationLoader->ReleaseVerified();
	mpFCLoader->ReleaseVerified();
    mpStatePropLoader->ReleaseVerified();
    mpFactoryLoader->ReleaseVerified();
 	mpAnimObjectLoader->ReleaseVerified();
}



tEntity* AnimObjDSGWrapperLoader::LoadObject( tChunkFile* file, tEntityStore* store )
{
    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
	AnimDynaPhysWrapper* wrapper = new AnimDynaPhysWrapper;
    HeapMgr()->PopHeap( GMA_LEVEL_OTHER );
	rAssert( file != NULL );
	rAssert( store != NULL );

	// Grab the name
	char buffer[256];
	file->GetPString( buffer );

	wrapper->SetName( buffer );



	// Grab version info
    int version = file->GetUChar();
	// Grab the flag that tell us whether or not the object has alpha or not
    int hasAlpha = file->GetUChar();
	
	wrapper->mHasAlpha = ( hasAlpha != 0x00000000 );

	while( file->ChunksRemaining() )
	{
		file->BeginChunk();
        unsigned int id = file->GetCurrentID();
		switch( id )
		{
		case P3D_COMPOSITE_DRAWABLE:
			{
				// We will save the composite drawable since we need it 
				// for our DSG objects
				rAssertMsg( wrapper->mCompDraw == NULL, "There must only be one composite drawable in the wrapper!" );
				wrapper->mCompDraw = (tCompositeDrawable*)mpCompDLoader->LoadObject( file, store );
				wrapper->mCompDraw->AddRef();
				store->Store( wrapper->mCompDraw );
				rAssert( wrapper->mCompDraw != NULL );
			}	
			break;
		case P3D_MULTI_CONTROLLER:
			{
				rAssert( wrapper->mMultiController == NULL );
				wrapper->mMultiController = (tMultiController*)mpMCLoader->LoadObject( file, store );
				rAssert( wrapper->mMultiController != NULL );
				// This should be inserted into the list of world render multicontrollers that
				// get advanced every frame
				wrapper->mMultiController->SetCycleMode( FORCE_CYCLIC );
				wrapper->mMultiController->AddRef();
                bool collision = store->TestCollision( wrapper->mMultiController->GetUID(), wrapper->mMultiController );
                if( !collision )
                {
                    store->Store( wrapper->mMultiController );
                }
                else
                {
                    HandleCollision( wrapper->mMultiController );
                }
			}
			break;
		case Pure3D::Animation::FrameControllerData::FRAME_CONTROLLER:
			{
				tFrameController* pFC = static_cast< tFrameController* >( mpFCLoader->LoadObject( file, store ) );
				rAssert( pFC != NULL );
				store->Store( pFC );
			}
			break;
		case Simulation::Collision::OBJECT:
			{

                rAssert( wrapper->mCollisionObject == NULL );
				wrapper->mCollisionObject = static_cast< sim::CollisionObject* > (mpCollObjLoader->LoadObject( file, store ));
                rAssert( wrapper->mCollisionObject != NULL );
                wrapper->mCollisionObject->AddRef();
				store->Store( wrapper->mCollisionObject );
                //wrapper->mCollisionObject->AddRef ();
			}
			break;
		case Pure3D::BillboardObject::QUAD_GROUP:
			{
				// Remember that we have our own billboard loader
				// make sure that these don't get dumped into the DSG, override first

				// Grab the loader
				BillboardWrappedLoader::OverrideLoader( true );
                BillboardWrappedLoader* pBBQLoader = static_cast<BillboardWrappedLoader*>(AllWrappers::GetInstance()->mpLoader(AllWrappers::msBillboard));

                tBillboardQuadGroup* pGroup = static_cast<tBillboardQuadGroup*>( pBBQLoader->LoadObject( file, store ) );
				rAssert( pGroup != NULL );
				store->Store( pGroup );	

				// Set the loader back to its normal state
				BillboardWrappedLoader::OverrideLoader( false );
			}
			break;
		case Pure3D::Mesh::MESH:
			{
				GeometryWrappedLoader* pGeoLoader = (GeometryWrappedLoader*)AllWrappers::GetInstance()->mpLoader( AllWrappers::msGeometry );
				tGeometry* pGeo = static_cast<tGeometry*>(pGeoLoader->LoadObject( file, store ) );			
				rAssert( pGeo != NULL );
				store->Store( pGeo );
			}
			break;
		case Pure3D::Animation::AnimationData::ANIMATION:
			{
				tAnimation* pAnimation = static_cast< tAnimation*> ( mpAnimationLoader->LoadObject( file, store ) );
				rAssert( pAnimation != NULL );
				store->Store( pAnimation );
			}
			break;
		case Simulation::Physics::OBJECT:

			rAssert( wrapper->mPhysicsObject == NULL );
			wrapper->mPhysicsObject = (sim::PhysicsObject*)mpPhysObjLoader->LoadObject( file,store );
			rAssert( wrapper->mPhysicsObject != NULL );
			store->Store( wrapper->mPhysicsObject );
            wrapper->mPhysicsObject->AddRef ();

			break;
		case P3D_SKELETON:		
			{

				tSkeleton* pSkeleton = static_cast< tSkeleton* > (mpSkelLoader->LoadObject( file, store ) );
				rAssert( pSkeleton != NULL );
				store->Store( pSkeleton );
			}
			break;
        case StateProp::STATEPROP:
            {
                CStatePropData* pPropData = static_cast< CStatePropData* > (mpStatePropLoader->LoadObject( file, store ) );
                rAssert( dynamic_cast< CStatePropData* >( pPropData ) != NULL );
                rAssert( pPropData != NULL );
                tRefCounted::Assign(wrapper->mStatePropData, pPropData);
                store->Store( pPropData );
            }
            break;
        case Pure3D::AnimatedObject::FACTORY:
            {

                tEntity* entity = mpFactoryLoader->LoadObject( file, store );
                rAssert( dynamic_cast< tAnimatedObjectFactory* >(entity) != NULL );
                store->Store( entity );
            }
            break;
		case Pure3D::AnimatedObject::OBJECT:
			{
				tEntity* pEntity = mpAnimObjectLoader->LoadObject( file, store );
				rAssert( pEntity != NULL );
				store->Store( pEntity );
			}
			break;
		default:
			rAssertMsg( false, "Unknown chunk in animated wrapper chunk file");
			break;
		};
        file->EndChunk();
	}

	return wrapper;
}
//===========================================================================
// AnimObjDSGWrapperLoader::SetRegdListener
//===========================================================================
// Description:
//		Informs the loader its listener is.
//
// Constraints:
//
// Parameters:
//		Pointer to the new listener. Integer for the data it sends to it OnChunkLoaded()
//
// Return:
//
//===========================================================================
void AnimObjDSGWrapperLoader::SetRegdListener( ChunkListenerCallback* pListenerCB,
											int iUserData )
{
	//
	// Follow protocol; notify old Listener, that it has been 
	// "disconnected".
	//
	if( mpListenerCB != NULL )
	{
	  mpListenerCB->OnChunkLoaded( NULL, iUserData, 0 );
	}
	mpListenerCB  = pListenerCB;
	mUserData     = iUserData;
}

//===========================================================================
// AnimObjDSGWrapperLoader::ModRegdListener
//===========================================================================
// Description:
//		Changes the data that gets sent to the listener
//
// Constraints:
//		Listener must have been set already via SetRegdListener
//
// Parameters:
//		Pointer to the listener. Integer holding the new data
//
// Return:
//
//===========================================================================
void AnimObjDSGWrapperLoader::ModRegdListener( ChunkListenerCallback* pListenerCB, int iUserData )
{
   rAssert( pListenerCB == mpListenerCB );

   mUserData = iUserData;
}

//===========================================================================
// AnimObjDSGWrapperLoader::IsMissionProp
//===========================================================================
// Description:
//		Returns boolean indicating if the prop is used in a mission
//
// Constraints:
//		Only one stateprop mission prop - the powerbox
//
// Parameters:
//		none
//
// Return:
//      bool indicating if this prop is used in a mission
//
//===========================================================================
bool AnimDynaPhysLoader::IsMissionProp( const char* name )const
{
    if ( strcmp( name, "l1z6_powerbox_Shape" ) == 0 )
        return true;
    else
        return false;
}

