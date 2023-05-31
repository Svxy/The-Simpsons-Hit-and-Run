//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   statepropcollectible
//
// Description: Exactly like a normal stateprop, except it will
//              identify low-speed collisions with the player vehicle and 
//              then attach itself to the car
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef STATEPROPCOLLECTIBLE_H
#define STATEPROPCOLLECTIBLE_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <render/DSG/statepropdsg.h>
#include <worldsim/redbrick/vehicle.h>
#include <presentation/gui/utility/hudmap.h>

//===========================================================================
// Forward References
//===========================================================================


//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================


//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//      A statepropdsg with a special postreacttocollision
//
// Constraints:
//      
//
//===========================================================================
class StatePropCollectible : public StatePropDSG, public IHudMapIconLocator
{
    public:
        StatePropCollectible();
        virtual ~StatePropCollectible();
        virtual sim::Solving_Answer PreReactToCollision( sim::SimState* pCollidedObj, sim::Collision& inCollision );
        virtual void AdvanceAnimation( float timeInMS );
        virtual void SetTransform( const rmt::Matrix& matrix );
        virtual void SetPosition( const rmt::Vector& position );
        virtual void LoadSetup( CStatePropData* statePropData, 
                                int startState,
                                const rmt::Matrix& transform,
                                CollisionAttributes* ipCollAttr,
                                bool isDynaLoaded = true,
                                tEntityStore* ipSearchStore = NULL,
                                bool useSharedtPose = false,
                                sim::CollisionObject* collisionObject = NULL,
                                sim::PhysicsObject* physicsObject = NULL );

        void AddToDSG();
        bool IsInDSG()const             { return m_IsInDSG; }
        void RemoveFromDSG();
        // Updates physics
        virtual void Update( float dt );

                
        virtual void RecieveEvent( int callback , CStateProp* stateProp );

        // Collectible explodes and dies. Event fired off signalling mission failure
        void Explode();

        // Inherited from IHudMapIconLocator
        virtual void GetPosition( rmt::Vector* currentLoc );
        virtual void GetHeading( rmt::Vector* heading );

        void EnableHudIcon( bool enable );
        void EnableCollisionTesting( bool enable ) { m_CollisionTestingEnabled = enable; }
        bool IsCollisionTestingEnabled()const { return m_CollisionTestingEnabled; }

        const rmt::Matrix& GetStartingPosition()const { return mOriginalLocation; }

    protected:

        int m_HudIcon;
        bool m_IsInDSG;
        bool m_CollisionTestingEnabled;
        rmt::Vector m_PreviousPosition;

        // Location of the original start position
        rmt::Matrix mOriginalLocation;

        void DoCollisionTesting();
                
        // Called by the camera shake callback
        void CameraShake();    

        unsigned int m_ShakeStartTime;
        bool m_CameraShaking;

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow StatePropCollectible from being copied and assigned.
        StatePropCollectible( const StatePropCollectible& );
        StatePropCollectible& operator=( const StatePropCollectible& );

};





#endif