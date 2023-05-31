//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        pathmanager.cpp
//
// Description: Implements PathManager class
//
// History:     09/20/2002 + Created -- Dusit Eakkachaichanvet
//
//=============================================================================

#include <pedpaths/pathmanager.h>
#include <pedpaths/path.h>
#include <memory/srrmemory.h>

// ************************** STATICS *********************************
PathManager* PathManager::mInstance = NULL;


PathManager* PathManager::GetInstance()
{
MEMTRACK_PUSH_GROUP( "PathManager" );
    if ( !mInstance )
    {
        mInstance = new PathManager();
    }
MEMTRACK_POP_GROUP( "PathManager" );

    return mInstance;
}

void PathManager::Destroy()
{
    delete mInstance;
    mInstance = NULL;
}

// **************************** NONSTATICS *******************************

PathManager::PathManager()
{
    mPaths = NULL;
    mnPaths = 0;

    mNextFreeIndex = 0;
    // TODO: 
    // Allow automatic detection of number of paths rather than use some
    // static amount.
    AllocatePaths( MAX_PATHS );

}
PathManager::~PathManager()
{
    if( mPaths != NULL )
    {
        delete[] mPaths;
        mPaths = NULL;
    }
    mnPaths = 0;
}


void PathManager::AllocatePaths( int nPaths )
{
    MEMTRACK_PUSH_GROUP( "PathManager" );
    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_LEVEL_OTHER );
    #endif

    if( mPaths != NULL )
    {
        // dont' allow allocate to be called more than once...
        rAssert( false );
        delete[] mPaths;
    }

    mnPaths = nPaths;
    mPaths = new Path[ mnPaths ];

    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_LEVEL_OTHER);
    #endif

    MEMTRACK_POP_GROUP( "PathManager" );
}

Path* PathManager::GetFreePath()
{
    rAssert( 0 <= mNextFreeIndex && mNextFreeIndex < mnPaths );
    rAssert( mPaths != NULL );

    mNextFreeIndex++;
    return &mPaths[mNextFreeIndex-1];
}
