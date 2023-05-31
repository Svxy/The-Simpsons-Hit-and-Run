//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        GameEngine.cpp
//
// Description: Implement GameEngine
//
// History:     19/07/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================

#ifdef WORLD_BUILDER
#include "main/toolhack.h"
#endif

#include <assert.h>
#include <tlmatrix.hpp>
#include <tlpoint.hpp>
#include <p3d/pointcamera.hpp>

//========================================
// Project Includes
//========================================
#include "GameEngine.h"
#include "nodes/railcamlocatornode.h"
#include "nodes/staticcameralocatornode.h"
#include "nodes/fovlocatornode.h"
#include "nodes/splinelocatornode.h"
#include "nodes/triggervolumenode.h"
#include "nodes/staticcameralocatornode.h"
#include "..\..\..\game\code\meta\recttriggervolume.h"
#include "..\..\..\game\code\meta\spheretriggervolume.h"
#include "..\..\..\game\code\meta\triggervolume.h"
#include "utility\mext.h"
#include "utility\glext.h"
#include "main\constants.h"
#include "wbcamtarget.h"

#include "..\..\..\game\code\camera\railcam.h"
#include "..\..\..\game\code\camera\staticcam.h"
#include "utility/transformmatrix.h"

#include <radtime.hpp>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
GameEngine* GameEngine::mInstance = NULL;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//=============================================================================
// GameEngine::CreateInstance
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GameEngine::CreateInstance()
{
    assert( mInstance == NULL );

    mInstance = new GameEngine();
}

//=============================================================================
// GameEngine::DestroyInstance
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GameEngine::DestroyInstance()
{
    assert( mInstance );

    delete mInstance;
    mInstance = NULL;
}

//=============================================================================
// GameEngine::GetInstance
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      GameEngine
//
//=============================================================================
GameEngine* GameEngine::GetInstance()
{
    return mInstance;
}

//=============================================================================
// GameEngine::UpdateRailCam
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& railCam )
//
// Return:      void 
//
//=============================================================================
void GameEngine::UpdateRailCam( MObject& railCamObj )
{
    MStatus status;

    MFnDependencyNode fnDepNode ( railCamObj );
    
    MPlug activePlug = fnDepNode.findPlug( RailCamLocatorNode::sActive, &status );
    assert( status );

    bool active = false;
    activePlug.getValue( active );

    //Test to see if this is active.
    if ( active )
    {
        MString targetName;
        fnDepNode.findPlug( RailCamLocatorNode::sTarget, &status ).getValue( targetName );
        assert( status );

        //Test to see if this has a target...
        if ( targetName.length() != 0 )
        {
            MDagPath targetPath;
            MObject targetObj;
            if ( !MExt::FindDagNodeByName( &targetPath, targetName ) )
            {
                //The target does not exist...
                return;
            }
            
            targetObj = targetPath.node();

            RailCam* railCam = dynamic_cast<RailCamLocatorNode*>(fnDepNode.userNode())->GetRailCam();
            assert( railCam );

            WBCamTarget* target = dynamic_cast<RailCamLocatorNode*>(fnDepNode.userNode())->GetTarget();
            assert( target );

            target->SetTarget( targetObj );

            rmt::Vector position;
            target->GetPosition( &position );

            //Get the spline locator (it has the triggers)
            MPlug railPlug = fnDepNode.findPlug( MString( "message" ), &status );
            assert( status );

            MPlugArray targets;
            railPlug.connectedTo( targets, false, true, &status );
            assert( status );

            assert( targets.length() == 1 );
            if ( targets.length() != 1 )
            {
                return;
            }

            MObject splineLoc = targets[0].node();

            MFnDependencyNode splineFNDepNode( splineLoc );
            MPlug triggersPlug = splineFNDepNode.findPlug( SplineLocatorNode::sTriggers, &status );
            assert( status );

            MPlugArray sources;
            MExt::ResolveConnections( &sources, &targets, triggersPlug, true, false );

            unsigned int i; 
            for ( i = 0; i < sources.length(); ++i )  
            {
                //Test to see if the target is in a trigger volume for this
                //Test against all triggers...
                MFnDependencyNode triggerFNDepNode( sources[i].node() );
                int type = 0;
                triggerFNDepNode.findPlug( TriggerVolumeNode::sType, &status ).getValue( type );
                assert( status );

                bool inside = false;

                tlMatrix mat;
                tlPoint scale;
                TriggerVolumeNode::GetScaleAndMatrix( sources[i].node(), mat, scale );

                switch ( type )
                {
                case TriggerVolumeNode::RECTANGLE:
                    {
                        RectTriggerVolume* trigVol = new RectTriggerVolume( mat.GetRow( 3 ), 
                                                   mat.GetRow( 0 ), 
                                                   mat.GetRow( 1 ), 
                                                   mat.GetRow( 2 ), 
                                                   scale.x,
                                                   scale.y,
                                                   scale.z ); 
                        
                        inside = trigVol->Contains( position );

                        trigVol->Release();
                    }
                    break;
                case TriggerVolumeNode::SPHERE:
                    {
                        SphereTriggerVolume* trigVol = new SphereTriggerVolume( mat.GetRow( 3 ), scale.x );

                        inside = trigVol->Contains( position );

                        trigVol->Release();
                    }
                    break;
                default:
                    assert( false );
                }
                
                if ( inside )
                {
                    //If the object is in a trigger volume update the camera and set
                    //the new position of the railCam

                    //We need to give it the splinecurve.
                    MPlug splineCurvePlug = fnDepNode.findPlug( RailCamLocatorNode::sRail, &status );
                    assert( status );

                    MPlugArray splines;
                    splineCurvePlug.connectedTo( splines, true, false, &status );
                    assert( status );

                    assert( splines.length() == 1 );
                    if ( splines.length() != 1 )
                    {
                        //Something has deleted the spline.
                        return;
                    }

                    MFnDependencyNode splineCurveFNDepNode( splines[0].node() );

                    MDagPath splineCurvePath;
                    bool found = MExt::FindDagNodeByName( &splineCurvePath, splineCurveFNDepNode.name() );
                    assert( found );

                    MFnNurbsCurve fnNurbsCurve( splineCurvePath, &status );
                    assert( status );

                    MPointArray cvs;
                    fnNurbsCurve.getCVs( cvs, MSpace::kWorld );

                    railCam->SetNumCVs( cvs.length() );

                    unsigned int cvCount;
                    for ( cvCount = 0; cvCount < cvs.length(); ++cvCount )
                    {
                        float x, y, z;

                        x = cvs[cvCount].x / WBConstants::Scale;
                        y = cvs[cvCount].y / WBConstants::Scale;
                        z = -cvs[cvCount].z / WBConstants::Scale;

                        rmt::Vector point( x, y, z );

                        railCam->SetVertex( cvCount, point );                            
                    }
                    
                    railCam->SetTarget( target );


                    //Test for FOV changes.
                    UpdateFOV( railCam, position );

                    UpdateRailSettings( railCam, railCamObj );

                    railCam->Update( 16 );

                    rmt::Vector newPos;
                    railCam->GetCamera()->GetPosition( &newPos );
                    rmt::Vector newTarg;
                    railCam->GetCamera()->GetTarget( &newTarg );
                    rmt::Vector newVUp;
                    railCam->GetCamera()->GetVUp( &newVUp );
                    float fov, aspect; 
                    railCam->GetCamera()->GetFOV( &fov, &aspect );
                       
                    mMayaCam->Set( newPos, newTarg, newVUp, rmt::RadianToDeg(fov) );

                    MPoint newPoint;
                    newPoint.x = newPos.x * WBConstants::Scale;
                    newPoint.y = newPos.y * WBConstants::Scale;
                    newPoint.z = -newPos.z * WBConstants::Scale;

                    MExt::SetWorldPosition( newPoint, railCamObj );
                    
                    //Get out of the loop.
                    break;
                }
            }
        }        
    }        
}


//=============================================================================
// GameEngine::UpdateStaticCam
//=============================================================================
// Description: Comment
//
// Parameters:  ( MObject& staticCam )
//
// Return:      void 
//
//=============================================================================
void GameEngine::UpdateStaticCam( MObject& staticCamObj )
{
    MStatus status;

    MFnDependencyNode fnDepNode ( staticCamObj );
    
    MPlug activePlug = fnDepNode.findPlug( StaticCameraLocatorNode::sActive, &status );
    assert( status );

    bool active = false;
    activePlug.getValue( active );

    //Test to see if this is active.
    if ( active )
    {
        bool tracking = false;
        fnDepNode.findPlug( StaticCameraLocatorNode::sTracking, &status ).getValue( tracking );
        assert( status );

        MString targetName;
        fnDepNode.findPlug( StaticCameraLocatorNode::sTarget, &status ).getValue( targetName );
        assert( status );

        //Test to see if this has a target...
        if ( targetName.length() != 0 )
        {
            MDagPath targetPath;
            MObject targetObj;
            if ( !MExt::FindDagNodeByName( &targetPath, targetName ) )
            {
                //The target does not exist...
                return;
            }
        
            //Now we have a target to test for being in the cameras trigger volume
            targetObj = targetPath.node();

            StaticCam* staticCam = reinterpret_cast<StaticCameraLocatorNode*>(fnDepNode.userNode())->GetStaticCam();
            assert( staticCam );

            WBCamTarget* target = reinterpret_cast<StaticCameraLocatorNode*>(fnDepNode.userNode())->GetTarget();
            assert( target );

            target->SetTarget( targetObj );

            rmt::Vector position;
            target->GetPosition( &position );

            //Test the trigger volumes
            MPlug triggersPlug = fnDepNode.findPlug( StaticCameraLocatorNode::sTriggers, &status );
            assert( status );

            MPlugArray sources, targets;
            MExt::ResolveConnections( &sources, &targets, triggersPlug, true, false );

            unsigned int i; 
            for ( i = 0; i < sources.length(); ++i )  
            {
                //Test to see if the target is in a trigger volume for this
                //Test against all triggers...
                MFnDependencyNode triggerFNDepNode( sources[i].node() );
                int type = 0;
                triggerFNDepNode.findPlug( TriggerVolumeNode::sType, &status ).getValue( type );
                assert( status );

                bool inside = false;

                tlMatrix mat;
                tlPoint scale;
                TriggerVolumeNode::GetScaleAndMatrix( sources[i].node(), mat, scale );

                switch ( type )
                {
                case TriggerVolumeNode::RECTANGLE:
                    {
                        RectTriggerVolume* trigVol = new RectTriggerVolume( mat.GetRow( 3 ), 
                                                   mat.GetRow( 0 ), 
                                                   mat.GetRow( 1 ), 
                                                   mat.GetRow( 2 ), 
                                                   scale.x,
                                                   scale.y,
                                                   scale.z ); 
                        
                        inside = trigVol->Contains( position );

                        trigVol->Release();
                    }
                    break;
                case TriggerVolumeNode::SPHERE:
                    {
                        SphereTriggerVolume* trigVol = new SphereTriggerVolume( mat.GetRow( 3 ), scale.x );

                        inside = trigVol->Contains( position );

                        trigVol->Release();
                    }
                    break;
                default:
                    assert( false );
                }
                
                if ( inside )
                {
                    //If the object is in a trigger volume update the camera and set
                    //the new position of the static cam
                    
                    staticCam->SetTarget( target );


                    //Test for FOV changes.
                    UpdateFOV( staticCam, position );

                    UpdateStaticCamSettings( staticCam, staticCamObj );

                    staticCam->Update( 16 );

                    rmt::Vector newPos;
                    staticCam->GetCamera()->GetPosition( &newPos );
                    rmt::Vector newTarg;
                    staticCam->GetCamera()->GetTarget( &newTarg );
                    rmt::Vector newVUp;
                    staticCam->GetCamera()->GetVUp( &newVUp );
                    float fov, aspect; 
                    staticCam->GetCamera()->GetFOV( &fov, &aspect );
                       
                    mMayaCam->Set( newPos, newTarg, newVUp, rmt::RadianToDeg(fov) );

                    MPoint newPoint;
                    newPoint.x = newPos.x * WBConstants::Scale;
                    newPoint.y = newPos.y * WBConstants::Scale;
                    newPoint.z = -newPos.z * WBConstants::Scale;

                    MExt::SetWorldPosition( newPoint, staticCamObj );
                    
                    //Get out of the loop.
                    break;
                }
            }
        }        
    }        
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//==============================================================================
// GameEngine::GameEngine
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
GameEngine::GameEngine() :
    mTimeStart(0)
{
    mMayaCam = new MayaCamera();
}

//==============================================================================
// GameEngine::~GameEngine
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
GameEngine::~GameEngine()
{
    Init();

    delete mMayaCam;
}

void GameEngine::Init()
{
}

//=============================================================================
// GameEngine::UpdateRailSettings
//=============================================================================
// Description: Comment
//
// Parameters:  ( RailCam* railCam, MObject obj )
//
// Return:      void 
//
//=============================================================================
void GameEngine::UpdateRailSettings( RailCam* railCam, MObject obj )
{
    MFnDependencyNode fnDepNode( obj );
    
    int behav;
    fnDepNode.findPlug( RailCamLocatorNode::sBehaviour).getValue( behav );
    railCam->SetBehaviour( (RailCam::Behaviour)(behav) );
    
    float minRad;
    fnDepNode.findPlug( RailCamLocatorNode::sMinRadius ).getValue( minRad );
    railCam->SetMinRadius( minRad );

    float maxRad;
    fnDepNode.findPlug( RailCamLocatorNode::sMaxRadius ).getValue( maxRad );
    railCam->SetMaxRadius( maxRad );

    bool trackRail;
    fnDepNode.findPlug( RailCamLocatorNode::sTrackRail ).getValue( trackRail );
    railCam->SetTrackRail( trackRail );

    float trackDist;
    fnDepNode.findPlug( RailCamLocatorNode::sTrackDist ).getValue( trackDist );
    railCam->SetTrackDist( trackDist );

    bool reverse;
    fnDepNode.findPlug( RailCamLocatorNode::sReverseSense ).getValue( reverse );
    railCam->SetReverseSense( reverse );
    
    float fov;
    fnDepNode.findPlug( RailCamLocatorNode::sFOV ).getValue( fov );
    railCam->SetFOV( rmt::DegToRadian(fov) );

    float x, y, z;
    fnDepNode.findPlug( RailCamLocatorNode::sFacingOffset ).child(0).getValue( x );
    fnDepNode.findPlug( RailCamLocatorNode::sFacingOffset ).child(1).getValue( y );
    fnDepNode.findPlug( RailCamLocatorNode::sFacingOffset ).child(2).getValue( z );
    railCam->SetTargetOffset( rmt::Vector( x, y, z ) );

    //Same with axis play... TODO

    float posLag;
    fnDepNode.findPlug( RailCamLocatorNode::sPositionLag ).getValue( posLag );
    railCam->SetPositionLag( posLag );

    float targLag;
    fnDepNode.findPlug( RailCamLocatorNode::sTargetLag ).getValue( targLag );
    railCam->SetTargetLag( targLag );   
}

//=============================================================================
// GameEngine::UpdateStaticCamSettings
//=============================================================================
// Description: Comment
//
// Parameters:  ( StaticCam* staticCam, MObject obj )
//
// Return:      void 
//
//=============================================================================
void GameEngine::UpdateStaticCamSettings( StaticCam* staticCam, MObject obj )
{
    MFnDependencyNode fnDepNode( obj );
       
    float fov;
    fnDepNode.findPlug( StaticCameraLocatorNode::sFOV ).getValue( fov );
    staticCam->SetFOV( rmt::DegToRadian(fov) );

    float targLag;
    fnDepNode.findPlug( StaticCameraLocatorNode::sTargetLag ).getValue( targLag );
    staticCam->SetTargetLag( targLag );  
    
    MPoint worldPos;
    MExt::GetWorldPosition( &worldPos, obj );

    rmt::Vector pos;
    pos.x = worldPos.x / WBConstants::Scale;
    pos.y = worldPos.y / WBConstants::Scale;
    pos.z = -worldPos.z / WBConstants::Scale;

    staticCam->SetPosition( pos );

    bool tracking = false;
    fnDepNode.findPlug( StaticCameraLocatorNode::sTracking ).getValue( tracking );
    staticCam->SetTracking( tracking );

    float x, y, z;
    fnDepNode.findPlug( StaticCameraLocatorNode::sFacingOffset ).child(0).getValue( x );
    fnDepNode.findPlug( StaticCameraLocatorNode::sFacingOffset ).child(1).getValue( y );
    fnDepNode.findPlug( StaticCameraLocatorNode::sFacingOffset ).child(2).getValue( z );

    if ( tracking )
    {
        staticCam->SetTargetOffset( rmt::Vector( x, y, z ) );
    }
    else
    {
        //Figure out the transformation on the locator node and apply it to the offset.
        MObject transform;
        MFnDagNode fnDAGNode( obj );
        transform = fnDAGNode.parent( 0 );
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

        if ( x == 0 && y == 0 && z == 0 )
        {
            z = 1.0f;
        }

        tlPoint offset( x, y, z );
        offset = VectorTransform( hmatrix, offset );

        rmt::Vector targPos = pos;
        targPos.Add( offset );
        staticCam->SetTargetOffset( targPos );
    }
}

//=============================================================================
// GameEngine::UpdateFOV
//=============================================================================
// Description: Comment
//
// Parameters:  ( SuperCam* cam, const rmt::Vector position )
//
// Return:      void 
//
//=============================================================================
void GameEngine::UpdateFOV( SuperCam* cam, const rmt::Vector position )
{
    MStatus status;
    MItDag dagIt( MItDag::kDepthFirst, MFn::kLocator );

    bool inside = false;

    while ( !dagIt.isDone() )
    {
        MFnDependencyNode fnNode( dagIt.item() );
        if ( fnNode.typeId() == FOVLocatorNode::id )
        {
            //This is a FOV locator, let's see if we're in it's trigger volume.
            
            MPlug triggersPlug = fnNode.findPlug( FOVLocatorNode::sTriggers, &status );
            assert( status );

            MPlugArray sources, targets;
            MExt::ResolveConnections( &sources, &targets, triggersPlug, true, false );

            unsigned int i; 
            for ( i = 0; i < sources.length(); ++i )  
            {
                //Test to see if the target is in a trigger volume for this
                //Test against all triggers...
                MFnDependencyNode triggerFNDepNode( sources[i].node() );
                int type = 0;
                triggerFNDepNode.findPlug( TriggerVolumeNode::sType, &status ).getValue( type );
                assert( status );

                tlMatrix mat;
                tlPoint scale;
                TriggerVolumeNode::GetScaleAndMatrix( sources[i].node(), mat, scale );

                switch ( type )
                {
                case TriggerVolumeNode::RECTANGLE:
                    {
                        RectTriggerVolume* trigVol = new RectTriggerVolume( mat.GetRow( 3 ), 
                                                   mat.GetRow( 0 ), 
                                                   mat.GetRow( 1 ), 
                                                   mat.GetRow( 2 ), 
                                                   scale.x,
                                                   scale.y,
                                                   scale.z ); 
                        
                        inside = trigVol->Contains( position );

                        trigVol->Release();
                    }
                    break;
                case TriggerVolumeNode::SPHERE:
                    {
                        SphereTriggerVolume* trigVol = new SphereTriggerVolume( mat.GetRow( 3 ), scale.x );

                        inside = trigVol->Contains( position );

                        trigVol->Release();
                    }
                    break;
                default:
                    assert( false );
                }
                
                if ( inside )
                {
                    float fov;
                    float time;
                    float rate;
                    fnNode.findPlug( FOVLocatorNode::sFOV ).getValue( fov );
                    fnNode.findPlug( FOVLocatorNode::sTime ).getValue( time );
                    fnNode.findPlug( FOVLocatorNode::sRate ).getValue( rate );

                    cam->SetFOVOverride( rmt::DegToRadian( fov ) );
                    cam->OverrideFOV( true, time, rate );

                    break;
                }
            }
        }

        dagIt.next();
    }

    if ( !inside )
    {
        cam->OverrideFOV( false, 2000, 0.04f );
    }
}
