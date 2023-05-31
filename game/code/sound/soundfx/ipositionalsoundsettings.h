//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        ipositionalsoundsettings.h
//
// Description: RadScript interface for positional sound settings
//
// History:     12/20/2002 + Created -- Darren
//
//=============================================================================

#ifndef IPOSITIONALSOUNDSETTINGS_H
#define IPOSITIONALSOUNDSETTINGS_H

//========================================
// Nested Includes
//========================================
#include <radobject.hpp>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    IPositionalSoundSettings
//
//=============================================================================

class IPositionalSoundSettings : public IRefCount
{
    //
    // Name of sound resource to play
    //
    virtual void SetClipName( const char* clipName ) = 0;

    //
    // Distance at which volume is maxed
    //
    virtual void SetMinDistance( float min ) = 0;

    //
    // Distance at which volume reaches zero
    //
    virtual void SetMaxDistance( float max ) = 0;

    //
    // Probability of sound playing
    //
    virtual void SetPlaybackProbability( float prob ) = 0;
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

#endif //IPOSITIONALSOUNDSETTINGS_H
