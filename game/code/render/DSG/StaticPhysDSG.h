#ifndef __StaticPhysDSG_H__
#define __StaticPhysDSG_H__

//========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   StaticPhysDSG
//
// Description: The StaticPhysDSG does STUFF
//
// History:     + Initial Implementation		      -- Devin [2002/05/27]
//
//========================================================================

#include <render/DSG/collisionentitydsg.h>
#include <worldsim/physicsairef.h>

//========================================================================
//
// Synopsis:   The StaticPhysDSG; Synopsis by Inspection.
//
//========================================================================
class StaticPhysDSG : public CollisionEntityDSG
{
public:
    StaticPhysDSG();
    ~StaticPhysDSG();

    // Implements ICollisionEntityDSG.
    //
    virtual sim::Solving_Answer PreReactToCollision( sim::SimState* pCollidedObj, sim::Collision& inCollision );
    virtual sim::Solving_Answer PostReactToCollision(rmt::Vector& impulse, sim::Collision& inCollision);

    virtual sim::SimState* GetSimState() const;
    virtual sim::SimState* mpSimState() const { return GetSimState(); }

    
	///////////////////////////////////////////////////////////////////////
    // Drawable
    ///////////////////////////////////////////////////////////////////////
    void Display();    

    void DisplayBoundingBox(tColour colour = tColour(0,255,0));
    void DisplayBoundingSphere(tColour colour = tColour(0,255,0));

    virtual void GetBoundingBox(rmt::Box3D* box);
    virtual void GetBoundingSphere(rmt::Sphere* sphere);

	// Inherited from IEntityDSG. Whether or not the object casts a shadow
	virtual int  CastsShadow(){ if(mpShadow != NULL ) return 1; else return 0;}
    virtual void SetShadow( tDrawable* ipShadow );
    virtual void RecomputeShadowPosition( float height_radius_bias = 1.0f );
    // like recompute shadow position, but avoids expensive ground intersect test
    // because the ground height is passed in as a parameter
    virtual void RecomputeShadowPositionNoIntersect( float height, const rmt::Vector& normal, float height_radius_bias = 1.0f, float scale = 1.0f );
    virtual void DisplaySimpleShadow();

   ///////////////////////////////////////////////////////////////////////
   // IEntityDSG
   ///////////////////////////////////////////////////////////////////////
   rmt::Vector*       pPosition();
   const rmt::Vector& rPosition();
   void               GetPosition( rmt::Vector* ipPosn );

    void RenderUpdate(); 

    virtual int GetAIRef() {return PhysicsAIRef::redBrickPhizStatic;}

    /*
    static int GetAIRef( void )
    {
        return PhysicsAIRef::redBrickPhizStatic;
    }
    */

protected:
    virtual void OnSetSimState( sim::SimState* ipSimState );

    virtual void SetInternalState( );

    rmt::Box3D mBBox;
    rmt::Sphere mSphere;
    rmt::Vector mPosn;
    sim::SimState* mpSimStateObj;

	tDrawable*			mpShadow;
	// Transform the shadow by this matrix to 
	// place it in the world
	rmt::Matrix*		mpShadowMatrix;

	rmt::Matrix*	CreateShadowMatrix( const rmt::Vector& objectPosition );
    bool ComputeShadowMatrix( const rmt::Vector& in_position, rmt::Matrix* out_pMatrix );

};

#endif
