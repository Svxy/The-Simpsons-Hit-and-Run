//=============================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   VirtualInput
//
// Description: Small helper class for dealing with virtual inputs.
//
// History:     Created 19/08/03.
//
//=============================================================================

#ifndef VIRTUALINPUTS_HPP
#define VIRTUALINPUTS_HPP

enum eMapType
{
    MAP_CHARACTER = 0,
    MAP_VEHICLE,
    MAP_FRONTEND,
    NUM_MAPTYPES
};

class VirtualInputs
{
public:
    static const char* GetName( int VirtualInput );
    static eMapType GetType( int VirtualInput );
    static int GetNumber();
};

#endif