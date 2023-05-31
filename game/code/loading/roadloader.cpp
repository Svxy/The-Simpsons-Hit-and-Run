//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        roadloader.cpp
//
// Description: Implement RoadLoader
//
// History:     15/03/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <p3d/chunkfile.hpp>
#include <p3d/inventory.hpp>
#include <p3d/entity.hpp>
#include <radmath/radmath.hpp>


#ifndef TOOLS
#include <memory/srrmemory.h>
#else
#define MEMTRACK_PUSH_GROUP(x)
#define MEMTRACK_POP_GROUP()
#endif

//========================================
// Project Includes
//========================================
#include <loading/roadloader.h>
#include <roads/roadmanager.h>
#include <roads/intersection.h>
#include <roads/roadsegmentdata.h>
#include <roads/roadsegment.h>
#include <roads/road.h>

#include <constants/srrchunks.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
static unsigned int sNumRoadsLoaded = 0;
static unsigned int sNumRoadSegmentsLoaded = 0;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// RoadLoader::RoadLoader
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
RoadLoader::RoadLoader() :
tSimpleChunkHandler(SRR2::ChunkID::ROAD)
{
   mpListenerCB  = NULL;
   mUserData     = -1;

}

//==============================================================================
// RoadLoader::~RoadLoader
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
RoadLoader::~RoadLoader()
{
}

//==============================================================================
// RoadLoader::Load
//==============================================================================
// Description: Comment
//
// Parameters:  (tChunkFile* f, tEntityStore* store)
//
// Return:      tLoadStatus
//
//==============================================================================
tLoadStatus RoadLoader::Load(tChunkFile* f, tEntityStore* store)
{
MEMTRACK_PUSH_GROUP( "Road Loading" );

    char name[256];
    f->GetPString( name );
    unsigned int type = f->GetUInt();

    RoadManager* rm = RoadManager::GetInstance();
    rAssert( rm );

    char start[256];
    f->GetString( start );
    Intersection* startIntersection = rm->FindIntersection( tEntity::MakeUID( start ) );
    rAssert( startIntersection );

    char end[256];
    f->GetString( end );
    Intersection* endIntersection = rm->FindIntersection( tEntity::MakeUID( end ) );
    rAssert( endIntersection );

    unsigned int density = f->GetUInt();

    unsigned int speed = f->GetUInt();


    RoadList segments;

    bool firstSeg = true;
    unsigned int numLanes = 0;

    if ( !f->ChunksRemaining() )
    {
        char error[256];
        sprintf( error, "The Road: %s has no segments!\n", name );
        rDebugString( error );
MEMTRACK_POP_GROUP( "Road Loading" );
        return LOAD_OK;
    }

    unsigned int segCount = 0;
    while ( f->ChunksRemaining() )
    {
        f->BeginChunk();

        unsigned int tmpNumLanes = 0;
        RoadSegment* segment = LoadRoadSegment( f, tmpNumLanes );

        if( firstSeg )
        {
            numLanes = tmpNumLanes;
        }
        else
        {
            rAssert( numLanes == tmpNumLanes );  //Should have the same number of lanes.
        }

        segments.push_back( segment );
        ++segCount;

        f->EndChunk();
    }

    Road* road = rm->GetFreeRoadMemory();
    rAssert( road );
    
#ifndef RAD_RELEASE
    road->SetName( name );
#endif

    rm->AddRoad( road );
    
    road->SetDestinationIntersection( endIntersection );
    road->SetSourceIntersection( startIntersection );
    road->SetNumLanes( numLanes );

    //This works differently now.
    //8 bits  - speed
    //8 bits  - difficulty level
    //1 bit   - id Short cut
    //15 bits - saved for later
    const int SPEED_MASK = 0x000000FF;
    const int DIFFIC_MASK = 0x0000FF00;
    const int SC_MASK = 0x00010000;

    road->SetSpeed( speed & SPEED_MASK );
    road->SetDifficulty( (speed & DIFFIC_MASK) >> 8 );
    road->SetShortCut( (speed & SC_MASK) ? true : false );
    road->SetDensity( density );
    
    startIntersection->AddRoadOut( road );
    endIntersection->AddRoadIn( road );

    road->AllocateSegments( segments.size() );


    //Now we sort the road segments to make sure they are in order from the 
    //starting intersection...


    RoadList::iterator i;

    //Let's sort these.
    float dist = 10000000000.0f;

    RoadSegment* closest = NULL;
    rmt::Vector startPoint;
    startIntersection->GetLocation( startPoint );

    for( i = segments.begin(); i != segments.end(); i++ )
    {
        RoadSegment* segment = (*i);

        rmt::Vector origin;
        segment->GetCorner( 0, origin );

        origin.Sub( startPoint );
        float segToInt = origin.MagnitudeSqr();

        if ( segToInt < dist )
        {
            closest = segment;
            dist = segToInt;
        }
    }

    unsigned int numAllocated = 0;
    RoadList allocatedSegments;

    allocatedSegments.push_back( closest );
    ++numAllocated;

    //Now that we have the closest, let's find them in order....

    RoadSegment* current = closest;

    while ( allocatedSegments.size() < segments.size() )
    {
        bool found = false;

        for ( i = segments.begin(); i != segments.end(); i++ )
        {
            RoadSegment* seg = *i;

            rmt::Vector currentTrailingLeft;
            rmt::Vector segOrigin;

            current->GetCorner( 1, currentTrailingLeft );
            seg->GetCorner( 0, segOrigin );

            if ( rmt::Epsilon( currentTrailingLeft.x, segOrigin.x, 0.5f ) &&
                 rmt::Epsilon( currentTrailingLeft.y, segOrigin.y, 0.5f ) &&
                 rmt::Epsilon( currentTrailingLeft.z, segOrigin.z, 0.5f ) )
            {
                //This is the next segment.
                current = seg;
                allocatedSegments.push_back( seg );
                ++numAllocated;

                found = true; 

                break;
            }
        }


        if ( !found )
        {
            bool error = true;
            //This could be the case where the intersection is of the shape where the origin is further away than the next segments.
            //If there is only on segment missing, then test this theory out.
            if ( segCount - numAllocated == 1 )
            {
                //This is the strange case of the missing first segment.
                //Find the missing segment and try to match it to the first element in the allocated segs list.
                //if they match then add the found one to the start of the list.
                for ( i = segments.begin(); i != segments.end(); i++ )
                {
                    bool notTheMissingOne = false;
                    RoadList::iterator j;
                    for ( j = allocatedSegments.begin(); j != allocatedSegments.end(); j++ )
                    {
                        if ( (*i) == (*j) )
                        {
                            notTheMissingOne = true;
                            break;
                        }
                    }

                    if ( !notTheMissingOne )
                    {
                        //AHA!
                        //segment i is not allocated!
                        //Test it's trailing left against the first one's origin.
                        rmt::Vector trailingLeft;
                        rmt::Vector origin;

                        (*i)->GetCorner( 1, trailingLeft );
                        (*(allocatedSegments.begin()))->GetCorner( 0, origin );

                        if ( rmt::Epsilon( trailingLeft.x, origin.x, 0.5f ) &&
                             rmt::Epsilon( trailingLeft.y, origin.y, 0.5f ) &&
                             rmt::Epsilon( trailingLeft.z, origin.z, 0.5f ) )
                        {
                            //This is the next segment.
                            allocatedSegments.push_front( (*i) );
                            ++numAllocated;
                            error = false;
                        }
                    }
                }
            }

            if ( error )
            {
            #ifndef RAD_RELEASE
                char message[500];
                sprintf( message, 
                    "\b ERROR! The road segment \"%s\" is not touching any others...\n"
                    "    Found: %d out of %d\n"
                    "    Possibly due to duplicated segments\n"
                    "    Please note the errors above and inform Cary/Dusit/Sheik\n"
                    "    These errors are fatal! Skipping will result in crash\n"
                    "    and/or lousy AI behaviour.\n", 
                    current->GetName(), 
                    numAllocated,
                    segCount );
                rTuneAssertMsg( false, message );
            #endif
            }
             
            break;
        }
    }

    float roadLen = 0.0f;
    for ( i = allocatedSegments.begin(); i != allocatedSegments.end(); i++ )
    {
        //
        // Tell the segment which road it belongs to as well as tell the road 
        // it contains the segment.
        //
        RoadSegment* seg = *i;
        seg->SetRoad( road );
        road->AddRoadSegment( (*i) );

        float segLen = seg->GetSegmentLength(); 
        roadLen += segLen;

#ifdef TOOLS
        store->Store( seg );
        seg->AddRef();
#endif
    }

    rAssert( roadLen >= 0.0f );
    road->SetRoadLength( roadLen );
    road->CreateLanes();

MEMTRACK_POP_GROUP( "Road Loading" );
        
#ifdef TOOLS
    road->SetName( name );
    store->Store( road );
    //HACK
    road->AddRef();
#endif

    sNumRoadsLoaded++;

    /*
    rDebugPrintf( "*** RoadSegments loaded = %d, Roads loaded = %d, sizeof segments = %d, sizeof roads = %d\n", 
        sNumRoadSegmentsLoaded, 
        sNumRoadsLoaded, 
        sizeof(RoadSegment) * sNumRoadSegmentsLoaded, 
        sizeof(Road) * sNumRoadsLoaded );
    */

    return LOAD_OK;
}

//==============================================================================
// RoadLoader::CheckChunkID
//==============================================================================
// Description: Comment
//
// Parameters:  (unsigned id)
//
// Return:      bool
//
//==============================================================================
bool RoadLoader::CheckChunkID(unsigned id)
{
    return SRR2::ChunkID::ROAD == id;
}



///////////////////////////////////////////////////////////////////////
// tSimpleChunkHandler
///////////////////////////////////////////////////////////////////////
//========================================================================
// RoadLoader::
//========================================================================
//
// Description: 
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: None.
//
//========================================================================
tEntity* RoadLoader::LoadObject(tChunkFile* f, tEntityStore* store)
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
void RoadLoader::SetRegdListener
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
void RoadLoader::ModRegdListener
( 
   ChunkListenerCallback* pListenerCB,
   int iUserData 
)
{
#if 0
   char DebugBuf[255];
   sprintf( DebugBuf, "RoadLoader::ModRegdListener: pListenerCB %X vs mpListenerCB %X\n", 
       pListenerCB, mpListenerCB );
   rDebugString( DebugBuf );
#endif
   rAssert( pListenerCB == mpListenerCB );

   mUserData = iUserData;
}




//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

RoadSegment* RoadLoader::LoadRoadSegment( tChunkFile* f, unsigned int& numLanes )
{
MEMTRACK_PUSH_GROUP( "RoadSeg Loader" );

    rAssert( f->GetCurrentID() == SRR2::ChunkID::ROAD_SEGMENT );

    char name[256];
    f->GetString( name );

    char segDataName[256];
    f->GetString( segDataName );

    // grab the matrix
    rmt::Matrix hierarchy;
    f->GetData( &hierarchy, 16, tFile::DWORD );

    // grab the scale along facing
    rmt::Matrix scale;
    f->GetData( &scale, 16, tFile::DWORD );
    rmt::Vector z( 0,0,1.0f );
    z.Transform( scale );
    float scaleAlongFacing = z.z;


    RoadManager* rm = RoadManager::GetInstance();
  
    RoadSegmentData* rsd = rm->FindRoadSegmentData( segDataName );
    rAssert( rsd );
  
    RoadSegment* rs = rm->GetFreeRoadSegmentMemory();
    rAssert( rs );

    rs->SetName( name );
    rs->Init( rsd, hierarchy, scaleAlongFacing );

    // increment count in road manager
    rm->AddRoadSegment( rs );

    numLanes = rsd->GetNumLanes();

MEMTRACK_POP_GROUP( "RoadSeg Loader" );

    // DSG stuff
#ifndef TOOLS
    mpListenerCB->OnChunkLoaded( rs, mUserData, SRR2::ChunkID::ROAD_SEGMENT );
#endif

    sNumRoadSegmentsLoaded++;

    return rs;
}

