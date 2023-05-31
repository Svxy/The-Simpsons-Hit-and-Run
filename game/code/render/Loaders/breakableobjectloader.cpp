//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   BreakableObjectLoader
//
// Description: Loads breakable objects
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================
#include <p3d/chunkfile.hpp>
#include <render/loaders/breakableobjectloader.h>
#include <constants/srrchunks.h>
#include <constants/chunkids.hpp>
#include <constants/chunks.h>
#include <render/particles/particlemanager.h>
#include <p3d/anim/animatedobject.hpp>
#include <p3d/anim/compositedrawable.hpp>
#include <p3d/anim/skeleton.hpp>
#include <p3d/inventory.hpp>
#include <p3d/utility.hpp>
#include <constants/srrchunks.h>
#include <memory/srrmemory.h>
#include <render/Loaders/AllWrappers.h>
#include <p3d/effects/particleloader.hpp>
#include <render/breakables/BreakablesManager.h>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Member Functions
//===========================================================================

//===========================================================================
// BreakableObjectLoader::BreakableObjectLoader
//===========================================================================
// Description:
//      BreakableObjectLoader constructor
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================
BreakableObjectLoader::BreakableObjectLoader() 
: tSimpleChunkHandler(SRR2::ChunkID::BREAKABLE_OBJECT)
{
    mpListenerCB = NULL;
    mUserData = -1;

    mpFactoryLoader = new (GMA_PERSISTENT)tAnimatedObjectFactoryLoader;
 	mpAnimObjectLoader = new (GMA_PERSISTENT)tAnimatedObjectLoader;

    mpControllerLoader = new (GMA_PERSISTENT)tFrameControllerLoader;

	mpCompDrawLoader = new (GMA_PERSISTENT)tCompositeDrawableLoader;
	mpP3DGeoLoader = new (GMA_PERSISTENT)tGeometryLoader;

    mpSkelLoader = new (GMA_PERSISTENT)tSkeletonLoader;
    mpAnimLoader = new (GMA_PERSISTENT)tAnimationLoader;

    mpParticleSystemLoader = new (GMA_PERSISTENT)tParticleSystemLoader;
    mpParticleSystemFactoryLoader = new (GMA_PERSISTENT)tParticleSystemFactoryLoader;



}
//===========================================================================
// BreakableObjectLoader::~BreakableObjectLoader
//===========================================================================
// Description:
//      BreakableObjectLoader destructor
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================
BreakableObjectLoader::~BreakableObjectLoader()
{
    mpFactoryLoader->ReleaseVerified();
 	mpAnimObjectLoader->ReleaseVerified();

    mpControllerLoader->ReleaseVerified();

	mpCompDrawLoader->ReleaseVerified();
	mpP3DGeoLoader->ReleaseVerified();

    mpSkelLoader->ReleaseVerified();
    mpAnimLoader->ReleaseVerified();

    mpParticleSystemLoader->ReleaseVerified();
    mpParticleSystemFactoryLoader->ReleaseVerified();

}

//===========================================================================
// BreakableObjectLoader::SetRegdListener
//===========================================================================
// Description:
//
//
// Constraints:
//
//
// Parameters:
//
//
// Return:
//
//===========================================================================

void BreakableObjectLoader::SetRegdListener( ChunkListenerCallback* pListenerCB,
                         int   iUserData )
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
// BreakableObjectLoader::SetRegdListener
//===========================================================================
// Description:
//
//
// Constraints:
//
//
// Parameters:
//
//
// Return:
//
//===========================================================================

void BreakableObjectLoader::ModRegdListener( ChunkListenerCallback* pListenerCB,
                         int   iUserData )
{
	rAssert( pListenerCB == mpListenerCB );
	mUserData = iUserData;
}
//===========================================================================
// BreakableObjectLoader::LoadObject
//===========================================================================
// Description:
//
//
// Constraints:
//
//
// Parameters:
//      tChunkFile - contains the input particle system to parse
//      tEntityStore - unused???
//
// Return:
//      An object that will get put into the inventory. Here we will put in a 
//      tBreakableObject
//
//===========================================================================

tEntity* BreakableObjectLoader::LoadObject(tChunkFile* file, tEntityStore* store)
{
    IEntityDSG::msDeletionsSafe=true;
MEMTRACK_PUSH_GROUP( "Breakables" );


    tAnimatedObjectFactory* pFactory = NULL;
    tAnimatedObjectFrameController* pController = NULL;


    unsigned long id = file->GetLong();
    BreakablesEnum::BreakableID type = BreakablesEnum::BreakableID( id );
    if ( GetBreakablesManager()->IsLoaded( type ) )
    {
        GetBreakablesManager()->AddToZoneList( type );
        MEMTRACK_POP_GROUP( "Breakables" );
        return NULL;
    }

    int maxInstances = static_cast<int>( file->GetLong() );

    while( file->ChunksRemaining() )
    {
        file->BeginChunk();
        switch( file->GetCurrentID() )
        {
        case Pure3D::AnimatedObject::FACTORY:
            {
                rAssert( pFactory == NULL );
                tEntity* entity = mpFactoryLoader->LoadObject( file, store );
                pFactory = static_cast<tAnimatedObjectFactory*> (entity);
                rAssert( dynamic_cast< tAnimatedObjectFactory* >(entity) != NULL );
            }
            break;
        case Pure3D::ParticleSystem::SYSTEM_FACTORY:
            {
                tEntity* pParticleSystemFactory = mpParticleSystemFactoryLoader->LoadObject( file, store );
                rAssert( pParticleSystemFactory != NULL );
                bool collision = store->TestCollision( pParticleSystemFactory->GetUID(), pParticleSystemFactory );
                if( !collision )
                {
                    store->Store( pParticleSystemFactory );
                }
                else
                {
                    HandleCollision( pParticleSystemFactory );
                }

            }
            break;
        case Pure3D::ParticleSystem::SYSTEM:
            {
                tEntity* pParticleSystem = mpParticleSystemLoader->LoadObject( file, store );
                rAssert( pParticleSystem != NULL );
                bool collision = store->TestCollision( pParticleSystem->GetUID(), pParticleSystem );
                if( !collision )
                {
                    store->Store( pParticleSystem );
                }
                else
                {
                    HandleCollision( pParticleSystem );
                }
                
            }
            break;
        case Pure3D::Animation::FrameControllerData::FRAME_CONTROLLER:
            if ( pController == NULL )
            {
                tEntity* pFC = mpControllerLoader->LoadObject( file, store ) ;
				pController = static_cast< tAnimatedObjectFrameController* > ( pFC );
                store->Store( pFC );
            }
            break;
		case Pure3D::AnimatedObject::OBJECT:
			{
				tEntity* pEntity = mpAnimObjectLoader->LoadObject( file, store );
				rAssert( pEntity != NULL );
				store->Store( pEntity );
			}
			break;
		case P3D_COMPOSITE_DRAWABLE:
			{
				tEntity* entity = mpCompDrawLoader->LoadObject( file, store );
                tCompositeDrawable* pCompDraw = static_cast< tCompositeDrawable* >( entity );
                rAssert( dynamic_cast< tCompositeDrawable* >(entity) != NULL );
                rAssert( pCompDraw != NULL );
				store->Store( pCompDraw );
			}
			break;
		case Pure3D::Mesh::MESH:
			{
				tEntity* pGeo = mpP3DGeoLoader->LoadObject( file, store );
                bool collision = store->TestCollision( pGeo->GetUID(), pGeo );
                if( !collision )
                {
                    store->Store( pGeo );
                }
                else
                {
                    HandleCollision( pGeo );
                }
			}
			break;
        case P3D_SKELETON:
            {
                tEntity* pSkel = mpSkelLoader->LoadObject( file, store );
                rAssert( pSkel != NULL );
                store->Store( pSkel );
            }
            break;
        case Pure3D::Animation::AnimationData::ANIMATION:
            {
                tEntity* pAnim = mpAnimLoader->LoadObject( file, store );
                rAssert( pAnim != NULL );
                store->Store( pAnim );
            }
            break;
        case P3D_MULTI_CONTROLLER:
            break;

        default:
            int currchunk = file->GetCurrentID();
			rAssertMsg(false, "Error-Unknown chunk in breakable object");

            break;
        }
        file->EndChunk();
    }

    BreakablesManager::GetInstance()->AllocateBreakables( type, pFactory, pController, maxInstances );
    
	if ( pFactory != NULL )
	{
		pFactory->ReleaseVerified();
	}

MEMTRACK_POP_GROUP("Breakables");
		
    IEntityDSG::msDeletionsSafe=false;
    return NULL;
}
