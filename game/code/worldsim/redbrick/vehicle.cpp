//=============================================================================
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// File:        vehicle.cpp
//
// Description: the car
//
// History:     Nov 16, 2001 + Created -- gmayer
//
//=============================================================================


//========================================
// System Includes
//========================================
#include <p3d/anim/skeleton.hpp>
#include <p3d/matrixstack.hpp>
#include <simcommon/simstate.hpp>
#include <simcommon/simstatearticulated.hpp>
#include <simcommon/simulatedobject.hpp>
#include <simcommon/simenvironment.hpp>
#include <simcollision/collisionobject.hpp>
#include <simcollision/collisionvolume.hpp>
#include <simcollision/collisionmanager.hpp>
#include <simphysics/articulatedphysicsobject.hpp>
#include <simphysics/physicsobject.hpp>
#include <mission/charactersheet/charactersheetmanager.h>
#include <simcollision/collisiondisplay.hpp>
#include <render/DSG/StatePropDSG.h>
#include <worldsim/character/characterrenderable.h>
#include <mission/statepropcollectible.h>
#include <mission/objectives/missionobjective.h>

#include <raddebug.hpp>
#include <raddebugwatch.hpp>

#include <string.h>

//========================================
// Project Includes
//========================================

#include <worldsim/redbrick/vehicle.h>

#include <worldsim/worldphysicsmanager.h>

#include <worldsim/redbrick/geometryvehicle.h>
#include <worldsim/redbrick/wheel.h>
#include <worldsim/redbrick/redbrickcollisionsolveragent.h>

#include <worldsim/redbrick/physicslocomotion.h>
#include <worldsim/redbrick/trafficlocomotion.h>
#include <worldsim/traffic/trafficmanager.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/avatar.h>
#include <worldsim/character/character.h>


#include <choreo/puppet.hpp>


#include <events/eventmanager.h>
#include <events/eventdata.h>

#include <roads/roadsegment.h>
#include <roads/geometry.h>


#include <memory/srrmemory.h>
#include <mission/gameplaymanager.h>

#include <meta/carstartlocator.h>

#include <debug/debuginfo.h>


#include <ai/actionbuttonhandler.h>
#include <ai/actionbuttonmanager.h>

#include <meta/eventlocator.h>
#include <meta/spheretriggervolume.h>
#include <meta/recttriggervolume.h>


#include <render/RenderManager/RenderManager.h>
#include <render/Culling/WorldScene.h>
#include <render/IntersectManager/IntersectManager.h>
#include <render/breakables/breakablesmanager.h>

#include <sound/soundcollisiondata.h>

#include <radmath/radmath.hpp>
#include <worldsim/coins/sparkle.h>

#include <cheats/cheatinputsystem.h>

#include <mission/gameplaymanager.h>
#include <supersprint/supersprintmanager.h>
#include <p3d/billboardobject.hpp>
#include <meta/triggervolumetracker.h>

#include <camera/supercammanager.h>
#include <camera/supercamcentral.h>

#include <presentation/gui/guisystem.h>

using namespace sim;

// note - methods that are only called once at initialization, moved to vehicleinit

// CONSTANTS
// In what radius the vehicle explosion will affect objects
const float EXPLOSION_EFFECT_RADIUS = 20.0f;
// How much force to apply to objects within this radius
const float EXPLOSION_FORCE = 20000.0f; 
// Set the center of the explosion so be below that car position so that objects get hurled upwards
const float EXPLOSION_Y_OFFSET = -5.0f;

const float HITPOINTS_REMOVED_FROM_VEHICLE_EXPLOSION = 0.5;
const float HITPOINTS_REMOVED_FROM_VEHICLE_EXPLOSION_PLAYER = 0.5;

static const float REST_LINEAR_TOL = 0.05f; // linear velocity tolerance
static const float REST_ANGULAR_TOL = 0.3f; // angular velocity tolerance

// Initialize static variables
float Vehicle::s_DamageFromExplosion = HITPOINTS_REMOVED_FROM_VEHICLE_EXPLOSION;
float Vehicle::s_DamageFromExplosionPlayer = HITPOINTS_REMOVED_FROM_VEHICLE_EXPLOSION_PLAYER;

bool Vehicle::sDoBounce = false;

void Vehicle::ActivateTriggers( bool activate )
{
    if(activate == mTriggerActive)
    {
        return;
    }

    if( mVehicleType == VT_AI && activate )
    {
        // NOT OK to add triggers for AI car doors, but OK to remove...
        // AI triggers remain as they are (so you can't get into the cars)
        //rAssert( false, "FALSE, Chuck, FALSE!" );
        return;
    }

    if(mVehicleDestroyed && activate)
    {
        return;
    }

    if(!GetVehicleCentral()->GetVehicleTriggersActive() && activate)
    {
        return;
    }

    if( mpEventLocator == NULL )
    {
        return;
    }

    mTriggerActive = activate;

    if( activate )
    {
        for( unsigned j = 0; j < mpEventLocator->GetNumTriggers(); j++ )
        {
            GetTriggerVolumeTracker()->AddTrigger( mpEventLocator->GetTriggerVolume( j ) );
        }
    }
    else
    {
        for( unsigned j = 0; j < mpEventLocator->GetNumTriggers(); j++ )
        {
            GetTriggerVolumeTracker()->RemoveTrigger( mpEventLocator->GetTriggerVolume( j ) );
        }
    }
}

void Vehicle::SetUserDrivingCar( bool b ) 
{
    // chooka set's this when a character get's in or out.
    mUserDrivingCar = b;  
    if( mVehicleType != VT_AI && mUserDrivingCar ) 
    {
        mVehicleType = VT_USER;
    }
}   


void Vehicle::TransitToAI()
{
    mVehicleType = VT_AI;
    ActivateTriggers( false );
    GetEventManager()->TriggerEvent( EVENT_USER_VEHICLE_REMOVED_FROM_WORLD, this );
}

//=============================================================================
// 
//=============================================================================
// Description: Comment
//
// Parameters: 
//
// Return:      void 
//
//=============================================================================
void Vehicle::EnteringJumpBoostVolume()
{
    mDoingJumpBoost = true; 
}

/*
void Vehicle::AddRef()
{
    if( mVehicleType == VT_TRAFFIC && TrafficManager::GetInstance()->IsVehicleTrafficVehicle(this) )
    {
        rDebugPrintf( "Booya!\n" );
    }
    tRefCounted::AddRef();
}

void Vehicle::Release()
{
    if( mVehicleType == VT_TRAFFIC && TrafficManager::GetInstance()->IsVehicleTrafficVehicle(this) )
    {
        rDebugPrintf( "Yaaaboo!\n" );
    }
    tRefCounted::Release();
}
*/

//=============================================================================
// Vehicle::
//=============================================================================
// Description: Comment
//
// Parameters:
//
// Return:      void 
//
//=============================================================================
void Vehicle::ExitingJumpBoostVolume()
{
    if(mDoingJumpBoost)
    {
        // if we were doing one, now might be a good time for this event: 
        if(mVehicleType == VT_USER)
        {       
            GetEventManager()->TriggerEvent(EVENT_BIG_AIR, (void*)(this->mpDriver));       
        }
    
    }

    mDoingJumpBoost = false;
}
    
int Vehicle::CastsShadow()
{
    int retVal;
    // Casts a shadow in the 2nd pass, if this vehicle has one (witch and ship don't)
    if ( m_IsSimpleShadow )
    {
        retVal = 989;
    }
    else
    {
        retVal = 0;
    }
    return retVal;
}
//=============================================================================
// Vehicle::DisplayShadow
//=============================================================================
// Description: Comment
//
// Parameters:  ( )
//
// Return:      void 
//
//=============================================================================
void Vehicle::DisplayShadow()
{
    if( !mOkToDrawSelf || !mDrawVehicle )
    {
        return;
    }

    BEGIN_PROFILE("Vehicle::DisplayShadow")
    if( !IsSimpleShadow() )
	{
		p3d::stack->Push();
		p3d::stack->Multiply(mTransform);

		mGeometryVehicle->DisplayShadow();

		p3d::stack->Pop();
	}
    END_PROFILE("Vehicle::DisplayShadow")
}

void Vehicle::DisplaySimpleShadow( void )
{
    if( !mOkToDrawSelf || !mDrawVehicle )
    {
        return;
    }

    BEGIN_PROFILE("Vehicle::DisplaySimpleShadow")
    p3d::pddi->SetZWrite(false);
	if( IsSimpleShadow() )
	{
        const float HeightRatio = 1.0f / 4.0f;
        rmt::Vector pos = mTransform.Row( 3 );
        float carY = pos.y;
        pos.y = GetGroundY();
		rmt::Vector norm;
		rmt::Vector forward;
    	forward = mTransform.Row( 2 );

        if( GetLocomotionType() == VL_TRAFFIC )
        {
            // We'll assume the traffic doesn't go jumping through the air...although they
            //could theorically get knocked through the air I guess. When that happens however
            //they are under physics control and Greg says they aren't VL_TRAFFIC any more.
    		norm = mTransform.Row( 1 );
        }
        else
        {
            //const rmt::Matrix& groundTrans = mGroundPlaneSimState->GetTransform();
    		//norm = groundTrans.Row( 2 );
    		
    		norm = this->mGroundPlaneWallVolume->mNormal;
        }

        BlobShadowParams p( pos, norm, forward );
        p.ShadowScale = rmt::Clamp( 1.0f - ( ( carY - ( pos.y + GetRestHeightAboveGround() ) ) * HeightRatio ), 0.0f, 1.0f );
        p.ShadowAlpha = p.ShadowScale * ( mInterior ? 0.5f : 1.0f );
        mGeometryVehicle->DisplayShadow( &p );
	}
    p3d::pddi->SetZWrite(true);
    END_PROFILE("Vehicle::DisplaySimpleShadow")
}



//=============================================================================
// Vehicle::SetInCarSimState
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::SetInCarSimState()
{
    if(mSimStateArticulatedOutOfCar)
    {
        if(!mUsingInCarPhysics)
        {
            if(mCollisionAreaIndex != -1)
            {
                GetWorldPhysicsManager()->RemoveFromAnyOtherCurrentDynamicsListAndCollisionArea(this);
            
                RemoveSelfFromCollisionManager();   // deals with our own index        
                GetWorldPhysicsManager()->EmptyCollisionAreaIndex(this->mCollisionAreaIndex);
                
            }
        
            mSimStateArticulatedOutOfCar->ResetVelocities();        
            mSimStateArticulatedInCar->ResetVelocities();
            
            rmt::Matrix transform = mSimStateArticulatedOutOfCar->GetTransform();
            
            mSimStateArticulatedInCar->SetControl(sim::simAICtrl);
            mSimStateArticulatedInCar->SetTransform(transform);
            mSimStateArticulatedInCar->SetControl(sim::simSimulationCtrl);
            
    
            
            mSimStateArticulated = mSimStateArticulatedInCar;
        
            mUsingInCarPhysics = true;
            
            if(mCollisionAreaIndex != -1)
            {
                AddSelfToCollisionManager();
            }
        }
           
    }       
    else
    {
        // please God let this be the last fucking hack in this game...
        mSimStateArticulated->ResetVelocities();

        rmt::Matrix transform = mSimStateArticulated->GetTransform();

        mSimStateArticulated->SetControl(sim::simAICtrl);
        mSimStateArticulated->SetTransform(transform);
        mSimStateArticulated->SetControl(sim::simSimulationCtrl);

        CalculateSuspensionLocationAndVelocity();   // just in case



    }
    

}


//=============================================================================
// Vehicle::SetOutOfCarSimState
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::SetOutOfCarSimState()
{    
    if(mSimStateArticulatedOutOfCar)
    {
        if(mUsingInCarPhysics)
        {    
            if(mCollisionAreaIndex != -1)
            {
                GetWorldPhysicsManager()->RemoveFromAnyOtherCurrentDynamicsListAndCollisionArea(this);
            
                RemoveSelfFromCollisionManager();   // deals with our own index     
                
                GetWorldPhysicsManager()->EmptyCollisionAreaIndex(this->mCollisionAreaIndex);
                
                // hmmm
                // this is probably at init.
                //
                // we need to do something to try and make the normal physics update loop run once
                // so that the cars settles into place with suspensionYOffset set by designers...
                
                // try this:
                //this->PreSubstepUpdate();
                //this->PreCollisionPrep(0.016f, true);
                //this->Update(0.016f);   
                //this->PostSubstepUpdate();
                
            
            }              
                
            mSimStateArticulatedOutOfCar->ResetVelocities();        
            mSimStateArticulatedInCar->ResetVelocities();
        
            
            rmt::Matrix transform = mSimStateArticulatedInCar->GetTransform();
            
            mSimStateArticulatedOutOfCar->SetControl(sim::simAICtrl);
            mSimStateArticulatedOutOfCar->SetTransform(transform);
            mSimStateArticulatedOutOfCar->SetControl(sim::simSimulationCtrl);
        
            mSimStateArticulated = mSimStateArticulatedOutOfCar;        
        
            CalculateSuspensionLocationAndVelocity();   // just in case
            
            mUsingInCarPhysics = false; 
            
            if(mCollisionAreaIndex != -1)
            {
                AddSelfToCollisionManager(); 
            }
        }

    }
    else
    {
        // please God let this be the last fucking hack in this game...
        mSimStateArticulated->ResetVelocities();

        rmt::Matrix transform = mSimStateArticulated->GetTransform();

        mSimStateArticulated->SetControl(sim::simAICtrl);
        mSimStateArticulated->SetTransform(transform);
        mSimStateArticulated->SetControl(sim::simSimulationCtrl);

        CalculateSuspensionLocationAndVelocity();   // just in case



    }
    
}
  
  



//=============================================================================
// Vehicle::SetLocomotion
//=============================================================================
// Description: Comment
//
// Parameters:  ( VehicleLocomotionType loco )
//
// Return:      void 
//
//=============================================================================
void Vehicle::SetLocomotion( VehicleLocomotionType loco )
{

    switch(loco)
    {
        case VL_PHYSICS:
    
            if(mLoco == VL_TRAFFIC)
            {
                // this is a traffic car that has just been hit
                // deactivate the AI so that when we get to a rest state
                // and return to AI control the car won't move
                ::GetEventManager()->TriggerEvent( EVENT_TRAFFIC_GOT_HIT, this );
                TrafficManager::GetInstance()->Deactivate( this );
            }

            mVehicleLocomotion = mPhysicsLocomotion;

            //mSimStateArticulated->ResetVelocities();  //hmmmmmmmmmmmmmmmmmmmmmmmmmm
            // looks like we have to do this

            //mSimStateArticulated->StoreJointState(0.016f);
            mSimStateArticulated->SetControl(simSimulationCtrl);
    
            mLocoSwitchedToPhysicsThisFrame = true;
            
            // hmm....
            // safe to set velocity of vehicle to mVelocityCM?
            //mSimStateArticulated->ResetVelocities();

            break; 

        case VL_TRAFFIC:
      
            mVehicleLocomotion = mTrafficLocomotion;
            mSimStateArticulated->SetControl(simAICtrl);

            mSimStateArticulated->ResetVelocities();

            break;

        default:
            rAssert(0);
    }

    mLoco = loco;
}



//=============================================================================
// Vehicle::IsAFlappingJoint
//=============================================================================
// Description: Comment
//
// Parameters:  (int index)
//
// Return:      bool 
//
//=============================================================================
bool Vehicle::IsAFlappingJoint(int index)
{
    if( index == mDoorDJoint ||
        index == mDoorPJoint ||
        index == mHoodJoint ||
        index == mTrunkJoint)
    {
        return true;
    }

    return false;

}


//=============================================================================
// Vehicle::CalculateDragCoeffBasedOnTopSpeed
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::CalculateDragCoeffBasedOnTopSpeed()
{
    // this will also have to be called again whenever the desginer params are reloaded


    // dragforce = 0.5 * coeff * speed * speed;
    // 
    // so, we need a coeff that causes dragforce to match engine force at that speed
    //
    // for now we know the max engine force - 2 * 1.0f * mDpGasScale

    float topspeedmps = mDesignerParams.mDpTopSpeedKmh / 3.6f;
    
    // * 2 because this is what we apply at each wheel
    // also need to subtract rolling friction for this to be accurate
    //float terminalForce = 2.0f * mDesignerParams.mDpGasScale * mDesignerParams.mDpMass - mRollingFrictionForce;
    
    float terminalForce = 0.0f;
    if(mDesignerParams.mDpGasScaleSpeedThreshold == 1.0f)
    {
        terminalForce = 2.0f * mDesignerParams.mDpGasScale * mDesignerParams.mDpMass;        //) - (mRollingFrictionForce * mDesignerParams.mDpMass);
    }
    else
    {
        // else threhold is < 1.0 so it is used
    
        terminalForce = 2.0f * mDesignerParams.mDpHighSpeedGasScale * mDesignerParams.mDpMass;        //) - (mRollingFrictionForce * mDesignerParams.mDpMass);
    }
    

    mDragCoeff = 2.0f* terminalForce / (topspeedmps * topspeedmps);


}



//=============================================================================
// Vehicle::IsJointAWheel
//=============================================================================
// Description: Comment
//
// Parameters:  (int jointIndex)
//
// Return:      bool 
//
//=============================================================================
bool Vehicle::IsJointAWheel(int jointIndex)
{
    if(mJointIndexToWheelMapping[jointIndex] >= 0)
    {
        return true;
    }   
    else
    {
        return false;
    }
}


//=============================================================================
// Vehicle::SetWheelCorrectionOffset
//=============================================================================
// Description: Comment
//
// Parameters:  (int jointNum, float objectSpaceYOffsetFromCurrentPosition)
//
// Return:      bool 
//
//=============================================================================
bool Vehicle::SetWheelCorrectionOffset(int jointNum, float objectSpaceYOffsetFromCurrentPosition, rmt::Vector& normalPointingAtCar, rmt::Vector& groundContactPoint)
{
    //bool bottomedOut = mWheels[mJointIndexToWheelMapping[jointNum]]->SetYOffsetFromCurrentPosition(objectSpaceYOffsetFromCurrentPosition);
    float bottomedOut = mWheels[mJointIndexToWheelMapping[jointNum]]->SetYOffsetFromCurrentPosition(objectSpaceYOffsetFromCurrentPosition);

    if(mLoco == VL_PHYSICS) // is this method even called if it's not?
    {
        mVehicleLocomotion->CompareNormalAndHeight(mJointIndexToWheelMapping[jointNum], normalPointingAtCar, groundContactPoint);
    }


    if(bottomedOut > 0.0f)
    {
        // if any wheel bottoms out we want to set this

        // currently this is just used by the parent Vehicle to do some debug rendering
        mBottomedOutThisFrame = true;

        // TODO ?
        // what to do with this value

        return true;
    }

    //return bottomedOut;
    return false;
}



//=============================================================================
// Vehicle::SetVehicleSimEnvironment
//=============================================================================
// Description: Comment
//
// Parameters:  (sim::SimEnvironment* se)
//
// Return:      void 
//
//=============================================================================
void Vehicle::SetVehicleSimEnvironment(sim::SimEnvironment* se)
{    
    mPhObj->SetSimEnvironment(se);
}


//=============================================================================
// Vehicle::GetCollisionAreaIndexAndAddSelf
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::GetCollisionAreaIndexAndAddSelf()
{
    mCollisionAreaIndex = GetWorldPhysicsManager()->GetVehicleCollisionAreaIndex();
    rAssert(mCollisionAreaIndex != -1);

    AddSelfToCollisionManager();
    
}


//=============================================================================
// Vehicle::RemoveSelfAndFreeCollisionAreaIndex
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::RemoveSelfAndFreeCollisionAreaIndex()
{
    RemoveSelfFromCollisionManager();

    GetWorldPhysicsManager()->FreeCollisionAreaIndex(mCollisionAreaIndex);
    mCollisionAreaIndex = -1;

}

//=============================================================================
// Vehicle::AddSelfToCollisionManager
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::AddSelfToCollisionManager()
{
    GetWorldPhysicsManager()->mCollisionManager->AddCollisionObject(mSimStateArticulated->GetCollisionObject(), mCollisionAreaIndex);
    GetWorldPhysicsManager()->mCollisionManager->AddCollisionObject(mGroundPlaneSimState->GetCollisionObject(), mCollisionAreaIndex);
    GetWorldPhysicsManager()->mCollisionManager->AddPair(mGroundPlaneSimState->GetCollisionObject(), mSimStateArticulated->GetCollisionObject(), mCollisionAreaIndex);
}


//=============================================================================
// Vehicle::RemoveSelfFromCollisionManager
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::RemoveSelfFromCollisionManager()
{
    rAssert(mCollisionAreaIndex != -1);
    GetWorldPhysicsManager()->mCollisionManager->RemoveCollisionObject(mSimStateArticulated->GetCollisionObject(), mCollisionAreaIndex);
    GetWorldPhysicsManager()->mCollisionManager->RemoveCollisionObject(mGroundPlaneSimState->GetCollisionObject(), mCollisionAreaIndex);
}

//=============================================================================
// Vehicle::AddToOtherCollisionArea
//=============================================================================
// Description: Comment
//
// Parameters:  (int index)
//
// Return:      void 
//
//=============================================================================
void Vehicle::AddToOtherCollisionArea(int index)
{
    GetWorldPhysicsManager()->mCollisionManager->AddCollisionObject(mSimStateArticulated->GetCollisionObject(), index);
}

//=============================================================================
// Vehicle::ReLoadDesignerParams
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::CalculateValuesBasedOnDesignerParams()
{
    // in the future, reload from file or something?
    //
    // right now, this method can just be called when a gamepad button is hit, to
    // pass down new settings (using the Marting-method in msdev) and force 
    // recalculation in the physicsvehicle and wheel as necessary

    int stophere = 1;   // break, and change values in msdev, then continue

    // 
    //mPhysicsVehicle->SetDesignerParams(&mDesignerParams);

    if(GetCheatInputSystem()->IsCheatEnabled(CHEAT_ID_HIGH_ACCELERATION))
    {
        //mDesignerParams.mDpGasScale *= 3.0f;
    }


    // TODO - reimplement as necessary here

    // account for new mass
    SetupPhysicsProperties();

    CalculateDragCoeffBasedOnTopSpeed();


    int i;
    for(i = 0; i < 4; i++)
    {
        mSuspensionRestPoints[i] = mSuspensionRestPointsFromFile[i];
        mSuspensionRestPoints[i].y += mDesignerParams.mDpSuspensionYOffset;

        mWheels[i]->SetDesignerParams(&mDesignerParams);
    }

    // reset hitpoints to full
    mHitPoints = mDesignerParams.mHitPoints;

    float health = GetCharacterSheetManager()->GetCarHealth( mCharacterSheetCarIndex );
    if( ( health >= 0.0f ) && ( health < 1.0f ) )
    {
        mHitPoints *= health;
        SyncVisualDamage( health );
    }
}


//=============================================================================
// Vehicle::TrafficSetTransform
//=============================================================================
// Description: Comment
//
// Parameters:  (rmt::Matrix &m)
//
// Return:      void 
//
//=============================================================================
void Vehicle::TrafficSetTransform(rmt::Matrix &m)
{
    // should already be under simAICtrl
    rAssert(mSimStateArticulated->GetControl() == simAICtrl);
    
    // the position in the incoming matrix is on the road surface
    //
    // need to figure out how much to bump this up along vup

    // ( I think the wheels settling a bit on the suspension will have to be 
    // handled elsewhere.. eg. suspensionjointdriver)
    
    float restHeightAboveGround = GetRestHeightAboveGround();
    //restHeightAboveGround = 2.0f;
    rmt::Vector fudge = mVehicleUp;
    
    const float hacktest = 0.1f;
    
    fudge.Scale(restHeightAboveGround - hacktest);

    rmt::Vector currentTrans = m.Row(3);
    currentTrans.Add(fudge);

    m.FillTranslate(currentTrans);


    mSimStateArticulated->SetTransform(m);
    mTransform = m;

    
}


//=============================================================================
// Vehicle::GetRestHeightAboveGround
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
float Vehicle::GetRestHeightAboveGround()
{
    // ie. height above ground for skeleton in rest pose
    //
    // assume all the wheels have same radius and same suspension point y

    // TODO - is this correct?  adequate?
    float suspensionPointToCenter = -1.0f * mSuspensionRestPoints[0].y;
    float wheelRadius = mWheels[0]->mRadius;

    return suspensionPointToCenter + wheelRadius;
}


//=============================================================================
// Vehicle::SetTransform
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Matrix* m )
//
// Return:      void 
//
//=============================================================================
void Vehicle::SetTransform( rmt::Matrix &m )
{
    if(mSimStateArticulated->GetControl() == simAICtrl)
    {
        mSimStateArticulated->SetTransform(m);

        // TODO
        // MS7 HACK
        //mSimStateArticulated->SetControl(simSimulationCtrl);
    }
    else
    {
        mSimStateArticulated->SetControl(simAICtrl);
        mSimStateArticulated->SetTransform(m);
        mSimStateArticulated->SetControl(simSimulationCtrl);
    }

    mSimStateArticulated->ResetVelocities();

    mTransform = mSimStateArticulated->GetTransform(-1);
 
    mPoseEngine->Begin(true);   // TODO - should this be true or false

    int i;
    for (i = 0; i < mPoseEngine->GetPassCount(); i++)
    {
        mPoseEngine->Advance(i, 0);
        mPoseEngine->Update(i);
    }
    mPoseEngine->End();
    mSimStateArticulated->UpdateJointState(0);

    // TODO - reset other state variables
    // TODO - need to touch pose engine here?

    
    mVelocityCM.Set(0.0f, 0.0f, 0.0f);
    mSpeed = 0.0f;
    mSpeedKmh = 0.0f;
    mPercentOfTopSpeed = 0.0f;

    mBrakeTimer = 0.0f;
    mBrakeActingAsReverse = false;

    mVehicleFacing = mTransform.Row(2);
    mVehicleUp = mTransform.Row(1);
    mVehicleTransverse = mTransform.Row(0);


    mGear = 0;  // neutral?

    // redundant, but just in case   
    for(i = 0; i < 4; i++)
    {
        mWheels[i]->Reset();
    }



    CalculateSuspensionLocationAndVelocity();

    mPhysicsLocomotion->SetTerrainIntersectCachePointsForNewTransform();

    //Only do this if the car is actually IN the DSG
    if ( mVehicleCentralIndex > -1 )
    {
        DSGUpdateAndMove();
    }
}

//=============================================================================
// Vehicle::SetPosition
//=============================================================================
// Description: Comment
//
// Parameters:  (rmt::Vector* newPos)
//
// Return:      void 
//
//=============================================================================
void Vehicle::SetPosition(rmt::Vector* newPos)
{
    rmt::Matrix m;
    m.Identity();

    m.FillTranslate(*newPos);

    SetTransform( m );
}

//
// Dumbledore here,
// This method is to be used for vehicles that get spawned at a locator position that 
// has been SNAPPED TO GROUND. Since Vehicle will assume initial position belongs
// to the car's center of mass, the car will start half-sunken into the ground..
// that is unless we can do the auto adjustment here... 
// 
void Vehicle::SetInitialPositionGroundOffsetAutoAdjust( rmt::Vector* newPos )
{
    mInitialPosition = *newPos;
    mInitialPosition.y += GetRestHeightAboveGround();
}

//=============================================================================
// Vehicle::SetInitialPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* newPos )
//
// Return:      void 
//
//=============================================================================
void Vehicle::SetInitialPosition( rmt::Vector* newPos ) 
{ 
    mInitialPosition = *newPos;
}

//=============================================================================
// Vehicle::SetResetFacingInRadians
//=============================================================================
// Description: Comment
//
// Parameters:  ( float rotation )
//
// Return:      void 
//
//=============================================================================
void Vehicle::SetResetFacingInRadians( float rotation ) 
{ 
    mResetFacingRadians = rotation; 
}

//=============================================================================
// Vehicle::Reset
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::Reset( bool resetDamage, bool clearTraffic )
{
    //SetPosition(&mInitialPosition);

    rmt::Matrix m;

    m.Identity();
    m.FillRotateXYZ( 0.0f, mResetFacingRadians, 0.0f );
    m.FillTranslate( mInitialPosition );
    int i;
    for(i = 0; i < 4; i++)
    {
        mWheels[i]->Reset();
    }

    SetTransform( m );

    /*
    // FORCE UPDATE THE AVATAR'S LAST PATH INFO 
    Avatar* avatar = GetAvatarManager()->GetAvatarForVehicle( this );
    if( avatar )
    {
        RoadManager::PathElement elem;
        RoadSegment* seg;
        float segT, roadT;
        avatar->GetLastPathInfo( elem, seg, segT, roadT );
    }
    */

    // if clearTraffic is set to true, we want to use the functionality of ResetOnSpot
    // but use the position that was filled into mInitialPosition
    if(clearTraffic)
    {
        this->ResetOnSpot(resetDamage, false);
    }
    else
    {
        


        ResetFlagsOnly( resetDamage );

        Avatar* playerAvatar = GetAvatarManager()->GetAvatarForVehicle( this );
        if ( playerAvatar )
        {
            GetSuperCamManager()->GetSCC( playerAvatar->GetPlayerId() )->DoCameraCut();
        }
    }
}


//=============================================================================
// Vehicle::ResetFlagsOnly
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::ResetFlagsOnly(bool resetDamage)
{
    mOkToDrawSelf = true;    

    mDoingBurnout = false;
    //mOkToCrashLand = false;

    mDoingRockford = false;

    mSpeedBurstTimer = 0.0f;
    mEBrakeTimer = 0.0f;
    mBuildingUpSpeedBurst = false;
    mDoSpeedBurst = false;
    mDoingJumpBoost = false;
    mBrakeLightsOn = false;
    mReverseLightsOn = false;
    mCMOffsetSetToOriginal = false;
    mStuckOnSideTimer = 0.0f;

    if( resetDamage )
    {
        mVehicleDestroyed = false;
        ResetDamageState();
        mAlreadyPlayedExplosion = false;

    }
   
   
    mDesiredDoorPosition[0] = mDesiredDoorPosition[1] = 0.0f;
    mDesiredDoorAction[0] = mDesiredDoorAction[1]  = DOORACTION_NONE;
    
    mDrawWireFrame = false;
    
    mWasAirborn = false;
    mWasAirbornTimer = 0.0f;
    
    mBottomOutSpeedMaintenance = 0.0f;

    // reset the AI pathfinding information, if we're an AI vehicle
    if( mVehicleType == VT_AI && mVehicleCentralIndex != -1 )
    {
        // if we're an AI car, dump our pathfinding data
        VehicleAI* vAI = dynamic_cast<VehicleAI*>( GetVehicleCentral()->GetVehicleController( mVehicleCentralIndex ) );
        if( vAI )
        {
            vAI->Reset();
        }
    } 
}

//=============================================================================
// Vehicle::ResetOnSpot
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::ResetOnSpot( bool resetDamage /*=true*/ , bool moveCarOntoRoad)
{

    // find road segment and put them on it
    
    // one branch or the other will fill these up
    rmt::Vector newVehiclePosition(0.0f, 0.0f, 0.0f);
    float ang = 0.0f;
    
    
    rmt::Vector currentVehiclePosition = this->rPosition();
    float radius = 100.0f; // this is the radius of query for nearest road

    RoadSegment* roadSeg = NULL;
    float dummy;

    bool useIntersection = false;
    Intersection* in = NULL;


    // We need to do something special for street races because they have race props, 
    // meaning that resetting onto the "nearest road" can take you outside the race
    // bounds. For normal race missions, we just find the closest road.
    //
    if( GetGameplayManager()->GetCurrentMission() != NULL && 
        GetGameplayManager()->GetCurrentMission()->IsRaceMission() )
    {
        RoadManager::PathElement elem;
        RoadSegment* seg = 0;
        float segT;
        float roadT;
    
        if( mVehicleType == VT_USER )
        {
            //// Use my UBER-search algorithm only when desperate times call for it.
            //RoadManager::PathfindingOptions options = RoadManager::PO_SEARCH_INTERSECTIONS | RoadManager::PO_SEARCH_ROADS;
            //RoadManager::GetInstance()->FindClosestPathElement( currentVehiclePosition, radius, options, elem, roadSeg, segT, roadT );

            GetAvatarManager()->GetAvatarForPlayer(0)->GetLastPathInfo(elem, roadSeg, segT, roadT);
            if( elem.type == RoadManager::ET_INTERSECTION )
            {
                useIntersection = true;
                in = (Intersection*) elem.elem;
            }
        }
        else 
        {
            rAssert( mVehicleType == VT_AI );
            VehicleAI* vAI = GetVehicleCentral()->GetVehicleAI( this );
            rAssert( vAI );
            
            vAI->GetLastPathInfo( elem, roadSeg, segT, roadT );
        }
    }
    else
    {
        GetIntersectManager()->FindClosestRoad( currentVehiclePosition, radius, roadSeg, dummy );
    }
    

    if( useIntersection )
    {
        rAssert( in );
        in->GetLocation( newVehiclePosition );

        // bump up position
        float h = this->GetRestHeightAboveGround();
        h += 1.0f;
        newVehiclePosition.y += h;

        ang = FacingIntoRad(mVehicleFacing);

    }
    else
    {
        if(roadSeg)
        {
            // from the road
            // want two points - centre of baseline and center of top
            
            rmt::Vector corner0, corner1, corner2, corner3;
            
            roadSeg->GetCorner(0, corner0);
            roadSeg->GetCorner(1, corner1);
            roadSeg->GetCorner(2, corner2);
            roadSeg->GetCorner(3, corner3);
                    
            rmt::Vector base = corner0;
            base.Add(corner3);
            base.Scale(0.5f);
            
            rmt::Vector top = corner1;
            top.Add(corner2);
            top.Scale(0.5f);
            
            rmt::Vector centerline = top;
            centerline.Sub(base);
            
            // we want the vehicle's new orientation to be along this vector, and the position
            // to be the projection (shortest distance) of vehicle's position onto this line
            
            rmt::Vector centerlineDir = centerline;
            
            rAssert(centerline.Magnitude() > 0.0f);
            
            centerlineDir.NormalizeSafe();
            
            rmt::Vector newVehicleFacing = centerlineDir;

            //If this is NOT SuperSprint, do this test.  Otherwise the car will
            //always face along the road.
            if ( !GetGameplayManager()->GetGameType() == GameplayManager::GT_SUPERSPRINT )
            {
                if(mVehicleFacing.DotProduct(centerlineDir) > 0.0f)
                {
                    // leave as is
                }
                else
                {
                    newVehicleFacing.Scale(-1.0f);
                }
            }

            ang = FacingIntoRad(newVehicleFacing);
            

            // for position, we just use Dusit's magical function
            
            // the line segment at which this closest point occurs
            //float FindClosestPointOnLine( const rmt::Vector& start,
            //                              const rmt::Vector& end,
            //                              const rmt::Vector& p,
            //                              rmt::Vector& closestPt );
            
            
            FindClosestPointOnLine(base, top, currentVehiclePosition, newVehiclePosition);




            // make sure there's no traffic in the way at that point
            rmt::Sphere s;
            s.centre = newVehiclePosition;
            //s.radius = 10.0f;
            s.radius = this->mWheelBase * 2.0f;
            
            TrafficManager::GetInstance()->ClearTrafficInSphere(s);
            



            // bump up position
            float h = this->GetRestHeightAboveGround();
            h += 1.0f;
            
            newVehiclePosition.y += h;
            
            
            //GetVehicleCentral()->ClearSpot(newVehiclePosition, this->mWheelBase * 2.0f, this);
            
        }
        else
        {    

            // this is the old debug version 
            //
            // literally resets on spot.
            newVehiclePosition = currentVehiclePosition;
            newVehiclePosition.y += 1.0f;
            
            ang = FacingIntoRad(mVehicleFacing);
        }
    }

    if(moveCarOntoRoad)
    {
        
        rmt::Matrix m;
        
        m.Identity();

        if ( GetGameplayManager()->GetGameType() == GameplayManager::GT_SUPERSPRINT )
        {
            if ( GetSSM()->IsTrackReversed() )
            {
                ang += rmt::PI;  //Turn, turn around
            }
        }
            
        //m.FillRotateXYZ( 0.0f, mResetFacingRadians, 0.0f );
        m.FillRotateXYZ( 0.0f, ang, 0.0f );
        
        m.FillTranslate(newVehiclePosition);
        int i;
        for(i = 0; i < 4; i++)
        {
            mWheels[i]->Reset();
        }

        SetTransform( m );
    }
       
    ResetFlagsOnly(resetDamage);
    
    Avatar* playerAvatar = GetAvatarManager()->GetAvatarForVehicle( this );
    if ( playerAvatar )
    {
        GetSuperCamManager()->GetSCC( playerAvatar->GetPlayerId() )->DoCameraCut();
    }
    
     mAlreadyCalledAutoResetOnSpot = false;
}


//=============================================================================
// Vehicle::GetFacingInRadians
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
float Vehicle::GetFacingInRadians()
{
    return FacingIntoRad(mVehicleFacing);
}


//=============================================================================
// Vehicle::FacingIntoRad
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
float Vehicle::FacingIntoRad(rmt::Vector facing)
{
    float test = rmt::ATan2(facing.x, facing.z);
    
    if(rmt::IsNan(test))
    {
        return 0.0f;
    }
    
    return test;
    //return 0.0f;
}

//=============================================================================
// Vehicle::SetGas
//=============================================================================
// Description: Comment
//
// Parameters:  (float gas)
//
// Return:      void 
//
//=============================================================================
void Vehicle::SetGas(float gas)
{    

    if(!mVehicleDestroyed && !mGasBrakeDisabled)
    {
        mGas = gas;
        //if(mGas > 0.8f)
        //{
        //    mGas = 1.0f;
        //}
    }
    else
    {
        mGas = 0.0f;
    }
}


//=============================================================================
// Vehicle::SetBrake
//=============================================================================
// Description: Comment
//
// Parameters:  (float brake)
//
// Return:      void 
//
//=============================================================================
void Vehicle::SetBrake(float brake)
{

    if(!mVehicleDestroyed && !mGasBrakeDisabled)
    {   
        // normal setting of value
               
        // the max we will set brake to is inverse of percentage of top speed
        
        float proj = mVelocityCM.DotProduct(mVehicleFacing);
        if(proj > 0.0f)
        {
            // only do all this crap if going forward, and using brake to slow down, not go in reverse
                

            float maxbrake = 1.0f - this->mPercentOfTopSpeed;
            if(maxbrake < 0.0f)
            {
                maxbrake = 0.0f;
            }
            
            if(brake > maxbrake)
            {
                brake = maxbrake;
            }
            
            if(mGas > 0.1f)
            {       
                if(brake > mGas)
                {
                    brake = mGas - 0.1f;
                }
            }
            if(brake < 0.0f)
            {
                brake = 0.0f;
            }
        }
           
    
        mBrake = brake;
    }
    else
    {
        mBrake = 0.0f;
    }
    
    /*
    if(mBrake > 0.0f && !mBrakeLightsOn)    
    {
        mGeometryVehicle->ShowBrakeLights();
        mBrakeLightsOn  = true;
    }
    
    if(mBrake == 0.0f && mBrakeLightsOn)
    {
        mGeometryVehicle->HideBrakeLights();
        mBrakeLightsOn  = false;
    }
    */
    
    if(mBrake > 0.0f && !mDontShowBrakeLights)
    {
        if(IsInReverse())
        {
            if(!mReverseLightsOn)
            {
                mGeometryVehicle->ShowReverseLights();
                mReverseLightsOn = true;
            }            
            if(mBrakeLightsOn)
            {
                mGeometryVehicle->HideBrakeLights();
                mBrakeLightsOn = false;                    
            }
            
        }
        else
        {
            if(mReverseLightsOn)
            {
                mGeometryVehicle->HideReverseLights();
                mReverseLightsOn = false;
            }
            
            if(!mBrakeLightsOn)
            {
                mGeometryVehicle->ShowBrakeLights();
                mBrakeLightsOn  = true;            
            }        
        }
        
    }
    
    if(mBrake == 0.0f || mDontShowBrakeLights)
    {
        if(mBrakeLightsOn)
        {
            mGeometryVehicle->HideBrakeLights();
            mBrakeLightsOn = false;        
        }
        if(mReverseLightsOn)
        {
            mGeometryVehicle->HideReverseLights();
            mReverseLightsOn = false;
        }         
    }
    
}


//=============================================================================
// Vehicle::SetEBrake
//=============================================================================
// Description: Comment
//
// Parameters:  (float ebrake)
//
// Return:      void 
//
//=============================================================================
void Vehicle::SetEBrake(float ebrake, float dt)
{
    const float magicEBrakeTimerLimit = 0.5f;   // seconds

    if(ebrake > 0.0f)
    {    
        //mEBrake = ebrake;
        
        if(mEBrake == 0.0f)
        {
            // ie. the button just went down, reset the timer
            mEBrakeTimer = 0.0f;    
        }        
        
        // hack for digital
        mEBrake = 1.0f;

        
    }
    else if (mEBrakeTimer < magicEBrakeTimerLimit)
    {
        // leave EBrake at last set value?
    }
    else
    {
        mEBrake = 0.0f;
        mEBrakeTimer = 0.0f;        

    }



    mEBrakeTimer += dt;        

    
    //  mEBrake = ebrake;  old implementation of this method
    
    

}


//=============================================================================
// Vehicle::SetWheelTurnAngle
//=============================================================================
// Description: Comment
//
// Parameters:  (float wheelTurnAngle, bool doNotModifyInputValue)
//
// Return:      void 
//
//=============================================================================
void Vehicle::SetWheelTurnAngle(float wheelTurnAngle, bool doNotModifyInputValue, float dt)
{

    mUnmodifiedInputWheelTurnAngle = wheelTurnAngle;  // just store for later use - things like doughnuts...

    // modify the h/w input to be nicer

    //char dbinfo[64];
    //sprintf(dbinfo, "input wheel turn angle: %.2f\n", wheelTurnAngle);    
    //DEBUGINFO_ADDSCREENTEXT( dbinfo );

    //doNotModifyInputValue = true;  //debug test
    if(doNotModifyInputValue)   // please ignore the name of the param :)
    {
        // this is a wheel
        // scale up input value
        //const float magicWheelScaleUp = 1.2f;
        //const float magicWheelScaleUp = 10.2f;
        const float magicWheelScaleUp = 2.75f;
        //const float magicWheelScaleUp = 1.5f;
        
        wheelTurnAngle *= magicWheelScaleUp;
        
        if(wheelTurnAngle < -1.0f)
        {
            wheelTurnAngle = -1.0f;
        }
        if(wheelTurnAngle > 1.0f)
        {
            wheelTurnAngle = 1.0f;
        }
    
        
    
    
    
    }
    if (1)  // plum wants high speed steering drop always
    {
        /*
        if(rmt::Fabs(wheelTurnAngle) < mSteeringInputThreshold)
        {
            wheelTurnAngle *= mSteeringPreSlope;
        }
        else
        {
            float steeringPostSlope = (1.0f - (mSteeringInputThreshold * mSteeringPreSlope)) / (1.0f - mSteeringInputThreshold);

            float pieceBelow = mSteeringInputThreshold * mSteeringPreSlope;
            float pieceAbove = (rmt::Fabs(wheelTurnAngle) - mSteeringInputThreshold) * steeringPostSlope;

            float wheelTurnAngleMag = pieceBelow + pieceAbove;
            wheelTurnAngle = wheelTurnAngleMag * rmt::Sign(wheelTurnAngle);
        }
        */
        
        // factor in sensitivity drop as speed increases
        //
        // simple slope-intercept formula
        float slope = mDesignerParams.mDpHighSpeedSteeringDrop - 1.0f;    // divided by 1.0 implied
        float yintercept = 1.0f;
        // input x is mPercentOfTopSpeed

        float value = slope * mPercentOfTopSpeed + yintercept;

        // this should still be a positive number
        // use to scale the angle (ie. formulat initialy setup for input value of 1.0)

        if(value < 0.0f)
        {
            value = 0.0f;   // should never hit this
        }

        wheelTurnAngle *= value;
    }
    
    
    
    //else
    //{
    //    float maxWheelTurnInRadians = rmt::DegToRadian(mDesignerParams.mDpMaxWheelTurnAngle);
    //    mWheelTurnAngle = wheelTurnAngle * maxWheelTurnInRadians;    
    //}

    
    // if we're below some low speed.....??? 60 kmh... have a time lag to reach the desired angle to reduce twichyness
    const float thresholdLowSpeed = 90.0f; // kmh
    const float timeAtZero = 0.3f;
    if(/*this->mSpeedKmh < thresholdLowSpeed && */ this->mVehicleType == VT_USER && !doNotModifyInputValue)
    {
        // closer to zero, the longer the lag.
                
        // at 60 it would take no time.
        
        // standard linear drop:
        //float secondsToChangeOneUnitAtCurrentSpeed = (1.0f - mSpeedKmh / thresholdLowSpeed) * timeAtZero;
        
        // parabolic drop:       
        float secondsToChangeOneUnitAtCurrentSpeed = ((1.0f - rmt::Sqr(mSpeedKmh / thresholdLowSpeed))  * timeAtZero);
        
        // new cap for plum
        if(secondsToChangeOneUnitAtCurrentSpeed < 0.15f)
        {
            secondsToChangeOneUnitAtCurrentSpeed = 0.15f;
        }
        
        rAssert(secondsToChangeOneUnitAtCurrentSpeed > 0.0f);
        if(secondsToChangeOneUnitAtCurrentSpeed > 0.0f)         // 2 lines of defense
        {
            float unitsPerSecond = 1.0f / secondsToChangeOneUnitAtCurrentSpeed;
            
            // this is the max amount we can change in this frame
            float unitsInThisFrame = unitsPerSecond * dt;   
            
            // at this point wheelTurnAngle is still our 'target', between 0 and 1
            
            // we can move a maximum of 'unitsInThisFrame' from mWheelTurnAngleInputValue (last value) towards our target, wheelTurnAngle
            
            if(wheelTurnAngle > mWheelTurnAngleInputValue)
            {
                if(wheelTurnAngle > (mWheelTurnAngleInputValue + unitsInThisFrame))
                {
                    mWheelTurnAngleInputValue += unitsInThisFrame;                
                }
                else
                {
                    mWheelTurnAngleInputValue = wheelTurnAngle;
                }
            
            
            }
            else if(wheelTurnAngle < mWheelTurnAngleInputValue)
            {
                if(wheelTurnAngle < (mWheelTurnAngleInputValue - unitsInThisFrame))
                {
                    mWheelTurnAngleInputValue -= unitsInThisFrame;
                }
                else
                {
                   mWheelTurnAngleInputValue = wheelTurnAngle;
                }
            
            }
            // else if they're == do nothing
            
        }
        
    }   
    else
    {
        mWheelTurnAngleInputValue = wheelTurnAngle;
    }
    
    
    // low speed uturn thingy    
    if(this->mSpeedKmh < 50.0f && this->mVehicleType == VT_USER)
    {
        // allow the max wheel turn angel to be higher
        // ?? try current + 10?
            
        // normal case - use limit set by designers    
        float maxWheelTurnInRadians = rmt::DegToRadian(mDesignerParams.mDpMaxWheelTurnAngle + 10.0f);        
        //mWheelTurnAngle = wheelTurnAngle * maxWheelTurnInRadians;
        mWheelTurnAngle = mWheelTurnAngleInputValue * maxWheelTurnInRadians;
       
        
    }
    else
    {
        // normal case - use limit set by designers    
        float maxWheelTurnInRadians = rmt::DegToRadian(mDesignerParams.mDpMaxWheelTurnAngle);        
        //mWheelTurnAngle = wheelTurnAngle * maxWheelTurnInRadians;
        mWheelTurnAngle = mWheelTurnAngleInputValue * maxWheelTurnInRadians;

    }
    

    int i;
    for(i = 0; i < 4; i++)
    {
        if(mWheels[i]->mSteerWheel)
        {
            mWheels[i]->mWheelTurnAngle = mWheelTurnAngle;
        }
    }
    
    //sprintf(dbinfo, "modified wheel turn angle: %.2f\n", mWheelTurnAngle);    

    //DEBUGINFO_ADDSCREENTEXT( dbinfo );

}


//=============================================================================
// Vehicle::SetWheelTurnAngleDirectlyInRadiansForDusitOnly
//=============================================================================
// Description: Comment
//
// Parameters:  (float rad)
//
// Return:      void 
//
//=============================================================================
void Vehicle::SetWheelTurnAngleDirectlyInRadiansForDusitOnly(float rad)
{
    float maxWheelTurnInRadians = rmt::DegToRadian(mDesignerParams.mDpMaxWheelTurnAngle);
    
    mWheelTurnAngle = rad;
    
    if(rad > maxWheelTurnInRadians)
    {
        mWheelTurnAngle = maxWheelTurnInRadians;    
    }
    if(rad < -maxWheelTurnInRadians)
    {
        mWheelTurnAngle = -maxWheelTurnInRadians;    
    }
    
  
    int i;
    for(i = 0; i < 4; i++)
    {
        if(mWheels[i]->mSteerWheel)
        {
            mWheels[i]->mWheelTurnAngle = mWheelTurnAngle;
        }
    }
       

}

//=============================================================================
// Vehicle::SetReverse
//=============================================================================
// Description: Comment
//
// Parameters:  (float reverse)
//
// Return:      void 
//
//=============================================================================
void Vehicle::SetReverse(float reverse)
{
    mReverse = reverse;
    if(mReverse > 0.0f)
    {
        //rAssertMsg(0, "see greg");
        //int stophere = 1;
    }
}



//=============================================================================
// Vehicle::PreSubstepUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::PreSubstepUpdate(float dt)
{      
    float dirSpeedKmh = mSpeedKmh * ( (mVelocityCM.DotProduct(mVehicleFacing) > 0.0f) ? 1.0f : -1.0f);
    float deltaKmh = dirSpeedKmh - mLastSpeedKmh;
    mAccelMss = (deltaKmh / 3600.0f) / (dt / 1000.0f);
    mLastSpeedKmh = mSpeedKmh;

    mBottomedOutThisFrame = false;

    /*
    if(mWaitingToSwitchToOutOfCar && mCollisionAreaIndex != -1)
    {   
        mOutOfCarSwitchTimer += dt;
        if(mOutOfCarSwitchTimer > 2.0f)
        {
            mOutOfCarSwitchTimer = 0.0f;
            mWaitingToSwitchToOutOfCar = false;
            
            SetOutOfCarSimState();
            
        }

    }
    */

    mVelocityCMLag = mVelocityCM;
    mPositionCMLag = this->rPosition();
    
    mVehicleLocomotion->PreSubstepUpdate();
}



//=============================================================================
// Vehicle::PreCollisionPrep
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::PreCollisionPrep(float dt, bool firstSubstep)
{
    BEGIN_PROFILE("mGeometryVehicle->Update")
    mGeometryVehicle->Update(dt);   // seems to work fine before
    END_PROFILE("mGeometryVehicle->Update")

    // new
    // TODO - how to improve the interfaces
    BEGIN_PROFILE("mVehicleLocomotion->PreCollisionPrep")
    mVehicleLocomotion->PreCollisionPrep(firstSubstep);
    END_PROFILE("mVehicleLocomotion->PreCollisionPrep")

    // used by physicslocomotion in suspension force calculation
    // but set by redbrickcollisionsolveragent - yuck
    mDamperShouldNotPullDown[0] = false;
    mDamperShouldNotPullDown[1] = false;
    mDamperShouldNotPullDown[2] = false;
    mDamperShouldNotPullDown[3] = false;

    // reset this flag
    //mHitJoint = -1;	

}


//=============================================================================
// Vehicle::SetNoDamperDownFlagOnWheel
//=============================================================================
// Description: Comment
//
// Parameters:  (int wheelIndex)
//
// Return:      void 
//
//=============================================================================
void Vehicle::SetNoDamperDownFlagOnWheel(int wheelIndex)
{
    mDamperShouldNotPullDown[wheelIndex] = true;    // just for this frame    
}


//=============================================================================
// Vehicle::CalculateSuspensionLocationAndVelocity
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::CalculateSuspensionLocationAndVelocity()
{
    // TODO - is this ok to actually change what we call world space suspension point for this frame if we're airborn???

    // do airborn calculation here?
    // only look at wheels 2,3
    
    // recall - right now this is getting called from physicslocomotion::update, after collision detection


    //if( !(mWheels[2]->mWheelInCollision) && !(mWheels[3]->mWheelInCollision))
    if( !(mWheels[2]->mWheelInCollision) || !(mWheels[3]->mWheelInCollision))
    {
        // consider this airbor
        // even if we're just cresting hill.
        mSteeringWheelsOutOfContact = true;
    }
    else
    {
        mSteeringWheelsOutOfContact = false;
    }


    // air born test
    int i;
    int count = 0;    
    for(i = 0; i < 4; i++)
    {
        if(!(mWheels[i]->mWheelInCollision))
        {
            count++;
        }
    }

    if(count > 3)
    {
        mAirBorn = true;
        mWasAirborn = true;
        mWasAirbornTimer = 0.0f;
    }
    else
    {
        mAirBorn = false;
    }






    // TODO - how 'bout flipped?
    
 
    for(i = 0; i < 4; i++)
    {

        mSuspensionWorldSpacePoints[i] = mSuspensionRestPoints[i];
        
        
        //if(1)//mSteeringWheelsOutOfContact)
        if(mSteeringWheelsOutOfContact)// || mVehicleState == VS_NORMAL)
        {
            // TODO - revisit this - I don't like it

            // ok, this was a serious fuck up
            // revisit during the tipping likelihood thing

            //mSuspensionWorldSpacePoints[i].y = mCMOffset.y;
        }

        mSuspensionWorldSpacePoints[i].Transform(mTransform);

        mSimStateArticulated->GetVelocity(mSuspensionWorldSpacePoints[i], mSuspensionPointVelocities[i]);        

    }


    // note:
    // mSteeringWheelsOutOfContact is a bit of a misnomer

    // do a test here for real airborn, all 4 wheels off, and vehicle upright
    //
    // this is the only place where mOkToCrashLand will get set to true
    //
    /*
    if( !(mWheels[0]->mWheelInCollision) && !(mWheels[1]->mWheelInCollision) && 
        !(mWheels[2]->mWheelInCollision) && !(mWheels[3]->mWheelInCollision) &&
        mVehicleUp.y > 0.0f)
    {
        mOkToCrashLand = true;
    }
    */

    // TODO - around here somewhere is the place to calculate and set a big-air event

}



//=============================================================================
// Vehicle::JumpOnHorn
//=============================================================================
// Description: Comment
//
// Parameters:  (float test)
//
// Return:      void 
//
//=============================================================================
void Vehicle::JumpOnHorn(float test)
{
    if(!(this->mAirBorn))
    {

        if(GetSimState()->GetControl() == sim::simAICtrl)
        {
            GetSimState()->SetControl(sim::simSimulationCtrl);
        }

        rmt::Vector boost = mVehicleFacing;
        boost.y += 2.0f;
        
        static float hack = 5.0f;
        //const float hack = 2.0f;
        
        boost.Scale(test * hack);
        
        rmt::Vector& linearVel = mSimStateArticulated->GetLinearVelocity();
        linearVel.Add(boost);
        
        if(GetSimState()->GetControl() == sim::simAICtrl)
        {
            GetSimState()->SetControl(sim::simSimulationCtrl);
        }
    }    
}


void Vehicle::TurboOnHorn()
{
    /*static*/ const float SECONDS_TURBO_RECHARGE = 0.5f;
    /*static*/ const float TURBO_SPEED_MPS = 10.0f;

    if( mSecondsTillNextTurbo <= 0.0f )
    {
        if( mNumTurbos > 0 )
        {
            // apply turbo speed
            if(GetSimState()->GetControl() == sim::simAICtrl)
            {
                GetSimState()->SetControl(sim::simSimulationCtrl);
            }
            rmt::Vector turbo = mVehicleFacing * TURBO_SPEED_MPS;
            rmt::Vector& linearVel = mSimStateArticulated->GetLinearVelocity();
            linearVel.Add( turbo );

            /*
            // TODO:
            // Maybe play a sound effect? Pass in the vehicle pointer? Ask Esan
            ::GetEventManager()->TriggerEvent( EVENT_USE_NITRO, this );
            */

            // decrement number of turbos
            mNumTurbos--;

            // reset mSecondsTillNextTurbo
            mSecondsTillNextTurbo = SECONDS_TURBO_RECHARGE;
        }
    }
}


//=============================================================================
// Vehicle::Update
//=============================================================================
// Description: Comment
//
// Parameters:  (float dt)
//
// Return:      void 
//
//=============================================================================
void Vehicle::Update(float dt)
{
    // Update gas, if the change in gas is significant
    
    if(mLocoSwitchedToPhysicsThisFrame)
    {
        this->PreCollisionPrep(dt, true);
    }
    mLocoSwitchedToPhysicsThisFrame = false;
    
    
    
    mDeltaGas = (mGas - mLastGas) / dt;
    mLastGas = mGas;

    // update turbo wind down time
    if( mSecondsTillNextTurbo > 0.0f )
    {
        mSecondsTillNextTurbo -= dt;
    }
    
    mNoDamageTimer -= dt;
    if(mNoDamageTimer < 0.0f)
    {
        mNoDamageTimer = 0.0f;
    }

    // TODO - before or after physics?
    //mGeometryVehicle->Update(dt);   // seems to work fine before

    // don't need this if traffic!
    //
    // actually, need a different one if this is traffic
    //CalculateSuspensionLocationAndVelocity();   // TODO - ? shouldn't this also be done before FetchingWheelTerrainCollisionInfo
    // moved to PhysicsLocomotion::PreUpdate

    // break up of work into pre and post update, outside of update, is a bit arbitrary 
    // I suppose
    
    mSimStateArticulated->StoreJointState(dt);  

    mVehicleLocomotion->PreUpdate();

    if(mVehicleType != VT_AI && GetGameplayManager()->GetGameType() != GameplayManager::GT_SUPERSPRINT)
    {
        if(mCollisionLateralResistanceDropFactor < 1.0f)
        {
            mCollisionLateralResistanceDropFactor += dt;
            if(mCollisionLateralResistanceDropFactor > 1.0f)
            {
                mCollisionLateralResistanceDropFactor = 1.0f;
            }
        }
    }
    else
    {
        mCollisionLateralResistanceDropFactor = 1.0f;     
    }

    mVehicleLocomotion->Update(dt);
    // basically - the only thing that is different after this is the mTransform,
    // whether traffic or physics
    //
    // ?? maybe traffic should just set mTransform directly
    // also set mSpeed???
    

    mVehicleLocomotion->PostUpdate();
    
       
    // TODO - trust these are normalized?
    mVehicleFacing = mTransform.Row(2);
    mVehicleUp = mTransform.Row(1);
    mVehicleTransverse = mTransform.Row(0);

    // is this gonna hurt framrate?
    mVehicleFacing.Normalize();
    mVehicleUp.Normalize();
    mVehicleTransverse.Normalize();


    // this is good
    // can leave as is for both traffic and physics
    //
    // just have to make sure that mSuspensionPointVelocities are correct
    // and it will use mSteeringWheelAngle if set.
    UpdateWheelRenderingInfo(dt);
   
    SetGeometryVehicleWheelSmokeLevel();

    if(mBurnoutLevel > 0.0f)
    {
        if ( !mDoingBurnout )
        {
            GetEventManager()->TriggerEvent( EVENT_BURNOUT );
            mDoingBurnout = true;
        }
    }
    else
    {
        if ( mDoingBurnout )
        {
            GetEventManager()->TriggerEvent( EVENT_BURNOUT_END );
            mDoingBurnout = false;
        }
    }



    mSpeedKmh = mSpeed * 3.6f;  // for watcher...
        
                       
    int i;
    for (i = 0; i < mPoseEngine->GetPassCount(); i++)
    {
        mPoseEngine->Advance(i, dt);
        mPoseEngine->Update(i);
    }

    // calculate forced door opening as a result of character getting into/out of car
    // TODO : this should probably be doen in the poseengine , but I don't yet understand 
    //    enough about  what poseengine/posedriver are actually doing to do that
    CalcDoors();

    // cap on huge impulses
    const float hugeSpeed = 200.0f; // that's about 720 kmh
    if(this->mSpeed > hugeSpeed)
    {
        // this is really fucking bad
        //rAssertMsg(0,"tell greg how you did this");
        mSimStateArticulated->ResetVelocities();
        mSimStateArticulated->GetSimulatedObject()->ResetCache();
        
        this->ResetOnSpot(false);
       // mVehicleLocomotion->PostUpdate();
        
        
    } 

    if(1)//mLoco == VL_PHYSICS)
    {      
        mSimStateArticulated->UpdateJointState(dt);
    }
 
 
    // for debugging:
    // we want the collision objects to draw in the correct places
    // TODO - want to call this here! every frame!!
    CollisionObject* collObj = mSimStateArticulated->GetCollisionObject();
    collObj->Update();

    DSGUpdateAndMove();

    //--------------------------
    // do some gear and rpm shit
    //--------------------------

    UpdateGearAndRPM();

    // Update the trigger volume.
    mpTriggerVolume->SetTransform( mTransform);
    
    mWasAirbornTimer += dt;
    if(mWasAirbornTimer > 1.0f)
    {
        mWasAirborn = false;
    }
    
    if(mBottomedOutThisFrame && mWasAirborn)
    {
        // try this here for Esan
        mWasAirborn = false;
        if( mTerrainType == TT_Road || mTerrainType == TT_Metal || mTerrainType == TT_Gravel )
        {
            GetSparkleManager()->AddBottomOut( GetPosition() );
        }
        GetEventManager()->TriggerEvent(EVENT_VEHICLE_SUSPENSION_BOTTOMED_OUT, (void*)this);
        //rDebugPrintf("vehicle bottomed out this frame \n");
        
    
    
    }  

}


//=============================================================================
// Vehicle::GetGroundY
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
float Vehicle::GetGroundY()
{
    // for James, for the shadows...

    if(this->mLoco == VL_PHYSICS)
    {
        // need this value for either case
        
        float avg = 0.0f;
        avg =   mPhysicsLocomotion->mTerrainIntersectCache[0].planePosn.y +
                mPhysicsLocomotion->mTerrainIntersectCache[1].planePosn.y +
                mPhysicsLocomotion->mTerrainIntersectCache[2].planePosn.y +
                mPhysicsLocomotion->mTerrainIntersectCache[3].planePosn.y;
            
        avg *= 0.25;           
    
        if(mAirBorn)    // all four wheels out of contact
        {
            // in this situation, give the average of the terrain intersect caches?            
            return avg;        
        }
        else
        {
            // here at least one wheel is in collision
            // so... give the average y of all the wheels that are in collision
            float wheelavg = 0.0f;
            float count = 0.0f;
            
            int i;
            for(i = 0; i < 4; i++)
            {
                if(mWheels[i]->mWheelInCollision)
                {
                    count = count + 1.0f;
                    
                    poser::Pose* pose = mPoseEngine->GetPose();
                    
                    poser::Joint* joint = pose->GetJoint(mWheelToJointIndexMapping[i]);
                    rmt::Vector trans = joint->GetWorldTranslation();
                    
                    wheelavg += ( trans.y - mWheels[ i ]->mRadius );
                }
            }
            
            if(count > 0.0f)
            {
                return wheelavg / count;                
            }
            return avg;
            
        }
    }
    else
    {
        float y = mTransform.m[3][1];
        float diff = GetRestHeightAboveGround();
        
        y -= diff;

        return y;
    }
    
    // should never get here
    return 0.0f;
}


// I'm hiding. I don't have a comment header. I'm a fugitive. I am hunted.
//
// that's ok Dusit, here you go:

//=============================================================================
// Vehicle::PostSubstepUpdate
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::PostSubstepUpdate(float dt)
{
    //-------------------
    // reset input values
    //-------------------
//    mGas = 0.0f;  // for tony, for hud
//    mBrake = 0.0f; ? safe to do this - want the IsInReverse method to work afterall...
    //mWheelTurnAngle = 0.0f;
    mReverse = 0.0f;
    //mEBrake = 0.0f;

    // skid setting

    if( mVehicleID == VehicleEnum::HUSKA )
    {
        rmt::Vector vel = mVelocityCM;
        if( ( rmt::Abs( vel.x ) + rmt::Abs( vel.z ) > 0.5f ) && ( mPhysicsLocomotion ) )
        {
            vel.y = 0.0f;
            vel.Normalize();
            // Make a sound effects call here.
            for( int wi = 0; wi < 2; ++wi )
            {
                eTerrainType tt = mPhysicsLocomotion->mTerrainIntersectCache[ wi ].mTerrainType;
                if( tt == TT_Road || tt == TT_Metal || tt == TT_Gravel )
                {
                    rmt::Vector pos = mSuspensionRestPoints[ wi ];
                    pos.y -= mWheels[ wi ]->mRadius;
                    pos.y += mWheels[ wi ]->mYOffset;
                    GetTransform().Transform( pos, &pos );
                    GetSparkleManager()->AddSparks( pos, vel, 0.1f );
                }
            }
        }
    }

    if(!mNoSkid)
    {
        //if(mSkidLevel > 0.0f)// && mVehicleType == VT_USER)
        
        if(mLoco == VL_PHYSICS)
        {        
            int i;
            if(mBurnoutLevel > 0.0f)
            {
                for(i = 0; i < 2; i++)
                {
                    if(mWheels[i]->mWheelInCollision)   // will this value still be valid here?
                    {   // need to fetch ground plane normal also
                        rAssert(mPhysicsLocomotion);
                        rmt::Vector normal = mPhysicsLocomotion->mIntersectNormalUsed[i];
                                     
                        mGeometryVehicle->SetSkidValues(i, mSkidLevel, normal, mPhysicsLocomotion->mTerrainIntersectCache[i].mTerrainType );
                        
                    }
                }
            }
            else
            {
                int numWheels = mNoFrontSkid ? 2 : 4; // We want to determine how many wheels
                // generate skids, taking advantage of the fact that wheels 0 and 1 are always the 
                // back wheels.
                for(i = 0; i < numWheels; i++)
                {
                    if(mWheels[i]->mWheelInCollision)   // will this value still be valid here?
                    {
                        rAssert(mPhysicsLocomotion);
                        rmt::Vector normal = mPhysicsLocomotion->mIntersectNormalUsed[i];
                        
                        mGeometryVehicle->SetSkidValues(i, mSkidLevel, normal, mPhysicsLocomotion->mTerrainIntersectCache[i].mTerrainType );
                    }
                }
            }   
        }
        
        
        mGeometryVehicle->UpdateSkids();

    }

    int i;
    int count = 0;
    for(i = 0; i < 4; i++)
    {
        if(mWheels[i]->mWheelInCollision)
        {
            count++;
        }
    }
    
    
    // only want to do the following two tests if we are the avatar's car
    //
    // unless, we are supersprint, then we want to do it for all cars, with no fade to black...
    if(GetAvatarManager()->GetAvatarForPlayer(0)->GetVehicle() == this ||
       GetGameplayManager()->GetGameType() ==  GameplayManager::GT_SUPERSPRINT)
    {    
        
        if(count == 0 && GetWorldPhysicsManager()->mWorldUp.DotProduct(mVehicleUp) < 0.0f)
        {
            float linear = mPastLinear.Smooth(GetSimState()->GetLinearVelocity().Magnitude());
            float angular = mPastAngular.Smooth(GetSimState()->GetAngularVelocity().Magnitude());
            
            // flipped
            if(rmt::Fabs(linear) < REST_LINEAR_TOL * 3.0f && rmt::Fabs(angular) < REST_ANGULAR_TOL && !mAlreadyCalledAutoResetOnSpot)
            {
                // put it at rest
                // reset
                
                if(GetGameplayManager()->GetGameType() ==  GameplayManager::GT_SUPERSPRINT)
                {
                    this->ResetOnSpot(false);                
                }
                else
                {
                    GetGuiSystem()->HandleMessage( GUI_MSG_MANUAL_RESET, reinterpret_cast< unsigned int >(this) );                
                }
                 
                mAlreadyCalledAutoResetOnSpot = true;
                
            }
        
        }
        
        // other on side reset test:
        const float lowspeed = 1.0f;
        if(mSpeed < lowspeed)
        {
        
            rmt::Vector up = GetWorldPhysicsManager()->mWorldUp;
            float tip = mVehicleUp.DotProduct(up);

            //float cos85 = 0.0872f;
            float test = 0.2f;
            if(tip < test)
            {            
            
                // anytime we're in here we should incrememnt the timer
                mStuckOnSideTimer += dt;
                if(mStuckOnSideTimer > 1.0f && !mAlreadyCalledAutoResetOnSpot)  // yeah for magic numbers!
                {
                    mStuckOnSideTimer = 0.0f;     /// this is done in the reset anyway, but easier to see like this
                                     
                    if(GetGameplayManager()->GetGameType() ==  GameplayManager::GT_SUPERSPRINT)
                    {
                        this->ResetOnSpot(false);                
                    }
                    else
                    {
                        GetGuiSystem()->HandleMessage( GUI_MSG_MANUAL_RESET, reinterpret_cast< unsigned int >(this) );                
                    }
                   
                    mAlreadyCalledAutoResetOnSpot = true;
                    
                }            
            }
            else
            {
                // just in case
                mStuckOnSideTimer = 0.0f;
            }
        }
        else
        {
            // not satisfied the on-side test so reset stuckonsidetimer
            mStuckOnSideTimer = 0.0f;
        }     
            
    }
    
    
    // revisit this
    //
    // with new husk system all you have to do here for all types of cars is play an explosion
    //
    // that should probably move to where the event is fired - so that we don't swap out the car and replace with husk before this?    
             
    if(mVehicleID != VehicleEnum::HUSKA)
    {
        if( mVehicleDestroyed && !mAlreadyPlayedExplosion )
        {
            // Lets detach any objects from this vehicle on explosion
            DetachCollectible( rmt::Vector( 0, 0, 0 ), true );
            if( this->mVehicleType == VT_USER || GetGameplayManager()->mIsDemo )
            {
                mDamageOutResetTimer += dt;
                if(mDamageOutResetTimer > 3.0f)
                {
                    // kaboom
                    PlayExplosionEffect();            
                    
                    mAlreadyPlayedExplosion = true; // this will get reset immediately in ResetOnSpot
            
                    if ( GetGameplayManager()->GetGameType() == GameplayManager::GT_SUPERSPRINT )
                    {
                        ResetOnSpot( true );
                    }
                    else
                    {
                        // appropriate manager should catch this and swap in husk
                        GetEventManager()->TriggerEvent(EVENT_VEHICLE_DESTROYED, (void*)this);
                    }
                }
                
            }
            else
            {
                mDamageOutResetTimer += dt;
                if( GetGameplayManager()->GetGameType() == GameplayManager::GT_NORMAL ||
                    mDamageOutResetTimer > 3.0f )
                {
                    // kaboom
                    PlayExplosionEffect();            
                    mAlreadyPlayedExplosion = true; // this will get reset immediately in ResetOnSpot
            
                    if ( GetGameplayManager()->GetGameType() == GameplayManager::GT_SUPERSPRINT )
                    {
                        ResetOnSpot( true );
                    }
                    else
                    {
                        // appropriate manager should catch this and swap in husk
                        GetEventManager()->TriggerEvent(EVENT_VEHICLE_DESTROYED, (void*)this);
                        if(mWasHitByVehicleType != VT_AI)
                        {
                            GetEventManager()->TriggerEvent(EVENT_VEHICLE_DESTROYED_BY_USER, (void*)this);
                        }

                    }
                }
            }           
        }
    
    }
    
    /*
    if(mDamageType == VDT_AI && mVehicleDestroyed && !mAlreadyPlayedExplosion)
    {
        // just kaboom and it sits there dead...... for now
        PlayExplosionEffect();
        mAlreadyPlayedExplosion = true;
            
    }
    
    if(mDamageType == VDT_TRAFFIC && mVehicleDestroyed && !mAlreadyPlayedExplosion && this->mVehicleID != VehicleEnum::HUSKA)
    {
        PlayExplosionEffect();
        mAlreadyPlayedExplosion = true;
        
        // Dusit here...
        // It seems that the way we set mDamageType to VDT_TRAFFIC doesn't necessarily 
        // imply that this is a traffic car. SwapInTrafficHusk is only safe to call if
        // the car is actually a traffic car (owned and initialized by TrafficManager)
        // So... if it's not a traffic car, we won't call SwapInTrafficHusk... It will
        // just sit there after explosions... for now... just like what's done for VDT_AI
        //
        if( this->mVehicleType == VT_TRAFFIC )
        {
            TrafficManager::GetInstance()->SwapInTrafficHusk(this);
        }
    }
    */
    

    // hmm... this might be the place to try moving the characters?
    BounceCharacters(dt);


    
}


//=============================================================================
// Vehicle::BounceCharacters
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::BounceCharacters(float dt)
{

    if(mVehicleType == VT_USER)
    {                
        Avatar* playerAvatar = GetAvatarManager()->GetAvatarForPlayer(0);
        if(sDoBounce && (playerAvatar->GetCharacter()->GetStateManager()->GetState() == CharacterAi::INCAR) && playerAvatar->GetVehicle() == this)
        {                     
            mYAccelForSeatingOffset = (mVelocityCM.y - mVelocityCMLag.y) / dt;
            // seems to hover around -1.0f to 1.0f a lot - goes up to 8 or 10 occassionally
            
            if(rmt::Fabs(mYAccelForSeatingOffset) > mBounceAccelThreshold)
            {                       
                // want to displace it opposite the accel dir, proportional to the amount?
                //static float magicShitScale = 0.25f;
                const float magicShitScale = 0.003f;
                float amountToDisplace = mYAccelForSeatingOffset * magicShitScale * -1.0f;
                if(rmt::Fabs(amountToDisplace) > mMaxBounceDisplacementPerSecond * dt)
                {
                    amountToDisplace = mMaxBounceDisplacementPerSecond * dt * rmt::Sign(amountToDisplace);
                }                             
                
                ApplyDisplacementToCharacters(amountToDisplace);
                
            }
            else
            {
                // want to move back to the middle               
                MoveCharactersTowardsRestPosition(dt);
            
            }                                  
            
        }                   
                
    }

}


//=============================================================================
// Vehicle::MoveCharactersTowardsRestPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::MoveCharactersTowardsRestPosition(float dt)
{
    // let's say, for first try, move at half max diplacment speed
    float rate = 0.5f;
    
    Avatar* playerAvatar = GetAvatarManager()->GetAvatarForPlayer(0);
    if( mpDriver && mpDriver != playerAvatar->GetCharacter() )
    {
        // there is both a driver and us
        //
        // sort of clunky to set every frame but whatever...
        mNPCRestSeatingPosition = mDriverLocation;
        mOurRestSeatingPosition = mPassengerLocation;
        
        // hackey, hackey - need to slide lisa up a little because of dress poking through car
        if(mpDriver->IsLisa())
        {
            mNPCRestSeatingPosition.y += 0.12f;
        }

        if(playerAvatar->GetCharacter()->IsLisa())
        {
            mOurRestSeatingPosition.y += 0.12f;
        }
        
        // us        
        Character* us = playerAvatar->GetCharacter();
        choreo::Puppet* ourPuppet = us->GetPuppet();
        const rmt::Vector& ourPuppetPosition = ourPuppet->GetPosition();
        rmt::Vector newOurPuppetPosition = ourPuppetPosition;
        
        // compare rest pos
        float diff = newOurPuppetPosition.y - mOurRestSeatingPosition.y;
        float maxchange = rate * dt * mMaxBounceDisplacementPerSecond;
        if(rmt::Fabs(diff) <= maxchange)
        {
            // easy - back at rest
            newOurPuppetPosition.y = mOurRestSeatingPosition.y;
            ourPuppet->SetPosition(newOurPuppetPosition);
        }
        else if(diff < 0.0f)
        {
            // add to it
            newOurPuppetPosition.y += maxchange;
            ourPuppet->SetPosition(newOurPuppetPosition);
            
        }
        else
        {
            // subtract 
            newOurPuppetPosition.y -= maxchange;
            ourPuppet->SetPosition(newOurPuppetPosition);           
            
        }                                       
            
        
        // driver    
        choreo::Puppet* npcPuppet = mpDriver->GetPuppet();
        const rmt::Vector& npcPuppetPosition = npcPuppet->GetPosition();
        rmt::Vector newNPCPuppetPosition = npcPuppetPosition;
                    
        
        diff = newNPCPuppetPosition.y - mNPCRestSeatingPosition.y;
        //maxchange = rate * dt * mMaxBounceDisplacementPerSecond;
        if(rmt::Fabs(diff) <= maxchange)
        {
            // easy - back at rest
            newNPCPuppetPosition.y = mNPCRestSeatingPosition.y;
            npcPuppet->SetPosition(newNPCPuppetPosition);
        }
        else if(diff < 0.0f)
        {
            // add to it
            newNPCPuppetPosition.y += maxchange;
            npcPuppet->SetPosition(newNPCPuppetPosition);
            
        }
        else
        {
            // subtract 
            newNPCPuppetPosition.y -= maxchange;
            npcPuppet->SetPosition(newNPCPuppetPosition);           
            
        }                                       
    }
    else
    {
        // if we're the driver... or if no driver (so we must be the driver)

        mOurRestSeatingPosition = mDriverLocation;

        if(playerAvatar->GetCharacter()->IsLisa())
        {
            mOurRestSeatingPosition.y += 0.12f;
        }
    
        // just us drivign?
        Character* us = playerAvatar->GetCharacter();
        choreo::Puppet* ourPuppet = us->GetPuppet();
        const rmt::Vector& ourPuppetPosition = ourPuppet->GetPosition();
        rmt::Vector newOurPuppetPosition = ourPuppetPosition;
        
        // compare rest pos
        float diff = newOurPuppetPosition.y - mOurRestSeatingPosition.y;
        float maxchange = rate * dt * mMaxBounceDisplacementPerSecond;
        if(rmt::Fabs(diff) <= maxchange)
        {
            // easy - back at rest
            newOurPuppetPosition.y = mOurRestSeatingPosition.y;
            ourPuppet->SetPosition(newOurPuppetPosition);
        }
        else if(diff < 0.0f)
        {
            // add to it
            newOurPuppetPosition.y += maxchange;
            ourPuppet->SetPosition(newOurPuppetPosition);
            
        }
        else
        {
            // subtract 
            newOurPuppetPosition.y -= maxchange;
            ourPuppet->SetPosition(newOurPuppetPosition);           
            
        }
    }
}


//=============================================================================
// Vehicle::ApplyDisplacementToCharacters
//=============================================================================
// Description: Comment
//
// Parameters:  (float displacement)
//
// Return:      void 
//
//=============================================================================
void Vehicle::ApplyDisplacementToCharacters(float displacement)
{
    // now either we're driving alone or we're a passenger and there's a driver
    Avatar* playerAvatar = GetAvatarManager()->GetAvatarForPlayer(0);
    if( mpDriver && mpDriver != playerAvatar->GetCharacter() )
    {
        // sort of clunky to set every frame but whatever...
        mNPCRestSeatingPosition = mDriverLocation;
        mOurRestSeatingPosition = mPassengerLocation;
    
    
        // there is both a driver and us
        Character* us = playerAvatar->GetCharacter();
        choreo::Puppet* ourPuppet = us->GetPuppet();
        const rmt::Vector& ourPuppetPosition = ourPuppet->GetPosition();
        rmt::Vector newOurPuppetPosition = ourPuppetPosition;
        
        //newOurPuppetPosition.y += 0.001f;
        newOurPuppetPosition.y += displacement;
        
        // test against limit
        if(newOurPuppetPosition.y > mOurRestSeatingPosition.y + mBounceLimit)
        {
            newOurPuppetPosition.y = mOurRestSeatingPosition.y + mBounceLimit;
        }
        if(newOurPuppetPosition.y < mOurRestSeatingPosition.y - mBounceLimit)
        {
            newOurPuppetPosition.y = mOurRestSeatingPosition.y - mBounceLimit;
        }
        
        ourPuppet->SetPosition(newOurPuppetPosition);
                                            
            
        choreo::Puppet* npcPuppet = mpDriver->GetPuppet();
        const rmt::Vector& npcPuppetPosition = npcPuppet->GetPosition();
        rmt::Vector newNPCPuppetPosition = npcPuppetPosition;
                        
        newNPCPuppetPosition.y += displacement;
        
        // test against limit
        if(newNPCPuppetPosition.y > mNPCRestSeatingPosition.y + mBounceLimit)
        {
            newNPCPuppetPosition.y = mNPCRestSeatingPosition.y + mBounceLimit;
        }
        if(newNPCPuppetPosition.y < mNPCRestSeatingPosition.y - mBounceLimit)
        {
            newNPCPuppetPosition.y = mNPCRestSeatingPosition.y - mBounceLimit;
        }
        
        npcPuppet->SetPosition(newNPCPuppetPosition);

    }
    else
    {
        // just us drivign?
        mOurRestSeatingPosition = mDriverLocation;
                
        Character* us = playerAvatar->GetCharacter();
        choreo::Puppet* ourPuppet = us->GetPuppet();
        const rmt::Vector& ourPuppetPosition = ourPuppet->GetPosition();
        rmt::Vector newOurPuppetPosition = ourPuppetPosition;
        
        //newOurPuppetPosition.y += 0.001f;
        newOurPuppetPosition.y += displacement;
    
    
        if(newOurPuppetPosition.y > mOurRestSeatingPosition.y + mBounceLimit)
        {
            newOurPuppetPosition.y = mOurRestSeatingPosition.y + mBounceLimit;
        }
        if(newOurPuppetPosition.y < mOurRestSeatingPosition.y - mBounceLimit)
        {
            newOurPuppetPosition.y = mOurRestSeatingPosition.y - mBounceLimit;
        }
        ourPuppet->SetPosition(newOurPuppetPosition);
    }
}


//=============================================================================
// Vehicle::RecordRestSeatingPositionsOnEntry
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::RecordRestSeatingPositionsOnEntry()
{
    // same logic as BounceCharacters to decide which of these we need

    return;
    
    if(mVehicleType == VT_USER)
    {                
        Avatar* playerAvatar = GetAvatarManager()->GetAvatarForPlayer(0);
        if(playerAvatar->IsInCar() && playerAvatar->GetVehicle() == this)
        {
            // now either we're driving alone or we're a passenger and there's a driver
            if(this->mpDriver)
            {
                // there is a driver
                if(this->mpDriver == playerAvatar->GetCharacter())
                {
                    // don't think we should ever hit this - the mpDriver is an NPC driver
                    int stophere = 1;
                }
                else
                {               
                    // there is both a driver and us
                    Character* us = playerAvatar->GetCharacter();
                    choreo::Puppet* ourPuppet = us->GetPuppet();
                    const rmt::Vector& ourPuppetPosition = ourPuppet->GetPosition();
                    mOurRestSeatingPosition = ourPuppetPosition;
                    
                    choreo::Puppet* npcPuppet = mpDriver->GetPuppet();
                    const rmt::Vector& npcPuppetPosition = npcPuppet->GetPosition();
                    mNPCRestSeatingPosition = npcPuppetPosition;
                                        

                }
            }
            else
            {
                // just us drivign?
                Character* us = playerAvatar->GetCharacter();
                choreo::Puppet* ourPuppet = us->GetPuppet();
                const rmt::Vector& ourPuppetPosition = ourPuppet->GetPosition();
                mOurRestSeatingPosition = ourPuppetPosition;
            

            }
        
        }
                
    }        
}


//=============================================================================
// Vehicle::RestTest
//=============================================================================
// Description: Comment
//
// Parameters:  (void)
//
// Return:      void 
//
//=============================================================================
void Vehicle::RestTest(void)
{
    // only traffic rests
    if(mVehicleType != VT_TRAFFIC)
    {
        return;
    }

    if(!GetSimState() || GetSimState()->GetControl() == sim::simAICtrl)
    {
        // already at rest
        return;
    }

    // check if smoothed velocities are below tolerance
    if((mPastLinear.Smooth(GetSimState()->GetLinearVelocity().Magnitude()) < REST_LINEAR_TOL) &&
       (mPastAngular.Smooth(GetSimState()->GetAngularVelocity().Magnitude()) < REST_ANGULAR_TOL))
    {
        // put it at rest
        // this is done inside the loco-switch
        //GetSimState()->SetControl(sim::simAICtrl);
        //GetSimState()->ResetVelocities();
        
    }
}



//=============================================================================
// Vehicle::SelfRestTest
//=============================================================================
// Description: Comment
//
// Parameters:  (void)
//
// Return:      void 
//
//=============================================================================
bool Vehicle::SelfRestTest(void)
{
    // new version of this for user and ai cars to call on themselves
    // during physicslocomotion updates so that at very low speeds they come to a complete stop

    if(!GetSimState())
    {        
        rAssert(0);
        return false;
    }


    int i;
    int count = 0;    
    for(i = 0; i < 4; i++)
    {
        if(mWheels[i]->mWheelInCollision)
        {
            count++;
        }
    }
    
    /*
    if(count == 0 && GetWorldPhysicsManager()->mWorldUp.DotProduct(mVehicleUp) < 0.0f)
    {
        // flipped
        if((mPastLinear.Smooth(GetSimState()->GetLinearVelocity().Magnitude()) < REST_LINEAR_TOL) &&
           (mPastAngular.Smooth(GetSimState()->GetAngularVelocity().Magnitude()) < REST_ANGULAR_TOL))
        {
            // put it at rest
            // reset
            
            this->ResetOnSpot(false);
            
        }
    
    }
    */
    Vehicle* playerVehicle = GetAvatarManager()->GetAvatarForPlayer(0)->GetVehicle();
    //Vehicle* playerVehicle = GetGameplayManager()->GetCurrentVehicle();

    if(mGas < 0.1f && mBrake < 0.2f && count > 2)
    {
        
        float dot = this->mVehicleUp.DotProduct(GetWorldPhysicsManager()->mWorldUp);
        
        float linear = mPastLinear.Smooth(GetSimState()->GetLinearVelocity().Magnitude());
        float angular = mPastAngular.Smooth(GetSimState()->GetAngularVelocity().Magnitude());       
        
        float ebrakefactor = 1.0f;
        if(this->mEBrake == 1.0f)
        {
            //ebrakefactor = 2.0f;
        }

        if(mUsingInCarPhysics)
        {
            if(1)//dot < 0.995f)
            {
                // steep, so more aggresive test
                //if( linear < 2.0f * REST_LINEAR_TOL * ebrakefactor && angular < 2.0f * REST_ANGULAR_TOL * ebrakefactor)
                if( linear < 3.0f * REST_LINEAR_TOL * ebrakefactor && angular < 3.0f * REST_ANGULAR_TOL * ebrakefactor)
                {
                    //GetSimState()->SetControl(sim::simAICtrl);
                    //GetSimState()->ResetVelocities();
                                    
                    
                    if(this->mVehicleType == VT_TRAFFIC && this != playerVehicle && !mCreatedByParkedCarManager)
                    {
                        GetSimState()->SetControl(sim::simAICtrl);
                        GetSimState()->ResetVelocities();
                    }                       
                    else
                    {
                        this->ZeroOutXZVelocity();
                    }
                    
                    
                    mAtRestAsFarAsTriggersAreConcerned = true;
                    return true;
                }
                
            }        
            else if( linear < REST_LINEAR_TOL * ebrakefactor && angular < REST_ANGULAR_TOL * ebrakefactor)
            {
                // put it at rest
                //GetSimState()->SetControl(sim::simAICtrl);
                //GetSimState()->ResetVelocities();
                //this->ZeroOutXZVelocity();
            
            
                if(this->mVehicleType == VT_TRAFFIC && this != playerVehicle && !mCreatedByParkedCarManager)
                {
                    GetSimState()->SetControl(sim::simAICtrl);
                    GetSimState()->ResetVelocities();
                }                       
                else
                {
                    this->ZeroOutXZVelocity();
                }
                
                    
                
                
                mAtRestAsFarAsTriggersAreConcerned = true;
                return true;
            }
        }
        else
        {
            // more aggressive test
            if( linear < 10.0f * REST_LINEAR_TOL * ebrakefactor && angular < 10.0f * REST_ANGULAR_TOL * ebrakefactor)
            {
                //GetSimState()->SetControl(sim::simAICtrl);
                //GetSimState()->ResetVelocities();
                                
                //this->ZeroOutXZVelocity();
            
            
                if(this->mVehicleType == VT_TRAFFIC && this != playerVehicle && !mCreatedByParkedCarManager)
                {
                    GetSimState()->SetControl(sim::simAICtrl);
                    GetSimState()->ResetVelocities();
                }                       
                else
                {
                    this->ZeroOutXZVelocity();
                }
                
                
                
                
                mAtRestAsFarAsTriggersAreConcerned = true;
                return true;
            }    
        
        }
    }
    else
    {
        if(0)//this->mVehicleType == VT_TRAFFIC)
        {
            GetSimState()->SetControl(sim::simSimulationCtrl);
        }
    }
    mAtRestAsFarAsTriggersAreConcerned = false;
    return false;
}



//=============================================================================
// Vehicle::ZeroOutXZVelocity
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::ZeroOutXZVelocity()
{
    rmt::Vector& linear = mSimStateArticulated->GetLinearVelocity();
    
    rmt::Vector& angular = mSimStateArticulated->GetAngularVelocity();
    
    rmt::Vector newlinear = this->mVehicleUp;
    float linearProj = linear.DotProduct(newlinear);
    
    newlinear.Scale(linearProj);
     
    linear = newlinear;
    
    
    angular.Set(0.0f, 0.0f, 0.0f);
}


//=============================================================================
// Vehicle::SetGeometryVehicleWheelSmokeLevel
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::SetGeometryVehicleWheelSmokeLevel()
{
    // just based on burnout level for now

    if(mBurnoutLevel > 0.0f)
    {
        if(this->mSpeedBurstTimer >= 2.5f)
        {
            // TODO
            // replace this with wheel flame
            // MKR - replaced. Greg, does this work ok?
            // -1 indicates apply to all wheel
            mGeometryVehicle->SetWheelSmoke( -1, ParticleEnum::eFireSpray, mBurnoutLevel);
        }
        else
        {
            // TODO - if anyone notices or complains, switch this to 
            // set on a wheel by wheel basis
            
            // for now, just base on James's overall value
            
            //TT_Road,	// Default road terrain. Also used for sidewalk. This is default. If not set, it's this.
	        //TT_Grass,	// Grass type terrain most everything else which isn't road or sidewalk.
	        //TT_Sand,	// Sand type terrain.
	        //TT_Gravel,	// Loose gravel type terrain.
	        //TT_Water,	// Water on surface type terrain.
	        //TT_Wood,	// Boardwalks, docks type terrain.
	        //TT_Metal,  // Powerplant and other structures.
            //TT_Dirt,   // Dirt type terrain.
	        //TT_NumTerrainTypes
                      
            for ( int i = 0 ; i < 2 ; i++)
            {
            
                switch( mPhysicsLocomotion->mTerrainIntersectCache[i].mTerrainType )
                {
                    case TT_Grass:            
                        mGeometryVehicle->SetWheelSmoke( i, ParticleEnum::eGrassSpray, mBurnoutLevel);                 
                        break;
                        
                    case TT_Gravel:
                    case TT_Dirt:
                    case TT_Sand:
                        mGeometryVehicle->SetWheelSmoke( i, ParticleEnum::eDirtSpray, mBurnoutLevel); 
                        break;                    
                        
                    case TT_Water:
                        mGeometryVehicle->SetWheelSmoke( i, ParticleEnum::eWaterSpray, mBurnoutLevel); 
                        break;
                        
                    default:
                        mGeometryVehicle->SetWheelSmoke( i, ParticleEnum::eSmokeSpray, mBurnoutLevel); 
                        break;
                                    
                }
            }
        }
    }
    else
    {
        mGeometryVehicle->SetWheelSmoke( -1, ParticleEnum::eNull, 0.0f);
    }

}



//=============================================================================
// Vehicle::DSGUpdateAndMove
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::DSGUpdateAndMove()
{
    /*    
    rmt::Box3D mBBox;
    rmt::Sphere mSphere;
    rmt::Vector mPosn;
    sim::SimState* mpSimStateObj;   // TODO - ever care about this?
    */

    rmt::Box3D oldBox = mBBox;

    mPosn = *((rmt::Vector*)mTransform.m[3]);     

    mBBox.low   = mPosn;
    mBBox.high  = mPosn;

    // ?
    // I think the same code should work for vehicle...
    mBBox.high += mSimStateArticulated->GetCollisionObject()->GetCollisionVolume()->mBoxSize;
    mBBox.low  -= mSimStateArticulated->GetCollisionObject()->GetCollisionVolume()->mBoxSize;

    mSphere.centre.Sub(mBBox.high,mBBox.low);
    mSphere.centre *= 0.5f;
    mSphere.centre.Add(mBBox.low);
    mSphere.radius = mSimStateArticulated->GetCollisionObject()->GetCollisionVolume()->mSphereRadius;

    
    // now move!
    GetRenderManager()->pWorldScene()->Move(oldBox, (IEntityDSG*)this);


}



//=============================================================================
// Vehicle::UpdateGearAndRPM
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::UpdateGearAndRPM()
{
    // make sure this method can work on both
    // physics and traffic updated vehicles.


    float speedAlongFacing;     

    speedAlongFacing = mVehicleFacing.DotProduct(mVelocityCM);

    // just use wheel 0

    float linearDistancePerRev = rmt::PI * mWheels[0]->mRadius * 2.0f;

    float revPerTime = speedAlongFacing / linearDistancePerRev;
    revPerTime = rmt::Fabs(revPerTime);

    // now we go backwards from this to figure out what the rpm is, 
    // and if necessary, shift gears

    // !!! TODO - deal with reverse!

    // first see if we're in neutral and just sitting there
    //static float smallRevPerTime = 0.1f;
    float smallRevPerTime = 0.1f;
    if(mGas == 0.0f && revPerTime < smallRevPerTime)
    {
        mGear = 0;
        mRPM = mBaseRPM;
        return;
    }
    
    // otherwise let's move!
    if(mGear == 0)
    {
        mGear = 1;
    } 

    if((mVehicleState == VS_NORMAL || mGas == 0) && mBurnoutLevel == 0.0f && !mDoSpeedBurst)
    {
        //--------------------------
        // determine tentative value
        //--------------------------
        float targetRPM = mGearRatios[mGear - 1] * mFinalDriveRatio * revPerTime * 60.0f;  // 60.0f to get to minutes instead of seconds
    
        
        //------------------------
        // only change by set rate
        //------------------------        
        if(targetRPM > mRPM)
        {   
            // shouldn't be able to make arbitrary changes in rpm
            mRPM += mRPMUpRate;
            if(mRPM > targetRPM)
            {
                mRPM = targetRPM;
            }
        }
        if(targetRPM < mRPM)
        {   
            // shouldn't be able to make arbitrary changes in rpm
            mRPM -= mRPMDownRate;
            if(mRPM < targetRPM)
            {
                mRPM = targetRPM;
            }
        }

        //--------------------------
        // change gears if necessary
        //--------------------------
        if(mRPM > mShiftPointHigh)
        {
            // change gears and recalculate
            mGear++;
            if(mGear > mNumGears)
            {
                mGear = mNumGears;
            }

        }
        else if(mRPM < mShiftPointLow)
        {
            mGear--;
            if(mGear < 1)
            {
                mGear = 1;      // TODO - again - deal with reverse!
            }

        }

        //-----------------------------------------
        // recalc target due to gear change - maybe
        //-----------------------------------------
        targetRPM = mGearRatios[mGear - 1] * mFinalDriveRatio * revPerTime * 60.0f;  // 60.0f to get to minutes instead of seconds

        
        //------------------------
        // only change by set rate
        //------------------------        
        if(targetRPM > mRPM)
        {   
            // shouldn't be able to make arbitrary changes in rpm
            mRPM += mRPMUpRate;
            if(mRPM > targetRPM)
            {
                mRPM = targetRPM;
            }
        }
        if(targetRPM < mRPM)
        {   
            // shouldn't be able to make arbitrary changes in rpm
            mRPM -= mRPMDownRate;
            if(mRPM < targetRPM)
            {
                mRPM = targetRPM;
            }
        }

        //------------------------
        // lock no lower than base
        //------------------------

        if(mRPM < mBaseRPM)
        {
            mRPM = mBaseRPM;
        }
    }
    else    // VS_SLIP
    {
        if(mGas > 0.0f)
        {
            const float hack = 0.1f;
            //mRPM *= 1.0f + (hack * mGas);
            
            const float hack2 = 3.1f;
            if(mBurnoutLevel > 0.0f)
            {
                mRPM += mGas * mRPMUpRate * hack2;
            }
            else
            {
                mRPM += mGas * mRPMUpRate * hack;
            }

            // gear same
        }
        if(mRPM > mShiftPointHigh)
        {
            mRPM = mShiftPointHigh;
        }
    }


}


//=============================================================================
// Vehicle::UpdateWheelRenderingInfo
//=============================================================================
// Description: Comment
//
// Parameters:  (float dt)
//
// Return:      void 
//
//=============================================================================
void Vehicle::UpdateWheelRenderingInfo(float dt)
{  
    int i;
    for(i = 0; i < 4; i++)
    {
        // setup this method such that it can be called to get the right info
        // cahced into the Wheel after either a physics or traffic update
        //
        // maybe this can be the single calculation-type method in 
        // Wheel.
        mWheels[i]->CalculateRotAngle(dt);
    }
}


//=============================================================================
// Vehicle::GetTransform
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const 
//
//=============================================================================
const rmt::Matrix& Vehicle::GetTransform()
{
    return mTransform;
}



//========================================================================
// vehicle::
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
rmt::Vector*        Vehicle::pPosition()
{
    return (rmt::Vector*)mTransform.m[3];
}
//========================================================================
// vehicle::
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
const rmt::Vector&  Vehicle::rPosition()
{
//    rAssert(false);
//    return NULL;
    return *((rmt::Vector*)mTransform.m[3]);
}

//------------------------------------------------------------------------
//=============================================================================
// Vehicle::Display
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::Display()
{
    ActivateTriggers(!(IsUnstable() || IsAirborn()) || (GetLocomotionType() == VL_TRAFFIC) || mAtRestAsFarAsTriggersAreConcerned);

    if(IS_DRAW_LONG) return;

    // not sure if these belong here?
    //p3d::stack->Push();
    //p3d::stack->Multiply(mTransform);  
 
    //return;
    
    
    //DebugDisplay();
    //return;
    
    if( !mOkToDrawSelf || !mDrawVehicle )
    {
        return;
    }

    DSG_BEGIN_PROFILE("  Vehicle::Display")
    BillboardQuadManager::Enable();
    //DisplaySimpleShadow();
    // TODO - how to set this up cleaner?

    // little debug test
    //
    //if(!(mWheels[2]->mWheelInCollision) || !(mWheels[3]->mWheelInCollision))
    if(0)//mWeebleOn)
    //if(mDrawWireFrame)
    //if(this->mAirBorn)
    {
        p3d::pddi->SetFillMode(PDDI_FILL_WIRE);        
    }


    // temp!!

    //p3d::pddi->SetFillMode(PDDI_FILL_WIRE);        

      
    mPoseEngine->End();     // copy over what we're gonna render
    mGeometryVehicle->Display();



    //p3d::stack->Pop();

    //if(!(mWheels[2]->mWheelInCollision) || !(mWheels[3]->mWheelInCollision))
    if(0)//mWeebleOn)
    //if(mDrawWireFrame)
    //if(this->mAirBorn)
    {
        p3d::pddi->SetFillMode(PDDI_FILL_SOLID);
    }
    // ugly, but just for debugging shit
    mDrawWireFrame = false;
    this->mLosingTractionDueToAccel = false;
    

    BillboardQuadManager::Disable();
    DSG_END_PROFILE("  Vehicle::Display")
}


//=============================================================================
// Vehicle::DebugDisplay
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::DebugDisplay()
{
    mSimStateArticulated->DebugDisplay(2);  // this one draws some sort of virtual centre of mass or something
    sim::DrawCollisionObject(mSimStateArticulated->GetCollisionObject());
}


//=============================================================================
// Vehicle::CarDisplay
//=============================================================================
// Description: Comment
//
// Parameters:  (bool doit)
//
// Return:      void 
//
//=============================================================================
void Vehicle::CarDisplay(bool doit)
{
    mOkToDrawSelf = doit;
}    


//=============================================================================
// Vehicle::GetSpeedKmh
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
float Vehicle::GetSpeedKmh()
{
    //return mSpeed * 3.6f;
    return mSpeedKmh;
}


float Vehicle::GetAccelMss()
{
    return mAccelMss;
}

//=============================================================================
// Vehicle::GetRPM
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
float Vehicle::GetRPM()
{
    return mRPM;
}


//=============================================================================
// Vehicle::GetSkidLevel
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
float Vehicle::GetSkidLevel()
{
    return mSkidLevel;
}


//=============================================================================
// Vehicle::GetGear
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      int 
//
//=============================================================================
int Vehicle::GetGear()
{
    // TODO - how to do the gearshift interface?

    return mGear;
}


//----------------------
// camera inteface stuff
//----------------------

//=============================================================================
// Vehicle::GetPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* position )
//
// Return:      void 
//
//=============================================================================
void Vehicle::GetPosition( rmt::Vector* position )
{
    *position = *((rmt::Vector*)mTransform.m[3]);
}


//=============================================================================
// Vehicle::GetHeading
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* heading )
//
// Return:      void 
//
//=============================================================================
void Vehicle::GetHeading( rmt::Vector* heading )
{
    *heading = mVehicleFacing;
}


//=============================================================================
// Vehicle::GetVUP
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* vup )
//
// Return:      void 
//
//=============================================================================
void Vehicle::GetVUP( rmt::Vector* vup )
{
    *vup = mVehicleUp;
}


//=============================================================================
// Vehicle::GetVelocity
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector* velocity )
//
// Return:      void 
//
//=============================================================================
void Vehicle::GetVelocity( rmt::Vector* velocity )
{
    // TODO - make sure this holds the right thing when 
    // we are being traffic locomoted
    *velocity = mVelocityCM;
}

//=============================================================================
// Vehicle::GetID
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
unsigned int Vehicle::GetID()
{
    return mVehicleID;
}

//=============================================================================
// Vehicle::IsCar
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool Vehicle::IsCar() const
{
    return true;
}


//=============================================================================
// Vehicle::IsAirborn
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool Vehicle::IsAirborn()
{    
    //return mSteeringWheelsOutOfContact;
    return mAirBorn;
}


//=============================================================================
// Vehicle::IsUnstable
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool Vehicle::IsUnstable()
{    
    // see how much it's tipped...   
    rmt::Vector up = GetWorldPhysicsManager()->mWorldUp;
    float tip = mVehicleUp.DotProduct(up);

    //float cos10 = 0.9848f;
    float cos30 = 0.866f;
    if(tip < cos30)
    {
        return true;
    }
    
    if(mAirBorn)
    {
        return true;
    }
    
    return false;
    
    // quick test
    //return mWeebleOn;
}


//=============================================================================
// Vehicle::IsSafeToUpShift
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool Vehicle::IsSafeToUpShift()
{
    // let's say if at least 1 wheel in contact with ground (ie. !airborn)
    // and the tip angle is less than 15 degrees, then ok.
    
    if(!mAirBorn)
    {
        rmt::Vector up = GetWorldPhysicsManager()->mWorldUp;
        float tip = mVehicleUp.DotProduct(up);
        
        //float cos15 = 0.9659f;
        float cos10 = 0.9848f;
        
        if( (tip > cos10) || (mVehicleFacing.DotProduct(up) <= 0.0f) )
        {
            return true;
        }
    
    }
    
    return false;
}


//=============================================================================
// Vehicle::IsQuickTurn
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool Vehicle::IsQuickTurn()
{
    // TODO
    return false;
}

//=============================================================================
// Vehicle::IsInReverse
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool Vehicle::IsInReverse()
{
    //return false;

    // first heuristic
    //
    // if the brake button is down and we're travelling backwards
    float proj = mVelocityCM.DotProduct(mVehicleFacing);

    // don't want just any slight backwards motion to trigger
    const float cos120 = -0.5f;

    if(mBrake > 0.1f && proj < cos120)
    {
        return true;
    }

    return false;
}

//=============================================================================
// Vehicle::GetTerrainIntersect
//=============================================================================
// Description: Comment
//
// Parameters:  ( rmt::Vector& pos, rmt::Vector& normal )
//
// Return:      void 
//
//=============================================================================
void Vehicle::GetTerrainIntersect( rmt::Vector& pos, rmt::Vector& normal ) const
{
    //THIS IS A LIE!
    pos = mTransform.Row(3);
    normal.Set( 0.0f, 1.0f, 0.0f );
}

//=============================================================================
// Vehicle::IsMovingBackward
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
bool Vehicle::IsMovingBackward()
{
    // this one does not require the brake input to be held down
    
    if(this->mPercentOfTopSpeed > 0.05f)
    {
        float proj = mVelocityCM.DotProduct(mVehicleFacing);
    
        const float cos120 = -0.5f;

        if(proj < cos120)
        {
            return true;
        }
    }
        
    return false;
    

}

//=============================================================================
// Vehicle::GetName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const 
//
//=============================================================================
const char* const Vehicle::GetName()
{
    return (const char*)mName;
}

const rmt::Vector& Vehicle::GetPassengerLocation( void ) const
{
    return mPassengerLocation;
}

const rmt::Vector& Vehicle::GetDriverLocation( void ) const
{
    return mDriverLocation;
}
//=============================================================================
// Vehicle::PreReactToCollision
//=============================================================================
// Description: Comment
//
// Parameters:  ( sim::SimState* pCollidedObj, sim::Collision& inCollision )
//
// Return:      bool 
//
//=============================================================================
sim::Solving_Answer Vehicle::PreReactToCollision( sim::SimState* pCollidedObj, sim::Collision& inCollision )
{
    // this is called from the generic worldcollisionsolveragentmanager

    // don't abort
    return sim::Solving_Continue;
}



//=============================================================================
// Vehicle::SetHitJoint
//=============================================================================
// Description: Comment
//
// Parameters:  (int hj)
//
// Return:      void 
//
//=============================================================================
/*
void Vehicle::SetHitJoint(int hj)
{
    // want to try and use the most "interesting" value

    if(IsAFlappingJoint(hj))
    {
        // set for sure
        mHitJoint = hj;
        return;
    }
    
    if(IsAFlappingJoint(mHitJoint))
    {
        // we already have a flapping joint set and the incoming one is not
        return;
    }

    // ok we'll use it.
    mHitJoint = hj;


}
*/



//=============================================================================
// Vehicle::ResetDamageState
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::ResetDamageState()
{
    mVehicleDestroyed = false;
    mDontShowBrakeLights = false;   
    mGeometryVehicle->SetLightsOffDueToDamage(false);
    mDamageOutResetTimer = 0.0f;
    mHitPoints = mDesignerParams.mHitPoints;

    mDesiredDoorPosition[0] = mDesiredDoorPosition[1] = 0.0f;
    mDesiredDoorAction[0] = mDesiredDoorAction[1]  = DOORACTION_NONE;

    ActivateTriggers(true);

    if (mbPlayerCar ==true)
    {
        GetCharacterSheetManager()->UpdateCarHealth( mCharacterSheetCarIndex, 1.0f );
    }

    switch(mDamageType)
    {
        //case 1:
        case VDT_USER:
        {            
            // reset flapping joints
         
            if(mHoodJoint != -1)
            {
                int index = mJointIndexToInertialJointDriverMapping[mHoodJoint];
                if(index != -1)
                {
                    mPhObj->GetJoint(mHoodJoint)->SetInvStiffness(0.0f);
                    mPhObj->GetJoint(mHoodJoint)->ResetDeformation();
                    mInertialJointDrivers[index]->SetIsEnabled(false);
                } 
            }


            if(mTrunkJoint != -1)
            {
                int index = mJointIndexToInertialJointDriverMapping[mTrunkJoint];
                if(index != -1)
                {
                    mPhObj->GetJoint(mTrunkJoint)->SetInvStiffness(0.0f);
                    mPhObj->GetJoint(mTrunkJoint)->ResetDeformation();
                    mInertialJointDrivers[index]->SetIsEnabled(false);
                } 
            }
            
            if(mDoorDJoint != -1)
            {
                int index = mJointIndexToInertialJointDriverMapping[mDoorDJoint];
                if(index != -1)
                {
                    mPhObj->GetJoint(mDoorDJoint)->SetInvStiffness(0.0f);
                    mPhObj->GetJoint(mDoorDJoint)->ResetDeformation();
                    mInertialJointDrivers[index]->SetIsEnabled(false);
                } 
            }

            
            if(mDoorPJoint != -1)
            {
                int index = mJointIndexToInertialJointDriverMapping[mDoorPJoint];
                if(index != -1)
                {
                    mPhObj->GetJoint(mDoorPJoint)->SetInvStiffness(0.0f);
                    mPhObj->GetJoint(mDoorPJoint)->ResetDeformation();
                    mInertialJointDrivers[index]->SetIsEnabled(false);
                } 
            }

            mGeometryVehicle->DamageTextureHood(false);
            mGeometryVehicle->DamageTextureTrunk(false);
            mGeometryVehicle->DamageTextureDoorD(false);
            mGeometryVehicle->DamageTextureDoorP(false);       
            mGeometryVehicle->SetEngineSmoke(ParticleEnum::eNull);

            mGeometryVehicle->HideFlappingPiece(mHoodJoint, false);     
            
        }
        break;

        //case 2:
        case VDT_AI:
        {
            mGeometryVehicle->DamageTextureHood(false);
            mGeometryVehicle->DamageTextureTrunk(false);
            mGeometryVehicle->DamageTextureDoorD(false);
            mGeometryVehicle->DamageTextureDoorP(false);       
            mGeometryVehicle->SetEngineSmoke(ParticleEnum::eNull);
        }
        break;

        //case 3:
        case VDT_TRAFFIC:
        {
            //mGeometryVehicle->DamageTextureChassis(false);
            //mGeometryVehicle->SetEngineSmoke(ParticleEnum::eNull);
            mGeometryVehicle->SetEngineSmoke(ParticleEnum::eNull);
        }
        break;

        default:
            rAssertMsg(0, "what are you doing here?");

    }

}

//=============================================================================
// Vehicle::DebugInflictDamageHood
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::DebugInflictDamageHood()
{
    //int stophere = 1;

    

    //float perc = GetVehicleLifePercentage();
    float perc = TriggerDamage(0.15f);
    
    switch(mDamageType)
    {
        //case 1: // user level - flaping joints etc...
        case VDT_USER:
        {            
            VisualDamageType1(1.0f - perc, dl_hood);
        }
        break;

        //case 2: // ai - localized damage textures but no flapping
        case VDT_AI:
        {
            VisualDamageType2(1.0f - perc, dl_hood);
        }
        break;

        //case 3: // traffic - one big poof and texture
        case VDT_TRAFFIC:
        {           
            VisualDamageType3(1.0f - perc);    
        }
        break;

        default:
            rAssertMsg(0, "what are you doing here?");
    }


}

//=============================================================================
// Vehicle::DebugInflictDamageBack
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::DebugInflictDamageBack()
{

    //TriggerDamage(0.15f);
    //float perc = GetVehicleLifePercentage();


    //float perc = GetVehicleLifePercentage();
    float perc = TriggerDamage(0.15f);

    
    switch(mDamageType)
    {
        //case 1: // user level - flaping joints etc...
        case VDT_USER:
        {            
            VisualDamageType1(1.0f - perc, dl_trunk);
        }
        break;

        //case 2: // ai - localized damage textures but no flapping
        case VDT_AI:
        {
            VisualDamageType2(1.0f - perc, dl_trunk);
        }
        break;

        //case 3: // traffic - one big poof and texture
        case VDT_TRAFFIC:
        {           
            VisualDamageType3(1.0f - perc);    
        }
        break;

        default:
            rAssertMsg(0, "what are you doing here?");
    }
}

//============================================================================= 
// Vehicle::DebugInflictDamageDriverSide
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::DebugInflictDamageDriverSide()
{

    //TriggerDamage(0.15f);
    //float perc = GetVehicleLifePercentage();
    
    
    //float perc = GetVehicleLifePercentage();
    float perc = TriggerDamage(0.15f);

    
    switch(mDamageType)
    {
        //Case 1: // user level - flaping joints etc...
        case VDT_USER:
        {            
            VisualDamageType1(1.0f - perc, dl_driverside);
        }
        break;

        //case 2: // ai - localized damage textures but no flapping
        case VDT_AI:
        {
            VisualDamageType2(1.0f - perc, dl_driverside);
        }
        break;

        //case 3: // traffic - one big poof and texture
        case VDT_TRAFFIC:
        {           
            VisualDamageType3(1.0f - perc);    
        }
        break;

        default:
            rAssertMsg(0, "what are you doing here?");
    }

}

//=============================================================================
// Vehicle::DebugInflictDamagePassengerSide
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::DebugInflictDamagePassengerSide()
{
    //TriggerDamage(0.15f);

    //float perc = GetVehicleLifePercentage();
    
    
    //float perc = GetVehicleLifePercentage();
    float perc = TriggerDamage(0.15f);

    
    switch(mDamageType)
    {
        //case 1: // user level - flaping joints etc...
        case VDT_USER:
        {            
            VisualDamageType1(1.0f - perc, dl_passengerside);
        }
        break;

        //case 2: // ai - localized damage textures but no flapping
        case VDT_AI:
        {
            VisualDamageType2(1.0f - perc, dl_passengerside);
        }
        break;

        //case 3: // traffic - one big poof and texture
        case VDT_TRAFFIC:
        {           
            VisualDamageType3(1.0f - perc);    
        }
        break;

        default:
            rAssertMsg(0, "what are you doing here?");
    }

}

//=============================================================================
// Vehicle::PostReactToCollision
//=============================================================================
// Description: Comment
//
// Parameters:  ( sim::SimState* pCollidedObj, sim::Collision& inCollision )
//
// Return:      sim
//
//=============================================================================
sim::Solving_Answer Vehicle::PostReactToCollision(rmt::Vector& impulse, sim::Collision& inCollision)
{

    // this is called from the generic worldcollisionsolveragentmanager

    // don't abort

    sim::CollisionObject* collObjA = inCollision.mCollisionObjectA;
    sim::CollisionObject* collObjB = inCollision.mCollisionObjectB;
    
    sim::SimState* simStateA = collObjA->GetSimState();
    sim::SimState* simStateB = collObjB->GetSimState();

    
    float impulseMagnitude = impulse.Magnitude();

    const float maxIntensity = 100000.0f;   // empircally determined

    // Check to see if the impact was strong enough to detach any attached collectibles.
    if ( impulseMagnitude > mForceToDetachCollectible )
    {
        rmt::Vector velocity = GetSimState()->GetLinearVelocity();
        DetachCollectible( velocity );
    }

    float normalizedMagnitude = impulseMagnitude / maxIntensity;
    
        
    if(simStateA->mAIRefIndex == PhysicsAIRef::redBrickVehicle && simStateB->mAIRefIndex == PhysicsAIRef::redBrickVehicle)
    {
        // stricter test
        if(normalizedMagnitude > 0.8f)
        {
            return sim::Solving_Aborted;
        }                   
        else if( (  ((Vehicle*)(simStateA->mAIRefPointer))->mVehicleID == VehicleEnum::BART_V && ((Vehicle*)(simStateB->mAIRefPointer))->mVehicleID == VehicleEnum::CKLIMO) ||
                 (  ((Vehicle*)(simStateA->mAIRefPointer))->mVehicleID == VehicleEnum::CKLIMO && ((Vehicle*)(simStateB->mAIRefPointer))->mVehicleID == VehicleEnum::BART_V) )
                 
        {
            if(normalizedMagnitude > 0.5f)                    
            {
                return sim::Solving_Aborted;
            }                 
        }        
    }

    if(normalizedMagnitude > 1.0f)
    {
        rDebugPrintf("Yikes!!! Enormous impulse!\n");
        //impulse.x /= normalizedMagnitude * 2;
        //impulse.y /= normalizedMagnitude * 2;
        //impulse.z /= normalizedMagnitude * 2;
        if(normalizedMagnitude > 2.0f)
        {
            return sim::Solving_Aborted;
        }
   
        
        normalizedMagnitude = 1.0f;
    }
    if(normalizedMagnitude < 0.0f)
    {
        rAssert(0);
    }


	if(mVehicleID == VehicleEnum::HUSKA)
    {
        normalizedMagnitude = 0.2f;
    }
    
    if(this->mVehicleID == VehicleEnum::DUNE_V)
    {
        if( simStateA->mAIRefIndex == PhysicsAIRef::redBrickPhizFence ||
            simStateB->mAIRefIndex == PhysicsAIRef::redBrickPhizFence)
            
        {
            // r/c car hitting a fence
            //? scale up impulse?
            
            impulse.Scale(2.0f);
        }
    }


    //Rumble the controller
    RumbleCollision rc;
    rc.normalizedForce = normalizedMagnitude;
    rc.vehicle = this;
    rc.point = inCollision.GetPositionA();  //This could be either.

    GetEventManager()->TriggerEvent( EVENT_RUMBLE_COLLISION, &rc );

    if(simStateA->mAIRefIndex == PhysicsAIRef::redBrickVehicle && simStateB->mAIRefIndex == PhysicsAIRef::redBrickVehicle)
    {        
        TestWhoHitWhom( simStateA, simStateB, normalizedMagnitude, inCollision );
        if(mVehicleType != VT_USER)
        {
            DusitsStunTest(normalizedMagnitude);
        }
        // Vehicle - vehicle collision
        // we want to emit paint flicks of the appropriate colour
        if ( mGeometryVehicle->HasVehicleColour() )
        {
            tColour vehicleColour = mGeometryVehicle->GetVehicleColour();
            float strength = impulseMagnitude * ( 1.0f / 5000.0f );
            float velocityScale = strength;
            const rmt::Vector& position = inCollision.GetPositionA();
            rmt::Vector velocity = mVelocityCM;
            if ( velocity.MagnitudeSqr() > 0.01f )
            {
                velocity.y = 0.0f;
                velocity.Normalize();
                velocity.Scale( velocityScale );
            }
            else
            {
                velocity = rmt::Vector(0,0,0);
            }

            GetSparkleManager()->AddPaintChips( position, velocity, vehicleColour, strength );
        }

        Avatar* playerAvatar = GetAvatarManager()->GetAvatarForPlayer(0);
        if(playerAvatar->GetVehicle() == this)
        {
            CarOnCarCollisionEventData data;

            //data.vehicle shall be the other vehicle.
            if ( simStateA->mAIRefPointer == this )
            {
                data.vehicle = ((Vehicle*)(simStateB->mAIRefPointer));
            }
            else
            {
                data.vehicle = ((Vehicle*)(simStateA->mAIRefPointer));
            }
            data.force = normalizedMagnitude;
            data.collision = &inCollision;
            GetEventManager()->TriggerEvent( EVENT_VEHICLE_VEHICLE_COLLISION, &data );
        }       
    }
    
   	if(mVehicleType == VT_USER)
	{
	    SparksTest(impulseMagnitude, inCollision);  // should this also be wrapped in mUserDrivingCar??
    } 
    
    //which one are we?
    bool thisIsA = true;

    if(simStateA->mAIRefPointer == this)
    {
        // ok
    }
    else
    {
        thisIsA = false;
    }   
    
    // try movign this down so that husks hitting cars and statics will still make sound
    //if(mVehicleID == VehicleEnum::HUSKA)
    //{
    //    return sim::Solving_Continue;
    //}
    
    // new debug test just for traffic collision
    bool oneTapTrafficDeath = false;
        
    
    if( GetCheatInputSystem()->IsCheatEnabled(CHEAT_ID_ONE_TAP_TRAFFIC_DEATH) )
    {
        
        if(simStateA->mAIRefIndex == PhysicsAIRef::redBrickVehicle && simStateB->mAIRefIndex == PhysicsAIRef::redBrickVehicle)
        {
            if(thisIsA)
            {
                if( (((Vehicle*)(simStateA->mAIRefPointer))->mVehicleType == VT_TRAFFIC || ((Vehicle*)(simStateA->mAIRefPointer))->mVehicleType == VT_AI) &&
                    ((Vehicle*)(simStateB->mAIRefPointer))->mVehicleType == VT_USER)
                    {
                        int stophere = 1;
                        oneTapTrafficDeath = true;
                        
                    }
            }
            else
            {
                if( (((Vehicle*)(simStateB->mAIRefPointer))->mVehicleType == VT_TRAFFIC || ((Vehicle*)(simStateB->mAIRefPointer))->mVehicleType == VT_AI) &&
                    ((Vehicle*)(simStateA->mAIRefPointer))->mVehicleType == VT_USER)
                
                    {
                        int stophere = 1;
                        oneTapTrafficDeath = true;
                    }
            }
        }    
    }
    
    // This is the "minimum damage threshold" for applying ANY damage
	// - think of the number as a percentage, i.e., any hit less than .1 (10 &) intensity
	//   won't cause any damage at all
	const float minDamageThreshold = 0.06f;
	
	
	
	if(!oneTapTrafficDeath)
	{
        if(normalizedMagnitude < minDamageThreshold)
        {
            //#ifdef RAD_DEBUG
            //char buffy[128];
            //sprintf(buffy, "normalizedMagnitude %.4f\n", normalizedMagnitude);
            //rDebugPrintf(buffy);
            //#endif
            
            return sim::Solving_Continue;
        }
    }    
        
        
    
    
    
    //Greg says to do this.  Won't work in 2-player.
    if (mUserDrivingCar)
    {
        CameraShakeTest(impulseMagnitude, inCollision);     // pass struct, by reference, for convenience
                                                            // pass in impulseMagnitude because we don't want to compute it twice.  
    }	
	

	
    
    SoundCollisionData soundData( normalizedMagnitude,
                                  static_cast<CollisionEntityDSG*>(simStateA->mAIRefPointer),
                                  static_cast<CollisionEntityDSG*>(simStateB->mAIRefPointer) );
    GetEventManager()->TriggerEvent( EVENT_COLLISION, &soundData );


    if(mVehicleID == VehicleEnum::HUSKA)
    {
        return sim::Solving_Continue;
    }
    


  // before further testing, see if this is a car that hit it's own ground plane
    // if so, abort
    
    Avatar* playerAvatar = GetAvatarManager()->GetAvatarForPlayer(0);
    
    if(GetGameplayManager()->GetGameType() != GameplayManager::GT_SUPERSPRINT && playerAvatar->GetVehicle() == this)
    {
        const float percentageOfSpeedToMaintain = 0.85f; // plum, change this number
        
        if(thisIsA)
        {
            if(simStateB->mAIRefIndex == PhysicsAIRef::redBrickPhizVehicleGroundPlane && this->mGas > 0.0f)
            {
                if(mBottomOutSpeedMaintenance == 0.0f)
                {
                    // not set
                    mBottomOutSpeedMaintenance = this->mSpeed * percentageOfSpeedToMaintain;                               
                }
                else
                {
                    // it is set
                    //
                    // so.... maintain it!
                    if(mBottomOutSpeedMaintenance > (this->mDesignerParams.mDpTopSpeedKmh / 3.6f) * 0.6f)
                    {
                        rmt::Vector& linearVel = mSimStateArticulated->GetLinearVelocity();
                     
                        float proj = linearVel.DotProduct(this->mVehicleFacing);
                        if(proj < mBottomOutSpeedMaintenance)
                        {
                            float diff = mBottomOutSpeedMaintenance - proj;
                            rmt::Vector boost = mVehicleFacing;
                            boost.Scale(diff);
                            
                            linearVel.Add(boost);                        
                        
                        }   
                        
                        
                    }
                }
            
                return sim::Solving_Continue;               
            }
            else
            {
                // didn't hit our ground plane this frame so make sure the maintenance speed is reset
                mBottomOutSpeedMaintenance = 0.0f;
            }
        }
        else
        {
            if(simStateA->mAIRefIndex == PhysicsAIRef::redBrickPhizVehicleGroundPlane  && this->mGas > 0.0f)
            {        
            
                if(mBottomOutSpeedMaintenance == 0.0f)
                {
                    // not set
                    mBottomOutSpeedMaintenance = this->mSpeed * percentageOfSpeedToMaintain;                                
                }
                else
                {
                    // it is set
                    //
                    // so.... maintain it!
                    
                    
                    if(mBottomOutSpeedMaintenance > (this->mDesignerParams.mDpTopSpeedKmh / 3.6f) * 0.6f)
                    {
                        rmt::Vector& linearVel = mSimStateArticulated->GetLinearVelocity();
                     
                        float proj = linearVel.DotProduct(this->mVehicleFacing);
                        if(proj < mBottomOutSpeedMaintenance)
                        {
                            float diff = mBottomOutSpeedMaintenance - proj;
                            rmt::Vector boost = mVehicleFacing;
                            boost.Scale(diff);
                            
                            linearVel.Add(boost);                        
                        
                        }   
                        
                        
                    }
                    
                    
                }
            
            
                return sim::Solving_Continue;               
            }
            else
            {
                // didn't hit our ground plane this frame so make sure the maintenance speed is reset
                mBottomOutSpeedMaintenance = 0.0f;        
            }
        
        }   
    }






    bool inflictDamage = true;


    if(simStateA->mAIRefIndex == PhysicsAIRef::redBrickVehicle && simStateB->mAIRefIndex == PhysicsAIRef::redBrickVehicle)
    {
        
        // lateral resistance drop

        if(mVehicleType != VT_USER)
        {
            rmt::Vector otherFacing;
            if(thisIsA)
            {
                otherFacing = ((Vehicle*)(simStateB->mAIRefPointer))->mVehicleFacing;
            }
            else
            {
                otherFacing = ((Vehicle*)(simStateA->mAIRefPointer))->mVehicleFacing;
            }

            float dp = mVehicleFacing.DotProduct(otherFacing);
            if(dp < 0.1f)
            {
                dp = 0.1f;
            }

            mCollisionLateralResistanceDropFactor = dp;
        
        
            // exaggerated hit results  - note the test for ! VT_USER ain't gonna help here
            /*
            rmt::Vector funImpulse = impulse;
            funImpulse.NormalizeSafe();
            funImpulse.y += 0.7f;
            
            funImpulse.Scale(impulseMagnitude);
            
            impulse = funImpulse;
            */
            
        }
    
    
        bool carOnCarDamage = CarOnCarDamageLogic(thisIsA, simStateA, simStateB);
        
        inflictDamage &= carOnCarDamage;


    }



    
    if(GetCheatInputSystem()->IsCheatEnabled(CHEAT_ID_INVINCIBLE_CAR))
    {
        if(GetAvatarManager()->GetAvatarForPlayer(0)->GetVehicle() == this)
        {
            return sim::Solving_Continue;   
        }
    }

    // time for damage inc.       
    
    // debug cheat for the testers...
    
    if(GetCheatInputSystem()->IsCheatEnabled(CHEAT_ID_FULL_DAMAGE_TO_CAR) && mVehicleType == VT_USER)
    {
        // debug cheat
     
        // note  - don't actually decrememtn hitpoints, just draw all damaged.
     
        switch(mDamageType)
        {
            //case 1: // user level - flaping joints etc...
            case VDT_USER:
            {
                DamageLocation dl = TranslateCollisionIntoLocation(inCollision);            
                VisualDamageType1(0.995f, dl);    // any point in this actually taking in a parameter

                

            }
            break;

            //case 2: // ai - localized damage textures but no flapping
            case VDT_AI:
            {
                DamageLocation dl = TranslateCollisionIntoLocation(inCollision);            
                VisualDamageType2(0.995f, dl);    // any point in this actually taking in a parameter

            }
            break;

            //case 3: // traffic - one big poof and texture
            case VDT_TRAFFIC:
            {           
                VisualDamageType3(0.995f);    // any point in this actually taking in a parameter
            }
            break;

            default:
                rAssertMsg(0, "what are you doing here?");

        }     
        
    
    }
    // normal case
    else if((mVehicleCanSustainDamage && inflictDamage) || oneTapTrafficDeath)// && this->mVehicleType != VT_TRAFFIC)  // todo - definately need to reassess the traffic test here
    {
        if(oneTapTrafficDeath)
        {
            this->TriggerDamage(100.0f);
        }    
        else if(mNoDamageTimer == 0.0f)
        {
            if( simStateA->mAIRefIndex == PhysicsAIRef::redBrickVehicle && 
                simStateB->mAIRefIndex == PhysicsAIRef::redBrickVehicle )
            {
                // The hitter sustains less damage, the hit sustains more
                if( !mWasHitByVehicle )
                {
                    normalizedMagnitude *= 0.5f; // I was the hitter! Yes!
                }
            }
            SwitchOnDamageTypeAndApply(normalizedMagnitude, inCollision);
        }
    }   


    // test
    //static float fun = 2.0f;
    //impulse.Scale(fun);
    
    return sim::Solving_Continue;   
    
}

//=============================================================================
// Vehicle::DusitsStunTest
//=============================================================================
// Description: Comment
//
// Parameters:  (float normalizedMagnitude)
//
// Return:      void 
//
//=============================================================================
void Vehicle::DusitsStunTest(float normalizedMagnitude)
{
    mCollidedWithVehicle = true;
    /*
    const float AI_VEHICLE_STUNNED_IMPACT_THRESHOLD = 0.01f;
	if( normalizedMagnitude > AI_VEHICLE_STUNNED_IMPACT_THRESHOLD )
    {
        mCollidedWithVehicle = true;
    }
    */
}
void Vehicle::TestWhoHitWhom( sim::SimState* simA, sim::SimState* simB, float normalizedMagnitude, const sim::Collision& inCollision )
{
    rAssert( simA->mAIRefIndex == PhysicsAIRef::redBrickVehicle && 
             simB->mAIRefIndex == PhysicsAIRef::redBrickVehicle );

    sim::Collision simCollision = inCollision;
    rAssert( rmt::Epsilon( simCollision.mNormal.MagnitudeSqr(),1.0f,0.0005f ) );

    // Want to determine which one I am.
    // We also want collision normal to point from him to me.. 
    // But since collision normal for all sim::Collision always points from B to A,
    // if he is simState A, then invert the collision normal.

    Vehicle* him = NULL;
    if( simA->mAIRefPointer == this )
    {
        him = (Vehicle*)simB->mAIRefPointer;
    }
    else
    {
        him = (Vehicle*)simA->mAIRefPointer;
        simCollision.mNormal.Scale( -1.0f );
    }

    rmt::Vector myVel;
    GetVelocity( &myVel );

    bool gotHitByHim = false;
    float velThreshold = 0.005f;

    // If one of the speeds is near zero, it's obvious who hit whom... 
    if( mSpeed < velThreshold )
    {
        gotHitByHim = true;
    }
    else if( him->mSpeed < velThreshold )
    {
        gotHitByHim = false;
    }
    else
    {
        // Deal with the ambiguous case of who hit whom, when 
        // both velocities are > zero. Sooo...
        // The collision normal is always pointing from him to me and is 
        // at this point NORMALIZED. We steal this and invert the vector
        // to get the heading vector from US to HIM. 
        rmt::Vector toHim = simCollision.mNormal * -1.0f;
		rAssert( rmt::Epsilon( toHim.MagnitudeSqr(), 1.0f, 0.001f ) );

        // Now... if my velocity vector is NOT pointing in "more or less" 
        // the same direction as my vector to him, then I'm not the hitter, 
        // so I got hit by him...
        rmt::Vector myNormalizedVel = myVel / mSpeed;
		rAssert( rmt::Epsilon( myNormalizedVel.MagnitudeSqr(), 1.0f, 0.001f ) );

        const float cosAlphaTest = 0.8660254f;  //approx cos30 (in either directions)
        if( myNormalizedVel.Dot( toHim ) < cosAlphaTest ) // angle greater than cosalpha
        {
            gotHitByHim = true; // we're not the hitter, so we're the hit
        }
        else
        {
			// ok, we're the hitter... BUT there's a special case for when 
			// we're in a head-on collision with the other car. If we detect
			// this case we don't want to set both as the hitter... we want 
			// each party to take full damage.... 

			rmt::Vector hisVel;
			him->GetVelocity( &hisVel );
			rmt::Vector hisNormalizedVel = hisVel / him->mSpeed;
			rAssert( rmt::Epsilon( hisNormalizedVel.MagnitudeSqr(), 1.0f, 0.001f ) );

			rmt::Vector toMe = toHim * -1.0f;
			rAssert( rmt::Epsilon( toMe.MagnitudeSqr(), 1.0f, 0.001f ) );

			if( hisNormalizedVel.Dot( toMe ) < cosAlphaTest )
			{
				// angle greater than tolerance, so he's not headed at me
				// therefore not a head-on collision. So do the setting
				// normally.
				gotHitByHim = false; // we're the hitter
			}
			else
			{
				// we're the hit (because he's coming straight at us with some 
				// tangible velocity.)
				gotHitByHim = true; 
			}
        }

    }

    // ok we figured out that ya we got hit, so store the appropriate data
    if( gotHitByHim )
    {
        // Do special transit to slip for all vehicles
        // NOTE: If don't want player vehicle to transit to slip from impact,
        //       just gotta put in a check here that we're not player vehicle
        if( normalizedMagnitude > 0.035f )
        {
            mVehicleState = VS_SLIP;
        }
        mWasHitByVehicle = true;
        mWasHitByVehicleType = him->mVehicleType;
        mNormalizedMagnitudeOfVehicleHit = normalizedMagnitude;
        mSwerveNormal = simCollision.mNormal;
    }
}


//=============================================================================
// Vehicle::SwitchOnDamageTypeAndApply
//=============================================================================
// Description: Comment
//
// Parameters:  (float normalizedMagnitude)
//
// Return:      void 
//
//=============================================================================
void Vehicle::SwitchOnDamageTypeAndApply(float normalizedMagnitude, sim::Collision& inCollision)
{
    
    // first just decrement the hit points
    //TriggerDamage(normalizedMagnitude);

    // make sure appropriate effects level is playing

    //float perc = GetVehicleLifePercentage();
    float perc = TriggerDamage(normalizedMagnitude);
    
    switch(mDamageType)
    {
        //case 1: // user level - flaping joints etc...
        case VDT_USER:
        {
            DamageLocation dl = TranslateCollisionIntoLocation(inCollision);            
            VisualDamageType1(1.0f - perc, dl);    // any point in this actually taking in a parameter

            

        }
        break;

        //case 2: // ai - localized damage textures but no flapping
        case VDT_AI:
        {
            DamageLocation dl = TranslateCollisionIntoLocation(inCollision);            
            VisualDamageType2(1.0f - perc, dl);    // any point in this actually taking in a parameter

        }
        break;
        
        case VDT_TRAFFIC:
        {                 
            VisualDamageType3(1.0f - perc);    // any point in this actually taking in a parameter
        }
        break;

        default:
            rAssertMsg(0, "what are you doing here?");

    }
    


}


//=============================================================================
// Vehicle::CarOnCarDamageLogic
//=============================================================================
// Description: Comment
//
// Parameters:  ( sim::SimState* simStateA,  sim::SimState* simStateB)
//
// Return:      bool 
//
//=============================================================================
bool Vehicle::CarOnCarDamageLogic(bool thisIsA, sim::SimState* simStateA,  sim::SimState* simStateB)
{
    
    if(this->mIsADestroyObjective)
    {
        if(thisIsA)
        {
            if( ((Vehicle*)(simStateB->mAIRefPointer))->mVehicleType == VT_USER)
            {
                // destroy objective hit by user car so it should take damage!
                return true;
            }
            else
            {
                //inflictDamage = false;
                return false;
            }
        }
        else
        {
            if( ((Vehicle*)(simStateA->mAIRefPointer))->mVehicleType == VT_USER)
            {
                // destroy objective hit by user car so it should take damage
                return true;
            }
            else
            {
                //inflictDamage = false;
                return false;
            }
        }
        

    }
    else if(this->mVehicleType == VT_USER)
    {
    
        // I want to know if this is a dump mission - if so, I shouldn't take damage....
        
    
    
        if(thisIsA)
        {
            if( ((Vehicle*)(simStateB->mAIRefPointer))->mIsADestroyObjective)
            {
                // user car hitting a destroy objective so it should not take damage
                //inflictDamage = false;
                
                //return false;
                
                // test - April 8, 2003
                // even during a destroy mission, the user will take damage when hitting the destory objective
                // makes stronger cars more important
                return true;
                
            }
        }
        else
        {
            if( ((Vehicle*)(simStateA->mAIRefPointer))->mIsADestroyObjective)
            {
                // destroy objective hit by user car so it should take damage
                //inflictDamage = false;
                
                
                //return false;
                // see note above
                return true;
            }
        }

    }   

    return true;
}


//=============================================================================
// Vehicle::CameraShakeTest
//=============================================================================
// Description: Comment
//
// Parameters:  (float impulseMagnitude, sim::Collision& inCollision)
//
// Return:      void 
//
//=============================================================================
void Vehicle::CameraShakeTest(float impulseMagnitude, sim::Collision& inCollision)
{

    //TODO: Greg can you make this more reliable?  Should I take into account
    //the mass of the vehicle?
    //Fudge some magnitude calc to send to the shaker.
    
    // greg responds: the mass is taken into account in the size of the impulse - I think...?
    
    const float cameraShakeThreshold = 19000.0f;
    const float maxIntensity = 100000.0f;   // copied from PostReactToCollision, but doesn't have to be the same
    
    if(impulseMagnitude > cameraShakeThreshold)
    {
        ShakeEventData shakeData;
        shakeData.playerID = 0;  //Hack...
        shakeData.force = impulseMagnitude / maxIntensity;

        rmt::Vector pointOnOtherObject;    

        sim::CollisionObject* collObjA = inCollision.mCollisionObjectA;
        sim::CollisionObject* collObjB = inCollision.mCollisionObjectB;
    
        sim::SimState* simStateA = collObjA->GetSimState();
        sim::SimState* simStateB = collObjB->GetSimState();

        if(simStateA == mSimStateArticulated)
        {
            pointOnOtherObject = inCollision.GetPositionB();
        }
        else if(simStateB == mSimStateArticulated)
        {
            pointOnOtherObject = inCollision.GetPositionA();    
        }
        else
        {
            rAssertMsg(0, "Ask Greg what went wrong!");
        }

        rmt::Vector contactPointToCenter;
        contactPointToCenter.Sub(this->rPosition(), pointOnOtherObject);
        contactPointToCenter.NormalizeSafe();

        shakeData.direction = contactPointToCenter;

        GetEventManager()->TriggerEvent( EVENT_CAMERA_SHAKE, (void*)(&shakeData) );
    }


}

//=============================================================================
// Vehicle::SparksTest
//=============================================================================
// Description: Comment
//
// Parameters:  (float impulseMagnitude, sim::Collision& inCollision)
//
// Return:      void 
//
//=============================================================================
void Vehicle::SparksTest(float impulseMagnitude, sim::Collision& inCollision)  // should this also be wrapped in mUserDrivingCar??
{
    sim::CollisionObject* collObjA = inCollision.mCollisionObjectA;
    sim::CollisionObject* collObjB = inCollision.mCollisionObjectB;
    
    sim::SimState* simStateA = collObjA->GetSimState(); // A is the vehicle.
    sim::SimState* simStateB;
    if( simStateA->mAIRefPointer != this )
    {
        simStateB = simStateA;
        simStateA = collObjB->GetSimState();
    }
    else
    {
        simStateB = collObjB->GetSimState();
    }

	// Lets add some vehicle specific particle effects upon collision
	// If the other collision object also a vehicle, lets create some sparks!
	// Better place for this constant?
	const float MIN_IMPULSE_VEHICLE_VEHICLE_SPARKS = 0.01f;
    float velocityScale = 0.0f;

    bool doStars = false;
    switch( simStateB->mAIRefIndex )
    {
    case PhysicsAIRef::redBrickPhizFence:
        impulseMagnitude *= ( 1.0f / 25000.0f );
        doStars = true;
        break;
    case PhysicsAIRef::redBrickVehicle:
        impulseMagnitude *= ( 1.0f / 5000.0f );
        velocityScale = impulseMagnitude;
        break;
    case PhysicsAIRef::redBrickPhizMoveable:
        impulseMagnitude *= ( 1.0f / 5000.0f );
        velocityScale = impulseMagnitude * 0.5f;
        break;
    case PhysicsAIRef::redBrickPhizStatic:
        impulseMagnitude *= ( 1.0f / 25000.0f );
        impulseMagnitude = rmt::Min( 1.0f, impulseMagnitude * 2.0f );
        doStars = true;
    default:
        return;
    }

    if( impulseMagnitude > 10.0f )
    {
        impulseMagnitude *= 0.1f;
    }

	if ( impulseMagnitude > MIN_IMPULSE_VEHICLE_VEHICLE_SPARKS )
	{
        const rmt::Vector& pos = inCollision.GetPositionA();
        if( doStars )
        {
            GetSparkleManager()->AddStars( pos, impulseMagnitude );
        }
        else
        {
            rmt::Vector vel = mVelocityCM;
            vel.y = 0.0f;
            vel.Normalize();
            vel.Scale( velocityScale );
            GetSparkleManager()->AddSparks( pos, vel, impulseMagnitude );
        }
    }
}

//=============================================================================
// Vehicle::VisualDamageType1
//=============================================================================
// Description: Comment
//
// Parameters:  (float percentageDamage)
//
// Return:      void 
//
//=============================================================================
void Vehicle::VisualDamageType1(float percentageDamage, DamageLocation dl)
{
/*
    // new design, new range
    
    0       normal
    10      flap
    30      texture
    60      white smoke
    99      grey smoke  (the one-hit indicator) - this will have been clamped to exactly this value
    100     black smoke and flame - car will blow in X seconds
    


*/

    // get the joint in question:
    int joint = -1;
    switch(dl)
    {
        case dl_hood:
            joint = mHoodJoint;
            break;

        case dl_trunk:
            joint = mTrunkJoint;
            break;

        case dl_driverside:
            joint = mDoorDJoint;
            break;

        case dl_passengerside:
            joint = mDoorPJoint;
            break;

        default:
            rAssert(0);
    }

    if(percentageDamage > 0.1f)    
    {
        // only join that got hit should flap        
        if(joint != -1)
        {
            int index = mJointIndexToInertialJointDriverMapping[joint];
            if(index != -1)
            {
                mPhObj->GetJoint(joint)->SetInvStiffness(1.0f);
                mInertialJointDrivers[index]->SetIsEnabled(true);
            }         
        }
    }


    if(percentageDamage > 0.3f)
    {
        // only the joint that got hit should have texture on it.

        switch(dl)
        {
            case dl_hood:
                mGeometryVehicle->DamageTextureHood(true);
                break;

            case dl_trunk:
                mGeometryVehicle->DamageTextureTrunk(true);
                mDontShowBrakeLights = true;
                break;

            case dl_driverside:
                mGeometryVehicle->DamageTextureDoorD(true);
                break;

            case dl_passengerside:
                mGeometryVehicle->DamageTextureDoorP(true);
                break;

            default:
                rAssert(0);
        }
        
        // but all should flap
            // all sides flapping ?
        int index;
    
        if(mHoodJoint != -1)
        {
            index = mJointIndexToInertialJointDriverMapping[mHoodJoint];
            if(index != -1)
            {
                mPhObj->GetJoint(mHoodJoint)->SetInvStiffness(1.0f);
                mInertialJointDrivers[index]->SetIsEnabled(true);
            }
        }

        if(mTrunkJoint != -1)
        {
            index = mJointIndexToInertialJointDriverMapping[mTrunkJoint];
            if(index != -1)
            {
                mPhObj->GetJoint(mTrunkJoint)->SetInvStiffness(1.0f);
                mInertialJointDrivers[index]->SetIsEnabled(true);
            }
        }

        if(mDoorDJoint != -1)
        {        
            index = mJointIndexToInertialJointDriverMapping[mDoorDJoint];
            if(index != -1)
            {
                mPhObj->GetJoint(mDoorDJoint)->SetInvStiffness(1.0f);
                mInertialJointDrivers[index]->SetIsEnabled(true);
            }
        }

        if(mDoorPJoint != -1)
        {
            index = mJointIndexToInertialJointDriverMapping[mDoorPJoint];
            if(index != -1)
            {
                mPhObj->GetJoint(mDoorPJoint)->SetInvStiffness(1.0f);
                mInertialJointDrivers[index]->SetIsEnabled(true);
            }
        }



    }

    
    if(percentageDamage > 0.6f)
    {
        // for now, just try taking off the hood before we make smoke pour out there.
        //int index;
    
        if(mHoodJoint != -1)
        {
            mGeometryVehicle->HideFlappingPiece(mHoodJoint, true);                
        }
        
        // all texture
        
        mGeometryVehicle->DamageTextureHood(true);
        mGeometryVehicle->DamageTextureTrunk(true);
        mGeometryVehicle->DamageTextureDoorD(true);
        mGeometryVehicle->DamageTextureDoorP(true);       

        // white smoke        
        mGeometryVehicle->SetEngineSmoke(ParticleEnum::eEngineSmokeLight);
        
        mDontShowBrakeLights = true;
        mGeometryVehicle->SetLightsOffDueToDamage(true);
        
        // at this point lights and brake lights should stop working
        

    }
    
    if(percentageDamage >= 0.98f)    // this is the one-more-hit warning
    {
        mGeometryVehicle->SetEngineSmoke(ParticleEnum::eEngineSmokeMedium);

    }
    
    if(percentageDamage > 0.99f)    // about to blow
    {
       mGeometryVehicle->SetEngineSmoke(ParticleEnum::eEngineSmokeHeavy);    
    }

}



//=============================================================================
// Vehicle::VisualDamageType2
//=============================================================================
// Description: Comment
//
// Parameters:  (float percentageDamage, DamageLocation dl)
//
// Return:      void 
//
//=============================================================================
void Vehicle::VisualDamageType2(float percentageDamage, DamageLocation dl)
{
/*
    try this range

    0       normal
    10      texture damage (localized front, back, driver side, passenger side)
    
    40      smoke level 1
    60      smoke level 2
    80      smoke level 3
    100     disabled


*/

    
    // note:
    // joint might still be -1, if the is a type 1 car but it doesn't have all four 
    // flapping joints?

    // this is bulky, but fuck it, gotta get this shit working like yesterday

    if(percentageDamage > 0.1f)
    {
        // only the joint that got hit should have texture on it.

        switch(dl)
        {
            case dl_hood:
                mGeometryVehicle->DamageTextureHood(true);
                break;

            case dl_trunk:
                mGeometryVehicle->DamageTextureTrunk(true);
                mDontShowBrakeLights = true;       
                break;

            case dl_driverside:
                mGeometryVehicle->DamageTextureDoorD(true);
                break;

            case dl_passengerside:
                mGeometryVehicle->DamageTextureDoorP(true);
                break;

            default:
                rAssert(0);
        }
        


    }

        
    if(percentageDamage > 0.4f)
    {
        // smoke 1
        //
        // ? all sides textured?

        mGeometryVehicle->DamageTextureHood(true);
        mGeometryVehicle->DamageTextureTrunk(true);
        mGeometryVehicle->DamageTextureDoorD(true);
        mGeometryVehicle->DamageTextureDoorP(true);       
        
        //mGeometryVehicle->SetEngineSmoke(ParticleEnum::eEngineSmokeLight);
        mGeometryVehicle->SetLightsOffDueToDamage(true);
        
    }
    
    if(percentageDamage > 0.6f)
    {
        mGeometryVehicle->SetEngineSmoke(ParticleEnum::eEngineSmokeLight);
        //mGeometryVehicle->SetEngineSmoke(ParticleEnum::eEngineSmokeMedium);
    }
    
    if(percentageDamage >= 0.98f)
    {
        mGeometryVehicle->SetEngineSmoke(ParticleEnum::eEngineSmokeMedium);
    }    

    if(percentageDamage > 0.99f)
    {
        mGeometryVehicle->SetEngineSmoke(ParticleEnum::eEngineSmokeHeavy);
    }

}



//=============================================================================
// Vehicle::VisualDamageType3
//=============================================================================
// Description: Comment
//
// Parameters:  (float percentageDamage)
//
// Return:      void 
//
//=============================================================================
void Vehicle::VisualDamageType3(float percentageDamage)
{   
    
    if(percentageDamage > 0.6f)
    {
        mGeometryVehicle->SetEngineSmoke(ParticleEnum::eEngineSmokeLight);
        //mGeometryVehicle->SetEngineSmoke(ParticleEnum::eEngineSmokeMedium);
    }
    
    if(percentageDamage >= 0.98f)
    {
        mGeometryVehicle->SetEngineSmoke(ParticleEnum::eEngineSmokeMedium);
    }    

    if(percentageDamage > 0.99f)
    {
        mGeometryVehicle->SetEngineSmoke(ParticleEnum::eEngineSmokeHeavy);
    }

}


//=============================================================================
// Vehicle::SyncVisualDamage
//=============================================================================
// Description: Comment
//
// Parameters:  ( float Health )
//
// Return:      void 
//
//=============================================================================
void Vehicle::SyncVisualDamage( float Health )
{
    if( Health >= 1.0f )
    {
        return;
    }
    else if( Health <= 0.0f )
    {
        // Husk.
        mHitPoints = 0.0f;
        mVehicleDestroyed = true;
        ActivateTriggers(false);
    }
    else
    {
        // this should use the same visual logic functions as the normal damage system
        // just need to make up a DamageLocation
        // how about the hood? :)
        switch(mDamageType)
        {
            //case 1: // user level - flaping joints etc...
            case VDT_USER:
            {                
                VisualDamageType1(1.0f - Health, dl_hood);    // any point in this actually taking in a parameter
            }
            break;

            //case 2: // ai - localized damage textures but no flapping
            case VDT_AI:
            {                
                VisualDamageType2(1.0f - Health, dl_hood);    // any point in this actually taking in a parameter
            }
            break;
            
            case VDT_TRAFFIC:
            {                 
                VisualDamageType3(1.0f - Health);    // any point in this actually taking in a parameter
            }
            break;

            default:
                rAssertMsg(0, "what are you doing here?");

        }
             
           
    
    
    
    
    
    /*
        if( Health <= 0.8f)
        {
            mGeometryVehicle->DamageTextureHood(true);
            mGeometryVehicle->DamageTextureTrunk(true);
            mGeometryVehicle->DamageTextureDoorD(true);
            mGeometryVehicle->DamageTextureDoorP(true);       
        }
        if( Health <= 0.6f )
        {
            mGeometryVehicle->SetLightsOffDueToDamage(true);   
            if( Health <= 0.01f )
            {
                mGeometryVehicle->SetEngineSmoke(ParticleEnum::eEngineSmokeHeavy);
            }
            else if( Health <= 0.4f )
            {
                mGeometryVehicle->SetEngineSmoke(ParticleEnum::eEngineSmokeMedium);
            }
            else
            {
                mGeometryVehicle->SetEngineSmoke(ParticleEnum::eEngineSmokeLight);
            }
        }
        
     */   
    }
}

//=============================================================================
// Vehicle::TranslateCollisionIntoLocation
//=============================================================================
// Description: Comment
//
// Parameters:  (sim::Collision& inCollision)
//
// Return:      
//
//=============================================================================
Vehicle::DamageLocation Vehicle::TranslateCollisionIntoLocation(sim::Collision& inCollision)
{

    sim::CollisionObject* collObjA = inCollision.mCollisionObjectA;
    sim::CollisionObject* collObjB = inCollision.mCollisionObjectB;
    
    sim::SimState* simStateA = collObjA->GetSimState();
    sim::SimState* simStateB = collObjB->GetSimState();

    rmt::Vector pointOnOtherCar;    
    
    if(simStateA == mSimStateArticulated)
    {
        pointOnOtherCar = inCollision.GetPositionB();
    }
    else if(simStateB == mSimStateArticulated)
    {
        pointOnOtherCar = inCollision.GetPositionA();    
    }
    else
    {
        rAssertMsg(0, "problem in Vehicle::TranslateCollisionIntoLocation");
    }

    rmt::Vector centreToContactPoint;
    centreToContactPoint.Sub(pointOnOtherCar, this->rPosition());
    centreToContactPoint.NormalizeSafe();

    float dot = mVehicleFacing.DotProduct(centreToContactPoint);

    // first pass:
    // if no more than 45 degrees from facing, use trunk or hood
    //
    // otherwise, project on transverse and see

    const float cos45 = 0.7071f;

    int joint = 0;    

    if(dot > cos45)
    {
        // hood
        //return mHoodJoint;
        //joint = mHoodJoint;
        
        return dl_hood;
    }
    else if(dot < -cos45)
    {
        // trunk
        //return mTrunkJoint;
        //joint = mTrunkJoint;

        return dl_trunk;
    }
    else
    {
        float dot2 = mVehicleTransverse.DotProduct(centreToContactPoint);
    
        if(dot2 > cos45)
        {
            // passenger side
            //return mDoorPJoint;
            //joint = mDoorPJoint;

            return dl_driverside;
        }
        else
        {
            // driver side
            //return mDoorDJoint;
            //joint = mDoorDJoint;

            return dl_passengerside;
        }

    }

    rAssert(0); // shouldn't really ever get here.
    return dl_hood;
}



//=============================================================================
// Vehicle::BeefUpHitPointsOnTrafficCarsWhenUserDriving
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Vehicle::BeefUpHitPointsOnTrafficCarsWhenUserDriving()
{
    // can't just use VT_TRAFFIC since we want the effect if we've gone to a phone booth
    if( this->mVehicleID == VehicleEnum::TAXIA ||   // plus all others Jeff gives me
        this->mVehicleID == VehicleEnum::COMPACTA ||
        this->mVehicleID == VehicleEnum::MINIVANA ||
        this->mVehicleID == VehicleEnum::PICKUPA ||
        this->mVehicleID == VehicleEnum::SEDANA ||
        this->mVehicleID == VehicleEnum::SEDANB ||
        this->mVehicleID == VehicleEnum::SPORTSA ||
        this->mVehicleID == VehicleEnum::SPORTSB ||
        this->mVehicleID == VehicleEnum::SUVA ||
        this->mVehicleID == VehicleEnum::WAGONA ||
        this->mVehicleID == VehicleEnum::COFFIN ||
        this->mVehicleID == VehicleEnum::HALLO ||
        this->mVehicleID == VehicleEnum::SHIP ||
        this->mVehicleID == VehicleEnum::WITCHCAR ||
        this->mVehicleID == VehicleEnum::AMBUL ||
        this->mVehicleID == VehicleEnum::BURNSARM ||
        this->mVehicleID == VehicleEnum::FISHTRUC ||
        this->mVehicleID == VehicleEnum::GARBAGE ||
        this->mVehicleID == VehicleEnum::GLASTRUC ||
        this->mVehicleID == VehicleEnum::ICECREAM ||
        this->mVehicleID == VehicleEnum::ISTRUCK ||
        this->mVehicleID == VehicleEnum::NUCTRUCK ||
        this->mVehicleID == VehicleEnum::PIZZA ||
        this->mVehicleID == VehicleEnum::SCHOOLBU ||
        this->mVehicleID == VehicleEnum::VOTETRUC ||
        this->mVehicleID == VehicleEnum::CBONE )
        
    {
        
        // only add to an undamaged car, so that you can't get in and out to repair
        if(this->GetVehicleLifePercentage(this->mHitPoints) == 1.0f)
        {
            
            if(this->mDesignerParams.mHitPoints <= 1.0f)
            {
                this->mDesignerParams.mHitPoints += 1.0f;
                this->mHitPoints = mDesignerParams.mHitPoints;                   
            }
        }
    }

}


//=============================================================================
// Vehicle::GetVehicleDamagePercentage
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      float 
//
//=============================================================================
float Vehicle::GetVehicleLifePercentage(float testvalue)
{
    float temp = testvalue / mDesignerParams.mHitPoints;
    if(temp < 0.0f)
    {
        // shouldn't ever really be here
        // going below 0 should be caught somewhere else
        temp = 0.0f;        
    }
    rAssert(temp <= 1.0f);
    rAssert(temp >= 0.0f);

    return temp;

}


//=============================================================================
// Vehicle::TriggerDamage
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
float Vehicle::TriggerDamage(float amount, bool clamp)
{
    
    // TODO - disable some updating stuff - like locomotive shit, if 
    // we are destroyed
    if(mVehicleDestroyed)
    {
        // already destroyed
        // do nothing
        
        return 0.0f;
    }

    // try the clamp to 99% for last hit on all car types
    if(clamp)//mDamageType == VDT_USER || mDamageType == VDT_AI)
    {
    
        float currentPerc = GetVehicleLifePercentage(mHitPoints);

        if(currentPerc > 0.02f)
        {
            // we want to make sure we clamp this hit at 1% life left

            float testvalue = mHitPoints;
            testvalue -= amount;         

            if(testvalue < 0.0f)
            {    
                testvalue = 0.0f;        
            }
            
            float perc = GetVehicleLifePercentage(testvalue);
    
            if(perc < 0.02f)
            {
                // the clamp case:
            
                mHitPoints = 0.001f;    // just some token amount

               //only update the charactersheet if this car belongs to the player
                if (mbPlayerCar == true)
                {
                    GetCharacterSheetManager()->UpdateCarHealth( mCharacterSheetCarIndex, perc);
                }
                            
                GetEventManager()->TriggerEvent(EVENT_VEHICLE_DAMAGED, (void*)this);
                mNoDamageTimer = 1.0f;  // set countdown
                return 0.02f;   // return fixed amount
            }        
            else
            {
                // just normal hit
                mHitPoints = testvalue;     // fall through and continue            
            }        

        }
        else
        {
            // this is the hit that takes us out
            mHitPoints = 0.0f;
        }
        

    }
    else
    {
        mHitPoints -= amount;    
    }    

    if(mHitPoints <= 0.0f)
    {
        // we've been destroyed
        mHitPoints = 0.0f;
        mVehicleDestroyed = true;
        
        ActivateTriggers(false);
        //GetEventManager()->TriggerEvent(EVENT_VEHICLE_DESTROYED_BY_USER, (void*)this);
        // move the triggering of this event to PostSubstepUpdate
        //
        // user car will pause for timer, other cars will send out immediately
        mDamageOutResetTimer = 0.0f;
        
        /*
        // if this is a traffic, then just give'r right here
        if(mDamageType == VDT_TRAFFIC)
        {
            
            ParticleAttributes pa;
            pa.mType = ParticleEnum::eCarExplosion;
            rmt::Vector pos = this->rPosition();
            
            GetParticleManager()->Add(pa, pos);
            
            TrafficManager::GetInstance()->SwapInTrafficHusk(this);
            
        }
        */
        
    }
    
    // make sure the damaged event fired off either way...
    GetEventManager()->TriggerEvent(EVENT_VEHICLE_DAMAGED, (void*)this);

    float health = GetVehicleLifePercentage(mHitPoints);
    
    //only update the charactersheet if this car belongs to the player
    if (mbPlayerCar == true)
    {
        GetCharacterSheetManager()->UpdateCarHealth( mCharacterSheetCarIndex, health );
    }
    return health;
}



//=============================================================================
// Vehicle door handling
//=============================================================================
//
//  Figure out the final position of the doors, based on current state, 
//  and whether someone is opening/closing them
//

bool Vehicle::NeedToOpenDoor(Door door)
{
    if(!mHasDoors)
    {
        return false;
    }

    if(mDesiredDoorPosition[door] < 0.8f) 
    {
        return true;
    }

    if(!HasActiveDoor(door))
    {
        return false;
    }

    return false;
}

bool Vehicle::NeedToCloseDoor(Door door)
{
    // TODO : there are cases where we might want to not close it 
    // (physics, missing door, door left open, etc)
    if(!mHasDoors)
    {
        return false;
    }

    // check if door was left open when we exited
    if(mDesiredDoorPosition[door] > 0.0f)
    {
        return true;
    }

    if(!HasActiveDoor(door))
    {
        return false;
    }

    return false;
}

bool Vehicle::HasActiveDoor(Door door)
{
    if(!mHasDoors)
    {
        return false;
    }

    switch(door)
    {
        case DOOR_DRIVER:
            if(mDoorDJoint != -1)
            {
                int inertialJointIndex = mJointIndexToInertialJointDriverMapping[ mDoorDJoint ];
                if( inertialJointIndex == -1 )
                {
                    rWarningMsg( false, "why is there no inertial joint? doesn't this car have a door?" );
                    return false;
                }
                sim::PhysicsJointInertialEffector* inertialJoint = mInertialJointDrivers[ inertialJointIndex ];
                if( inertialJoint == NULL )
                {
                    return false;
                }
                if( inertialJoint->IsEnabled() )
                {
                    return false;
                }
            }

            break;

        case DOOR_PASSENGER:
            if(mDoorPJoint != -1)
            {
                if(mInertialJointDrivers[mJointIndexToInertialJointDriverMapping[mDoorPJoint]]->IsEnabled())
                    return false;
            }
            break;
    }
    return true;
}

void Vehicle::UpdateDoorState(void)
{
    if(mDesiredDoorPosition[DOOR_DRIVER] == 0.0f)
    {
        if((mDoorDJoint != -1) && (mPhObj->GetJoint(mDoorDJoint)))
        {
            mDesiredDoorPosition[DOOR_DRIVER] = mPhObj->GetJoint(mDoorDJoint)->Deformation();
        }
    }

    if(mDesiredDoorPosition[DOOR_PASSENGER] == 0.0f)
    {
        if((mDoorPJoint != -1) && (mPhObj->GetJoint(mDoorPJoint)))
        {
            mDesiredDoorPosition[DOOR_PASSENGER] = mPhObj->GetJoint(mDoorPJoint)->Deformation();
        }
    }
}

void Vehicle::ReleaseDoors(void)
{
    mDesiredDoorPosition[DOOR_DRIVER] = 0.0f;
    mDesiredDoorPosition[DOOR_PASSENGER] = 0.0f;
}

void Vehicle::PlayExplosionEffect()
{
    
    //rmt::Vector explosionCenter = rPosition();
   // explosionCenter.y += EXPLOSION_Y_OFFSET;
    GetWorldPhysicsManager()->ApplyForceToDynamicsSpherical( mCollisionAreaIndex, rPosition(), EXPLOSION_EFFECT_RADIUS, EXPLOSION_FORCE );
    
    // Lets get the explosion position as the center of the wheels
    rmt::Vector explosionCenter(0,0,0);
    for ( int i = 0 ; i < 4 ; i++ )
    {
        explosionCenter += mSuspensionWorldSpacePoints[i];
    }
    explosionCenter *= 0.25f;
    rmt::Matrix explosionTransform = GetTransform();
    explosionTransform.FillTranslate( explosionCenter );
    
    GetBreakablesManager()->Play( BreakablesEnum::eCarExplosion, explosionTransform );

    GetEventManager()->TriggerEvent( EVENT_BIG_BOOM_SOUND, this );
}


void Vehicle::AddToSimulation()
{
    //DynaPhysDSG::AddToSimulation();   // greg
                                        // jan 31, 2003
                                        // I don't think any vehicle should do this - just use it's own ground plane
    SetLocomotion( VL_PHYSICS );
}

void Vehicle::ApplyForce( const rmt::Vector& direction, float force )
{
    SetLocomotion( VL_PHYSICS );
    rmt::Vector& rVelocity = GetSimState()->GetLinearVelocity();
    float deltaV = force / GetMass();
    // Apply delta velocity
    rVelocity += (direction * deltaV);
    // Make it interact with the world
    AddToSimulation();

    // Damage the vehicle
    if ( mUserDrivingCar )
    {      
        TriggerDamage( force * s_DamageFromExplosionPlayer / EXPLOSION_FORCE, false );
    }
    else
    {
        TriggerDamage( force * s_DamageFromExplosion / EXPLOSION_FORCE, false );
    }     
}

    
bool Vehicle::AttachCollectible( StatePropCollectible* drawable )
{
    if ( drawable->GetState() != 0 )
        return false;

    bool wasAttached = mGeometryVehicle->AttachCollectible( drawable );
    if ( wasAttached )
    {
        GetEventManager()->TriggerEvent( EVENT_VEHICLE_COLLECTED_PROP, this );
    }
    return wasAttached;
}

StatePropCollectible* Vehicle::GetAttachedCollectible()
{
    return mGeometryVehicle->GetAttachedCollectible();
}

void Vehicle::DetachCollectible( const rmt::Vector& velocity, bool explode )
{
    mGeometryVehicle->DetachCollectible(velocity, explode);            
}


// move the door  to the specified location (called by the character AI during get in/out of car)
void  Vehicle::MoveDoor(Door door, DoorAction action, float position)
{
    rAssert(door < 2);

    mDesiredDoorPosition[door] = position;
    mDesiredDoorAction[door] = action;
}

// calculate the position of a single door
void Vehicle::CalcDoor(unsigned doorIndex, unsigned joint, float doorOpen)
{
    // clamp desired door position to 0 -> 1
    if(mDesiredDoorPosition[doorIndex] > rmt::Abs(doorOpen)) 
        mDesiredDoorPosition[doorIndex] = rmt::Abs(doorOpen);

    if(mDesiredDoorPosition[doorIndex] < 0.0f)
        mDesiredDoorPosition[doorIndex] = 0.0f;

    // grab the rest pose
    poser::Pose* pose = mPoseEngine->GetPose();
    rmt::Matrix matrix = pose->GetSkeleton()->GetJoint(joint)->restPose;
    rmt::Matrix tmp;

    // rotate by the scaled desired position
    tmp.Identity();
    tmp.FillRotateY(mDesiredDoorPosition[doorIndex] * doorOpen);
    tmp.Mult(matrix);

    // set the new joint position
    pose->GetJoint(joint)->SetObjectMatrix(tmp);

    // if we just closed the door all the way, turn off future door activity
    if((mDesiredDoorAction[doorIndex] == DOORACTION_CLOSE) && (mDesiredDoorPosition[doorIndex] == 0.0f))
    {
        // if neccesary, reset physics state
        int index = mJointIndexToInertialJointDriverMapping[joint];
        if( index != -1 )
        {
            if(mInertialJointDrivers[index]->IsEnabled())
            {
                mPhObj->GetJoint(joint)->ResetDeformation();
            }
        }
        mDesiredDoorAction[doorIndex] = DOORACTION_NONE;
    }

}

void Vehicle::CalcDoors(void)
{
    // angle in radians that represents a fully open door 
    // TODO : could be tweakable
    const float doorOpen = 1.0f;
       
    // update each door, if neccesary
    if((mDesiredDoorAction[0] != DOORACTION_NONE) || (mDesiredDoorAction[1] != DOORACTION_NONE))
    {
        if((mDesiredDoorAction[0] != DOORACTION_NONE) && mDoorDJoint != -1)
        {
            CalcDoor(0, mDoorDJoint, doorOpen);
        }

        if((mDesiredDoorAction[1] != DOORACTION_NONE) && mDoorPJoint != -1)
        {
            // passenger door is backwards, so we do negative rotation
            CalcDoor(1, mDoorPJoint, -1.0f * doorOpen); 
        }
    }
}

void Vehicle::SetDriverName(const char* name)
{
    if(!name)
    {
        mDriverName[0] = 0;
        return;
    }

    rAssert(strlen(name) < 32);
    strcpy(mDriverName, name);
}

const char* Vehicle::GetDriverName(void)
{
    return mDriverName;
}


void Vehicle::SetDriver(Character* d) 
{                          
    tRefCounted::Assign(mpDriver, d);
}

void Vehicle::SetShadowAdjustments( float Adjustments[ 4 ][ 2 ] ) 
{ 
    mGeometryVehicle->SetShadowAdjustments( Adjustments ); 
}

void Vehicle::SetShininess( unsigned char EnvRef )
{
    mGeometryVehicle->SetShininess( EnvRef );
}

//=============================================================================
// Vehicle::GetTopSpeedKmh
//=============================================================================
// Description: returns the top speed of which this vehicle is capable in world
//              units, not KPH
//
// Parameters:  NONE
//
// Return:      float 
//
//=============================================================================
float Vehicle::GetTopSpeed() const
{
    return mDesignerParams.mDpTopSpeedKmh * ( 1000.0f / 3600.0f );
}