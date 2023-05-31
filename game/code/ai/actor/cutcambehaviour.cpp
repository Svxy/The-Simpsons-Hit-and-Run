//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   cutcambehaviour
//
// Description: While inside a trigger volume, the actor will switch to its POV
//              
//
// Authors:     Michael Riegger
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <ai/actor/cutcambehaviour.h>
#include <ai/actor/actor.h>
#include <events/eventmanager.h>
#include <worldsim/avatarmanager.h>
#include <camera/supercammanager.h>
#include <camera/supercam.h>
#include <camera/surveillancecam.h>
#include <memory/srrmemory.h>


//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================




//===========================================================================
// Member Functions
//===========================================================================


CutCamBehaviour::CutCamBehaviour( float radius ):
m_TriggerRadiusSqr( radius * radius ),
m_SwitchingEnabled( true ),
m_CutCamViewActivated( false),
m_SuperCamID( UINT_MAX ),
m_Registered( false ),
m_ParentActor( NULL )
{
    HeapMgr()->PushHeap( GMA_LEVEL_OTHER );

    SetExclusive( true );
    GetEventManager()->AddListener( this, EVENT_VEHICLE_DESTROYED );
    GetEventManager()->AddListener( this, EVENT_OBJECT_DESTROYED );
    GetEventManager()->AddListener( this, EVENT_BIG_CRASH );
    GetEventManager()->AddListener( this, EVENT_BIG_VEHICLE_CRASH );
    GetEventManager()->AddListener( this, EVENT_BIG_AIR );
    GetEventManager()->AddListener( this, EVENT_PEDESTRIAN_DODGE );
    GetEventManager()->AddListener( this, EVENT_PEDESTRIAN_SMACKDOWN );
    GetEventManager()->AddListener( this, EVENT_BREAK_CAMERA_OR_BOX );
    GetEventManager()->AddListener( this, EVENT_GAG_END );
    
    

    // Allocate a camera
    mpCamera = new SurveillanceCam;
    mpCamera->AddRef();

    HeapMgr()->PopHeap(GMA_LEVEL_OTHER);
}

CutCamBehaviour::~CutCamBehaviour()
{
    GetEventManager()->RemoveListener( this, EVENT_VEHICLE_DESTROYED );
    GetEventManager()->RemoveListener( this, EVENT_OBJECT_DESTROYED );
    GetEventManager()->RemoveListener( this, EVENT_BIG_CRASH );
    GetEventManager()->RemoveListener( this, EVENT_BIG_VEHICLE_CRASH );
    GetEventManager()->RemoveListener( this, EVENT_BIG_AIR );
    GetEventManager()->RemoveListener( this, EVENT_PEDESTRIAN_DODGE );
    GetEventManager()->RemoveListener( this, EVENT_PEDESTRIAN_SMACKDOWN );
    GetEventManager()->RemoveListener( this, EVENT_BREAK_CAMERA_OR_BOX );
    GetEventManager()->RemoveListener( this, EVENT_GAG_END );
    if ( m_SuperCamID < UINT_MAX )
    {
        rAssert( m_SuperCamID < UINT_MAX );
        SuperCamCentral* scc = GetSuperCamManager()->GetSCC( 0 );
        if ( scc != NULL )
        {
            scc->UnregisterSuperCam( m_SuperCamID );
        }
    }

    mpCamera->ReleaseVerified();
}

void
CutCamBehaviour::Apply( Actor* actor, unsigned int timeInMS )
{
    m_ParentActor = actor;

    // If we can switch to the camera, we can't do anything else, set the 
    // exclusive flag accordingly (we can't change behaviours)
    SetExclusive( m_SwitchingEnabled );
    if ( m_SwitchingEnabled )
    {
        // Check the range to the actor, if within range. Switch to its POV
        // Get Avatar position
        Avatar* currAvatar = GetAvatarManager()->GetAvatarForPlayer( 0 );
        rmt::Vector currAvatarPos;     
        currAvatar->GetPosition( currAvatarPos );
        currAvatarPos.y += 0.5f;
        actor->LookAt( currAvatarPos, timeInMS );
        // Get Actor position
        rmt::Vector actorPosition;
        actor->GetPosition( &actorPosition );
        // Compute distanceSqr
        float distanceToAvatarSqr = (actorPosition - currAvatarPos).MagnitudeSqr();
        if ( distanceToAvatarSqr < m_TriggerRadiusSqr )
        {
            if ( m_CutCamViewActivated == false )
            {

                SwitchToCutCam();
                m_CutCamViewActivated = true;
            }
            else
            {
                // Turn off rendering of the object, we don't want to see it from the inside
//                actor->SetVisibility( false );
            }
            mpCamera->SetPosition( actorPosition );
        }    
        else
        {
            if ( m_CutCamViewActivated )
            {
//                actor->SetVisibility( true );
                RevertCamera();
                m_CutCamViewActivated = false;
            }
        }
    }   
}


void 
CutCamBehaviour::HandleEvent( EventEnum id, void* pEventData )
{
    switch ( id )
    {
    case EVENT_VEHICLE_DESTROYED:
    case EVENT_OBJECT_DESTROYED:
    case EVENT_BIG_CRASH:
    case EVENT_BIG_VEHICLE_CRASH:
    case EVENT_BIG_AIR:
    case EVENT_PEDESTRIAN_DODGE:
    case EVENT_PEDESTRIAN_SMACKDOWN:
    case EVENT_BREAK_CAMERA_OR_BOX:
    case EVENT_GAG_END:
        // Kill this behaviour
        m_SwitchingEnabled = false;
        // Are we in cutcam mode, get out of it.
        if ( m_CutCamViewActivated )
        {
            RevertCamera();
            m_CutCamViewActivated = false;
        }
        break;
    default:
        rAssert( false );
        break;
    };
}

void CutCamBehaviour::Activate()
{
    
}

void CutCamBehaviour::Deactivate()
{
    if ( m_CutCamViewActivated )
    {
        if ( m_ParentActor )
        {
//            m_ParentActor->SetVisibility( true );
        }
        RevertCamera();
        m_CutCamViewActivated = false;
    }    
}


void 
CutCamBehaviour::SwitchToCutCam()
{
    SuperCamCentral* superCamCentral = GetSuperCamManager()->GetSCC( 0 );
    rAssert( superCamCentral );

    if ( !m_Registered )
    {
        // Register the supercam with the supercamcentral and store the ID returned so
        // we can reference it again
        m_SuperCamID = superCamCentral->RegisterSuperCam( mpCamera );
        m_Registered = true;
    }

    // Finally, switch to our new supercam
    superCamCentral->SelectSuperCam( m_SuperCamID );     
}
 
void 
CutCamBehaviour::RevertCamera()
{
    if ( m_Registered )
    {
        // Tell  the SuperCamCentral that we don't need this camera anymore
        SuperCamCentral* superCamCentral = GetSuperCamManager()->GetSCC( 0 );
        rAssert( superCamCentral );

        superCamCentral->UnregisterSuperCam( m_SuperCamID );
        m_Registered = false;
    }

    // reset our current supercamcentral ID 
    m_SuperCamID = UINT_MAX;    
}



