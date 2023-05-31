//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        mayacamera.h
//
// Description: Blahblahblah
//
// History:     22/07/2002 + Created -- CaryBrisebois
//
//=============================================================================

#ifndef MAYACAMERA_H
#define MAYACAMERA_H

//========================================
// Nested Includes
//========================================
#include "precompiled/PCH.h"

#include <radmath/radmath.hpp>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class MayaCamera
{
public:
    MayaCamera();
    virtual ~MayaCamera();

    void EnsureCamExists();
    void Set( const rmt::Vector& position, const rmt::Vector& target, const rmt::Vector& vup, float fov );

private:

    //Prevent wasteful constructor creation.
    MayaCamera( const MayaCamera& mayacamera );
    MayaCamera& operator=( const MayaCamera& mayacamera );
};


#endif //MAYACAMERA_H
