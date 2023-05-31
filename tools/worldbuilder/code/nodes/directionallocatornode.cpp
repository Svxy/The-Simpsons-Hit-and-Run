//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        DirectionalLocatorNode.cpp
//
// Description: Implement DirectionalLocatorNode
//
// History:     29/07/2002 + Created -- Cary Brisebois
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
#include "nodes/DirectionalLocatorNode.h"
#include "main/constants.h"
#include "main/worldbuilder.h"
#include "utility/glext.h"
#include "utility/mext.h"
#include "utility/nodehelper.h"
#include "utility/transformmatrix.h"
#include "nodes/triggervolumenode.h"

#include "resources/resource.h"

#include "../../../game/code/meta/locatortypes.h"



//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
MTypeId DirectionalLocatorNode::id( WBConstants::TypeIDPrefix, WBConstants::NodeIDs::DirectionalLocator );
const char* DirectionalLocatorNode::stringId = "DirectionalLocatorNode";

const int DirectionalLocatorNode::ACTIVE_COLOUR = 15;
const int DirectionalLocatorNode::INACTIVE_COLOUR = 12;
const float DirectionalLocatorNode::SCALE = 1.0f * WBConstants::Scale;

char DirectionalLocatorNode::sNewName[MAX_NAME_LEN];

//******************************************************************************
//
// Callbacks
//
//******************************************************************************

BOOL CALLBACK DirectionalLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam )
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
                char name[DirectionalLocatorNode::MAX_NAME_LEN];
                GetDlgItemText( hWnd, IDC_EDIT1, name, DirectionalLocatorNode::MAX_NAME_LEN );

                if ( strcmp(name, "") == 0 )
                {
                    MExt::DisplayWarning("You must input a new name for the Driectional Locator!");
                    return false;
                }

                MString newName( WorldBuilder::GetPrefix() );
                newName += MString( name );
                
                DirectionalLocatorNode::SetNewName( newName.asChar() );
                EndDialog( hWnd, 0 );  //this is how you close the window.                
                return true;
            }
            else if( LOWORD(wParam) == IDCANCEL )
            {
                DirectionalLocatorNode::SetNewName( "" );
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
// DirectionalLocatorNode::DirectionalLocatorNode
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
DirectionalLocatorNode::DirectionalLocatorNode()
{
}

//==============================================================================
// DirectionalLocatorNode::~DirectionalLocatorNode
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
DirectionalLocatorNode::~DirectionalLocatorNode()
{
}

//=============================================================================
// DirectionalLocatorNode::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
void* DirectionalLocatorNode::creator()
{
    return new DirectionalLocatorNode();
}

//=============================================================================
// DirectionalLocatorNode::draw
//=============================================================================
// Description: Comment
//
// Parameters:  ( M3dView& view, const MDagPath& path, M3dView::DisplayStyle displayStyle, M3dView::DisplayStatus displayStatus )
//
// Return:      void 
//
//=============================================================================
void DirectionalLocatorNode::draw( M3dView& view, 
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
        GLExt::drawCrossHair3D( SCALE, 0,0,0, 5.0 );
        GLExt::drawArrow( MPoint( 0, 0, 0 ), MPoint( 0, 0, -1 * WBConstants::Scale ), 5.0 );

        glPopAttrib();
        view.endGL();
    }
}

//=============================================================================
// DirectionalLocatorNode::initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MStatus 
//
//=============================================================================
MStatus DirectionalLocatorNode::initialize()
{
    return MStatus::kSuccess;
}

//=============================================================================
// DirectionalLocatorNode::postConstructor
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void DirectionalLocatorNode::postConstructor()
{
}

//=============================================================================
// DirectionalLocatorNode::Export
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& directionalLocatorNode )
//
// Return:      tlDataChunk
//
//=============================================================================
tlDataChunk* DirectionalLocatorNode::Export( MObject& directionalLocatorNode )
{
    MFnDagNode fnNode( directionalLocatorNode );

    if ( fnNode.typeId() == DirectionalLocatorNode::id )
    {
        //Create a tlDataChunk and return it filled with the appropriate data.
        tlWBLocatorChunk* locator = new tlWBLocatorChunk;

        locator->SetName( fnNode.name().asChar() );

        locator->SetType( LocatorType::DIRECTIONAL );

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

        unsigned int length = 3 * 3; //3 vectors

        unsigned long* data;

        data = new unsigned long[ length ];

        unsigned int row;
        for ( row = 0; row < 3; ++row )
        {
            tlPoint point = hmatrix.GetRow( row );

            memcpy( &data[row * 3], &point.x, sizeof(float) * 3 );
        }

        locator->SetDataElements( data, length );
        locator->SetNumDataElements( length );

        MPoint thisPosition;
        MExt::GetWorldPosition( &thisPosition, directionalLocatorNode );

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
