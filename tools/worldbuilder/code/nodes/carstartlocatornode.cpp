//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        CarStartLocatorNode.cpp
//
// Description: Implement CarStartLocatorNode
//
// History:     28/05/2002 + Created -- Cary Brisebois
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
#include "nodes/CarStartLocatorNode.h"
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
MTypeId CarStartLocatorNode::id( WBConstants::TypeIDPrefix, WBConstants::NodeIDs::CarStartLocator );
const char* CarStartLocatorNode::stringId = "CarStartLocatorNode";

const int CarStartLocatorNode::ACTIVE_COLOUR = 15;
const int CarStartLocatorNode::INACTIVE_COLOUR = 12;
const float CarStartLocatorNode::SCALE = 1.0f * WBConstants::Scale;

char CarStartLocatorNode::sNewName[MAX_NAME_LEN];

const char*  CarStartLocatorNode::ISPARKED_NAME_SHORT = "ip";
const char*  CarStartLocatorNode::ISPARKED_NAME_LONG  = "isParked";
MObject CarStartLocatorNode::sIsParked;

const char*  CarStartLocatorNode::SPECIAL_NAME_SHORT = "spc";
const char*  CarStartLocatorNode::SPECIAL_NAME_LONG = "specialName";
MObject CarStartLocatorNode::sSpecialCarName;



//******************************************************************************
//
// Callbacks
//
//******************************************************************************

BOOL CALLBACK CarStartLocatorNameCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam )
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
                char name[CarStartLocatorNode::MAX_NAME_LEN];
                GetDlgItemText( hWnd, IDC_EDIT1, name, CarStartLocatorNode::MAX_NAME_LEN );

                if ( strcmp(name, "") == 0 )
                {
                    MExt::DisplayWarning("You must input a new name for the Car Start Locator!");
                    return false;
                }

                MString newName( WorldBuilder::GetPrefix() );
                newName += MString( name );
                
                CarStartLocatorNode::SetNewName( newName.asChar() );
                EndDialog( hWnd, 0 );  //this is how you close the window.                
                return true;
            }
            else if( LOWORD(wParam) == IDCANCEL )
            {
                CarStartLocatorNode::SetNewName( "" );
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
// CarStartLocatorNode::CarStartLocatorNode
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
CarStartLocatorNode::CarStartLocatorNode()
{
}

//==============================================================================
// CarStartLocatorNode::~CarStartLocatorNode
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
CarStartLocatorNode::~CarStartLocatorNode()
{
}

//=============================================================================
// CarStartLocatorNode::creator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
void* CarStartLocatorNode::creator()
{
    return new CarStartLocatorNode();
}

//=============================================================================
// CarStartLocatorNode::draw
//=============================================================================
// Description: Comment
//
// Parameters:  ( M3dView& view, const MDagPath& path, M3dView::DisplayStyle displayStyle, M3dView::DisplayStatus displayStatus )
//
// Return:      void 
//
//=============================================================================
void CarStartLocatorNode::draw( M3dView& view, 
                             const MDagPath& path, 
                             M3dView::DisplayStyle displayStyle, 
                             M3dView::DisplayStatus displayStatus )
{
    if ( WorldBuilder::GetDisplayLevel() & WorldBuilder::CARSTART_LOCATORS )
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
        GLExt::drawPyramid( SCALE, 0,0,0, 5.0 );
        GLExt::drawCar( SCALE, 0,1,0, 5.0 );
        GLExt::drawArrow( MPoint( 0, 0, 0 ), MPoint( 0, 0, 1 * WBConstants::Scale ), 5.0 );

        glPopAttrib();
        view.endGL();
    }
}

//=============================================================================
// CarStartLocatorNode::initialize
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      MStatus 
//
//=============================================================================
MStatus CarStartLocatorNode::initialize()
{
    MStatus status;

    MFnNumericAttribute numericAttr;
    sIsParked = numericAttr.create( ISPARKED_NAME_LONG, ISPARKED_NAME_SHORT, MFnNumericData::kBoolean, false, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( numericAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( numericAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sIsParked ) );

    MFnTypedAttribute typedAttr;
    sSpecialCarName = typedAttr.create( SPECIAL_NAME_LONG, SPECIAL_NAME_SHORT, MFnData::kString, &status );
    RETURN_STATUS_ON_FAILURE( status );
    RETURN_STATUS_ON_FAILURE( typedAttr.setReadable( true ) );
    RETURN_STATUS_ON_FAILURE( typedAttr.setWritable( true ) );
    RETURN_STATUS_ON_FAILURE( addAttribute( sSpecialCarName ) );

    return MStatus::kSuccess;
}

//=============================================================================
// CarStartLocatorNode::postConstructor
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void CarStartLocatorNode::postConstructor()
{
}

//=============================================================================
// CarStartLocatorNode::Export
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& CarStartLocatorNode )
//
// Return:      tlDataChunk
//
//=============================================================================
tlDataChunk* CarStartLocatorNode::Export( MObject& carStartLocatorNode )
{
    MFnDagNode fnNode( carStartLocatorNode );

    if ( fnNode.typeId() == CarStartLocatorNode::id )
    {
        //Create a tlDataChunk and return it filled with the appropriate data.
        tlWBLocatorChunk* locator = new tlWBLocatorChunk;

        locator->SetName( fnNode.name().asChar() );

        locator->SetType( LocatorType::CAR_START );

        unsigned int numDataElements = 2;

        MString specialName;
        fnNode.findPlug( sSpecialCarName ).getValue( specialName );
        if ( specialName.length() > 0 )
        {
            //Add the length of the string + 1 to the num of 
            //elements.
            numDataElements += specialName.length() / 4 + 1;
        }

        unsigned long* data = new unsigned long[numDataElements];  //1 (rotation) + 1 (isParked) + special name len + 1
        memset( data, 0, numDataElements * 4 );

        //The data here is the Y rotation associated with this locator.
        double rotationDouble;
        MObject transform = fnNode.parent( 0 );
        MFnDependencyNode fnDepNode( transform );
        fnDepNode.findPlug( MString("ry") ).getValue( rotationDouble );
        
        //This is for p3d!
        rotationDouble *= -1;
       
        float rotationFloat = (float)rotationDouble;
        
        memcpy( &data[0], &rotationFloat, sizeof(float) );


        //Do the is Parked Car
        bool isParked = false; 
        fnNode.findPlug( sIsParked ).getValue( isParked );
        data[1] = isParked ? 1 : 0;

        if ( specialName.length() > 0 )
        {
            //Add the name
            memcpy( &data[2], specialName.asChar(), specialName.length() );
        }

        locator->SetDataElements( data, numDataElements );
        locator->SetNumDataElements( numDataElements );

        MPoint thisPosition;
        MExt::GetWorldPosition( &thisPosition, carStartLocatorNode );

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
