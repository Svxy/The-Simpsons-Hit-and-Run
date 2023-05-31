//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        groundplanepool.cpp
//
// Description: manage pool of ground planes for dynamic physics objects
//
// History:     July 31, 2002 - created, gmayer
//
//=============================================================================


//========================================
// System Includes
//========================================

#include <simcommon/simstate.hpp>


#include <simcommon/simenvironment.hpp>         
#include <simcollision/collisionmanager.hpp>

#include <simcollision/collisionvolume.hpp>
#include <simcollision/collisiondisplay.hpp>

#include <simcommon/physicsproperties.hpp>

#include <raddebug.hpp>


//========================================
// Project Includes
//========================================

#include <worldsim/groundplanepool.h>
#include <worldsim/physicsairef.h>

#include <memory/srrmemory.h>


//=============================================================================
// GroundPlanePool::GroundPlanePool
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      GroundPlanePool
//
//=============================================================================
GroundPlanePool::GroundPlanePool(int num)
{
MEMTRACK_PUSH_GROUP( "GroundPlanePool" );
    mTotalNum = num;

//    #ifdef RAD_GAMECUBE
//        HeapMgr()->PushHeap( GMA_GC_VMM );
//    #else 
        HeapMgr()->PushHeap( GMA_PERSISTENT );
//    #endif

    mPool = new sim::ManualSimState*[mTotalNum];
    //mPool = new sim::SimState*[mTotalNum];
    mInUse = new bool[mTotalNum];
    
    mSimStateOwners = new sim::SimState*[mTotalNum];
    
    mGroundPlanePhysicsProperties = new sim::PhysicsProperties;
    
    int i;
    for(i = 0; i < mTotalNum; i++)
    {
        rmt::Vector p(0.0f, 0.0f, 0.0f);
        rmt::Vector n(0.0f, 1.0f, 0.0f);

        sim::WallVolume* tempwall = new sim::WallVolume(p, n);        

        // TODO - are the temp volumes getting deleted by the sim state?
        mPool[i] = (sim::ManualSimState*)(sim::SimState::CreateManualSimState(tempwall));
        
        //static SimState* CreateSimState(CollisionVolume* inCollisionVolume, char* inName = NULL, tEntityStore* inStore = NULL);

        //mPool[i] = sim::SimState::CreateSimState(tempwall);      
        
        mPool[i]->AddRef();

        mPool[i]->GetCollisionObject()->SetManualUpdate(true); 
        mPool[i]->GetCollisionObject()->SetAutoPair(false);         
        mPool[i]->GetCollisionObject()->SetIsStatic(true);
        
        //mPool[i]->GetCollisionObject()->SetPhysicsProperties(mGroundPlanePhysicsProperties);
        mPool[i]->SetPhysicsProperties(this->mGroundPlanePhysicsProperties);

        // give a reasonable name for debugging purposes...
        char buffy[128];
        sprintf(buffy, "groundplanepool_id%d", i);
        mPool[i]->GetCollisionObject()->SetName(buffy); 

        mInUse[i] = false;

        mPool[i]->GetCollisionObject()->SetCollisionEnabled(false);  

        mPool[i]->mAIRefIndex = PhysicsAIRef::redBrickPhizMoveableGroundPlane;
        mPool[i]->mAIRefPointer = 0;  // only set if object is derived from CollisionEntityDSG
        
        
        mSimStateOwners[i] = 0;
        
        
        
    }
//    #ifdef RAD_GAMECUBE
//        HeapMgr()->PopHeap( GMA_GC_VMM );
//    #else 
        HeapMgr()->PopHeap( GMA_PERSISTENT );
//    #endif
MEMTRACK_POP_GROUP( "GroundPlanePool" );
}

//=============================================================================
// GroundPlanePool::~GroundPlanePool
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      GroundPlanePool
//
//=============================================================================
GroundPlanePool::~GroundPlanePool()
{
    int i;
    for(i = 0; i < mTotalNum; i++)
    {
        mPool[i]->Release();              
    }
    delete mPool;
    delete mInUse;   
   
    delete mSimStateOwners;    

}


//=============================================================================
// GroundPlanePool::GetNewGroundPlane
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      int 
//
//=============================================================================
int GroundPlanePool::GetNewGroundPlane(sim::SimState* simStateOwner)
{
    // find first one that's not in use
    int i;
    for(i = 0; i < mTotalNum; i++)
    {
        if(mInUse[i] == false)
        {
            mInUse[i] = true;
            
            mSimStateOwners[i] = simStateOwner;
            
            return i;
        }
    }

    // failure
    return -1;
}

//=============================================================================
// GroundPlanePool::UpdateGroundPlane
//=============================================================================
// Description: Comment
//
// Parameters:  (int index)
//
// Return:      void 
//
//=============================================================================
void GroundPlanePool::UpdateGroundPlane(int index, rmt::Vector& position, rmt::Vector& normal)
{
    rAssert( 0 <= index && index < mTotalNum );

    if(index > -1 && index < mTotalNum) // just in case
    {
        rAssert(mInUse[index]);

        // for convenience:
        sim::CollisionObject* co = mPool[index]->GetCollisionObject();
        sim::WallVolume* wall = (sim::WallVolume*)(co->GetCollisionVolume());

        wall->mPosition = position;
        wall->mNormal = normal;
        
        co->PostManualUpdate();

        //co->Relocated();
        //obbox->UpdateBBox();

        // only need to do this once - TODO .. ? only enable when 'owner' object get's hit? - 
        //co->SetCollisionEnabled(true);    
    }
}


//=============================================================================
// GroundPlanePool::EnableCollision
//=============================================================================
// Description: Comment
//
// Parameters:  (int index)
//
// Return:      void 
//
//=============================================================================
void GroundPlanePool::EnableCollision(int index)
{
    rAssert( 0 <= index && index < mTotalNum );
    rAssert( mInUse[index] );

    sim::CollisionObject* co = mPool[index]->GetCollisionObject();
    co->SetCollisionEnabled(true);    
}


//=============================================================================
// GroundPlanePool::DisableCollision
//=============================================================================
// Description: Comment
//
// Parameters:  (int index)
//
// Return:      void 
//
//=============================================================================
void GroundPlanePool::DisableCollision(int index)
{
    rAssert( 0 <= index && index < mTotalNum );
    rAssert( mInUse[index] );

    sim::CollisionObject* co = mPool[index]->GetCollisionObject();
    co->SetCollisionEnabled(false);    
}


//=============================================================================
// GroundPlanePool::FreeGroundPlane
//=============================================================================
// Description: Comment
//
// Parameters:  (int index)
//
// Return:      void 
//
//=============================================================================
void GroundPlanePool::FreeGroundPlane(int index)
{
    // make this safe to call with any value.
    //
    // note: we are not responsible for taking it out of any collision or anything like that.
    if(index < 0 || index >= mTotalNum)
    {
        return;
    }
    mInUse[index] = false;
    mSimStateOwners[index] = 0;
}



//=============================================================================
// GroundPlanePool::FreeAllGroundPlanes
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool GroundPlanePool::FreeAllGroundPlanes() // returns false if there was a problem...
{
    // called when quitting game or a level
    bool ok = true;
    int i;
    for(i = 0; i < mTotalNum; i++)
    {
        if(mInUse[i])
        {
            ok = false;
            rAssert(false);
        }
        mInUse[i] = false;
        mSimStateOwners[i] = 0;
    }
    return ok;

}



//============================================================================= 
// GroundPlanePool::GetSimState
//=============================================================================
// Description: Comment
//
// Parameters:  (int index)
//
// Return:      sim
//
//=============================================================================
sim::ManualSimState* GroundPlanePool::GetSimState(int index)
//sim::SimState* GroundPlanePool::GetSimState(int index)
{
    rAssert( 0 <= index && index < mTotalNum );
    rAssert(mInUse[index]);

    return mPool[index];
}


