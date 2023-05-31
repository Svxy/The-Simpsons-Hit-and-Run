//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        fencedsg.h
//
// Description: Blahblahblah
//
// History:     created june 27th, 2002 - gmayer
//
//=============================================================================

#ifndef FENCEENTITYDSG_H
#define FENCEENTITYDSG_H

//========================================
// Nested Includes
//========================================

//=================================================
// System Includes
//=================================================


#include <p3d/refcounted.hpp>
#include <radmath/radmath.hpp>

//=================================================
// Project Includes
//=================================================
//#include <render/DSG/IEntityDSG.h>
#include <render/DSG/collisionentitydsg.h>
#include <worldsim/physicsairef.h>



class FenceEntityDSG
:
public CollisionEntityDSG
{
public:
    FenceEntityDSG( void );
    virtual ~FenceEntityDSG( void ); 

   ///////////////////////////////////////////////////////////////////////
   // Drawable
   ///////////////////////////////////////////////////////////////////////
   void Display();    

   void DisplayBoundingBox(tColour colour = tColour(0,255,0));
   void DisplayBoundingSphere(tColour colour = tColour(0,255,0));

   void GetBoundingBox(rmt::Box3D* box);
   void GetBoundingSphere(rmt::Sphere* sphere);

   ///////////////////////////////////////////////////////////////////////
   // IEntityDSG
   ///////////////////////////////////////////////////////////////////////
   rmt::Vector*       pPosition();
   const rmt::Vector& rPosition();
   void               GetPosition( rmt::Vector* ipPosn );

//////////////////////////////////////////////////////////////////////////
   // override these methods so we can stub them out
    virtual sim::Solving_Answer PreReactToCollision( sim::SimState* pCollidedObj, sim::Collision& inCollision );
    virtual sim::Solving_Answer PostReactToCollision(rmt::Vector& impulse, sim::Collision& inCollision);

    // the only reason to inherit from CollisionEntityDSG is so desingers can
    // tweak and tune the friction values when you hit a wall.
    CollisionAttributes* GetCollisionAttributes( void ) const;
    void SetCollisionAttributes( CollisionAttributes* pCollisionAttributes );

    //
    // Implement pure virtual function from CollisionEntityDSG
    //
    int GetAIRef() { return( PhysicsAIRef::redBrickPhizFence ); }


    
    //-----------------------
    // the guts of this class
    //-----------------------
    rmt::Vector mStartPoint;
    rmt::Vector mEndPoint;
    rmt::Vector mNormal;


protected:

private:
    // Contained by CollisionEntityDSG
    //CollisionAttributes* mpCollisionAttributes;
};

#endif //FENCEENTITYDSG_H