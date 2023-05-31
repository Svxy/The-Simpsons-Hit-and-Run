//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        DynaPhysDSG.cpp
//
// Description: Implementation for DynaPhysDSG class.
//
// History:     Implemented	                         --Devin [6/17/2002]
//========================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <render/DSG/DynaPhysDSG.h>

#include <worldsim/character/character.h>
#include <worldsim/worldphysicsmanager.h>
#include <mission/gameplaymanager.h>

#include <simcommon/simulatedobject.hpp>

//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************

// tuning values for rest testing
static const float REST_LINEAR_TOL = 0.03f; // linear velocity tolerance
static const float REST_ANGULAR_TOL = 0.2f; // angular velocity tolerance
static const int   REST_HISTORY = 15;       // number of sim frames to average velocities

//************************************************************************
//
// Public Member Functions : DynaPhysDSG Interface
//
//************************************************************************
//========================================================================
// DynaPhysDSG::
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
DynaPhysDSG::DynaPhysDSG() :
    mPastLinear(REST_LINEAR_TOL * 2.0f, 15), 
    mPastAngular(REST_ANGULAR_TOL  * 2.0f, 15)
{   
    mGroundPlaneIndex = -1; 
    mGroundPlaneRefs = 0;
    
    mIsHit = false;
}
//========================================================================
// DynaPhysDSG::
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
DynaPhysDSG::~DynaPhysDSG()
{
}



//=============================================================================
// DynaPhysDSG::Update
//=============================================================================
// Description: Comment
//
// Parameters:  (float dt)
//
// Return:      void 
//
//=============================================================================
void DynaPhysDSG::Update(float dt)
{

}


//=============================================================================
// DynaPhysDSG::FetchGroundPlane
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
int DynaPhysDSG::FetchGroundPlane()
{
    //mGroundPlaneRefs++;   moved down to a point where we know we got one

    if(mGroundPlaneIndex != -1)
    {
        // we already got one       
        mGroundPlaneRefs++;
        return mGroundPlaneIndex;
    }

    rAssert(this->GetSimState());
    mGroundPlaneIndex = GetWorldPhysicsManager()->GetNewGroundPlane(this->GetSimState());
    rAssert(mGroundPlaneIndex > -1);
    if(mGroundPlaneIndex != -1)
    {
        mGroundPlaneRefs++;
    }    

    return mGroundPlaneIndex;

}

//=============================================================================
// DynaPhysDSG::FreeGroundPlane
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void DynaPhysDSG::FreeGroundPlane()
{
    mGroundPlaneRefs--;
    if(mGroundPlaneRefs == 0)
    {
        GetWorldPhysicsManager()->DisableGroundPlaneCollision(mGroundPlaneIndex);   // also disable when thing is at rest?        
        GetWorldPhysicsManager()->FreeGroundPlane(mGroundPlaneIndex);
        mGroundPlaneIndex = -1;
    }


    // this shoudln't matter, because the ground plane shouldn't be 
    // removed unless it is already under AI control, but better safe than sorry 
    mpSimStateObj->SetControl(sim::simAICtrl);
    mpSimStateObj->ResetVelocities();

    // remove from collision
}

//=============================================================================
// DynaPhysDSG::IsAtRest
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool DynaPhysDSG::IsAtRest()
{
    // object under AI control are always "at rest"
    if(!GetSimState() || GetSimState()->GetControl() == sim::simAICtrl)
    {
        return true;
    }

    return false;
}

void DynaPhysDSG::RestTest()
{
    // already at rest
    sim::SimState* simState = GetSimState();

    if(!simState || simState->GetControl() == sim::simAICtrl)
    {
        return;
    }

    // don't update characters (which should always be in sim), vehicles (they have their own rest test)
    // or breakables (they are always at rest, but removing them from physics screws them up)
    if((simState->mAIRefIndex != PhysicsAIRef::redBrickVehicle) &&
       (simState->mAIRefIndex != PhysicsAIRef::PlayerCharacter ) &&
       (simState->mAIRefIndex != PhysicsAIRef::NPCharacter ) &&
       (!mpCollisionAttributes || (mpCollisionAttributes->GetClasstypeid() != PROP_BREAKABLE)))
    {
        // check if smoothed velocities are below tolerance
        if((mPastLinear.Smooth(simState->GetLinearVelocity().Magnitude()) < REST_LINEAR_TOL) &&
           (mPastAngular.Smooth(simState->GetAngularVelocity().Magnitude()) < REST_ANGULAR_TOL))
        {
            // put it at rest
            simState->SetControl(sim::simAICtrl);
            simState->ResetVelocities();
            OnTransitToAICtrl();
        }
    }
    else if( simState->mAIRefIndex == PhysicsAIRef::NPCharacter ||
             simState->mAIRefIndex == PhysicsAIRef::PlayerCharacter ) 
    {
        static const float PC_REST_LINEAR_TOL = 1.0f;
        static const float PC_REST_ANGULAR_TOL = 1.5f;
        static const float NPC_REST_LINEAR_TOL = 0.5f;
        static const float NPC_REST_ANGULAR_TOL = 0.7f;

        float linearTol = 0.0f;
        float angularTol = 0.0f;

        if( simState->mAIRefIndex == PhysicsAIRef::PlayerCharacter )
        {
            linearTol = PC_REST_LINEAR_TOL;
            angularTol = PC_REST_ANGULAR_TOL;
        }
        else
        {
            linearTol = NPC_REST_LINEAR_TOL;
            angularTol = NPC_REST_ANGULAR_TOL;
        }

        // check if smoothed velocities are below tolerance
        float linVel = simState->GetLinearVelocity().Magnitude();
        float angVel = simState->GetAngularVelocity().Magnitude();
        float testLin = mPastLinear.Smooth( linVel );
        float testAng = mPastAngular.Smooth( angVel );

        if( testLin < linearTol && testAng < angularTol )
        {
            bool okToRest = true;

            Character* character = (Character*) simState->mAIRefPointer;

            // 
            // Only test for aborting rest test for NPCs... We want to
            // restore player control over his character ASAP, so no delays there.
            //
            if( character->IsNPC() )
            {
                rmt::Vector myPos;
                character->GetPosition( &myPos );


                ////////////////////////////////////////////////////////////////
                // NOTE: Do this if we encounter probs with characters coming to rest
                //       while in the air. It can potentially cause problems if you got
                //       teleported and your ground pos is out of synch with where you've
                //       been teleported to.
                // 
                rmt::Vector myGroundPos, myGroundNormal;
                character->GetTerrainIntersect( myGroundPos, myGroundNormal );

                const float DELTA_ABOVE_GROUND_OR_STATIC_KEEP_IN_SIM = 1.0f;
                if( myPos.y > (myGroundPos.y + DELTA_ABOVE_GROUND_OR_STATIC_KEEP_IN_SIM) )
                {
                    okToRest = false;
                }
                ////////////////////////////////////////////////////////////////


                if( okToRest )
                {
                    // don't transit back out of sim if still colliding with a vehicle
                    if( character->mbCollidedWithVehicle )
                    {
                        okToRest = false;
                    }
                    else
                    {
                        Vehicle* playerVehicle = GetGameplayManager()->GetCurrentVehicle();
                        if( playerVehicle )
                        {
                            // do a quick test if we're inside the vehicle's volume
                            // if so, don't put at rest.
                            rmt::Vector box = playerVehicle->GetExtents();
                            rmt::Matrix worldToCar = playerVehicle->GetTransform();
                            worldToCar.Invert();

                            // put the character position in car space
                            // so now the box is "axis"-aligned and we
                            // can do simple containment test
                            myPos.Transform( worldToCar );

                            // ignore y... if we're anywhere in car's x or z space
                            // then we don't allow putting at rest.
                            const float FUDGE_EXTENT = 0.2f;
                            float xExtent = box.x + FUDGE_EXTENT;
                            float zExtent = box.z + FUDGE_EXTENT;

                            if( -xExtent < myPos.x && myPos.x < xExtent &&
                                -zExtent < myPos.z && myPos.z < zExtent )
                            {
                                okToRest = false;
                            }
                        }
                    }
                }
            }

            if( okToRest )
            {
                // put it at rest
                simState->SetControl(sim::simAICtrl);
                simState->ResetVelocities();
                OnTransitToAICtrl();
            }
            else
            {
                // Keep turning 
                rmt::Vector linearAdd( linearTol, 0.0f, linearTol );
                rmt::Vector angularAdd( angularTol, angularTol, angularTol );

                rmt::Vector& linearVel = simState->GetLinearVelocity();
                linearVel.Add( linearAdd );

                rmt::Vector& angularVel = simState->GetAngularVelocity();
                angularVel.Add( angularAdd );

            }
        }
    }
}

void
DynaPhysDSG::AddToSimulation()
{
    sim::SimState* pSimState = GetSimState();
    if ( pSimState )
    {
        if ( pSimState->GetSimulatedObject() != NULL )
        {
            pSimState->SetControl( sim::simSimulationCtrl );
            //int groundPlaneIndex = FetchGroundPlane();
            if ( this->mGroundPlaneIndex >= 0 )
            {
                GetWorldPhysicsManager()->EnableGroundPlaneCollision( mGroundPlaneIndex );
            }
        }
    }
}


void 
DynaPhysDSG::ApplyForce( const rmt::Vector& direction, float force )
{
    if ( mpSimStateObj == NULL )
    {
        return;
    }

    const float DEFAULT_MASS = 100.0f;

    float mass;
    if ( mpCollisionAttributes != NULL )
    {
        mass = mpCollisionAttributes->GetMass();
    }
    else
    {
        mass = DEFAULT_MASS;
    }

    if ( rmt::Epsilon( mass, 0.0f ) )
    {
#ifdef RAD_DEBUG
        char error[128];
        sprintf( error, "DynaPhysDsg object: %s has 0 mass\n", GetName() );
        rAssertMsg( false, error );
#endif
        mass = DEFAULT_MASS;
    }

    if(mpSimStateObj->GetControl() == sim::simAICtrl)
    {
        mpSimStateObj->ResetVelocities();
    }

    if(mpSimStateObj->GetSimulatedObject())
    {
        mpSimStateObj->GetSimulatedObject()->ResetRestingDetector();

        rmt::Vector& rVelocity = mpSimStateObj->GetLinearVelocity();
        // Apply delta velocity
        float deltaV = force / mass;
        rVelocity += (direction * deltaV);
        // Make it interact with the world
        AddToSimulation();
    }
}

bool 
DynaPhysDSG::IsCollisionEnabled()const
{
    sim::SimState* pSimState = GetSimState();
    if ( pSimState == NULL )
        return false; // no simstate, no collision possible

    sim::CollisionObject* pCollisionObject = pSimState->GetCollisionObject();
    if ( pCollisionObject == NULL )
        return false; // ditto for the held collision object

    return pCollisionObject->GetCollisionEnabled();    
}

//************************************************************************
//
// Protected Member Functions : DynaPhysDSG 
//
//************************************************************************

//************************************************************************
//
// Private Member Functions : DynaPhysDSG 
//
//************************************************************************
