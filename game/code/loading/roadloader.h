//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        roadloader.h
//
// Description: This is the P3D Loader class for Road chunks.
//
// History:     15/03/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef ROADLOADER_H
#define ROADLOADER_H

//========================================
// Nested Includes
//========================================
#include <p3d/loadmanager.hpp>
#include <p3d/p3dtypes.hpp>
#include <render/Loaders/IWrappedLoader.h>

#include <list>
#include <memory/stlallocators.h>


//========================================
// Forward References
//========================================

class RoadSegment;

//=============================================================================
//
// Synopsis:    This is the P3D Loader class for Road chunks.
//
//=============================================================================


class RoadLoader : 
  public tSimpleChunkHandler,
  public IWrappedLoader 

{
public:
    RoadLoader();
    virtual ~RoadLoader();

#ifndef TOOLS
    typedef std::list< RoadSegment*, s2alloc<RoadSegment*> > RoadList;
#else
    typedef std::list< RoadSegment*> RoadList;
#endif

    // P3D chunk loader.
    virtual tLoadStatus Load(tChunkFile* f, tEntityStore* store);

    // P3D chunk id.
    virtual bool CheckChunkID(unsigned id);

    
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



private:

    RoadSegment* LoadRoadSegment( tChunkFile* f, unsigned int& numLanes );

    // No copying or assignment. Declare but don't define.
    //
    RoadLoader( const RoadLoader& );
    RoadLoader& operator= ( const RoadLoader& );
};

#endif //ROADLOADER_H