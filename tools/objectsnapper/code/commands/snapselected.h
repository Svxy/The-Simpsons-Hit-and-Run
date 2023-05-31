//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        snapselectedcmd.h
//
// Description: Blahblahblah
//
// History:     18/03/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef SNAPSELECTEDCMD_H
#define SNAPSELECTEDCMD_H

//========================================
// Nested Includes
//========================================
#include "main/mayaincludes.h"
#include "precompiled/PCH.h"

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class SnapSelectedCmd : public MPxCommand
{
public:
    SnapSelectedCmd();
    virtual ~SnapSelectedCmd();

    static void* creator();
    virtual MStatus doIt( const MArgList& args );
    virtual MStatus undoIt();
    virtual MStatus redoIt();
    virtual bool isUndoable() const;

    static const char* stringId;

private:

    MDagPathArray   mObjDagPaths;
    MVectorArray    mOldPositions;
    MVectorArray    mNewPositions;
    
    void GetChildMeshNames( MDagPath objDagPath, MStringArray& names );

    //Prevent wasteful constructor creation.
SnapSelectedCmd( const SnapSelectedCmd& snapselectedcmd );
SnapSelectedCmd& operator=( const SnapSelectedCmd& snapselectedcmd );
};

#endif //SNAPSELECTEDCMD_H

