//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   BreakableObjectDSG
//
// Description: Breakable object that can be placed in the DSG
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef BREAKABLEOBJECTDSG_H
#define BREAKABLEOBJECTDSG_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <render/dsg/inststatentitydsg.h>

//===========================================================================
// Forward References
//===========================================================================

class tAnimatedObject;
class tAnimatedObjectFactory;
class tAnimatedObjectFrameController;

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//      Generally, describe what behaviour this class possesses that
//      clients can rely on, and the actions that this service
//      guarantees to clients.
//
// Constraints:
//      Describe here what you require of the client which uses or
//      has this class - essentially, the converse of the description
//      above. A constraint is an expression of some semantic
//      condition that must be preserved, an invariant of a class or
//      relationship that must be preserved while the system is in a
//      steady state.
//
//===========================================================================
class BreakableObjectDSG : public InstStatEntityDSG
{
    public:
        BreakableObjectDSG();
        ~BreakableObjectDSG();
       
        void Init( tAnimatedObjectFactory* pFactory, tAnimatedObjectFrameController* pController);

        // Update time in milliseconds
        void Update( float deltaTime );
        void SetTransform( const rmt::Matrix& transform );
        
        void Reset();
        int LastFrameReached();
 

        virtual void Display();
        virtual void GetBoundingBox(rmt::Box3D* box);
        virtual void GetBoundingSphere(rmt::Sphere* pSphere);

        virtual rmt::Vector* pPosition();
        virtual const rmt::Vector& rPosition();
        virtual void GetPosition( rmt::Vector* ipPosn );

    protected:

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow BreakableObjectDSG from being copied and assigned.
        BreakableObjectDSG( const BreakableObjectDSG& );
        BreakableObjectDSG& operator=( const BreakableObjectDSG& );

        tAnimatedObject* mpObject;
        tAnimatedObjectFrameController* mpController;


};

#endif