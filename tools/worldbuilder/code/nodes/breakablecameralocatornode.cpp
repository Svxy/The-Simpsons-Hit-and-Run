//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        BreakableCameraLocatorNode.cpp
//
// Description: Implement BreakableCameraLocatorNode
//
// History:     9/17/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
// Foundation Tech
#include <raddebug.hpp>

//========================================
// Project Includes
//========================================
#include "BreakableCameraLocatorNode.h"

#include "main/constants.h"
#include "main/worldbuilder.h"
#include "utility/glext.h"
#include "utility/mext.h"
#include "utility/nodehelper.h"
#include "utility/transformmatrix.h"

#include "resources/resource.h"

#include "../../../game/code/meta/locatortypes.h"

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
MTypeId BreakableCameraLocatorNode::id( WBConstants::TypeIDPrefix, WBConstants::NodeIDs::BreakableCameraLocator );
const char* BreakableCameraLocatorNode::stringId = "BreakableCameraLocatorNode";

const int BreakableCameraLocatorNode::ACTIVE_COLOUR = 15;
const int BreakableCameraLocatorNode::INACTIVE_COLOUR = 12;
const float BreakableCameraLocatorNode::SCALE = 1.0f * WBConstants::Scale;

char BreakableCameraLocatorNode::sNewName[MAX_NAME_LEN];

const char*  BreakableCameraLocatorNode::FOV_NAME_SHORT = "fov";
const char*  BreakableCameraLocatorNode::FOV_NAME_LONG = "fieldOfView";
MObject BreakableCameraLocatorNode::sFOV;


//******************************************************************************
//
// Callbacks
//
//******************************************************************************

BOOL CALLBACK BreakableCameraLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam )
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
                char name[BreakableCameraLocatorNode::MAX_NAME_LEN];
                GetDlgItemText( hWnd, IDC_EDIT1, name, BreakableCameraLocatorNode::MAX_NAME_LEN );

                if ( strcmp(name, "") == 0 )
                {
                    MExt::DisplayWarning("You must input a new name for the Breakable Camera Locator!");
                    return false;
                }

                MString newName( WorldBuilder::GetPrefix() );
                newName += MString( name );
                
                BreakableCameraLocatorNode::SetNewName( newName.asChar() );
                EndDialog( hWnd, 0 );  //this is how you close the window.                
                return true;
            }
            else if( LOWORD(wParam) == IDCANCEL )
            {
                BreakableCameraLocatorNode::SetNewName( "" );
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
// BreakableCameraLocatorNode::BreakableCameraLocatorNode
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
BreakableCameraLocatorNode::BreakableCameraLocatorNode()
{
}

//==============================================================================
// BreakableCameraLocatorNode::~BreakableCameraLocatorNode
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
BreakableCameraLocatorNode::~BreakableCameraLocatorNode()
{
}

//=============================================================================
// BreakableCameraLocatorNode::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
void* BreakableCameraLocatorNode::creator()
{
    return new BreakableCameraLocatorNode();
}

//=============================================================================
// BreakableCameraLocatorNode::draw
//=============================================================================
// Description: Comment
//
// Parameters:  ( M3dView& view, const MDagPath& path, M3dView::DisplayStyle displayStyle, M3dView::DisplayStatus displayStatus )
//
// Return:      void 
//
//=============================================================================
void BreakableCameraLocatorNode::draw( M3dView& view, 
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

        //Draw a star to represent the locator.
        GLExt::drawCrossHair3D( SCALE * 0.15f, 0,0,0, 5.0f );            
        GLExt::drawCamera3D( SCALE * 0.3f, 0,0.5f,0, 5.0 );

        glPopAttrib();
        view.endGL();
    }
}

//=============================================================================
// BreakableCameraLocatorNode::initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MStatus 
//
//=============================================================================
MStatus BreakableCameraLocatorNode::initialize()
{
    MStatus status;
    MFnNumericAttribute numAttr;
    sFOV = numAttr.create( FOV_NAME_LONG, FOV_NAME_SHORT, MFnNumericData::kFloat, 90.0f, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( numAttr.setMin(0.1f) );
    RETURN_STATUS_ON_FAILURE( numAttr.setMax(180.0f) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sFOV ) );

    return MStatus::kSuccess;
}

//=============================================================================
// BreakableCameraLocatorNode::postConstructor
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void BreakableCameraLocatorNode::postConstructor()
{
}

//=============================================================================
// BreakableCameraLocatorNode::Export
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& breakableCameraLocatorNode )
//
// Return:      tlDataChunk
//
//=============================================================================
tlDataChunk* BreakableCameraLocatorNode::Export( MObject& breakableCameraLocatorNode )
{
    MFnDagNode fnNode( breakableCameraLocatorNode );

    if ( fnNode.typeId() == BreakableCameraLocatorNode::id )
    {
        //Create a tlDataChunk and return it filled with the appropriate data.
        tlWBLocatorChunk* locator = new tlWBLocatorChunk;

        locator->SetName( fnNode.name().asChar() );

        locator->SetType( LocatorType::BREAKABLE_CAMERA );

        //Also get the direction.
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

        unsigned int length = (3 * 3) + 1; //3 vectors + 1 float

        unsigned long* data;

        data = new unsigned long[ length ];

        unsigned int row;
        for ( row = 0; row < 3; ++row )
        {
            tlPoint point = hmatrix.GetRow( row );

            memcpy( &data[row * 3], &point.x, sizeof(float) * 3 );
        }

        float fov = 90.0f;
        fnNode.findPlug( sFOV ).getValue( fov );
        memcpy( &data[row * 3], &fov, sizeof(float) );

        locator->SetDataElements( data, length );
        locator->SetNumDataElements( length );

        MPoint thisPosition;
        MExt::GetWorldPosition( &thisPosition, breakableCameraLocatorNode );

       //Set the values.
        tlPoint point;

        point[0] = thisPosition[0] / WBConstants::Scale;
        point[1] = thisPosition[1] / WBConstants::Scale;
        point[2] = -thisPosition[2] / WBConstants::Scale;  //Maya vs. P3D...
        locator->SetPosition( point );

        return locator;
    }    

    return NULL;
}
//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
