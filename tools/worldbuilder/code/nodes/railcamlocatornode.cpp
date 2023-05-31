//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        RailCamLocatorNode.cpp
//
// Description: Implement RailCamLocatorNode
//
// History:     18/07/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
#include "main/toolhack.h"
#include <toollib.hpp>
#include <p3d/pointcamera.hpp>

//========================================
// Project Includes
//========================================
#include "RailCamLocatorNode.h"
#include "main/constants.h"
#include "main/worldbuilder.h"
#include "utility/glext.h"
#include "utility/mext.h"
#include "nodes/triggervolumenode.h"
#include "utility/nodehelper.h"

#include "gameengine/gameengine.h"
#include "gameengine/wbcamtarget.h"

#include "resources/resource.h"

#include "../../../game/code/camera/railcam.h"

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
MTypeId RailCamLocatorNode::id( WBConstants::TypeIDPrefix, WBConstants::NodeIDs::RailCamLocator );
const char* RailCamLocatorNode::stringId = "RailCamLocatorNode";

const int RailCamLocatorNode::ACTIVE_COLOUR = 15;
const int RailCamLocatorNode::INACTIVE_COLOUR = 12;
const float RailCamLocatorNode::SCALE = 1.0f * WBConstants::Scale;

const char* RailCamLocatorNode::RAIL_NAME_SHORT = "rail";
const char* RailCamLocatorNode::RAIL_NAME_LONG = "camRail";
MObject RailCamLocatorNode::sRail;

const char*  RailCamLocatorNode::BEHAVIOUR_NAME_SHORT = "behav";
const char*  RailCamLocatorNode::BEHAVIOUR_NAME_LONG = "behaviour";
MObject RailCamLocatorNode::sBehaviour;

const char*  RailCamLocatorNode::MIN_RADIUS_NAME_SHORT = "minR";
const char*  RailCamLocatorNode::MIN_RADIUS_NAME_LONG = "minRadius";
MObject RailCamLocatorNode::sMinRadius;

const char*  RailCamLocatorNode::MAX_RADIUS_NAME_SHORT = "maxR";
const char*  RailCamLocatorNode::MAX_RADIUS_NAME_LONG = "maxRadius";
MObject RailCamLocatorNode::sMaxRadius;

const char*  RailCamLocatorNode::TRACK_RAIL_NAME_SHORT = "trkRl";
const char*  RailCamLocatorNode::TRACK_RAIL_NAME_LONG = "trackRail";
MObject RailCamLocatorNode::sTrackRail;

const char*  RailCamLocatorNode::TRACK_DIST_NAME_SHORT = "tDst";
const char*  RailCamLocatorNode::TRACK_DIST_NAME_LONG  = "trackDist";
MObject RailCamLocatorNode::sTrackDist;

const char*  RailCamLocatorNode::REVERSE_SENSE_NAME_SHORT ="rvsns";
const char*  RailCamLocatorNode::REVERSE_SENSE_NAME_LONG = "reverseSense";
MObject RailCamLocatorNode::sReverseSense;


const char*  RailCamLocatorNode::FOV_NAME_SHORT = "fov";
const char*  RailCamLocatorNode::FOV_NAME_LONG = "fieldOfView";
MObject RailCamLocatorNode::sFOV;

const char*  RailCamLocatorNode::FACING_OFFSET_NAME_SHORT = "facoff";
const char*  RailCamLocatorNode::FACING_OFFSET_NAME_LONG = "facingOffset";
MObject RailCamLocatorNode::sFacingOffset;

const char*  RailCamLocatorNode::AXIS_PLAY_NAME_SHORT = "axsply";
const char*  RailCamLocatorNode::AXIS_PLAY_NAME_LONG = "axisPlay";
MObject RailCamLocatorNode::sAxisPlay;

const char*  RailCamLocatorNode::ACTIVE_NAME_SHORT = "act";
const char*  RailCamLocatorNode::ACTIVE_NAME_LONG = "active";
MObject RailCamLocatorNode::sActive;

const char*  RailCamLocatorNode::TARGET_NAME_SHORT = "trg";
const char*  RailCamLocatorNode::TARGET_NAME_LONG = "target";
MObject RailCamLocatorNode::sTarget;

const char*  RailCamLocatorNode::POS_LAG_NAME_SHORT = "plg";
const char*  RailCamLocatorNode::POS_LAG_NAME_LONG = "positionLag";
MObject RailCamLocatorNode::sPositionLag;

const char*  RailCamLocatorNode::TARGET_LAG_NAME_SHORT = "trglg";
const char*  RailCamLocatorNode::TARGET_LAG_NAME_LONG = "targetLag";
MObject RailCamLocatorNode::sTargetLag;

const char* RailCamLocatorNode::TRANSITION_TO_RATE_NAME_LONG = "transitionTo";
const char* RailCamLocatorNode::TRANSITION_TO_RATE_NAME_SHORT = "trnto";
MObject RailCamLocatorNode::sTransitionToRate;

const char* RailCamLocatorNode::NOFOV_NAME_LONG = "noFOV";
const char* RailCamLocatorNode::NOFOV_NAME_SHORT = "nf";
MObject RailCamLocatorNode::sNoFOV;

const char* RailCamLocatorNode::CAR_ONLY_NAME_LONG = "carOnly";
const char* RailCamLocatorNode::CAR_ONLY_NAME_SHORT = "co";
MObject RailCamLocatorNode::sCarOnly;

const char* RailCamLocatorNode::ON_FOOT_ONLY_NAME_LONG = "onFootOnly";
const char* RailCamLocatorNode::ON_FOOT_ONLY_NAME_SHORT = "ofo";
MObject RailCamLocatorNode::sOnFootOnly;

const char* RailCamLocatorNode::CUTALL_NAME_LONG = "cutInOut";
const char* RailCamLocatorNode::CUTALL_NAME_SHORT = "cin";
MObject RailCamLocatorNode::sCutAll;

const char* RailCamLocatorNode::RESET_NAME_LONG = "reset";
const char* RailCamLocatorNode::RESET_NAME_SHORT = "rst";
MObject RailCamLocatorNode::sReset;

char RailCamLocatorNode::sNewName[MAX_NAME_LEN];

//******************************************************************************
//
// Callbacks
//
//******************************************************************************

BOOL CALLBACK RailCamLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam )
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
                char name[RailCamLocatorNode::MAX_NAME_LEN];
                GetDlgItemText( hWnd, IDC_EDIT1, name, RailCamLocatorNode::MAX_NAME_LEN );

                if ( strcmp(name, "") == 0 )
                {
                    MExt::DisplayWarning("You must input a new name for the RailCam Locator!");
                    return false;
                }

                MString newName( WorldBuilder::GetPrefix() );
                newName += MString( name );
                
                RailCamLocatorNode::SetNewName( newName.asChar() );
                EndDialog( hWnd, 0 );  //this is how you close the window.                
                return true;
            }
            else if( LOWORD(wParam) == IDCANCEL )
            {
                RailCamLocatorNode::SetNewName( "" );
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

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// RailCamLocatorNode::RailCamLocatorNode
//=============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
RailCamLocatorNode::RailCamLocatorNode()
{
    mRailCam = new RailCam();
    mRailCam->AddRef();
    mRailCam->SetPlayerID( 0 );

    mRailCam->SetCamera( new tPointCamera() );

    mCamTarget = new WBCamTarget();
}

//=============================================================================
// RailCamLocatorNode::~RailCamLocatorNode
//=============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
RailCamLocatorNode::~RailCamLocatorNode()
{
    mRailCam->Release();
    mRailCam = NULL;

    if ( mCamTarget )
    {
        delete mCamTarget;
    }
}

//=============================================================================
// RailCamLocatorNode::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
void* RailCamLocatorNode::creator()
{
    return new RailCamLocatorNode();
}

//=============================================================================
// RailCamLocatorNode::draw
//=============================================================================
// Description: Comment
//
// Parameters:  ( M3dView& view, const MDagPath& path, M3dView::DisplayStyle displayStyle, M3dView::DisplayStatus displayStatus )
//
// Return:      void 
//
//=============================================================================
void RailCamLocatorNode::draw( M3dView& view, 
                             const MDagPath& path, 
                             M3dView::DisplayStyle displayStyle, 
                             M3dView::DisplayStatus displayStatus )
{
    if ( WorldBuilder::GetDisplayLevel() & WorldBuilder::RAILCAM_LOCATORS )
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

        GameEngine::GetInstance()->UpdateRailCam( thisMObject() );

        //Draw a star to represent the locator.
        GLExt::drawCrossHair3D( SCALE, 0,0,0, 5.0 );

        glPopAttrib();
        view.endGL();
    }
}

//=============================================================================
// RailCamLocatorNode::initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MStatus 
//
//=============================================================================
MStatus RailCamLocatorNode::initialize()
{
    MFnMessageAttribute msgAttr;
    MStatus status;

    sRail = msgAttr.create( RAIL_NAME_LONG, RAIL_NAME_SHORT, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( msgAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( msgAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sRail ) );

    MFnEnumAttribute enumAttr;
    sBehaviour = enumAttr.create( BEHAVIOUR_NAME_LONG, BEHAVIOUR_NAME_SHORT, RailCam::PROJECTION, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( enumAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( enumAttr.setWritable( true ) );

    unsigned int i;
    for ( i = 1; i < RailCam::NUM_BEHAVIOUR; ++i )  
    {
        RETURN_STATUS_ON_FAILURE( enumAttr.addField( MString( RailCam::BehaviourNames[i-1] ), i ) );
    }
    RETURN_STATUS_ON_FAILURE( addAttribute( sBehaviour ) );

    MFnNumericAttribute numericAttr;
    sMinRadius = numericAttr.create( MIN_RADIUS_NAME_LONG, MIN_RADIUS_NAME_SHORT, MFnNumericData::kFloat, 1.0f, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setMin(0.0f) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setMax(30.0f) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sMinRadius ) );

    sMaxRadius = numericAttr.create( MAX_RADIUS_NAME_LONG, MAX_RADIUS_NAME_SHORT, MFnNumericData::kFloat, 2.0f, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setMin(0.0f) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setMax(30.0f) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sMaxRadius ) );

    sTrackRail = numericAttr.create( TRACK_RAIL_NAME_LONG, TRACK_RAIL_NAME_SHORT, MFnNumericData::kBoolean, false, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sTrackRail ) );

    sTrackDist = numericAttr.create( TRACK_DIST_NAME_LONG, TRACK_DIST_NAME_SHORT, MFnNumericData::kFloat, 0.0f, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setMin(-5.0f) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setMax(5.0f) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setDefault( 0.0f ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sTrackDist ) );
    
    sReverseSense = numericAttr.create( REVERSE_SENSE_NAME_LONG, REVERSE_SENSE_NAME_SHORT, MFnNumericData::kBoolean, false, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sReverseSense ) );

    sFOV = numericAttr.create( FOV_NAME_LONG, FOV_NAME_SHORT, MFnNumericData::kInt, 90, &status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setMin(0.0f) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setMax(180.0f) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sFOV ) );

    sFacingOffset = numericAttr.create( FACING_OFFSET_NAME_LONG, FACING_OFFSET_NAME_SHORT, MFnNumericData::k3Float, 0.0f, &status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sFacingOffset ) );
    
    sAxisPlay = numericAttr.create( AXIS_PLAY_NAME_LONG, AXIS_PLAY_NAME_SHORT, MFnNumericData::k3Float, 0.0f, &status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sAxisPlay ) );

    sActive = numericAttr.create( ACTIVE_NAME_LONG, ACTIVE_NAME_SHORT, MFnNumericData::kBoolean, false, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sActive ) );

    MFnTypedAttribute typedAttr;
    sTarget = typedAttr.create( TARGET_NAME_LONG, TARGET_NAME_SHORT, MFnData::kString, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( typedAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( typedAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sTarget ) );

    sPositionLag = numericAttr.create( POS_LAG_NAME_LONG, POS_LAG_NAME_SHORT, MFnNumericData::kFloat, 0.04f, &status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setMin(0.0f) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setMax(1.0f) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sPositionLag ) );

    sTargetLag = numericAttr.create( TARGET_LAG_NAME_LONG, TARGET_LAG_NAME_SHORT, MFnNumericData::kFloat, 0.04f, &status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setMin(0.0f) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setMax(1.0f) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sTargetLag ) );

    sTransitionToRate = numericAttr.create( TRANSITION_TO_RATE_NAME_LONG, TRANSITION_TO_RATE_NAME_SHORT, MFnNumericData::kFloat, 0.04f, &status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setMin(0.0f) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setMax(1.0f) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sTransitionToRate ) );

    sNoFOV = numericAttr.create( NOFOV_NAME_LONG, NOFOV_NAME_SHORT, MFnNumericData::kBoolean, false, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sNoFOV ) );

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

    sCutAll = numericAttr.create( CUTALL_NAME_LONG, CUTALL_NAME_SHORT, MFnNumericData::kBoolean, false, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sCutAll ) );

    sReset = numericAttr.create( RESET_NAME_LONG, RESET_NAME_SHORT, MFnNumericData::kBoolean, false, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sReset ) );

    return MStatus::kSuccess;
}

//=============================================================================
// RailCamLocatorNode::postConstructor
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void RailCamLocatorNode::postConstructor()
{
}

//=============================================================================
// RailCamLocatorNode::Export
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& railCamLocatorNode )
//
// Return:      tlDataChunk
//
//=============================================================================
tlDataChunk* RailCamLocatorNode::Export( MObject& railCamLocatorNode )
{
    MFnDagNode fnNode( railCamLocatorNode );

    if ( fnNode.typeId() == RailCamLocatorNode::id )
    {
        tlWBRailCamChunk* railChunk = new tlWBRailCamChunk();
        assert( railChunk );

        railChunk->SetName( fnNode.name().asChar() );

        int behaviour;
        fnNode.findPlug( sBehaviour ).getValue( behaviour );
        railChunk->SetBehaviour( (RailCam::Behaviour)(behaviour) );

        float minRad;
        fnNode.findPlug( sMinRadius ).getValue( minRad );
        railChunk->SetMinRadius( minRad );

        float maxRad;
        fnNode.findPlug( sMaxRadius ).getValue( maxRad );
        railChunk->SetMaxRadius( maxRad );

        bool trackRail;
        fnNode.findPlug( sTrackRail ).getValue( trackRail );
        railChunk->SetTrackRail( trackRail ? 1 : 0 );

        float trackDist;
        fnNode.findPlug( sTrackDist ).getValue( trackDist );
        railChunk->SetTrackDist( trackDist );

        bool reverseSense;
        fnNode.findPlug( sReverseSense ).getValue( reverseSense );
        railChunk->SetReverseSense( reverseSense ? 1 : 0 );

        float fov;
        fnNode.findPlug( sFOV ).getValue( fov );
        railChunk->SetFOV( fov );

        float x, y, z;
        fnNode.findPlug( sFacingOffset ).child( 0 ).getValue( x );
        fnNode.findPlug( sFacingOffset ).child( 1 ).getValue( y );
        fnNode.findPlug( sFacingOffset ).child( 2 ).getValue( z );
        railChunk->SetTargetOffset( tlPoint( x, y, z ) );

        //  THIS IS A HACK.  Since we're not going to support axis play.  We're 
        // going to hide the transition rate in the data.  Sorry.
//        fnNode.findPlug( sAxisPlay ).child( 0 ).getValue( x );
//        fnNode.findPlug( sAxisPlay ).child( 1 ).getValue( y );
//        fnNode.findPlug( sAxisPlay ).child( 2 ).getValue( z );
        fnNode.findPlug( sTransitionToRate ).getValue( x ); //transition rate. 

        //Now we're hiding the flags in the y..  THIS SUCKS!
        bool noF = false;
        fnNode.findPlug( sNoFOV ).getValue( noF );
        y = (float)( noF ? 1 : 0 );

        //This just GETS WORSE!
        int values = 0;

        bool carOnly = false;
        fnNode.findPlug( sCarOnly ).getValue( carOnly );
        values |= ( carOnly ? 0x00000001 : 0 );

        bool cut = false;
        fnNode.findPlug( sCutAll ).getValue( cut );
        values |= ( cut ? 0x00000002 : 0 );
 
        bool reset = false;
        fnNode.findPlug( sReset ).getValue( reset );
        values |= ( reset ? 0x00000004 : 0 );

        bool onFootOnly = false;
        fnNode.findPlug( sOnFootOnly ).getValue( onFootOnly );
        values |= ( onFootOnly ? 0x00000008 : 0 );

        if( carOnly && onFootOnly )
        {
            MExt::DisplayError( "Rail Camera: %s has onFootOnly and carOnly flags set!\n", fnNode.name().asChar() );
        }

        z = (float)values;
        railChunk->SetAxisPlay( tlPoint( x, y, z ) );

        float posLag;
        fnNode.findPlug( sPositionLag ).getValue( posLag );
        railChunk->SetPositionLag( posLag );

        float targLag;
        fnNode.findPlug( sTargetLag ).getValue( targLag );
        railChunk->SetTargetLag( targLag );

        return railChunk;
    }    

    return NULL;
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************
