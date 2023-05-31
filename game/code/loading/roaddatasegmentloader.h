//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        roaddatasegmentloader.h
//
// Description: Blahblahblah
//
// History:     27/06/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef ROADDATASEGMENTLOADER_H
#define ROADDATASEGMENTLOADER_H

//========================================
// Nested Includes
//========================================
// Pure 3D
#include <p3d/loadmanager.hpp>
#include <p3d/p3dtypes.hpp>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class RoadDataSegmentLoader : public tChunkHandler
{
public:
    RoadDataSegmentLoader();
    virtual ~RoadDataSegmentLoader();

    // P3D chunk loader.
    virtual tLoadStatus Load(tChunkFile* f, tEntityStore* store);

    // P3D chunk id.
    virtual bool CheckChunkID(unsigned id);
    virtual unsigned int GetChunkID();

private:

    //Prevent wasteful constructor creation.
    RoadDataSegmentLoader( const RoadDataSegmentLoader& roaddatasegmentloader );
    RoadDataSegmentLoader& operator=( const RoadDataSegmentLoader& roaddatasegmentloader );
};


#endif //ROADDATASEGMENTLOADER_H
