//===========================================================================
// Copyright (C) 1999 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   animentitydsg
//
// Description: Animated object without collision detection for use in Devin's 
//              scene graph
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef ANIMENTITYDSG_H
#define ANIMENTITYDSG_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <render/dsg/IEntityDSG.h>
#include <radmath/radmath.hpp>
#include <render/Enums/RenderEnums.h>
#include <p3d/refcounted.hpp>
#include <p3d/anim/compositedrawable.hpp>


//===========================================================================
// Forward References
//===========================================================================

class tAnimationFrameController;
class tAnimation;
class tCompositeDrawable;
class tEntityStore;
class tMultiController;

namespace poser
{
    class PoseEngine;
}
namespace sim
{
    class SimStateArticulated;
};

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//      An Animated Object, nearly identical to AnimCollisionEntityDSG but 
//      without the collision information and processing. These are stored
//      in a separate array in the Spatial Tree Nodes
//
// Constraints:
//
//      RenderUpdate() function - ask what this is supposed to do. The other
//      implementations are all empty.
//
//===========================================================================
class AnimEntityDSG
: public IEntityDSG
{
    public:
        AnimEntityDSG();
        ~AnimEntityDSG();
    
        virtual void Display();
		virtual void SetVisibility( bool vis ) { mIsVisible = vis; }
		virtual bool GetVisibility() const { return mIsVisible; }
		virtual void PlayAnimation( bool anim ) { mIsAnimationPlaying = anim; } 
		void Reset();

        virtual rmt::Vector*       pPosition();
        virtual const rmt::Vector& rPosition();
        virtual void GetPosition( rmt::Vector* ipPosn );
        
        // Purpose of this function?
        virtual void RenderUpdate(){ ; }     

        virtual void Update( float timeins );
        virtual void GetBoundingBox( rmt::Box3D* box );
        virtual void GetBoundingSphere( rmt::Sphere* sphere );

        // Calculate the ground height underneath the object
        // and move the object to there plus the given height
        // Returns true if intersection found, false if not
        bool PlaceOnGround( float heightAboveGround, bool ibMoveOnFail = true );

        //////////////////////////////////////////////////////////////////////////
        // Load Interface
        //////////////////////////////////////////////////////////////////////////
        void LoadSetUp(tCompositeDrawable* pCompD, tMultiController* pMC, tEntityStore* ipStore, rmt::Vector& irPosition );

        // Tells the object what render layer it is currently occupying
        // Note - the default layer if this function is not used is the Level layer
        void SetRenderLayer( RenderEnums::LayerEnum renderLayer ) { mRenderLayer = renderLayer; }
        // Returns the render layer enumeration that the object resides in
        RenderEnums::LayerEnum GetRenderLayer() const { return mRenderLayer; }

        bool AddToUpdateList() {return mbAddToUpdateList;}
        int  TrackSeparately() {return mTrackSeparately;} 

        void SetAddToUpdateList(bool b){ mbAddToUpdateList=b; }
        void SetTrackSeparately(int  iFlag){ mTrackSeparately=iFlag; }
        
        void SetAnimRootHeadingYUp( rmt::Vector& irHeading );
        void GetAnimRootHeading(    rmt::Vector& orHeading );

        void SetPosition( rmt::Vector& irNewPosition );

    protected:
        bool mbAddToUpdateList;
        int  mTrackSeparately;
        //this is a hack specifically reserved for arrow use.
        //it also doubles as returning the intended index,
        //where 1 is index 0, for direcitonal arrow, and
        //      2 is indeex 1, for wrong way arrow

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow AnimEntityDSG from being copied and assigned.
        AnimEntityDSG( const AnimEntityDSG& );
        AnimEntityDSG& operator=( const AnimEntityDSG& );

        tCompositeDrawable* mpDrawable;
        // Holds position and orientation of entity
        rmt::Matrix mTransform;
        rmt::Matrix mTransformOffset;

        // Animation data
        tMultiController * mpMultiController;

        // We need to store our own bounding boxes and spheres.
        // the tCompositeDrawable's are stored in world space,
        // but they are not updated when an object animates.
        // we will update these boxes once per update.
        //
        rmt::Box3D mBoundingBox;
        rmt::Sphere mBoundingSphere;

		// Whether not we can see the object 
        bool mIsVisible          : 1;

		// Whether or not the animation is playing
        bool mIsAnimationPlaying : 1;
        bool debugflag           : 1;

        // Lets keep a list of pointers to ParticleSystems inside the composite
        // Drawable so that when we advance the framecontroller, we also cause it
        // to release new particles. If we dont do this, they will only emit 
        // upon Display(). If this happens, the particles will appear to 
        // only start sprouting when you look at them.
        SwapArray< tCompositeDrawable::DrawableEffectElement* > mEffectElements;

        // Searches the given drawable and inits the mEffectElements array
        void FindEffectElements( tCompositeDrawable* drawable );

        // The Current render layer, needed for when the objects want to move themselves in the DSG
        RenderEnums::LayerEnum mRenderLayer;

        // A bounding box, centered at the origin, computed using 
        // tParticleSystem::ComputePreciseBoundingBox
         
        rmt::Box3D* mParticleSystemBoundingBox;

        // Initializes the above bounding box using the given composite drawable
        bool GetParticleSystemBoundingBox( tCompositeDrawable* drawable, rmt::Box3D* out_box );


        unsigned int m_TimeLastParticleUpdate;
        float mTimeSinceLastUpdate; 
        unsigned updateIndex;

};

#endif