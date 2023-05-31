//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   Name of subsystem or component this class belongs to.
//
// Description: This file contains the implementation of...
//
// Authors:     Name Here
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <mission/ufo/ufo.h>
#include <render/DSG/StatePropDSG.h>
#include <mission/ufo/weapon.h>
#include <console/console.h>
#include <render/RenderManager/RenderManager.h>
#include <render/RenderManager/WorldRenderLayer.h>
#include <render/IntersectManager/IntersectManager.h>
#include <render/DSG/DynaPhysDSG.h>
#include <memory/map.h>
#include <console/console.h>
#include <events/eventmanager.h>

//#include <iostream>
//#include <stl.h>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

const float SCAN_RADIUS = 200.0f;
const float UFO_SPEED = 0.005f;

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Member Functions
//===========================================================================

//===========================================================================
// XxxClassName::MemberFunction2
//===========================================================================
// Description:
//      Generally, describe what behaviour this service possesses on
//      which clients can depend, i.e. the actions that this service
//      guarantees to clients.
//
// Constraints:
//      Describe here what you require of the client which calls this
//      service, the behaviour of a client upon which this service
//      relies - essentially, the converse of description above.
//      Also, a constraint is an expression of some semantic
//      condition that must be preserved, an invariant.  For example,
//      this service may not be re-entrant because it sets some
//      static variable on which it will depend the next time it is
//      called. Not required if there are no constaints.
//
// Parameters:
//      arg1 - brief description of the parameter
//      arg2 - brief description of the parameter
//      argName - brief description of the parameter
//
// Return:
//      Describe the return value here.  Not required if void return
//      type.
//
//===========================================================================
/*
// Abstract base state class ctor and dtor
UFOState::UFOState()
{

}
UFOState::~UFOState()
{

}
// Moving state, the state has a target and moves unerringly toward it
UFOMovingState::UFOMovingState()
{

}

UFOMovingState::~UFOMovingState()
{

}


*/

UFO::UFO():
mpDrawable( NULL ),
mVelocity( 0, 0, 0 ),
mPosition( 25, 5, 307 )
{
    // This thing is gonna be a hovering arsenal
    mWeaponList.reserve( 10 );
    mTargets.Allocate( 200 );

    GetEventManager()->AddListener( this, EVENT_OBJECT_DESTROYED );
    GetEventManager()->AddListener( this, EVENT_VEHICLE_DESTROYED );
}

UFO::~UFO()
{

    if ( mpDrawable != NULL )
    {
        // Remove it from the DSG
        WorldRenderLayer* pLayer = static_cast< WorldRenderLayer* >( GetRenderManager()->mpLayer( RenderEnums::LevelSlot ));
        pLayer->pWorldScene()->Remove( mpDrawable );


        mpDrawable->Release();
        mpDrawable = NULL;
    }
    // Remove weapons
    for (unsigned int i = 0 ; i < mWeaponList.size() ; i++)
    {
        mWeaponList[ i ].weapon->Release();
    }

}

bool 
UFO::LoadSetup( const char* statePropDSGName, const rmt::Vector& position )
{
    mPosition = position;
    mStatePropName = statePropDSGName;

    rAssert( mpDrawable == NULL );
    mpDrawable = p3d::find< StatePropDSG >( statePropDSGName );
    bool success;
    if ( mpDrawable != NULL )
    {
        mpDrawable->AddRef();
        // Insert it into the level layer in the dsg
        WorldRenderLayer* pLayer = static_cast< WorldRenderLayer* >( GetRenderManager()->mpLayer( RenderEnums::LevelSlot ));
        pLayer->pWorldScene()->Add( mpDrawable );
        success = true;



        // Place the object in the mission section
        p3d::inventory->PushSection();
        p3d::inventory->SelectSection( "Mission" );
        p3d::inventory->Store( this );                                        
        p3d::inventory->PopSection();
    }
    else
    {
        success = false;
    }
    // Need a mechanism for attaching weapons to the surface of the UFO
    // Register a console function handler

    return success;
}


void 
UFO::AddWeapon( Weapon* pWeapon, const rmt::Vector& offset )
{
    UFOWeapon ufoWeapon;
    ufoWeapon.weapon = pWeapon;
    pWeapon->SetPosition( mPosition + offset );
    ufoWeapon.offset = offset;
    mWeaponList.push_back( ufoWeapon );
}


void
UFO::Update( float timeInMS )
{

    if ( mpDrawable != NULL )
    {
      //  if ( mTargets.mUseSize == 0 )
        {
            ScanForTargets();
        }

        Navigate( timeInMS );
        Move( timeInMS );

        if ( mpDrawable )
        {
            // Iterate through all the weapons
            for ( unsigned int i = 0 ; i < mWeaponList.size() ; i++)
            {
                mWeaponList[ i ].weapon->Update( timeInMS );
                for ( int j = 0 ; j < mTargets.mUseSize; j++)
                {
                    if ( mWeaponList[ i ].weapon->CanFire( mTargets[j] ) )
                    {
                        mWeaponList[ i ].weapon->Fire( mTargets[j] );
                    }
                }
            }
            // Update the state prop animations
            mpDrawable->Update( timeInMS );
        }
    }
    else
    {   
        LoadSetup( mStatePropName.GetText(), mPosition );
    }
}


// Figure out the correct acceleration to get to where we want to go
// Apply the acceleration to the velocity
void 
UFO::Navigate( float timeInMS )
{
    // Calculate the acceleration required to get to the target
    // Lets have simple little movement formula
  
    if ( mTarget && 0  )
    {
        rmt::Vector toTarget;
        rmt::Vector targetPos;
        mTarget->GetPosition( &targetPos );
        toTarget = targetPos - mPosition;
        toTarget.Normalize();
        toTarget.y = 0;
        mVelocity = toTarget * UFO_SPEED;
    }
    else
    {
        mVelocity = rmt::Vector(0,0,0);
    }

}
// Apply velocity and move the object, also take care of the DSG move call
void 
UFO::Move( float timeInMS )
{
    mPosition += mVelocity * timeInMS;

    rmt::Box3D oldBoundingBox;
    mpDrawable->GetBoundingBox( &oldBoundingBox );

    mpDrawable->SetPosition( mPosition );

    // Insert it into the level layer in the dsg
    WorldRenderLayer* pLayer = static_cast< WorldRenderLayer* >( GetRenderManager()->mpLayer( RenderEnums::LevelSlot ));
    pLayer->pWorldScene()->Move( oldBoundingBox, mpDrawable );

    // Move the weapons
    for ( unsigned int i = 0 ; i < mWeaponList.size() ; i++ )
    {
        rmt::Vector position = mPosition + mWeaponList[ i ].offset;
        mWeaponList[ i ].weapon->SetPosition( position );
    }
}


void
UFO::ScanForTargets()
{
    rmt::Vector position;
    mpDrawable->GetPosition( &position );

    if ( mTargets.mUseSize > 0 )
        mTargets.ClearUse();

    GetIntersectManager()->FindDynaPhysElems(  position, 
                                               SCAN_RADIUS,
                                               mTargets );
    
    // Find the closest target
    // Sort targets by distance
    // Calc ranges and store into a multimap
    Map< float, int > range;
    
    for (int i = 0;i < mTargets.mUseSize ; i++)
    {
        // Check the weapons and see if any wants to fire at it
        bool validTarget = true;
        for ( unsigned int j = 0 ; j < mWeaponList.size() ; j++)
        {
            if ( mWeaponList[ j ].weapon->IsValidTarget( mTargets[i] ) == false )
            {
                validTarget = false;
                break;
            }
        }
        if ( validTarget )
        {
            rmt::Vector targetPos;
            mTargets[i]->GetPosition( &targetPos );
            float distanceSqr = ( targetPos - mPosition ).MagnitudeSqr();
            range.insert( distanceSqr, i );
        }
    }
    if ( range.size() > 0 )
    {
        int targetIndex = range.begin()->second;

        mTarget = mTargets[ targetIndex ];
        //std::cout << "Found " << mTargets.mUseSize << " targets." << std::endl;
    }
    else
    {
        mTarget = NULL;
    }
}

UFO::State 
UFO::ChooseNextState()
{
    return UFO::eID4Attack;
}

void 
UFO::HandleEvent( EventEnum id, void* pEventData )
{
    switch ( id )
    {   
        // An object was killed, trigger a rescan by clearing the target list
    case EVENT_OBJECT_DESTROYED: 
    case EVENT_VEHICLE_DESTROYED:
        mTargets.ClearUse();
        break;
    default:
        break;
    }
}


