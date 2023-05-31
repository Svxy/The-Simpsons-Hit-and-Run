//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        InstDynaPhysDSG.cpp
//
// Description: Implementation for InstDynaPhysDSG class.
//
// History:     Implemented	                         --Devin [6/17/2002]
//========================================================================

//========================================
// System Includes
//========================================


#include <simcollision/collisionobject.hpp>
#include <simcollision/collisionvolume.hpp>

//========================================
// Project Includes
//========================================
#include <render/DSG/InstDynaPhysDSG.h>

#include <render/RenderManager/WorldRenderLayer.h>
#include <render/RenderManager/RenderManager.h>
#include <render/Culling/WorldScene.h>

#include <worldsim/worldphysicsmanager.h>
#include <worldsim/character/character.h>

#include <render/particles/particlemanager.h>
#include <render/breakables/breakablesmanager.h>
#include <render/IntersectManager/IntersectManager.h>

#include <mission/gameplaymanager.h>
#include <worldsim/worldphysicsmanager.h>

#include <p3d/billboardobject.hpp>

//************************************************************************
//
// Global Data, Local Data, Local Classes
//
//************************************************************************


// A bias for the mass of a breakable object that determines how much force is required to smash it
// if ( impactForce > mass * MASS_IMPULSE_BREAK_BIAS )
//	object breaks
const float MASS_IMPULSE_BREAK_BIAS = 0.0f; //200.0f;
const float INST_DYNA_MASS_IMPULSE_PARTICLE_BIAS = 5.0f;

//************************************************************************
//
// Public Member Functions : InstDynaPhysDSG Interface
//
//************************************************************************
//========================================================================
// InstDynaPhysDSG::
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
InstDynaPhysDSG::InstDynaPhysDSG()
:  /*mIsHit( false ), moved up to DynaPhysDSG*/ mpGeo( NULL ), mHideOnHitIndex( -1 )
{

    // move to DynaPhysDSG
    //mGroundPlaneIndex = -1;
}
//========================================================================
// InstDynaPhysDSG::
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
InstDynaPhysDSG::~InstDynaPhysDSG()
{
BEGIN_PROFILE( "InstDynaPhysDSG Destroy" );
    //mpPhysObj->Release(); // the owner simstate does this.
    if ( mpGeo != NULL )
	{
		mpGeo->Release();
	}

END_PROFILE( "InstDynaPhysDSG Destroy" );
}
//========================================================================
// InstDynaPhysDSG::
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

void InstDynaPhysDSG::Display()
{
    if(IS_DRAW_LONG) return;
#ifdef PROFILER_ENABLED
    char profileName[] = "  InstDynaPhysDSG Display";
#endif
    DSG_BEGIN_PROFILE(profileName)
    if(CastsShadow())
    {
        BillboardQuadManager::Enable();
    }
    p3d::pddi->PushMultMatrix(PDDI_MATRIX_MODELVIEW, &mMatrix);
    if( mHideOnHitIndex > -1 )
    {
        tCompositeDrawable* compDraw = static_cast<tCompositeDrawable*>( mpGeo );
        compDraw->GetDrawableElement( mHideOnHitIndex )->SetVisibility( !mIsHit );
    }
    mpGeo->Display();
    p3d::pddi->PopMatrix(PDDI_MATRIX_MODELVIEW);
    if(CastsShadow())
    {
        BillboardQuadManager::Disable();
        DisplaySimpleShadow();
    }
    DSG_END_PROFILE(profileName)
}
//========================================================================
// InstDynaPhysDSG::
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
void InstDynaPhysDSG::GetBoundingBox(rmt::Box3D* box)
{
    //box->low = mBBox.low;
    //box->high = mBBox.high;

    // note: - because the box is assumed to be axis aligned, we just want to translate it,
    // not rotate!
    
    (box->low).Add(mBBox.low, mPosn);
    (box->high).Add(mBBox.high, mPosn);
}
//========================================================================
// InstDynaPhysDSG::
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
void InstDynaPhysDSG::GetBoundingSphere(rmt::Sphere* pSphere)
{
    // transform here is overkill
    (pSphere->centre).Add(mSphere.centre, mPosn);

    pSphere->radius = mSphere.radius;
}


//========================================================================
// InstDynaPhysDSG::
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
sim::PhysicsObject* InstDynaPhysDSG::pPhysObj()
{
    // not sure we need this?
    return mpPhysObj;
}
//========================================================================
// InstDynaPhysDSG::
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
rmt::Matrix* InstDynaPhysDSG::pMatrix()
{
    return &mMatrix;
}
//========================================================================
// InstDynaPhysDSG::
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
tDrawable* InstDynaPhysDSG::pGeo()
{
    return mpGeo;
}







//=============================================================================
// InstDynaPhysDSG::Update
//=============================================================================
// Description: Comment
//
// Parameters:  (float dt)
//
// Return:      void 
//
//=============================================================================
void InstDynaPhysDSG::Update(float dt)
{
    mpPhysObj->Update(dt);    
    
    sim::CollisionObject* collObj = mpSimStateObj->GetCollisionObject();
    if ( collObj->HasMoved( ) || collObj->HasRelocated( ) )
    {
        collObj->Update();

    
        // do this _before_ updating matrix!
        rmt::Box3D oldBox;
 
        (oldBox.low).Add(mBBox.low, mPosn);
        (oldBox.high).Add(mBBox.high, mPosn);

        mMatrix = mpSimStateObj->GetTransform();
        mPosn = mpSimStateObj->GetPosition();

        WorldRenderLayer* pWorldRenderLayer = static_cast< WorldRenderLayer* > (GetRenderManager()->mpLayer( mRenderLayer ));
        // Sanity check
        rAssert( dynamic_cast<WorldRenderLayer*>(pWorldRenderLayer) != NULL );
        pWorldRenderLayer->pWorldScene()->Move( oldBox, this );   
    }    

    if(mIsHit && (GetCollisionAttributes()->GetClasstypeid() == PROP_ONETIME_MOVEABLE))
    {
        if(!GetGameplayManager()->TestPosInFrustrumOfPlayer( mPosn, 0, mSphere.radius))
        {
            this->AddRef();
            ((WorldRenderLayer*)GetRenderManager()->mpLayer(RenderEnums::LevelSlot))->RemoveGuts(this);
            GetWorldPhysicsManager()->RemoveFromAnyOtherCurrentDynamicsListAndCollisionArea(this);
            GetRenderManager()->mEntityDeletionList.Add(this);
        }
    }
}


//=============================================================================
// InstDynaPhysDSG::PreReactToCollision
//=============================================================================
// Description: Comment
//
// Parameters:  (sim::SimState* pCollidedObj, sim::Collision& inCollision)
//
// Return:      bool 
//
//=============================================================================
sim::Solving_Answer InstDynaPhysDSG::PreReactToCollision(sim::SimState* pCollidedObj, sim::Collision& inCollision)
{
    if(this->GetSimState()->GetControl() == sim::simSimulationCtrl)
    {
		return sim::Solving_Continue;
    }

	//Character* pCharacter = dynamic_cast<Character*>( static_cast<IEntityDSG*>( pCollidedObj->mAIRefPointer ) );
    if ( pCollidedObj->mAIRefIndex == PhysicsAIRef::PlayerCharacter || 
         pCollidedObj->mAIRefIndex == PhysicsAIRef::NPCharacter )
    {
        rAssert( dynamic_cast< Character* >( static_cast< tRefCounted* >( pCollidedObj->mAIRefPointer ) ) );
        Character* pCharacter = static_cast< Character* >( pCollidedObj->mAIRefPointer );
        pCharacter->TouchProp( this );	
		return sim::Solving_Aborted;
	}
	else
	{

        // if this object is under AI ctrl and it collided with a static it is just a poory placed prop
        // 
        // return
        
        if(this->GetSimState()->GetControl() == sim::simAICtrl && ((pCollidedObj->GetCollisionObject()->IsStatic() == true) || (pCollidedObj->mAIRefIndex == PhysicsAIRef::redBrickPhizMoveableGroundPlane)))
        {
            return sim::Solving_Aborted;
        }        


        // try moving this to post....
        
        
		// Moving to sim. Breakables use a bit of a trick, they don't
		// set the object to go under sim control immediately, but get set in
		// PostReactToCollision. Everything else can be turned on automatically

        mIsHit = true;
        //if ( mpCollisionAttributes == NULL ||
		//	 mpCollisionAttributes->GetClasstypeid() != PROP_BREAKABLE )
			 
		{
			AddToSimulation( );
		}

		return sim::Solving_Continue;
        //return sim::Solving_Aborted;
	}
    
}
/*
==============================================================================
InstDynaPhysDSG::AddToSimulation
==============================================================================
Description:	Comment

Parameters:		( void )

Return:			void 

=============================================================================
*/
void InstDynaPhysDSG::AddToSimulation( void )
{
	mpSimStateObj->SetControl(sim::simSimulationCtrl);

//	rAssert(mGroundPlaneIndex != -1);
    if ( mGroundPlaneIndex != -1 ) 
    {
    //    mGroundPlaneIndex = FetchGroundPlane();
    
        GetWorldPhysicsManager()->EnableGroundPlaneCollision(mGroundPlaneIndex);
    
    }
}

//=============================================================================
// InstDynaPhysDSG::PostReactToCollision
//=============================================================================
// Description: Comment
//
// Parameters:  (sim::SimState* pCollidedObj, sim::Collision& inCollision)
//
// Return:      sim
//
//=============================================================================
sim::Solving_Answer InstDynaPhysDSG::PostReactToCollision(rmt::Vector& impulse, sim::Collision& inCollision)
{


    // subclass-specific shit here


    // If it is a breakable object and has an assicated particle animation with it (it should)
    // play the associated particle effect
    if (mpCollisionAttributes != NULL)
    {
        if( mpCollisionAttributes->GetClasstypeid() == PROP_BREAKABLE ||
            mpCollisionAttributes->GetClasstypeid() == PROP_MOVEABLE ||
            mpCollisionAttributes->GetClasstypeid() == PROP_ONETIME_MOVEABLE)

        {
 
            //float impulsemag = impulse.Magnitude();

			// Ok, we are going to be doing a bit of a trick to determine whether or not a breakable
			// object is hit with enough power to blow it right out of the DSG.
			// How hard an object is to break is determined directly by its mass * a scalar bias


			// if the impact force exceeds the threshold, the object is broken
			// What happens is that Greg switches it from AI control to sim control.
			// If the object remained under AI control, the vehicle, despite hitting it hard enough
			// to annihilate it, would still bounce off if it as if it hit a brick wall
			// So we switch it to SimControl. The vehicle is slowed down somewhat by the impact
			// but it won't just bounce off it.
			
			
			
			/*
			
			    greg
			    jan 29, 2003
			    
			    move this to pre react to collision since the threshold is 0
			
			
			float threshold = mpCollisionAttributes->GetMass() * MASS_IMPULSE_BREAK_BIAS;

            // temp - until we get the mass/threshold thing worked out.
            if(mpSimStateObj->GetControl() == sim::simAICtrl && impulsemag > threshold)
            {
                this->AddToSimulation();
                return sim::Solving_Aborted;  // was this so that we don't get the static reaction? yes.
                //return sim::Solving_Continue;
            }
            */
            
        }

        bool wasBroken = false;
        if ( mpSimStateObj->GetControl() == sim::simSimulationCtrl )    
        {
            wasBroken = Break();
        }
        
        // Calc the minimum amount of force required to emit particles. The function is trivial
        // if impulsemag > mass * scale
        //    emit particles
        float particleThreshold = mpCollisionAttributes->GetMass() * INST_DYNA_MASS_IMPULSE_PARTICLE_BIAS;

        if ( mpCollisionAttributes->GetParticle() != ParticleEnum::eNull && 
             mWasParticleEffectTriggered == false && 
             wasBroken == false &&
             impulse.Magnitude() > particleThreshold )
        {
			ParticleAttributes attr;
			attr.mType = mpCollisionAttributes->GetParticle();
			GetParticleManager()->Add( attr, mMatrix );          
			mWasParticleEffectTriggered = true;
        }

        
        /*
        
            I don't think this test had any real effect
        
        if( mpCollisionAttributes->GetClasstypeid() == PROP_BREAKABLE && 
            mpCollisionAttributes->GetBreakable() == BreakablesEnum::eKrustyGlassBreaking )
        {
            // we don't want the car to receive any impulse from this
            return sim::Solving_Aborted;
        }
        */
        
        
    }



    return CollisionEntityDSG::PostReactToCollision(impulse, inCollision);

}

bool InstDynaPhysDSG::Break()
{
    bool success;

    // Any shadows or light pools should be deactivated
    SetShadow( NULL );

    if ( mpCollisionAttributes->GetClasstypeid() == PROP_BREAKABLE && 
            mpCollisionAttributes->GetBreakable() != BreakablesEnum::eNull )    
    {
        GetBreakablesManager()->Play( mpCollisionAttributes->GetBreakable(), mMatrix );
        GetBreakablesManager()->RemoveBrokenObjectFromWorld( this, GetRenderLayer(), true );
        success = true;
    }    
    else
    {
        success = false;
    }
    return success;
}


//=============================================================================
// InstDynaPhysDSG::OnSetSimState
//=============================================================================
// Description: Comment
//
// Parameters:  ( sim::SimState* ipSimState )
//
// Return:      void 
//
//=============================================================================
void InstDynaPhysDSG::OnSetSimState( sim::SimState* ipSimState )
{
    tRefCounted::Assign( mpSimStateObj, ipSimState );
    
    //mpSimStateObj->mAIRefIndex = InstDynaPhysDSG::GetAIRef();
    mpSimStateObj->mAIRefIndex = this->GetAIRef();

    mpSimStateObj->ResetVelocities();

    mPosn = mpSimStateObj->GetPosition();
    
    
    // set up box and sphere for DSG
    //
    // note, unlike the StaticPhysDSG, this box and sphere are in local space

    //mPosn = mpSimStateObj->GetCollisionObject()->GetCollisionVolume()->mPosition;

    /*   
    mBBox.low   = mPosn;
    mBBox.high  = mPosn;
    mBBox.high += mpSimStateObj->GetCollisionObject()->GetCollisionVolume()->mBoxSize;
    mBBox.low  -= mpSimStateObj->GetCollisionObject()->GetCollisionVolume()->mBoxSize;

    mSphere.centre.Sub(mBBox.high,mBBox.low);
    mSphere.centre *= 0.5f;
    mSphere.centre.Add(mBBox.low);
    mSphere.radius = mpSimStateObj->GetCollisionObject()->GetCollisionVolume()->mSphereRadius;
    */
    

    rmt::Vector center = mpPhysObj->GetExternalCMOffset();
    float radius = mpSimStateObj->GetSphereRadius();

    mBBox.low   = center;
    mBBox.high  = center;
    mBBox.high += mpSimStateObj->GetCollisionObject()->GetCollisionVolume()->mBoxSize;
    mBBox.low  -= mpSimStateObj->GetCollisionObject()->GetCollisionVolume()->mBoxSize;

    mSphere.centre = center;
    mSphere.radius = radius;

   
    // assing physics attributes
    rAssert(mpCollisionAttributes);
    mpSimStateObj->SetPhysicsProperties(mpCollisionAttributes->GetPhysicsProperties());


}


   
///////////////////////////////////////////////////////////////////////
// Load interface
///////////////////////////////////////////////////////////////////////

//========================================================================
// InstDynaPhysDSG::
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
void InstDynaPhysDSG::LoadSetUp
(  
    sim::SimState*      ipSimState, 
    CollisionAttributes* ipCollAttr,
    const rmt::Matrix&  iMatrix, 
    tDrawable*          ipGeo,
    tDrawable*			ipShadow )
{

    //mpPhysObj   = ipPhysObj;
    mpPhysObj = (sim::PhysicsObject*)(ipSimState->GetSimulatedObject());
    
    mMatrix     = iMatrix;
    mpGeo       = ipGeo;
    mpGeo->AddRef();

    SetCollisionAttributes(ipCollAttr);
    SetSimState(ipSimState);
    
    //mShaderUID = ipGeo->GetShader(0)->GetUID();
    ipGeo->ProcessShaders(*this);

    // update some shit with collision attributes:
    
    //StaticEntityDSG* pStatEntityDSG = static_cast< StaticEntityDSG* >( this );
  //  rAssert( dynamic_cast<StaticEntityDSG* >(this) != NULL )
    SetShadow( ipShadow );
}

InstDynaPhysDSG* InstDynaPhysDSG::Clone(const char* Name, const rmt::Matrix& iMatrix)
{
    InstDynaPhysDSG* c = new InstDynaPhysDSG();
    rAssert(c);
    c->SetName(Name);
    c->mMatrix = iMatrix;
    c->mpGeo = this->mpGeo;
    c->mpGeo->AddRef();
    c->SetCollisionAttributes(this->GetCollisionAttributes());
    sim::SimState* simState = sim::SimState::CreateSimState(this->GetSimState());
    simState->SetControl(sim::simAICtrl);
    simState->SetTransform(iMatrix);
    c->mpPhysObj = (sim::PhysicsObject*)(simState->GetSimulatedObject());
    c->SetSimState(simState);
    mpGeo->ProcessShaders(*c);
    c->SetShadow(this->mpShadow);
    return c;
}

// Override setrank, shadows get drawn first always!
void 
InstDynaPhysDSG::SetRank(rmt::Vector& irRefPosn, rmt::Vector& irRefVect)
{
    if ( CastsShadow() )
    {
        mRank = FLT_MAX;
    }
    else
    {
        IEntityDSG::SetRank( irRefPosn, irRefVect );
    }
}

//************************************************************************
//
// Protected Member Functions : InstDynaPhysDSG 
//
//************************************************************************



//************************************************************************
//
// Private Member Functions : InstDynaPhysDSG 
//
//************************************************************************
