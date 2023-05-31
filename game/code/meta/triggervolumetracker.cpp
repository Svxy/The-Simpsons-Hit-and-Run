//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        triggervolumetracker.cpp
//
// Description: Implement TriggerVolumeTracker
//
// History:     13/05/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <raddebugwatch.hpp>
#include <radmath/radmath.hpp>
#include <radtime.hpp>

#include <p3d/debugdraw.hpp>

//========================================
// Project Includes
//========================================
#include <meta/triggervolumetracker.h>
#include <meta/triggervolume.h>
#include <meta/triggerlocator.h>
#include <meta/locatorevents.h>
#include <meta/eventlocator.h>
#include <meta/locatortypes.h>
#include <meta/zoneeventlocator.h>

#include <main/commandlineoptions.h>

#include <memory/srrmemory.h>

#include <worldsim/avatarmanager.h>
#include <worldsim/avatar.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/redbrick/vehicle.h>

#include <mission/gameplaymanager.h>

#include <debug/profiler.h>

#include <render/intersectmanager/intersectmanager.h>

#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>
#include <camera/supercam.h>

#include <simcommon/simstate.hpp>
#include <simcommon/simstatearticulated.hpp>
#include <simcommon/simulatedobject.hpp>
#include <simcommon/simenvironment.hpp>
#include <simcollision/collisionobject.hpp>
#include <simcollision/collisionvolume.hpp>
#include <simcollision/collisionmanager.hpp>


LocatorEvent::Event ALLOWED_AI_EVENTS[] =
{
    LocatorEvent::DEATH,
    LocatorEvent::PARKED_BIRDS,
    LocatorEvent::WHACKY_GRAVITY,
    LocatorEvent::GOO_DAMAGE
};

unsigned int NUM_ALLOWED_AI_EVENTS = sizeof( ALLOWED_AI_EVENTS ) / sizeof( ALLOWED_AI_EVENTS[0] );

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

// Static pointer to instance of singleton.
TriggerVolumeTracker* TriggerVolumeTracker::spInstance = NULL;

rmt::Vector TriggerVolumeTracker::sP1;
rmt::Vector TriggerVolumeTracker::sP2;
rmt::Vector TriggerVolumeTracker::sP3;
rmt::Vector TriggerVolumeTracker::sP4;

sim::CollisionVolumeTypeEnum TriggerVolumeTracker::sColType;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

void TriggerVolumeTracker::CreateInstance()
{
    MEMTRACK_PUSH_GROUP( "TriggerVolumeTracker" );
    rAssert( spInstance == NULL );
    spInstance = new(GMA_PERSISTENT) TriggerVolumeTracker();
    rAssert( spInstance );
    MEMTRACK_POP_GROUP( "TriggerVolumeTracker" );
}

//=============================================================================
// TriggerVolumeTracker::GetInstance
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      TriggerVolumeTracker
//
//=============================================================================
TriggerVolumeTracker* TriggerVolumeTracker::GetInstance()
{
    //rAssert( spInstance );
    return spInstance;
}

//=============================================================================
// TriggerVolumeTracker::DestroyInstance
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TriggerVolumeTracker::DestroyInstance()
{
    if( spInstance != NULL )
    {
        delete( GMA_PERSISTENT, spInstance );
        spInstance = NULL;
    }
}

//==============================================================================
// TriggerVolumeTracker::TriggerVolumeTracker
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
TriggerVolumeTracker::TriggerVolumeTracker() :
    mpTriggerSphere( NULL ),
    mNumRegisteredAI( 0 ),
    mIgnoreTriggers ( false )
#ifndef RAD_RELEASE
    ,
    mDisplayAmbients( false ),
    mDisplayActive( true ),
    mDisplayInactive( false ),
    mDisplayAll( false )
#endif
{
    int i;
    for( i = 0; i < MAX_PLAYERS; i++ )
    {
        mActiveCount[ i ] = 0;
    }

    for ( i = 0; i < MAX_AI; ++i )
    {
        mAICount[ i ] = 0;
    }

    mTriggerCount = 0;

#ifndef RAD_RELEASE
    radDbgWatchAddBoolean( &mDisplayActive, "Display Active Vols" , "Trigger Volume Tracker" );
    radDbgWatchAddBoolean( &mDisplayInactive, "Display Inactive Vols" , "Trigger Volume Tracker" );
    radDbgWatchAddBoolean( &mDisplayAll, "Display ALL" , "Trigger Volume Tracker" );

    for ( i = 0; i < LocatorType::NUM_TYPES; ++i )
    {
        char name[256];
        sprintf( name, "Display %s", LocatorType::Name[i] );
        radDbgWatchAddBoolean( &mDisplayEnable[i], name , "Trigger Volume Tracker" );
        mDisplayEnable[i] = false;
    }

    radDbgWatchAddBoolean( &mDisplayAmbients, "Display Ambients", "Trigger Volume Tracker" );

    material = NULL;

#endif
}


#ifndef RAD_RELEASE
pddiShader* TriggerVolumeTracker::GetMaterial(void) 
{ 
    if(!material)
    {
        HeapMgr()->PushHeap(GMA_PERSISTENT);
        material = p3d::device->NewShader( "simple" );
        material->AddRef();
        material->SetInt( PDDI_SP_BLENDMODE, PDDI_BLEND_ADD);
        material->SetColour( PDDI_SP_AMBIENT, pddiColour( 128, 128, 128, 64 ));
        material->SetColour( PDDI_SP_DIFFUSE, pddiColour( 128, 128, 128, 64 ));
        HeapMgr()->PopHeap(GMA_PERSISTENT);
    }

    return material; 
}
#endif

//==============================================================================
// TriggerVolumeTracker::~TriggerVolumeTracker
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
TriggerVolumeTracker::~TriggerVolumeTracker()
{
    Cleanup();

#ifndef RAD_RELEASE
    radDbgWatchDelete( &mDisplayActive );
    radDbgWatchDelete( &mDisplayInactive );
    radDbgWatchDelete( &mDisplayAll );

    int i;
    for ( i = 0; i < LocatorType::NUM_TYPES; ++i )
    {
        radDbgWatchDelete( &mDisplayEnable[i] );
    }

    radDbgWatchDelete( &mDisplayAmbients );

    if ( material )
    {
        material->Release();
    }
#endif
}

//=============================================================================
// TriggerVolumeTracker::Cleanup
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TriggerVolumeTracker::Cleanup()
{
    unsigned int i;
    int j;
    for( j = 0; j < MAX_PLAYERS; j++ )
    {
        for( i = 0; i < mActiveCount[ j ]; i++ )
        {
            mActiveVolumes[ j ][ i ]->Release();
            mActiveVolumes[ j ][ i ] = NULL;            
        }

        mActiveCount[ j ] = 0;
    }

    for ( j = 0; j < MAX_AI; ++j )
    {
        for ( i = 0; i < mAICount[ j ]; ++i )
        {
            mActiveAIVolumes[ j ][ i ]->Release();
            mActiveAIVolumes[ j ][ i ] = NULL;
        }

        mAICount[ j ] = 0;
    }

    if ( mNumRegisteredAI > 0 )
    {
        for ( i = 0; i < mNumRegisteredAI; ++i )
        {
            mRegisteredVehicles[ i ].mVehicle->Release();
            mRegisteredVehicles[ i ].mVehicle = NULL;
        }
    }

    mNumRegisteredAI = 0;

    for( i = 0; i < mTriggerCount; i++ )
    {
        mTriggerVolumes[ i ]->Release();
        mTriggerVolumes[ i ] = NULL;
    }

    mTriggerCount = 0;
}


//=============================================================================
// TriggerVolumeTracker::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( int elapsedTime )
//
// Return:      void 
//
//=============================================================================
void TriggerVolumeTracker::Update( int elapsedTime )
{
    if ( mIgnoreTriggers )
    {
        //Do nothing
        return;
    }

BEGIN_PROFILE("Trigger Tracker");

    rmt::Vector center;
    const float radius = 1.0f;

    unsigned int iNumPlayers = GetGameplayManager()->GetNumPlayers();

    unsigned int i;
    for( i = 0; i < iNumPlayers; i++ )
    {
        // get the player's location and bounding sphere radius
        GetAvatarManager()->GetAvatarForPlayer( i )->GetPosition( center );

        CheckForActiveVolumes( i, center, radius );
    }

    VehicleCentral* vc = GetVehicleCentral();

    for ( i = 0; i < mNumRegisteredAI; ++i )
    {
        rmt::Vector pos;
        mRegisteredVehicles[ i ].mVehicle->GetPosition( &pos );

        int id = vc->GetVehicleId( mRegisteredVehicles[ i ].mVehicle );

        CheckForActiveVolumes( id + MAX_PLAYERS, pos, radius );
    }

END_PROFILE("Trigger Tracker");
}


//=============================================================================
// TriggerVolumeTracker::AddTrigger
//=============================================================================
// Description: Comment
//
// Parameters:  ( TriggerVolume* triggerVolume )
//
// Return:      void 
//
//=============================================================================
void TriggerVolumeTracker::AddTrigger( TriggerVolume* triggerVolume )
{
    rAssert( mTriggerCount < MAX_VOLUMES );
    unsigned int index;
    for( index = 0; index < mTriggerCount; index++ )
    {
        if( mTriggerVolumes[ index ] == triggerVolume )
        {
            return;
        }
    }

    mTriggerVolumes[ mTriggerCount ] = triggerVolume;
    triggerVolume->AddRef();
    
    mTriggerCount++;
}

//=============================================================================
// TriggerVolumeTracker::RemoveTrigger
//=============================================================================
// Description: Comment
//
// Parameters:  (  TriggerVolume* triggerVolume )
//
// Return:      void 
//
//=============================================================================
void TriggerVolumeTracker::RemoveTrigger( TriggerVolume* triggerVolume )
{
    unsigned int index;
    for( index = 0; index < mTriggerCount; index++ )
    {
        if( mTriggerVolumes[ index ] == triggerVolume )
        {
            //Shift the last trigger into the new empty position.
            mTriggerVolumes[ index ] = mTriggerVolumes[ mTriggerCount-1 ];
            triggerVolume->Release();
    
            mTriggerCount--;

            break;
        }
    }

    for( int player = 0; player < GetGameplayManager()->GetNumPlayers(); player++ )
    {
        for( index = 0; index < mActiveCount[ player ]; index++ )
        {
            if( mActiveVolumes[ player ][ index ] == triggerVolume )
            {
                RemoveActive( player, index );
                break;
            }
        }
    }

    for ( unsigned int ai = 0; ai < mNumRegisteredAI; ++ai )
    {
        for ( index = 0; index < mAICount[ ai ]; ++index )
        {
            if ( mActiveAIVolumes[ ai ][ index ] == triggerVolume )
            {
                RemoveAIActive( ai, index );
            }
        }
    }
}


//=============================================================================
// TriggerVolumeTracker::UnregisterFromInventory
//=============================================================================
// Description: Comment
//
// Parameters:  ( tInventory* inv )
//
// Return:      void 
//
//=============================================================================
void TriggerVolumeTracker::UnregisterFromInventory( tInventory* inv )
{
//BEGIN_PROFILE( "Trigger Tracker Unregister Inventory ");

    rAssert( inv );

    if ( inv )
    {
        HeapMgr()->PushHeap( GMA_TEMP );
        tInventory::Iterator<TriggerVolume> it;
        HeapMgr()->PopHeap( GMA_TEMP );

        TriggerVolume* tv = it.First();

        while ( tv != NULL )
        {
            RemoveTrigger( tv );

            tv = it.Next();
        }
    }
//END_PROFILE( "Trigger Tracker Unregister Inventory ");
}


//=============================================================================
// TriggerVolumeTracker::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void TriggerVolumeTracker::HandleEvent( EventEnum id, void* pEventData )
{
    switch( id )
    {
    case EVENT_LOCATOR:
        {
            break;
        }
    default:
        {
            // implement all events this class is registered for!
            rAssert( false );
        }
    }
}

//=============================================================================
// TriggerVolumeTracker::Render
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TriggerVolumeTracker::Render()
{
#ifndef RAD_RELEASE
    const Locator* locator;
    unsigned int i;

/*
    Avatar* playerAvatar = GetAvatarManager()->GetAvatarForPlayer( 0 );
    rAssert( playerAvatar );

    if ( playerAvatar->IsInCar() )
    {
        //Test the car like it was the "Green Lantern"  | 0 |.  Two lines and
        //a sphere.
        //Get the BBOX of the car

        Vehicle* vehicle = playerAvatar->GetVehicle();

        sim::CollisionVolume* vecol = vehicle->GetSimState()->GetCollisionObject()->GetCollisionVolume()->GetSubCollisionVolume( 0 );
        rAssert( vecol->Type() == sim::OBBoxVolumeType );

        sim::OBBoxVolume* obBox = static_cast<sim::OBBoxVolume*>( vecol );

        rmt::Vector minCorner, maxCorner;

        minCorner.x = - obBox->mLength[0];
        minCorner.y = - obBox->mLength[1];
        minCorner.z = - obBox->mLength[2];

        maxCorner.x = obBox->mLength[0];
        maxCorner.y = obBox->mLength[1];
        maxCorner.z = obBox->mLength[2];

        p3d::stack->Push();

        rmt::Matrix positionMatrix;
        positionMatrix.Identity();
        positionMatrix.Row(0) = obBox->mAxis[0];
        positionMatrix.Row(1) = obBox->mAxis[1];
        positionMatrix.Row(2) = obBox->mAxis[2];
        positionMatrix.Row(3) = obBox->mPosition;


        p3d::stack->Multiply(positionMatrix);


        tShader* shader = new tShader("simple");
        shader->AddRef();

        P3DDrawOrientedBox( minCorner, maxCorner, *shader );

        shader->Release();

        p3d::stack->Pop();
    }
*/
    for( i = 0; i < mTriggerCount; i++ )
    {
        if ( mDisplayAll )
        {
            mTriggerVolumes[ i ]->Render();
            continue;
        }
        
        locator = mTriggerVolumes[ i ]->GetLocator();

        if ( CommandLineOptions::Get( CLO_SHOW_DYNA_ZONES ) && locator->GetDataType() == LocatorType::DYNAMIC_ZONE )
        {
            mTriggerVolumes[ i ]->Render();
        }
        else if ( !locator )
        {
        }
        else if ( mDisplayActive && locator->GetFlag( Locator::ACTIVE ) )
        {
            if ( mDisplayEnable[ locator->GetDataType() ] )
            {
                mTriggerVolumes[ i ]->Render();
            }
            else if ( locator->GetDataType() == LocatorType::EVENT )
            {
                //We want to look at the ambient events.
                EventLocator* evtLoc = (EventLocator*)( locator );
                LocatorEvent::Event event = evtLoc->GetEventType();
                if ( mDisplayAmbients &&
                     (( event >= LocatorEvent::AMBIENT_SOUND_CITY &&
                       event <= LocatorEvent::AMBIENT_SOUND_MANSION_INTERIOR ) ||
                     ( event >= LocatorEvent::AMBIENT_SOUND_COUNTRY_NIGHT &&
                       event <= LocatorEvent::AMBIENT_SOUND_PLACEHOLDER9 ) ||
                     ( event >= LocatorEvent::AMBIENT_SOUND_SEASIDE_NIGHT &&
                       event <= LocatorEvent::AMBIENT_SOUND_PLACEHOLDER16 ))
                   )
                {
                    mTriggerVolumes[ i ]->Render();
                }
            }
        }
        else if ( mDisplayInactive && !locator->GetFlag( Locator::ACTIVE ) )
        {
            if ( mDisplayEnable[ locator->GetDataType() ] )
            {
                mTriggerVolumes[ i ]->Render();
            }
        }
    }

/*    
    rmt::Vector center;
    GetAvatarManager()->GetAvatarForPlayer( 0 )->GetPosition( center );

    IntersectManager* im = IntersectManager::GetInstance();
    rAssert( im );

    ReserveArray<TriggerVolume*> volDSGList;     

    //float radius = GetSuperCamManager()->GetSCC( 0 )->GetActiveSuperCam()->GetFarPlane();
    float radius = 180.0f;
    im->FindTrigVolElems( center, radius, volDSGList );

    unsigned int time = radTimeGetMilliseconds();

    //Test all the triggers around the car
    for ( i = 0; i < static_cast<unsigned int>(volDSGList.mUseSize); ++i )
    {
        TriggerVolume* vol = volDSGList[ i ];
        if ( mDisplayAll )
        {
            vol->Render();
            continue;
        }
        
        locator = vol->GetLocator();

        if ( mDisplayActive && locator->GetFlag( Locator::ACTIVE ) )
        {
            if ( mDisplayEnable[ locator->GetDataType() ] )
            {
                vol->Render();
            }
        }
        else if ( mDisplayInactive && !locator->GetFlag( Locator::ACTIVE ) )
        {
            if ( mDisplayEnable[ locator->GetDataType() ] )
            {
                vol->Render();
            }
        }
    }
*/
#endif
}


//=============================================================================
// TriggerVolumeTracker::GetTriggerByName
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      TriggerVolume
//
//=============================================================================
TriggerVolume* TriggerVolumeTracker::GetTriggerByName( const char* name )
{
    unsigned int i;
    for( i = 0; i < mTriggerCount; i++ )
    {
        if( strcmp( name, mTriggerVolumes[ i ]->GetName() ) == 0 )
        {
            return( mTriggerVolumes[ i ] );
        }
    }

    return( NULL );
}

//=============================================================================
// TriggerVolumeTracker::RegisterAI
//=============================================================================
// Description: Comment
//
// Parameters:  ( Vehicle* ai, int locatorTypes )
//
// Return:      void 
//
//=============================================================================
void TriggerVolumeTracker::RegisterAI( Vehicle* ai, int locatorTypes )
{
    rAssert( mNumRegisteredAI < MAX_AI );

#ifdef RAD_DEBUG
    unsigned int i;
    for ( i = 0; i < mNumRegisteredAI; ++i )
    {
        rAssert( mRegisteredVehicles[ i ].mVehicle != ai );
    }
#endif

    if ( mNumRegisteredAI < MAX_AI )
    {
        tRefCounted::Assign( mRegisteredVehicles[ mNumRegisteredAI ].mVehicle, ai );
        mRegisteredVehicles[ mNumRegisteredAI ].mTriggerTypes = locatorTypes;
    }

    mNumRegisteredAI++;
}

//=============================================================================
// TriggerVolumeTracker::UnregisterAI
//=============================================================================
// Description: Comment
//
// Parameters:  ( Vehicle* ai )
//
// Return:      void 
//
//=============================================================================
void TriggerVolumeTracker::UnregisterAI( Vehicle* ai )
{
    unsigned int i;
    for ( i = 0; i < mNumRegisteredAI; ++i )
    {
        if ( mRegisteredVehicles[ i ].mVehicle == ai )
        {
            mRegisteredVehicles[ i ].mVehicle->Release();
            mRegisteredVehicles[ i ].mVehicle = NULL;

            if ( mNumRegisteredAI > 1 )
            {
                mRegisteredVehicles[ i ] = mRegisteredVehicles[ mNumRegisteredAI - 1 ];
                mRegisteredVehicles[ mNumRegisteredAI - 1 ].mVehicle = NULL;
            }

            mNumRegisteredAI -= 1;
            break;
        }
    }
}

//=============================================================================
// TriggerVolumeTracker::GetAI
//=============================================================================
// Description: Comment
//
// Parameters:  ( int id )
//
// Return:      Vehicle
//
//=============================================================================
Vehicle* TriggerVolumeTracker::GetAI( int id )
{
    rAssert( id - MAX_PLAYERS < static_cast<int>(mNumRegisteredAI) );

    return mRegisteredVehicles[ id - MAX_PLAYERS ].mVehicle;
}

//=============================================================================
// TriggerVolumeTracker::RegisterLocatorTypeForAI
//=============================================================================
// Description: Comment
//
// Parameters:  ( LocatorType::Type type, Vehicle* ai )
//
// Return:      void 
//
//=============================================================================
void TriggerVolumeTracker::RegisterLocatorTypeForAI( LocatorType::Type type, Vehicle* ai )
{
    unsigned int i;
    for ( i = 0; i < mNumRegisteredAI; ++i )
    {
        if ( mRegisteredVehicles[ i ].mVehicle == ai )
        {
            mRegisteredVehicles[ i ].mTriggerTypes |= ( 1 << type );
            break;
        }
    }
}

//=============================================================================
// TriggerVolumeTracker::UnregisterLocatorTypeForAI
//=============================================================================
// Description: Comment
//
// Parameters:  ( LocatorType::Type type, Vehicle* ai )
//
// Return:      void 
//
//=============================================================================
void TriggerVolumeTracker::UnregisterLocatorTypeForAI( LocatorType::Type type, Vehicle* ai )
{
    unsigned int i;
    for ( i = 0; i < mNumRegisteredAI; ++i )
    {
        if ( mRegisteredVehicles[ i ].mVehicle == ai )
        {
            mRegisteredVehicles[ i ].mTriggerTypes &= ~( 1 << type );
            break;
        }
    }
}

//=============================================================================
// TriggerVolumeTracker::ResetDynaloadZones
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TriggerVolumeTracker::ResetDynaloadZones()
{
    for( int player = 0; player < GetGameplayManager()->GetNumPlayers(); player++ )
    {
        for( int index = 0; index < static_cast<int>( mActiveCount[ player ] ); index++ )
        {
            Locator* loc = mActiveVolumes[ player ][ index ]->GetLocator();
            if ( !loc )
            {
                continue;
            }

            if( loc->GetDataType() == LocatorType::DYNAMIC_ZONE )
            {
                RemoveActive( player, index );
                break;
            }
        }
    }
}
//========================================================================
// triggervolumetracker::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void TriggerVolumeTracker::ActivateNearestInteriorLoadZone(int iPlayerID, rmt::Vector& irPosnRef, float iMaxDistance)
{
    ZoneEventLocator* pNearestZEL = NULL;
    float nearestLocatorDistSqr = iMaxDistance*iMaxDistance;

    Locator* loc = NULL;
    rmt::Sphere sphere;
    
    int i;
    
    for ( i = 0; i < static_cast<int>( mTriggerCount ); ++i )
    {
        loc = mTriggerVolumes[i]->GetLocator();
        if(loc == NULL) continue;

        if( loc->GetDataType() == LocatorType::DYNAMIC_ZONE )
        {
            ZoneEventLocator* pZEL = (ZoneEventLocator*)(loc);
            if(pZEL->IsInteriorLoad())
            {
                mTriggerVolumes[i]->GetBoundingSphere(&sphere);

                rmt::Vector dist;
                dist.Sub(sphere.centre, irPosnRef);
                float curDistSqr = dist.MagnitudeSqr();

                if( (curDistSqr < nearestLocatorDistSqr)
                    && (curDistSqr < ((iMaxDistance + sphere.radius) * (iMaxDistance + sphere.radius))))
                {
                    pNearestZEL = pZEL;
                    nearestLocatorDistSqr = curDistSqr;
                }
            }
        }
    }

    if(pNearestZEL!=NULL)
    {
        pNearestZEL->Trigger(iPlayerID,true);
    }
 }


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// TriggerVolumeTracker::AddActive
//=============================================================================
// Description: Comment
//
// Parameters:  ( int playerID, TriggerVolume* triggerVolume )
//
// Return:      void 
//
//=============================================================================
void TriggerVolumeTracker::AddActive( int playerID, TriggerVolume* triggerVolume )
{
    rAssert( triggerVolume != NULL );

    if ( triggerVolume == NULL || !triggerVolume->TriggerAllowed( playerID ) )
        return;

    rAssert( mActiveCount[ playerID ] < MAX_ACTIVE );

    mActiveVolumes[ playerID ][ mActiveCount[ playerID ] ] = triggerVolume;
    mActiveCount[ playerID ]++;

    triggerVolume->AddRef();

    TriggerLocator* trigLoc = triggerVolume->GetLocator();
    if ( trigLoc && trigLoc->IsPlayerTracked( playerID ) == 0 )  //returns number tracked.
    {
        triggerVolume->Trigger( playerID, true );
    }

    triggerVolume->SetTrackingPlayer( playerID, true );    
}

//=============================================================================
// TriggerVolumeTracker::CheckForActiveVolumes
//=============================================================================
// Description: Comment
//
// Parameters:  ( int playerID, rmt::Vector& center, float radius )
//
// Return:      void 
//
//=============================================================================
void TriggerVolumeTracker::CheckForActiveVolumes( int playerID, rmt::Vector& center, float radius )
{
    unsigned int time = radTimeGetMilliseconds();
    unsigned int i;

//BEGIN_PROFILE( "CheckForActiveVolumes" );
//    IntersectManager* im = IntersectManager::GetInstance();
//    rAssert( im );
//
//    ReserveArray<TriggerVolume*> volDSGList;     
//
//BEGIN_PROFILE( "Query Intersect Manager" );
//    im->FindTrigVolElems( center, radius, volDSGList );
//END_PROFILE( "Query Intersect Manager" );

//BEGIN_PROFILE( "Car Triggers" );
    //Test all the triggers around the car
//    for ( i = 0; i < static_cast<unsigned int>(volDSGList.mUseSize); ++i )
//    {
//        TriggerVolume* vol = volDSGList[ i ];
//
//        TestVolume( vol, center, radius, playerID, time );
//    }
//END_PROFILE( "Car Triggers" );

    int localID = playerID;
    if ( playerID >= MAX_PLAYERS )
    {
        //localize the id
        VehicleCentral* vc = GetVehicleCentral();

        for ( i = 0; i < mNumRegisteredAI; ++i )
        {
            if ( mRegisteredVehicles[ i ].mVehicle == vc->GetVehicle( playerID - MAX_PLAYERS ) )
            {
                localID = i + MAX_PLAYERS;
                break;
            }
        }

        rAssert( localID != -1 );
    }
    else
    {
        //This is a player.  Store their points for testing.
        //Test the car like it was the "Green Lantern"  | 0 |.  Two lines and
        //a sphere.
        //Get the BBOX of the car

        Avatar* playerAvatar = GetAvatarManager()->GetAvatarForPlayer( playerID );
        Vehicle* vehicle = playerAvatar->GetVehicle();

        if ( vehicle )
        {
            /*
            rmt::Vector box = vehicle->GetExtents();
            rmt::Matrix carToWorld = vehicle->GetTransform();

            sP1.x = -box.x;
            sP1.y = -box.y;
            sP1.z = -box.z;
            sP1.Transform( carToWorld );

            sP2.x = +box.x;
            sP2.y = -box.y;
            sP2.z = -box.z;
            sP2.Transform( carToWorld );

            sP3.x = +box.x;
            sP3.y = +box.y;
            sP3.z = +box.z;
            sP3.Transform( carToWorld );

            sP4.x = -box.x;
            sP4.y = +box.y;
            sP4.z = +box.z;
            sP4.Transform( carToWorld );
            */

            sim::CollisionVolume* vecol = vehicle->GetSimState()->GetCollisionObject()->GetCollisionVolume()->GetSubCollisionVolume( 0 );
            sColType = vecol->Type();

            if( sColType == sim::OBBoxVolumeType )
            {
                sim::OBBoxVolume* obBox = static_cast<sim::OBBoxVolume*>( vecol );

                sP1.x = - obBox->mLength[0];
                sP1.y = - obBox->mLength[1];
                sP1.z = - obBox->mLength[2];

                sP2.x = obBox->mLength[0];
                sP2.y = - obBox->mLength[1];
                sP2.z = - obBox->mLength[2];

                sP3.x = obBox->mLength[0];
                sP3.y = obBox->mLength[1];
                sP3.z = obBox->mLength[2];

                sP4.x = - obBox->mLength[0];
                sP4.y = obBox->mLength[1];
                sP4.z = obBox->mLength[2];

                rmt::Matrix positionMatrix;
                positionMatrix.Identity();
                positionMatrix.Row(0) = obBox->mAxis[0];
                positionMatrix.Row(1) = obBox->mAxis[1];
                positionMatrix.Row(2) = obBox->mAxis[2];
                positionMatrix.Row(3) = obBox->mPosition;

                sP1.Transform( positionMatrix );
                sP2.Transform( positionMatrix );
                sP3.Transform( positionMatrix );
                sP4.Transform( positionMatrix );

                radius = rmt::Sqrt(obBox->mLength[0] + obBox->mLength[1]);
            }
        }
    }

//BEGIN_PROFILE( "Added Triggers" );
    //Test all the triggers added to the trigger tracker
    for ( i = 0; i < mTriggerCount; ++i )
    {
        rmt::Sphere sphere;
        mTriggerVolumes[ i ]->GetBoundingSphere(&sphere);

        rmt::Vector dist;
        dist.Sub(sphere.centre, center);

        if(dist.MagnitudeSqr() < ((radius + sphere.radius) * (radius + sphere.radius)))
        {
            TestVolume( mTriggerVolumes[ i ], center, radius, localID, time );
        }
    }
//END_PROFILE( "Added Triggers" );

    if ( playerID < MAX_PLAYERS )
    {
        //BEGIN_PROFILE( "Active Triggers" );
        //Test for triggers that we've left.
        for( i = 0; i < mActiveCount[ playerID ]; ++i )
        {
            // Added a test for the active trigger volume, because it would
            // seem that if a locator were not active, you wouldn't want
            // the TV in the active list.
            //
            // TBJ [8/23/2002] 
            //
            if( (mActiveVolumes[ playerID ][ i ]->GetLocator() && !mActiveVolumes[ playerID ][ i ]->GetLocator()->GetFlag( Locator::ACTIVE )) || 
                mActiveVolumes[ playerID ][ i ]->GetFrameUsed() != time )
            {
                RemoveActive( playerID, i );
            }
        }
        //END_PROFILE( "Active Triggers" );
    }
    else
    {
        localID -= MAX_PLAYERS;

        for ( i = 0; i < mAICount[ localID ]; ++i )
        {
            if ( !mActiveAIVolumes[ localID ][ i ]->GetLocator()->GetFlag( Locator::ACTIVE ) ||
                 mActiveAIVolumes[ localID ][ i ]->GetFrameUsed() != time )
            {
                RemoveAIActive( localID, i );
            }
        }
    }
    
//END_PROFILE( "CheckForActiveVolumes" );
}


//=============================================================================
// TriggerVolumeTracker::RemoveActive
//=============================================================================
// Description: Comment
//
// Parameters:  ( int playerID, int index )
//
// Return:      void 
//
//=============================================================================
void TriggerVolumeTracker::RemoveActive( int playerID, int index )
{
    TriggerVolume* vol = mActiveVolumes[ playerID ][ index ];

    TriggerLocator* trigLoc = vol->GetLocator();
    if ( trigLoc && trigLoc->IsPlayerTracked( playerID ) <= 1 ) //Some locators are screwy so they never track trigger voluemes.  Gags, mostly
    {
        vol->Trigger( playerID, false );
    }

    vol->SetTrackingPlayer( playerID, false );
    vol->Release();

    if ( mActiveCount[ playerID ] > 0 )
    {
        mActiveVolumes[ playerID ][ index ] = mActiveVolumes[ playerID ][ mActiveCount[ playerID ] - 1 ];
        mActiveVolumes[ playerID ][ mActiveCount[ playerID ] - 1 ] = NULL;
    }

    mActiveCount[ playerID ]--;
}

//=============================================================================
// TriggerVolumeTracker::AddAIActive
//=============================================================================
// Description: Comment
//
// Parameters:  ( int playerID, TriggerVolume* triggerVolume )
//
// Return:      void 
//
//=============================================================================
void TriggerVolumeTracker::AddAIActive( int playerID, TriggerVolume* triggerVolume )
{
    rAssert( mAICount[ playerID ] < MAX_AI_VOLUMES );

    if ( !triggerVolume->TriggerAllowed( playerID ) )
    { 
        return;
    }

    mActiveAIVolumes[ playerID ][ mAICount[ playerID ] ] = triggerVolume;
    triggerVolume->AddRef();

    TriggerLocator* trigLoc = triggerVolume->GetLocator();
    if ( trigLoc && trigLoc->IsPlayerTracked( playerID + MAX_PLAYERS ) == 0 )
    {
        triggerVolume->Trigger( playerID + MAX_PLAYERS, true );  //And the hacks go on
    }

    triggerVolume->SetTrackingPlayer( playerID + MAX_PLAYERS, true );    

    mAICount[ playerID ] += 1;
}

//=============================================================================
// TriggerVolumeTracker::RemoveAIActive
//=============================================================================
// Description: Comment
//
// Parameters:  ( int playerID, int index )
//
// Return:      void 
//
//=============================================================================
void TriggerVolumeTracker::RemoveAIActive( int playerID, int index )
{
    TriggerVolume* vol = mActiveAIVolumes[ playerID ][ index ];

    TriggerLocator* trigLoc = vol->GetLocator();
    if ( trigLoc && trigLoc->IsPlayerTracked( playerID + MAX_PLAYERS ) == 1 )
    {
        vol->Trigger( playerID + MAX_PLAYERS, false );
    }

    vol->SetTrackingPlayer( playerID + MAX_PLAYERS, false );  //This is a hack.
    vol->Release();

    if ( mAICount[ playerID ] > 0 )
    {
        mActiveAIVolumes[ playerID ][ index ] = mActiveAIVolumes[ playerID ][ mAICount[ playerID ] - 1 ];
        mActiveAIVolumes[ playerID ][ mAICount[ playerID ] - 1 ] = NULL;
    }

    mAICount[ playerID ] -= 1;
}

//=============================================================================
// TriggerVolumeTracker::TestVolume
//=============================================================================
// Description: Comment
//
// Parameters:  ( TriggerVolume* vol, rmt::Vector& center, float radius, int playerID, unsigned int time )
//
// Return:      void 
//
//=============================================================================
void TriggerVolumeTracker::TestVolume( TriggerVolume* vol, 
                                              rmt::Vector& center, 
                                              float radius, 
                                              int playerID, 
                                              unsigned int time )
{
    rAssert( vol );

    // This is added because the AmbientDialogueTriggers do not have a locator associated with them
    // with them.
    if ( vol->GetLocator() != NULL )
    {
        if ( playerID >= MAX_PLAYERS &&
            ( ((1 << vol->GetLocator()->GetDataType()) & mRegisteredVehicles[ playerID - MAX_PLAYERS ].mTriggerTypes) == 0 ) )
        {
            return;
        }
        else if ( playerID >= MAX_PLAYERS )
        {
            //Test if this is a legal event.
            if ( vol->GetLocator()->GetDataType() == LocatorType::EVENT )
            {
                EventLocator* evtLoc = static_cast<EventLocator*>(vol->GetLocator());
                unsigned int i;
                for ( i = 0; i < NUM_ALLOWED_AI_EVENTS; ++i )
                {
                    if ( evtLoc->GetEventType() == ALLOWED_AI_EVENTS[ i ] )
                    {
                        break;
                    }
                }

                if ( i == NUM_ALLOWED_AI_EVENTS )
                {
                    //Not allowed to trigger this.
                    return;
                }
            }
        }
    }

    bool collision = false;

    Locator* loc = vol->GetLocator();

    if ( playerID < MAX_PLAYERS &&
         GetAvatarManager()->GetAvatarForPlayer( playerID )->IsInCar() &&
         ( loc && loc->GetDataType() != LocatorType::OCCLUSION ) )
    {
        if ( GetGameplayManager()->GetGameType() == GameplayManager::GT_SUPERSPRINT )
        {
            collision = (!loc || loc->GetFlag( Locator::ACTIVE )) && vol->IntersectsSphere( center, 1.5f );
        }
        else
        {
            collision = (!loc || loc->GetFlag( Locator::ACTIVE )) && 
                (vol->IntersectsSphere( center, 1.5f ) || vol->IntersectsBox( sP1, sP2, sP4, sP3 ));
        }
    }
    else
    {
        collision = (!loc || loc->GetFlag( Locator::ACTIVE )) && 
                    vol->Contains( center, radius );
    }

//BEGIN_PROFILE( "Test Volume" );
    if ( collision )
    {
        //Test to see if this is a DEATH volume.
        if ( loc->GetDataType() == LocatorType::EVENT )
        {
            EventLocator* evtLoc = static_cast<EventLocator*>(loc);
            if ( evtLoc->GetEventType() == LocatorEvent::DEATH )
            {
                //This is so that death volumes always say you're in them until you leave.
                //Hack for Chuka.
                vol->Trigger( playerID, true );
                return;  //Early death.
            }
        }

//BEGIN_PROFILE( "Is Player Tracking" );
        if ( !vol->IsPlayerTracking( playerID ) )
        {
            //New trigger!
#ifdef RAD_DEBUG
            char name[256];
            sprintf(name, "Hit trigger: %s\n", vol->GetName() );
            rDebugString( name );
#endif

            if ( playerID < MAX_PLAYERS )
            {
                //BEGIN_PROFILE( "AddActive" );
                AddActive( playerID, vol );
                //END_PROFILE( "AddActive" );
            }
            else
            {
                AddAIActive( playerID - MAX_PLAYERS, vol );
            }
        }

        vol->SetFrameUsed( time, (int)this );
    }
//END_PROFILE( "Test Volume" );
}
