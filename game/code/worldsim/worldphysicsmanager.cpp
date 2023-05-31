//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        worldphysicsmanager.cpp
//
// Description: bleek
//
// History:     May 1, 2002 - created, gmayer
//
//=============================================================================

//========================================
// System Includes
//========================================

#include <raddebug.hpp>

#include <simcommon/simstatearticulated.hpp>         
#include <simcommon/simenvironment.hpp>         
#include <simcollision/collisionmanager.hpp>
#include <simcommon/physicsproperties.hpp>

#include <simcollision/collisionvolume.hpp>
#include <simcollision/collisiondisplay.hpp>
#include <simcommon/simutility.hpp>

//========================================
// Project Includes
//========================================
#include <render/DSG/animcollisionentitydsg.h>
#include <worldsim/worldphysicsmanager.h>
#include <worldsim/worldcollisionsolveragent.h>

#include <worldsim/groundplanepool.h>

#include <worldsim/avatarmanager.h>
#include <worldsim/vehiclecentral.h>

#include <memory/srrmemory.h>

#include <worldsim/redbrick/vehicle.h>
#include <worldsim/redbrick/physicslocomotion.h> // just for debug prinout

#include <camera/supercammanager.h>

#include <debug/profiler.h>
#include <debug/debuginfo.h>


#include <render/Culling/ReserveArray.h>
#include <render/DSG/StaticPhysDSG.h>
#include <render/DSG/StatePropDSG.h>
#include <render/DSG/FenceEntityDSG.h>
#include <render/IntersectManager/IntersectManager.h>


#include <worldsim/character/charactermanager.h>
#include <worldsim/character/character.h>
#include <ai/actionbuttonmanager.h>

#include <constants/maxplayers.h>
#include <constants/maxnpccharacters.h>

#include <sound/soundmanager.h>

#include <mission/gameplaymanager.h>

// TODO
// can remove
// just here to hack in a light so I can see the texture damage states better
#include <render/RenderManager/RenderManager.h>
#include <render/RenderManager/RenderLayer.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

WorldPhysicsManager* WorldPhysicsManager::spInstance = 0;

//=============================================================================
// WorldPhysicsManager::WorldPhysicsManager
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      WorldPhysicsManager
//
//=============================================================================
WorldPhysicsManager::WorldPhysicsManager()
{
    mCollisionDistanceCGS = 2.0f;
    mWorldUp.Set(0.0f, 1.0f, 0.0f);
    mTotalTime = 0.0f;
    updateFrame = 0;
    mLoopTime = 0.0f;
    
    sim::InitializeSimulation(MetersUnits);

    //SetSimUnits();

    // move to bootupcontext onstart
    //Init();
    mLastTime = 30;
}


//=============================================================================
// WorldPhysicsManager::~WorldPhysicsManager
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      WorldPhysicsManager
//
//=============================================================================
WorldPhysicsManager::~WorldPhysicsManager()
{    
    mSimEnvironment = 0;

    int i, j;
    for(i = 0; i < mNumCollisionAreas; i++)
    {
        for( j = 0; j < mMaxFencePerArea; j++ )
        {
            mFences[i][j].mFenceSimState->Release();
        }

        delete [] mCurrentStatics[i];
        delete [] mCurrentAnimCollisions[i];
        delete [] mCurrentUpdateAnimCollisions[i];
        delete [] mFences[i];
        delete [] mCurrentDynamics[i];
    }
    delete [] mCurrentStatics;
    delete [] mCurrentAnimCollisions;
    delete [] mCurrentUpdateAnimCollisions;
    delete [] mFences;
    delete [] mFencesInEachArea;
    delete [] mCurrentDynamics;
    delete mGroundPlanePool;

    for( i = 0; i < mMaxFencePerArea; i++ )
    {
        mFenceDSGResults[i]->Release();
    }
    delete[] mFenceDSGResults;

    mFencePhysicsProperties->Release();

    mCollisionManager->Release();

    mpWorldCollisionSolverAgentManager->Release();
  
    sim::CleanupLineDrawing ();

    delete [] mCollisionAreaAllocated;
    delete [] mCollisionAreaActive;

    sim::ResetSimulation ();
}


//=============================================================================
// WorldPhysicsManager::GetInstance
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      WorldPhysicsManager
//
//=============================================================================
WorldPhysicsManager* WorldPhysicsManager::GetInstance()
{
    rAssert(spInstance);
    return spInstance;
}


//=============================================================================
// WorldPhysicsManager::CreateInstance
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      WorldPhysicsManager
//
//=============================================================================
WorldPhysicsManager* WorldPhysicsManager::CreateInstance()
{
    rAssert(spInstance == 0);

#ifdef RAD_GAMECUBE
    HeapMgr()->PushHeap( GMA_GC_VMM );
    spInstance = new WorldPhysicsManager;
    HeapMgr()->PopHeap( GMA_GC_VMM );
#else
    HeapMgr()->PushHeap( GMA_PERSISTENT );
    spInstance = new WorldPhysicsManager;
    HeapMgr()->PopHeap( GMA_PERSISTENT );
#endif
    rAssert(spInstance);
    
    return spInstance;

}


//=============================================================================
// WorldPhysicsManager::Init
//=============================================================================
// Description: 
// TODO - need this? - where to call it from?
// ok to call from CreateSingletons
//
// no - it would be called from the a context OnStart if necessary
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::Init()    
{    
    mSimEnvironment = sim::SimEnvironment::GetDefaultSimEnvironment();

    rAssert(mSimEnvironment);
    //mSimEnvironment->AddRef();    // TODO
    mSimEnvironment->SetCollisionDistanceCGS(mCollisionDistanceCGS);
       
    //mSimEnvironment->SetGravityCGS(0.0f, -981.0f, 0.0f);
    //mSimEnvironment->SetGravityCGS(0.0f, -2000.0f, 0.0f);
	mSimEnvironment->SetGravityCGS(0.0f, -1600.0f, 0.0f);   // plum's setting
	//mSimEnvironment->SetGravityCGS(0.0f, -2500.0f, 0.0f);
    mTimerTime = 0.0f;
    mTimerOn = false;
    
    InitCollisionManager();

    sim::SetupLineDrawing ();
}


//=============================================================================
// WorldPhysicsManager::StartTimer
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::StartTimer()
{
    mTimerOn = true;
}


//=============================================================================
// WorldPhysicsManager::StopTimer
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::StopTimer()
{
    mTimerOn = false;
}


//=============================================================================
// WorldPhysicsManager::ResetTimer
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::ResetTimer()
{
    mTimerTime = 0.0f;
}


//=============================================================================
// WorldPhysicsManager::ToggleTimerState
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::ToggleTimerState()
{
    if(mTimerOn)
    {
        mTimerOn = false;
    }
    else if(mTimerTime > 0.0f)
    {
        mTimerTime = 0.0f;
    }
    else
    {
        mTimerOn = true;
    }
}

int WorldPhysicsManager::ApplyForceToDynamics( int collisionAreaIndex, 
                                               const rmt::Vector& position, 
                                               float radius, 
                                               const rmt::Vector& direction, 
                                               float force, 
                                               WorldPhysicsManager::NumObjectsHit* pObjectsHit,
                                               CollisionEntityDSGList* pCollisionEntityList )
{
    rAssert( collisionAreaIndex > WorldPhysicsManager::INVALID_COLLISION_AREA );

    int i;
    int numObjectsAffected = 0;
    float radiusSqr = radius * radius;

    if( pCollisionEntityList != NULL )
    {
        //
        // Initialize entity list
        //
        for( i = 0; i < CollisionEntityDSGList::NUM_COLLISION_LIST_ENTITIES; i++ )
        {
            pCollisionEntityList->collisionEntity[i] = NULL;
        }
    }

    for (i = 0 ; i < mMaxDynamics ; i++)
    {
        DynaPhysDSG* pDSG = mCurrentDynamics[collisionAreaIndex][i].mDynamicPhysDSG;
        if ( pDSG != NULL )
        {
            rmt::Vector objPosition;
            rmt::Box3D boundingBox;
            pDSG->GetBoundingBox( &boundingBox );
            rmt::Sphere boundingSphere( position, radius );
            pDSG->GetPosition(&objPosition);
            if ( boundingBox.Intersects( boundingSphere ) )
            {              
                if ( pDSG->IsCollisionEnabled() )
                {
                    pDSG->ApplyForce( direction, force );  

                    if( ( pCollisionEntityList != NULL )
                        && ( numObjectsAffected < CollisionEntityDSGList::NUM_COLLISION_LIST_ENTITIES ) )
                    {
                        pCollisionEntityList->collisionEntity[numObjectsAffected] = pDSG;
                    }
                    ++numObjectsAffected;                
                }
            }
        }
    }
    return numObjectsAffected;
}

int WorldPhysicsManager::ApplyForceToDynamicsSpherical( int collisionAreaIndex,
                                                        const rmt::Vector& position,
                                                        float radius,
                                                        float force,
                                                        CollisionEntityDSGList* pCollisionEntityList )
{
    rAssert( collisionAreaIndex > WorldPhysicsManager::INVALID_COLLISION_AREA );

    int i;
    int numObjectsAffected = 0;
    float radiusSqr = radius * radius;

    if( pCollisionEntityList != NULL )
    {
        //
        // Initialize entity list
        //
        for( i = 0; i < CollisionEntityDSGList::NUM_COLLISION_LIST_ENTITIES; i++ )
        {
            pCollisionEntityList->collisionEntity[i] = NULL;
        }
    }

    for (i = 0 ; i < mMaxDynamics ; i++)
    {
        DynaPhysDSG* pDSG = mCurrentDynamics[collisionAreaIndex][i].mDynamicPhysDSG;
        if ( pDSG != NULL )
        {
            rmt::Vector objPosition;
            rmt::Box3D boundingBox;
            pDSG->GetBoundingBox( &boundingBox );
            rmt::Sphere boundingSphere( position, radius );
            pDSG->GetPosition(&objPosition);
            if ( boundingBox.Intersects( boundingSphere ) )
            {
                if ( pDSG->IsCollisionEnabled() )
                {
                    // Calculate the vector from the position to the object
                    rmt::Vector direction = boundingBox.Mid() - position;
                    direction.Normalize();
                    pDSG->ApplyForce( direction, force );
                    if( ( pCollisionEntityList != NULL )
                        && ( numObjectsAffected < CollisionEntityDSGList::NUM_COLLISION_LIST_ENTITIES ) )
                    {
                        pCollisionEntityList->collisionEntity[numObjectsAffected] = pDSG;
                    }

                    ++numObjectsAffected;       
                }
            }
        }
    }
    return numObjectsAffected;

}


//=============================================================================
// WorldPhysicsManager::InitCollisionManager
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::InitCollisionManager()
{
MEMTRACK_PUSH_GROUP( "WorldPhysicsManager" );

    mCollisionManager = CollisionManager::GetInstance();
    mCollisionManager->AddRef();

    //mCollisionManager->SetCollisionDistanceCGS(mCollisionDistanceCGS);

    //mCollisionManager->SetCollisionManagerAttributes(CM_DetectIfMoving);    // TODO - which attribute should this be?
    mCollisionManager->SetCollisionManagerAttributes(CM_DetectAll); // TODO - should be able to use IfMoving

    

    //mReservedCollisionAreas = 2;
    mReservedCollisionAreas = 0;

    // setup areas:
    //
    // total number of areas =    max allowed number of active vehicles  
    //                          + max active (human) characters {always 1 I think} 
    //                          + cameras for active cars or characters 
    //
    // (new)                    + initial reserved slots
    
    //mMaxVehicles = GetVehicleCentral()->GetMaxActiveVehicles();

    //mMaxChars = GetCharacterManager()->GetMaxCharacters();
    
    // new
    // jan 12, 2003
    // test
    // values for vehicles and especially characters are now way too high
    //
    // only need slots for shit that is in existence simultaneously
    
    // 10 is probably enough
    mMaxVehicles = 15;

    // 12 - 15 would probably be enough
    mMaxChars = 18; 
    
    

    mMaxCameras = MAX_PLAYERS;

    // this total is the number of collision areas we need + reserved

    // following indexing standard will be used into the collision areas array:

    /*

    reserved:

        0
        1
        2
        .
        .
        .
    mReservedCollisionAreas - 1

    vehicles:

        mReservedCollisionAreas                   
        .                   
        .
        .
        .
        .
        mReservedCollisionAreas + maxVehicle - 1

        
    characters:

        mReservedCollisionAreas + maxVehicle
        .
        .
        mReservedCollisionAreas+ maxVehicle + maxChars - 1

    cameras:

        mReservedCollisionAreas + maxVehicle + maxChars
        .
        .
        mReservedCollisionAreas + maxVehicle + maxChars + maxCameras - 1


    */

    mNumCollisionAreas = mReservedCollisionAreas + mMaxVehicles + mMaxChars + mMaxCameras;
    mCollisionManager->SetNbArea(mNumCollisionAreas);

    // goddamn, goddamn!!
    mCollisionManager->SetUseExclusiveAutoPair(true);

    mCollisionManager->ActivateAllAreas();
    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_PERSISTENT );
    #endif

    mCollisionAreaAllocated = new bool[mNumCollisionAreas];
    mCollisionAreaActive = new bool[mNumCollisionAreas];

    //--------
    // statics
    //--------

    // TODO - find the right number here
    // TODO - different depending on thing?
    mMaxStatics = 30;
        
    mCurrentStatics = new StaticsInCollisionDetection*[mNumCollisionAreas];

    int i,j;
    for(i = 0; i < mNumCollisionAreas; i++)
    {
        mCollisionAreaAllocated[i] = false;
        mCollisionAreaActive[i] = false;
    
        mCurrentStatics[i] = new StaticsInCollisionDetection[mMaxStatics];
        
        for(j = 0; j < mMaxStatics; j++)
        {
            mCurrentStatics[i][j].mStaticPhysDSG = 0;
            mCurrentStatics[i][j].clean = false;   
        }       
    }
    

    //----------------
    // anim collisions
    //----------------

    // Todo: TBJ - make this value smaller.  
    // It has to be larger because of the large radius set in CharacterManager.
    // When anim objects move in the DSG, we can reduce this to a smaller number.
    //
    mMaxAnimCollisions = 20;
        
    mCurrentAnimCollisions = new AnimCollisionInCollisionDetection*[mNumCollisionAreas];
    
    for(i = 0; i < mNumCollisionAreas; i++)
    {        
        mCurrentAnimCollisions[i] = new AnimCollisionInCollisionDetection[mMaxAnimCollisions];
        
        for(j = 0; j < mMaxAnimCollisions; j++)
        {
            mCurrentAnimCollisions[i][j].mAnimCollisionEntityDSG = 0;
            mCurrentAnimCollisions[i][j].clean = false;   
        }       
    }


    mMaxUpdateAnimCollisions = 64;
    
    mCurrentUpdateAnimCollisions = new AnimCollisionInCollisionDetection*[mNumCollisionAreas];
    
    for(i = 0; i < mNumCollisionAreas; i++)
    {        
        mCurrentUpdateAnimCollisions[i] = new AnimCollisionInCollisionDetection[mMaxUpdateAnimCollisions];
        
        for(j = 0; j < mMaxUpdateAnimCollisions; j++)
        {
            mCurrentUpdateAnimCollisions[i][j].mAnimCollisionEntityDSG = 0;
            mCurrentUpdateAnimCollisions[i][j].clean = false;   
        }       
    }

    //-------
    // fences
    //-------

    // TODO - find the right number here!
    mMaxFencePerArea = 8;

    mFences = new FencePieces*[mNumCollisionAreas];

    mFencesInEachArea = new int[mNumCollisionAreas];
    // keep a total for efficient submission/removal

    
    // new - to save memory
    // May 7, 2003
    //
    // make all fences and ground planes in the pool use the same physics properties
    // (perhaps later change so that one for fences and one for ground planes)
    
    mFencePhysicsProperties = new sim::PhysicsProperties;
    mFencePhysicsProperties->AddRef();
    

    
    for(i = 0; i < mNumCollisionAreas; i++)
    {
        mFences[i] = new FencePieces[mMaxFencePerArea];

        for(j = 0; j < mMaxFencePerArea; j++)
        {
            // need to make collision volumes here!
            rmt::Vector center(0.0f, 0.0f, 0.0f);
            rmt::Vector o0(1.0f, 0.0f, 0.0f);
            rmt::Vector o1(0.0f, 1.0f, 0.0f);
            rmt::Vector o2(0.0f, 0.0f, 1.0f);

            sim::OBBoxVolume* tempOBBox = new OBBoxVolume(center, o0, o1, o2, 1.0f, 1.0f, 1.0f);            


            mFences[i][j].mFenceSimState = (sim::ManualSimState*)(SimState::CreateManualSimState(tempOBBox));
            mFences[i][j].mFenceSimState->AddRef();

            mFences[i][j].mFenceSimState->GetCollisionObject()->SetManualUpdate(true); 
            //mFences[i][j].mFenceSimState->GetCollisionObject()->SetAutoPair(true);
            mFences[i][j].mFenceSimState->GetCollisionObject()->SetAutoPair(false);

            // setting this should free the unnecessarly allocated default one:
            //mFences[i][j].mFenceSimState->GetCollisionObject()->SetPhysicsProperties(mFencePhysicsProperties);
            mFences[i][j].mFenceSimState->SetPhysicsProperties(this->mFencePhysicsProperties);
            

            // give this thing a reasonable name for debug purposes
            char buffy[128];
            sprintf(buffy, "fence_a%d_n%d", i, j);

            mFences[i][j].mFenceSimState->GetCollisionObject()->SetName(buffy);

            mFences[i][j].mFenceSimState->mAIRefIndex = PhysicsAIRef::redBrickPhizFence;
            mFences[i][j].mFenceSimState->mAIRefPointer = 0;    // only set if object is derived from CollisionEntityDSG


            mFences[i][j].mInCollision = false;
            mFences[i][j].mClean = false;

        }

        mFencesInEachArea[i] = 0;

    }

    // debug drawing
    mNumDebugFences = 0;

    mFenceDSGResults = new FenceEntityDSG*[mMaxFencePerArea];
    for(i = 0; i < mMaxFencePerArea; i++)
    {
        mFenceDSGResults[i] = new FenceEntityDSG;
        mFenceDSGResults[i]->AddRef();
    }

    //---------
    // dynamics
    //---------

    mMaxDynamics = 20;
        
    mCurrentDynamics = new DynamicsInCollisionDetection*[mNumCollisionAreas];

    for(i = 0; i < mNumCollisionAreas; i++)
    {    
        mCurrentDynamics[i] = new DynamicsInCollisionDetection[mMaxDynamics];
        
        for(j = 0; j < mMaxDynamics; j++)
        {
            mCurrentDynamics[i][j].mDynamicPhysDSG = 0;
            mCurrentDynamics[i][j].clean = false;   
        }       
    }

    //--------------
    // ground planes
    //--------------

    mGroundPlanePool = new GroundPlanePool(mMaxDynamics * 2);    // TODO - ????? how many


    //-----------------------
    // collision solver agent
    //-----------------------
    mpWorldCollisionSolverAgentManager = new WorldCollisionSolverAgentManager;
    mpWorldCollisionSolverAgentManager->AddRef();
    
    mCollisionManager->GetImpulseBasedCollisionSolver()->SetCollisionSolverAgent(mpWorldCollisionSolverAgentManager);


    //enum DrawVolumeMethod {DrawVolumeOutline=0, DrawVolumeShape};
    //sim::SetDrawVolumeMethod(DrawVolumeOutline);
    sim::SetDrawVolumeMethod(DrawVolumeShape);

    mInInterior = false;    

    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_PERSISTENT );
    #endif
MEMTRACK_POP_GROUP("WorldPhysicsManager");
}

//=============================================================================
// WorldPhysicsManager::SuspendForInterior
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::SuspendForInterior()
{
    mInInterior = true;
}


//=============================================================================
// WorldPhysicsManager::ResumeForOutside
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::ResumeForOutside()
{
    mInInterior = false;    
}



//=============================================================================
// WorldPhysicsManager::EmptyCollisionAreaIndex
//=============================================================================
// Description: Comment
//
// Parameters:  (int index)
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::EmptyCollisionAreaIndex(int index)
{
    rAssert( index > WorldPhysicsManager::INVALID_COLLISION_AREA );
    rAssert(mCollisionAreaAllocated[index]);    // should not be trying to empty one that is not in use

    mCollisionManager->ResetArea(index);    // this will take everything out of that area!

    int i;
    for(i = 0; i < mMaxStatics; i++)
    {
        tRefCounted::Release( mCurrentStatics[index][i].mStaticPhysDSG );
        mCurrentStatics[index][i].mStaticPhysDSG = 0;
        mCurrentStatics[index][i].clean = false;  
    }

    for(i = 0; i < mMaxAnimCollisions; i++)
    {
        tRefCounted::Release( mCurrentAnimCollisions[index][i].mAnimCollisionEntityDSG );
        mCurrentAnimCollisions[index][i].mAnimCollisionEntityDSG = 0;
        mCurrentAnimCollisions[index][i].clean = false;  
    }

    for(i = 0; i < mMaxUpdateAnimCollisions; i++)
    {
        tRefCounted::Release( mCurrentUpdateAnimCollisions[index][i].mAnimCollisionEntityDSG );
        mCurrentUpdateAnimCollisions[index][i].mAnimCollisionEntityDSG = 0;
        mCurrentUpdateAnimCollisions[index][i].clean = false;  
    }

    for(i = 0; i < mMaxFencePerArea; i++)
    {
        // don't need to add ref and release these 'cause they're only internal to the fences.
        mFences[index][i].mInCollision = false;
    }
    //mFencesInEachArea[index] = 0;


    for(i = 0; i < mMaxDynamics; i++)
    {
        if(mCurrentDynamics[index][i].mDynamicPhysDSG)
        {
            //TODO
            if(mCurrentDynamics[index][i].mDynamicPhysDSG->GetAIRef() != PhysicsAIRef::redBrickVehicle)
            {
                mCurrentDynamics[index][i].mDynamicPhysDSG->FreeGroundPlane();  // in case it still has one

                                                                            // this is also where the ground plane collision object will be disabled if 
                                                                            // there are no more refs
            }




    
            tRefCounted::Release( mCurrentDynamics[index][i].mDynamicPhysDSG );
            mCurrentDynamics[index][i].mDynamicPhysDSG = 0;
            mCurrentDynamics[index][i].clean = false;
        }

    }

    // TODO - setup some sort of enable/disable interface also?
}

//=============================================================================
// WorldPhysicsManager::FreeCollisionAreaIndex
//=============================================================================
// Description: Comment
//
// Parameters:  (int index)
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::FreeCollisionAreaIndex(int index)
{
    rAssert( index > WorldPhysicsManager::INVALID_COLLISION_AREA );
    rAssert(mCollisionAreaAllocated[index]);    // should not be trying to free up one that is not in use

    EmptyCollisionAreaIndex( index );

    mCollisionAreaAllocated[index] = false;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void WorldPhysicsManager::FreeAllCollisionAreaIndicies()
{
    int index = 0;
    for(; index<mNumCollisionAreas; index++)
    {
        if(mCollisionAreaAllocated[index])    // should not be trying to free up one that is not in use
        {
            FreeCollisionAreaIndex(index);
        }
    }
}

//=============================================================================
// WorldPhysicsManager::RemoveVehicleFromAnyOtherCurrentDynamicsList
//=============================================================================
// Description: Comment
//
// Parameters:  (Vehicle* vehicle)
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::RemoveFromAnyOtherCurrentDynamicsListAndCollisionArea(DynaPhysDSG* obj)
{
    int i;

 //   Vehicle* vehicle = dynamic_cast<Vehicle*>(obj);
 //   Character* character= dynamic_cast<Character*>(obj);
    Vehicle* vehicle = NULL;
    Character* character = NULL;

    if ( obj->GetAIRef() == PhysicsAIRef::NPCharacter || 
         obj->GetAIRef() == PhysicsAIRef::PlayerCharacter )
    {
        rAssert( dynamic_cast< Character* >( obj ) );
        character = static_cast< Character*>( obj ) ;            
    }
    else if ( obj->GetAIRef() == PhysicsAIRef::redBrickVehicle )
    {
        rAssert( dynamic_cast< Vehicle* >( obj ));
        vehicle = static_cast< Vehicle*>( obj );            
    }
    else
    {
        rAssert( dynamic_cast< Character* >( obj )==false);
        rAssert( dynamic_cast< Vehicle* >( obj )==false);    
    }

    for(i = 0; i < mNumCollisionAreas; i++)
    {   
        if(vehicle && (i == vehicle->mCollisionAreaIndex))
        {
            continue;
        }

        if(character && (i == character->GetCollisionAreaIndex()))
        {
            continue;
        }

        if(!mCollisionAreaAllocated[i])
        {
            continue;
        }

        int j;
        for(j = 0; j < mMaxDynamics; j++)
        {
            if(mCurrentDynamics[i][j].mDynamicPhysDSG == obj)
            {
                mCollisionManager->RemoveCollisionObject(obj->GetSimState()->GetCollisionObject(), i);

                
                // only deal with ground plane here if the incoming object is not a car
                if(!(vehicle))
                {
                    int groundPlaneIndex = mCurrentDynamics[i][j].mDynamicPhysDSG->GetGroundPlaneIndex();
                    if(groundPlaneIndex != -1)  // should never be false... 
                    {
                        sim::CollisionObject* gpCollObj = mGroundPlanePool->GetSimState(groundPlaneIndex)->GetCollisionObject();
                
                        mCollisionManager->RemoveCollisionObject(gpCollObj, i);

                        mCurrentDynamics[i][j].mDynamicPhysDSG->FreeGroundPlane();     // this decrements the groundplaneref, but we still need to take it out of 
                    }
                }

                mCurrentDynamics[i][j].mDynamicPhysDSG = 0;
                mCurrentDynamics[i][j].clean = false;

                obj->Release();
             

            }

        }
    }
}

//=============================================================================
// WorldPhysicsManager::OnQuitLevel
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::OnQuitLevel()
{
    // make sure ground plane pool has been properly free'd up
    if(!(mGroundPlanePool->FreeAllGroundPlanes()))
    {
        // there was some ground planes that were not yet free'd when this was called!
        rAssertMsg(false, "not all the ground planes were free'd by their dynamic object owners!  bad dynamic objects!\n");
    }
    
}

//=============================================================================
// WorldPhysicsManager::GetCameraCollisionAreaIndex
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      int 
//
//=============================================================================
int WorldPhysicsManager::GetCameraCollisionAreaIndex()
{  
    // moved to member data
    //int maxVehicles = GetVehicleCentral()->GetMaxActiveVehicles();
    //int maxChars = MAX_PLAYERS;
    //int maxCameras = MAX_PLAYERS;
    
    int start = mReservedCollisionAreas + mMaxVehicles + mMaxChars;
    int end = mReservedCollisionAreas + mMaxVehicles + mMaxChars + mMaxCameras - 1;

    int i;
    for(i = start; i <= end; i++)
    {
        // look for first free index
        if(mCollisionAreaAllocated[i])
        {
            continue;
        }
        else
        {
            mCollisionAreaAllocated[i] = true;
            return i;
        }
    }
    
    rReleaseAssertMsg( 0, "not enough camera collision indices\n" );
    return WorldPhysicsManager::INVALID_COLLISION_AREA;
}   


//=============================================================================
// WorldPhysicsManager::GetVehicleCollisionAreaIndex
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      int 
//
//=============================================================================
int WorldPhysicsManager::GetVehicleCollisionAreaIndex()
{
    //int maxVehicles = GetVehicleCentral()->GetMaxActiveVehicles();
    
    int start = mReservedCollisionAreas;
    int end = mReservedCollisionAreas + mMaxVehicles - 1;

    int i;
    for(i = start; i <= end; i++)
    {
        // look for first free index
        if(mCollisionAreaAllocated[i])
        {
            continue;
        }
        else
        {
            mCollisionAreaAllocated[i] = true;
            return i;
        }
    }
    
    rReleaseAssertMsg( 0, "not enough vehicle collision indices\n" );
    return WorldPhysicsManager::INVALID_COLLISION_AREA;
}


//=============================================================================
// WorldPhysicsManager::GetCharacterCollisionAreaIndex
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      int 
//
//=============================================================================
int WorldPhysicsManager::GetCharacterCollisionAreaIndex()
{    
    //int maxVehicles = GetVehicleCentral()->GetMaxActiveVehicles();
    //int maxChars = MAX_PLAYERS;
    
    int start = mReservedCollisionAreas + mMaxVehicles;
    int end = mReservedCollisionAreas + mMaxVehicles + mMaxChars - 1;

    int i;
    for(i = start; i <= end; i++)
    {
        // look for first free index
        if(mCollisionAreaAllocated[i])
        {
            continue;
        }
        else
        {
            mCollisionAreaAllocated[i] = true;
            return i;
        }
    }
    
    rReleaseAssertMsg( 0, "not enough character collision indices\n" );
    return WorldPhysicsManager::INVALID_COLLISION_AREA;
}


//=============================================================================
// WorldPhysicsManager::DestroyInstance
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::DestroyInstance()
{
    rAssert(spInstance);
    
    // TODO - Release() collision manager

    delete(GMA_PERSISTENT, spInstance);
    spInstance = NULL;



}



//=============================================================================
// WorldPhysicsManager::SetSimUnits
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
/*
void WorldPhysicsManager::SetSimUnits()
{
    float LSCALE = 0.01f;
    //float MSCALE = 0.001f;
    float MSCALE = 1.0f;
    float TSCALE = 1.0f;
	SimUnitsManager um;
	um.SetUnits(LSCALE, MSCALE, TSCALE); 
}
*/

//=============================================================================
// WorldPhysicsManager::Update
//=============================================================================
// Description: Comment
//
// Parameters:  (unsigned int timeDeltaSeconds)
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::Update(unsigned int timeDeltaMilliSeconds)
{


    BEGIN_PROFILE("WorldPhysicsManager::Update")

  
    BEGIN_PROFILE("Vehicle Submits")
    GetVehicleCentral()->SubmitStatics();
    GetVehicleCentral()->SubmitAnimCollisions();
    GetVehicleCentral()->SubmitDynamics();
    END_PROFILE("Vehicle Submits")
    

    BEGIN_PROFILE("Character Submits")
    GetCharacterManager()->SubmitStatics(); 
    GetCharacterManager()->SubmitAnimCollisions();  // both collision and update I assume
    GetCharacterManager()->SubmitDynamics();
    END_PROFILE("Character Submits")
    
    BEGIN_PROFILE("Cam Submits")
    GetSuperCamManager()->SubmitStatics();
    END_PROFILE("Cam Submits")
    
                
    BEGIN_PROFILE("Update Ground")
    UpdateSimluatingDynamicObjectGroundPlanes();
    END_PROFILE("Update Ground")

    // new substep logic - nbrooke 12/7/03
    //
    // we want to try to minimize it ticking over into two substep mode unless it absolutly neccesary
    // and try to run a 30fps on the ps2 all the time therefore I have 
    //    1) upped the threshold for going to two substeps to 35 ms from 32ms
    //    2) only go into multiple substeps if we get two consecutive frames over the threshold or 
    //       one frame WAY over the threshold
    unsigned numSubsteps = 1;

    if(((mLastTime > 35) && (timeDeltaMilliSeconds > 35)) || (timeDeltaMilliSeconds > 50))
    {
        numSubsteps = (timeDeltaMilliSeconds + 34) / 35;
    }

    mLastTime = timeDeltaMilliSeconds;

    float substep = (float(timeDeltaMilliSeconds) * 0.001f) / float(numSubsteps);
    float dt = static_cast<float>(timeDeltaMilliSeconds) * 0.001f;

    // cap
    if(numSubsteps > 10)
    {
        rReleasePrintf("\nhit 10 substeps!\n");
        numSubsteps = 10;
    }
   
    
    mLoopTime = dt;
    
    if(mTimerOn)
    {
        mTimerTime += dt;
    }
    
    BEGIN_PROFILE("WorldPhysicsManager::PreSubstepUpdate")

    GetVehicleCentral()->PreSubstepUpdate(dt);

    END_PROFILE("WorldPhysicsManager::PreSubstepUpdate")

    BEGIN_PROFILE("PreSim")
    GetCharacterManager( )->PreSimUpdate( dt );
    END_PROFILE("PreSim")

    bool firstSubstep = true;
    //while(countDown > timestep)// * 1.5f)
    for(unsigned i = 0; i < numSubsteps; i++)
    {
        // do shit
        WorldSimSubstepGuts(substep, firstSubstep);
        firstSubstep = false;
    }

    BEGIN_PROFILE("VehiclePost")
    GetVehicleCentral()->PostSubstepUpdate(dt);
    END_PROFILE("VehiclePost")

    BEGIN_PROFILE("CharPost")
    GetCharacterManager()->PostSimUpdate( dt );
    END_PROFILE("CharPost")

    END_PROFILE("WorldPhysicsManager::Update")

    BEGIN_PROFILE("Phys DebugInfo")
    DebugInfoDisplay();
    END_PROFILE("Phys DebugInfo")

}


//=============================================================================
// WorldPhysicsManager::DebugInfoDisplay
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::DebugInfoDisplay()
{
    // maybe "Display" is a bit deceiving.

    #ifdef DEBUGINFO_ENABLED

    DEBUGINFO_PUSH_SECTION( "Vehicle Shit" );

    // just display the vehicles position
    // whichever vehicle player is driving

    Avatar* playerAvatar = GetAvatarManager()->GetAvatarForPlayer(0);
    
    Vehicle* playerVehicle = 0;
    
    if(playerAvatar)
    {    
        playerVehicle = playerAvatar->GetVehicle();

        if(playerVehicle)
        {             
            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
    
            rmt::Vector pos = playerVehicle->GetPosition();
            x = pos.x;
            y = pos.y;
            z = pos.z;


            char buffy[128];
            sprintf( buffy, "position - x: %.2f y: %.2f z: %.2f", x, y, z);

            DEBUGINFO_ADDSCREENTEXT( buffy );
            DEBUGINFO_ADDSCREENTEXT( "" );  //?


            float life = playerVehicle->GetVehicleLifePercentage(playerVehicle->mHitPoints);
            sprintf( buffy, "damage inc. - %.3f", 1.0f - life);

            DEBUGINFO_ADDSCREENTEXT( buffy );
            DEBUGINFO_ADDSCREENTEXT( "" );  //?

            if(playerVehicle->mVehicleState == VS_NORMAL)
            {
                sprintf( buffy, "VS_NORMAL");
            }
            else if(playerVehicle->mVehicleState == VS_EBRAKE_SLIP)
            {
                sprintf( buffy, "VS_EBRAKE_SLIP");
            }
            else
            {
                sprintf( buffy, "VS_SLIP");
            }
            
            DEBUGINFO_ADDSCREENTEXT( buffy );
            DEBUGINFO_ADDSCREENTEXT( "" );  //?

            /*
            if(playerVehicle->mLosingTractionDueToAccel)
            {
                sprintf(buffy, "LOSING TRACTION DUE TO ACCELERATION!!");
            }
            else
            {
                sprintf(buffy, "unmodified traction");
            }
            
                
            DEBUGINFO_ADDSCREENTEXT( buffy );
            DEBUGINFO_ADDSCREENTEXT( "" );  //?

            if(playerVehicle->mLoco == VL_PHYSICS)  // I think this will always be the case
            {
                sprintf(buffy, "currentSteeringForce: %.2f", playerVehicle->mPhysicsLocomotion->mCurrentSteeringForce);
                DEBUGINFO_ADDSCREENTEXT( buffy );
                DEBUGINFO_ADDSCREENTEXT( "" );  //?
            }
            */


            sprintf(buffy, "wheel turn angle unmodified input: %.2f", playerVehicle->mUnmodifiedInputWheelTurnAngle);
            DEBUGINFO_ADDSCREENTEXT( buffy );
            DEBUGINFO_ADDSCREENTEXT( "" );  //?


            sprintf(buffy, "wheel turn angle normalized: %.2f", playerVehicle->mWheelTurnAngleInputValue);
            DEBUGINFO_ADDSCREENTEXT( buffy );
            DEBUGINFO_ADDSCREENTEXT( "" );  //?

            
            sprintf(buffy, "wheel turn angle: %.2f", playerVehicle->mWheelTurnAngle);
            DEBUGINFO_ADDSCREENTEXT( buffy );
            DEBUGINFO_ADDSCREENTEXT( "" );  //?



            sprintf(buffy, "kmh: %.2f", playerVehicle->mSpeedKmh);
            DEBUGINFO_ADDSCREENTEXT( buffy );
            DEBUGINFO_ADDSCREENTEXT( "" );  //?


            sprintf(buffy, "ebrake: %.2f", playerVehicle->mEBrake);
            DEBUGINFO_ADDSCREENTEXT( buffy );
            DEBUGINFO_ADDSCREENTEXT( "" );  //?


            sprintf(buffy, "gas: %.2f", playerVehicle->mGas);
            DEBUGINFO_ADDSCREENTEXT( buffy );
            DEBUGINFO_ADDSCREENTEXT( "" );  //?



            sprintf(buffy, "timer: %.3f", mTimerTime);
            DEBUGINFO_ADDSCREENTEXT( buffy );
            DEBUGINFO_ADDSCREENTEXT( "" );  //?

            /*
    
            sprintf(buffy, "speed burst timer: %.3f", playerVehicle->mSpeedBurstTimer);
            DEBUGINFO_ADDSCREENTEXT( buffy );
            DEBUGINFO_ADDSCREENTEXT( "" );  //?

            if(playerVehicle->mDoSpeedBurst)
            {
                sprintf(buffy, "doing speed burst!");
            }
            else
            {
                sprintf(buffy, "not doing speed burst");
            }
            */
                    
            
        }

    }

    
    DEBUGINFO_POP_SECTION();
    
  
    
    
    if(playerVehicle)
    {
        char temp[128];
        DebugInfo::GetInstance()->Push( "Vehicle Terrain Type" );
        
                
        sprintf( temp, "average for vehicle: %d. %sside.", playerVehicle->mTerrainType, playerVehicle->mInterior ? "In" : "Out" );
        DebugInfo::GetInstance()->AddScreenText( temp, tColour( 25, 150, 125 ) );
        
        sprintf( temp, "wheel 0: %d", playerVehicle->mPhysicsLocomotion->mTerrainIntersectCache[0].mTerrainType);
        DebugInfo::GetInstance()->AddScreenText( temp, tColour( 25, 150, 125 ) );
                
        sprintf( temp, "wheel 1: %d", playerVehicle->mPhysicsLocomotion->mTerrainIntersectCache[1].mTerrainType);
        DebugInfo::GetInstance()->AddScreenText( temp, tColour( 25, 150, 125 ) );
                
        sprintf( temp, "wheel 2: %d", playerVehicle->mPhysicsLocomotion->mTerrainIntersectCache[2].mTerrainType);
        DebugInfo::GetInstance()->AddScreenText( temp, tColour( 25, 150, 125 ) );
                
        sprintf( temp, "wheel 3: %d", playerVehicle->mPhysicsLocomotion->mTerrainIntersectCache[3].mTerrainType);
        DebugInfo::GetInstance()->AddScreenText( temp, tColour( 25, 150, 125 ) );
        
        
        
        DebugInfo::GetInstance()->Pop();
        
        
        /*
        DebugInfo::GetInstance()->Push( "Player Vehicle Collision Info" );
        
        int colindex = playerVehicle->mCollisionAreaIndex;
        sprintf( temp, "number of collision pairs in player collision area: %d", this->mCollisionManager->GetCollisionObjectPairList(colindex)->ArraySize());
                
        DEBUGINFO_ADDSCREENTEXT( temp );
        DEBUGINFO_ADDSCREENTEXT( "" );  //?

        DebugInfo::GetInstance()->Pop();
        */
        
        
    }

    

    #endif // DEBUGINFO_ENABLED


}

//=============================================================================
// WorldPhysicsManager::WorldSimSubstepGuts
//=============================================================================
// Description: Comment
//
// Parameters:  (float dt)
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::WorldSimSubstepGuts(float dt, bool firstSubstep)
{
    BEGIN_PROFILE("WorldSimSubstepGuts")

    mTotalTime += dt;
    updateFrame++;

    BEGIN_PROFILE("VC::PreCollisionPrep")
    if(!mInInterior)
    {
        GetVehicleCentral()->PreCollisionPrep(dt, firstSubstep);
    }
    END_PROFILE("VC::PreCollisionPrep")

    BEGIN_PROFILE("CharMgr::PreSimUpdate")
    GetCharacterManager()->PreSubstepUpdate( dt );
    END_PROFILE("CharMgr::PreSimUpdate")

    BEGIN_PROFILE("ABMgr::Update")
    GetActionButtonManager( )->Update( dt );
    END_PROFILE("ABMgr::Update")

//    BEGIN_PROFILE("SuperCamMgr::PreCollPrep")
//    GetSuperCamManager()->PreCollisionPrep();
//    END_PROFILE("SuperCamMgr::PreCollPrep")

    BEGIN_PROFILE("ResetCollisionFlags")
    mpWorldCollisionSolverAgentManager->ResetCollisionFlags();
    END_PROFILE("ResetCollisionFlags")



BEGIN_PROFILE("DetectCollisions")
    bool printOut = false;
    mCollisionManager->ClearCollisions();
    mCollisionManager->DetectCollision(dt, mTotalTime, printOut );
    mCollisionManager->SolveCollision(dt, mTotalTime);

END_PROFILE("DetectCollisions")
    
    
    BEGIN_PROFILE("GetVehicleCentral()->Update")
    GetVehicleCentral()->Update(dt);
    END_PROFILE("GetVehicleCentral()->Update")
    BEGIN_PROFILE("GetCharacterManager()->Update")
    GetCharacterManager()->Update( dt );
    END_PROFILE("GetCharacterManager()->Update")
    BEGIN_PROFILE("GetCharacterManager()->PostSimUpdate")
    GetCharacterManager()->PostSubstepUpdate(dt);
    END_PROFILE("GetCharacterManager()->PostSimUpdate")

    //Update the position of the super camers
    BEGIN_PROFILE("GetSuperCamManager()->Update")
    GetSuperCamManager()->Update( rmt::FtoL(dt * 1000.0f), firstSubstep );
    END_PROFILE("GetSuperCamManager()->Update")

    //Detect collisions on the new position of the super camera
    int area = GetSuperCamManager()->GetSCC( 0 )->mCollisionAreaIndex;
    mCollisionManager->ClearCollisions();
    BEGIN_PROFILE("SuperCamMgr::PreCollPrep")
        GetSuperCamManager()->PreCollisionPrep();
    END_PROFILE("SuperCamMgr::PreCollPrep")

    mCollisionManager->DetectCollision( area, dt, mTotalTime, printOut );
    mCollisionManager->SolveCollision( area, dt, mTotalTime );

    //Correct the position of the super camera
    GetSuperCamManager()->GetSCC( 0 )->UpdateForPhysics( rmt::FtoL(dt * 1000.0f) );

    END_PROFILE("WorldSimSubstepGuts")
}



//=============================================================================
// WorldPhysicsManager::DisplayCollisionObjectsExceptVehicleInArea
//=============================================================================
// Description: Comment
//
// Parameters:  (int area)
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::DisplayFencesInArea(int area)
{
    rAssert( area > WorldPhysicsManager::INVALID_COLLISION_AREA );

    int i;
    for(i = 0; i < mMaxFencePerArea; i++)
    {        
        if(mFences[area][i].mInCollision)
        {
            sim::ManualSimState* mss = mFences[area][i].mFenceSimState;
            sim::DrawCollisionObject(mss->GetCollisionObject());
        }

    }
    for(i = 0; i < mNumDebugFences; i++)
    {
        // debug draw it!
        mFenceDSGResults[i]->Display();
    }

}

//=============================================================================
// WorldPhysicsManager::SubmitStaticsPseudoCallback
//=============================================================================
// Description: Comment
//
// Parameters:  (rmt::Vector& position, float radius, int collisionAreaIndex)
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::SubmitStaticsPseudoCallback(rmt::Vector& position, float radius, int collisionAreaIndex, sim::SimState* callerSimState, bool allowAutoPairing)
{
    rAssert( collisionAreaIndex > WorldPhysicsManager::INVALID_COLLISION_AREA );

    BEGIN_PROFILE( "SubmitStaticsPseudoCallback" );

    ReserveArray<StaticPhysDSG*> staticPhysDSGList;     
    
    int i;  
    for(i = 0; i < mMaxStatics; i++)
    {
        mCurrentStatics[collisionAreaIndex][i].clean = false; // probably slower to check if there's
                                                    // even something pointed to than just
                                                    // to do this
    }   
    
    BEGIN_PROFILE( "FindStaticPhysElems" );
    GetIntersectManager()->FindStaticPhysElems(position, radius, staticPhysDSGList);  
    END_PROFILE( "FindStaticPhysElems" );

    int numResults = staticPhysDSGList.mUseSize;
    
    // !!!!
    // temp
    //
    // TODO
    // hmmm... what to do here?
    // pick different max or try and prioritize further?     
    //
    // also TODO - need different maxes for different lists

    if(numResults > mMaxStatics)
    {
        numResults = mMaxStatics;
        rDebugPrintf("\n!!! too many statics !!!\n");        
    }


    for(i = 0; i < numResults; i++)
    {
        // go through results and submit to collision.
        
        // first see if already in our list
        StaticPhysDSG* curr = staticPhysDSGList[i];

        int j;
        bool alreadyIn = false;
        for(j = 0; j < mMaxStatics; j++)
        {
            if(mCurrentStatics[collisionAreaIndex][j].mStaticPhysDSG == curr)
            {
                // this one already in list
                mCurrentStatics[collisionAreaIndex][j].clean = true;
                alreadyIn = true;
                break;

            }
        }    

        if(!alreadyIn)
        {
            // add to our list and to collision manager
            //
            // look for first available slot
            int k;
            for(k = 0; k < mMaxStatics; k++)
            {
                if(mCurrentStatics[collisionAreaIndex][k].mStaticPhysDSG == 0)
                {
                    // here is a slot
                    mCurrentStatics[collisionAreaIndex][k].mStaticPhysDSG = curr;
                    mCurrentStatics[collisionAreaIndex][k].mStaticPhysDSG->AddRef();
                    mCurrentStatics[collisionAreaIndex][k].clean = true;
                    
                    sim::CollisionObject* currCollObj = curr->mpSimState()->GetCollisionObject();

                    
    BEGIN_PROFILE( "AddCollisionObject" );  
    
                    if(1)//allowAutoPairing)
                    {
                        currCollObj->SetAutoPair(true);
                    }
                    else
                    {
                        currCollObj->SetAutoPair(false);
                    }
                    
                    mCollisionManager->AddCollisionObject(currCollObj, collisionAreaIndex);
                    
                    //if(!allowAutoPairing)
                    {
                        mCollisionManager->AddPair(currCollObj, callerSimState->GetCollisionObject(), collisionAreaIndex);
                    }
                    
                 
      
    
    END_PROFILE( "AddCollisionObject" );
                    break;
                }
            }
    
            rAssert(k < mMaxStatics);    
            // TODO - deal with no available slots....
        }       
    }

    // now look through remaining list and remove unclean
    for(i = 0; i < mMaxStatics; i++)
    {
        if(mCurrentStatics[collisionAreaIndex][i].mStaticPhysDSG != 0 && 
           mCurrentStatics[collisionAreaIndex][i].clean == false)
        {
    BEGIN_PROFILE( "RemoveCollisionObject" );
            mCollisionManager->RemoveCollisionObject(mCurrentStatics[collisionAreaIndex][i].mStaticPhysDSG->mpSimState()->GetCollisionObject(), collisionAreaIndex);
    END_PROFILE( "RemoveCollisionObject" );
            mCurrentStatics[collisionAreaIndex][i].mStaticPhysDSG->Release();
            mCurrentStatics[collisionAreaIndex][i].mStaticPhysDSG = 0;
            mCurrentStatics[collisionAreaIndex][i].clean = false; // this is redundant
        }
    }
    END_PROFILE( "SubmitStaticsPseudoCallback" );
}


//=============================================================================
// WorldPhysicsManager::SubmitAnimCollisionsPseudoCallback
//=============================================================================
// Description: Comment
//
// Parameters:  (rmt::Vector& position, float radius, int collisionAreaIndex)
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::SubmitAnimCollisionsPseudoCallback(rmt::Vector& position, float radius, int collisionAreaIndex, sim::SimState* callerSimState)
{
    rAssert( collisionAreaIndex > WorldPhysicsManager::INVALID_COLLISION_AREA );

    ReserveArray<AnimCollisionEntityDSG*> animCollisionEntityDSGList;     
    
    int i;  
    for(i = 0; i < mMaxAnimCollisions; i++)
    {
        mCurrentAnimCollisions[collisionAreaIndex][i].clean = false;
    }   
    
    GetIntersectManager()->FindAnimPhysElems(position, radius, animCollisionEntityDSGList);  

    int numResults = animCollisionEntityDSGList.mUseSize;
    
    if(numResults > mMaxAnimCollisions)
    {
        numResults = mMaxAnimCollisions;
        rDebugPrintf("\n!!! too many animating collision objects - goddamn !!!\n");        
    }


    for(i = 0; i < numResults; i++)
    {
        // go through results and submit to collision.
        
        // first see if already in our list
        AnimCollisionEntityDSG* curr = animCollisionEntityDSGList[i];

        int j;
        bool alreadyIn = false;
        for(j = 0; j < mMaxAnimCollisions; j++)
        {
            if(mCurrentAnimCollisions[collisionAreaIndex][j].mAnimCollisionEntityDSG == curr)
            {
                // this one already in list
                mCurrentAnimCollisions[collisionAreaIndex][j].clean = true;
                alreadyIn = true;
                break;

            }
        }    

        if(!alreadyIn)
        {
            // add to our list and to collision manager
            //
            // look for first available slot
            int k;
            for(k = 0; k < mMaxAnimCollisions; k++)
            {
                if(mCurrentAnimCollisions[collisionAreaIndex][k].mAnimCollisionEntityDSG == 0)
                {
                    // here is a slot
                    mCurrentAnimCollisions[collisionAreaIndex][k].mAnimCollisionEntityDSG = curr;
                    mCurrentAnimCollisions[collisionAreaIndex][k].mAnimCollisionEntityDSG->AddRef( );
                    mCurrentAnimCollisions[collisionAreaIndex][k].clean = true;
                    
                    sim::CollisionObject* currCollObj = curr->GetSimState()->GetCollisionObject();

                    currCollObj->SetAutoPair(false);
                    mCollisionManager->AddCollisionObject(currCollObj, collisionAreaIndex);
                    mCollisionManager->AddPair(currCollObj, callerSimState->GetCollisionObject(), collisionAreaIndex);
                    
                    break;
                }
            }
    
            rAssert(k < mMaxAnimCollisions);    
            // TODO - deal with no available slots....
        }       
    }

    // now look through remaining list and remove unclean
    for(i = 0; i < mMaxAnimCollisions; i++)
    {
        if(mCurrentAnimCollisions[collisionAreaIndex][i].mAnimCollisionEntityDSG != 0 && 
           mCurrentAnimCollisions[collisionAreaIndex][i].clean == false)
        {
            mCollisionManager->RemoveCollisionObject(mCurrentAnimCollisions[collisionAreaIndex][i].mAnimCollisionEntityDSG->GetSimState()->GetCollisionObject(), collisionAreaIndex);
            mCurrentAnimCollisions[collisionAreaIndex][i].mAnimCollisionEntityDSG->Release( );
            mCurrentAnimCollisions[collisionAreaIndex][i].mAnimCollisionEntityDSG = 0;
            mCurrentAnimCollisions[collisionAreaIndex][i].clean = false; // this is redundant
        }
    }

}


//=============================================================================
// WorldPhysicsManager::SubmitAnimCollisionsForUpdateOnly
//=============================================================================
// Description: Comment
//
// Parameters:  (rmt::Vector& position, float radius, int collisionAreaIndex)
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::SubmitAnimCollisionsForUpdateOnly(rmt::Vector& position, float radius, int collisionAreaIndex)
{
    rAssert( collisionAreaIndex > WorldPhysicsManager::INVALID_COLLISION_AREA );

    ReserveArray<AnimCollisionEntityDSG*> animCollisionEntityDSGList;     
    
    int i;  
    for(i = 0; i < mMaxUpdateAnimCollisions; i++)
    {
        mCurrentUpdateAnimCollisions[collisionAreaIndex][i].clean = false;
    }   
    
    GetIntersectManager()->FindAnimPhysElems(position, radius, animCollisionEntityDSGList);  

    int numResults = animCollisionEntityDSGList.mUseSize;
    
    if(numResults > mMaxUpdateAnimCollisions)
    {
        numResults = mMaxUpdateAnimCollisions;
        rDebugPrintf("\n!!! too many animating collision objects - goddamn !!!\n");        
    }


    for(i = 0; i < numResults; i++)
    {
        // go through results and submit to collision.
        
        // first see if already in our list
        AnimCollisionEntityDSG* curr = animCollisionEntityDSGList[i];

        int j;
        bool alreadyIn = false;
        for(j = 0; j < mMaxUpdateAnimCollisions; j++)
        {
            if(mCurrentUpdateAnimCollisions[collisionAreaIndex][j].mAnimCollisionEntityDSG == curr)
            {
                // this one already in list
                mCurrentUpdateAnimCollisions[collisionAreaIndex][j].clean = true;
                alreadyIn = true;
                break;

            }
        }    

        if(!alreadyIn)
        {
            // add to our list and to collision manager
            //
            // look for first available slot
            int k;
            for(k = 0; k < mMaxUpdateAnimCollisions; k++)
            {
                if(mCurrentUpdateAnimCollisions[collisionAreaIndex][k].mAnimCollisionEntityDSG == 0)
                {
                    // here is a slot
                    mCurrentUpdateAnimCollisions[collisionAreaIndex][k].mAnimCollisionEntityDSG = curr;
                    // Hang on to this object.
                    //
                    mCurrentUpdateAnimCollisions[collisionAreaIndex][k].mAnimCollisionEntityDSG->AddRef( );
                    mCurrentUpdateAnimCollisions[collisionAreaIndex][k].clean = true;
                    
                    break;
                }
            }
    
            rAssert(k < mMaxUpdateAnimCollisions);    
            // TODO - deal with no available slots....
        }       
    }

    // now look through remaining list and remove unclean
    for(i = 0; i < mMaxUpdateAnimCollisions; i++)
    {
        if(mCurrentUpdateAnimCollisions[collisionAreaIndex][i].mAnimCollisionEntityDSG != 0 && 
           mCurrentUpdateAnimCollisions[collisionAreaIndex][i].clean == false)
        {    
            // Let this thing go.
            //
            mCurrentUpdateAnimCollisions[collisionAreaIndex][i].mAnimCollisionEntityDSG->Release( );
            mCurrentUpdateAnimCollisions[collisionAreaIndex][i].mAnimCollisionEntityDSG = 0;
            mCurrentUpdateAnimCollisions[collisionAreaIndex][i].clean = false; // this is redundant
        }
    }
}

//=============================================================================
// WorldPhysicsManager::SubmitDynamicsPseudoCallback
//=============================================================================
// Description: Comment
//
// Parameters:  (rmt::Vector& position, float radius, int collisionAreaIndex)
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::SubmitDynamicsPseudoCallback(rmt::Vector& position, float radius, int collisionAreaIndex, sim::SimState* callerSimState, bool allowAutoPairing)
{
    rAssert( collisionAreaIndex > WorldPhysicsManager::INVALID_COLLISION_AREA );

    // hmmm...
    // safe to make this an InstDynaPhysDSG list?? - doesn't compile
    ReserveArray<DynaPhysDSG*> dynamicsPhysDSGList;     
    
    int i;  
    for(i = 0; i < mMaxDynamics; i++)
    {
        mCurrentDynamics[collisionAreaIndex][i].clean = false; // probably slower to check if there's
                                                               // even something pointed to than just
                                                               // to do this
    }   
    
    // this is just commented out until it exists
    GetIntersectManager()->FindDynaPhysElems(position, radius, dynamicsPhysDSGList);  

    int numResults = dynamicsPhysDSGList.mUseSize;
    
    if(numResults > 1)
    {
        int stophere = 1;
    }

  
    // TODO
    // hmmm... what to do here?
    if(numResults > mMaxDynamics)
    {
        numResults = mMaxDynamics;
        rDebugPrintf("\n!!! too many dynamics returned from query !!!\n");        
    }

    for(i = 0; i < numResults; i++)
    {
        // go through results and submit to collision.
        
        // first see if already in our list
        DynaPhysDSG* curr = dynamicsPhysDSGList[i];

        // temp
        //
        // this actually might be the way to do it?
        //if(curr == caller)
        if((curr->mpSimState() == callerSimState) || (curr->mpSimState() == NULL))
        {
            continue;
        }
        
        // new test
        // just for Cary
        if(callerSimState->mAIRefIndex == PhysicsAIRef::CameraSphere)
        {
            if( curr->mpSimState()->mAIRefIndex != PhysicsAIRef::redBrickVehicle &&
                curr->mpSimState()->mAIRefIndex != PhysicsAIRef::NPCharacter &&
                curr->mpSimState()->mAIRefIndex != PhysicsAIRef::PlayerCharacter )
            {
                continue;
            }
                    
        }
                

        // not optimization further below
        //
        // only gonna pair "submitters" with ourselves
        // 
        // this logic here is to see if the current "submitter" is trying to pair itself against another "submitter" vehicle we already called earlier, that
        // already set up this pair                
        if(callerSimState->mAIRefIndex == PhysicsAIRef::redBrickVehicle && curr->mpSimState()->mAIRefIndex == PhysicsAIRef::redBrickVehicle)
        {
  
            // April 22, 2003
            // problem when cars that are calling this are very close to each other
            //
            // many duplicate pairs put in more than one collision area
            //
            // the fastest, most optimal check we can do to minimize this is:
            // (first re-order the vehicle calls to this to make sure VT_USER called first)
            // then, if we are a car, and return a car, and either it's a VT_USER or it's and AI with 
            // a lower vehicle central index than us, then skipt it - they've already submitted us, and autopaird all round!
            Vehicle* submittingVehicle = (Vehicle*)(callerSimState->mAIRefPointer);
            Vehicle* submittedVehicle = (Vehicle*)(curr->mpSimState()->mAIRefPointer);
            
            rTuneAssert(submittingVehicle->mVehicleCentralIndex != -1);
            rTuneAssert(submittedVehicle->mVehicleCentralIndex != -1);
            
            if(submittingVehicle->mVehicleType == VT_USER)
            {
                if(submittedVehicle->mVehicleType == VT_USER)
                {
                    if(submittingVehicle->mVehicleCentralIndex > submittedVehicle->mVehicleCentralIndex)
                    {
                        // both user vehicles, but...
                        // the submitted vehicle has been called first, so it wins
                        continue;                    
                    }
                }                
            }
            else
            {
                // not a user vehicle
                if(submittedVehicle->mVehicleType == VT_USER)
                {
                    // it has already added us
                    continue;
                }
                else if(submittingVehicle->mVehicleType == VT_AI)
                {
                    if(submittingVehicle->mVehicleCentralIndex > submittedVehicle->mVehicleCentralIndex)
                    {
                        // the submitted vehicle has been called first, so it wins
                        continue;                    
                    }
                }            
            }
  
        }
        
        
        int j;
        bool alreadyIn = false;
        for(j = 0; j < mMaxDynamics; j++)
        {
            if(mCurrentDynamics[collisionAreaIndex][j].mDynamicPhysDSG == curr)
            {
                // this one already in list
                mCurrentDynamics[collisionAreaIndex][j].clean = true;
                alreadyIn = true;
                break;

            }
        }    

        if(!alreadyIn)
        {
            // add to our list and to collision manager
            //
            // look for first available slot
            int k;
            for(k = 0; k < mMaxDynamics; k++)
            {
                if(mCurrentDynamics[collisionAreaIndex][k].mDynamicPhysDSG == 0)
                {
                    // here is a slot
                    mCurrentDynamics[collisionAreaIndex][k].mDynamicPhysDSG = curr;
                    mCurrentDynamics[collisionAreaIndex][k].mDynamicPhysDSG->AddRef( );
                    mCurrentDynamics[collisionAreaIndex][k].clean = true;
                    
                    sim::CollisionObject* currCollObj = curr->mpSimState()->GetCollisionObject();

                    // final, simplest possible optimization
                    //
                    // don't autopair dynamics period
                    // any dynamic we submit only pairs with us!
                    //
                    // who knows what else this thing will do in it's own area (if it has one), so just in case turn
                    // turn flag back on after
                    
                    
                        // only pair with us
                        //
                        
                        if(allowAutoPairing && curr->mpSimState()->mAIRefIndex != PhysicsAIRef::redBrickVehicle)
                        {
                            currCollObj->SetAutoPair(true);    
                        }
                        else
                        {                    
                            currCollObj->SetAutoPair(false);
                        }                        
                        
                        mCollisionManager->AddCollisionObject(currCollObj, collisionAreaIndex);
                        
                        //if(!allowAutoPairing || curr->mpSimState()->mAIRefIndex == PhysicsAIRef::redBrickVehicle)
                        {
                            mCollisionManager->AddPair(currCollObj, callerSimState->GetCollisionObject(), collisionAreaIndex);
                        }
                        
                        
                        
                        /*
                        // new??
                        // is this gonna work, and work well?
                        
                        //!!
                        // only do this for the player character!
                        
                        
                        int l;
                        for(l = 0; l < this->mMaxStatics; l++)
                        {
                            if(mCurrentStatics[collisionAreaIndex][l].mStaticPhysDSG)
                            {
                                // there's one there...
                                // make sure we are paired with it
                                mCollisionManager->AddPair(currCollObj,                            
                                                           mCurrentStatics[collisionAreaIndex][l].mStaticPhysDSG->mpSimState()->GetCollisionObject(),
                                                           collisionAreaIndex);
                            }
                        }
                        for(l = 0; l < this->mMaxFencePerArea; l++)
                        {
                            if(mFences[collisionAreaIndex][l].mInCollision)
                            {
                                mCollisionManager->AddPair(currCollObj, 
                                                           mFences[collisionAreaIndex][l].mFenceSimState->GetCollisionObject(),
                                                           collisionAreaIndex);
                            
                            }
                        
                        }
                        */
                        
                    
                    // all vehicles and player character have their own groundplanes already                    
                    if( curr->GetAIRef() != PhysicsAIRef::redBrickVehicle && curr->GetAIRef() != PhysicsAIRef::PlayerCharacter ) 
                    {
                        int groundPlaneIndex = mCurrentDynamics[collisionAreaIndex][k].mDynamicPhysDSG->FetchGroundPlane();
                                              
                        if(groundPlaneIndex != -1)  // just in case, though this should never be -1
                        {
                            sim::CollisionObject* gpCollObj = mGroundPlanePool->GetSimState(groundPlaneIndex)->GetCollisionObject();
                            gpCollObj->SetRayCastingEnabled( false );

                            
        
                            // recall, collision is disabled on the ground plane until it is set on by the object using it
                            mCollisionManager->AddCollisionObject( gpCollObj, collisionAreaIndex );
                    
                            mCollisionManager->AddPair(currCollObj, gpCollObj, collisionAreaIndex);
                        }
                        
                    }


                    break;
                }
            }
    
            //rAssert(k < mMaxDynamics);    
            // TODO - deal with no available slots....
        }       
    }

    // now look through remaining list and remove unclean
    //
    // at this stage, we can only remove the ones that are unclean and at rest.

    PurgeDynamics( collisionAreaIndex );
  
}


//=============================================================================
// static bool IsBreakable
//=============================================================================
// Description: Comment
//
// Parameters:  ( DynaPhysDSG* pObject )
//
// Return:      static 
//
//=============================================================================
static bool IsBreakable( DynaPhysDSG* pObject )
{
    bool isBreakable;
    CollisionAttributes* attr = pObject->GetCollisionAttributes();
    if ( pObject->GetAIRef() == PhysicsAIRef::StateProp ||
        pObject->GetAIRef() == PhysicsAIRef::ActorStateProp )
    {
        // *some* stateprops can break, but not ones flagged as PROP_ONE_TIME_MOVEABLE
        if ( pObject->GetCollisionAttributes()->GetClasstypeid() == PROP_ONETIME_MOVEABLE )
            isBreakable = false;
        else
            isBreakable = true;
    }
    else if ( attr != NULL )
    {
        if ( attr->GetClasstypeid() == PROP_BREAKABLE )
        {
            isBreakable = true;
        }
        else
        {
            isBreakable = false;
        }
    }
    else
    {
        isBreakable = false;
    }

    return isBreakable;
}


//=============================================================================
// WorldPhysicsManager::PurgeDynamics
//=============================================================================
// Description: Comment
//
// Parameters:  ( int collisionAreaIndex )
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::PurgeDynamics( int collisionAreaIndex )
{
    rAssert( collisionAreaIndex > WorldPhysicsManager::INVALID_COLLISION_AREA );

    for(int i = 0; i < mMaxDynamics; i++)
    {
        // TODO ! 
        // test for vehicles must be different;
        // they use they're own ground plane!        

        if( mCurrentDynamics[collisionAreaIndex][i].mDynamicPhysDSG != 0 && 
            mCurrentDynamics[collisionAreaIndex][i].clean == false )
        {
            DynaPhysDSG* pObject = mCurrentDynamics[collisionAreaIndex][i].mDynamicPhysDSG;

            // need to keep updating when "at rest" if it's a onetime movable that has been hit
            // so that it can be removed when it goes out of view
            bool atRest = pObject->IsAtRest();

            if(atRest && pObject->GetCollisionAttributes())
            {
                atRest = atRest && !((pObject->GetCollisionAttributes()->GetClasstypeid() == PROP_ONETIME_MOVEABLE)
                         && pObject->mIsHit);
            }

            if ( atRest || IsBreakable( pObject ) )
            {
                mCollisionManager->RemoveCollisionObject(pObject->mpSimState()->GetCollisionObject(), collisionAreaIndex);

                // don't do fuck if we are a car - temp TODO!!
                // this test doesn't seem to work
                if(pObject->GetAIRef() != PhysicsAIRef::redBrickVehicle)
                {          
                    int groundPlaneIndex = pObject->GetGroundPlaneIndex();
                    if(groundPlaneIndex != -1)  // should never be false...
                    {
                        sim::CollisionObject* gpCollObj = mGroundPlanePool->GetSimState(groundPlaneIndex)->GetCollisionObject();
                    
                        mCollisionManager->RemoveCollisionObject(gpCollObj, collisionAreaIndex);

                        pObject->FreeGroundPlane();     // this decrements the groundplaneref, but we still need to take it out of 
                        // there are no more refs
                    }
                }
                pObject->Release( );
                mCurrentDynamics[collisionAreaIndex][i].mDynamicPhysDSG = 0;
                mCurrentDynamics[collisionAreaIndex][i].clean = false; // this is redundant
            }
        }
     }    
}


//=============================================================================
// WorldPhysicsManager::UpdateSimluatingDynamicObjectGroundPlanes
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::UpdateSimluatingDynamicObjectGroundPlanes()
{
    int i;
    for(i = 0; i < mNumCollisionAreas; i++)
    {        
        if(!mCollisionAreaAllocated[i])
        {
            continue;
        }


        int j;
        for(j = 0; j < mMaxDynamics; j++)
        {
            if( mCurrentDynamics[i][j].mDynamicPhysDSG && 
                mCurrentDynamics[i][j].mDynamicPhysDSG->GetSimState()->GetControl() == sim::simSimulationCtrl &&
                mCurrentDynamics[i][j].mDynamicPhysDSG->GetSimState()->mAIRefIndex != PhysicsAIRef::redBrickVehicle &&
                mCurrentDynamics[i][j].mDynamicPhysDSG->GetSimState()->mAIRefIndex != PhysicsAIRef::PlayerCharacter )    // TODO - vehicles use also?

                // TODO - rest test??? or is that covered under sim control?

            {
                // need to update this ground plane
                // TODO
                // is the position of the DSG object being updated correctly???????
                //
                // no implementatino of that for dynamics yet!

                rmt::Vector inPosition =  mCurrentDynamics[i][j].mDynamicPhysDSG->rPosition();
                float inRadius = 1.0f;
    
                // ignore the tri values
                rmt::Vector triNormal;
                rmt::Vector triPosition;

                bool foundPlane;
                rmt::Vector planeNormal;
                rmt::Vector planePosition;


                GetIntersectManager()->FindIntersection(inPosition, 
                                                        foundPlane,
                                                        planeNormal,
                                                        planePosition);


                if(foundPlane)
                {                    
                    mGroundPlanePool->UpdateGroundPlane(mCurrentDynamics[i][j].mDynamicPhysDSG->GetGroundPlaneIndex(), planePosition, planeNormal);
                }
                else
                {
                    //rAssert(0);
                    int stophere = 1;   // hopefully not.
                }

            }

        }
    } 
    

}


//=============================================================================
// WorldPhysicsManager::GetNewGroundPlane
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      int 
//
//=============================================================================
int WorldPhysicsManager::GetNewGroundPlane(sim::SimState* simStateOwner)
{
    return mGroundPlanePool->GetNewGroundPlane(simStateOwner);
}


//=============================================================================
// WorldPhysicsManager::FreeGroundPlane
//=============================================================================
// Description: Comment
//
// Parameters:  (int index)
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::FreeGroundPlane(int index)
{
    mGroundPlanePool->FreeGroundPlane(index);
}


//=============================================================================
// WorldPhysicsManager::EnableGroundPlaneCollision
//=============================================================================
// Description: Comment
//
// Parameters:  (int index)
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::EnableGroundPlaneCollision(int index)
{
    //char buffy[128];
    //sprintf(buffy, "enabling ground plane index %d\n", index);

    //rReleaseString(buffy);

    mGroundPlanePool->EnableCollision(index);
}


//=============================================================================
// WorldPhysicsManager::DisableGroundPlaneCollision
//=============================================================================
// Description: Comment
//
// Parameters:  (int index)
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::DisableGroundPlaneCollision(int index)
{
    //char buffy[128];
    //sprintf(buffy, "disabling ground plane index %d\n", index);

    //rReleaseString(buffy);

    mGroundPlanePool->DisableCollision(index);
}

//=============================================================================
// WorldPhysicsManager::UpdateDynamicObjects
//=============================================================================
// Description: Comment
//
// Parameters:  (float dt, int collisionAreaIndex)
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::UpdateDynamicObjects(float dt, int collisionAreaIndex)
{
    rAssert( collisionAreaIndex > WorldPhysicsManager::INVALID_COLLISION_AREA );
    rAssert( mCollisionAreaAllocated[collisionAreaIndex] );

    int j;
    for(j = 0; j < mMaxDynamics; j++)
    {
        if(mCurrentDynamics[collisionAreaIndex][j].mDynamicPhysDSG) 
        {
            mCurrentDynamics[collisionAreaIndex][j].mDynamicPhysDSG->RestTest();

            bool atRest = mCurrentDynamics[collisionAreaIndex][j].mDynamicPhysDSG->IsAtRest();

            // need to keep updating when "at rest" if it's a onetime movable that has been hit
            if(atRest && mCurrentDynamics[collisionAreaIndex][j].mDynamicPhysDSG->GetCollisionAttributes())
            {
                atRest = atRest && !((mCurrentDynamics[collisionAreaIndex][j].mDynamicPhysDSG->GetCollisionAttributes()->GetClasstypeid() == PROP_ONETIME_MOVEABLE)
                         && mCurrentDynamics[collisionAreaIndex][j].mDynamicPhysDSG->mIsHit);
            }

            // don't update vehicles or characters (they are updated elsewhere), 
            // stuff at rest, or stuff that has already been updated.
            if((mCurrentDynamics[collisionAreaIndex][j].mDynamicPhysDSG->GetSimState()->mAIRefIndex != PhysicsAIRef::redBrickVehicle) &&
               (mCurrentDynamics[collisionAreaIndex][j].mDynamicPhysDSG->GetSimState()->mAIRefIndex != PhysicsAIRef::PlayerCharacter ) &&
               (!atRest) &&
               (mCurrentDynamics[collisionAreaIndex][j].mDynamicPhysDSG->GetLastUpdate() != updateFrame))
            {
                // set the update tick so we wont update this again
                mCurrentDynamics[collisionAreaIndex][j].mDynamicPhysDSG->SetLastUpdate(updateFrame); 
                mCurrentDynamics[collisionAreaIndex][j].mDynamicPhysDSG->Update(dt);
            }
        }
    }

}

//=============================================================================
// WorldPhysicsManager::UpdateAnimCollisions
//=============================================================================
// Description: Comment
//
// Parameters:  (float dt, int collisionAreaIndex)
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::UpdateAnimCollisions(float dt, int collisionAreaIndex)
{
    rAssert( collisionAreaIndex > WorldPhysicsManager::INVALID_COLLISION_AREA );
    rAssert(mCollisionAreaAllocated[collisionAreaIndex]);

    int j;
    for(j = 0; j < mMaxUpdateAnimCollisions; j++)
    {
        if(mCurrentUpdateAnimCollisions[collisionAreaIndex][j].mAnimCollisionEntityDSG)
        {                
            if(mCurrentUpdateAnimCollisions[collisionAreaIndex][j].mAnimCollisionEntityDSG->GetLastUpdate() != updateFrame)
            {
                // set the update tick so we wont update this again
                mCurrentUpdateAnimCollisions[collisionAreaIndex][j].mAnimCollisionEntityDSG->SetLastUpdate(updateFrame); 
                mCurrentUpdateAnimCollisions[collisionAreaIndex][j].mAnimCollisionEntityDSG->Update(dt);            
            }
        }

    }
}


//=============================================================================
// WorldPhysicsManager::SubmitFencePiecesPseudoCallback
//=============================================================================
// Description: Comment
//
// Parameters:  (position, radius, collisionAreaIndex)
//
// Return:      void 
//
//=============================================================================
void WorldPhysicsManager::SubmitFencePiecesPseudoCallback(rmt::Vector& position, float radius, int collisionAreaIndex, sim::SimState* callerSimState, bool allowAutoPairing)
{
    rAssert( collisionAreaIndex > WorldPhysicsManager::INVALID_COLLISION_AREA );
    
    ReserveArray<FenceEntityDSG*> fenceDSGList;         

    BEGIN_PROFILE( "FindFenceElems" );
    GetIntersectManager()->FindFenceElems(position, radius, fenceDSGList);  
    END_PROFILE( "FindFenceElems" );
    
    int numResults = fenceDSGList.mUseSize;       

    //int numResults = 1;
    if(numResults > 0)
    {
        int stophere = 1;
    }
    
    if(numResults > mMaxFencePerArea)
    {
        numResults = mMaxFencePerArea;
    }

    // fill in debug rendering list:
    int i;

    for(i = 0; i < numResults; i++)
    {
        mFenceDSGResults[i]->mStartPoint = fenceDSGList[i]->mStartPoint;
        mFenceDSGResults[i]->mEndPoint = fenceDSGList[i]->mEndPoint;
        mFenceDSGResults[i]->mNormal = fenceDSGList[i]->mNormal;   

        mFenceDSGResults[i]->mStartPoint.y = position.y;
        mFenceDSGResults[i]->mEndPoint.y = position.y;
        

    }
    mNumDebugFences = numResults;

    // reset list
    for(i = 0; i < mMaxFencePerArea; i++)
    {
        mFences[collisionAreaIndex][i].mClean = false;
    }
    
    for(i = 0; i < numResults; i++)
    {

        // i is candidate for submission

        // just in case, do this in function, not here
        //fenceDSGList[i]->mStartPoint.y = position.y;
        //fenceDSGList[i]->mEndPoint.y = position.y;        

        mFences[collisionAreaIndex][i].start = fenceDSGList[i]->mStartPoint;
        mFences[collisionAreaIndex][i].end = fenceDSGList[i]->mEndPoint;

        if(UpdateFencePiece(position, mFences[collisionAreaIndex][i].mFenceSimState, 
                            fenceDSGList[i]->mStartPoint, fenceDSGList[i]->mEndPoint, 
                            fenceDSGList[i]->mNormal,
                            callerSimState))
        {
            // ok for this one to be in collision
            //

            if(mFences[collisionAreaIndex][i].mInCollision == false)
            {
                // we need to add this into the collision manager
                if(allowAutoPairing)
                {
                    mFences[collisionAreaIndex][i].mFenceSimState->GetCollisionObject()->SetAutoPair(true);
                }
                else
                {
                    mFences[collisionAreaIndex][i].mFenceSimState->GetCollisionObject()->SetAutoPair(false);                
                }
                
                mCollisionManager->AddCollisionObject(mFences[collisionAreaIndex][i].mFenceSimState->GetCollisionObject(), collisionAreaIndex);
                mCollisionManager->AddPair(callerSimState->GetCollisionObject(), mFences[collisionAreaIndex][i].mFenceSimState->GetCollisionObject(), collisionAreaIndex);
                mFences[collisionAreaIndex][i].mInCollision = true;
                
            }   
            mFences[collisionAreaIndex][i].mClean = true;
          
        }
    }

    // update list
    for(i = 0; i < mMaxFencePerArea; i++)
    {
        if(mFences[collisionAreaIndex][i].mInCollision == true && 
           mFences[collisionAreaIndex][i].mClean == false)
        {
            mCollisionManager->RemoveCollisionObject(mFences[collisionAreaIndex][i].mFenceSimState->GetCollisionObject(), collisionAreaIndex);
            mFences[collisionAreaIndex][i].mInCollision = false;
        }


    }

}


//=============================================================================
// WorldPhysicsManager::UpdateFencePiece
//=============================================================================
// Description: Comment
//
// Parameters:  (SimState* fencePiece, rmt::Vector* end0, rmt::Vector* end1, rmt::Vector* normal)
//
// Return:      void 
//
//=============================================================================
bool WorldPhysicsManager::UpdateFencePiece(rmt::Vector& callerPosition, sim::ManualSimState* fencePiece, rmt::Vector& end0, 
                                           rmt::Vector& end1, rmt::Vector& normal, sim::SimState* callerSimState)
{

    // on the collision volume
    // set position, 
    // mAxis
    // mLength
    rmt::Vector o0, o1, o2;
    float l0, l1, l2;

    // pain in the ass - just get it to fucking work
    rmt::Vector localEnd0 = end0;
    rmt::Vector localEnd1 = end1;

    localEnd0.y = callerPosition.y;
    localEnd1.y = callerPosition.y;

    o1.Set(0.0f, 1.0f, 0.0f);   // y is always straight up

    o2 = normal;   // safe to assume this is normalized?
    
    o0.CrossProduct(o1, o2);

    //l1 = 10000.0f;  // magic big ass number
    l1 = 10.0f;  // use a smaller number for now so the debug drawn volumes are easier to see

    rmt::Vector segment;
    segment.Sub(localEnd1, localEnd0);

    float segmentLength = segment.Magnitude();
    
    l0 = segmentLength * 0.5f;
    
    
    l2 = 10.0f; // ? whatever?  TODO - is this value ok?

    if(callerSimState->mAIRefIndex == PhysicsAIRef::CameraSphere)
    {
        // Cary!
        // change this number!
        
        // this is half the length of the fence piece obbox
    
        l2 = 2.0f;
        //l2 = 10.0f;
    }
    
    rmt::Vector midpoint;
    midpoint.Add(localEnd0, localEnd1);
    midpoint.Scale(0.5f);


    // before doing anything else, test if we are pointed at the fucker
    // TODO - maybe this test should be moved up to the submit part?
    // maybe not
    rmt::Vector test;
    test.Sub(callerPosition, midpoint);

    if(test.DotProduct(normal) < 0.0f && callerSimState->mAIRefIndex != PhysicsAIRef::CameraSphere)
    {
        
        fencePiece->GetCollisionObject()->SetCollisionEnabled(false);
        return false;
    }

    rmt::Vector position = midpoint;
    rmt::Vector centerAdjust = o2;
    centerAdjust.Scale(l2);
    position.Sub(centerAdjust); 


    // for convenience:
    sim::CollisionObject* co = fencePiece->GetCollisionObject();
    sim::OBBoxVolume* obbox = (OBBoxVolume*)(co->GetCollisionVolume());

    obbox->Set(position, o0, o1, o2, l0, l1, l2);
    
    co->PostManualUpdate();

    //co->Relocated();
    //obbox->UpdateBBox();

    // only need to do this once
    co->SetCollisionEnabled(true);


    // on the Object call Relocated
    // 
    //virtual void UpdateBBox() {}        
    // make sure mUpdated is true
    // what about dP??
    //
    // what about OptimizeAxis
    // what about SetRotation?

    // GetCollisionObject()->SetCollisionEnabled(true);

    
    //#ifdef RAD_DEBUG

    //sim::DrawCollisionObject(CollisionObject* inObject);
    //sim::DrawCollisionVolume(obbox);
    
    //#endif

    return true;


}

//----------------------------------------------------------------------------
// Segment intersection, stolen, as usual, from www.magic-software.com
//----------------------------------------------------------------------------

// even though we're using 3d vectors (cause that's what we have) this is only a 2d intersection
// in x & z
static bool Find (const rmt::Vector& rkP0, const rmt::Vector& rkD0,
    const rmt::Vector& rkP1, const rmt::Vector& rkD1, rmt::Vector& rkDiff,
    float& rfD0SqrLen, int& riQuantity, float afT[2])
{
    // Intersection is a solution to P0+s*D0 = P1+t*D1.  Rewrite as
    // s*D0 - t*D1 = P1 - P0, a 2x2 system of equations.  If D0 = (x0,y0)
    // and D1 = (x1,y1) and P1 - P0 = (c0,c1), then the system is
    // x0*s - x1*t = c0 and y0*s - y1*t = c1.  The error tests are relative
    // to the size of the direction vectors, |Cross(D0,D1)| >= e*|D0|*|D1|
    // rather than absolute tests |Cross(D0,D1)| >= e.  The quantities
    // P1-P0, |D0|^2, and |D1|^2 are returned for use by calling functions.

    float fDet = rkD1.x*rkD0.z - rkD1.z*rkD0.x;
    rkDiff = rkP1 - rkP0;
    rfD0SqrLen = rkD0.MagnitudeSqr();

    const float fEpsilon = 1e-06f;
    if ( fDet*fDet > fEpsilon*rfD0SqrLen*rkD1.MagnitudeSqr() )
    {
        // Lines intersect in a single point.  Return both s and t values for
        // use by calling functions.
        float fInvDet = 1.0f/fDet;
        riQuantity = 1;
        afT[0] = (rkD1.x*rkDiff.z - rkD1.z*rkDiff.x)*fInvDet;
        afT[1] = (rkD0.x*rkDiff.z - rkD0.z*rkDiff.x)*fInvDet;
    }
    else
    {
        // lines are parallel
        fDet = rkD0.x*rkDiff.z - rkD0.z*rkDiff.x;
        if ( fDet*fDet > fEpsilon*rfD0SqrLen*rkDiff.MagnitudeSqr() )
        {
            // lines are disjoint
            riQuantity = 0;
        }
        else
        {
            // lines are the same
            riQuantity = 2;
        }
    }

    return riQuantity != 0;
}

//----------------------------------------------------------------------------
static bool FindIntersection (const rmt::Vector& origin0, const rmt::Vector& direction0,
                            const rmt::Vector& origin1, const rmt::Vector& direction1,
                            int& riQuantity)
{
    float afT[2];

    rmt::Vector kDiff;
    float fD0SqrLen;
    bool bIntersects = Find(origin0, direction0,
        origin1,direction1,kDiff,fD0SqrLen,
        riQuantity,afT);

    if ( bIntersects )
    {
        if ( riQuantity == 1 )
        {
            if ( afT[0] < 0.0f || afT[0] > 1.0f
            ||   afT[1] < 0.0f || afT[1] > 1.0f )
            {
                // lines intersect, but segments do not
                riQuantity = 0;
            }
        }
        else
        {
            // segments are on the same line
            float fDotRS = direction0.Dot(direction1);
            float fDot0, fDot1;
            if ( fDotRS > 0.0f )

            {
                fDot0 = kDiff.Dot(direction0);
                fDot1 = fDot0 + fDotRS;
            }
            else
            {
                fDot1 = kDiff.Dot(direction0);
                fDot0 = fDot1 + fDotRS;
            }

            // compute intersection of [t0,t1] and [0,1]
            if ( fDot1 < 0.0f || fDot0 > fD0SqrLen )
            {
                riQuantity = 0;
            }
            else if ( fDot1 > 0.0f )
            {
                if ( fDot0 < fD0SqrLen )
                {
                    float fInvLen = 1.0f/fD0SqrLen;
                    riQuantity = 2;
                    afT[0] = ( fDot0 < 0.0f ? 0.0f : fDot0*fInvLen );
                    afT[1] = ( fDot1 > fD0SqrLen ? 1.0f : fDot1*fInvLen );
                }
                else  // fT0 == 1
                {
                    riQuantity = 1;
                    afT[0] = 1.0f;
                }
            }
            else  // fT1 == 0
            {
                riQuantity = 1;
                afT[0] = 0.0f;
            }
        }
    }

    return riQuantity != 0;
}

bool WorldPhysicsManager::FenceSanityCheck(unsigned collisionAreaIndex, const rmt::Vector lastFrame, const rmt::Vector thisFrame, rmt::Vector* fixPos)
{
    rmt::Vector dir, dirTweak;
    dir.Sub(thisFrame, lastFrame);

    if(dir.MagnitudeSqr() == 0.0f)
    {
        return false;
    }

    dirTweak = dir;
    dirTweak.Normalize();
    dirTweak.Scale(0.07f);
    dir += dirTweak;

    for(int i = 0; i < mMaxFencePerArea; i++)
    {
        if(mFences[collisionAreaIndex][i].mClean)
        {
            rmt::Vector fenceDir, fenceTweak, fenceStart;
            fenceDir.Sub(mFences[collisionAreaIndex][i].end, mFences[collisionAreaIndex][i].start);

            fenceTweak = fenceDir;
            fenceTweak.Normalize();
            fenceTweak.Scale(0.07f);

            fenceDir += fenceTweak;
            fenceDir += fenceTweak;

            fenceStart = mFences[collisionAreaIndex][i].start;
            fenceStart -= fenceTweak;
            
            int quantity;

            if(FindIntersection(lastFrame, dir, fenceStart, fenceDir, quantity))
            {
                if(quantity == 1)
                {
                    *fixPos = lastFrame;
                    fixPos->y = thisFrame.y;
                    return true;
                }
            }
        }
    }
    return false;
}

float WorldPhysicsManager::GetLoopTime()
{
    return mLoopTime;
}
