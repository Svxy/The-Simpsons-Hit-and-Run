//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   breakableobjectloader
//
// Description: Loads breakable objects
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef BREAKABLEOBJECTLOADER_H
#define BREAKABLEOBJECTLOADER_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <render/Loaders/IWrappedLoader.h>


//===========================================================================
// Forward References
//===========================================================================

class tAnimatedObjectFactoryLoader;
class tAnimatedObjectLoader;
class tFrameControllerLoader;
class tCompositeDrawableLoader;
class tGeometryLoader;
class tSkeletonLoader;
class tAnimationLoader;
class tParticleSystemLoader;
class tParticleSystemFactoryLoader;

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//		Loads breakableobject chunks. Essentially just a tAnimatedObjectFactory thats
//		cloned off N times.
//
// Constraints:
//
//===========================================================================
class BreakableObjectLoader
: public tSimpleChunkHandler,
  public IWrappedLoader
{
    public:
        BreakableObjectLoader();
        virtual ~BreakableObjectLoader();

    ///////////////////////////////////////////////////////////////////////
    // IWrappedLoader
    ///////////////////////////////////////////////////////////////////////
    virtual void SetRegdListener( ChunkListenerCallback* pListenerCB,
                         int   iUserData );

    virtual void ModRegdListener( ChunkListenerCallback* pListenerCB,
                         int   iUserData );

    ///////////////////////////////////////////////////////////////////////
    // tSimpleChunkHandler
    ///////////////////////////////////////////////////////////////////////
    virtual tEntity* LoadObject(tChunkFile* file, tEntityStore* store);

    protected:

    private:

        tAnimatedObjectFactoryLoader* mpFactoryLoader;
 	    tAnimatedObjectLoader* mpAnimObjectLoader;

        tFrameControllerLoader* mpControllerLoader;

	    tCompositeDrawableLoader* mpCompDrawLoader;
	    tGeometryLoader* mpP3DGeoLoader;

        tSkeletonLoader* mpSkelLoader;
        tAnimationLoader* mpAnimLoader;

        tParticleSystemLoader* mpParticleSystemLoader;
        tParticleSystemFactoryLoader* mpParticleSystemFactoryLoader;


    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow BreakableObjectLoader from being copied and assigned.
        BreakableObjectLoader( const BreakableObjectLoader& );
        BreakableObjectLoader& operator=( const BreakableObjectLoader& );
};

#endif