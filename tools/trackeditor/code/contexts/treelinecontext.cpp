//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        TreeLineContext.cpp
//
// Description: Implement TreeLineContext
//
// History:     27/05/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
#include "precompiled/PCH.h"

//========================================
// Project Includes
//========================================
#include "contexts/TreeLineContext.h"
#include "utility/mext.h"
#include "main/constants.h"
#include "main/trackeditor.h"
#include "nodes/treelineshapenode.h"



//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
const char* TreeLineContext::stringId = "TreeLineContext";
MObject TreeLineContext::mCurrentTreeLine;
bool TreeLineContext::mWorking = false;


//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// TreeLineContext::TreeLineContext
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
TreeLineContext::TreeLineContext() :
    mXCurrent( 0 ),
    mYCurrent( 0 )
{
    SetHelpString();

    setTitleString( "Ye Tree Line Tool" ); 
    
} 

//==============================================================================
// TreeLineContext::~TreeLineContext
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
TreeLineContext::~TreeLineContext()
{
}

//=============================================================================
// TreeLineContext::toolOnSetup
//=============================================================================
// Description: Comment
//
// Parameters:  ( MEvent& )
//
// Return:      void 
//
//=============================================================================
void TreeLineContext::toolOnSetup( MEvent& event )
{
    setCursor( MCursor::crossHairCursor );

    mPoints.clear();
    mWorking = false;
    mCurrentTreeLine = MObject::kNullObj;
}

//=============================================================================
// TreeLineContext::toolOffCleanup
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TreeLineContext::toolOffCleanup()
{
    mPoints.clear();
    mCurrentTreeLine = MObject::kNullObj;
}

//=============================================================================
// TreeLineContext::doPress
//=============================================================================
// Description: Comment
//
// Parameters:  ( MEvent& event )
//
// Return:      MStatus 
//
//=============================================================================
MStatus TreeLineContext::doPress( MEvent& event )
{
    event.getPosition( mXCurrent, mYCurrent );
    ProcessState( BUTTONDOWN );
    return MStatus::kSuccess;
}

//=============================================================================
// TreeLineContext::doDrag
//=============================================================================
// Description: Comment
//
// Parameters:  ( MEvent& event )
//
// Return:      MStatus 
//
//=============================================================================
MStatus TreeLineContext::doDrag( MEvent& event )
{
    event.getPosition( mXCurrent, mYCurrent );
    ProcessState( MOUSEDRAG );
    return MStatus::kSuccess;
}

//=============================================================================
// TreeLineContext::doRelease
//=============================================================================
// Description: Comment
//
// Parameters:  ( MEvent& event )
//
// Return:      MStatus 
//
//=============================================================================
MStatus TreeLineContext::doRelease( MEvent& event )
{
    if ( event.mouseButton() == MEvent::kLeftMouse )
    {
        event.getPosition( mXCurrent, mYCurrent );
        ProcessState( BUTTONUP );
    }

    return MStatus::kSuccess;
}

//=============================================================================
// TreeLineContext::doHold
//=============================================================================
// Description: Comment
//
// Parameters:  ( MEvent& event )
//
// Return:      MStatus 
//
//=============================================================================
MStatus TreeLineContext::doHold( MEvent& event )
{
    return MStatus::kSuccess;
}

//=============================================================================
// TreeLineContext::doEnterRegion
//=============================================================================
// Description: Comment
//
// Parameters:  ( MEvent& event )
//
// Return:      MStatus 
//
//=============================================================================
MStatus TreeLineContext::doEnterRegion( MEvent& event )
{
    SetHelpString();

    return MStatus::kSuccess;
}

//=============================================================================
// TreeLineContext::deleteAction
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TreeLineContext::deleteAction()
{
    ProcessState( DELETED );
}

//=============================================================================
// TreeLineContext::completeAction
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TreeLineContext::completeAction()
{
    ProcessState( COMPLETED );
}

//=============================================================================
// TreeLineContext::abortAction
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TreeLineContext::abortAction()
{
    ProcessState( ABORTED );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// TreeLineContext::ProcessState
//=============================================================================
// Description: Comment
//
// Parameters:  ( Stimulus stimulus )
//
// Return:      void 
//
//=============================================================================
void TreeLineContext::ProcessState( Stimulus stimulus )
{
    switch( stimulus )
    {
    case BUTTONDOWN:
        {
        }
        break;

    case BUTTONUP:
        {
            if ( !mWorking )
            {
                //Let's create our working Treeline!
                MObject transform;
                MString name( TETreeLine::TreelineShapeNode::stringId );
                MExt::CreateNode( &mCurrentTreeLine, 
                                  &transform, 
                                  MString( TETreeLine::TreelineShapeNode::stringId ),
                                  &name );
                mWorking = true;

                MFnTransform fnTransform( transform );
                fnTransform.findPlug( MString("translate") ).setLocked( true );
                fnTransform.findPlug( MString("rotate") ).setLocked( true );
                fnTransform.findPlug( MString("scale") ).setLocked( true );

                TrackEditor::AddChild( mCurrentTreeLine );
            }

            //Set the position
            MPoint intersectPoint;
            if ( !MExt::MeshClickIntersect( mXCurrent, mYCurrent, intersectPoint ) )
            {
                //Put it at 0.
                MPoint vp( mXCurrent, mYCurrent, 0 );
                MExt::ViewToWorldAtY( &intersectPoint, vp, 0 );  //This is to y = 0
            }

            intersectPoint = intersectPoint / TEConstants::Scale;

            MStatus status;
            MFnDependencyNode fnDepNode( mCurrentTreeLine );

            MPlug verticesPlug = fnDepNode.findPlug( TETreeLine::TreelineShapeNode::mControlPoints, &status );
            assert( status );
            
            unsigned int elementCount = verticesPlug.numElements();
            MPlug vertex = verticesPlug.elementByLogicalIndex( elementCount, &status );
            assert( status );

            MPlug x = vertex.child( TETreeLine::TreelineShapeNode::mControlValueX, &status );
            assert( status );

            x.setValue( intersectPoint.x * TEConstants::Scale );

            MPlug y = vertex.child( TETreeLine::TreelineShapeNode::mControlValueY, &status );
            assert( status );
            y.setValue( intersectPoint.y * TEConstants::Scale );

            MPlug z = vertex.child( TETreeLine::TreelineShapeNode::mControlValueZ, &status );
            assert( status );
            z.setValue( intersectPoint.z * TEConstants::Scale );
      
            MGlobal::select( mCurrentTreeLine, MGlobal::kReplaceList );
        }
        break;
    case DELETED:
        {
            DeleteLast();
        }
        break;
    case ABORTED:
    case COMPLETED:
        {
            //Start new treeline
            mWorking = false;
            mCurrentTreeLine = MObject::kNullObj;
        }
        break;
    default:
        {
        }
        break;
    }

    SetHelpString();
}

//=============================================================================
// TreeLineContext::AddPoint
//=============================================================================
// Description: Comment
//
// Parameters:  ( MPoint& point )
//
// Return:      void 
//
//=============================================================================
void TreeLineContext::AddPoint( MPoint& point )
{
    mPoints.append( point );
}

//=============================================================================
// TreeLineContext::DeleteLast
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TreeLineContext::DeleteLast()
{
    unsigned int size = mPoints.length();

    if ( size )
    {
        MStatus status;

        mPoints.remove( size - 1 );
    }
}

//=============================================================================
// TreeLineContext::SetHelpString
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TreeLineContext::SetHelpString()
{
    mHelp = "Click to place vertices in the line.";

    setHelpString( mHelp );
}
