/*===========================================================================
   redbrickcollisionsolveragent.cpp

   created Jan 28, 2002
   by Greg Mayer

   Copyright (c) 2002 Radical Entertainment, Inc.
   All rights reserved.

===========================================================================*/
#include <simcommon/simstatearticulated.hpp>
#include <simcommon/simulatedobject.hpp>
#include <worldsim/redbrick/redbrickcollisionsolveragent.h>
#include <worldsim/redbrick/vehicle.h>
#include <worldsim/redbrick/wheel.h>
#include <worldsim/worldphysicsmanager.h>
#include <mission/gameplaymanager.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/avatar.h>

#include <events/eventmanager.h>
#include <sound/soundcollisiondata.h>

#ifdef RAD_XBOX
#include <float.h>
#endif


//------------------------------------------------------------------------
RedBrickCollisionSolverAgent::RedBrickCollisionSolverAgent()
{
    //
    mBottomedOut = false;

    //mBottomedOutScale = 1.0e-5f;
    //mBottomedOutScale = 1.0f;
    //mBottomedOutScale = 0.0002f;
    mBottomedOutScale = 0.0003f;

    
    mWheelSidewaysHit = false;
    
    mWheelSidewaysHitScale = 0.002f;

}
    

//------------------------------------------------------------------------
RedBrickCollisionSolverAgent::~RedBrickCollisionSolverAgent()
{
    //
}


//------------------------------------------------------------------------------------------------
void RedBrickCollisionSolverAgent::ResetCollisionFlags()
{
    // TODO - how to properly tie this to rest of vehicle module????

    // TODO - need these flags on a car by car basis

    mBottomedOut = false;
    mWheelSidewaysHit = false;

}


//------------------------------------------------------------------------------------------------
Solving_Answer RedBrickCollisionSolverAgent::TestImpulse(rmt::Vector& impulse, Collision& inCollision)
{

    // currently this method does nothing...
    // 
    // test some collision results...

    



    // TODO -
    // something more efficient here so we don't have to test this shit every time

    CollisionObject* collObjA = inCollision.mCollisionObjectA;
    CollisionObject* collObjB = inCollision.mCollisionObjectB;
    
    SimState* simStateA = collObjA->GetSimState();
    SimState* simStateB = collObjB->GetSimState();

    //
    // SOUND EVENT HERE?
    //

    // need to convert the impulse to a nice friendly float between 0.0 and 1.0
    //
    // just looking at some empirical data, the range of impulse magnitude seems to be from around
    // 0.0 to 100000.0 - at the high end would be a heavy vehicle (3000 kg) hitting a static wall at 122kmh
    // a 2000kg car hitting a wall at 150kmh gave 78706.7





#ifdef RAD_DEBUG
    if(rmt::IsNan(impulse.x) || rmt::IsNan(impulse.y) || rmt::IsNan(impulse.z)) 
    {        
        rAssert(0);
    }
#endif





    float impulseMagnitude = impulse.Magnitude();

    if(impulseMagnitude > 100000.0f)
    {
        int stophere = 1;
    }
    


    const float maxIntensity = 100000.0f;

    float soundScale = impulseMagnitude / maxIntensity;
    if(soundScale > 1.0f)
    {
        soundScale = 1.0f;
    }
    if(soundScale < 0.0f)
    {
        rAssert(0);
    }

//    SoundCollisionData soundData( soundScale, simStateA->mAIRefPointer, simStateB->mAIRefPointer );
//    GetEventManager()->TriggerEvent( EVENT_COLLISION, &soundData );
    // up to this point is pretty generic for sound stuff - ie. the guts of PostReactToCollision might look a lot like this?


    // here is more vehicle intensive

    //if(simStateA->mAIRefIndex == Vehicle::GetAIRef() && simStateB->mAIRefIndex == Vehicle::GetAIRef())
    if(simStateA->mAIRefIndex == PhysicsAIRef::redBrickVehicle && simStateB->mAIRefIndex == PhysicsAIRef::redBrickVehicle)
    {      
        // car on car
        
        //static float magic = 1.0f;

        // fun test
        //impulse.Scale(magic);

        //int stophere = 1;

        if( ((Vehicle*)(simStateA->mAIRefPointer))->mVehicleType == VT_TRAFFIC )
        {

        }

    }

    if(simStateA->mAIRefIndex == PhysicsAIRef::redBrickVehicle || simStateB->mAIRefIndex == PhysicsAIRef::redBrickVehicle)
    {   
        //PhysicsVehicle* physicsVehicle;
        Vehicle* vehicle;

        float test = impulse.Magnitude();


        if(simStateA->mAIRefIndex == PhysicsAIRef::redBrickVehicle )
        {
            vehicle = (Vehicle*)(simStateA->mAIRefPointer);
        }
        else
        {
            vehicle = (Vehicle*)(simStateB->mAIRefPointer);
        }


        // TODO - more detailed test...     
    
        // TODO - I'm not sure I like this system...

        if(mBottomedOut) // todo - more detailed test for if we are dealing with a wheel
        {
            //mImpulse.Scale(mBottomedOutScale);
        }
        if(mWheelSidewaysHit)
        {
            //mImpulse.Scale(mWheelSidewaysHitScale);
    
            //mImpulse.y = 0.0f;  // TODO - expand to below later...
            
        }

    }

    return Solving_Continue;
}

//=============================================================================
// RedBrickCollisionSolverAgent::CarOnCarAction
//=============================================================================
// Description: Comment
//
// Parameters:  (Collision& inCollision)
//
// Return:      Solving_Answer 
//
//=============================================================================
Solving_Answer RedBrickCollisionSolverAgent::CarOnCarPreTest(Collision& inCollision, int inPass)
{  
    CollisionObject* collObjA = inCollision.mCollisionObjectA;
    CollisionObject* collObjB = inCollision.mCollisionObjectB;
    
    SimState* simStateA = collObjA->GetSimState();
    SimState* simStateB = collObjB->GetSimState();

    Vehicle* vehicleA = (Vehicle*)(simStateA->mAIRefPointer);
    Vehicle* vehicleB = (Vehicle*)(simStateB->mAIRefPointer);
 
    int jointIndexA = inCollision.mIndexA;
    int jointIndexB = inCollision.mIndexB;

    
   
    //vehicleA->TriggerDamage();
    //vehicleB->TriggerDamage(); - move the damage triggering to Vehicle::PostReactToCollision

//    vehicleA->SetHitJoint(jointIndexA);
//    vehicleB->SetHitJoint(jointIndexB);

 
    // for now...    
    if(1)//vehicleA->mUsingInCarPhysics)
    {
        if(vehicleA->IsJointAWheel(jointIndexA) || vehicleB->IsJointAWheel(jointIndexB))
        {
            return Solving_Aborted;
        }

        if(vehicleA->IsAFlappingJoint(jointIndexA) || vehicleB->IsAFlappingJoint(jointIndexB))
        {
            // TODO - try taking this out?
            return Solving_Aborted;
        }
    }

    //hmmm
    // is this the place to switch loco?
    if(vehicleA->GetLocomotionType() == VL_TRAFFIC)
    {     
        vehicleA->SetLocomotion(VL_PHYSICS);
    }

    if(vehicleB->GetLocomotionType() == VL_TRAFFIC)
    {        
        vehicleB->SetLocomotion(VL_PHYSICS);
    }
    
    // just in case it has come to rest....
    
    if(simStateA->GetControl() == sim::simAICtrl)
    {
        simStateA->SetControl(sim::simSimulationCtrl);
    }

    if(simStateB->GetControl() == sim::simAICtrl)
    {
        simStateB->SetControl(sim::simSimulationCtrl);
    }
    

    return CollisionSolverAgent::PreCollisionEvent(inCollision, inPass);
}

//=============================================================================
// RedBrickCollisionSolverAgent::PreCollisionEvent
//=============================================================================
// Description: Comment
//
// Parameters:  (Collision& inCollision, int inPass)
//
// Return:      Solving_Answer 
//
//=============================================================================
Solving_Answer RedBrickCollisionSolverAgent::PreCollisionEvent(Collision& inCollision, int inPass)
{    
    // have a look at objects A and B and see if we care about them:

    // TODO 
    // also, depending how steeply we've hit something, we also want to let the solver do it's thing...
    // ? mabye cancel out the y component?

    // also may skip this depending how tipped we are


    CollisionObject* collObjA = inCollision.mCollisionObjectA;
    CollisionObject* collObjB = inCollision.mCollisionObjectB;
    
    SimState* simStateA = collObjA->GetSimState();
    SimState* simStateB = collObjB->GetSimState();

    if(simStateA->mAIRefIndex == PhysicsAIRef::redBrickVehicle || simStateB->mAIRefIndex == PhysicsAIRef::redBrickVehicle)
    {      
        //--------------------------------------
        // deal specially with car on car action
        //--------------------------------------
        if(simStateA->mAIRefIndex == PhysicsAIRef::redBrickVehicle && simStateB->mAIRefIndex == PhysicsAIRef::redBrickVehicle)
        {
            // test for hitting a wheel or a flapping joint in here
            //
            return CarOnCarPreTest(inCollision, inPass);
        }

        Vehicle* vehicle;

        int jointIndex;

        rmt::Vector groundContactPoint;
        rmt::Vector normalPointingAtCar;

        bool carisA = true;

        if(simStateA->mAIRefIndex == PhysicsAIRef::redBrickVehicle)
        {       
            vehicle = (Vehicle*)(simStateA->mAIRefPointer);
            jointIndex = inCollision.mIndexA;

            groundContactPoint = inCollision.GetPositionB();
            normalPointingAtCar = inCollision.mNormal;

        }
        else
        {
            vehicle = (Vehicle*)(simStateB->mAIRefPointer);
            jointIndex = inCollision.mIndexB;

            groundContactPoint = inCollision.GetPositionA();
            normalPointingAtCar = inCollision.mNormal;
            normalPointingAtCar.Scale(-1.0f);
            carisA = false;
        }


        // temp hack cause I think we're hitting our driver every frame:
/*       
        if(carisA)
        {
            if(simStateB->mAIRefIndex == PhysicsAIRef::redBrickPhizMoveable)
            {
                return Solving_Aborted;
            }
        }
        else
        {
            if(simStateA->mAIRefIndex == PhysicsAIRef::redBrickPhizMoveable)
            {
                return Solving_Aborted;
            }
            
        }
*/       


        /*
        
            this now encompassed below...
        
        if(carisA)
        {
            if(simStateB->mAIRefIndex == PhysicsAIRef::StateProp)
            {
                Avatar* playerAvatar = GetAvatarManager()->GetAvatarForPlayer(0);
                if(playerAvatar->GetVehicle() == vehicle)
                {
                    GetEventManager()->TriggerEvent( EVENT_HIT_MOVEABLE, (void*)simStateB );
                }
            }
        }
        else
        {
            if(simStateA->mAIRefIndex == PhysicsAIRef::StateProp)
            {
                Avatar* playerAvatar = GetAvatarManager()->GetAvatarForPlayer(0);
                if(playerAvatar->GetVehicle() == vehicle)
                {
                    GetEventManager()->TriggerEvent( EVENT_HIT_MOVEABLE, (void*)simStateA );
                }
            }
        }
        */

        if(carisA)
        {
            if(simStateB->mAIRefIndex == PhysicsAIRef::redBrickPhizMoveable || simStateB->mAIRefIndex == PhysicsAIRef::StateProp)
            {                
                sim::SimControlEnum control = simStateB->GetControl();
                bool ishit = ((DynaPhysDSG*)(simStateB->mAIRefPointer))->mIsHit;
                enClasstypeID classid = (enClasstypeID)((DynaPhysDSG*)(simStateB->mAIRefPointer))->GetCollisionAttributes()->GetClasstypeid();
            
            
                if(simStateB->GetControl() == sim::simAICtrl && ((DynaPhysDSG*)(simStateB->mAIRefPointer))->mIsHit &&
                   ((DynaPhysDSG*)(simStateB->mAIRefPointer))->GetCollisionAttributes()->GetClasstypeid() == PROP_ONETIME_MOVEABLE)
                {
                    return Solving_Aborted;
                }
                else
                {
                    Avatar* playerAvatar = GetAvatarManager()->GetAvatarForPlayer(0);
                    if(playerAvatar->GetVehicle() == vehicle)
                    {
                        GetEventManager()->TriggerEvent( EVENT_HIT_MOVEABLE, (void*)simStateB );
                    }
                }
            }
        }
        else
        {
            if(simStateA->mAIRefIndex == PhysicsAIRef::redBrickPhizMoveable || simStateA->mAIRefIndex == PhysicsAIRef::StateProp)
            {
            
                sim::SimControlEnum control = simStateA->GetControl();
                bool ishit = ((DynaPhysDSG*)(simStateA->mAIRefPointer))->mIsHit;
                enClasstypeID classid = (enClasstypeID)((DynaPhysDSG*)(simStateA->mAIRefPointer))->GetCollisionAttributes()->GetClasstypeid();
            
            
            
                if(simStateA->GetControl() == sim::simAICtrl && ((DynaPhysDSG*)(simStateA->mAIRefPointer))->mIsHit &&
                  ((DynaPhysDSG*)(simStateA->mAIRefPointer))->GetCollisionAttributes()->GetClasstypeid() == PROP_ONETIME_MOVEABLE)   // safe cast?                
                {
                    return Solving_Aborted;
                }
                else
                {
                    Avatar* playerAvatar = GetAvatarManager()->GetAvatarForPlayer(0);
                    if(playerAvatar->GetVehicle() == vehicle)
                    {
                        GetEventManager()->TriggerEvent( EVENT_HIT_MOVEABLE, (void*)simStateA );
                    }
                }
                
            }
            
        }

        if(vehicle->mUsingInCarPhysics)
        {
            if(vehicle->IsAFlappingJoint(jointIndex))
            {
            
    //            vehicle->SetHitJoint(jointIndex);

                return Solving_Aborted;
            }
        }



        if(vehicle->IsJointAWheel(jointIndex))
        {      

            if(carisA)
            {
                if(simStateB->mAIRefIndex == PhysicsAIRef::redBrickPhizFence)
                {
                    int stophere = 1;
                    
                    // the old tip test
                    rmt::Vector worldUp = GetWorldPhysicsManager()->mWorldUp;
                    
                    float cos10 = 0.9848f;
                    //if(worldUp.DotProduct(vehicle->mVehicleUp) < cos10)
                    {
                        // will this work??
                        inCollision.mIndexA = 0;
                        return Solving_Continue;                  
                
                    }
                    
                }
                
                
            
                if(simStateB->mAIRefIndex != PhysicsAIRef::redBrickPhizStatic)  // redBrickPhizMoveableAnim? -> don't think we need to 
                {
                    if(vehicle->mUsingInCarPhysics)
                    {
                        return Solving_Aborted;                    
                    }
                    else
                    {
                        return Solving_Continue;
                    }
                }
                
                
            }
            else
            {
            
                if(simStateA->mAIRefIndex == PhysicsAIRef::redBrickPhizFence)
                {
                    // the old tip test
                    rmt::Vector worldUp = GetWorldPhysicsManager()->mWorldUp;
                    
                    float cos10 = 0.9848f;
                    //if(worldUp.DotProduct(vehicle->mVehicleUp) < cos10)                    
                    {
                        // will this work??
                        inCollision.mIndexB = 0;
                        return Solving_Continue;                  
                
                    }
                
                    int stophere = 1;
                }
                
            
                if(simStateA->mAIRefIndex != PhysicsAIRef::redBrickPhizStatic)
                {
                    if(vehicle->mUsingInCarPhysics)
                    {
                        return Solving_Aborted;                    
                    }
                    else
                    {
                        return Solving_Continue;
                    }
                }
            }


            /*
            // if this is a wheel, but the thing we hit is a vehicle ground plane, then we want to ignore it.
            if(carisA)
            {
                if(simStateB->mAIRefIndex == vehicle->GetGroundPlaneAIRef() || 
                   simStateB->mAIRefIndex == PhysicsAIRef::redBrickPhizMoveable)    // new - do we want to keep this?
                {
                    return Solving_Aborted;                    
                }
            }
            else
            {
                if(simStateA->mAIRefIndex == vehicle->GetGroundPlaneAIRef() ||
                   simStateA->mAIRefIndex == PhysicsAIRef::redBrickPhizMoveable)    // new - do we want to keep this?
                {
                    return Solving_Aborted;
                }

            }   
            */

            // recall:
            // mPositionA = mPositionB + mNormal * mDistance

            // SG::phizSim.mCollisionDistanceCGS;

            // amount we want to keep in collision
            // TODO - even want to do this?
            //float slightlyColliding = (SG::phizSim.mCollisionDistanceCGS * 0.1f) * 0.01f; // last factor to make sure we're in meters...
  
            //float slightlyColliding = (SG::phizSim.mCollisionDistanceCGS * 1.0f) * 0.01f; // last factor to make sure we're in meters...

            // TODO
            // hack to fix for no phizsim
            float collisionDistanceCGS = 2.0f;

            //float slightlyColliding = (SG::phizSim.mCollisionDistanceCGS * 1.0f) * 0.01f; // last factor to make sure we're in meters...
            float slightlyColliding = (collisionDistanceCGS * 1.0f) * 0.01f; // last factor to make sure we're in meters...
  
            // TODO - choose right value here based on coefficient of restituion blah blah
            // initial random guessof 0.1 was so low it was never used since the impulses
            // kept shit apart.
            //
            // hmmmm... maybe want to use that?  but then how does suspension compress?
            //


            // if we hit something sideways with the wheel, just apply impulse
            float cos80 = 0.1736f;
            float cos70 = 0.342f;

            // TODO - this isn't nearly adequate!
            
            // TODO 
            // temp fix
           
            rmt::Vector worldUp = GetWorldPhysicsManager()->mWorldUp;
         
            // new test
            //if(vehicle->mPercentOfTopSpeed > 0.3f)
            if(vehicle->mPercentOfTopSpeed > 0.1f)
            {
                int wheelIndex = vehicle->mJointIndexToWheelMapping[jointIndex];
                rAssert(wheelIndex >= 0);
                rAssert(wheelIndex < 5);
                
                rmt::Vector normVel = vehicle->mSuspensionPointVelocities[wheelIndex];
            
                normVel.NormalizeSafe();
                float cos45 = 0.7071f;
                if(normalPointingAtCar.DotProduct(normVel) > cos45)
                //if(normalPointingAtCar.DotProduct(normVel) > cos70)
                {
                    // the collision is pointing the way we're going so fuck it
                    //char buffy[128];
                    //sprintf(buffy, "aborting wheel-static collision fix - pointing too much along velocity\n");
                    //rDebugPrintf(buffy);
                    
                    
                    vehicle->SetNoDamperDownFlagOnWheel(wheelIndex);
                    
                    return Solving_Aborted;   
                }
            }
         
         
         
            float cos55 = 0.5736f;
           
            //if( rmt::Fabs((inCollision.mNormal).DotProduct(vehicle->mVehicleUp)) < cos70 || // recall - this is the new line
            if( rmt::Fabs((inCollision.mNormal).DotProduct(vehicle->mVehicleUp)) < cos55 || // recall - this is the new line
                worldUp.DotProduct(vehicle->mVehicleUp) < cos80) // leave this on at 80        
            {

                //char buffy[128];
                //sprintf(buffy, "aborting wheel-static collision fix - too horizontal\n");
                //rDebugPrintf(buffy);

                int wheelIndex = vehicle->mJointIndexToWheelMapping[jointIndex];
                rAssert(wheelIndex >= 0);
                rAssert(wheelIndex < 5);
   
                vehicle->SetNoDamperDownFlagOnWheel(wheelIndex);

                // temp
                // TODO - something?
                return Solving_Aborted;

                // let collision solver just apply some impulse
                //
                // TODO - scale down impulse??
                
                mWheelSidewaysHit = true;

                if(carisA)
                {
                    inCollision.mIndexA = 0;
                }
                else
                {
                    inCollision.mIndexB = 0;
                }

                return CollisionSolverAgent::PreCollisionEvent(inCollision, inPass);
                
            }
         



            if(inCollision.mDistance < slightlyColliding)
            {                
                // need to move along normal by:
                float fixAlongCollisionNormal = slightlyColliding - inCollision.mDistance;

                float fixAlongSuspensionAxis = fixAlongCollisionNormal / rmt::Fabs( (vehicle->mVehicleUp).DotProduct(inCollision.mNormal) );

                // now fixAlongSuspensionAxis is the object space y offset that we should correct to
                
                // this will return true for bottom out
                if(vehicle->SetWheelCorrectionOffset(jointIndex, fixAlongSuspensionAxis, normalPointingAtCar, groundContactPoint))
                //if(fixAlongSuspensionAxis > 2.0f * physicsVehicle->GetWheelByJoint(jointIndex)->GetSuspensionLimit() )
                {
                    // we've bottomed out
                    //
                    // so also let solver apply impulse
                    mBottomedOut = true;    // TODO - should also store the joint! - ...later...why?

                    // if we've bottomed out, we only want to transfer impulse to the chassis if 
                    // it's more or less up

                    const float cos20 = 0.9397f;

                    //if( rmt::Fabs((inCollision.mNormal).DotProduct(vehicle->mVehicleUp)) > cos20  )
                    if(normalPointingAtCar.DotProduct(vehicle->mVehicleUp) > cos20)
                    {
                        /*
                        if(carisA)
                        {
                            inCollision.mIndexA = 0;
                        }
                        else
                        {
                            inCollision.mIndexB = 0;
                        }

                        return CollisionSolverAgent::PreCollisionEvent(inCollision, inPass);                    
                        
                        
                        april 11, 2003
                        this makes an absolute world of difference in jumps!!!!
                        
                        
                        */
                        
                        return Solving_Aborted;
                    }
                    else
                    {
                        return Solving_Aborted;
                    }

                }
                
                return Solving_Aborted;

            }

       

        } // end of the IsJointAWheel block...

        
        // if we got here we are the chassis hitting a static or our own groudn plane
        if(carisA)
        {
            if(simStateB->mAIRefIndex == PhysicsAIRef::redBrickPhizVehicleGroundPlane)
            {
            
                rmt::Vector up = GetWorldPhysicsManager()->mWorldUp;
                float tip = vehicle->mVehicleUp.DotProduct(up);

                //float cos10 = 0.9848f;       
                float cos16 = 0.9613f;
                
                if(vehicle->mAirBorn && tip > cos16)
                {
                    //int stophere = 1;
                    return Solving_Aborted;
                }                       
            
            
                //vehicle->mChassisHitGroundPlaneThisFrame = true;
            }
        }
        else
        {
            if(simStateA->mAIRefIndex == PhysicsAIRef::redBrickPhizVehicleGroundPlane)
            {
            
            
                rmt::Vector up = GetWorldPhysicsManager()->mWorldUp;
                float tip = vehicle->mVehicleUp.DotProduct(up);

                //float cos10 = 0.9848f;       
                float cos16 = 0.9613f;
                
                if(vehicle->mAirBorn && tip > cos16)
                {
                    //int stophere = 1;
                    return Solving_Aborted;
                }              
            
            
            
                //vehicle->mChassisHitGroundPlaneThisFrame = true;
            }
        
        }

        

 
        if(inCollision.mDistance < -0.5f)
        //if(inCollision.mDistance < -0.25f)
        //if(inCollision.mDistance < -0.1f)
        {
            //if(vehicle->mVehicleID == VehicleEnum::FAMIL_V)
            //{
            //    int stophere = 1;   // motherfucking goddamn data conditions on breakpoints not working!!
            //}
     
            // greg
            // jan 15, 2003
            
            // did you know, that to penetrate something 0.5m in 32ms, you only have to be going 56 kmh ?
            
            // live and learn
        
            //return Solving_Aborted;   
            //inCollision.mDistance = -0.1f; // fuck
        }
        
        // this is just chassis hitting something here...
        
        // test - if all wheels are out of collision, transffer impulse to root...?
                      

     }
     return Solving_Continue;
}



Solving_Answer RedBrickCollisionSolverAgent::EndObjectCollision(SimState* inSimState, int inIndex)
{

    if(inSimState->mAIRefIndex == PhysicsAIRef::redBrickVehicle)
    {      
        Vehicle* vehicle = (Vehicle*)(inSimState->mAIRefPointer);           

        if(vehicle->IsAFlappingJoint(inIndex))
        {
            //vehicle->mSimStateArticulated->GetSimulatedObject()->ResetCache();
        }
    }

    return CollisionSolverAgent::EndObjectCollision(inSimState, inIndex);
}



Solving_Answer RedBrickCollisionSolverAgent::TestCache(SimState* inSimState, int inIndex)
{
    // this is called everytime an impulse is added on an object. Getting the cache give information 

/*
    if(inSimState->mAIRefIndex == PhysicsAIRef::redBrickVehicle)
    {      
        // car on car
        
        //static float magic = 1.0f;

        // fun test
        //impulse.Scale(magic);

        //int stophere = 1;

        if( ((Vehicle*)(inSimState->mAIRefPointer))->mVehicleType == VT_TRAFFIC )
        {
            static float magic = 5.0f;

            // fun test
            impulse.Scale(magic);

            SimulatedObject* simobj = inSimState->GetSimulatedObject(-1);
            
            rmt::Vector cachev, cachew;
            simobj->GetCollisionCache(cachev, cachew, -1); //retrieve the collision cache of the physical object.

            
            if (cachev.MagnitudeSqr() > maxDv2)
            {
                bool fuck = true;
            }


        }
    }

*/


#if 0 
    SimulatedObject* simobj = inSimState->GetSimulatedObject(-1);
    

    if ( simobj && (simobj->Type() == RigidObjectType || simobj->Type() == ArticulatedObjectType) )
    {
        /*
        static float thresholdfactor1 = 100.0f;
        Vector currentv, vcmv, cachev, cachew;
        currentv = inSimState->GetLinearVelocity(); //current speed of the object
        if (inSimState->GetVirtualCM()==NULL) 
            vcmv.Clear();
        else
            vcmv = inSimState->GetVirtualCM()->GetVelocity(); //The current speed of the vcm of the object
        simobj->GetCollisionCache(cachev, cachew, -1); //retrieve the collision cache of the physical object.
        cachev.Add(vcmv); //The combined vcm's speed and the cached speed.
        float testSpeed = Max(currentv.DotProduct(currentv),Sqr(simobj->GetMinimumLinSpeed()));
        if ( cachev.DotProduct(cachev) > thresholdfactor1*testSpeed )
        {
            //inSimState->SetControl(simSimulationCtrl);
            return Solving_Aborted;
        }
        */
        Vector cachev, cachew;
        simobj->GetCollisionCache(cachev, cachew, -1); //retrieve the collision cache of the physical object.

        static float maxDv2 = 27.0f*27.0f;
        if (cachev.MagnitudeSqr() > maxDv2)
        {
            bool fuck = true;
        }
    }
#endif

    /*
    if (simobj && simobj->Type() == ArticulatedObjectType)
    {
        //Looks if there is a vcm installed on this joint:
        SimStateArticulated *simStateArt = (SimStateArticulated*)inSimState;
        JointVCMpArray *vcma = &simStateArt->mVirtualCMList;
        for (int i=0 ; i<vcma->GetSize() ; i++)
        {
            JointVirtualCM *vcm = vcma->GetAt(i);
            if (vcm->GetIndex() == inIndex)
            {
                static float thresholdfactor2=100.0f;
                Vector currentv, vcmv, cachev, cachew;
                simStateArt->GetVelocity(vcm->GetPosition(), currentv, inIndex);
                vcmv = vcm->GetVelocity();
                simobj->GetCollisionCache(cachev, cachew, inIndex);
                if ( cachev.DotProduct(cachev) > thresholdfactor2*currentv.DotProduct(currentv) )
                {
                    simStateArt->SetControl(simSimulationCtrl);
                    return Solving_Aborted;
                }
            }
        }
    }
    */
    return Solving_Continue;
}

