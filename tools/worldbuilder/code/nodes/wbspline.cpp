//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        WBSpline.cpp
//
// Description: Implement WBSpline
//
// History:     05/06/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
#include "main/toolhack.h"
#include <toollib.hpp>

//========================================
// Project Includes
//========================================
#include "nodes/WBSpline.h"
#include "nodes/triggervolumenode.h"
#include "main/constants.h"


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
char WBSpline::sName[MAX_NAME_LEN];


//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

void WBSpline::SetName( const char* name )
{
    strncpy( sName, name, MAX_NAME_LEN );
}

const char* const WBSpline::GetName()
{
    return sName;
}

//=============================================================================
// WBSpline::Export
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& obj )
//
// Return:      tlDataChunk
//
//=============================================================================
tlDataChunk* WBSpline::Export( MObject& obj )
{
    MStatus status;

    MFnDagNode fnDagNode( obj );
    MDagPath dagPath;
    fnDagNode.getPath( dagPath );

    MFnNurbsCurve fnNurbs( dagPath, &status );
    assert( status );

    if ( status )
    {
       //Create a tlDataChunk and return it filled with the appropriate data.
        tlWBSplineChunk* spline = new tlWBSplineChunk;

        spline->SetName( fnNurbs.name().asChar() );

        int numCVs = fnNurbs.numCVs( &status );
        assert( status );

        MPointArray cvs;
        fnNurbs.getCVs( cvs, MSpace::kWorld );

        tlPoint* tlCVs = new tlPoint[numCVs];

        int i;
        for ( i = 0; i < numCVs; ++i )
        {
            tlCVs[i].x = cvs[i].x / WBConstants::Scale;
            tlCVs[i].y = cvs[i].y / WBConstants::Scale;
            tlCVs[i].z = -cvs[i].z / WBConstants::Scale;
        }

        spline->SetCVs( tlCVs, numCVs );
        spline->SetNumCVs( numCVs );

        delete[] tlCVs;

        return spline;
    }

    return NULL;
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
