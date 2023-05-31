//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        intersectionloader.cpp
//
// Description: Implement IntersectionLoader
//
// History:     14/03/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>
#include <p3d/chunkfile.hpp>
#include <p3d/inventory.hpp>

//========================================
// Project Includes
//========================================
#include <loading/IntersectionLoader.h>
#include <roads/intersection.h>

#include <constants/srrchunks.h>

//STUB!!
#include <roads/roadmanager.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************


//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// IntersectionLoader::IntersectionLoader
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
IntersectionLoader::IntersectionLoader()
{
}

//==============================================================================
// IntersectionLoader::~IntersectionLoader
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
IntersectionLoader::~IntersectionLoader()
{
}

//==============================================================================
// IntersectionLoader::Load
//==============================================================================
// Description: This is a P3D chunk loader for Intersections.  This will create 
//              a simple intersection and put it in the inventory.
//
// Parameters:  (tChunkFile* f, tEntityStore* store)
//
// Return:      tLoadStatus 
//
//==============================================================================
tLoadStatus IntersectionLoader::Load(tChunkFile* f, tEntityStore* store)
{
    char name[256];
    f->GetString( name );

    rmt::Vector loc;
    loc.x = f->GetFloat();
    loc.y = f->GetFloat();
    loc.z = f->GetFloat();

    float radius = f->GetFloat();

    unsigned int type = f->GetUInt();

    RoadManager* rm = RoadManager::GetInstance();

    Intersection* intersection = NULL;

    //First see if we've already got this one!
    intersection = rm->FindIntersection( name );

    if ( NULL == intersection )
    {
        //Not here yet.
        intersection = rm->GetFreeIntersectionMemory();
        rAssert( intersection );

        intersection->SetName( name );
        intersection->SetType( (Intersection::Type)type );
        intersection->SetRadius( radius );
        intersection->SetLocation( loc );

        rm->AddIntersection( intersection );    
    }

#ifdef TOOLS
    store->Store( intersection );
    //HACK
    intersection->AddRef();
#endif

    return LOAD_OK;
}


//==============================================================================
// IntersectionLoader::CheckChunkID
//==============================================================================
// Description: Comment
//
// Parameters:  (unsigned id)
//
// Return:      bool 
//
//==============================================================================
bool IntersectionLoader::CheckChunkID(unsigned id)
{
    return SRR2::ChunkID::INTERSECTION == id;
}
    
unsigned int IntersectionLoader::GetChunkID()
{
	return SRR2::ChunkID::INTERSECTION;
}
    

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************




