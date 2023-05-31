//----------------------------------------
// System Includes
//----------------------------------------
#include <math.h>

//----------------------------------------
// Project Includes
//----------------------------------------

#include "Locatorcontext.h"
#include "nodes/EventLocatorNode.h"
#include "nodes/ScriptLocatorNode.h"
#include "nodes/GenericLocatorNode.h"
#include "nodes/CarStartLocatorNode.h"
#include "nodes/TriggerVolumeNode.h"
#include "nodes/zoneeventlocatornode.h"
#include "nodes/occlusionlocatornode.h"
#include "nodes/interiorentrancelocatornode.h"
#include "nodes/directionallocatornode.h"
#include "nodes/ActionEventLocatorNode.h"
#include "nodes/FOVLocatorNode.h"
#include "nodes/BreakableCameraLocatorNode.h"
#include "nodes/StaticCameraLocatorNode.h"
#include "nodes/PedGroupLocator.h"
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
const double DEFAULT_SCALE = 5;

const char* LocatorContext::stringId = "LocatorContext";

LocatorType::Type LocatorContext::mLastLocatorType = LocatorType::NUM_TYPES;
LocatorEvent::Event LocatorContext::mLastEvent = LocatorEvent::CHECK_POINT;

//==============================================================================
// LocatorContextCmd::LocatorContextCmd
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      LocatorContextCmd
//
//==============================================================================
LocatorContextCmd::LocatorContextCmd()
{
}

//==============================================================================
// LocatorContextCmd::~LocatorContextCmd
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      LocatorContextCmd
//
//==============================================================================
LocatorContextCmd::~LocatorContextCmd()
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
void* LocatorContextCmd::creator()
{
    return new LocatorContextCmd();
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
MPxContext* LocatorContextCmd::makeObj()
{
    return new LocatorContext();
}

//==============================================================================
// LocatorContext::LocatorContext
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      LocatorContext
//
//==============================================================================
LocatorContext::LocatorContext() :
    mXCurrent( 0 ),
    mYCurrent( 0 )
{
    SetHelpString();

    setTitleString( "Locator Creation Tool" );    
} 

//==============================================================================
// LocatorContext::~LocatorContext
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      LocatorContext
//
//==============================================================================
LocatorContext::~LocatorContext()
{
}

//==============================================================================
// LocatorContext::abortAction
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//==============================================================================
void LocatorContext::abortAction()
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
void LocatorContext::completeAction()
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
void LocatorContext::deleteAction()
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
MStatus LocatorContext::doDrag( MEvent& event )
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
MStatus LocatorContext::doEnterRegion( MEvent& event )
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
MStatus LocatorContext::doHold( MEvent& event )
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
MStatus LocatorContext::doPress( MEvent& event )
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
MStatus LocatorContext::doRelease( MEvent& event )
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
void LocatorContext::toolOffCleanup()
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
void LocatorContext::toolOnSetup( MEvent& event )
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
void LocatorContext::ProcessState( Stimulus stimulus )
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
// LocatorContext::SetHelpString
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//==============================================================================
void LocatorContext::SetHelpString()
{
    mHelp = "Click to create Locator.";

    setHelpString( mHelp );
}

//==============================================================================
// LocatorContext::InitLocator
//==============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//==============================================================================
void LocatorContext::InitLocator()
{
    //Get the mesh below the clicked point and find it's y height.
    MPoint intersectPoint;
    
    if ( MExt::MeshClickIntersect( mXCurrent, mYCurrent, intersectPoint ) )
    {
        bool needsTriggerVolume = false;
        bool lockTriggerRotation = false;
        MString newName;

        //What kind of locator do we want?
        switch ( WorldBuilder::GetLocatorType() )
        {
        case LocatorType::EVENT:
            {
                MUI::PopupDialogue( IDD_DIALOG1, &EventLocatorNameCallBack );

                newName = MString( EventLocatorNode::GetNewName() );
                
                if ( newName.length() > 0 )
                {
//                    if ( mLastLocatorType == LocatorType::EVENT )
//                    {
//                        if ( !mLocator.isNull() )
//                        {
//                            //Make this the same kind of event as the last one
//                            MFnDependencyNode fnNode( mLocator );
//                            fnNode.findPlug( EventLocatorNode::sEvent ).getValue( mLastEvent );
//                        }
//                    }
//                    else
                    {
                        mLastLocatorType = LocatorType::EVENT;
                    }

                    //TODO, parent this properly.
                    MExt::CreateNode( mLocator, 
                                      mLocatorTransform, 
                                      MString( EventLocatorNode::stringId ), 
                                      &newName );

                    needsTriggerVolume = true;


                    MFnDependencyNode fnNode( mLocator );
                    fnNode.findPlug( EventLocatorNode::sEvent ).setValue( mLastEvent );

                    //Lock out the x and z axis on the transform.
//                    MFnDependencyNode fnDepNode( mLocatorTransform );
//                    fnDepNode.findPlug( MString("rx") ).setLocked( true );
//                    fnDepNode.findPlug( MString("ry") ).setLocked( true );
//                    fnDepNode.findPlug( MString("rz") ).setLocked( true );
                }
                else
                {
                    goto DIE; //Die outta here. It's cancelled.
                }
            }
            break;
        case LocatorType::SCRIPT:
            {
                mLastLocatorType = LocatorType::SCRIPT;

                MUI::PopupDialogue( IDD_DIALOG5, &ScriptLocatorNameCallBack );

                newName = MString( ScriptLocatorNode::GetNewName() );
            
                const char* script = ScriptLocatorNode::GetScriptName();

                if ( newName.length() > 0 )
                {

                    //TODO, parent this properly.
                    MExt::CreateNode( mLocator, 
                                      mLocatorTransform, 
                                      MString( ScriptLocatorNode::stringId ), 
                                      &newName );

                    needsTriggerVolume = true;

                    //Lock out the x and z axis on the transform.
                    MFnDependencyNode fnDepNode( mLocatorTransform );
                    fnDepNode.findPlug( MString("rx") ).setLocked( true );
                    fnDepNode.findPlug( MString("ry") ).setLocked( true );
                    fnDepNode.findPlug( MString("rz") ).setLocked( true );

                    fnDepNode.setObject( mLocator );
                    fnDepNode.findPlug( ScriptLocatorNode::sScript ).setValue( script );
                }
                else
                {
                    goto DIE; //Die outta here. It's cancelled.
                }
            }
            break;
        case LocatorType::GENERIC:
            {
                mLastLocatorType = LocatorType::GENERIC;

                MUI::PopupDialogue( IDD_DIALOG1, &GenericLocatorNameCallBack );

                newName = MString( GenericLocatorNode::GetNewName() );

                if ( newName.length() > 0 )
                {
                   //TODO, parent this properly.
                    MExt::CreateNode( mLocator, 
                                      mLocatorTransform, 
                                      MString( GenericLocatorNode::stringId ), 
                                      &newName );

                    //Lock out the x and z axis on the transform.
                    MFnDependencyNode fnDepNode( mLocatorTransform );
                    fnDepNode.findPlug( MString("rx") ).setLocked( true );
                    fnDepNode.findPlug( MString("ry") ).setLocked( true );
                    fnDepNode.findPlug( MString("rz") ).setLocked( true );
                 }
                 else
                 {
                    goto DIE; //Die outta here. It's cancelled.
                 }
            }
            break;
        case LocatorType::CAR_START:
            {
                mLastLocatorType = LocatorType::CAR_START;

                MUI::PopupDialogue( IDD_DIALOG1, &CarStartLocatorNameCallBack );

                newName = MString( CarStartLocatorNode::GetNewName() );

                if ( newName.length() > 0 )
                {
                   //TODO, parent this properly.
                    MExt::CreateNode( mLocator, 
                                      mLocatorTransform, 
                                      MString( CarStartLocatorNode::stringId ), 
                                      &newName );

                    //Lock out the x and z axis on the transform.
                    MFnDependencyNode fnDepNode( mLocatorTransform );
                    fnDepNode.findPlug( MString("rx") ).setLocked( true );
                    fnDepNode.findPlug( MString("rz") ).setLocked( true );
                 }
                 else
                 {
                    goto DIE; //Die outta here. It's cancelled.
                 }
            }
            break;
        case LocatorType::DYNAMIC_ZONE:
            {
                mLastLocatorType = LocatorType::DYNAMIC_ZONE;

                MUI::PopupDialogue( IDD_DIALOG3, &ZoneEventLocatorNameCallBack );

                newName = MString( ZoneEventLocatorNode::GetNewName() );

                const char* zone = ZoneEventLocatorNode::GetZoneName();
                
                if ( newName.length() > 0 )
                {

                    //TODO, parent this properly.
                    MExt::CreateNode( mLocator, 
                                      mLocatorTransform, 
                                      MString( ZoneEventLocatorNode::stringId ), 
                                      &newName );

                    needsTriggerVolume = true;

                    //Lock out the x and z axis on the transform.
                    MFnDependencyNode fnDepNode( mLocatorTransform );
                    fnDepNode.findPlug( MString("rx") ).setLocked( true );
                    fnDepNode.findPlug( MString("ry") ).setLocked( true );
                    fnDepNode.findPlug( MString("rz") ).setLocked( true );

                    fnDepNode.setObject( mLocator );
                    fnDepNode.findPlug( ZoneEventLocatorNode::sZone ).setValue( zone );
                }
                else
                {
                    goto DIE; //Die outta here. It's cancelled.
                }
            }
            break;
        case LocatorType::OCCLUSION:
            {
                mLastLocatorType = LocatorType::OCCLUSION;

                MUI::PopupDialogue( IDD_DIALOG1, &OcclusionLocatorNameCallBack );

                newName = MString( OcclusionLocatorNode::GetNewName() );
               
                if ( newName.length() > 0 )
                {

                    //TODO, parent this properly.
                    MExt::CreateNode( mLocator, 
                                      mLocatorTransform, 
                                      MString( OcclusionLocatorNode::stringId ), 
                                      &newName );

                    needsTriggerVolume = true;

                    //Lock out the x and z axis on the transform.
                    MFnDependencyNode fnDepNode( mLocatorTransform );
                    fnDepNode.findPlug( MString("rx") ).setLocked( true );
                    fnDepNode.findPlug( MString("ry") ).setLocked( true );
                    fnDepNode.findPlug( MString("rz") ).setLocked( true );

                    fnDepNode.setObject( mLocator );
                }
                else
                {
                    goto DIE; //Die outta here. It's cancelled.
                }
            }
            break;
        case LocatorType::INTERIOR_ENTRANCE:
            {
                mLastLocatorType = LocatorType::INTERIOR_ENTRANCE;

                MUI::PopupDialogue( IDD_DIALOG3, &InteriorEntranceLocatorNameCallBack );

                newName = MString( InteriorEntranceLocatorNode::GetNewName() );

                const char* zone = InteriorEntranceLocatorNode::GetZoneName();
                
                if ( newName.length() > 0 )
                {

                    //TODO, parent this properly.
                    MExt::CreateNode( mLocator, 
                                      mLocatorTransform, 
                                      MString( InteriorEntranceLocatorNode::stringId ), 
                                      &newName );

                    needsTriggerVolume = true;

//                    //Lock out the x and z axis on the transform.
                    MFnDependencyNode fnDepNode( mLocatorTransform );
//                    fnDepNode.findPlug( MString("rx") ).setLocked( true );
//                    fnDepNode.findPlug( MString("ry") ).setLocked( true );
//                    fnDepNode.findPlug( MString("rz") ).setLocked( true );

                    fnDepNode.setObject( mLocator );
                    fnDepNode.findPlug( InteriorEntranceLocatorNode::sZone ).setValue( zone );
                }
                else
                {
                    goto DIE; //Die outta here. It's cancelled.
                }
            }
            break;
        case LocatorType::DIRECTIONAL:
            {
                mLastLocatorType = LocatorType::DIRECTIONAL;

                MUI::PopupDialogue( IDD_DIALOG1, &DirectionalLocatorNameCallBack );

                newName = MString( DirectionalLocatorNode::GetNewName() );
               
                if ( newName.length() > 0 )
                {

                    //TODO, parent this properly.
                    MExt::CreateNode( mLocator, 
                                      mLocatorTransform, 
                                      MString( DirectionalLocatorNode::stringId ), 
                                      &newName );
                }
                else
                {
                    goto DIE; //Die outta here. It's cancelled.
                }
            }
            break;
        case LocatorType::ACTION:
            {
                mLastLocatorType = LocatorType::ACTION;

                MUI::PopupDialogue( IDD_DIALOG4, &ActionEventLocatorNameCallBack );

                newName = MString( ActionEventLocatorNode::GetNewName() );
                MString objName = ActionEventLocatorNode::GetNewObj();
                MString jointName = ActionEventLocatorNode::GetNewJoint();
               
                if ( newName.length() > 0 )
                {

                    //TODO, parent this properly.
                    MExt::CreateNode( mLocator, 
                                      mLocatorTransform, 
                                      MString( ActionEventLocatorNode::stringId ), 
                                      &newName );
                    
                    needsTriggerVolume = true;

                    //Lock out the x and z axis on the transform.
                    MFnDependencyNode fnDepNode( mLocatorTransform );
                    //fnDepNode.findPlug( MString("rx") ).setLocked( true );
                    //fnDepNode.findPlug( MString("ry") ).setLocked( true );
                    //fnDepNode.findPlug( MString("rz") ).setLocked( true );

                    fnDepNode.setObject( mLocator );
                    fnDepNode.findPlug( ActionEventLocatorNode::sObject ).setValue( objName );
                    fnDepNode.findPlug( ActionEventLocatorNode::sJoint ).setValue( jointName );
                }
                else
                {
                    goto DIE; //Die outta here. It's cancelled.
                }
            }
            break;
        case LocatorType::FOV:
            {
                mLastLocatorType = LocatorType::FOV;

                MUI::PopupDialogue( IDD_DIALOG1, &FOVLocatorNameCallBack );

                newName = MString( FOVLocatorNode::GetNewName() );
               
                if ( newName.length() > 0 )
                {

                    //TODO, parent this properly.
                    MExt::CreateNode( mLocator, 
                                      mLocatorTransform, 
                                      MString( FOVLocatorNode::stringId ), 
                                      &newName );
                    
                    needsTriggerVolume = true;

                    //Lock out the x and z axis on the transform.
                    MFnDependencyNode fnDepNode( mLocatorTransform );
                    fnDepNode.findPlug( MString("rx") ).setLocked( true );
                    fnDepNode.findPlug( MString("ry") ).setLocked( true );
                    fnDepNode.findPlug( MString("rz") ).setLocked( true );
                }
                else
                {
                    goto DIE; //Die outta here. It's cancelled.
                }
            }
            break;
        case LocatorType::BREAKABLE_CAMERA:
            {
                mLastLocatorType = LocatorType::BREAKABLE_CAMERA;

                MUI::PopupDialogue( IDD_DIALOG1, &BreakableCameraLocatorNameCallBack );

                newName = MString( BreakableCameraLocatorNode::GetNewName() );
               
                if ( newName.length() > 0 )
                {

                    //TODO, parent this properly.
                    MExt::CreateNode( mLocator, 
                                      mLocatorTransform, 
                                      MString( BreakableCameraLocatorNode::stringId ), 
                                      &newName );
                }
                else
                {
                    goto DIE; //Die outta here. It's cancelled.
                }
            }
            break;
        case LocatorType::STATIC_CAMERA:
            {
                mLastLocatorType = LocatorType::STATIC_CAMERA;

                MUI::PopupDialogue( IDD_DIALOG1, &StaticCameraLocatorNameCallBack );

                newName = MString( StaticCameraLocatorNode::GetNewName() );
               
                if ( newName.length() > 0 )
                {

                    //TODO, parent this properly.
                    MExt::CreateNode( mLocator, 
                                      mLocatorTransform, 
                                      MString( StaticCameraLocatorNode::stringId ), 
                                      &newName );

                    needsTriggerVolume = true;
                }
                else
                {
                    goto DIE; //Die outta here. It's cancelled.
                }
            }
            break;
        case LocatorType::PED_GROUP:
            {
                mLastLocatorType = LocatorType::PED_GROUP;

                MUI::PopupDialogue( IDD_DIALOG1, &PedGroupLocatorNameCallBack );

                newName = MString( PedGroupLocatorNode::GetNewName() );
               
                if ( newName.length() > 0 )
                {

                    //TODO, parent this properly.
                    MExt::CreateNode( mLocator, 
                                      mLocatorTransform, 
                                      MString( PedGroupLocatorNode::stringId ), 
                                      &newName );

                    needsTriggerVolume = true;
                }
                else
                {
                    goto DIE; //Die outta here. It's cancelled.
                }
            }
            break;
        default:
            assert( false );
            break;
        }
        
        assert( !mLocator.isNull() );

        MExt::SetWorldPosition( intersectPoint, mLocator );

        if ( needsTriggerVolume )
        {
            //Create and connect a trigger volume to this node.
            MObject tv;
            MObject tvt;

            MString name( newName );
            name += MString( "Trigger" );

            MExt::CreateNode( tv, 
                              tvt, 
                              MString( TriggerVolumeNode::stringId ), 
                              &name );

            MExt::SetWorldPosition( intersectPoint, tv );

            MFnTransform fnTransform( tvt );
            const double scale[3] = { DEFAULT_SCALE, DEFAULT_SCALE, DEFAULT_SCALE };
            
            //Scale this bad-boy!
            fnTransform.setScale( scale );

            if ( lockTriggerRotation )
            {
                fnTransform.findPlug( MString( "rx" ) ).setLocked( true );
                fnTransform.findPlug( MString( "ry" ) ).setLocked( true );
                fnTransform.findPlug( MString( "rz" ) ).setLocked( true );
            }

            //Connect the Trigger Volume to the Locator.
            MExt::Connect( tv, "locator", mLocator, "triggers" );

            WorldBuilder::AddChild( tv );
        }

        WorldBuilder::AddChild( mLocator );

    }

DIE:
    MGlobal::clearSelectionList();
}
