#include "precompiled/PCH.h"

#include "walllocator.h"
#include "main/constants.h"
#include "utility/glext.h"
#include "utility/mext.h"
#include "utility/nodehelper.h"

#include <toollib.hpp>


MTypeId WallLocatorNode::id( TEConstants::TypeIDPrefix, TEConstants::NodeIDs::WallLocator );
const char* WallLocatorNode::stringId = "WallLocatorNode";

const int WallLocatorNode::ACTIVE_COLOUR = 13;
const int WallLocatorNode::INACTIVE_COLOUR = 22;
const float WallLocatorNode::SCALE = 1.0f * TEConstants::Scale;

//This is an attribute.

const char* WallLocatorNode::LEFTRIGHT_NAME_LONG = "leftRight";
const char* WallLocatorNode::LEFTRIGHT_NAME_SHORT = "lr";
MObject WallLocatorNode::mLeftRight;

const char* WallLocatorNode::PREVNODE_NAME_LONG = "prevNode";
const char* WallLocatorNode::PREVNODE_NAME_SHORT = "pn";
MObject WallLocatorNode::mPrevNode;

const char* WallLocatorNode::NEXTNODE_NAME_LONG = "nextNode";
const char* WallLocatorNode::NEXTNODE_NAME_SHORT = "nn";
MObject WallLocatorNode::mNextNode;

const char* WallLocatorNode::ID_NAME_LONG = "callbackID";
const char* WallLocatorNode::ID_NAME_SHORT = "cb";
MObject WallLocatorNode::mCallbackId;

//==============================================================================
// WallLocatorNode::WallLocatorNode
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      WallLocatorNode
//
//==============================================================================
WallLocatorNode::WallLocatorNode() {};

//==============================================================================
// WallLocatorNode::~WallLocatorNode
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      WallLocatorNode
//
//==============================================================================
WallLocatorNode::~WallLocatorNode() {};

//==============================================================================
// WallLocatorNode::creator
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//==============================================================================
void* WallLocatorNode::creator()
{
    return new WallLocatorNode();
}

//==============================================================================
// WallLocatorNode::initialize
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MStatus 
//
//==============================================================================
MStatus WallLocatorNode::initialize()
{
    MFnMessageAttribute fnMessage;
    MFnNumericAttribute fnNumeric;
    MStatus status;

    //Create the left/right attrib
    mLeftRight = fnNumeric.create( LEFTRIGHT_NAME_LONG, LEFTRIGHT_NAME_SHORT, MFnNumericData::kInt, LEFT, &status );
    RETURN_STATUS_ON_FAILURE( status );
    fnNumeric.setDefault(LEFT);

    RETURN_STATUS_ON_FAILURE( addAttribute( mLeftRight ) );


    //Create the sttribute for the previous node.
    mPrevNode = fnMessage.create( PREVNODE_NAME_LONG, PREVNODE_NAME_SHORT, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( fnMessage.setReadable( false ) );
    RETURN_STATUS_ON_FAILURE( fnMessage.setWritable( true ) );

    RETURN_STATUS_ON_FAILURE( addAttribute( mPrevNode ) );

    //Create the sttribute for the next node.
    mNextNode = fnMessage.create( NEXTNODE_NAME_LONG, NEXTNODE_NAME_SHORT, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( fnMessage.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( fnMessage.setWritable( false ) );

    RETURN_STATUS_ON_FAILURE( addAttribute( mNextNode ) );


    mCallbackId = fnNumeric.create( ID_NAME_LONG, ID_NAME_SHORT, MFnNumericData::kLong, 0, &status );
    RETURN_STATUS_ON_FAILURE( status );
    fnNumeric.setDefault( 0 );

    RETURN_STATUS_ON_FAILURE( addAttribute( mCallbackId ) );

    return MS::kSuccess;
}

//==============================================================================
// WallLocatorNode::legalConnection 
//==============================================================================
// Description: Comment
//
// Parameters:  ( const MPlug & plug, const MPlug & otherPlug, bool asSrc, bool& result )
//
// Return:      MStatus 
//
//==============================================================================
MStatus WallLocatorNode::legalConnection ( const MPlug & plug, const MPlug & otherPlug, bool asSrc, bool& result ) const
{
    if ( otherPlug.node() == thisMObject() )
    {
        result = false;
        return MS::kSuccess;
    }

    if ( plug == mNextNode )
    {
        //This is the source of the connection.  
        //Therefore the connection is legal if I'm not already connected to the same node by the input.
        MFnDependencyNode fnNode;
        MStatus status;

        fnNode.setObject( thisMObject() );
        MPlug prevPlug = fnNode.findPlug( mPrevNode, &status );
        assert( status );

        if ( prevPlug.node() != otherPlug.node() )
        {
            //Go ahead and connect.
            result = true;
        }
        else
        {
            //Already connected to this node.  No 2-Node loops please.
            result = false;
        }

        return MS::kSuccess;
    }
    else if ( plug == mPrevNode )
    {
        //This is the destination of the connection.  
        //Therefore the connection is legal if I'm not already connected to the same node by the output
        MFnDependencyNode fnNode;
        MStatus status;

        fnNode.setObject( thisMObject() );
        MPlug nextPlug = fnNode.findPlug( mNextNode, &status );
        assert( status );

        if ( nextPlug.node() != otherPlug.node() )
        {
            //Go ahead and connect.
            result = true;
        }
        else
        {
            //Already connected to this node.  No 2-Node loops please.
            result = false;
        }
        return MS::kSuccess;
    }

    return MS::kUnknownParameter;
}

//==============================================================================
// WallLocatorNode::postConstructor
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//==============================================================================
void WallLocatorNode::postConstructor()
{
   //
    // Register a callback that will notify us just prior to this node being
    // deleted.
    // 
    MStatus status;
    MFnDependencyNode fnNode;

    fnNode.setObject( thisMObject() );
    MPlug plug = fnNode.findPlug( mCallbackId, &status );
    assert( status );

    int id = MNodeMessage::addNodeAboutToDeleteCallback( 
                        thisMObject(), 
                        NodeAboutToDeleteCallback,
                        (void*)(this),
                        &status
                 );

    plug.setValue( id );

    //Since this is a planar dealie, we want the Y to stay at 0...
    MPlug lyPlug( thisMObject(), localPositionY );
    lyPlug.setLocked( true );

    MPlug wyPlug( thisMObject(), worldPositionY );
    wyPlug.setLocked( true );

    assert( status );
}

//==============================================================================
// WallLocatorNode::draw
//==============================================================================
// Description: Comment
//
// Parameters:  ( M3dView & view,
//                const MDagPath & path, 
//                M3dView::DisplayStyle style,
//                M3dView::DisplayStatus status )
//
// Return:      void 
//
//==============================================================================
void WallLocatorNode::draw( M3dView & view,
                            const MDagPath & path, 
			                M3dView::DisplayStyle style,
					        M3dView::DisplayStatus status )
{
    view.beginGL();
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );

    //If there is a connected wall locator node, draw a line to it.  Then draw the arrow halfway between them.
    if ( MExt::IsConnected( thisMObject(), mNextNode ) )
    {
        //Can we stop the GL system from adding this to it's selection mechanism?

        //When we are in render mode, we draw the lines between the nodes.
        //If this was in GL_SELECTION_MODE, we would not draw the lines, so they won't interfere
        //with selection.
        GLint value;
        glGetIntegerv( GL_RENDER_MODE, &value );

        if ( (value == GL_RENDER) )
        {
            //Get the world position of the next node
            MStatus st;
            MPlugArray pa;
            MFnDependencyNode fnNode;

            fnNode.setObject( thisMObject() );
            MPlug plug = fnNode.findPlug( mNextNode, &st );
            assert( st );

            plug.connectedTo( pa, false, true, &st );
            assert( st );

            //There is only one thing plugged into this...
            MPlug nextPlug = pa[0];

            //Got the nextNode's plug, let's get the WorldPosition of the other node.
            MPoint nnwp;
            MExt::GetWorldPosition( &nnwp, nextPlug.node() );

            //Get the world position of this node.
            MPoint wp;
            MExt::GetWorldPosition( &wp, thisMObject() );

            MPoint localPosNN( nnwp - wp );
            MPoint localOrigin; // (0,0,0)

            int colour = NodeHelper::OverrideNodeColour( thisMObject(), INACTIVE_COLOUR );

	        view.setDrawColor( colour, M3dView::kDormantColors );

            GLExt::drawLine( localOrigin, localPosNN );


            //Draw the LEFT / RIGHT line 
            MPoint wpMiddleOfLine = MExt::GetWorldPositionBetween( thisMObject(), nextPlug.node() );

            MVector arrow;
            if ( CalculateNormal( nnwp, &arrow ) )
            {
                MPoint arrowFrom( wpMiddleOfLine - wp );
                double scale = ( localPosNN.distanceTo(localOrigin) / 6 );
                if ( scale > 5 * TEConstants::Scale )
                {
                    scale = 5 * TEConstants::Scale;
                }

                MPoint arrowTo( ( arrow * scale ) + arrowFrom );

                GLExt::drawLine( arrowFrom, arrowTo, 5.0f );
            }
        }
    }

    if ( status == M3dView::kDormant ) 
    {
            int colour = NodeHelper::OverrideNodeColour( thisMObject(), INACTIVE_COLOUR );

	        view.setDrawColor( colour, M3dView::kDormantColors );
    }  
    else
    {
	    view.setDrawColor( ACTIVE_COLOUR, M3dView::kDormantColors );
    }

    //Draw a star to represent the locator.
    GLExt::drawCrossHair3D( SCALE );

    glPopAttrib();
    view.endGL();
}

//==============================================================================
// WallLocatorNode::NodeAboutToDeleteCallback
//==============================================================================
// Description: Comment
//
// Parameters:  ( MDGModifier& modifier, void* data )
//
// Return:      void 
//
//==============================================================================
void WallLocatorNode::NodeAboutToDeleteCallback( MDGModifier& modifier, void* data )
{
    //
    // Get the this pointer for the node being deleted.
    // 
    WallLocatorNode* thisNode = (WallLocatorNode*)(data);
    assert( thisNode );

    //
    // Get the MObject corresponding to this node.
    //
    MObject node = thisNode->thisMObject();

    //Attach the neighbour nodes to eachother.
    MObject nextNode;
    MObject prevNode;

    if ( MExt::IsConnected( node, mNextNode ) && MExt::IsConnected( node, mPrevNode ))
    {
        MStatus status;
        MFnDependencyNode fnNode;
        fnNode.setObject( node );

        MPlug plug = fnNode.findPlug( mNextNode, &status );

        MPlugArray pa;
        plug.connectedTo( pa, false, true, &status );
        assert( status );

        MPlug nextPlug = pa[0];

        nextNode = nextPlug.node();


        fnNode.setObject( node );

        plug = fnNode.findPlug( mPrevNode, &status );

        plug.connectedTo( pa, true, false, &status );
        assert( status );

        MPlug prevPlug = pa[0];

        prevNode = prevPlug.node();

        //Remove all connections to this node.
        MExt::DisconnectAll( node, mNextNode );
        MExt::DisconnectAll( node, mPrevNode );


        //Connect the nodes together...  THANKS!
        if ( prevNode != nextNode )
        {
            MExt::Connect( prevNode, WallLocatorNode::NEXTNODE_NAME_LONG, nextNode, WallLocatorNode::PREVNODE_NAME_LONG );
        }
    } 

    //
    // cancel callback.
    //
    MStatus status;
    MFnDependencyNode fnNode;
    fnNode.setObject( node );

    int id;
    MPlug plug = fnNode.findPlug( mCallbackId, &status );
    plug.getValue( id );

    MMessage::removeCallback( id );
}

//==============================================================================
// WallLocatorNode::CalculateNormal
//==============================================================================
// Description: Comment
//
// Parameters:  ( MPoint& nextNodeWP, MVector* normal )
//
// Return:      bool 
//
//==============================================================================
bool WallLocatorNode::CalculateNormal( MPoint& nextNodeWP, MVector* normal )
{
    return CalculateNormal( thisMObject(), nextNodeWP, normal );
}

//==============================================================================
// WallLocatorNode::CalculateNormal
//==============================================================================
// Description: Comment
//
// Parameters:  ( MObject& thisNode, MPoint& nextNodeWP, MVector* normal )
//
// Return:      bool 
//
//==============================================================================
bool WallLocatorNode::CalculateNormal( MObject& thisNode, MPoint& nextNodeWP, MVector* normal )
{
    //Get the world position of this node.
    MPoint wp;
    MExt::GetWorldPosition( &wp, thisNode );

    MPoint localPosNN( nextNodeWP - wp );

    MVector nextNode( localPosNN );

    int isLeft = NONE;
    MExt::Attr::Get( &isLeft, thisNode, mLeftRight );

    if ( isLeft == LEFT )
    {
        MVector yUp( 0, -1.0f, 0 );
        *normal = nextNode ^ yUp; //Cross product.
    }
    else if ( isLeft == RIGHT)
    {
        MVector yUp( 0, 1.0f, 0 );
        *normal = nextNode ^ yUp; //Cross product.
    }
    else
    {
        return false;
    }

    normal->normalize();
    return true;
}

//==============================================================================
// WallLocatorNode::Export
//==============================================================================
// Description: Comment
//
// Parameters:  ( MObject& wallLocatorNode, tlHistory& history )
//
// Return:      tlDataChunk
//
//==============================================================================
tlDataChunk* WallLocatorNode::Export( MObject& wallLocatorNode, tlHistory& history )
{
    MFnDagNode fnNode( wallLocatorNode );

    if ( fnNode.typeId() == WallLocatorNode::id )
    {
        //Create a tlDataChunk and return it filled with the appropriate data.
        tlWallChunk* wall = new tlWallChunk;

        MStatus st;
        MPlugArray pa;
        MPlug nextPlug = fnNode.findPlug( mNextNode, &st );
        nextPlug.connectedTo( pa, false, true, &st );
        assert( st );

        //There is only one thing plugged into this...
        MPlug nextNodePlug = pa[0];
        MObject nextNode = nextNodePlug.node();
  
        MPoint thisPosition;
        MExt::GetWorldPosition( &thisPosition, wallLocatorNode );

        MPoint nextPosition;
        MExt::GetWorldPosition( &nextPosition, nextNode );

        MVector normal;
        bool hasNormal = CalculateNormal( wallLocatorNode, nextPosition, &normal );

        //Set the values.
        tlPoint point;

        point[0] = thisPosition[0] / TEConstants::Scale;
        point[1] = thisPosition[1] / TEConstants::Scale;
        point[2] = -thisPosition[2] / TEConstants::Scale;  //Maya vs. P3D...
        wall->SetStart( point );

        point[0] = nextPosition[0] / TEConstants::Scale;
        point[1] = nextPosition[1] / TEConstants::Scale;
        point[2] = -nextPosition[2] / TEConstants::Scale;  //Maya vs. P3D...
        wall->SetEnd( point );

        if ( hasNormal )
        {
            normal.normalize();
            point[0] = normal[0];
            point[1] = normal[1];
            point[2] = -normal[2];  //Maya vs. P3D...
        }
        else
        {
            point[0] = 0;
            point[1] = 0;
            point[2] = 0;
        }
        wall->SetNormal( point );

        return wall;
    }

    assert( false );
    return NULL;
}