//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   InstAnimDynaPhysDSG
//
// Description: An InstDynaPhysDSG object that has a simple cyclic animation
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef INSTANIMDYNAPHYSDSG_H
#define INSTANIMDYNAPHYSDSG_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <render/DSG/InstDynaPhysDSG.h>
#include <memory/srrmemory.h>

//===========================================================================
// Forward References
//===========================================================================
class tMultiController;
class CStatePropData;

namespace sim
{
	class SimState;
    class SimStateArticulated;
    class CollisionVolume;
    class CollisionObject;
};
namespace ActionButton
{
    class AnimSwitch;
};
namespace poser
{
    class Pose;
}

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//		An instanced DSG object with a simple animation (texture, billboards, uv, etc)
//		that has physics, could possibly be breakable.
//
// Constraints:
//		Animation playback is set to cycle on all objects, animation is not instanced
//
//===========================================================================
class InstAnimDynaPhysDSG : public InstDynaPhysDSG
{
    public:
        InstAnimDynaPhysDSG();
        virtual ~InstAnimDynaPhysDSG();

		virtual void Display();
		virtual void DisplayBoundingBox( tColour colour = tColour( 255,0 ,0 ) );
		virtual void Update(float dt);

		// Set position and orientation
		virtual void SetTransform( const rmt::Matrix& transform );

		void LoadSetUp( CollisionAttributes*  ipCollAttr,
                        const rmt::Matrix&    iMatrix, 
                        tCompositeDrawable*	  ipCompDrawable,
						tMultiController*	  ipMultiController,
						tEntityStore*		  ipSearchStore );
		
        void SetAction( ActionButton::AnimSwitch* pActionButton );
        // If the object is a breakable, play the breakable animation
        // and flag this object for removal
        // otherwise, do nothing
        virtual bool Break();

    protected:

		tCompositeDrawable*			mpCompDraw;
		tMultiController*			mpMultiController;
        ActionButton::AnimSwitch*   mpActionButton;
        poser::Pose*                mpPose;

	protected:

		void UpdatePose( float deltaTime );
		void UpdateBBox( sim::CollisionVolume* pVolume );
    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow InstAnimDynaPhysDSG from being copied and assigned.
        InstAnimDynaPhysDSG( const InstAnimDynaPhysDSG& );
        InstAnimDynaPhysDSG& operator=( const InstAnimDynaPhysDSG& );

		friend class AnimDynaPhysLoader;

};
//===========================================================================
//
// Description: 
//		Wrapper for Instanced Animated Objects. The wrapper contains the compdrawables
//		all multicontrollers, meshes, billboards, etc.
//
// Constraints:
//
//
//===========================================================================

class AnimDynaPhysWrapper : public tEntity
{
public:

	AnimDynaPhysWrapper();
	virtual ~AnimDynaPhysWrapper();

	float GetVolume()const;
	bool HasAlpha()const;

	tCompositeDrawable* GetDrawable()const;
	tMultiController* GetController()const;
    CStatePropData* GetStatePropData()const;
    sim::PhysicsObject*	  GetPhysicsObject()const { return mPhysicsObject; }
    sim::CollisionObject* GetCollisionObject()const { return mCollisionObject; }

    void SetHasAnimation( bool hasAnim ) { mHasAnimations = hasAnim; }
    bool HasAnimation()const { return mHasAnimations; }

	// Creates a new InstAnimDynaPhys object,
	// optionally places it on the given heap
	InstAnimDynaPhysDSG* CreateDSGObject( CollisionAttributes* pAttr, const rmt::Matrix& transform, GameMemoryAllocator );

private:

    AnimDynaPhysWrapper( const AnimDynaPhysWrapper& );
    AnimDynaPhysWrapper& operator=( const AnimDynaPhysWrapper& );

private:

	tCompositeDrawable*		mCompDraw;
	tMultiController*		mMultiController;
	sim::PhysicsObject*		mPhysicsObject;
	sim::CollisionObject*   mCollisionObject;
	bool					mHasAlpha;
    CStatePropData*         mStatePropData;
    bool                    mHasAnimations;

	friend class AnimDynaPhysWrapperLoader;
	friend class AnimObjDSGWrapperLoader;
};

#endif