//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        walkercamdatachunk.h
//
// Description: Blahblahblah
//
// History:     17/06/2002 + Created -- NAME
//
//=============================================================================

#ifndef WALKERCAMDATACHUNK_H
#define WALKERCAMDATACHUNK_H

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

class WalkerCamDataChunk : public tEntity
{
public:
    WalkerCamDataChunk() {};
    virtual ~WalkerCamDataChunk() {};

    unsigned int mID;

    float mMinMagnitude;
    float mMaxMagnitude;
    float mElevation;

    rmt::Vector mTargetOffset;

private:

    //Prevent wasteful constructor creation.
    WalkerCamDataChunk( const WalkerCamDataChunk& walkercamdatachunk );
    WalkerCamDataChunk& operator=( const WalkerCamDataChunk& walkercamdatachunk );
};


#endif //WALKERCAMDATACHUNK_H
