//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   StatePropDSG
//
// Description: DSG object that contains a state prop
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef STATEPROPDSG_H
#define STATEPROPDSG_H


//===========================================================================
// Nested Includes
//===========================================================================
#include <render/dsg/DynaPhysDsg.h>
#include <stateprop/stateprop.hpp>
#include <loading/loadingmanager.h>
#include <worldsim/physicsairef.h>
#include <memory/map.h>

//===========================================================================
// Forward References
//===========================================================================

namespace poser
{
    class Pose;
};
namespace sim
{
    class PhysicsObject;
};

class tEntityStore;

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================


//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//      A State Prop Object suited for entry into a DSG
//
// Constraints:
//
//
//===========================================================================
class StatePropDSGProcAnimator;

class StatePropDSG : public DynaPhysDSG, public CStatePropListener 
{
    public:

        enum Event
        {   
            IDLE,
            FADE_IN,
            FADE_OUT,
            MOVING,
            ATTACK_CHARGING,
            ATTACK_CHARGED,
            ATTACKING,
            DESTROYED,
            HIT,
            FEATHER_TOUCH,
            STOMP,
            VEHICLE_HIT
        };

        StatePropDSG();
        virtual ~StatePropDSG();

        virtual void LoadSetup( CStatePropData* statePropData, 
                        int startState,
                        const rmt::Matrix& transform,
                        CollisionAttributes* ipCollAttr,
                        bool isDynaLoaded = true,
                        tEntityStore* ipSearchStore = NULL,
                        bool useSharedtPose = false,
                        sim::CollisionObject* collisionObject = NULL,
                        sim::PhysicsObject* physicsObject = NULL );

            
        StatePropDSG* Clone(const char* Name, const rmt::Matrix& iMatrix) const;


        virtual void Display();
        // Updates animation
        virtual void AdvanceAnimation( float timeInMS );
        // Updates physics
        virtual void Update( float dt );

        virtual void GetBoundingBox( rmt::Box3D* box );
        virtual void GetBoundingSphere( rmt::Sphere* sphere );

        // Dangerous function, assert on use
        virtual rmt::Vector*       pPosition();
        virtual const rmt::Vector& rPosition();
        virtual void GetPosition( rmt::Vector* ipPosn );
        virtual void SetPosition( const rmt::Vector& position );
        virtual void SetTransform( const rmt::Matrix& matrix );
        virtual void GetTransform( rmt::Matrix* pMatrix );
        // This will have to be changed, default to true for now
        virtual bool HasAlpha()const { return true; }

        virtual sim::Solving_Answer PreReactToCollision( sim::SimState* pCollidedObj, sim::Collision& inCollision );
        virtual sim::Solving_Answer PostReactToCollision(rmt::Vector& impulse, sim::Collision& inCollision);
        virtual int GetAIRef();
        virtual void RecieveEvent( int callback , CStateProp* stateProp );

    	virtual void AddToSimulation( void );     
        virtual void ApplyForce( const rmt::Vector& direction, float force );

        virtual int CastsShadow();
        virtual void DisplaySimpleShadow();
        virtual void SetRank(rmt::Vector& irRefPosn, rmt::Vector& mViewVector);

        // Sets which drawable element is the shadow/lightpool and should
        // be projected on the ground and drawn only in the shadow pass
        void SetShadowElement( tUID elementName );

        // Turn collisions on and off. Used by the artist callback : REMOVE_COLLISION_VOLUME
        void EnableCollisionVolume( bool enable );
        // Kills a subvolume, called by callbacks of the same name
        void RemoveSubCollisionVolume( int volumeIndex );

        virtual void GenerateCoins( int numCoins );


        void SetState( int state );
        bool HandleEvent( Event );
        unsigned int GetState()const;
        tUID GetStatePropUID()const { return mpStateProp->GetUID(); }
        tUID GetDrawableUID()const { return mpStateProp->GetDrawable()->GetUID(); }

        void SetProcAnimator( StatePropDSGProcAnimator* Animator ) { mpProcAnimator = Animator; }
        StatePropDSGProcAnimator* GetProcAnimator( void ) const { return mpProcAnimator; }

        // Sets which joint the sim state should animate upon
        void SetSimJoint( int jointId );

        // Returns type of stateprop based upon CollisionAttributes
        // either PROP_MOVEABLE / BREAKABLE / STATIC
        int GetType()const;

        // Add and remove a CStatePropListener
        void AddStatePropListener( CStatePropListener* statePropListener );
	    void RemoveStatePropListener( CStatePropListener* statePropListener );

        // Turn on/off visibility
        void EnableVisibility( bool enable );

        // Remove the shared tPose pool
        static void RemoveAllSharedtPoses();

        // Typedefs
        typedef Map< tUID, tPose* > PoseMap;
        typedef PoseMap::iterator PoseMapIt;

    protected:

        virtual void OnSetSimState( sim::SimState* ipCollObj );

        rmt::Matrix mTransform;
        CStateProp* mpStateProp;

        sim::PhysicsObject* mpPhysObj;
        bool m_IsDynaLoaded;
        // The drawable element index
        StatePropDSGProcAnimator* mpProcAnimator;
        int m_ShadowElement;

        // Which joint any sim animation is tied to
        int m_SimAnimJoint;

        // Uses m_SimJoint to animate the collision volume
        void AnimateCollisionVolume( );

        // Memory optimizations: optionally make the tPose shared amongst 
        // all elements of that stateprop type
        // Find a tPose from the given stateprop type
        // If none exist, then NULL is returned
        static tPose* GetSharedtPose( tUID type );
        // Add the given shared pose to the shared pose list
        static void AddNewSharedtPose( tUID uid, tPose* );

        // List of all tPoses, indexed by stateprop type UID
        // So that the tCompositeDrawables don't have to allocate a new tPose
        // every time.
        static PoseMap* sp_SharedtPoses;

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow StatePropDSG from being copied and assigned.
        StatePropDSG( const StatePropDSG& );
        StatePropDSG& operator=( const StatePropDSG& );
};

#endif