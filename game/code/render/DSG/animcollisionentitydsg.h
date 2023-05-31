//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        animcollisionentitydsg.h
//
// Description: Blahblahblah
//
// History:     05/07/2002 + Created -- NAME
//
//=============================================================================

#ifndef ANIMCOLLLISIONENTITYDSG_H
#define ANIMCOLLLISIONENTITYDSG_H

//========================================
// Nested Includes
//========================================
#include <p3d/refcounted.hpp>
#include <p3d/anim/pose.hpp>
#include <radmath/radmath.hpp>

#include <render/DSG/collisionentitydsg.h>
#include <worldsim/physicsairef.h>


//========================================
// Forward References
//========================================
class tCompositeDrawable;
class tMultiController;
class tAnimation;
class RootMatrixDriver;
//class tPose::Joint;   this doesn't compile on ps2


namespace sim
{
    class SimStateArticulated;
    class CollisionVolume;
    class CollisionObject;
};

namespace poser
{
    class PoseEngine;
    class Joint;
};

namespace ActionButton
{
    class AnimSwitch;
};

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class AnimCollisionEntityDSG
:
public CollisionEntityDSG
{
public:
    AnimCollisionEntityDSG( void );
    ~AnimCollisionEntityDSG( void );

    bool Create( const char* objectName, const char* animName );
    
    void AdvanceAnimation( float timeins );
    void Update( float timeins );

    void UpdateVisibility( void );
    
    virtual sim::Solving_Answer PreReactToCollision( sim::SimState* pCollidedObj, sim::Collision& inCollision );
    virtual sim::Solving_Answer PostReactToCollision(rmt::Vector& impulse, sim::Collision& inCollision);

    virtual void Display();    
    virtual rmt::Vector*       pPosition() ;
    virtual const rmt::Vector& rPosition() ;
    virtual void GetPosition( rmt::Vector* ipPosn );

    tCompositeDrawable* GetDrawable( void ) const
    {
        return mpDrawable;
    }

    sim::SimStateArticulated* GetSimState( void ) const
    {
        return mpSimStateArticulated;
    }
    
    tMultiController* GetAnimController( void ) const
    {
        return mpAnimController;
    }

    poser::PoseEngine* GetPoseEngine( void ) const
    {
        return mpPoseEngine;
    }
    
    RootMatrixDriver* GetRootMatrixDriver( void ) const
    {
        return mpRootMatrixDriver;
    }

    void SetTransform( const rmt::Matrix& transform )
    {
        mTransform = transform;
    }
    rmt::Matrix* GetTransformPointer( void )
    {
        return &mTransform;
    }

    rmt::Matrix& GetTransformRef( void )
    {
        return mTransform;
    }

    tPose::Joint* GetPoseJoint( int jointIndex, bool bAttachToJoint );
 
    float& GetAnimationDirection( void )
    {
        return mfDirection;
    }
    void SetAnimationDirection( float fDirection )
    {
        mfDirection = fDirection;
    }
    
    void SetAction( ActionButton::AnimSwitch* pActionButton );
    //////////////////////////////////////////////////////////////////////////
    // tDrawable
    //////////////////////////////////////////////////////////////////////////
    void GetBoundingBox(rmt::Box3D* box);
    void GetBoundingSphere(rmt::Sphere* sphere);

    //////////////////////////////////////////////////////////////////////////
    // Load Interface
    //////////////////////////////////////////////////////////////////////////
    void LoadSetUp(tCompositeDrawable* pCompD, tMultiController* pAnimFC, sim::CollisionObject* pCollObject, tEntityStore* ipStore );

    //
    // Implement pure virtual function from CollisionEntityDSG
    //
    int GetAIRef() { return( PhysicsAIRef::redBrickPhizMoveableAnim ); }

protected:
    virtual void OnSetSimState( sim::SimState* ipCollObj );
    void UpdatePose( float timeins );
    void UpdateBBox( sim::CollisionVolume* pVolume );
private:
    void findSoundName();

    tCompositeDrawable* mpDrawable;
    sim::SimStateArticulated* mpSimStateArticulated;
    
    tMultiController* mpAnimController;

    poser::PoseEngine* mpPoseEngine;
    RootMatrixDriver* mpRootMatrixDriver;

    rmt::Matrix mTransform;

    //Prevent wasteful constructor creation.
	AnimCollisionEntityDSG( const AnimCollisionEntityDSG& AnimCollisionEntityDSG );
    AnimCollisionEntityDSG& operator=( const AnimCollisionEntityDSG& AnimCollisionEntityDSG );
    float mfDirection;

    ActionButton::AnimSwitch* mpActionButton;
    // We need to store our own bounding boxes and spheres.
    // the tCompositeDrawable's are stored in world space,
    // but they are not updated when an object animates.
    // we will update these boxes once per update.
    //
    rmt::Box3D mBoundingBox;
    rmt::Sphere mBoundingSphere;
};


#endif //ANIMCOLLLISIONENTITYDSG_H
