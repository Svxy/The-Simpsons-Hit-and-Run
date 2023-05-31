//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        followcamdatachunk.h
//
// Description: Blahblahblah
//
// History:     17/06/2002 + Created -- NAME
//
//=============================================================================

#ifndef FOLLOWCAMDATACHUNK_H
#define FOLLOWCAMDATACHUNK_H

//========================================
// Nested Includes
//========================================
#include <p3d/entity.hpp>
#include <radmath/radmath.hpp>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class FollowCamDataChunk
{
public:
    FollowCamDataChunk() {};
    virtual ~FollowCamDataChunk() { SetName( NULL ); };

    void SetName( const char* name ) { mName.SetText( name ); };

    unsigned int mID;

    float mRotation;
    float mElevation;
    float mMagnitude;

    rmt::Vector mTargetOffset;

    tName mName;
    
    FollowCamDataChunk& operator=( const FollowCamDataChunk& followcamdatachunk );
    bool operator==( const FollowCamDataChunk& followcamdatachunk );
private:
    //Prevent wasteful constructor creation.
    FollowCamDataChunk( const FollowCamDataChunk& followcamdatachunk );
};

//=============================================================================
// ::operator=
//=============================================================================
// Description: Comment
//
// Parameters:  ( const FollowCamDataChunk& followcamdatachunk )
//
// Return:      FollowCamDataChunk
//
//=============================================================================
inline FollowCamDataChunk& FollowCamDataChunk::operator=( const FollowCamDataChunk& followcamdatachunk )
{
    mID = followcamdatachunk.mID;
    mRotation = followcamdatachunk.mRotation;
    mElevation = followcamdatachunk.mElevation;
    mMagnitude = followcamdatachunk.mMagnitude;

    mTargetOffset = followcamdatachunk.mTargetOffset;

    mName = followcamdatachunk.mName;

    return *this;
}

//=============================================================================
// ::operator==
//=============================================================================
// Description: Comment
//
// Parameters:  ( const FollowCamDataChunk& followcamdatachunk )
//
// Return:      bool 
//
//=============================================================================
inline bool FollowCamDataChunk::operator==( const FollowCamDataChunk& followcamdatachunk )
{
    return ( mID == followcamdatachunk.mID &&
             mRotation == followcamdatachunk.mRotation &&
             mElevation == followcamdatachunk.mElevation &&
             mMagnitude == followcamdatachunk.mMagnitude &&
             mTargetOffset == followcamdatachunk.mTargetOffset &&
             mName == followcamdatachunk.mName );
}

#endif //FOLLOWCAMDATACHUNK_H
