//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        AnimatedCam.cpp
//
// Description: Implement AnimatedCam
//
// History:     24/04/2002 + Created -- Cary Brisebois, based from the Bumper Camera
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <camera/animatedcam.h>
#include <camera/supercammanager.h>
#include <events/eventmanager.h>
#include <input/inputmanager.h>
#include <p3d/anim/multicontroller.hpp>
#include <p3d/camera.hpp>
#include <p3d/utility.hpp>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenletterbox.h>
#include <presentation/gui/ingame/guiscreenmultihud.h>
#include <presentation/gui/guimanager.h>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/guiwindow.h>
#include <interiors/interiormanager.h>

//========================================
// Definitions
//========================================

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************
#define SKIPPING_DEFAULT false
static tName               g_CameraName;
static tName               g_MulticontrollerName;
static tName               g_MissionStartCameraName;
static tName               g_MissionStartMulticontrollerName;
static tCamera*            g_Camera = NULL;
static tMultiController*   g_Multicontroller = NULL;
static bool                g_CameraSwitchPending = false;
static bool                g_TriggeredNextCamera = false;
static int                 g_InstanceCount = 0;
static int                 g_CameraTransitionFlags = 0;
static bool                g_AllowSkipping = SKIPPING_DEFAULT;
static bool                g_SurpressNextLetterbox = false;

const int DEFAULT_TRANSITION_FLAGS = SuperCamCentral::QUICK | SuperCamCentral::FORCE;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************


void AnimatedCam::Abort()
{
    if( g_Multicontroller != NULL )
    {
        int flags = DEFAULT_TRANSITION_FLAGS;

        if ( 0 != g_CameraTransitionFlags )
        {
            //Override the flags.
            flags = g_CameraTransitionFlags;
        }

        rAssert( m_NextCameraType != NUM_TYPES );
        GetSuperCamManager()->GetSCC( 0 )->SelectSuperCam( m_NextCameraType, flags, 3000 );
        g_TriggeredNextCamera = true;

        //Reset the flags override.
        g_CameraTransitionFlags = 0;

        tRefCounted::Release( g_Multicontroller );
        tRefCounted::Release( g_Camera );
        AllowSkipping( SKIPPING_DEFAULT );
    }
}


//==============================================================================
// AnimatedCam::AnimatedCam
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
AnimatedCam::AnimatedCam():
    m_NextCameraType( INVALID )
{
    ++g_InstanceCount;
}

//==============================================================================
// AnimatedCam::~AnimatedCam
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
AnimatedCam::~AnimatedCam()
{
    --g_InstanceCount;
    if( g_InstanceCount == 0 )
    {

        tRefCounted::Release( g_Multicontroller );
        tRefCounted::Release( g_Camera );
        g_CameraName.SetText(NULL);
        g_MulticontrollerName.SetText(NULL);
        g_MissionStartCameraName.SetText(NULL);
        g_MissionStartMulticontrollerName.SetText(NULL);
        g_TriggeredNextCamera = false;
    }
}

//==============================================================================
// AnimatedCam::AllowSkipping
//==============================================================================
// Description: should the animated camera allow the user to skip it with a 
//              button press
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
void AnimatedCam::AllowSkipping( const bool skippingAllowed )
{
    g_AllowSkipping = skippingAllowed;
}

//==============================================================================
// AnimatedCam::CameraSwitchPending
//==============================================================================
// Description: is there a camera switch pending?
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
bool AnimatedCam::CameraSwitchPending()
{
    return g_CameraSwitchPending;
}

//==============================================================================
// AnimatedCam::CheckPendingCameraSwitch
//==============================================================================
// Description: checks if we've queued up a camera switch to the animated camera
//              because the camera system isn't always set up in time.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
void AnimatedCam::CheckPendingCameraSwitch()
{
    if( CameraSwitchPending() )
    {
        LookupCamera();
        LookupMulticontroller();
        SuperCamManager* scm = GetSuperCamManager();
        SuperCamCentral* scc = scm->GetSCC( 0 );
        SuperCam*        sc  = scc->GetActiveSuperCam();
        if( sc == NULL )
        {
            GetSuperCamManager()->GetSCC( 0 )->SelectSuperCam( ANIMATED_CAM );
        }
        else
        {
            SuperCam::Type type  = sc->GetType();
            if( type != ANIMATED_CAM )
            {
                GetSuperCamManager()->GetSCC( 0 )->SelectSuperCam( ANIMATED_CAM );
            }
        }
        SetCameraSwitchPending( false );
    }
}

//=============================================================================
// AnimatedCam::ClearCamera
//=============================================================================
// Description: clears out the animated camera
//
// Parameters:  none
//
// Return:      none
//
//=============================================================================
void AnimatedCam::ClearCamera()
{
    tCamera* pCamera = NULL;
    tMultiController* pMultiController = NULL;

    tRefCounted::Assign( g_Camera, pCamera);
    tRefCounted::Assign( g_Multicontroller,pMultiController );

    
}

//=============================================================================
// AnimatedCam::GetName
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const 
//
//=============================================================================
const char* const AnimatedCam::GetName() const
{
    return "ANIMATED_CAM";
}

//=============================================================================
// AnimatedCam::GetType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      Type 
//
//=============================================================================
SuperCam::Type AnimatedCam::GetType()
{
    return ANIMATED_CAM;
}

//=============================================================================
// AnimatedCam::GetWatcherName
//=============================================================================
// Description: the name of the class for the watcher or other debug purposes
//
// Parameters:  NONE
//
// Return:      const char* - the name of the class 
//
//=============================================================================
#ifdef DEBUGWATCH
const char* AnimatedCam::GetWatcherName() const
{
    return "AnimatedCam";
}
#endif


//=============================================================================
// AnimatedCam::LetterBoxStart
//=============================================================================
// Description: start the letterbox for this camera
//
// Parameters:  ()
//
// Return:      Type 
//
//=============================================================================
void AnimatedCam::LetterBoxStart()
{
    if( !g_SurpressNextLetterbox )
    {
        CGuiManager* gm = GetGuiSystem()->GetCurrentManager();
        CGuiWindow::eGuiWindowID currentScreenId = gm->GetCurrentScreen();
        CGuiWindow* currentWindow = gm->FindWindowByID( currentScreenId );
        currentWindow->ForceClearTransitions();
        CGuiScreenLetterBox::SuppressAcceptCancelButtons();
        if( g_AllowSkipping )
        {
            CGuiScreenLetterBox::UnSurpressSkipButton();
        }
        else
        {
            CGuiScreenLetterBox::SurpressSkipButton();
        }
        GetGuiSystem()->GotoScreen( CGuiWindow::GUI_SCREEN_ID_LETTER_BOX, 0x00, 0x00, CLEAR_WINDOW_HISTORY );

        CGuiManagerInGame* guiIngameManager = GetGuiSystem()->GetInGameManager();
        if( guiIngameManager != NULL )
        {
            CGuiWindow* window = guiIngameManager->FindWindowByID( CGuiWindow::GUI_SCREEN_ID_LETTER_BOX );
            CGuiScreenLetterBox* letterBox = dynamic_cast< CGuiScreenLetterBox* >( window );
            letterBox->CheckIfScreenShouldBeBlank();

            window = guiIngameManager->FindWindowByID( CGuiWindow::GUI_SCREEN_ID_MULTI_HUD );
            CGuiScreenMultiHud* multiHud = dynamic_cast< CGuiScreenMultiHud* >( window );
            if( multiHud != NULL )
            {
                multiHud->ShowLetterBox();
            }
        }
    }
}

//=============================================================================
// AnimatedCam::LetterBoxStop
//=============================================================================
// Description: stop the letterbox for this camera
//
// Parameters:  ()
//
// Return:      Type 
//
//=============================================================================
void AnimatedCam::LetterBoxStop()
{
    if( !g_SurpressNextLetterbox )
    {
        CGuiScreenLetterBox::ForceOpen();
        CGuiScreenLetterBox::SuppressAcceptCancelButtons( false );
        GetGuiSystem()->GotoScreen( CGuiWindow::GUI_SCREEN_ID_HUD, 0x00, 0x00, CLEAR_WINDOW_HISTORY );
    }
    else
    {
        g_SurpressNextLetterbox = false;
    }
}

//=============================================================================
// AnimatedCam::LookupCamera
//=============================================================================
// Description: sets up the camera pointer for this animated camera
//
// Parameters:  name - tName of the camera to find in the inventory
//
// Return:      void 
//
//=============================================================================
void AnimatedCam::LookupCamera()
{
    if( g_Camera == NULL )
    {
        if( g_CameraName.GetUID() != static_cast< tUID >( 0 ) )
        {
            tCamera* found = p3d::find< tCamera >( g_CameraName.GetUID() );
            if( found != NULL )
            {
                tRefCounted::Assign( g_Camera, found );
            }
        }
    }
}

//=============================================================================
// AnimatedCam::LookupMulticontroller
//=============================================================================
// Description: finds the multicontroller in the inventory
//
// Parameters:  none
//
// Return:      void 
//
//=============================================================================
void AnimatedCam::LookupMulticontroller()
{
    if( g_Multicontroller == NULL )
    {
        if( g_MulticontrollerName.GetUID() != static_cast< tUID >( 0 ) )
        {
            tMultiController* found = p3d::find< tMultiController >( g_MulticontrollerName.GetUID() );
            if( found != NULL )
            {
                tRefCounted::Assign( g_Multicontroller, found );
            }
            Reset();
        }
    }
}

//=============================================================================
// AnimatedCam::OnInit
//=============================================================================
// Description: called when the camera is started
//
// Parameters:  NONE
//
// Return:      void 
//
//=============================================================================
void AnimatedCam::OnInit()
{
    LetterBoxStart();
    InitMyController();
    InputManager::GetInstance()->SetGameState( Input::ACTIVE_ANIM_CAM );
}

//=============================================================================
// AnimatedCam::OnShutdown
//=============================================================================
// Description: called when the camera is deactivated
//
// Parameters:  NONE
//
// Return:      void 
//
//=============================================================================
void AnimatedCam::OnShutdown()
{
    LetterBoxStop();
    InputManager::GetInstance()->SetGameState( Input::DEACTIVE_ANIM_CAM );
    GetEventManager()->TriggerEvent( EVENT_ANIMATED_CAM_SHUTDOWN );
    m_NextCameraType = INVALID;
}

//=============================================================================
// AnimatedCam::Reset
//=============================================================================
// Description: resets the camera to the start of the animation
//
// Parameters:  NONE
//
// Return:      void 
//
//=============================================================================
void AnimatedCam::Reset()
{
    if( g_Multicontroller != NULL )
    {
        g_Multicontroller->Reset();
    }
    g_TriggeredNextCamera = false;
}

//=============================================================================
// AnimatedCam::SetCamera
//=============================================================================
// Description: sets up the camera pointer for this animated camera
//
// Parameters:  name - tName of the camera to find in the inventory
//
// Return:      void 
//
//=============================================================================
void AnimatedCam::SetCamera( tName name )
{
   // tRefCounted::Release( g_Camera );
    g_CameraName = name;
    if( g_CameraName.GetUID() == static_cast< tUID >( 0 ) )
    {
        SetCameraSwitchPending( false );
    }
    else
    {
        SetCameraSwitchPending( true );
    }
}

//=============================================================================
// AnimatedCam::SetMulticontroller
//=============================================================================
// Description: sets up the multicontroller pointer for this animated camera
//
// Parameters:  name - tName of the camera to find in the inventory
//
// Return:      void 
//
//=============================================================================
void AnimatedCam::SetMulticontroller( tName name )
{
    //tRefCounted::Release( g_Multicontroller );
    g_MulticontrollerName = name;
    if( g_MulticontrollerName.GetUID() == static_cast< tUID >( 0 ) )
    {
        SetCameraSwitchPending( false );
    }
    else
    {
        SetCameraSwitchPending( true );
    }
}

//=============================================================================
// AnimatedCam::SetMissionStartCamera
//=============================================================================
// Description: sets up the camera pointer for this animated camera
//
// Parameters:  name - tName of the camera to find in the inventory
//
// Return:      void 
//
//=============================================================================
void AnimatedCam::SetMissionStartCamera( tName name )
{
    g_MissionStartCameraName = name;
}

//=============================================================================
// AnimatedCam::SetMissionStartMulticontroller
//=============================================================================
// Description: sets up the multicontroller pointer for this animated camera
//
// Parameters:  name - tName of the camera to find in the inventory
//
// Return:      void 
//
//=============================================================================
void AnimatedCam::SetMissionStartMulticontroller( tName name )
{
    g_MissionStartMulticontrollerName = name;
}


//=============================================================================
// AnimatedCam::SetNextCameraType
//=============================================================================
// Description: sets up the camera that will get used after this camera is done
//
// Parameters:  type - what kind of camera should we use
//
// Return:      void 
//
//=============================================================================
void AnimatedCam::SetNextCameraType( const SuperCam::Type type )
{
    rAssert( type != ANIMATED_CAM );
    m_NextCameraType = type;
    //m_NextCameraType = SuperCam::DEFAULT_CAM;
}

//=============================================================================
// AnimatedCam::SetTarget
//=============================================================================
// Description: sets the target (actually sets it for the next camera in the 
//              chain
//
// Parameters:  target - the target for the camera to look at
//
// Return:      void 
//
//=============================================================================
void AnimatedCam::SetTarget( ISuperCamTarget* target )
{
    //nothing
}

//=============================================================================
// AnimatedCam::Skip
//=============================================================================
// Description: skips the animated camera
//
// Parameters:  NONE
//
// Return:      void 
//
//=============================================================================
void AnimatedCam::Skip()
{
    if( g_AllowSkipping )
    {
        if( g_Multicontroller != NULL )
        {
            g_Multicontroller->Advance( 20000.0f );
        }
        AllowSkipping( SKIPPING_DEFAULT );
    }
}

//=============================================================================
// AnimatedCam::TriggerMissionStartCamera
//=============================================================================
// Description: switches to the mission start camera
//
// Parameters:  NONE
//
// Return:      void 
//
//=============================================================================
void AnimatedCam::TriggerMissionStartCamera()
{
    SetCamera( g_MissionStartCameraName );
    SetMulticontroller( g_MissionStartMulticontrollerName );
    g_MissionStartCameraName          = "";
    g_MissionStartMulticontrollerName = "";
}

//=============================================================================
// AnimatedCam::SetCameraSwitchPending
//=============================================================================
// Description: changes the setting on the camera switch flag
//
// Parameters:  pending - the new state of the flag
//
// Return:      void 
//
//=============================================================================
void AnimatedCam::SetCameraSwitchPending( const bool pending )
{
    g_CameraSwitchPending = pending;
}

//=============================================================================
// AnimatedCam::SetCameraTransitionFlags
//=============================================================================
// Description: Overrides the default camera transition flags
//
// Parameters:  ( int flags )
//
// Return:      void 
//
//=============================================================================
void AnimatedCam::SetCameraTransitionFlags( int flags )
{
    g_CameraTransitionFlags = flags;
}

//=============================================================================
// AnimatedCam::SupressNextLetterbox
//=============================================================================
// Description: won't trigger the letterbox next time
//
// Parameters:  
//
// Return:      void 
//
//=============================================================================
void AnimatedCam::SupressNextLetterbox()
{
    g_SurpressNextLetterbox = true;
}

//=============================================================================
// AnimatedCam::Update
//=============================================================================
// Description: called to animate the camera
//
// Parameters:  ( unsigned int milliseconds
//
// Return:      void 
//
//=============================================================================
void AnimatedCam::Update( unsigned int milliseconds )
{
    if( g_TriggeredNextCamera )
    {
        return;
    }
    if( g_Multicontroller != NULL )
    {
        g_Multicontroller->Advance( static_cast< float >( milliseconds ) );

        InteriorManager* im = GetInteriorManager();
        
        //This is a last chance test to make sure that we do not play cameras
        //in interiors.
        if( g_Multicontroller->LastFrameReached() || 
            (im && ( im->IsEntering() || im->IsInside() || im->IsExiting() ) ) )
        {
            //
            // The animation has looped
            //
            if( g_TriggeredNextCamera == false )
            { 
                int flags = DEFAULT_TRANSITION_FLAGS;

                if ( 0 != g_CameraTransitionFlags )
                {
                    //Override the flags.
                    flags = g_CameraTransitionFlags;
                }

                rAssert( m_NextCameraType != NUM_TYPES );
                GetSuperCamManager()->GetSCC( 0 )->SelectSuperCam( m_NextCameraType, flags, 3000 );
                g_TriggeredNextCamera = true;

                //Reset the flags override.
                g_CameraTransitionFlags = 0;

                tRefCounted::Release( g_Multicontroller );
                tRefCounted::Release( g_Camera );
                AllowSkipping( SKIPPING_DEFAULT );
            }
        }
        else
        {
            //
            // Update the camera values
            //
            rmt::Vector position;
            rmt::Vector target;
            g_Camera->GetWorldPosition( &position );
            g_Camera->GetWorldLookAtDirection( &target );

            static rmt::Vector oldPosition = position;
            static float maxDistance = 30;

            target *= 10.0f;
            target += position;
            SetCameraValues( milliseconds, position, target);
        }
    }
    else
    {
        //
        // We never found the multicontroller in the inventory
        //
        int flags = DEFAULT_TRANSITION_FLAGS;

        if ( 0 != g_CameraTransitionFlags )
        {
            //Override the flags.
            flags = g_CameraTransitionFlags;
        }

        GetSuperCamManager()->GetSCC( 0 )->SelectSuperCam( SuperCam::DEFAULT_CAM, flags, 0 );
        g_TriggeredNextCamera = true;

        //Reset the flags override.
        g_CameraTransitionFlags = 0;

        tRefCounted::Release( g_Multicontroller );
        tRefCounted::Release( g_Camera );

        //
        // Since our camera system doesn't use regular p3d cameras, i have to fake
        // the following information 
        //
        if( g_Camera == NULL )
        {
            //this is a bad bad case to ever get into
            rWarningMsg( false, "Camera could not be loaded" );
            //SetCameraValues( milliseconds, rmt::Vector( 0, 0, 0 ), rmt::Vector( 1, 0, 0 ) );
        }

    }
}
