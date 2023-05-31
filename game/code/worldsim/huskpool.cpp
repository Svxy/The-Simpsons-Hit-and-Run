//=============================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// File:        huskpool.cpp
//
// Description: 
//
// History:     Mar 27, 2003 - created, gmayer
//
//=============================================================================


#include <worldsim/vehiclecentral.h>
#include <worldsim/redbrick/geometryvehicle.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/huskpool.h>
#include <worldsim/redbrick/trafficlocomotion.h>

#include <memory/srrmemory.h>

#include <mission/gameplaymanager.h>


//=============================================================================
// HuskPool::HuskPool
//=============================================================================
// Description: Comment
//
// Parameters:  (int num)
//
// Return:      HuskPool
//
//=============================================================================
HuskPool::HuskPool()
{
    mTotalNum = 0;
}


//=============================================================================
// ::~HuskPool
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      HuskPool
//
//=============================================================================
HuskPool::~HuskPool()
{

}


//=============================================================================
// HuskPool::Init
//=============================================================================
// Description: Comment
//
//      this has to be called from GameplayContext and DemoContext OnStart

//
// Parameters:  (int num)
//
// Return:      void 
//
//=============================================================================
void HuskPool::Init(int num)
{

MEMTRACK_PUSH_GROUP( "HuskPool Init" );

    GameMemoryAllocator gma = GetGameplayManager()->GetCurrentMissionHeap();    
    HeapMgr()->PushHeap (gma);

    mTotalNum = num;

    mHuskArray = new HuskData[num];
    
    for( int i=0; i<num; i++)
    {
        // initially make all husks VT_TRAFFIC
        // set the husk type to whatever the vehicle that just got destroyed is...
    
        Vehicle* huskVehicle = GetVehicleCentral()->InitVehicle("huskA", false, 0, VT_TRAFFIC);
        rAssert( huskVehicle );

        huskVehicle->AddRef();
        huskVehicle->SetLocomotion( VL_PHYSICS );
#ifdef DEBUGWATCH
        // HACK:
        // Automatically take it out of vehicleAIRender, so we don't 
        // clutter up the render slots
        huskVehicle->mTrafficLocomotion->GetAI()->UnregisterAI();
#endif

        mHuskArray[i].huskVehicle = huskVehicle;
        mHuskArray[i].originalVehicle = NULL;
        mHuskArray[i].inUse = false;    
    }    
    HeapMgr()->PopHeap (gma);

MEMTRACK_PUSH_GROUP( "HuskPool Init" );
   
}


//=============================================================================
// HuskPool::Empty
//=============================================================================
// Description: Comment
//
//
//  called from PauseContext and DemoContext OnStop
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void HuskPool::Empty()
{
    // called when you're leaving the level
    int i;
    for(i = 0; i < mTotalNum; i++)
    {
        // TODO:
        // Should do this just in case?
        //GetVehicleCentral()->RemoveVehicleFromActiveList( mHuskArray[i].huskVehicle );

        mHuskArray[i].huskVehicle->ReleaseVerified();    
        mHuskArray[i].huskVehicle = NULL;

        if( mHuskArray[i].originalVehicle )
        {
            mHuskArray[i].originalVehicle->Release();
            mHuskArray[i].originalVehicle = NULL;
        }
    }
    
    mTotalNum = 0;
    delete[] mHuskArray;
}


//=============================================================================
// HuskPool::RequestHusk
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      Vehicle
//
//=============================================================================
Vehicle* HuskPool::RequestHusk( VehicleType vt, Vehicle* originalVehicle )
{
    rAssert( originalVehicle );
    if( originalVehicle == NULL )
    {
        return NULL;
    }

    if( !WillMakeConvincingHusk( originalVehicle ) )
    {
        return NULL;
    }

    for( int i = 0; i < mTotalNum; i++ )
    {
        if(mHuskArray[i].inUse == false)
        {
            // give 'em this one
            mHuskArray[i].inUse = true;
            mHuskArray[i].huskVehicle->mVehicleType = vt;   // I'm pretty sure this is safe to set on the fly
            mHuskArray[i].huskVehicle->mGeometryVehicle->SetFadeAlpha( 255 ); // restore vehicle fade alpha

            // store away the original vehicle
            rAssert( mHuskArray[i].originalVehicle == NULL );
            mHuskArray[i].originalVehicle = originalVehicle;
            mHuskArray[i].originalVehicle->AddRef();

            return mHuskArray[i].huskVehicle;            
        }
    }

    //rReleaseAssertMsg(0, "Not enough husks to go around!");
    return NULL;
}


//=============================================================================
// HuskPool::FreeHusk
//=============================================================================
// Description: Comment
//
// Parameters:  (Vehicle* husk)
//
// Return:      void 
//
//=============================================================================
void HuskPool::FreeHusk(Vehicle* husk)
{
    if( husk == NULL )
    {
        return;
    }

    for( int i = 0; i < mTotalNum; i++)
    {
        if( mHuskArray[i].huskVehicle == husk &&
			mHuskArray[i].inUse )
        {
            mHuskArray[i].inUse = false;

            rAssert( mHuskArray[i].originalVehicle );
            mHuskArray[i].originalVehicle->Release();
            mHuskArray[i].originalVehicle = NULL;

            return;
        }
    }
}

bool HuskPool::IsHuskType( VehicleEnum::VehicleID id )
{   
    // NOTE:
    // If we start using other Husk models other than "huskA", 
    // we add the ID checks here...
    if( id == VehicleEnum::HUSKA )
    {
        return true;
    }
    return false;
}


Vehicle* HuskPool::FindOriginalVehicleGivenHusk( Vehicle* husk )
{
    if( husk == NULL )
    {
        return NULL;
    }

    for( int i = 0; i < mTotalNum; i++)
    {
        if(mHuskArray[i].huskVehicle == husk)
        {
            rAssert( mHuskArray[i].inUse );
            rAssert( mHuskArray[i].originalVehicle );
            return mHuskArray[i].originalVehicle;
        }
    }

    return NULL;
}

Vehicle* HuskPool::FindHuskGivenOriginalVehicle( Vehicle* v )
{
    if( v == NULL )
    {
        return NULL;
    }

    for( int i = 0; i < mTotalNum; i++)
    {
        if(mHuskArray[i].originalVehicle == v)
        {
            rAssert( mHuskArray[i].inUse );
            rAssert( mHuskArray[i].huskVehicle );
            return mHuskArray[i].huskVehicle;
        }
    }

    return NULL;

}


bool HuskPool::WillMakeConvincingHusk( Vehicle* origV )
{
    // these cars don't look anything remotely like 
    // HuskA, so we blow them up and leave nothing behind

    if( origV->mVehicleID == VehicleEnum::HONOR_V || 
        origV->mVehicleID == VehicleEnum::SCHOOLBU || 
        origV->mVehicleID == VehicleEnum::WILLI_V || 
        origV->mVehicleID == VehicleEnum::HALLO   || 
        origV->mVehicleID == VehicleEnum::OBLIT_V || 

        origV->mVehicleID == VehicleEnum::MONO_V  || 
        origV->mVehicleID == VehicleEnum::KNIGH_V || 
        origV->mVehicleID == VehicleEnum::CFIRE_V || 
        origV->mVehicleID == VehicleEnum::CCOLA   || 
        origV->mVehicleID == VehicleEnum::HBIKE_V || 

        origV->mVehicleID == VehicleEnum::ROCKE_V || 
        origV->mVehicleID == VehicleEnum::ATV_V   || 
        origV->mVehicleID == VehicleEnum::DUNE_V  || 
        origV->mVehicleID == VehicleEnum::COFFIN  || 
        origV->mVehicleID == VehicleEnum::SHIP    || 

        origV->mVehicleID == VehicleEnum::WITCHCAR )
    {
        return false;
    }
    return true;
}