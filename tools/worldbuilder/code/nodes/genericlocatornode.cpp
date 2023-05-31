//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        GenericLocatorNode.cpp
//
// Description: Implement GenericLocatorNode
//
// History:     27/05/2002 + Created -- Cary Brisebois
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
#include "nodes/GenericLocatorNode.h"
#include "main/constants.h"
#include "main/worldbuilder.h"
#include "utility/glext.h"
#include "utility/mext.h"
#include "utility/nodehelper.h"
#include "nodes/triggervolumenode.h"

#include "resources/resource.h"

#include "../../../game/code/meta/locatorevents.h"
#include "../../../game/code/meta/locatortypes.h"



//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
MTypeId GenericLocatorNode::id( WBConstants::TypeIDPrefix, WBConstants::NodeIDs::GenericLocator );
const char* GenericLocatorNode::stringId = "GenericLocatorNode";

const int GenericLocatorNode::ACTIVE_COLOUR = 15;
const int GenericLocatorNode::INACTIVE_COLOUR = 12;
const float GenericLocatorNode::SCALE = 1.0f * WBConstants::Scale;

char GenericLocatorNode::sNewName[MAX_NAME_LEN];


//******************************************************************************
//
// Callbacks
//
//******************************************************************************

BOOL CALLBACK GenericLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam )
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
                char name[GenericLocatorNode::MAX_NAME_LEN];
                GetDlgItemText( hWnd, IDC_EDIT1, name, GenericLocatorNode::MAX_NAME_LEN );

                if ( strcmp(name, "") == 0 )
                {
                    MExt::DisplayWarning("You must input a new name for the Generic Locator!");
                    return false;
                }
                
                MString newName( WorldBuilder::GetPrefix() );
                newName += MString( name );

                GenericLocatorNode::SetNewName( newName.asChar() );
                EndDialog( hWnd, 0 );  //this is how you close the window.                
                return true;
            }
            else if( LOWORD(wParam) == IDCANCEL )
            {
                GenericLocatorNode::SetNewName( "" );
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
// GenericLocatorNode::GenericLocatorNode
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
GenericLocatorNode::GenericLocatorNode()
{
}

//==============================================================================
// GenericLocatorNode::~GenericLocatorNode
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
GenericLocatorNode::~GenericLocatorNode()
{
}

//=============================================================================
// GenericLocatorNode::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
void* GenericLocatorNode::creator()
{
    return new GenericLocatorNode();
}

//=============================================================================
// GenericLocatorNode::draw
//=============================================================================
// Description: Comment
//
// Parameters:  ( M3dView& view, const MDagPath& path, M3dView::DisplayStyle displayStyle, M3dView::DisplayStatus displayStatus )
//
// Return:      void 
//
//=============================================================================
void GenericLocatorNode::draw( M3dView& view, 
                             const MDagPath& path, 
                             M3dView::DisplayStyle displayStyle, 
                             M3dView::DisplayStatus displayStatus )
{
    if ( WorldBuilder::GetDisplayLevel() & WorldBuilder::GENERIC_LOCATORS )
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

        glPopAttrib();
        view.endGL();
    }
}

//=============================================================================
// GenericLocatorNode::initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MStatus 
//
//=============================================================================
MStatus GenericLocatorNode::initialize()
{
    return MStatus::kSuccess;
}

//=============================================================================
// GenericLocatorNode::postConstructor
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GenericLocatorNode::postConstructor()
{
}

//=============================================================================
// GenericLocatorNode::Export
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& GenericLocatorNode )
//
// Return:      tlDataChunk
//
//=============================================================================
tlDataChunk* GenericLocatorNode::Export( MObject& genericLocatorNode )
{
    MFnDagNode fnNode( genericLocatorNode );

    if ( fnNode.typeId() == GenericLocatorNode::id )
    {
        //Create a tlDataChunk and return it filled with the appropriate data.
        tlWBLocatorChunk* locator = new tlWBLocatorChunk;

        locator->SetName( fnNode.name().asChar() );
        
        if ( fnNode.name().substring(0, 3) == MString("coin") )
        {
            locator->SetType( LocatorType::COIN );
        }
        else
        {
            locator->SetType( LocatorType::GENERIC );
        }

        locator->SetNumDataElements( 0 );

        MPoint thisPosition;
        MExt::GetWorldPosition( &thisPosition, genericLocatorNode );

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
