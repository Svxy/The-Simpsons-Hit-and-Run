//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        SnapSelectedTreelines.cpp
//
// Description: Implement SnapSelectedTreelines
//
// History:     27/05/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================


//========================================
// Project Includes
//========================================
#include "commands/TreeLineCommand.h"
#include "main/trackeditor.h"
#include "nodes/treelineshapenode.h"

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
const char* SnapSelectedTreelines::stringId = "TE_SnapSelectedTreelines";
const char* ConvertTreelineToGeometry::stringId = "TE_ConvertTreelineToGeometry";
const char* SetDeleteTreeline::stringId = "TE_SetDeleteTreeline";

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// SnapSelectedTreelines::SnapSelectedTreelines
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SnapSelectedTreelines::SnapSelectedTreelines()
{
}

//==============================================================================
// SnapSelectedTreelines::~SnapSelectedTreelines
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SnapSelectedTreelines::~SnapSelectedTreelines()
{
}

//=============================================================================
// SnapSelectedTreelines::doIt
//=============================================================================
// Description: Comment
//
// Parameters:  ( const MArgList& args )
//
// Return:      MStatus 
//
//=============================================================================
MStatus SnapSelectedTreelines::doIt( const MArgList& args )
{
    //For each treeline in the selection list, call

    MSelectionList selectionList;
    MGlobal::getActiveSelectionList( selectionList );

    MItSelectionList itSel( selectionList );

    while ( !itSel.isDone() )
    {
        MObject obj;
        itSel.getDependNode( obj );

        Recurse( obj );

        itSel.next();
    }

    return MStatus::kSuccess;
}

//=============================================================================
// SnapSelectedTreelines::Recurse
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& obj )
//
// Return:      void 
//
//=============================================================================
void SnapSelectedTreelines::Recurse( MObject& obj )
{
    MStatus status;

    MFnTransform FnTransform( obj, &status );

    if ( status )
    {
        //This is a transform...

        MFnDagNode fnDagNode( obj );
        
        unsigned int i;
        for ( i = 0; i < fnDagNode.childCount(); ++i )
        {
            Recurse( fnDagNode.child( i ) );
        }
    }
    else
    {
        MFnDependencyNode fnDepNode( obj );

        if ( fnDepNode.typeId() == TETreeLine::TreelineShapeNode::id )
        {
            TETreeLine::TreelineShapeNode::SnapTreeline( obj );
        }

    }
}

//*****************************************************************************
//
// ConvertTreelineToGeometry
//
//*****************************************************************************

//=============================================================================
// ConvertTreelineToGeometry::ConvertTreelineToGeometry
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      ConvertTreelineToGeometry
//
//=============================================================================
ConvertTreelineToGeometry::ConvertTreelineToGeometry()
{
}

//=============================================================================
// ConvertTreelineToGeometry::~ConvertTreelineToGeometry
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      ConvertTreelineToGeometry
//
//=============================================================================
ConvertTreelineToGeometry::~ConvertTreelineToGeometry()
{
}

//=============================================================================
// ConvertTreelineToGeometry::doIt
//=============================================================================
// Description: Comment
//
// Parameters:  ( const MArgList& args )
//
// Return:      MStatus 
//
//=============================================================================
MStatus ConvertTreelineToGeometry::doIt( const MArgList& args )
{
    //For each treeline in the world, call

    MSelectionList selectionList;
    selectionList.clear();

    MItDag itDag( MItDag::kDepthFirst );

    while ( !itDag.isDone() )
    {     
        MDagPath dagPath;
        itDag.getPath( dagPath );

        selectionList.add( dagPath );

        itDag.next();
    }

    if ( selectionList.length() > 0 )
    {

        MItSelectionList itSel( selectionList );

        while ( !itSel.isDone() )
        {
            MObject obj;
            itSel.getDependNode( obj );

            Recurse( obj );

            itSel.next();
        }
    }

    return MStatus::kSuccess;
}

//=============================================================================
// ConvertTreelineToGeometry::Recurse
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& obj )
//
// Return:      void 
//
//=============================================================================
void ConvertTreelineToGeometry::Recurse( MObject& obj )
{
    MStatus status;

/*
    MFnTransform FnTransform( obj, &status );

    if ( status )
    {
        //This is a transform...

        MFnDagNode fnDagNode( obj );
        
        unsigned int i;
        for ( i = 0; i < fnDagNode.childCount(); ++i )
        {
            Recurse( fnDagNode.child( i ) );
        }
    }
    else
    {
*/
        MFnDependencyNode fnDepNode( obj );

        if ( fnDepNode.typeId() == TETreeLine::TreelineShapeNode::id )
        {
            TETreeLine::TreelineShapeNode::ConvertToGeometry( obj );
        }

//    }
}

//*****************************************************************************
//
// SetDeleteTreeline
//
//*****************************************************************************

//=============================================================================
// SetDeleteTreeline::SetDeleteTreeline
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      SetDeleteTreeline
//
//=============================================================================
SetDeleteTreeline::SetDeleteTreeline()
{
}

//=============================================================================
// SetDeleteTreeline::~SetDeleteTreeline
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      SetDeleteTreeline
//
//=============================================================================
SetDeleteTreeline::~SetDeleteTreeline()
{
}

//=============================================================================
// SetDeleteTreeline::doIt
//=============================================================================
// Description: Comment
//
// Parameters:  ( const MArgList& args )
//
// Return:      MStatus 
//
//=============================================================================
MStatus SetDeleteTreeline::doIt( const MArgList& args )
{
    assert( args.length() == 1 );

    bool del;
    args.get( 0, del );

    TrackEditor::SetDeleteTreelines( del );

    return MStatus::kSuccess;
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
