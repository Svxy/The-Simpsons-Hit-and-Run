//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        RoadDataSegmentLoader.cpp
//
// Description: Implement RoadDataSegmentLoader
//
// History:     27/06/2002 + Created -- Cary Brisebois  
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

//========================================
// Project Includes
//========================================
#include <loading/RoadDataSegmentLoader.h>
#include <constants/srrchunks.h>
#include <roads/roadmanager.h>
#include <roads/roadsegmentdata.h>

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
// RoadDataSegmentLoader::RoadDataSegmentLoader
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
RoadDataSegmentLoader::RoadDataSegmentLoader()
{
}

//==============================================================================
// RoadDataSegmentLoader::~RoadDataSegmentLoader
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
RoadDataSegmentLoader::~RoadDataSegmentLoader()
{
}

//=============================================================================
// RoadDataSegmentLoader::Load
//=============================================================================
// Description: Comment
//
// Parameters:  (tChunkFile* f, tEntityStore* store)
//
// Return:      tLoadStatus 
//
//=============================================================================
tLoadStatus RoadDataSegmentLoader::Load(tChunkFile* f, tEntityStore* store)
{
    char name[256];
    f->GetString( name );
    unsigned int type = f->GetUInt();
    unsigned int numLanes = f->GetUInt();
    bool hasShoulder = ( f->GetUInt() ? true : false );

    rmt::Vector direction;
    direction.x = f->GetFloat();
    direction.y = f->GetFloat();
    direction.z = f->GetFloat();

    rmt::Vector top;
    top.x = f->GetFloat();
    top.y = f->GetFloat();
    top.z = f->GetFloat();

    rmt::Vector bottom;
    bottom.x = f->GetFloat();
    bottom.y = f->GetFloat();
    bottom.z = f->GetFloat();

    RoadManager* rm = RoadManager::GetInstance();

    RoadSegmentData* rsd = rm->GetFreeRoadSegmentDataMemory();
    rAssert( rsd );

    rsd->SetName( name );
    rsd->SetData( direction, top, bottom, numLanes );

    rm->AddRoadSegmentData( rsd );

    return LOAD_OK;
}

//=============================================================================
// RoadDataSegmentLoader::CheckChunkID
//=============================================================================
// Description: Comment
//
// Parameters:  (unsigned id)
//
// Return:      bool 
//
//=============================================================================
bool RoadDataSegmentLoader::CheckChunkID(unsigned id)
{
    return ( SRR2::ChunkID::ROAD_SEGMENT_DATA == id );
}

unsigned int RoadDataSegmentLoader::GetChunkID()
{
	return SRR2::ChunkID::ROAD_SEGMENT_DATA;
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
