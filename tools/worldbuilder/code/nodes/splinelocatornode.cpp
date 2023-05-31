//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        SplineLocatorNode.cpp
//
// Description: Implement SplineLocatorNode
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
#include "nodes/SplineLocatorNode.h"
#include "main/constants.h"
#include "main/worldbuilder.h"
#include "utility/mext.h"
#include "utility/glext.h"
#include "nodes/triggervolumenode.h"
#include "nodes/wbspline.h"
#include "nodes/railcamlocatornode.h"

#include "resources/resource.h"

#include "../../../game/code/meta/locatortypes.h"


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
const char* SplineLocatorNode::TRIGGERS_NAME_SHORT   = "trigs";
const char* SplineLocatorNode::TRIGGERS_NAME_LONG    = "triggers";
MObject SplineLocatorNode::sTriggers;

const char* SplineLocatorNode::SPLINE_NAME_LONG = "spline";
const char* SplineLocatorNode::SPLINE_NAME_SHORT = "spl";
MObject SplineLocatorNode::sSpline;

const char* SplineLocatorNode::CAMERA_NAME_LONG = "camera";
const char* SplineLocatorNode::CAMERA_NAME_SHORT = "cam";
MObject SplineLocatorNode::sCamera;

const char* SplineLocatorNode::IS_COIN_SHORT  = "ic";
const char* SplineLocatorNode::IS_COIN_LONG = "isCoin";
MObject SplineLocatorNode::sIsCoin;


const char* SplineLocatorNode::stringId = "SplineLocatorNode";
MTypeId SplineLocatorNode::id = WBConstants::NodeIDs::SplineLocator;


MPointArray GetCoinPoints( MObject& nurbsCurve, MObject& locator, bool justWP )
{
    MFnDagNode fnDagNode( nurbsCurve );
    MDagPath dagPath;
    fnDagNode.getPath( dagPath );

    MStatus status;

    MFnNurbsCurve fnNurbs( dagPath, &status );
    assert( status );

    const char* name = fnNurbs.name().asChar();

    MPointArray points;

    if ( status )
    {
        int numCVs = fnNurbs.numCVs( &status );
        assert( status );

        //Get the number of coins
        int numCoins = 0;
        fnDagNode.findPlug( MString("numCoins") ).getValue( numCoins );
        assert( numCoins );
 
        if ( numCoins == 0 )
        {
            MExt::DisplayError( "Coin spline: %s has no coins!", fnDagNode.name().asChar() );
        }
        else
        {
            //Need to undo funny transformations...
            MMatrix mat;
            mat.setToIdentity();

            MObject transform;
            MFnDependencyNode locDepNode( locator );
            MDagPath locatorDagPath;
            MExt::FindDagNodeByName( &locatorDagPath, locDepNode.name() );
            MFnDagNode locDagNode( locatorDagPath );

            const char* otherName = locDagNode.name().asChar();

            transform = locDagNode.parent( 0 );
            MFnDependencyNode transNode( transform );
            MDagPath transPath;
            MExt::FindDagNodeByName( &transPath, transNode.name() );
            MFnTransform fnTransform( transPath );

            const char* nameHere = fnTransform.name().asChar();

            MFnTransform splineTrans( dagPath ); //This is the splines transform.
            MPoint splinePos( splineTrans.translation( MSpace::kWorld ) );


            mat = fnTransform.transformationMatrix(); 

            mat = mat.inverse();

            double numCoinSpans = fnNurbs.numSpans();
            
            double coinsPerFullSpan = (double)numCoins / numCoinSpans;

            //Draw a coin at even intervals.
            double interval = 1.0 / coinsPerFullSpan;

            double i;
            for ( i = 0; i < numCoins; ++i )
            {
                MPoint point;
                fnNurbs.getPointAtParam( interval * i, point, MSpace::kWorld );

                if ( !justWP )
                {
                    //Put in world space
                    point *= mat;
                    point += splinePos;
                }

                points.append( point );
            }
        }
    } 

    return points;
}

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// SplineLocatorNode::SplineLocatorNode
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SplineLocatorNode::SplineLocatorNode()
{
}

//==============================================================================
// SplineLocatorNode::~SplineLocatorNode
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SplineLocatorNode::~SplineLocatorNode()
{
}

//=============================================================================
// SplineLocatorNode::draw
//=============================================================================
// Description: Comment
//
// Parameters:  ( M3dView& view, const MDagPath& path, M3dView::DisplayStyle displayStyle, M3dView::DisplayStatus displayStatus )
//
// Return:      void 
//
//=============================================================================
void SplineLocatorNode::draw( M3dView& view, 
                              const MDagPath& path, 
                              M3dView::DisplayStyle displayStyle, 
                              M3dView::DisplayStatus displayStatus )
{
    MStatus status;
    MFnDependencyNode fnNode( thisMObject() );
    bool isCoins = false;
    fnNode.findPlug( sIsCoin ).getValue( isCoins );
    if ( isCoins )
    {

        const char* name = fnNode.name().asChar();

        //When we are in render mode, we draw the lines between the nodes.
        //If this was in GL_SELECTION_MODE, we would not draw the lines, so they won't interfere
        //with selection.
        GLint value;
        glGetIntegerv( GL_RENDER_MODE, &value );

        //Draw things here we don't want selectable.
        if ( (value == GL_RENDER) )
        {
            //Get the attached Maya spline.
            MPlugArray sources, targets;
            //Get the spline associated with this bad-boy.
            MPlug splinePlug = fnNode.findPlug( sSpline );
            MExt::ResolveConnections( &sources, &targets, splinePlug, true, false );
            assert( sources.length() == 1 );

            MPointArray points = GetCoinPoints( sources[0].node(), thisMObject(), false );
            unsigned int i;
            for ( i = 0; i < points.length(); ++i )
            {
                GLExt::drawO( 1.0 * WBConstants::Scale, points[i].x, points[i].y + 0.25, points[i].z, 3.0f );
            }
        }
    }
}

//=============================================================================
// SplineLocatorNode::initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MStatus 
//
//=============================================================================
MStatus SplineLocatorNode::initialize()
{
    MFnMessageAttribute msgAttr;
    MStatus status;

    sTriggers = msgAttr.create( TRIGGERS_NAME_LONG, TRIGGERS_NAME_SHORT, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( msgAttr.setReadable( false ) );
    RETURN_STATUS_ON_FAILURE( msgAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( msgAttr.setArray( true ) );
    RETURN_STATUS_ON_FAILURE( msgAttr.setIndexMatters( false ) );

    RETURN_STATUS_ON_FAILURE( addAttribute( sTriggers ) );

    sSpline = msgAttr.create( SPLINE_NAME_LONG, SPLINE_NAME_SHORT, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( msgAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( msgAttr.setWritable( true ) );

    RETURN_STATUS_ON_FAILURE( addAttribute( sSpline ) );

    sCamera = msgAttr.create( CAMERA_NAME_LONG, CAMERA_NAME_SHORT, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( msgAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( msgAttr.setWritable( true ) );

    RETURN_STATUS_ON_FAILURE( addAttribute( sCamera ) );

    MFnNumericAttribute numAttr;
    sIsCoin = numAttr.create( IS_COIN_LONG, IS_COIN_SHORT, MFnNumericData::kBoolean, false, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numAttr.setWritable( true ) );

    RETURN_STATUS_ON_FAILURE( addAttribute( sIsCoin ) );

    return MStatus::kSuccess;
}

//=============================================================================
// SplineLocatorNode::postConstructor
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SplineLocatorNode::postConstructor()
{
    setExistWithoutInConnections( false );
    setExistWithoutOutConnections( false );
}

//=============================================================================
// SplineLocatorNode::Export
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& splineLocatorNode )
//
// Return:      tlDataChunk
//
//=============================================================================
tlDataChunk* SplineLocatorNode::Export( MObject& splineLocatorNode )
{
    MFnDagNode fnNode( splineLocatorNode );

    if ( fnNode.typeId() == SplineLocatorNode::id )
    {
        bool isCoinSpline = false;
        fnNode.findPlug( sIsCoin ).getValue( isCoinSpline );
        if ( isCoinSpline )
        {
            //Export a coin spline.
            MPlugArray sources, targets;
            //Get the spline associated with this bad-boy.
            MPlug splinePlug = fnNode.findPlug( sSpline );
            MExt::ResolveConnections( &sources, &targets, splinePlug, true, false );
            assert( sources.length() == 1 );

            MPointArray points = GetCoinPoints( sources[0].node(), splineLocatorNode, true );

            tlWBLocatorChunk* locator = NULL;

            unsigned int coinCount = 0;

            if ( points.length() > 0 )
            {
                locator = new tlWBLocatorChunk;

                locator->SetType( LocatorType::GENERIC );

                unsigned int i;
                for ( i = 0; i < points.length(); ++i )
                {
                    tlWBLocatorChunk* coinLoc = new tlWBLocatorChunk;
                    coinLoc->SetType( LocatorType::COIN );

                    tlPoint point;
                    point.x = points[i].x / WBConstants::Scale;
                    point.y = points[i].y / WBConstants::Scale;
                    point.z = -points[i].z / WBConstants::Scale;
            
                    coinLoc->SetPosition( point );
                    
                    char name[256];
                    sprintf( name, "%sCoin%d", fnNode.name().asChar(), coinCount );
                    coinLoc->SetName( name );
                    ++coinCount;


                    locator->AppendSubChunk( coinLoc );
                }
            }

            return locator;

        }
        else
        {
            //Create a tlDataChunk and return it filled with the appropriate data.
            tlWBLocatorChunk* locator = new tlWBLocatorChunk;

            locator->SetName( fnNode.name().asChar() );

            locator->SetType( LocatorType::SPLINE );

            //The data here is the event associated with this locator.
            locator->SetNumDataElements( 0 );

            MPoint thisPosition;
            MExt::GetWorldPosition( &thisPosition, splineLocatorNode );

           //Set the values.
            tlPoint point;

            point[0] = thisPosition[0] / WBConstants::Scale;
            point[1] = thisPosition[1] / WBConstants::Scale;
            point[2] = -thisPosition[2] / WBConstants::Scale;  //Maya vs. P3D...
            locator->SetPosition( point );

            MPlugArray sources, targets;
            //Get the spline associated with this bad-boy.
            MPlug splinePlug = fnNode.findPlug( sSpline );
            MExt::ResolveConnections( &sources, &targets, splinePlug, true, false );
            assert( sources.length() == 1 );

            tlDataChunk* splineChunk = WBSpline::Export( sources[ 0 ].node() );
            assert( splineChunk );

            if ( splineChunk )
            {
                //Get the camera and append it here.
                MPlug railCamPlug = fnNode.findPlug( sCamera );
                MExt::ResolveConnections( &sources, &targets, railCamPlug, true, false );
                assert( targets.length() == 1 );

                tlDataChunk* railCamChunk = RailCamLocatorNode::Export( sources[0].node() );
                assert( railCamChunk );
                splineChunk->AppendSubChunk( railCamChunk );

                locator->AppendSubChunk( splineChunk );
            }
            else
            {
                MExt::DisplayError( "No spline attached to: %s!", fnNode.name().asChar() );
            }

            //Make the trigger volumes a sub-chunk of the locator...
            MPlug triggersPlug = fnNode.findPlug( sTriggers );
            MExt::ResolveConnections( &sources, &targets, triggersPlug, true, false );

            unsigned int i;
            for ( i = 0; i < sources.length(); ++i )
            {
                tlDataChunk* trigger = TriggerVolumeNode::Export( sources[ i ].node() );
                assert( trigger );

                locator->AppendSubChunk( trigger );
            }

            locator->SetNumTriggers( i );
 
            return locator;
        }     

    }

    return NULL;
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
