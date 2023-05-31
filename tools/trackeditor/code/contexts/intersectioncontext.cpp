//----------------------------------------
// System Includes
//----------------------------------------
#include <math.h>

//----------------------------------------
// Project Includes
//----------------------------------------

#include "intersectioncontext.h"
#include "nodes/intersection.h"
#include "utility/mext.h"
#include "main/trackeditor.h"

//----------------------------------------
// Constants, Typedefs and Statics
//----------------------------------------

const short OFFSET = 10;
const double SCALE_FACTOR = 0.002;

const char* IntersectionContext::stringId = "IntersectionContext";

//==============================================================================
// IntersectionContextCmd::IntersectionContextCmd
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      IntersectionContextCmd
//
//==============================================================================
IntersectionContextCmd::IntersectionContextCmd()
{
}

//==============================================================================
// IntersectionContextCmd::~IntersectionContextCmd
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      IntersectionContextCmd
//
//==============================================================================
IntersectionContextCmd::~IntersectionContextCmd()
{
}

//-----------------------------------------------------------------------------
// c r e a t o r 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void* IntersectionContextCmd::creator()
{
    return new IntersectionContextCmd();
}

//-----------------------------------------------------------------------------
// m a k e O b j 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
MPxContext* IntersectionContextCmd::makeObj()
{
    return new IntersectionContext();
}

//==============================================================================
// IntersectionContext::IntersectionContext
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      IntersectionContext
//
//==============================================================================
IntersectionContext::IntersectionContext() :
    mXCurrent( 0 ),
    mYCurrent( 0 ),
    mIntersection( MObject::kNullObj ),
    mIntersectionTransform( MObject::kNullObj )
{
    SetHelpString();

    setTitleString( "Intersection Overlay Tool" );    
} 

//==============================================================================
// IntersectionContext::~IntersectionContext
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      IntersectionContext
//
//==============================================================================
IntersectionContext::~IntersectionContext()
{
}

//==============================================================================
// IntersectionContext::abortAction
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//==============================================================================
void IntersectionContext::abortAction()
{
    ProcessState( ABORTED );
}

//-----------------------------------------------------------------------------
// c o m p l e t e A c t i o n 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void IntersectionContext::completeAction()
{
    ProcessState( COMPLETED );
}

//-----------------------------------------------------------------------------
// d e l e t e A c t i o n 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void IntersectionContext::deleteAction()
{
    ProcessState( DELETED );
}

//-----------------------------------------------------------------------------
// d o D r a g 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
MStatus IntersectionContext::doDrag( MEvent& event )
{

    event.getPosition( mXDrag, mYDrag );
    ProcessState( MOUSEDRAG );
    return MS::kSuccess;
}

//-----------------------------------------------------------------------------
// d o E n t e r R e g i o n 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
MStatus IntersectionContext::doEnterRegion( MEvent& event )
{
    SetHelpString();

    return MS::kSuccess;
}

//-----------------------------------------------------------------------------
// d o H o l d 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
MStatus IntersectionContext::doHold( MEvent& event )
{
    MStatus status = MS::kSuccess;
    return status;
}

//-----------------------------------------------------------------------------
// d o P r e s s 
//
// Synopsis:    
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
MStatus IntersectionContext::doPress( MEvent& event )
{
    event.getPosition( mXCurrent, mYCurrent );
    ProcessState( BUTTONDOWN );
    return MS::kSuccess;
}

//-----------------------------------------------------------------------------
// d o R e l e a s e 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
MStatus IntersectionContext::doRelease( MEvent& event )
{
    if ( event.mouseButton() == MEvent::kLeftMouse )
    {
        event.getPosition( mXCurrent, mYCurrent );
        ProcessState( BUTTONUP );
    }

    return MS::kSuccess;
}

//-----------------------------------------------------------------------------
// t o o l O f f C l e a n u p
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void IntersectionContext::toolOffCleanup()
{
    if ( mIntersectionTransform != MObject::kNullObj )
    {
        mIntersection = MObject::kNullObj;
        mIntersectionTransform = MObject::kNullObj;
    }
} 

//-----------------------------------------------------------------------------
// t o o l O n S e t u p 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void IntersectionContext::toolOnSetup( MEvent& event )
{
    setCursor( MCursor::crossHairCursor );
}

//-----------------------------------------------------------------------------
//
// P R I V A T E   M E M B E R S
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// p r o c e s s S t a t e 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void IntersectionContext::ProcessState( Stimulus stimulus )
{
    switch( stimulus )
    {
    case BUTTONDOWN:
        {
            InitIntersection();
        }
        break;
    case MOUSEDRAG:
        {
            //Scale the intersection according to drag dist.
            short diffX = mXCurrent - mXDrag;
            short diffY = mYCurrent - mYDrag;

            double dist = 25.0 + sqrt( ( diffX*diffX + diffY*diffY ) ) * SCALE_FACTOR;

            double scaleFactor[3] = { dist, dist, dist };

            MFnTransform fnTransform( mIntersectionTransform );

            fnTransform.setScale( scaleFactor );
        }
        break;
    case BUTTONUP:
    case COMPLETED:
        {
        }
        break;
    case ABORTED:
        {
            if ( mIntersectionTransform != MObject::kNullObj )
            {
                mIntersection = MObject::kNullObj;
                mIntersectionTransform = MObject::kNullObj;
            }
        }
        break;
    case DELETED:
        {
            if ( mIntersectionTransform != MObject::kNullObj )
            {
                MGlobal::deleteNode( mIntersection );
                MGlobal::deleteNode( mIntersectionTransform );
                mIntersection = MObject::kNullObj;
                mIntersectionTransform = MObject::kNullObj;
            }
        }
        break;
    default:
        {
        }
        break;
    }

    SetHelpString();
}

//==============================================================================
// IntersectionContext::SetHelpString
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//==============================================================================
void IntersectionContext::SetHelpString()
{
    mHelp = "Click and drag to create intersection.";

    setHelpString( mHelp );
}

//==============================================================================
// IntersectionContext::InitIntersection
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//==============================================================================
void IntersectionContext::InitIntersection()
{
    //Get the mesh below the clicked point and find it's y height.
    short xStart, xEnd, yStart, yEnd;

    xStart = 0;
    xEnd = M3dView::active3dView().portWidth();
    yStart = M3dView::active3dView().portHeight();
    yEnd = 0;

    MGlobal::selectFromScreen( xStart, 
                               yStart, 
                               xEnd,
                               yEnd,
                               MGlobal::kReplaceList );

    MSelectionList selectionList;

    MGlobal::getActiveSelectionList( selectionList );

    if ( selectionList.length() > 0 )
    {
        //Go through each selected object and see if the ray intersects it.
        MItSelectionList selectIt( selectionList, MFn::kMesh );

        MPoint nearClick, farClick;
        M3dView activeView = M3dView::active3dView();
        activeView.viewToWorld( mXCurrent, mYCurrent, nearClick, farClick );
        MVector rayDir( MVector( farClick ) - MVector( nearClick ) );
        MPointArray intersectPoints;
        MDagPath objDag;

        while ( !selectIt.isDone() )
        {
            selectIt.getDagPath( objDag );

            MFnMesh mesh( objDag );

            mesh.intersect( nearClick, rayDir, intersectPoints, 0.001f, MSpace::kWorld );

            if ( intersectPoints.length() > 0 )
            {
                MObject transform;
                MExt::CreateNode( mIntersection, 
                                  mIntersectionTransform, 
                                  MString( IntersectionLocatorNode::stringId ) );

                assert( !mIntersection.isNull() );

                MExt::SetWorldPosition( intersectPoints[0], mIntersection );

                MFnTransform fnTransform( mIntersectionTransform );

                const double scale[3] = { 25.0, 25.0, 25.0 };
                fnTransform.setScale( scale );


                TrackEditor::AddChild( mIntersection );

                break;
            }

            selectIt.next();
        }
    }

    MGlobal::clearSelectionList();
}
