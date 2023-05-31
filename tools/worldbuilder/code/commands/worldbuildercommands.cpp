#include "worldbuildercommands.h"
#include "main/worldbuilder.h"
#include "utility/mui.h"
#include "utility/mext.h"
#include "resources/resource.h"
#include "main/constants.h"
#include "nodes/wbspline.h"
#include "nodes/splinelocatornode.h"
#include "nodes/triggervolumenode.h"
#include "nodes/railcamlocatornode.h"

const char* WBChangeDisplayCommand::stringId    = "WB_ChangeDisplay";
const char* WBSetLocatorTypeCmd::stringId       = "WB_SetLocatorType";
const char* WBSetPrefixCmd::stringId            = "WB_SetPrefix";
const char* WBSnapLocatorCmd::stringId          = "WB_SnapLocator";
const char* WBSplineCompleteCmd::stringId       = "WB_SplineComplete";
const char* WBCoinSplineCompleteCmd::stringId       = "WB_CoinSplineComplete";
const char* WBSelectObjectCmd::stringId         = "WB_SelectObject";

//******************************************************************************
//
// Callbacks
//
//******************************************************************************
 
BOOL CALLBACK SetPrefixLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam )
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        {
            SetDlgItemText( hWnd, IDC_EDIT1, WorldBuilder::GetPrefix() );
            return true;
        }
        break;
    case WM_COMMAND:
        {
            if ( LOWORD(wParam) == IDCANCEL || 
                 LOWORD(wParam) == IDC_BUTTON1 ||
                 LOWORD(wParam) == IDOK )
            {
                //Get the entry in the text field.
                char name[WorldBuilder::MAX_PREFIX_LENGTH];
                GetDlgItemText( hWnd, IDC_EDIT1, name, WorldBuilder::MAX_PREFIX_LENGTH );
              
                WorldBuilder::SetPrefix( name );
                EndDialog( hWnd, 0 );  //this is how you close the window.                
                return true;
            }

            return false;
        }
        break;
    default:
        {
            return false;
        }
        break;
    }
}

BOOL CALLBACK SplineNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam )
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        {
            SetDlgItemText( hWnd, IDC_EDIT2, WorldBuilder::GetPrefix() );
            return true;
        }
        break;
    case WM_COMMAND:
        {
            if ( LOWORD(wParam) == IDC_BUTTON1 || LOWORD(wParam) == IDOK )
            {
                //Get the entry in the text field.
                char name[WBSpline::MAX_NAME_LEN];
                GetDlgItemText( hWnd, IDC_EDIT1, name, WBSpline::MAX_NAME_LEN );

                if ( strcmp(name, "") == 0 )
                {
                    MExt::DisplayWarning("You must input a new name for the Spline!");
                    return false;
                }

                MString newName( WorldBuilder::GetPrefix() );
                newName += MString( name );
                
                WBSpline::SetName( newName.asChar() );
                EndDialog( hWnd, 0 );  //this is how you close the window.                
                return true;
            }
            else if( LOWORD(wParam) == IDCANCEL )
            {
                WBSpline::SetName( "" );
                EndDialog( hWnd, 0 );  //this is how you close the window.                
                return true;
            }

            return false;
        }
        break;
    default:
        {
            return false;
        }
        break;
    }
}

//******************************************************************************
//
// Commands
//
//******************************************************************************

//=============================================================================
// WBChangeDisplayCommand::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
void* WBChangeDisplayCommand::creator()
{
    return new WBChangeDisplayCommand();
}

//=============================================================================
// WBChangeDisplayCommand::doIt
//=============================================================================
// Description: Comment
//
// Parameters:  ( const MArgList& args )
//
// Return:      MStatus 
//
//=============================================================================
MStatus WBChangeDisplayCommand::doIt( const MArgList& args )
{
    assert( args.length() == 2 );
    int arg;
    args.get(0, arg);

    int on;
    args.get(1, on);

    WorldBuilder::SetDisplayLevel( arg, on == 1 );

    //Careful, don't call this too often!
    M3dView::active3dView().refresh( true, true );
    return MStatus::kSuccess;
}

//=============================================================================
// WBSetLocatorTypeCmd::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
void* WBSetLocatorTypeCmd::creator()
{
    return new WBSetLocatorTypeCmd();
}

//=============================================================================
// WBSetLocatorTypeCmd::doIt
//=============================================================================
// Description: Comment
//
// Parameters:  ( const MArgList& args )
//
// Return:      MStatus 
//
//=============================================================================
MStatus WBSetLocatorTypeCmd::doIt( const MArgList& args )
{
    assert( args.length() == 1 );

    MString string;
    args.get( 0, string );
    
    LocatorType::Type type;

    int i;
    for ( i = 0; i < LocatorType::NUM_TYPES; ++i )
    {
        if ( string == MString( LocatorType::Name[i] ) )
        {
            type = (LocatorType::Type)i;
            WorldBuilder::SetLocatorType( type );

            return MStatus::kSuccess;
        }
    }

    assert( false );

    return MStatus::kFailure;
}

//=============================================================================
// WBSetPrefixCmd::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
void* WBSetPrefixCmd::creator()
{
    return new WBSetPrefixCmd();
}

//=============================================================================
// WBSetPrefixCmd::doIt
//=============================================================================
// Description: Comment
//
// Parameters:  ( const MArgList& args )
//
// Return:      MStatus 
//
//=============================================================================
MStatus WBSetPrefixCmd::doIt( const MArgList& args )
{
    assert( args.length() == 0 );

    MUI::PopupDialogue( IDD_DIALOG2, &SetPrefixLocatorNameCallBack );
        
    return MStatus::kSuccess;
}

//=============================================================================
// WBSnapLocatorCmd::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
void* WBSnapLocatorCmd::creator()
{
    return new WBSnapLocatorCmd();
}

//=============================================================================
// WBSnapLocatorCmd::doIt
//=============================================================================
// Description: Comment
//
// Parameters:  ( const MArgList& args )
//
// Return:      MStatus 
//
//=============================================================================
MStatus WBSnapLocatorCmd::doIt( const MArgList& args )
{
    assert( args.length() == 1 );

    double OFFSET = 0;

    args.get( 0, OFFSET );

    OFFSET *= WBConstants::Scale;

    MSelectionList list;
    MGlobal::getActiveSelectionList( list );
    
    MItSelectionList i( list );

    MFnDagNode fnDagNode;
    MDagPath dagPath;
    MItDag itDag;
    MObject obj;
    MDagPath objDagPath;
    MObject childObj;

    //For all selected objects.
    for ( ; !i.isDone(); i.next() )
    {
        i.getDagPath( dagPath );

        itDag.reset( dagPath, MItDag::kBreadthFirst, MFn::kTransform );

        for ( ; !itDag.isDone() && itDag.depth() < 2; itDag.next() )
        {
            obj = itDag.item();
            fnDagNode.setObject( obj );

            const char* objName = fnDagNode.name().asChar();
            const char* objTypeName = fnDagNode.typeName().asChar();
    
            int childCount = fnDagNode.childCount();

            int whichChild;

            for ( whichChild = 0; whichChild < childCount; ++whichChild )
            {
                childObj = fnDagNode.child( whichChild );
                fnDagNode.setObject( childObj );

                const char* childObjName = fnDagNode.name().asChar();
                const char* childObjTypeName = fnDagNode.typeName().asChar();

                //Find a mesh below me and move my pivot to the intersection.
                itDag.getPath( objDagPath );
                MFnTransform fnTrans( objDagPath );


                MVector pos = fnTrans.translation( MSpace::kWorld );
                MPoint rotate = fnTrans.rotatePivot( MSpace::kWorld );
                MVector rayDir( 0, -1.0, 0 );

                MItDag meshIt( MItDag::kDepthFirst, MFn::kMesh );
                MDagPath meshDagPath;
                MPointArray intersectPoints;

                bool found = false;

                for ( ; !meshIt.isDone(); meshIt.next() )
                {
                    meshIt.getPath( meshDagPath );
                    MFnMesh mesh( meshDagPath );

                    mesh.intersect( rotate, rayDir, intersectPoints, 0.001f, MSpace::kWorld );

                    if ( intersectPoints.length() > 0 )
                    {
                        MVector diff( intersectPoints[ 0 ] - rotate );
                        diff.y += OFFSET;

                        fnTrans.translateBy( diff, MSpace::kWorld );

                        found = true;

                        break;
                    }
                }

                if ( !found )
                {
                    //Look up
                    MPoint rotate = fnTrans.rotatePivot( MSpace::kWorld );
                    MVector rayDir( 0, 1.0, 0 );

                    MItDag meshIt( MItDag::kDepthFirst, MFn::kMesh );
                    MDagPath meshDagPath;
                    MPointArray intersectPoints;

                    for ( ; !meshIt.isDone(); meshIt.next() )
                    {
                        meshIt.getPath( meshDagPath );
                        MFnMesh mesh( meshDagPath );

                        mesh.intersect( rotate, rayDir, intersectPoints, 0.001f, MSpace::kWorld );

                        if ( intersectPoints.length() > 0 )
                        {
                            MVector diff( intersectPoints[ 0 ] - rotate );
                            diff.y -= OFFSET;

                            fnTrans.translateBy( diff, MSpace::kWorld );

                            found = true;

                            break;
                        }
                    }
                }

                if ( !found )
                {
                    MString errorMsg( "The object: " );
                    errorMsg += fnTrans.name();
                    errorMsg += MString( " has no mesh below it.\nNo snapping done on it." );

                    MUI::InfoDialog( errorMsg.asChar() );
                }
            }
        }
    }

    return MStatus::kSuccess;
}

//=============================================================================
// WBSplineCompleteCmd::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
void* WBSplineCompleteCmd::creator()
{
    return new WBSplineCompleteCmd();
}

//=============================================================================
// WBSplineCompleteCmd::doIt
//=============================================================================
// Description: Comment
//
// Parameters:  ( const MArgList& args )
//
// Return:      WBSplineCompleteCmd
//
//=============================================================================
MStatus WBSplineCompleteCmd::doIt( const MArgList& args )
{
    MSelectionList selectionList;

    MGlobal::getActiveSelectionList( selectionList );
    
    if ( selectionList.length() == 0 )
    {
        return MStatus::kSuccess;
    }

    MObject spline;
    selectionList.getDependNode( 0, spline );

    MStatus status;

    MFnTransform fnTransform( spline, &status );
    assert( status );

    if ( status )
    {
        MDagPath dagPath;
        selectionList.getDagPath(0, dagPath);
        dagPath.extendToShape();

        MFnNurbsCurve fnNurbs( dagPath, &status );
        assert( status );

        int numCvs = fnNurbs.numCVs();
        assert( numCvs );

        MPoint cv;
        MBoundingBox bBox;
        int i;
        for ( i = 0; i < numCvs; ++i )
        {
            fnNurbs.getCV( i, cv, MSpace::kWorld );
            bBox.expand( cv );
        }

        MPoint halfway = bBox.center();

        double radius = bBox.height() > bBox.width() ? bBox.height() : bBox.width();
        radius = radius > bBox.depth() ? radius : bBox.depth();
        radius = radius / WBConstants::Scale / 2.0;

        MFnDependencyNode fnDepNodeSpline( fnTransform.child(0) );
    
        MUI::PopupDialogue( IDD_DIALOG1, SplineNameCallBack );

        //Get the new name of the spline...
        const char* name = WBSpline::GetName();
        MString newName( name );

        MObject locator, triggerVolume, transform;

        //Create the spline locator
        MExt::CreateNode( locator, 
                          transform, 
                          MString( SplineLocatorNode::stringId ), 
                          &newName );

        MExt::SetWorldPosition( halfway, locator );
        
        WorldBuilder::AddChild( locator );

        //Name the spline
        MString splineName = newName + MString("Spline");
        fnTransform.setName( splineName );
        WorldBuilder::AddChild( fnDepNodeSpline.object() );

        //Adding an extra attribute to the spline!!
        MFnMessageAttribute msgAttr;
        MObject locatorAttr;
        locatorAttr = msgAttr.create( "locator", "l", &status );
        assert( status );

        msgAttr.setReadable( true );
        msgAttr.setWritable( false );
        
        fnDepNodeSpline.addAttribute( locatorAttr );

        //Connect the Spline to the Locator.
        MExt::Connect( fnDepNodeSpline.object(), "locator", locator, SplineLocatorNode::SPLINE_NAME_LONG );

        //Create the Trigger Volume
        MString triggerName = newName + MString( "Trigger" );

        MExt::CreateNode( triggerVolume, 
                          transform, 
                          MString( TriggerVolumeNode::stringId ), 
                          &triggerName );
 
        MExt::SetWorldPosition( halfway, triggerVolume );

        MFnTransform fnTransform( transform );
        const double scale[3] = { radius, radius, radius };
        
        //Scale this bad-boy!
        fnTransform.setScale( scale );

        //Connect the Trigger Volume to the Locator.
        MExt::Connect( triggerVolume, TriggerVolumeNode::LOCATOR_NAME_LONG, locator, SplineLocatorNode::TRIGGERS_NAME_LONG );

        WorldBuilder::AddChild( triggerVolume );

        //Create the railcam
        MObject railLocator;
        MString railName = newName + MString( "RailCam" );
        MExt::CreateNode( railLocator, 
                          transform, 
                          MString( RailCamLocatorNode::stringId ), 
                          &railName );

        fnNurbs.getCV( 0, cv, MSpace::kWorld );
        MExt::SetWorldPosition( cv, railLocator );
        MExt::Connect( railLocator, "message", locator, SplineLocatorNode::CAMERA_NAME_LONG );
        MExt::Connect( fnDepNodeSpline.object(), "message", railLocator, RailCamLocatorNode::RAIL_NAME_LONG );
        
        WorldBuilder::AddChild( railLocator );
    }

    return MStatus::kSuccess; 
}

//=============================================================================
// WBCoinSplineCompleteCmd::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
void* WBCoinSplineCompleteCmd::creator()
{
    return new WBCoinSplineCompleteCmd();
}

//=============================================================================
// WBCoinSplineCompleteCmd::doIt
//=============================================================================
// Description: Comment
//
// Parameters:  ( const MArgList& args )
//
// Return:      WBCoinSplineCompleteCmd
//
//=============================================================================
MStatus WBCoinSplineCompleteCmd::doIt( const MArgList& args )
{
    MSelectionList selectionList;

    MGlobal::getActiveSelectionList( selectionList );

    if ( selectionList.length() == 0 )
    {
        return MStatus::kSuccess;
    }


    MObject spline;
    selectionList.getDependNode( 0, spline );

    MStatus status;

    MFnTransform fnTransform( spline, &status );
    assert( status );

    if ( status )
    {
        MDagPath dagPath;
        selectionList.getDagPath(0, dagPath);
        dagPath.extendToShape();

        MFnNurbsCurve fnNurbs( dagPath, &status );
        assert( status );

        int numCvs = fnNurbs.numCVs();
        assert( numCvs );

        MFnDependencyNode fnDepNodeSpline( fnTransform.child(0) );
    
        MUI::PopupDialogue( IDD_DIALOG1, SplineNameCallBack );

        //Get the new name of the spline...
        const char* name = WBSpline::GetName();
        MString newName( name );

        MObject locator, triggerVolume, transform;

        //Create the spline locator
        MExt::CreateNode( locator, 
                          transform, 
                          MString( SplineLocatorNode::stringId ), 
                          &newName );

        MExt::SetWorldPosition( MPoint(0,0,0), locator );

        //Set this locator to say that it is a coin locator
        MFnDependencyNode fnNode( locator );
        fnNode.findPlug( SplineLocatorNode::sIsCoin ).setValue( true );
        
        WorldBuilder::AddChild( locator );

        //Name the spline
        MString splineName = newName + MString("Spline");
        fnTransform.setName( splineName );
        WorldBuilder::AddChild( fnDepNodeSpline.object() );

        //Adding an extra attribute to the spline!!
        MFnMessageAttribute msgAttr;
        MObject locatorAttr;
        locatorAttr = msgAttr.create( "locator", "l", &status );
        assert( status );

        msgAttr.setReadable( true );
        msgAttr.setWritable( false );
        
        fnDepNodeSpline.addAttribute( locatorAttr );

        //Add a num coins attribute to the spline
        MFnNumericAttribute numAttr;
        MObject numCoinAttr;
        numCoinAttr = numAttr.create( "numCoins", "nc", MFnNumericData::kInt, 10, &status );
        assert( status );

        numAttr.setReadable( true );
        numAttr.setWritable( true );

        fnDepNodeSpline.addAttribute( numCoinAttr );

        //Connect the Spline to the Locator.
        MExt::Connect( fnDepNodeSpline.object(), "locator", locator, SplineLocatorNode::SPLINE_NAME_LONG );
    }

    return MStatus::kSuccess; 
}

//=============================================================================
// WBSelectObjectCmd::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
void* WBSelectObjectCmd::creator()
{
    return new WBSelectObjectCmd();
}

MStatus WBSelectObjectCmd::doIt( const MArgList& args )
{
    assert( args.length() == 1 );
 
    MString objName;
    
    args.get( 0, objName );

    WorldBuilder::SetSelectedLocator( objName.asChar() );

    return MStatus::kSuccess;
}