//----------------------------------------
// System Includes
//----------------------------------------
#include <math.h>

//----------------------------------------
// Project Includes
//----------------------------------------
#include <nodes/triggervolumenode.h>
#include <nodes/occlusionlocatornode.h>
#include "triggercontext.h"
#include "utility/mext.h"
#include "utility/mui.h"
#include "main/worldbuilder.h"
#include "main/constants.h"

#include "resources/resource.h"

//----------------------------------------
// Constants, Typedefs and Statics
//----------------------------------------

const short OFFSET = 10;
const double SCALE_FACTOR = 0.002;
const double DEFAULT_SCALE = 25;

const char* TriggerContext::stringId = "TriggerContext";

//==============================================================================
// TriggerContextCmd::TriggerContextCmd
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      TriggerContextCmd
//
//==============================================================================
TriggerContextCmd::TriggerContextCmd()
{
}

//==============================================================================
// TriggerContextCmd::~TriggerContextCmd
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      TriggerContextCmd
//
//==============================================================================
TriggerContextCmd::~TriggerContextCmd()
{
}

//-----------------------------------------------------------------------------
// c r e a t o r 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void* TriggerContextCmd::creator()
{
    return new TriggerContextCmd();
}

//-----------------------------------------------------------------------------
// m a k e O b j 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
MPxContext* TriggerContextCmd::makeObj()
{
    return new TriggerContext();
}

//==============================================================================
// TriggerContext::TriggerContext
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      TriggerContext
//
//==============================================================================
TriggerContext::TriggerContext() :
    mXCurrent( 0 ),
    mYCurrent( 0 )
{
    SetHelpString();

    setTitleString( "Trigger Creation Tool" );    
} 

//==============================================================================
// TriggerContext::~TriggerContext
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      TriggerContext
//
//==============================================================================
TriggerContext::~TriggerContext()
{
}

//==============================================================================
// TriggerContext::abortAction
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//==============================================================================
void TriggerContext::abortAction()
{
    ProcessState( ABORTED );
}

//-----------------------------------------------------------------------------
// c o m p l e t e A c t i o n 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void TriggerContext::completeAction()
{
    ProcessState( COMPLETED );
}

//-----------------------------------------------------------------------------
// d e l e t e A c t i o n 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void TriggerContext::deleteAction()
{
    ProcessState( DELETED );
}

//-----------------------------------------------------------------------------
// d o D r a g 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
MStatus TriggerContext::doDrag( MEvent& event )
{

    event.getPosition( mXDrag, mYDrag );
    ProcessState( MOUSEDRAG );
    return MS::kSuccess;
}

//-----------------------------------------------------------------------------
// d o E n t e r R e g i o n 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
MStatus TriggerContext::doEnterRegion( MEvent& event )
{
    SetHelpString();

    return MS::kSuccess;
}

//-----------------------------------------------------------------------------
// d o H o l d 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
MStatus TriggerContext::doHold( MEvent& event )
{
    MStatus status = MS::kSuccess;
    return status;
}

//-----------------------------------------------------------------------------
// d o P r e s s 
//
// Synopsis:    
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
MStatus TriggerContext::doPress( MEvent& event )
{
    event.getPosition( mXCurrent, mYCurrent );
    ProcessState( BUTTONDOWN );
    return MS::kSuccess;
}

//-----------------------------------------------------------------------------
// d o R e l e a s e 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
MStatus TriggerContext::doRelease( MEvent& event )
{
    if ( event.mouseButton() == MEvent::kLeftMouse )
    {
        event.getPosition( mXCurrent, mYCurrent );
        ProcessState( BUTTONUP );
    }

    return MS::kSuccess;
}

//-----------------------------------------------------------------------------
// t o o l O f f C l e a n u p
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void TriggerContext::toolOffCleanup()
{
    if ( mLocatorTransform != MObject::kNullObj )
    {
        mLocator = MObject::kNullObj;
        mLocatorTransform = MObject::kNullObj;
    }
} 

//-----------------------------------------------------------------------------
// t o o l O n S e t u p 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void TriggerContext::toolOnSetup( MEvent& event )
{
    setCursor( MCursor::crossHairCursor );
}

//-----------------------------------------------------------------------------
//
// P R I V A T E   M E M B E R S
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// p r o c e s s S t a t e 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void TriggerContext::ProcessState( Stimulus stimulus )
{
    switch( stimulus )
    {
    case BUTTONDOWN:
        {
        }
        break;
    case MOUSEDRAG:
        {
        }
        break;
    case BUTTONUP:
        {
            InitLocator();
        }
        break;
    case COMPLETED:
    case ABORTED:
        {
            if ( mLocatorTransform != MObject::kNullObj )
            {
                mLocator = MObject::kNullObj;
                mLocatorTransform = MObject::kNullObj;
            }
        }
        break;
    case DELETED:
        {
            if ( mLocatorTransform != MObject::kNullObj )
            {
                MGlobal::deleteNode( mLocator );
                MGlobal::deleteNode( mLocatorTransform );
            }
        }
        break;
    default:
        {
        }
        break;
    }

    SetHelpString();
}

//==============================================================================
// TriggerContext::SetHelpString
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//==============================================================================
void TriggerContext::SetHelpString()
{
    mHelp = "Click to create Locator.";

    setHelpString( mHelp );
}

//==============================================================================
// TriggerContext::InitLocator
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//==============================================================================
void TriggerContext::InitLocator()
{
    //Get the mesh below the clicked point and find it's y height.
    MPoint intersectPoint;
    
    if ( MExt::MeshClickIntersect( mXCurrent, mYCurrent, intersectPoint ) )
    {
        MDagPath dagPath;
        MExt::FindDagNodeByName( &dagPath, MString( WorldBuilder::GetSelectedLocator() ) );

        MFnDagNode fnDagNode( dagPath );

        bool isOcclusionNode = false;
        bool isVisibler = false;

        if ( fnDagNode.typeId() == OcclusionLocatorNode::id )
        {
            isOcclusionNode = true;

            MString cmd( "IsVisibler" );

            int result = 0;
            MGlobal::executeCommand( cmd, result );
            isVisibler = (result == 1);
        }

        MString selectedObjectName;
        selectedObjectName = fnDagNode.name();
      
        if ( selectedObjectName.length() > 0 )
        {
            MString newName;

            if ( isOcclusionNode )
            {
                if ( isVisibler )
                {
                    newName = MString("Vis") + selectedObjectName + MString("Trig");
                }
                else
                {
                    newName = MString("Occ") + selectedObjectName + MString("Trig");
                }
            }
            else
            {
                newName = selectedObjectName + MString("Trigger");
            }

            //TODO, parent this properly.
            MExt::CreateNode( mLocator, 
                              mLocatorTransform, 
                              MString( TriggerVolumeNode::stringId ), 
                              &newName );


            if ( isOcclusionNode )
            {
                //Lock out the x and z axis on the transform.
                MFnDependencyNode fnDepNode( mLocatorTransform );
                fnDepNode.findPlug( MString("rx") ).setLocked( true );
                fnDepNode.findPlug( MString("ry") ).setLocked( true );
                fnDepNode.findPlug( MString("rz") ).setLocked( true );
            }

            MExt::SetWorldPosition( intersectPoint, mLocator );

            MFnTransform fnTransform( mLocatorTransform );
            const double scale[3] = { DEFAULT_SCALE, DEFAULT_SCALE, DEFAULT_SCALE };

            //Scale this bad-boy!
            fnTransform.setScale( scale );


            //Connect the Trigger Volume to the Locator.
            MExt::Connect( mLocator, "locator", fnDagNode.object(), "triggers" );

            WorldBuilder::AddChild( mLocator );
        }
        else
        {
            goto DIE; //Die outta here. It's cancelled.
        }
    }

DIE:
    MGlobal::clearSelectionList();
}
