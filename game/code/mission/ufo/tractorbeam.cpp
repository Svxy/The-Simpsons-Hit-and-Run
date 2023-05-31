//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   Name of subsystem or component this class belongs to.
//
// Description: This file contains the implementation of...
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <mission/ufo/tractorbeam.h>
#include <render/DSG/StatePropDSG.h>
#include <render/RenderManager/RenderManager.h>
#include <render/RenderManager/WorldRenderLayer.h>
#include <render/DSG/DynaPhysDSG.h>
#include <worldsim/traffic/trafficmanager.h>
#include <worldsim/redbrick/vehicle.h>
#include <events/eventmanager.h>
#include <p3d/context.hpp>
#include <p3d/matrixstack.hpp>

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

const float BEAM_SPEED = 2.50f;
const float MAX_TRACTORBEAM_RANGE_SQR = 3.0f * 3.0f;
const int MAX_CAUGHT_OBJECTS = 50;
const float SWALLOW_RADIUS_SQR = 1.0f * 1.0f;
const float MAX_BEAM_ANGLE = 0.707f;

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================



//===========================================================================
// Member Functions
//===========================================================================



TractorBeam::TractorBeam():
mpDrawable( NULL ),
mNextTarget( NULL )
{
    mCaughtObjects.Allocate( MAX_CAUGHT_OBJECTS );
    // Set the beam to point straight down
    mTransform.Identity();
}

TractorBeam::~TractorBeam()
{
    if ( mpDrawable != NULL )
    {
        mpDrawable->Release();
        mpDrawable = NULL;
    }
}

bool 
TractorBeam::LoadSetup( const char* statePropDSGName )
{
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

    return success;    
}


void
TractorBeam::Update( float timeInMS )
{
    if ( mpDrawable != NULL )
    {
        mpDrawable->Update( timeInMS );
        // Move the beam

        // SLERP between the beam and the target   
        rmt::Matrix targetOrientation;
        {
            rmt::Vector up;
            if ( mNextTarget != NULL )
            {
  
                rmt::Vector targetPos;
                mNextTarget->GetPosition( &targetPos );
                rmt::Vector toTarget = targetPos - mTransform.Row(3);
                toTarget.Normalize();

                if ( toTarget.Dot( rmt::Vector( 0, -1.0f ,0 )) > MAX_BEAM_ANGLE )
                {
                    up = -toTarget;                
                }
                else
                {
                    up = rmt::Vector(0, 1.0f, 0);
                }               
            }
            else
            {
                up = rmt::Vector( 0, 1.0f, 0 );
            }
            rmt::Vector forward;
            forward.CrossProduct( up, rmt::Vector(1,0,0) );
            rmt::Vector right;
            right.CrossProduct( up, forward );
    
            targetOrientation.Row(0) = right;
            targetOrientation.Row(1) = up;
            targetOrientation.Row(2) = forward;
        
            const float BEAM_ANGULAR_SPEED = 0.1f;
            rmt::Quaternion orientation = Slerp( targetOrientation, mTransform, timeInMS ,BEAM_ANGULAR_SPEED);
            mTransform.FillRotation( orientation );

            mpDrawable->SetTransform( mTransform );
        }

        // Go through the list and remove the caught objects
        for ( int i = 0 ; i < mCaughtObjects.mUseSize ; i++)
        {
            
            // Fetch object position
            rmt::Vector objPosition;
            mCaughtObjects[i]->GetPosition( &objPosition );
           

            // Check to see if it is within range of the swallow emitter
            float distanceToBeamEmitterSqr = (mTransform.Row(3) - objPosition).MagnitudeSqr();
            if ( distanceToBeamEmitterSqr < SWALLOW_RADIUS_SQR )
            {
                SwallowObject( mCaughtObjects[i] );
                mCaughtObjects[i]->Release();
                mCaughtObjects.Remove( i );

                // Remove the object from the list
            }            
            else
            {
                // Move the object upwards
                //rmt::Vector* pPosition = mCaughtObjects[i]->pPosition();
                //pPosition->y -= timeInMS * BEAM_SPEED;
                sim::SimState* pSimState = mCaughtObjects[ i ]->GetSimState();
                //rmt::Matrix& rTransform = pSimState->GetTransform();
                rmt::Vector& rVelocity = pSimState->GetLinearVelocity();
                //Set the velocity to be constant toward the tractorbeam center
                           
                rmt::Vector toTractorBeam = (mTransform.Row(3) - objPosition);
                toTractorBeam.Normalize();
                toTractorBeam *= BEAM_SPEED;
                rVelocity = toTractorBeam;
            }
        }
    }
    else
    {
        LoadSetup( mStatePropName.GetText() );
    }   
}

bool 
TractorBeam::IsValidTarget( DynaPhysDSG* pDSG )const
{
    bool valid;
    int i;
    for ( i = 0 ; i < mCaughtObjects.mUseSize ; i++)
    {      
        if ( pDSG == mCaughtObjects.mpData[i] )
        {
            break;
        }
    }
    if ( i == mCaughtObjects.mUseSize )
    {
        valid = true;    
    }
    else
    {
        valid = false;    
    }
    return valid;
}

bool
TractorBeam::CanFire( DynaPhysDSG* pDSG )const
{
    bool canFireAtTarget;
    // If the beam can't hold any more objects, return false
    if ( mCaughtObjects.mUseSize < mCaughtObjects.mSize )
    {
        // If within max range, and not already caught, return true      
        rmt::Vector targetPos;
        pDSG->GetPosition( &targetPos );
        rmt::Vector targetVector = targetPos - mTransform.Row(3);
        targetVector.Normalize();


        if ( targetVector.Dot( rmt::Vector( 0, -1.0f, 0 ) ) > MAX_BEAM_ANGLE )
        {
            if ( IsValidTarget( pDSG ) )
            {
                canFireAtTarget = true;
            }
            else
            {
                canFireAtTarget = false;
            }   
        }
        else 
            canFireAtTarget = false;

    }
    else
    {
        canFireAtTarget = false;
    }

    return canFireAtTarget;
}

void
TractorBeam::Fire( DynaPhysDSG* pDSG )
{
    pDSG->AddToSimulation();
    pDSG->AddRef();
    // Add the object to the mCaughtObjects list

    mCaughtObjects.Add( pDSG );
    mNextTarget = pDSG;
}

void 
TractorBeam::Display()
{
    rAssert( mpDrawable != NULL );


    p3d::stack->PushMultiply( mTransform );
    mpDrawable->Display();
    p3d::stack->Pop();
}

void
TractorBeam::SetPosition( const rmt::Vector& position )
{
    mTransform.Row(3) = position;
    if ( mpDrawable != NULL )
    {
        rmt::Box3D oldBoundingBox;
        mpDrawable->GetBoundingBox( &oldBoundingBox );
        mpDrawable->SetPosition( position );

        WorldRenderLayer* pLayer = static_cast< WorldRenderLayer* >( GetRenderManager()->mpLayer( RenderEnums::LevelSlot ));
        pLayer->pWorldScene()->Move( oldBoundingBox, mpDrawable );
    }
}
        
void 
TractorBeam::SwallowObject( DynaPhysDSG* pDSG )
{
    rAssert( pDSG != NULL );
    // switch on AIref
    // peds have to be removed
    int aiRef = pDSG->GetAIRef();
    switch( aiRef )
    {
    case PhysicsAIRef::PlayerCharacter:
        break;
    case PhysicsAIRef::NPCharacter:
        break;
    case PhysicsAIRef::redBrickVehicle:
        {
            // Remove the object via the traffic manager
            rAssert( dynamic_cast< Vehicle* > ( pDSG ) );
            Vehicle* pVehicle = static_cast< Vehicle* >( pDSG );
            if ( pVehicle->mVehicleType == VT_TRAFFIC )
            {
                TrafficManager::GetInstance()->RemoveTraffic( static_cast< Vehicle* > ( pDSG ) );            
            }        
        }
        GetEventManager()->TriggerEvent( EVENT_VEHICLE_DESTROYED, NULL );        
        break;
    default:
        WorldRenderLayer* wrl = static_cast< WorldRenderLayer* >( GetRenderManager()->mpLayer( RenderEnums::LevelSlot ) );
        rAssert( dynamic_cast< WorldRenderLayer* > ( wrl ) );
        wrl->RemoveGuts( pDSG );
        GetEventManager()->TriggerEvent( EVENT_OBJECT_DESTROYED, NULL );
        break;
    };
}

bool
TractorBeam::IsWithinBeam( const rmt::Vector& targetVector )const
{
    rmt::Vector beamDir = -mTransform.Row(1);
    float dot = targetVector.Dot( beamDir );
    // cosine of the angle ( cosine (45 deg ) )
    bool withinRange;
    if ( dot > MAX_BEAM_ANGLE  )
    {
        withinRange = true;
    }
    else 
    {
        withinRange = false;
    }
    return withinRange;
}

rmt::Quaternion
TractorBeam::Slerp( const rmt::Matrix m1, const rmt::Matrix m2, float deltaTime, float angularVelocity )
{
    rmt::Quaternion q1,q2;
    q1.BuildFromMatrix( m1 );
    q2.BuildFromMatrix( m2 );

    rmt::Quaternion qresult;

    float t = 0.5f;
    
    qresult.Slerp( q1, q2, t );

    return qresult;
}


