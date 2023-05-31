//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        tetreelinecommand.h
//
// Description: Blahblahblah
//
// History:     27/05/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef TETREELINECOMMAND_H
#define TETREELINECOMMAND_H

//========================================
// Nested Includes
//========================================
#include "precompiled/PCH.h"

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class SnapSelectedTreelines : public MPxCommand
{
public:
    SnapSelectedTreelines();
    virtual ~SnapSelectedTreelines();
 
    static void*    creator();
    virtual MStatus doIt( const MArgList& args );  

    static const char*  stringId;

private:

    void Recurse( MObject& obj );

    //Prevent wasteful constructor creation.
    SnapSelectedTreelines( const SnapSelectedTreelines& tetreelinecommand );
    SnapSelectedTreelines& operator=( const SnapSelectedTreelines& tetreelinecommand );
};

class ConvertTreelineToGeometry : public MPxCommand
{
public:
    ConvertTreelineToGeometry();
    ~ConvertTreelineToGeometry();

    static void*    creator();
    virtual MStatus doIt( const MArgList& args );  

    static const char*  stringId;

private:
    void Recurse( MObject& obj );

    //Prevent wasteful constructor creation.
    ConvertTreelineToGeometry( const ConvertTreelineToGeometry& tetreelinecommand );
    ConvertTreelineToGeometry& operator=( const ConvertTreelineToGeometry& tetreelinecommand );
};

class SetDeleteTreeline : public MPxCommand
{
public:
    SetDeleteTreeline();
    ~SetDeleteTreeline();

    static void*    creator();
    virtual MStatus doIt( const MArgList& args );  

    static const char*  stringId;

private:

    //Prevent wasteful constructor creation.
    SetDeleteTreeline( const SetDeleteTreeline& tetreelinecommand );
    SetDeleteTreeline& operator=( const SetDeleteTreeline& tetreelinecommand );
};
//******************************************************************************
//
// Inline Public Functions
//
//******************************************************************************

//=============================================================================
// SnapSelectedTreelines::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
inline void* SnapSelectedTreelines::creator()
{
    return new SnapSelectedTreelines();
}

//=============================================================================
// ConvertTreelineToGeometry::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
inline void* ConvertTreelineToGeometry::creator()
{
    return new ConvertTreelineToGeometry();
}

//=============================================================================
// SetDeleteTreeline::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
inline void* SetDeleteTreeline::creator()
{
    return new SetDeleteTreeline();
}
#endif //TETREELINECOMMAND_H
