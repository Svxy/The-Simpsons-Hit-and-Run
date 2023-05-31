#include "precompiled/PCH.h"

#include "intersection.h"
#include "main/constants.h"
#include "utility/glext.h"
#include "utility/mext.h"
#include "utility/nodehelper.h"

#include "utility/transformmatrix.h"
#include <toollib.hpp>


MTypeId IntersectionLocatorNode::id( TEConstants::TypeIDPrefix, TEConstants::NodeIDs::Intersection );
const char* IntersectionLocatorNode::stringId = "IntersectionLocatorNode";

//Attribute
const char* IntersectionLocatorNode::TYPE_NAME_LONG = "IntersectionType";
const char* IntersectionLocatorNode::TYPE_NAME_SHORT = "it";
MObject IntersectionLocatorNode::mType;

const char* IntersectionLocatorNode::ROAD_LONG = "Roads";
const char* IntersectionLocatorNode::ROAD_SHORT = "R";
MObject IntersectionLocatorNode::mRoads; 


const int IntersectionLocatorNode::ACTIVE_COLOUR = 13;
const int IntersectionLocatorNode::INACTIVE_COLOUR = 22;
const float IntersectionLocatorNode::SCALE = 1.0f * TEConstants::Scale;

IntersectionLocatorNode::IntersectionLocatorNode() {};
IntersectionLocatorNode::~IntersectionLocatorNode() {};

//==============================================================================
// IntersectionLocatorNode::creator
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//==============================================================================
void* IntersectionLocatorNode::creator()
{
    return new IntersectionLocatorNode();
}

//==============================================================================
// IntersectionLocatorNode::initialize
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MStatus 
//
//==============================================================================
MStatus IntersectionLocatorNode::initialize()
{
    MStatus status;
    MFnTypedAttribute fnTyped;
    MFnMessageAttribute fnMessage;

    mType = fnTyped.create( TYPE_NAME_LONG, TYPE_NAME_SHORT, MFnData::kString, MObject::kNullObj, &status );
    RETURN_STATUS_ON_FAILURE( status );
    
    RETURN_STATUS_ON_FAILURE( addAttribute( mType ) );

    mRoads = fnMessage.create( ROAD_LONG, ROAD_SHORT, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( fnMessage.setReadable( false ) );
    RETURN_STATUS_ON_FAILURE( fnMessage.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( fnMessage.setArray( true ) );
    RETURN_STATUS_ON_FAILURE( fnMessage.setIndexMatters( false ) );

    RETURN_STATUS_ON_FAILURE( addAttribute( mRoads ) );


    return MS::kSuccess;
}

//==============================================================================
// IntersectionLocatorNode::postConstructor
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//==============================================================================
void IntersectionLocatorNode::postConstructor()
{
}


//==============================================================================
// IntersectionLocatorNode::draw
//==============================================================================
// Description: Comment
//
// Parameters:  draw( M3dView & view,
//                    const MDagPath & path, 
//       	          M3dView::DisplayStyle style,
//					  M3dView::DisplayStatus status )
//
// Return:      void 
//
//==============================================================================
void IntersectionLocatorNode::draw( M3dView & view,
                            const MDagPath & path, 
			                M3dView::DisplayStyle style,
					        M3dView::DisplayStatus status )
{
    view.beginGL();
    glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );

    if ( status == M3dView::kDormant ) 
    {
            int colour = NodeHelper::OverrideNodeColour( thisMObject(), INACTIVE_COLOUR );

	        view.setDrawColor( colour, M3dView::kDormantColors );
    }  
    else
    {
	    view.setDrawColor( ACTIVE_COLOUR, M3dView::kActiveColors );
    }

    //Draw a star to represent the locator.
    GLExt::drawI( TEConstants::Scale );
    GLExt::drawSphere( SCALE );

    glPopAttrib();
    view.endGL();
}

//==============================================================================
// IntersectionLocatorNode::Export
//==============================================================================
// Description: Comment
//
// Parameters:  ( MObject& intersectionLocatorNode, tlHistory& history )
//
// Return:      tlDataChunk
//
//==============================================================================
tlDataChunk* IntersectionLocatorNode::Export( MObject& intersectionLocatorNode, 
                                              tlHistory& history )
{
    MFnDagNode fnNode( intersectionLocatorNode );

    if ( fnNode.typeId() == IntersectionLocatorNode::id )
    {
        tlIntersectionChunk* intersectionChunk = new tlIntersectionChunk;

        intersectionChunk->SetName( fnNode.name().asChar() );

        MPoint thisPosition;
        MExt::GetWorldPosition( &thisPosition, intersectionLocatorNode );

        tlPoint point;
        point[0] = thisPosition[0] / TEConstants::Scale;
        point[1] = thisPosition[1] / TEConstants::Scale;
        point[2] = -thisPosition[2] / TEConstants::Scale;  //Maya vs. P3D...

        intersectionChunk->SetCentre( rmt::Vector( point ) );

        //GetScale...
        MObject transform;
        transform = fnNode.parent( 0 );
        MFnTransform fnTransform( transform );

        MDagPath dagPath;
        MExt::FindDagNodeByName( &dagPath, fnTransform.name() );
        TransformMatrix tm( dagPath );

        double scaleX;
        fnTransform.findPlug( MString( "sx" ) ).getValue( scaleX );

        intersectionChunk->SetRadius( (float)scaleX );

        MPlug typePlug = fnNode.findPlug( mType );
        MString type;
        typePlug.getValue( type );

        if ( MString("NoStop") == type )
        {
            intersectionChunk->SetType( 0 );
        }
        else if ( MString("NWay") == type )
        {
            intersectionChunk->SetType( 1 );
        }
        else if ( MString("FourWay") == type )
        {
            intersectionChunk->SetType( 2 );
        }
        else if ( MString("NoStopN") == type )
        {
            intersectionChunk->SetType( 3 );
        }
        else //if ( MString("NWay") == type )
        {
            intersectionChunk->SetType( 4 );
        }


        return intersectionChunk;
    }

    assert( false );
    return NULL;
}