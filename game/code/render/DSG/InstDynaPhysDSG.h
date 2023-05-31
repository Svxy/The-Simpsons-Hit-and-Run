#ifndef __InstDynaPhysDSG_H__
#define __InstDynaPhysDSG_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   InstDynaPhysDSG
//
// Description: The InstDynaPhysDSG does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/06/17]
//
//========================================================================

//=================================================
// System Includes
//=================================================

//=================================================
// Project Includes
//=================================================
#include <render/DSG/DynaPhysDSG.h>
#include <worldsim/physicsairef.h>

class tDrawable;

namespace sim 
{
    class SimState;
    class Collision;
    class PhysicsObject;
}

//========================================================================
//
// Synopsis:   The InstDynaPhysDSG; Synopsis by Inspection.
//
//========================================================================
class InstDynaPhysDSG 
: public DynaPhysDSG
{
public:
    InstDynaPhysDSG();
    virtual ~InstDynaPhysDSG();

    //////////////////////////////////////////////////////////////////////////
    // tDrawable
    //////////////////////////////////////////////////////////////////////////
    virtual void Display();
    virtual void GetBoundingBox(rmt::Box3D* box);
    virtual void GetBoundingSphere(rmt::Sphere* sphere);		

    virtual void Update(float dt);

    virtual sim::Solving_Answer PreReactToCollision( sim::SimState* pCollidedObj, sim::Collision& inCollision );  
    virtual sim::Solving_Answer PostReactToCollision(rmt::Vector& impulse, sim::Collision& inCollision);
    // Plays the breakable animation if one exists
    // and flags the object for DSG removal
    // returns true if the object was broken, false if it was not(no animation available)
    virtual bool Break();

	void AddToSimulation( void );
    virtual int GetAIRef() {return PhysicsAIRef::redBrickPhizMoveable;}
    /*
    static int GetAIRef()
    {
        return PhysicsAIRef::redBrickPhizMoveable;
    }
    */
    virtual void OnSetSimState( sim::SimState* ipSimState ); 
    

    ///////////////////////////////////////////////////////////////////////
    // Accessors
    ///////////////////////////////////////////////////////////////////////

    sim::PhysicsObject* pPhysObj();
    rmt::Matrix* pMatrix();
    tDrawable* pGeo();
        
    //virtual void FetchGroundPlane();    // tell object to get itself a ground plane, through worldphysicsmanager, through groundplanepool
    //virtual void FreeGroundPlane();     // make this safe to call even when we don't have one?
    
    //virtual bool IsAtRest();

    // need these?
    //virtual int GetGroundPlaneIndex() {return mGroundPlaneIndex;}
    //virtual void SetGroundPlaneIndex(int index) {mGroundPlaneIndex = index;}
   
    ///////////////////////////////////////////////////////////////////////
    // Load interface
    ///////////////////////////////////////////////////////////////////////
	// Shadow is optional, set NULL to disable
    void LoadSetUp(  sim::SimState*      ipSimState, 
                     CollisionAttributes* ipCollAttr,
                     const rmt::Matrix&  iMatrix, 
                     tDrawable*          ipGeo,
					 tDrawable*			 ipShadow = NULL);
    InstDynaPhysDSG* Clone(const char* Name, const rmt::Matrix& iMatrix);
    virtual void SetRank(rmt::Vector& irRefPosn, rmt::Vector& irRefVect);

    // you unbelievably stupid motherfucker! (greg talking to himself)
    //bool                mIsHit;
    
protected:
    
    // just for convenience I guess    
    sim::PhysicsObject* mpPhysObj;

    rmt::Matrix         mMatrix;
    tDrawable*          mpGeo;
    int                 mHideOnHitIndex;
    

    // need to hold this here, since the same object might be in more than one list..
    // moved to DynaPhysDSG...
    //int mGroundPlaneIndex;

private:


};

#endif
