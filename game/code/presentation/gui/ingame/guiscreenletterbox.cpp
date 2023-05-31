//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenLetterBox
//
// Description: Implementation of the CGuiScreenLetterBox class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/08/21      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/ingame/guiscreenletterbox.h>

#include <events/eventmanager.h>
#include <input/inputmanager.h>
#include <interiors/interiormanager.h>
#include <mission/missionmanager.h>
#include <mission/missionstage.h>
#include <mission/objectives/missionobjective.h>

#include <raddebug.hpp> // Foundation
#include <group.h>
#include <page.h>
#include <pure3dobject.h>
#include <screen.h>
//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

const float BAR_SLIDING_TIME = 500.0f; // in msec
bool CGuiScreenLetterBox::m_enableReopen = false;
bool CGuiScreenLetterBox::m_forceOpen = false;
static bool g_SurpressNextSkipButton  = false;
GuiSFX::Translator   g_TopIn( "Top In" );
GuiSFX::Translator   g_BottomIn( "Bottom In" );
GuiSFX::Translator   g_TopOut( "Top Out" );
GuiSFX::Translator   g_BottomOut( "Bottom Out" );
GuiSFX::Translator   g_TopClose( "Top Close" );
GuiSFX::Translator   g_BottomClose( "Outro bottom close" );
GuiSFX::SendEvent    g_ClosedEvent( "Outro closed event" );
GuiSFX::Pause        s_OutroPauseBetweenCloseAndIris( "Outro pause between close and open" );
GuiSFX::IrisWipeOpen s_OutroIrisOpen( "Outro Iris Open" );
bool CGuiScreenLetterBox::s_suppressAcceptCancelButtons = false;

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenLetterBox::CGuiScreenLetterBox
//===========================================================================
// Description: Constructor.
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
CGuiScreenLetterBox::CGuiScreenLetterBox
(
    Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:	CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_LETTER_BOX ),
    m_topBar( NULL ),
    m_bottomBar( NULL ),
    m_skipButton( NULL ),
    m_skipLabel( NULL ),
    m_elapsedTime( 0 ),
    m_OutroPending( false )
{
    // Retrieve the Scrooby drawing elements.
    //
    m_Page = m_pScroobyScreen->GetPage( "LetterBox" );	rAssert( m_Page != NULL );
    m_IrisPage = m_pScroobyScreen->GetPage( "3dIris" ); rAssert( m_IrisPage != NULL );
    m_Iris = m_IrisPage->GetPure3dObject( "p3d_iris" );

    // get the top and bottom letterbox bars
    //
    m_topBar = m_Page->GetGroup( "TopBar" );
    m_bottomBar = m_Page->GetGroup( "BottomBar" );

    // get letter box buttons from 'LetterBoxButtons.pag'
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "LetterBoxButtons" );
    rAssert( pPage != NULL );

    m_skipButton = pPage->GetGroup( "Skip" );
    rAssert( m_skipButton != NULL );

    m_skipLabel = m_skipButton->GetGroup( "SkipLabel" );
    m_buttonIcons[ BUTTON_ICON_ACCEPT ] = m_skipButton->GetGroup( "AcceptLabel" );
    m_buttonIcons[ BUTTON_ICON_BACK ] = m_skipButton->GetGroup( "BackLabel" );

    if( this->IsWideScreenDisplay() )
    {
        m_skipButton->ResetTransformation();
        this->ApplyWideScreenCorrectionScale( m_skipButton );
    }

    AddTransition( g_TopIn         );
    AddTransition( g_BottomIn      );
    AddTransition( g_TopOut        );
    AddTransition( g_BottomOut     );
    AddTransition( g_TopClose      );
    AddTransition( g_BottomClose   );
    AddTransition( s_OutroIrisOpen );
    AddTransition( s_OutroPauseBetweenCloseAndIris );
    DoneAddingTransitions();

    g_TopIn.SetDrawable( m_topBar );
    g_BottomIn.SetDrawable( m_bottomBar );
    g_TopOut.SetDrawable( m_topBar );
    g_BottomOut.SetDrawable( m_bottomBar );
    g_TopClose.SetDrawable( m_topBar );
    g_BottomClose.SetDrawable( m_bottomBar );
    g_ClosedEvent.SetEvent( EVENT_LETTERBOX_CLOSED );

    SetIntroFromOpen();
    g_TopOut.SetCoordsStart(  0,  240 - 70 );
    g_TopOut.SetCoordsEnd(    0,  240      );
    g_TopOut.SetTimeInterval( BAR_SLIDING_TIME );
    g_BottomOut.SetCoordsStart( 0, -240 + 70 );
    g_BottomOut.SetCoordsEnd(   0, -240      );
    g_BottomOut.SetTimeInterval( BAR_SLIDING_TIME );
    g_TopClose.SetCoordsStart( 0, 240 - 70 );
    g_TopClose.SetCoordsEnd( 0, 0 );
    g_TopClose.SetTimeInterval( BAR_SLIDING_TIME );
    g_BottomClose.SetCoordsStart( 0, -240 + 70 );
    g_BottomClose.SetCoordsEnd( 0, 0 );
    g_BottomClose.SetTimeInterval( BAR_SLIDING_TIME );
    s_OutroPauseBetweenCloseAndIris.SetTimeInterval( 500 );
    s_OutroIrisShow.SetDrawable( m_Iris );
    s_OutroLetterBoxHide.SetDrawable( m_Page );

    g_BottomClose.SetNextTransition( s_OutroPauseBetweenCloseAndIris );
    s_OutroPauseBetweenCloseAndIris.SetNextTransition( s_OutroIrisShow );
    s_OutroIrisShow.SetNextTransition( s_OutroLetterBoxHide );
    s_OutroLetterBoxHide.SetNextTransition( g_ClosedEvent );
    g_ClosedEvent.SetNextTransition( s_OutroIrisOpen );
}


//===========================================================================
// CGuiScreenLetterBox::~CGuiScreenLetterBox
//===========================================================================
// Description: Destructor.
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
CGuiScreenLetterBox::~CGuiScreenLetterBox()
{
}

//===========================================================================
// CGuiScreenLetterBox::CheckIfScreenShouldBeBlank
//===========================================================================
// Description: this is a huge hack for the one frame glitch you get when 
//              switching screens sometimes
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenLetterBox::CheckIfScreenShouldBeBlank()
{
    bool irisClosed = GetGameplayManager()->IsIrisClosed();
    if( irisClosed )
    {
        m_topBar->ResetTransformation();
        m_topBar->Translate( 0, 0 );
        m_bottomBar->ResetTransformation();
        m_bottomBar->Translate( 0, 0 );
    }
    else
    {
        m_topBar->ResetTransformation();
        m_topBar->Translate( 0, 240 );
        m_bottomBar->ResetTransformation();
        m_bottomBar->Translate( 0, -240 );
    }
}

//===========================================================================
// CGuiScreenLetterBox::ForceOpen
//===========================================================================
// Description: forces the screen to open next time
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenLetterBox::ForceOpen()
{
    m_forceOpen = true;
}

//===========================================================================
// CGuiScreenLetterBox::HandleMessage
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenLetterBox::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2 
)
{
    if( message == GUI_MSG_UPDATE )
    {
        ResetMovableObjects();
        float deltaT = static_cast< float >( param1 );
        UpdateTransitions( deltaT );

#ifdef RAD_DEMO
        // reset idle timer when we're in a conversation
        //
        GetGameplayManager()->ResetIdleTime();
#endif
    }

    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        switch( message )
        {
            case GUI_MSG_CONTROLLER_SELECT:
            {
                if( m_skipButton->IsVisible() )
                {
                    if( this->IsButtonVisible( BUTTON_ICON_ACCEPT ) )
                    {
                        this->OnAccept();
                    }
                    else
                    {
                        this->OnSkip();
                    }
                }

                break;
            }
            case GUI_MSG_CONTROLLER_BACK:
            {
                if( m_skipButton->IsVisible() && this->IsButtonVisible( BUTTON_ICON_BACK ) )
                {
                    this->OnCancel();
                }

                break;
            }
            default:
            {
                break;
            }
        }
    }
    else if( m_state == GUI_WINDOW_STATE_INTRO )
    {
        switch( message )
        {
            case GUI_MSG_UPDATE:
            {
                if( g_TopIn.IsChainDone() )
                {
                    --m_numTransitionsPending;
                }
                break;
            }

            default:
            {
                break;
            }
        }
    }
    else if( m_state == GUI_WINDOW_STATE_OUTRO )
    {
        switch( message )
        {
            case GUI_MSG_UPDATE:
            {
                if( m_enableReopen )
                {
                }
                else
                {
                    if( g_BottomClose.IsChainDone() || g_BottomOut.IsChainDone() )
                    {
                        --m_numTransitionsPending;
                    }
                }

                break;
            }

            default:
            {
                break;
            }
        }
    }

	// Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );
}

void
CGuiScreenLetterBox::HandleEvent( EventEnum id, void* pEventData )
{
    rAssert( id == EVENT_CONVERSATION_DONE );

    GetEventManager()->TriggerEvent( EVENT_CONVERSATION_DONE_AND_FINISHED );
}


//===========================================================================
// CGuiScreenLetterBox::InitIntro
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenLetterBox::InitIntro()
{
    s_OutroIrisOpen.Reset();

    m_Page->SetVisible( true );
    m_Iris->SetVisible( false );
    m_topBar->SetVisible( true );
    m_bottomBar->SetVisible( true );
    ResetTransitions();
    m_topBar->Translate( 0, 240 );
    m_bottomBar->Translate( 0, -240 );
    g_TopIn.Activate();
    g_BottomIn.Activate();
    m_elapsedTime = 0;
    m_numTransitionsPending++;
    m_OutroPending = false;

    //
    // Is the screen already black from some sort of fade?
    //
    bool irisClosed = GetGameplayManager()->IsIrisClosed();
    if( irisClosed )
    {
        SetIntroFromClosed();
    }
    else
    {
        SetIntroFromOpen();
    }


    rAssert( m_skipButton != NULL );
    if( g_SurpressNextSkipButton )
    {
        m_skipButton->SetVisible( false );
    }
    else
    {
        m_skipButton->SetVisible( true );

        MissionStage* currentStage = GetGameplayManager()->GetCurrentMission()->GetCurrentStage();
        rAssert( currentStage != NULL );
        bool showAcceptCancel = GetGameplayManager()->IsSundayDrive() &&
                                currentStage->IsMissionAbortAllowed() &&
                                !GetInteriorManager()->IsPlayingISMovieDialog();

        if( s_suppressAcceptCancelButtons )
        {
            showAcceptCancel = false;
        }

        rAssert( m_skipLabel != NULL );
        m_skipLabel->SetVisible( !showAcceptCancel );
        this->SetButtonVisible( BUTTON_ICON_ACCEPT, showAcceptCancel );
        this->SetButtonVisible( BUTTON_ICON_BACK, showAcceptCancel );
    }

    s_suppressAcceptCancelButtons = false; // reset this flag

    GetEventManager()->AddListener( this, EVENT_CONVERSATION_DONE );

    //
    // Double check if we were doing a "fade in" beforehand, and abort it
    //
    bool fadeInProgress = GetGameplayManager()->FadeInProgress();
    GetGameplayManager()->AbortFade();
}


//===========================================================================
// CGuiScreenLetterBox::InitRunning
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenLetterBox::InitRunning()
{
}

//===========================================================================
// CGuiScreenLetterBox::InitOutro
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenLetterBox::InitOutro()
{ 
    if( m_OutroPending )
    {
        return;
    }
    m_OutroPending = true;
    g_TopIn.Deactivate();
    g_BottomIn.Deactivate();
    bool sundayDrive   = GetGameplayManager()->IsSundayDrive();
    int  numStages     = GetGameplayManager()->GetCurrentMission()->GetNumStages();
    int  currentIndex  = GetGameplayManager()->GetCurrentMission()->GetCurrentStageIndex();
    Mission* mission   = GetGameplayManager()->GetCurrentMission();
    MissionStage* stage = mission->GetCurrentStage();
    MissionObjective* obj = stage->GetObjective();
    bool pattyAndSelma = obj->IsPattyAndSelmaDialog();
    bool raceDialog    = obj->IsRaceDialog();
    
    g_BottomClose.SetNextTransition( NULL );

    //
    // Sometimes we close the letterbox, sometimes it just opens up. when does
    // it do what? If we're going to a hud screen, then it closes first, if 
    // we're staying in gameplay, then it just opens up
    //
    if( m_forceOpen )
    {
        g_TopOut.Activate();
        g_BottomOut.Activate();
        m_forceOpen = false;
    }
    else
    {
        if( ( sundayDrive && (currentIndex >= numStages - 1) ) || pattyAndSelma || raceDialog )
        {
            g_TopClose.Activate();
            g_BottomClose.Activate();
        }
        else
        {
            g_TopOut.Activate();
            g_BottomOut.Activate();
        }
    }

    rAssert( m_skipButton != NULL );
    m_skipButton->SetVisible( false );

    m_elapsedTime = 0;
    m_numTransitionsPending++;
    g_SurpressNextSkipButton = false;

    GetEventManager()->RemoveListener( this, EVENT_CONVERSATION_DONE );
}

//===========================================================================
// CGuiScreenLetterBox::SurpressSkipButton
//===========================================================================
// Description: Surpresses showing
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenLetterBox::SurpressSkipButton()
{
    g_SurpressNextSkipButton = true;
}

//===========================================================================
// CGuiScreenLetterBox::UnSurpressSkipButton
//===========================================================================
// Description: The skip button will be visible again
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenLetterBox::UnSurpressSkipButton()
{
    g_SurpressNextSkipButton = false;
}

//===========================================================================
// CGuiScreenLetterBox::OnAccept
//===========================================================================
// Description: If necessary, stop conversation.  Then, start mission.
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenLetterBox::OnAccept()
{
    GetEventManager()->TriggerEvent( EVENT_FE_MENU_SELECT ); // sound effect

    // skip conversation in case it's still playing
    //
    GetEventManager()->TriggerEvent( EVENT_CONVERSATION_SKIP );
/*
    // send event to dialog objective to indicate that user has accepted
    // the mission
    //
    GetEventManager()->TriggerEvent( EVENT_CONVERSATION_DONE_AND_FINISHED );
*/
}

//===========================================================================
// CGuiScreenLetterBox::OnCancel
//===========================================================================
// Description: If necessary, stop conversation.  Then, return to sunday
//              drive.
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenLetterBox::OnCancel()
{
    GetEventManager()->TriggerEvent( EVENT_FE_MENU_BACK ); // sound effect

    // don't listen to this event on cancelling
    //
    GetEventManager()->RemoveListener( this, EVENT_CONVERSATION_DONE );

    // skip conversation in case it's still playing
    //
    GetEventManager()->TriggerEvent( EVENT_CONVERSATION_SKIP );

    if( GetGameplayManager()->IsBonusMissionDesired() )
    {
        // cancel bonus mission
        //
        GetGameplayManager()->CancelBonusMission();
    }
    else
    {
        // go back a stage in sunday drive mode
        //
        GetGameplayManager()->GetCurrentMission()->PrevStage();
        GetGameplayManager()->GetCurrentMission()->GetCurrentStage()->Start();
    }

    this->ForceOpen();

    GetInputManager()->SetGameState(Input::ACTIVE_GAMEPLAY);
    m_pParent->HandleMessage( GUI_MSG_RESUME_INGAME );
}

//===========================================================================
// CGuiScreenLetterBox::OnSkip
//===========================================================================
// Description: Skip NIS conversation.
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenLetterBox::OnSkip()
{
    GetEventManager()->TriggerEvent( EVENT_FE_MENU_SELECT ); // sound effect

    // skip conversation in case it's still playing
    //
    GetEventManager()->TriggerEvent( EVENT_CONVERSATION_SKIP );
}

//===========================================================================
// CGuiScreenLetterBox::SetIntroFromClosed
//===========================================================================
// Description: sets things up so the letterbox opens from the start
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenLetterBox::SetIntroFromClosed()
{
    g_TopIn.SetCoordsStart(  0,  0      );
    g_TopIn.SetCoordsEnd(    0,  240 - 70 );
    g_TopIn.SetTimeInterval( BAR_SLIDING_TIME );
    g_BottomIn.SetCoordsStart( 0, 0      );
    g_BottomIn.SetCoordsEnd(   0, -240 + 70 );
    g_BottomIn.SetTimeInterval( BAR_SLIDING_TIME );
    m_topBar->ResetTransformation();
    m_topBar->Translate( 0, 0 );
    m_bottomBar->ResetTransformation();
    m_bottomBar->Translate( 0, 0 );
}

//===========================================================================
// CGuiScreenLetterBox::SetIntroFromOpen
//===========================================================================
// Description: sets things up so the letterbox closes a little from the start
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenLetterBox::SetIntroFromOpen()
{
    g_TopIn.SetCoordsStart(  0,  240      );
    g_TopIn.SetCoordsEnd(    0,  240 - 70 );
    g_TopIn.SetTimeInterval( BAR_SLIDING_TIME );
    g_BottomIn.SetCoordsStart( 0, -240      );
    g_BottomIn.SetCoordsEnd(   0, -240 + 70 );
    g_BottomIn.SetTimeInterval( BAR_SLIDING_TIME );
}

#ifdef DEBUGWATCH
const char* CGuiScreenLetterBox::GetWatcherName() const
{
    return "CGuiScreenLetterBox";
}
#endif