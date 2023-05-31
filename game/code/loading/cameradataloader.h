//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        cameradataloader.h
//
// Description: Blahblahblah
//
// History:     17/06/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef CAMERADATALOADER_H
#define CAMERADATALOADER_H

//========================================
// Nested Includes
//========================================
#include <constants/srrchunks.h>
#include <p3d/loadmanager.hpp>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class CameraDataLoader : public tChunkHandler
{
public:
    CameraDataLoader();
    virtual ~CameraDataLoader();

    // P3D chunk loader.
    virtual tLoadStatus Load(tChunkFile* f, tEntityStore* store);

    // P3D chunk id.
    virtual bool CheckChunkID(unsigned id);
    virtual unsigned int GetChunkID();

private:

    //Prevent wasteful constructor creation.
    CameraDataLoader( const CameraDataLoader& cameradataloader );
    CameraDataLoader& operator=( const CameraDataLoader& cameradataloader );
};

//******************************************************************************
//
// Inline Public Member Functions
//
//******************************************************************************

//=============================================================================
// CameraDataLoader::CheckChunkID
//=============================================================================
// Description: Comment
//
// Parameters:  (unsigned id)
//
// Return:      bool 
//
//=============================================================================
inline bool CameraDataLoader::CheckChunkID(unsigned id)
{
    return( SRR2::ChunkID::FOLLOWCAM == id || SRR2::ChunkID::WALKERCAM == id );
}

inline unsigned int CameraDataLoader::GetChunkID()
{
	return SRR2::ChunkID::FOLLOWCAM | SRR2::ChunkID::WALKERCAM;
}

#endif //CAMERADATALOADER_H
