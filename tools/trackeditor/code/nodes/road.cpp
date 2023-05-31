#include "road.h"
#include "utility/mext.h"
#include "utility/transformmatrix.h"
#include <toollib.hpp>

const tlPoint MVectorTotlPoint( const MVector& vector )
{
    tlPoint point;
    point[0] = vector[0];
    point[1] = vector[1];
    point[2] = vector[2];

    return point;
}

MTypeId RoadNode::id(  TEConstants::TypeIDPrefix, TEConstants::NodeIDs::Road );
const char*  RoadNode::stringId = "RoadNode";

const char*  RoadNode::ROAD_SEG_NAME_SHORT = "RoadSegments";
const char*  RoadNode::ROAD_SEG_NAME_LONG = "rs";
MObject      RoadNode::mRoadSegments;

const char*  RoadNode::INTERSECTION_START_SHORT = "is";
const char*  RoadNode::INTERSECTION_START_LONG = "IntersectionStart";
MObject      RoadNode::mIntersectionStart;

const char*  RoadNode::INTERSECTION_END_SHORT = "ie";
const char*  RoadNode::INTERSECTION_END_LONG = "IntersectionEnd";
MObject      RoadNode::mIntersectionEnd;

const char*  RoadNode::DENSITY_SHORT = "den";
const char*  RoadNode::DENSITY_LONG = "density";
MObject      RoadNode::mDensity;

const char*  RoadNode::SPEED_SHORT = "spd";
const char*  RoadNode::SPEED_LONG = "speed";
MObject      RoadNode::mSpeed;

const char*  RoadNode::DIFF_SHORT = "diff";
const char*  RoadNode::DIFF_LONG = "difficulty";
MObject      RoadNode::mDiff;

const char*  RoadNode::SHORTCUT_SHORT = "shct";
const char*  RoadNode::SHORTCUT_LONG = "shortCut";
MObject      RoadNode::mShortcut;



//==============================================================================
// RoadNode::RoadNode
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      RoadNode
//
//==============================================================================
RoadNode::RoadNode() {}

//==============================================================================
// RoadNode::~RoadNode
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      RoadNode
//
//==============================================================================
RoadNode::~RoadNode() {}

//==============================================================================
// RoadNode::creator
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//==============================================================================
void* RoadNode::creator()
{
    return new RoadNode();
}

//==============================================================================
// RoadNode::initialize
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MStatus 
//
//==============================================================================
MStatus RoadNode::initialize()
{   
    MStatus status;
    MFnMessageAttribute fnMessage;

    mRoadSegments = fnMessage.create( ROAD_SEG_NAME_LONG, ROAD_SEG_NAME_SHORT, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( fnMessage.setReadable( false ) );
    RETURN_STATUS_ON_FAILURE( fnMessage.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( fnMessage.setArray( true ) );
    RETURN_STATUS_ON_FAILURE( fnMessage.setIndexMatters( false ) );

    RETURN_STATUS_ON_FAILURE( addAttribute( mRoadSegments ) );

    mIntersectionStart = fnMessage.create( INTERSECTION_START_LONG, INTERSECTION_START_SHORT, &status );
    RETURN_STATUS_ON_FAILURE( status );

    RETURN_STATUS_ON_FAILURE( addAttribute( mIntersectionStart ) );

    mIntersectionEnd = fnMessage.create( INTERSECTION_END_LONG, INTERSECTION_END_SHORT, &status );
    RETURN_STATUS_ON_FAILURE( status );

    RETURN_STATUS_ON_FAILURE( addAttribute( mIntersectionEnd ) );

    MFnNumericAttribute fnNumeric;
    mDensity = fnNumeric.create( DENSITY_LONG, DENSITY_SHORT, MFnNumericData::kInt, 5, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( fnNumeric.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( fnNumeric.setWritable( true ) );

    RETURN_STATUS_ON_FAILURE( addAttribute( mDensity ) );

    mSpeed = fnNumeric.create( SPEED_LONG, SPEED_SHORT, MFnNumericData::kInt, 50, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( fnNumeric.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( fnNumeric.setWritable( true ) );

    RETURN_STATUS_ON_FAILURE( addAttribute( mSpeed ) );

    mShortcut = fnNumeric.create( SHORTCUT_LONG, SHORTCUT_SHORT, MFnNumericData::kBoolean, false, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( fnNumeric.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( fnNumeric.setWritable( true ) );

    RETURN_STATUS_ON_FAILURE( addAttribute( mShortcut ) );

    mDiff = fnNumeric.create( DIFF_LONG, DIFF_SHORT, MFnNumericData::kInt, 0, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( fnNumeric.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( fnNumeric.setWritable( true ) );

    RETURN_STATUS_ON_FAILURE( addAttribute( mDiff ) );


    return MS::kSuccess;
}

//==============================================================================
// RoadNode::postConstructor
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//==============================================================================
void RoadNode::postConstructor()
{
    //No moving the road.
    MPlug lPlug( thisMObject(), localPosition );
    lPlug.setLocked( true );

    MPlug wPlug( thisMObject(), worldPosition );
    wPlug.setLocked( true );
}

//This is how you export one of these.
//=============================================================================
// RoadNode::Export
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& roadNode, tlHistory& history, tlDataChunk* outChunk )
//
// Return:      tlDataChunk
//
//=============================================================================
tlDataChunk* RoadNode::Export( MObject& roadNode, tlHistory& history, tlDataChunk* outChunk ) //I think this is hackish.
{
    //This fenceline assumes that there are fences below it.
    MFnDagNode fnNode( roadNode );

    if ( fnNode.typeId() == RoadNode::id )
    {
        MFnDependencyNode fnTempNode;

        tlRoadChunk* roadChunk = new tlRoadChunk;

        roadChunk->SetName( fnNode.name().asChar() );
        roadChunk->SetType( 0 );

        //Get the intersections.

        //START
        MPlug intersectionPlug = fnNode.findPlug( mIntersectionStart );
        MPlugArray source, dest;
        MExt::ResolveConnections( &source, &dest, intersectionPlug, AS_SOURCE );

        if ( dest.length() == 0 )
        {
            MExt::DisplayError( "ERROR: Road %s has no start intersection!", fnNode.name().asChar() );
            delete roadChunk;
            return NULL;
        }

        fnTempNode.setObject( dest[0].node() );
        roadChunk->SetStartIntersection( fnTempNode.name().asChar() );

        //END
        intersectionPlug = fnNode.findPlug( mIntersectionEnd );
        MExt::ResolveConnections( &source, &dest, intersectionPlug, AS_SOURCE );

        if ( dest.length() == 0 )
        {
            MExt::DisplayError( "ERROR: Road %s has no end intersection!", fnNode.name().asChar() );
            delete roadChunk;
            return NULL;
        }

        fnTempNode.setObject( dest[0].node() );
        roadChunk->SetEndIntersection( fnTempNode.name().asChar() );

        int density;
        fnNode.findPlug( mDensity ).getValue( density );
        roadChunk->SetDensity( density );

        int speed;
        fnNode.findPlug( mSpeed ).getValue( speed );
        if ( speed > 255 || speed < 0 )
        {
            speed = 255;  //No need to mask, but what the hell.
        }

        int diff;
        fnNode.findPlug( mDiff ).getValue( diff );
        if ( diff > 255 || diff < 0 )
        {
            diff = 255;
        }

        bool sc;
        fnNode.findPlug( mShortcut ).getValue( sc );


        //This works differently now.
        //8 bits  - speed
        //8 bits  - difficulty level
        //1 bit   - id Short cut
        //15 bits - saved for later
        const int SPEED_MASK = 0x000000FF;
        const int DIFFIC_MASK = 0x0000FF00;
        const int SC_MASK = 0x00010000;

        roadChunk->SetSpeed( speed | ( diff << 8 ) | ( sc ? SC_MASK : 0 ) );

        //New set all the road segment chunks.
        MPlug roadSegPlug = fnNode.findPlug( mRoadSegments );
        MExt::ResolveConnections( &source, &dest, roadSegPlug, AS_DEST );

        MDagPath dagPath;
        MPlug tempPlug;
        tlRoadSegmentChunk* roadSegChunk = NULL;
        tlRoadSegmentDataChunk* roadSegDataChunk = NULL;
        unsigned int i; 
        for ( i = 0; i < source.length(); ++i )
        {
            //Create new tlRoadSegmentChunks
            roadSegChunk = new tlRoadSegmentChunk;
            roadSegDataChunk = new tlRoadSegmentDataChunk;

            fnTempNode.setObject( source[ i ].node() );
            MExt::FindDagNodeByName( &dagPath, fnTempNode.name() );
            MFnMesh fnMesh( dagPath );

            roadSegChunk->SetName( fnMesh.name().asChar() );
            roadSegChunk->SetRoadSegmentData( fnMesh.name().asChar() );
            roadSegDataChunk->SetName( fnMesh.name().asChar() );

            tempPlug = fnMesh.findPlug( MString( "teType" ) );
            int type;
            tempPlug.getValue( type );
            roadSegDataChunk->SetType( type < 0 ? 0 : type );

            tempPlug = fnMesh.findPlug( MString( "teLanes" ) );
            int lanes;
            tempPlug.getValue( lanes );
            roadSegDataChunk->SetNumLanes( lanes );

            tempPlug = fnMesh.findPlug( MString( "teShoulder" ) );
            bool shoulder;
            tempPlug.getValue( shoulder );
            roadSegDataChunk->SetHasShoulder( shoulder ? 1 : 0 );

            MPointArray points;
            fnMesh.getPoints( points, MSpace::kWorld );

            //ORIGIN
            tempPlug = fnMesh.findPlug( MString( "teOrigin" ) );
            int origin;
            tempPlug.getValue( origin );
            assert( origin >= 0 );
            MPoint orig;
            orig[ 0 ] = points[origin][0] / TEConstants::Scale;
            orig[ 1 ] = points[origin][1] / TEConstants::Scale;
            orig[ 2 ] = -points[origin][2] / TEConstants::Scale;

            //DIRECTION
            tempPlug = fnMesh.findPlug( MString( "teRoad" ) );
            int direction;
            tempPlug.getValue( direction );
            assert( direction >= 0 );
            MPoint dir;
            dir[ 0 ] = points[direction][0] / TEConstants::Scale;
            dir[ 1 ] = points[direction][1] / TEConstants::Scale;
            dir[ 2 ] = -points[direction][2] / TEConstants::Scale;

            MVector vDir = dir - orig;
            roadSegDataChunk->SetDirection( MVectorTotlPoint( vDir ) );

            //TOP
            tempPlug = fnMesh.findPlug( MString( "teTop" ) );
            int top;
            tempPlug.getValue( top );
            assert( top >= 0 );
            MPoint topPoint;
            topPoint[ 0 ] = points[top][0] / TEConstants::Scale;
            topPoint[ 1 ] = points[top][1] / TEConstants::Scale;
            topPoint[ 2 ] = -points[top][2] / TEConstants::Scale;

            MVector vTop = topPoint - orig;
            roadSegDataChunk->SetTop( MVectorTotlPoint( vTop ) );

            //BOTTOM
            tempPlug = fnMesh.findPlug( MString( "teBottom" ) );
            int bottom;
            tempPlug.getValue( bottom );
            assert( bottom >= 0 );
            MPoint bot;
            bot[ 0 ] = points[bottom][0] / TEConstants::Scale;
            bot[ 1 ] = points[bottom][1] / TEConstants::Scale;
            bot[ 2 ] = -points[bottom][2] / TEConstants::Scale;

            MVector vBottom = bot - orig;
            roadSegDataChunk->SetBottom( MVectorTotlPoint( vBottom ) );


            //Lets to the SCALE and ROTATION of the segment.
            MPointArray worldPoints;
            fnMesh.getPoints( worldPoints, MSpace::kWorld );

            //WORLD ORIGIN
            MPoint worldOrig;
            worldOrig[ 0 ] = worldPoints[origin][0] / TEConstants::Scale;
            worldOrig[ 1 ] = worldPoints[origin][1] / TEConstants::Scale;
            worldOrig[ 2 ] = -worldPoints[origin][2] / TEConstants::Scale;

            //Get the parent transform matrix for the mesh.
            MObject meshTransformObj = fnMesh.parent( 0 );
            MFnTransform fnTransform( meshTransformObj );
            MExt::FindDagNodeByName( &dagPath, fnTransform.name() );
            TransformMatrix tm( dagPath );

            tlMatrix hmatrix;
            tm.GetHierarchyMatrixLHS( hmatrix );
            //Make this p3d friendly...
            hmatrix.element[3][0] /= TEConstants::Scale;
            hmatrix.element[3][1] /= TEConstants::Scale;
            hmatrix.element[3][2] /= TEConstants::Scale;

            if ( hmatrix.element[3][0] == 0.0f &&
                 hmatrix.element[3][1] == 0.0f && 
                 hmatrix.element[3][2] == 0.0f )
            {
                //This could be a frozen
                MExt::DisplayWarning( "%s could have it's transforms frozen! NOT GOOD! Forced to assume object was built at origin.", fnMesh.name().asChar() );

                hmatrix.element[3][0] = worldOrig.x;
                hmatrix.element[3][1] = worldOrig.y;
                hmatrix.element[3][2] = worldOrig.z;
            }

            tlMatrix smatrix;
            tm.GetScaleMatrixLHS( smatrix );

            //MATRICIES
            roadSegChunk->SetHierarchyMatrix( hmatrix );
            roadSegChunk->SetScaleMatrix( smatrix );
            
            //DONE
            roadChunk->AppendSubChunk( roadSegChunk );

            outChunk->AppendSubChunk( roadSegDataChunk );
        }

        return roadChunk;
    }

    assert( false );
    return NULL;
}

