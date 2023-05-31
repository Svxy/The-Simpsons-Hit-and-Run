//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        StaticCameraLocatorNode.cpp
//
// Description: Implement StaticCameraLocatorNode
//
// History:     9/17/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include "main/toolhack.h"
#include <raddebug.hpp>
#include <p3d/pointcamera.hpp>

//========================================
// Project Includes
//========================================
#include "StaticCameraLocatorNode.h"

#include "main/constants.h"
#include "main/worldbuilder.h"
#include "utility/glext.h"
#include "utility/mext.h"
#include "utility/nodehelper.h"
#include "utility/transformmatrix.h"

#include "resources/resource.h"

#include "../../../game/code/meta/locatortypes.h"
#include "../../../game/code/camera/staticcam.h"

#include "nodes/triggervolumenode.h"

#include "gameengine/gameengine.h"
#include "gameengine/wbcamtarget.h"

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
MTypeId StaticCameraLocatorNode::id( WBConstants::TypeIDPrefix, WBConstants::NodeIDs::StaticCameraLocator );
const char* StaticCameraLocatorNode::stringId = "StaticCameraLocatorNode";

const int StaticCameraLocatorNode::ACTIVE_COLOUR = 15;
const int StaticCameraLocatorNode::INACTIVE_COLOUR = 12;
const float StaticCameraLocatorNode::SCALE = 1.0f * WBConstants::Scale;

char StaticCameraLocatorNode::sNewName[MAX_NAME_LEN];

const char* StaticCameraLocatorNode::TRIGGERS_NAME_SHORT   = "trigs";
const char* StaticCameraLocatorNode::TRIGGERS_NAME_LONG    = "triggers";
MObject StaticCameraLocatorNode::sTriggers;

const char*  StaticCameraLocatorNode::FOV_NAME_SHORT = "fov";
const char*  StaticCameraLocatorNode::FOV_NAME_LONG = "fieldOfView";
MObject StaticCameraLocatorNode::sFOV;

const char*  StaticCameraLocatorNode::TARGET_NAME_SHORT = "trg";
const char*  StaticCameraLocatorNode::TARGET_NAME_LONG = "target";
MObject StaticCameraLocatorNode::sTarget;

const char*  StaticCameraLocatorNode::FACING_OFFSET_NAME_SHORT = "facoff";
const char*  StaticCameraLocatorNode::FACING_OFFSET_NAME_LONG = "facingOffset";
MObject StaticCameraLocatorNode::sFacingOffset;

const char* StaticCameraLocatorNode::TRACKING_NAME_SHORT = "trkng";
const char* StaticCameraLocatorNode::TRACKING_NAME_LONG = "tracking";
MObject StaticCameraLocatorNode::sTracking;

const char*  StaticCameraLocatorNode::TARGET_LAG_NAME_SHORT = "trglg";
const char*  StaticCameraLocatorNode::TARGET_LAG_NAME_LONG = "targetLag";
MObject StaticCameraLocatorNode::sTargetLag;

const char*  StaticCameraLocatorNode::ACTIVE_NAME_SHORT = "act";
const char*  StaticCameraLocatorNode::ACTIVE_NAME_LONG = "active";
MObject StaticCameraLocatorNode::sActive;

const char* StaticCameraLocatorNode::TRANSITION_TO_RATE_NAME_LONG = "transitionTo";
const char* StaticCameraLocatorNode::TRANSITION_TO_RATE_NAME_SHORT = "trnto";
MObject StaticCameraLocatorNode::sTransitionToRate;

const char* StaticCameraLocatorNode::ONESHOT_NAME_LONG = "oneShot";
const char* StaticCameraLocatorNode::ONESHOT_NAME_SHORT = "oShot";
MObject StaticCameraLocatorNode::sOneShot;

const char* StaticCameraLocatorNode::NOFOV_NAME_LONG = "noFOV";
const char* StaticCameraLocatorNode::NOFOV_NAME_SHORT = "nf";
MObject StaticCameraLocatorNode::sNoFOV;

const char* StaticCameraLocatorNode::CUTALL_NAME_LONG = "cutInOut";
const char* StaticCameraLocatorNode::CUTALL_NAME_SHORT = "cin";
MObject StaticCameraLocatorNode::sCutAll;

const char* StaticCameraLocatorNode::CAR_ONLY_NAME_LONG = "carOnly";
const char* StaticCameraLocatorNode::CAR_ONLY_NAME_SHORT = "co";
MObject StaticCameraLocatorNode::sCarOnly;

const char* StaticCameraLocatorNode::ON_FOOT_ONLY_NAME_LONG = "onFootOnly";
const char* StaticCameraLocatorNode::ON_FOOT_ONLY_NAME_SHORT = "ofo";
MObject StaticCameraLocatorNode::sOnFootOnly;

//******************************************************************************
//
// Callbacks
//
//******************************************************************************

BOOL CALLBACK StaticCameraLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam )
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
                char name[StaticCameraLocatorNode::MAX_NAME_LEN];
                GetDlgItemText( hWnd, IDC_EDIT1, name, StaticCameraLocatorNode::MAX_NAME_LEN );

                if ( strcmp(name, "") == 0 )
                {
                    MExt::DisplayWarning("You must input a new name for the Static Camera Locator!");
                    return false;
                }

                MString newName( WorldBuilder::GetPrefix() );
                newName += MString( name );
                
                StaticCameraLocatorNode::SetNewName( newName.asChar() );
                EndDialog( hWnd, 0 );  //this is how you close the window.                
                return true;
            }
            else if( LOWORD(wParam) == IDCANCEL )
            {
                StaticCameraLocatorNode::SetNewName( "" );
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
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// BreakableCameraLocatorNode::StaticCameraLocatorNode
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
StaticCameraLocatorNode::StaticCameraLocatorNode()
{
    mStaticCam = new StaticCam();
    mStaticCam->AddRef();
    mStaticCam->SetPlayerID( 0 );

    mStaticCam->SetCamera( new tPointCamera() );

    mCamTarget = new WBCamTarget();
}

//==============================================================================
// StaticCameraLocatorNode::~StaticCameraLocatorNode
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
StaticCameraLocatorNode::~StaticCameraLocatorNode()
{
    mStaticCam->Release();
    mStaticCam = NULL;

    if ( mCamTarget )
    {
        delete mCamTarget;
    }
}

//=============================================================================
// StaticCameraLocatorNode::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
void* StaticCameraLocatorNode::creator()
{
    return new StaticCameraLocatorNode();
}

//=============================================================================
// StaticCameraLocatorNode::draw
//=============================================================================
// Description: Comment
//
// Parameters:  ( M3dView& view, const MDagPath& path, M3dView::DisplayStyle displayStyle, M3dView::DisplayStatus displayStatus )
//
// Return:      void 
//
//=============================================================================
void StaticCameraLocatorNode::draw( M3dView& view, 
                             const MDagPath& path, 
                             M3dView::DisplayStyle displayStyle, 
                             M3dView::DisplayStatus displayStatus )
{
    if ( WorldBuilder::GetDisplayLevel() & WorldBuilder::DIRECTIONAL_LOCATORS )
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

        GameEngine::GetInstance()->UpdateStaticCam( thisMObject() );

        //Draw a star to represent the locator.
        GLExt::drawCrossHair3D( SCALE, 0,0,0, 5.0f );            
        GLExt::drawCamera3D( SCALE, 0,0,0, 5.0 );

        glPopAttrib();
        view.endGL();
    }
}

//=============================================================================
// StaticCameraLocatorNode::initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MStatus 
//
//=============================================================================
MStatus StaticCameraLocatorNode::initialize()
{
    MStatus status;
    MFnMessageAttribute msgAttr;
    sTriggers = msgAttr.create( TRIGGERS_NAME_LONG, TRIGGERS_NAME_SHORT, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( msgAttr.setReadable( false ) );
    RETURN_STATUS_ON_FAILURE( msgAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( msgAttr.setArray( true ) );
    RETURN_STATUS_ON_FAILURE( msgAttr.setIndexMatters( false ) );

    RETURN_STATUS_ON_FAILURE( addAttribute( sTriggers ) );

    MFnNumericAttribute numericAttr;
    sFOV = numericAttr.create( FOV_NAME_LONG, FOV_NAME_SHORT, MFnNumericData::kFloat, 90.0f, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setMin(0.1f) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setMax(180.0f) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sFOV ) );

    MFnTypedAttribute typedAttr;
    sTarget = typedAttr.create( TARGET_NAME_LONG, TARGET_NAME_SHORT, MFnData::kString, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( typedAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( typedAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sTarget ) );

    sFacingOffset = numericAttr.create( FACING_OFFSET_NAME_LONG, FACING_OFFSET_NAME_SHORT, MFnNumericData::k3Float, 0.0f, &status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sFacingOffset ) );

    sTracking = numericAttr.create( TRACKING_NAME_LONG, TRACKING_NAME_SHORT, MFnNumericData::kBoolean, false, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sTracking ) );

    sTargetLag = numericAttr.create( TARGET_LAG_NAME_LONG, TARGET_LAG_NAME_SHORT, MFnNumericData::kFloat, 0.04f, &status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setMin(0.0f) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setMax(1.0f) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sTargetLag ) );

    sActive = numericAttr.create( ACTIVE_NAME_LONG, ACTIVE_NAME_SHORT, MFnNumericData::kBoolean, false, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sActive ) );

    sTransitionToRate = numericAttr.create( TRANSITION_TO_RATE_NAME_LONG, TRANSITION_TO_RATE_NAME_SHORT, MFnNumericData::kFloat, 0.04f, &status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setMin(0.0f) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setMax(1.0f) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sTransitionToRate ) );

    sOneShot = numericAttr.create( ONESHOT_NAME_LONG, ONESHOT_NAME_SHORT, MFnNumericData::kBoolean, false, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sOneShot ) );

    sNoFOV = numericAttr.create( NOFOV_NAME_LONG, NOFOV_NAME_SHORT, MFnNumericData::kBoolean, false, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sNoFOV ) );

    sCutAll = numericAttr.create( CUTALL_NAME_LONG, CUTALL_NAME_SHORT, MFnNumericData::kBoolean, false, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sCutAll ) );

    sCarOnly = numericAttr.create( CAR_ONLY_NAME_LONG, CAR_ONLY_NAME_SHORT, MFnNumericData::kBoolean, false, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sCarOnly ) );

    sOnFootOnly = numericAttr.create( ON_FOOT_ONLY_NAME_LONG, ON_FOOT_ONLY_NAME_SHORT, MFnNumericData::kBoolean, false, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sOnFootOnly ) );

    return MStatus::kSuccess;
}

//=============================================================================
// StaticCameraLocatorNode::postConstructor
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void StaticCameraLocatorNode::postConstructor()
{
}

//=============================================================================
// StaticCameraLocatorNode::Export
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& staticCameraLocatorNode )
//
// Return:      tlDataChunk
//
//=============================================================================
tlDataChunk* StaticCameraLocatorNode::Export( MObject& staticCameraLocatorNode )
{
    MFnDagNode fnNode( staticCameraLocatorNode );

    if ( fnNode.typeId() == StaticCameraLocatorNode::id )
    {
        //Create a tlDataChunk and return it filled with the appropriate data.
        tlWBLocatorChunk* locator = new tlWBLocatorChunk;

        locator->SetName( fnNode.name().asChar() );

        locator->SetType( LocatorType::STATIC_CAMERA );

        //Set the position
        MPoint thisPosition;
        MExt::GetWorldPosition( &thisPosition, staticCameraLocatorNode );

       //Set the values.
        tlPoint position;

        position[0] = thisPosition[0] / WBConstants::Scale;
        position[1] = thisPosition[1] / WBConstants::Scale;
        position[2] = -thisPosition[2] / WBConstants::Scale;  //Maya vs. P3D...
        locator->SetPosition( position );

        //Also get the direction.
        MObject transform;
        transform = fnNode.parent( 0 );
        MFnTransform fnTransform( transform );

        MDagPath dagPath;
        MExt::FindDagNodeByName( &dagPath, fnTransform.name() );
        TransformMatrix tm( dagPath );

        unsigned int length = (1 * 3) + 2 + 1 + 1 + 1 + 1 + 1; //1 vector + 2 floats + 1 bool + transition rate (offset + fov & lag + tracking + trate) + one shot + CUT_ALL + CarOnly/OnFootOnly

        unsigned long* data;

        data = new unsigned long[ length ];

        rmt::Vector offset;

        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;

        fnNode.findPlug( StaticCameraLocatorNode::sFacingOffset ).child(0).getValue( x );
        fnNode.findPlug( StaticCameraLocatorNode::sFacingOffset ).child(1).getValue( y );
        fnNode.findPlug( StaticCameraLocatorNode::sFacingOffset ).child(2).getValue( z );

        bool tracking = false;
        fnNode.findPlug( StaticCameraLocatorNode::sTracking ).getValue( tracking );

        if ( tracking )
        {
            offset = rmt::Vector( x, y, z );
        }
        else
        {
            //Figure out the transformation on the locator node and apply it to the offset.
            MObject transform;
            transform = fnNode.parent( 0 );
            MFnTransform fnTransform( transform );

            MDagPath dagPath;
            MExt::FindDagNodeByName( &dagPath, fnTransform.name() );
            TransformMatrix tm( dagPath );

            tlMatrix hmatrix;
            tm.GetHierarchyMatrixLHS( hmatrix );
            //Make this p3d friendly...
            hmatrix.element[3][0];
            hmatrix.element[3][1];
            hmatrix.element[3][2];

            tlPoint point( 0.0f, 0.0f, 10.0f ); 
            point = VectorTransform( hmatrix, point );

            offset = position;
            offset.Add( point );
        }
        
        //Copy the offset to the locator
        memcpy( &data[0], &offset.x, 3 * sizeof(float) );

        float fov = 90.0f;
        fnNode.findPlug( sFOV ).getValue( fov );
        memcpy( &data[3], &fov, sizeof(float) );

        float lag = 0.04f;
        fnNode.findPlug( sTargetLag ).getValue( lag );
        memcpy( &data[4], &lag, sizeof(float) );

        //Also tracking 
        if ( tracking )
        {
            data[5] = 1;
        }
        else
        {
            data[5] = 0;
        }

        float tRate = 0.04f;
        fnNode.findPlug( sTransitionToRate ).getValue( tRate );
        memcpy( &data[6], &tRate, sizeof(float) );

        bool oneShot = false;
        fnNode.findPlug( sOneShot ).getValue( oneShot );
        if ( oneShot )
        {
            data[7] = 1;
        }
        else
        {
            data[7] = 0;
        }

        bool noFOV = false;
        fnNode.findPlug( sNoFOV ).getValue( noFOV );
        data[7] |= ( (noFOV ? 1 : 0 ) << 1 );

        bool cutInOut = false;
        fnNode.findPlug( sCutAll ).getValue( cutInOut );
        data[8] = ( (cutInOut ? 1 : 0 ) );

        bool carOnly = false;
        fnNode.findPlug( sCarOnly ).getValue( carOnly );
        data[9] = ( (carOnly ? 1 : 0 ) );

        bool onFootOnly = false;
        fnNode.findPlug( sOnFootOnly ).getValue( onFootOnly );
        data[9] |= ( (onFootOnly ? 1 << 1 : 0 ) );

        if( carOnly && onFootOnly )
        {
            MExt::DisplayError( "Static Camera: %s has onFootOnly and carOnly flags set!\n", fnNode.name().asChar() );
        }

        locator->SetDataElements( data, length );
        locator->SetNumDataElements( length );

        //Make the trigger volumes a sub-chunk of the locator...
        MPlugArray sources, targets;
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

    return NULL;
}
//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
