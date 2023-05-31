//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   InstParticleSystemLoader
//
// Description: Loads particle systems (and also breakable objects)
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <render/loaders/InstParticleSystemLoader.h>
#include <constants/srrchunks.h>
#include <constants/chunkids.hpp>
#include <render/particles/particlemanager.h>
#include <p3d/effects/particlesystem.hpp>
#include <p3d/effects/particleloader.hpp>
#include <p3d/chunkfile.hpp>
#include <p3d/utility.hpp>
#include <constants/srrchunks.h>
#include <memory/srrmemory.h>
#include <render/Loaders/AllWrappers.h>
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
// InstParticleSystemLoader::InstParticleSystemLoader
//===========================================================================
// Description:
//      InstParticleSystemLoader constructor
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================
InstParticleSystemLoader::InstParticleSystemLoader() 
: tSimpleChunkHandler(SRR2::ChunkID::INST_PARTICLE_SYSTEM)
{
    mpListenerCB = NULL;
    mUserData = -1;
}
//===========================================================================
// InstParticleSystemLoader::~InstParticleSystemLoader
//===========================================================================
// Description:
//      InstParticleSystemLoader destructor
//
// Constraints:
//
// Parameters:
//
// Return:
//
//===========================================================================
InstParticleSystemLoader::~InstParticleSystemLoader()
{

}

//===========================================================================
// InstParticleSystemLoader::SetRegdListener
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

void InstParticleSystemLoader::SetRegdListener( ChunkListenerCallback* pListenerCB,
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
// InstParticleSystemLoader::SetRegdListener
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

void InstParticleSystemLoader::ModRegdListener( ChunkListenerCallback* pListenerCB,
                         int   iUserData )
{
	rAssert( pListenerCB == mpListenerCB );
	mUserData = iUserData;
}
//===========================================================================
// InstParticleSystemLoader::LoadObject
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
//      tParticleSystem
//
//===========================================================================

tEntity* InstParticleSystemLoader::LoadObject(tChunkFile* file, tEntityStore* store)
{
    IEntityDSG::msDeletionsSafe=true;
MEMTRACK_PUSH_GROUP( "Particle Systems" );

    tParticleSystemFactoryLoader* pFactoryLoader = new (GMA_TEMP) tParticleSystemFactoryLoader;
    tParticleSystemFactory* pFactory = NULL;
    tFrameControllerLoader* pControllerLoader = new (GMA_TEMP) tFrameControllerLoader;
    tEffectController* pController = NULL;

    unsigned long id = file->GetLong();
    ParticleEnum::ParticleID type = ParticleEnum::ParticleID(id);

    int maxInstances = static_cast<int>( file->GetLong() );

    while( file->ChunksRemaining() )
    {
        file->BeginChunk();
        switch( file->GetCurrentID() )
        {
        case Pure3D::ParticleSystem::SYSTEM_FACTORY:
        {

            pFactory = static_cast<tParticleSystemFactory*> (pFactoryLoader->LoadObject( file, store ) );
            bool collision = store->TestCollision( pFactory->GetUID(), pFactory );
            if( !collision )
            {
                store->Store( pFactory );
            }
            else
            {
                pFactory->Release();
                pFactory = 0;
            }
            break;
        }
        case Pure3D::Animation::FrameControllerData::FRAME_CONTROLLER:
        {                  
            if ( pController == NULL )
            {
                tFrameController* pfc = (tFrameController*)pControllerLoader->LoadObject(file,store);
                pController = static_cast<tEffectController*>(pfc);
                if ( pController != NULL )
                {
                    bool collision = store->TestCollision( pController->GetUID(), pController );
                    if( !collision )
                    {
                        store->Store( pController );
                    }
                    else
                    {
                        pController->Release();
                        pController = 0;
                    }
                }
            }
            break;
        }
		// Textures, shaders
        default:
            unsigned int fileId = file->GetCurrentID();
            tChunkHandler* pChunkHandler = p3d::loadManager->GetP3DHandler()->GetHandler( file->GetCurrentID() );
            if ( pChunkHandler != NULL )
                tLoadStatus status = pChunkHandler->Load( file, store );

            break;
        }
        file->EndChunk();
    }

    if(pFactory)
    {
        ParticleManager::GetInstance()->InitializeSystem( type, pFactory, pController, maxInstances );
    }
    
    pFactoryLoader->ReleaseVerified();
    pControllerLoader->ReleaseVerified();

MEMTRACK_POP_GROUP( "Particle Systems" );

    IEntityDSG::msDeletionsSafe=false;
    return NULL;
}
