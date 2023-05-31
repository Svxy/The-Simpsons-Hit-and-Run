//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   AnimDSGLoader
//
// Description: Loads animated DSG objects that do NOT have collision data 
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef ANIMDSGLOADER_H
#define ANIMDSGLOADER_H


//===========================================================================
// Nested Includes
//===========================================================================
#include <render/Loaders/IWrappedLoader.h>


class tCompositeDrawableLoader;
class tMultiControllerLoader;
class tFrameControllerLoader;
class AnimEntityDSG;
class tEntityStore;
class tMultiController;

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
class AnimDSGLoader 
: public tSimpleChunkHandler,
  public IWrappedLoader 
{
public:
   AnimDSGLoader();
   virtual ~AnimDSGLoader();
 
   ///////////////////////////////////////////////////////////////////////
   // IWrappedLoader
   ///////////////////////////////////////////////////////////////////////
   void SetRegdListener( ChunkListenerCallback* pListenerCB,
                         int   iUserData );

   void ModRegdListener( ChunkListenerCallback* pListenerCB,
                         int   iUserData );

   ///////////////////////////////////////////////////////////////////////
   // tSimpleChunkHandler
   ///////////////////////////////////////////////////////////////////////
   virtual tEntity* LoadObject(tChunkFile* file, tEntityStore* store);

protected:
   tCompositeDrawableLoader*    mpCompDLoader;
   tMultiControllerLoader*      mpMCLoader;
   tFrameControllerLoader*		mpFCLoader;
   ///////////////////////////////////////////////////////////////////////
   // IWrappedLoader
   ///////////////////////////////////////////////////////////////////////
   //ChunkListenerCallback*  mpListenerCB;
   //void* mpUserData;

    AnimEntityDSG* LoadAnimAtIntersections( tCompositeDrawable* ipCompD, 
                                            tMultiController* ipAnimMC, 
                                            tEntityStore* ipStore,
                                            bool iHasAlpha,
                                            char* ipName,
                                            int   iLinkEnum);

private:
};


#endif
