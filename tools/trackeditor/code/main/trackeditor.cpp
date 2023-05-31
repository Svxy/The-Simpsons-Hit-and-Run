#include "precompiled/PCH.h"

#include "trackeditor.h"
#include "utility/mext.h"

#include "nodes/tiledisplay.h"
#include "nodes/fenceline.h"
#include "nodes/intersection.h"
#include "nodes/walllocator.h"
#include "nodes/road.h"
#include "nodes/treelineshapenode.h"
#include "nodes/pedpath.h"

const char*  TrackEditor::Name = "TrackEditorNode";
TrackEditor::EditMode TrackEditor::sEditMode = TrackEditor::OFF;
unsigned int TrackEditor::sNodeAddedbackID = 0;
unsigned int TrackEditor::sWindowClosedCallbackID = 0;
bool TrackEditor::sDeleteTreelines = true;


//==============================================================================    
// TrackEditor::TrackEditor
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      TrackEditor
//
//==============================================================================
TrackEditor::TrackEditor() 
{
//    sNodeAddedbackID = MDGMessage::addNodeAddedCallback ( NodeAddedCB,
//                                                          MString( "surfaceShape" ) );
};

//==============================================================================
// TrackEditor::~TrackEditor
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      TrackEditor
//
//==============================================================================
TrackEditor::~TrackEditor() 
{
    if ( sNodeAddedbackID )
    {
        MDGMessage::removeCallback( sNodeAddedbackID );
    }

    if ( sWindowClosedCallbackID )
    {
        MUiMessage::removeCallback( sWindowClosedCallbackID );
    }

    RemoveTileDisplayNode();
};

//==============================================================================
// TrackEditor::Exists
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//==============================================================================
bool TrackEditor::Exists()
{
    MDagPath pathToTrackEditor;
    return MExt::FindDagNodeByName( &pathToTrackEditor, MString( TrackEditor::Name ) );
}

//==============================================================================
// TrackEditor::AddChild
//==============================================================================
// Description: Comment
//
// Parameters:  ( MObject& obj )
//
// Return:      MStatus 
//
//==============================================================================
MStatus TrackEditor::AddChild( MObject& obj )
{
    //Make sure this exists.
    CreateTrackEditorNode();

    MDagPath pathToTrackEditor;

    bool good = false;

    if ( MExt::FindDagNodeByName( &pathToTrackEditor, MString( TrackEditor::Name ) ) )
    {
        good = true;
    }
    else
    {
        MGlobal::sourceFile( MString( "te_setup.mel" ) );

        if ( MExt::FindDagNodeByName( &pathToTrackEditor, MString( TrackEditor::Name ) ) )
        {
            good = true;
        }
    }

    if ( good )
    {
        MFnDagNode fnDagNodeTE;

        //Which type?
        MFnDagNode fnDagNodeObj( obj );

        if ( fnDagNodeObj.typeId() == FenceLineNode::id )
        {
            //This is a fenceline, parent to the "Fences" node.
            MDagPath dagPath;
            if ( MExt::FindDagNodeByName( &dagPath, MString("Fences"), pathToTrackEditor.node() ) )
            {
                fnDagNodeTE.setObject( dagPath.node() );
            }
            else
            {
                MExt::DisplayError( "Someone has deleted Terrain Edit nodes!!" );
            }
        }
        if ( fnDagNodeObj.typeId() == PedPathNode::id )
        {
            //This is a ped path, parent to the "PedPaths" node.
            MDagPath dagPath;
            if ( MExt::FindDagNodeByName( &dagPath, MString("PedPaths"), pathToTrackEditor.node() ) )
            {
                fnDagNodeTE.setObject( dagPath.node() );
            }
            else
            {
                MExt::DisplayError( "Someone has deleted Terrain Edit nodes!!" );
            }
        }
        else if ( fnDagNodeObj.typeId() == IntersectionLocatorNode::id )
        {
            //This is a fenceline, parent to the "Intersections" node.
            MDagPath dagPath;
            if ( MExt::FindDagNodeByName( &dagPath, MString("Intersections"), pathToTrackEditor.node() ) )
            {
                fnDagNodeTE.setObject( dagPath.node() );
            }
            else
            {
                MExt::DisplayError( "Someone has deleted Terrain Edit nodes!!" );
            }
        }
        else if ( fnDagNodeObj.typeId() == RoadNode::id )
        {
            //This is a fenceline, parent to the "Roads" node.
            MDagPath dagPath;
            if ( MExt::FindDagNodeByName( &dagPath, MString("Roads"), pathToTrackEditor.node() ) )
            {
                fnDagNodeTE.setObject( dagPath.node() );
            }
            else
            {
                MExt::DisplayError( "Someone has deleted Terrain Edit nodes!!" );
            }
        }
        else if ( fnDagNodeObj.typeId() == TETreeLine::TreelineShapeNode::id )
        {
            //This is a tree line, add to the "Treelines" node
            MDagPath dagPath;
            if ( MExt::FindDagNodeByName( &dagPath, MString("Treelines"), pathToTrackEditor.node() ) )
            {
                fnDagNodeTE.setObject( dagPath.node() );
            }
            else
            {
                MExt::DisplayError( "Someone has deleted Terrain Edit nodes!!" );
            }
        }
        else
        {
            fnDagNodeTE.setObject( pathToTrackEditor.node() );
        }

        MObject objT = fnDagNodeObj.parent( 0 );

        return fnDagNodeTE.addChild( objT );
    }

    return MS::kFailure;
}

//Edit mode stuff.
//==============================================================================
// TrackEditor::GetEditMode
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      TrackEditor
//
//==============================================================================
TrackEditor::EditMode TrackEditor::GetEditMode()
{
    //Test the track editor radio buttons for their state.

    return sEditMode;
};

//==============================================================================
// TrackEditor::SetEditMode
//==============================================================================
// Description: Comment
//
// Parameters:  ( TrackEditor::EditMode mode )
//
// Return:      void 
//
//==============================================================================
void TrackEditor::SetEditMode( TrackEditor::EditMode mode )
{
    //Setup whatever needs setting up for the given mode.
    switch( mode )
    {
    case OFF:
        {
            RemoveTileDisplayNode();

            MGlobal::executeCommand( MString("teCloseEditorWindow()") );
        }
        break;
    case EDIT:
        {
            //Make sure this exists;
            CreateTileDisplayNode();

            MGlobal::executeCommand( MString("teOpenEditorWindow()") );

            //We should register a callback for when the window is closed.
            sWindowClosedCallbackID = MUiMessage::addUiDeletedCallback( MString( "TE_TileEditor" ), WindowClosedCB );                                                               
        }
        break;
    case DISPLAY:
        {
            //Make sure this exists;
            CreateTileDisplayNode();

            MGlobal::executeCommand( MString("teCloseEditorWindow()") );
        }
        break;
    default:
        {
            break;
        }
    }

    sEditMode = mode;
}

//==============================================================================
// TrackEditor::NodeAddedCB
//==============================================================================
// Description: Comment
//
// Parameters:  ( MObject& node, void* data )
//
// Return:      void 
//
//==============================================================================
void TrackEditor::NodeAddedCB( MObject& node, void* data )
{
//    assert( false );
}

//==============================================================================
// TrackEditor::WindowClosedCB
//==============================================================================
// Description: Comment
//
// Parameters:  ( void* data )
//
// Return:      void 
//
//==============================================================================
void TrackEditor::WindowClosedCB( void* data )
{
    SetEditMode( OFF );

    MUiMessage::removeCallback( sWindowClosedCallbackID );
}

//==============================================================================
// TrackEditor::CreateTrackEditorNode
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//==============================================================================
void TrackEditor::CreateTrackEditorNode()
{
    MGlobal::executeCommand( "te_Create_TrackEditorNode()" );
}

//==============================================================================
// TrackEditor::CreateTileDisplayNode
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//==============================================================================
void TrackEditor::CreateTileDisplayNode()
{
    if ( !MExt::FindDagNodeByName( NULL, MString( TileDisplayNode::stringId ) ) )
    {
        MExt::CreateNode( 0, 0, MString( TileDisplayNode::stringId ) );
    }
}

//==============================================================================
// TrackEditor::RemoveTileDisplayNode
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//==============================================================================
void TrackEditor::RemoveTileDisplayNode()
{
    MDagPath dagPath;

    if ( MExt::FindDagNodeByName( &dagPath, MString( TileDisplayNode::stringId ) ) )
    { 
        MFnDagNode fnDagNode( dagPath );

        MExt::DeleteNode( fnDagNode.object(), true );
    }
}   
 
