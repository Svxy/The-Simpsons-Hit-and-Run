//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        TriggerVolumeNode.cpp
//
// Description: Implement TriggerVolumeNode
//
// History:     23/05/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <windows.h>
#include <GL/glu.h>
#include "main/toolhack.h"
#include <toollib.hpp>

//========================================
// Project Includes
//========================================
#include "nodes/TriggerVolumeNode.h"
#include "main/constants.h"
#include "main/worldbuilder.h"
#include "utility/glext.h"
#include "utility/mext.h"
#include "utility/nodehelper.h"

#include "utility/transformmatrix.h"

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
MTypeId TriggerVolumeNode::id( WBConstants::TypeIDPrefix, WBConstants::NodeIDs::TriggerVolume );
const char* TriggerVolumeNode::stringId = "TriggerVolumeNode";

const int TriggerVolumeNode::ACTIVE_COLOUR = 15;
const int TriggerVolumeNode::INACTIVE_COLOUR = 22;
const float TriggerVolumeNode::SCALE = 1.0f * WBConstants::Scale;
const float TriggerVolumeNode::LINE_WIDTH = 5.0f;

const char* TriggerVolumeNode::LOCATOR_NAME_SHORT = "l";
const char*  TriggerVolumeNode::LOCATOR_NAME_LONG = "locator";
MObject TriggerVolumeNode::sLocator;

const char* TriggerVolumeNode::TYPE_NAME_SHORT = "typ";
const char* TriggerVolumeNode::TYPE_NAME_LONG = "type";
MObject TriggerVolumeNode::sType;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// TriggerVolumeNode::TriggerVolumeNode
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
TriggerVolumeNode::TriggerVolumeNode() :
    mType( SPHERE )
{
}

//==============================================================================
// TriggerVolumeNode::~TriggerVolumeNode
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
TriggerVolumeNode::~TriggerVolumeNode()
{
}

//=============================================================================
// TriggerVolumeNode::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
void* TriggerVolumeNode::creator()
{
    return new TriggerVolumeNode();
}

//=============================================================================
// TriggerVolumeNode::draw
//=============================================================================
// Description: Comment
//
// Parameters:  ( M3dView& view, const MDagPath& path, M3dView::DisplayStyle displayStyle, M3dView::DisplayStatus displayStatus )
//
// Return:      void 
//
//=============================================================================
void TriggerVolumeNode::draw( M3dView& view, const MDagPath& path, M3dView::DisplayStyle displayStyle, M3dView::DisplayStatus displayStatus )
{
    if ( WorldBuilder::GetDisplayLevel() & WorldBuilder::TRIGGER_VOLUMES )
    {
        view.beginGL();
        glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT );

        //When we are in render mode, we draw the lines between the nodes.
        //If this was in GL_SELECTION_MODE, we would not draw the lines, so they won't interfere
        //with selection.
        GLint value;
        glGetIntegerv( GL_RENDER_MODE, &value );

        //Draw things here we don't want selectable.
        if ( (value == GL_RENDER) )
        {
    //        view.setDrawColor( INACTIVE_COLOUR, M3dView::kDormantColors );
    //        GLExt::drawLine( localOrigin, localPosNN );
        }

        if ( displayStatus == M3dView::kDormant ) 
        {
            int colour = NodeHelper::OverrideNodeColour( thisMObject(), INACTIVE_COLOUR );

	        view.setDrawColor( colour, M3dView::kDormantColors );
        }  
        else
        {
	        view.setDrawColor( ACTIVE_COLOUR, M3dView::kActiveColors );
        }


        //Determine the type of trigger volume.
        MFnDagNode fnDag( thisMObject() );

        MStatus status;
        MPlug typePlug = fnDag.findPlug( sType, &status );
        assert( status );
        
        int type;
        typePlug.getValue( type );

        //Test to see if there was a change.
        if( (Type)type != mType )
        {
            //Hmm, changing trigger volume types...

            if ( (Type)type == SPHERE )
            {
                //Becoming a sphere.
                MObject transform;

                transform = fnDag.parent( 0 );
                assert( !transform.isNull() );

                MFnDagNode fnTransDag( transform );

                //Test the scaling factors.  If one is larger than
                //another, set them all to the same value.
                MPlug scaleX, scaleY, scaleZ;
                double x, y, z;

                scaleX = fnTransDag.findPlug( MString("sx"), &status );
                assert( status );
                scaleX.getValue( x );
                assert( status );

                scaleY = fnTransDag.findPlug( MString("sy"), &status );
                assert( status );
                scaleY.getValue( y );
                assert( status );

                scaleZ = fnTransDag.findPlug( MString("sz"), &status );
                assert( status );
                scaleZ.getValue( z );
                assert( status );

                //which is larger?
                double largest = 0;
                if ( x > largest )
                {
                    largest = x;
                }
                if ( y > largest )
                {
                    largest = y;
                }
                if ( z > largest )
                {
                    largest = z;
                }

                x = largest;
                y = largest;
                z = largest;

                scaleX.setValue( x );
                scaleY.setValue( y );
                scaleZ.setValue( z );

                mType = SPHERE;
            }
            else if ( (Type)type == RECTANGLE )
            {
                //Becoming a rectangle.

                mType = RECTANGLE;
            }
            else
            {
                //Don't know what this is!
                assert( false );
            }
        }
        else
        {
            if ( mType == SPHERE )
            {
                MObject transform;

                transform = fnDag.parent( 0 );
                assert( !transform.isNull() );

                MFnDagNode fnTransDag( transform );

                //Test the scaling factors.  If one is larger than
                //another, set them all to the same value.
                MPlug scaleX, scaleY, scaleZ;
                double x;

                scaleX = fnTransDag.findPlug( MString("sx"), &status );
                assert( status );
                scaleX.getValue( x );
                assert( status );

                scaleY = fnTransDag.findPlug( MString("sy"), &status );
                assert( status );

                scaleZ = fnTransDag.findPlug( MString("sz"), &status );
                assert( status );

                scaleY.setValue( x );
                scaleZ.setValue( x );

                GLExt::drawSphere( SCALE, 0,0,0, 5.0 );

            }
            else if ( mType == RECTANGLE )
            {
                MPoint p1( SCALE, SCALE, -SCALE );
                MPoint p2( -SCALE, -SCALE, SCALE );

                GLExt::drawBox( p1, p2, 5.0 );
            }
            else
            {
                assert( false );
            }
        }

        glPopAttrib();
        view.endGL();
    }
}


//=============================================================================
// TriggerVolumeNode::initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MStatus 
//
//=============================================================================
MStatus TriggerVolumeNode::initialize()
{
    MFnMessageAttribute msgAttr;
    MStatus status;

    sLocator = msgAttr.create( LOCATOR_NAME_LONG, LOCATOR_NAME_SHORT, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( msgAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( msgAttr.setWritable( false ) );

    RETURN_STATUS_ON_FAILURE( addAttribute( sLocator ) );

    MFnEnumAttribute enumAttr;
    //Default to a SPHERE!
    sType = enumAttr.create( TYPE_NAME_LONG, TYPE_NAME_SHORT, 0, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( enumAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( enumAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( enumAttr.addField( MString( "sphere" ), SPHERE ) );
    RETURN_STATUS_ON_FAILURE( enumAttr.addField( MString( "rectangle" ), RECTANGLE ) );

    RETURN_STATUS_ON_FAILURE( addAttribute( sType ) );

    return MStatus::kSuccess;
}

//=============================================================================
// TriggerVolumeNode::postConstructor
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TriggerVolumeNode::postConstructor()
{
    //When my locator goes away, so do I.
    setExistWithoutOutConnections( false );
}

//=============================================================================
// TriggerVolumeNode::Export
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& triggerVolumeNode )
//
// Return:      tlDataChunk
//
//=============================================================================
tlDataChunk* TriggerVolumeNode::Export( MObject& triggerVolumeNode )
{
    MFnDagNode fnNode( triggerVolumeNode );

    if ( fnNode.typeId() == TriggerVolumeNode::id )
    {
        //Create a tlDataChunk and return it filled with the appropriate data.
        tlWBTriggerVolumeChunk* trigger = new tlWBTriggerVolumeChunk;

        trigger->SetName( fnNode.name().asChar() );

        int type;
        fnNode.findPlug( sType ).getValue( type );
        trigger->SetType( type );

        MObject transform;
        transform = fnNode.parent( 0 );
        MFnTransform fnTransform( transform );

        MDagPath dagPath;
        MExt::FindDagNodeByName( &dagPath, fnTransform.name() );
        TransformMatrix tm( dagPath );

        tlMatrix hmatrix;
        tm.GetHierarchyMatrixLHS( hmatrix );
        //Make this p3d friendly...
        hmatrix.element[3][0] /= WBConstants::Scale;
        hmatrix.element[3][1] /= WBConstants::Scale;
        hmatrix.element[3][2] /= WBConstants::Scale;

        //This is the translation / rotation matrix
        trigger->SetMatrix( hmatrix );

        //This is the scale.
        double scaleX, scaleY, scaleZ;
        fnTransform.findPlug( MString( "sx" ) ).getValue( scaleX );
        fnTransform.findPlug( MString( "sy" ) ).getValue( scaleY );
        fnTransform.findPlug( MString( "sz" ) ).getValue( scaleZ );

        trigger->SetScale( tlPoint( scaleX, scaleY, scaleZ ) );
        return trigger;
    }
    
    return NULL;
}

//=============================================================================
// TriggerVolumeNode::GetScaleAndMatrix
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& triggerVolumeNode, tlMatrix& mat, tlPoint& point)
//
// Return:      void 
//
//=============================================================================
void TriggerVolumeNode::GetScaleAndMatrix( MObject& triggerVolumeNode, tlMatrix& mat, tlPoint& point)
{
    MFnDagNode fnNode( triggerVolumeNode );

    if ( fnNode.typeId() == TriggerVolumeNode::id )
    {
        int type;
        fnNode.findPlug( sType ).getValue( type );

        MObject transform;
        transform = fnNode.parent( 0 );
        MFnTransform fnTransform( transform );

        MDagPath dagPath;
        MExt::FindDagNodeByName( &dagPath, fnTransform.name() );
        TransformMatrix tm( dagPath );

        tlMatrix hmatrix;
        tm.GetHierarchyMatrixLHS( mat );
        //Make this p3d friendly...
        mat.element[3][0] /= WBConstants::Scale;
        mat.element[3][1] /= WBConstants::Scale;
        mat.element[3][2] /= WBConstants::Scale;

        //This is the scale.
        double scaleX, scaleY, scaleZ;
        fnTransform.findPlug( MString( "sx" ) ).getValue( scaleX );
        fnTransform.findPlug( MString( "sy" ) ).getValue( scaleY );
        fnTransform.findPlug( MString( "sz" ) ).getValue( scaleZ );

        point = tlPoint( scaleX, scaleY, scaleZ );
    }
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
