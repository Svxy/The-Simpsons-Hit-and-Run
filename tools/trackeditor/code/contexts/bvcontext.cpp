//----------------------------------------
// System Includes
//----------------------------------------


//----------------------------------------
// Project Includes
//----------------------------------------

#include "bvcontext.h"
#include "utility/Mext.h"
#include "nodes/walllocator.h"
#include "nodes/fenceline.h"
#include "nodes/nu.h"
#include "main/trackeditor.h"

//----------------------------------------
// Constants, Typedefs and Statics
//----------------------------------------
const char* BVContext::stringId = "BVContext";
int BVContext::sLeftSide = WallLocatorNode::LEFT;
const MString BVContext::DEFAULT_GROUP_NAME = "FenceLine";
MObject BVContext::sCurrentGroup;


const char* BVSplitCmd::stringId = "BVSplitSelected";

//==============================================================================
// BVContextCmd::BVContextCmd
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      BVContextCmd
//
//==============================================================================
BVContextCmd::BVContextCmd()
{
}

//==============================================================================
// BVContextCmd::~BVContextCmd
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      BVContextCmd
//
//==============================================================================
BVContextCmd::~BVContextCmd()
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
void* BVContextCmd::creator()
{
    return new BVContextCmd();
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
MPxContext* BVContextCmd::makeObj()
{
    return new BVContext();
}

//==============================================================================
// BVContext::BVContext
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      BVContext
//
//==============================================================================
BVContext::BVContext() :
    mXCurrent( 0 ),
    mYCurrent( 0 )
{
    SetHelpString();

    setTitleString( "Bounding Volume Path Tool" );    
} 

//==============================================================================
// BVContext::~BVContext
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      BVContext
//
//==============================================================================
BVContext::~BVContext()
{
}

//==============================================================================
// BVContext::abortAction
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//==============================================================================
void BVContext::abortAction()
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
void BVContext::completeAction()
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
void BVContext::deleteAction()
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
MStatus BVContext::doDrag( MEvent& event )
{

    event.getPosition( mXCurrent, mYCurrent );
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
MStatus BVContext::doEnterRegion( MEvent& event )
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
MStatus BVContext::doHold( MEvent& event )
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
MStatus BVContext::doPress( MEvent& event )
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
MStatus BVContext::doRelease( MEvent& event )
{
    if ( event.mouseButton() == MEvent::kLeftMouse )
    {
        event.getPosition( mXCurrent, mYCurrent );
        ProcessState( BUTTONUP );
    }
    else if ( event.mouseButton() == MEvent::kMiddleMouse )
    {
        //Toggle the leftness...
        sLeftSide = sLeftSide == WallLocatorNode::LEFT ? WallLocatorNode::RIGHT : WallLocatorNode::LEFT;

        SetHelpString();
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
void BVContext::toolOffCleanup()
{
    CloseLoop();
    mPoints.clear();
    sCurrentGroup = MObject::kNullObj;
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
void BVContext::toolOnSetup( MEvent& event )
{
    setCursor( MCursor::crossHairCursor );

    mPoints.clear();
    sCurrentGroup = MObject::kNullObj;
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
void BVContext::ProcessState( Stimulus stimulus )
{
    switch( stimulus )
    {
    case BUTTONDOWN:
        {
        }
        break;

    case BUTTONUP:
        {
            MObject newNode;
            MObject nodeTransform;

            MExt::CreateNode( newNode, nodeTransform, MString( WallLocatorNode::stringId )  );

            NODE_UTIL::DisableAttributes( newNode );

            MExt::Attr::Set( sLeftSide, 
                             newNode, 
                             WallLocatorNode::LEFTRIGHT_NAME_LONG );

            //Set the position

            MPoint vp( mXCurrent, mYCurrent, 0 );
            MPoint wp;
            MExt::ViewToWorldAtY( &wp, vp, 0 );
            MExt::SetWorldPosition( wp, newNode );

            AddPoint( newNode );
        }
        break;
    case DELETED:
        {
            DeleteLast();
        }
        break;
    case COMPLETED:
        {
            //Complete the loop and start a new one.
            CloseLoop();
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
// BVContext::AddPoint
//==============================================================================
// Description: Comment
//
// Parameters:  ( MObject obj )
//
// Return:      void 
//
//==============================================================================
void BVContext::AddPoint( MObject obj )
{
    MStatus status;
    unsigned int size = mPoints.length();

    if ( size )
    {
        MObject lastNode;

        lastNode = mPoints[ size - 1 ];

        if ( lastNode.isNull() )
        {
            //Someone has been deleting nodes.
            MExt::DisplayError( "Someone has deleted something..." );
            return;
        }

        MExt::Connect( lastNode, WallLocatorNode::NEXTNODE_NAME_LONG, obj, WallLocatorNode::PREVNODE_NAME_LONG );
    }
    else
    {
        //Starting a new group
        MObject flT;
        MString name( DEFAULT_GROUP_NAME );

        MExt::CreateNode( sCurrentGroup, flT, MString( FenceLineNode::stringId ), &name );

        //Parent this group to the main TrackEditor Node if it exists.
        TrackEditor::AddChild( sCurrentGroup );
    }

    mPoints.append( obj );

    //Add the point (wall) to the current fence
    FenceLineNode::AddWall( sCurrentGroup, obj );
}

//==============================================================================
// BVContext::DeleteLast
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//==============================================================================
void BVContext::DeleteLast()
{
    unsigned int size = mPoints.length();

    if ( size )
    {
        MStatus status;

        MObject obj = mPoints[ size - 1 ];
        mPoints.remove( size - 1 );

        MExt::DeleteNode( obj, true );
    }

    if ( mPoints.length() == 0 && !sCurrentGroup.isNull() )
    {
        //we deleted the last one.
        //Remove the group object.
        MExt::DeleteNode( sCurrentGroup, true );
    }
}

//==============================================================================
// BVContext::CloseLoop
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//==============================================================================
void BVContext::CloseLoop()
{
    unsigned int size = mPoints.length();

    if ( size == 1 )
    {
        MExt::DisplayWarning( "There was only one point in the BV loop.  It will be deleted." );

        DeleteLast();
    }
    else if ( size == 2 )
    {
        MExt::DisplayWarning( "There were only two points in the BV loop.  They will be deleted." );

        DeleteLast();
        DeleteLast();
    }
    else if ( size > 2 )
    {
        MObject lastNode, firstNode;
        MStatus status;

        lastNode = mPoints[ size - 1 ];
        firstNode = mPoints[ 0 ];

        MExt::Connect( lastNode, WallLocatorNode::NEXTNODE_NAME_LONG, firstNode, WallLocatorNode::PREVNODE_NAME_LONG );

        //Clear the points list to start a new loop.
        mPoints.clear();
        sCurrentGroup;
    }
}

//==============================================================================
// BVContext::SetHelpString
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//==============================================================================
void BVContext::SetHelpString()
{
    mHelp = "Click to place nodes in the path.";

    if ( sLeftSide )
    {
        mHelp += "LEFT-SIDED";
    }
    else
    {
        mHelp += "RIGHT-SIDED";
    }

    setHelpString( mHelp );

}

//SPLIT COMMAND
//==============================================================================
// BVSplitCmd::creator
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//==============================================================================
void* BVSplitCmd::creator()
{
    return new BVSplitCmd();
}

//==============================================================================
// BVSplitCmd::doIt
//==============================================================================
// Description: Comment
//
// Parameters:  ( const MArgList &args )
//
// Return:      MStatus 
//
//==============================================================================
MStatus BVSplitCmd::doIt( const MArgList &args )
{
    MSelectionList selectionList;
    
    MGlobal::getActiveSelectionList( selectionList );

    if ( selectionList.isEmpty() )
    {
        //Nothing to do.
        return MS::kSuccess;
    }

    //Get the number of objects in the list.
    unsigned int numObjs = selectionList.length();

    MObject obj;
    MFnDependencyNode fnNode;
    MObjectArray objArray;

    unsigned int i;
    for ( i = 0; i < numObjs; ++i )
    {
        selectionList.getDependNode( i, obj );
        fnNode.setObject( obj );

        if ( fnNode.typeId() == WallLocatorNode::id )
        {
            //This is a wall locator, add it to the array.
            objArray.append( obj );
        }
        else
        {
            //This could be a transform, let's test the child node.
            MFnDagNode dagNode( obj );
            if( dagNode.childCount() )
            {
                //Get the first child
                MObject child = dagNode.child( 0 );

                fnNode.setObject( child );
                if ( fnNode.typeId() == WallLocatorNode::id )
                {
                    //This is a wall locator, add it to the array.
                    objArray.append( child );
                }
            }
        }
    }

    if ( objArray.length() <= 1 )
    {
        //Nothing to do.
        return MS::kSuccess;
    }

    //For each object in the objArray that is connected to another, create a node in-between...
    MStatus status;
    MObject obj1, obj2;
    MFnDependencyNode fnNode1, fnNode2;
    MPlug nextPlug, prevPlug;

    unsigned int j;
    for ( i = 0; i < objArray.length() - 1; ++i )
    {
        for ( j = i + 1; j < objArray.length(); ++j )
        {
            //Check if i and j are connected.
            obj1 = objArray[i];
            obj2 = objArray[j];

            fnNode1.setObject( obj1 );
            fnNode2.setObject( obj2 );
            
            //Compare obj1.next to obj2.prev
            nextPlug = fnNode1.findPlug( WallLocatorNode::NEXTNODE_NAME_LONG, &status );
            assert( status );
            prevPlug = fnNode2.findPlug( WallLocatorNode::PREVNODE_NAME_LONG, &status );
            assert( status );

            if ( MExt::IsConnected( nextPlug, prevPlug ) )
            {
                //Split and connect these two objects.
                Split( obj1, obj2 );
            }
            else
            {
                //Compare obj2.next to obj1.prev
                nextPlug = fnNode2.findPlug( WallLocatorNode::NEXTNODE_NAME_LONG, &status );
                assert( status );
                prevPlug = fnNode1.findPlug( WallLocatorNode::PREVNODE_NAME_LONG, &status );
                assert( status );

                if ( MExt::IsConnected( nextPlug, prevPlug ) )
                {
                    //Split and connect these two objects.
                    Split( obj2, obj1 );
                }
            }
        }
    }

    return MS::kSuccess;
}

//==============================================================================
// BVSplitCmd::Split
//==============================================================================
// Description: Comment
//
// Parameters:  ( MObject& node1, MObject& node2 )
//
// Return:      void 
//
//==============================================================================
void BVSplitCmd::Split( MObject& node1, MObject& node2 )
{
    //Take node1 and node2, create a newNode between them and connect
    /// node1.next -> newNode.prev and newNode.next -> node2.prev

    //Disconnect the nodes.
    MExt::DisconnectAll( node1, WallLocatorNode::NEXTNODE_NAME_LONG );

    MObject newNode;
    MObject nodeTransform;

    MExt::CreateNode( newNode, nodeTransform, MString( WallLocatorNode::stringId )  );

    NODE_UTIL::DisableAttributes( newNode );

    //This will split based on one of the others.
    int isLeft;
    MExt::Attr::Get( &isLeft, node1, WallLocatorNode::LEFTRIGHT_NAME_LONG );

    MExt::Attr::Set( !isLeft == WallLocatorNode::LEFT ? WallLocatorNode::RIGHT : WallLocatorNode::LEFT, 
                     newNode, 
                     WallLocatorNode::LEFTRIGHT_NAME_LONG );

    MPoint newWP = MExt::GetWorldPositionBetween( node1, node2 );
    //Lock the y to 0;
    newWP[1] = 0;
   
    MExt::SetWorldPosition( newWP, newNode );

    //Connect the nodes in their new order.
    MExt::Connect( node1, WallLocatorNode::NEXTNODE_NAME_LONG, newNode, WallLocatorNode::PREVNODE_NAME_LONG );
    MExt::Connect( newNode, WallLocatorNode::NEXTNODE_NAME_LONG, node2, WallLocatorNode::PREVNODE_NAME_LONG );

    //Make sure the node is parented properly...

    MFnDagNode fnDagNode( node1 );
    MObject parentT = fnDagNode.parent( 0 );

    fnDagNode.setObject( parentT );
    MObject groupT = fnDagNode.parent( 0 );

    FenceLineNode::AddWall( groupT, newNode );
}

