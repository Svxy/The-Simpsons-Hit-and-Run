//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        wbspline.h
//
// Description: Blahblahblah
//
// History:     05/06/2002 + Created -- NAME
//
//=============================================================================

#ifndef WBSPLINE_H
#define WBSPLINE_H

//========================================
// Nested Includes
//========================================
#include "main/toolhack.h"

#include "precompiled/PCH.h"

//========================================
// Forward References
//========================================
class tlDataChunk;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class WBSpline
{
public:
    static tlDataChunk* Export( MObject& obj );

    enum { MAX_NAME_LEN = 256 };
  
    static void SetName( const char* name );
    static const char* const GetName();
    static char sName[MAX_NAME_LEN];

private:
    WBSpline();
    virtual ~WBSpline();

    //Prevent wasteful constructor creation.
    WBSpline( const WBSpline& wbspline );
    WBSpline& operator=( const WBSpline& wbspline );
};


#endif //WBSPLINE_H
