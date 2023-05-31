//============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        pathloader.cpp
//
// Description: Implements PathLoader class
//
// History:     09/18/2002 + Created -- Dusit Eakkachaichanvet
//
//============================================================================

#include <p3d/chunkfile.hpp>
#include <loading/pathloader.h>
#include <pedpaths/pathmanager.h>
#include <pedpaths/path.h>
#include <pedpaths/pathsegment.h>
#include <constants/srrchunks.h>

static unsigned int sNumPathsLoaded = 0;
static unsigned int sNumPathSegmentsLoaded = 0;

PathLoader::PathLoader() :
    tSimpleChunkHandler(SRR2::ChunkID::PED_PATH)
{
   mpListenerCB  = NULL;
   mUserData     = -1;
}

PathLoader::~PathLoader() 
{
}

bool PathLoader::CheckChunkID(unsigned id)
{
    unsigned int expectedID = SRR2::ChunkID::PED_PATH;
    return expectedID == id;
}

tLoadStatus PathLoader::Load(tChunkFile* f, tEntityStore* store)
{
    // parse number of points (must be at least 2 points)
    unsigned long int nPoints = f->GetUInt();
    rAssert( nPoints >= 2 );

    int nSegments = nPoints-1;

    // Get path from PathManager, who keeps a list of Paths
    // The manager should be a singleton, so do GetInstance
    PathManager* pm = PathManager::GetInstance();
    rAssert( pm != NULL );

    // Get a free path & set up its segments
    Path* path = pm->GetFreePath();
    rAssert( path != NULL );

    path->AllocateSegments( nSegments );



    PathSegment* ps = NULL;
    rmt::Vector first, start, end;

    start.x = f->GetFloat();
    start.y = f->GetFloat();
    start.z = f->GetFloat();

    first = start;

    float myEpsilon = 0.001f;

    long int i;
    for( i=0; i<nSegments; i++ )
    {
        end.x = f->GetFloat();
        end.y = f->GetFloat();
        end.z = f->GetFloat();

        // make sure that the points span a tangible distance
        rAssert( !rmt::Epsilon( start.x, end.x, myEpsilon ) || 
                 !rmt::Epsilon( start.y, end.y, myEpsilon ) || 
                 !rmt::Epsilon( start.z, end.z, myEpsilon ) );

        ps = path->GetPathSegmentByIndex( i );
        ps->Initialize( path, i, start, end );
   
        // DSG stuff
        mpListenerCB->OnChunkLoaded( ps, mUserData, SRR2::ChunkID::PED_PATH_SEGMENT );
        start = end;

        sNumPathSegmentsLoaded++;
    }
    /*
    int counter = 0;
    long int i;
    for( i=0; i<(nSegments-1); i = i+2 )
    {
        f->GetFloat(); f->GetFloat(); f->GetFloat();
        end.x = f->GetFloat();
        end.y = f->GetFloat();
        end.z = f->GetFloat();

        // make sure that the points span a tangible distance
        rAssert( !rmt::Epsilon( start.x, end.x, myEpsilon ) || 
                 !rmt::Epsilon( start.y, end.y, myEpsilon ) || 
                 !rmt::Epsilon( start.z, end.z, myEpsilon ) );

        ps = path->GetPathSegmentByIndex( counter );
        ps->Initialize( path, counter, start, end );
   
        // DSG stuff
        mpListenerCB->OnChunkLoaded( ps, mUserData, SRR2::ChunkID::PED_PATH_SEGMENT );
        start = end;

        counter++;
        sNumPathSegmentsLoaded++;
    }
    */

    // determine if path is closed or open
    if( rmt::Epsilon( first.x, end.x, myEpsilon ) &&
        rmt::Epsilon( first.y, end.y, myEpsilon ) &&
        rmt::Epsilon( first.z, end.z, myEpsilon ) )
    {
        path->SetIsClosed( true );
    }

    sNumPathsLoaded++;

    /*
    rDebugPrintf( "Pathsegments Loaded = %d, paths loaded = %d, sizeof segments = %d, sizeof paths = %d\n", 
        sNumPathSegmentsLoaded, 
        sNumPathsLoaded, 
        sizeof(PathSegment) * sNumPathSegmentsLoaded, 
        sizeof(Path) * sNumPathsLoaded );
    */

    return LOAD_OK;
}




tEntity* PathLoader::LoadObject(tChunkFile* f, tEntityStore* store)
{
    return NULL;
}


///////////////////////////////////////////////////////////////////////
// IWrappedLoader
///////////////////////////////////////////////////////////////////////
//========================================================================
// RoadLoader::SetRegdListener
//========================================================================
//
// Description: Register a new listener/caretaker, notify old listener of 
//              severed connection.
//
// Parameters:  pListenerCB: Callback to call OnChunkLoaded
//              pUserData:   Data to pass along for filtering, etc
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void PathLoader::SetRegdListener
(
   ChunkListenerCallback* pListenerCB,
   int iUserData 
)
{
   //
   // Follow protocol; notify old Listener, that it has been 
   // "disconnected".
   //
   if( mpListenerCB != NULL )
   {
      mpListenerCB->OnChunkLoaded( NULL, iUserData, 0 );
   }
   mpListenerCB  = pListenerCB;
   mUserData     = iUserData;
}

//========================================================================
// RoadLoader::ModRegdListener
//========================================================================
//
// Description: Just fuck with the current pUserData
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
void PathLoader::ModRegdListener
( 
   ChunkListenerCallback* pListenerCB,
   int iUserData 
)
{
#if 0
   char DebugBuf[255];
   sprintf( DebugBuf, "PathLoader::ModRegdListener: pListenerCB %X vs mpListenerCB %X\n", 
       pListenerCB, mpListenerCB );
   rDebugString( DebugBuf );
#endif
   rAssert( pListenerCB == mpListenerCB );

   mUserData = iUserData;
}

