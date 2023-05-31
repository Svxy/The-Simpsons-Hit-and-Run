//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        pathloader.h
//
// Description: Defines PathLoader class
//
// History:     09/18/2002 + Created -- Dusit Eakkachaichanvet
//
//=============================================================================

#ifndef PATHLOADER_H
#define PATHLOADER_H

#include <p3d/loadmanager.hpp>
#include <p3d/p3dtypes.hpp>
#include <render/Loaders/IWrappedLoader.h>



class PathLoader : 
    public tSimpleChunkHandler, 
    public IWrappedLoader
{
//MEMBERS
public:

//METHODS
public:
    PathLoader();
    virtual ~PathLoader();

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
    ///////////////////////////////////////////////////////////////////////
    // tSimpleChunkHandler
    ///////////////////////////////////////////////////////////////////////
    virtual tEntity* LoadObject(tChunkFile* f, tEntityStore* store);
    ///////////////////////////////////////////////////////////////////////



//MEMBERS
private:
    
    // prevent wasteful copy constructors
    PathLoader( const PathLoader& );
    PathLoader& operator= ( const PathLoader& );

};

// *********************************** INLINES *******************************




#endif //PATHLOADER_H