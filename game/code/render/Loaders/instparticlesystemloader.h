//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   InstParticleSystemLoader
//
// Description: Loads particle systems (and also breakable objects)
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef INSTPARTICLESYSTEMLOADER_H
#define INSTPARTICLESYSTEMLOADER_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <render/Loaders/IWrappedLoader.h>


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
class InstParticleSystemLoader
: public tSimpleChunkHandler,
  public IWrappedLoader
{
    public:
        InstParticleSystemLoader();
        virtual ~InstParticleSystemLoader();

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
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow InstParticleSystemLoader from being copied and assigned.
        InstParticleSystemLoader( const InstParticleSystemLoader& );
        InstParticleSystemLoader& operator=( const InstParticleSystemLoader& );
};

#endif