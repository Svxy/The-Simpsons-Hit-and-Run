//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMissionLoad
//
// Description: Implementation of the CGuiScreenMissionLoad class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/07/04      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <camera/animatedcam.h>
#include <events/eventmanager.h>
#include <mission/gameplaymanager.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenmissionload.h>
#include <presentation/gui/ingame/guiscreenmissionselect.h>
#include <presentation/gui/guimenu.h>
#include <presentation/gui/utility/specialfx.h>
#include <p3d/utility.hpp>
#include <raddebug.hpp> // Foundation
#include <radmath/matrix.hpp>
#include <radmath/util.hpp>
#include <loading/loadingmanager.h>
#include <strings/unicodestring.h>

#include <screen.h>
#include <page.h>

#include <p3d/unicode.hpp>
#include <string.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================
#ifndef PAL
    // convert mission title string to all caps
    //
    #define MISSION_TITLE_ALL_CAPS
#endif

#define TRANSITION_IN_TIME 250.0f

const    UnicodeChar EMPTY_MISSION_HINT = '*';
const    float startDelayTimeMs                       = 500.0f;
const    float DARKEN_ANIMATION_DELAY_BEFORE_STARTING = startDelayTimeMs;
const    float DARKEN_ANIMATION_INTRO_TIME            = 1000.0f;
int            g_BitmapPositionX;
int            g_BitmapPositionY;
int            g_BitmapSizeX;
int            g_BitmapSizeY;
int            g_TitlePositionX;
int            g_TitlePositionY;

int            g_BitmapPositionWagerX =   0;
int            g_BitmapPositionWagerY = 200;
int            g_BitmapSizeWagerX     =  20;
int            g_BitmapSizeWagerY     =  20;
int            g_TitlePositionWagerX  = 200;
int            g_TitlePositionWagerY  = 370;

GuiSFX::Show                     g_LoadCompletedShow( "LoadCompletedShow" );
GuiSFX::ColorChange              g_LoadCompletedTransitionIn( "LoadCompletedTransitionIn" );

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenMissionLoad::CGuiScreenMissionLoad
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
CGuiScreenMissionLoad::CGuiScreenMissionLoad
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
):   
    CGuiScreenMissionBase( pScreen, pParent, GUI_SCREEN_ID_MISSION_LOAD ),
    m_LoadIsDone( true ),
    m_loadingText( NULL ),
    m_elapsedIdleTime( 0 )
{
    ExtractNormalPositions();
    Scrooby::Group* loadCompleted = GetLoadCompletedGroup();
    g_LoadCompletedShow.SetDrawable( loadCompleted );

    g_LoadCompletedTransitionIn.SetDrawable( loadCompleted );
    g_LoadCompletedTransitionIn.SetStartColour( tColour( 255, 255, 255, 0   ) );
    g_LoadCompletedTransitionIn.SetEndColour  ( tColour( 255, 255, 255, 255 ) );
    g_LoadCompletedTransitionIn.SetTimeInterval( TRANSITION_IN_TIME );
    g_LoadCompletedTransitionIn.Deactivate();
    WATCH( g_LoadCompletedTransitionIn, GetWatcherName() );

    g_LoadCompletedShow.        SetNextTransition( g_LoadCompletedTransitionIn  );
    g_LoadCompletedTransitionIn.SetNextTransition( NULL                         );

    AddTransition( g_LoadCompletedTransitionIn );

    AddListeners();

    // get loading text
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "LoadingText" );
    if( pPage != NULL )
    {
        m_loadingText = pPage->GetText( "Loading" );
        rAssert( m_loadingText != NULL );
        m_loadingText->SetVisible( false ); // hide by default
    }
}


//===========================================================================
// CGuiScreenMissionLoad::~CGuiScreenMissionLoad
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
CGuiScreenMissionLoad::~CGuiScreenMissionLoad()
{
    RemoveListeners();
    p3d::pddi->DrawSync();
}

//===========================================================================
// CGuiScreenMissionLoad::AddListeners
//===========================================================================
// Description: Adds all the listeners this class is ever going to need
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMissionLoad::AddListeners()
{
    GetEventManager()->AddListener( this, EVENT_GUI_MISSION_LOAD_COMPLETE );
}

//===========================================================================
// CGuiScreenMissionLoad::ExtractNormalPositions
//===========================================================================
// Description: the default positions of the objects are taken out of scrooby
//              this function actually goes and gets them
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMissionLoad::ExtractNormalPositions()
{
    // extract the position of the mission loading bitmap
    Scrooby::Drawable* missionStartBitmap = GetMissionStartBitmap();
    missionStartBitmap->GetOriginPosition(  g_BitmapPositionX, g_BitmapPositionY );
    missionStartBitmap->GetBoundingBoxSize( g_BitmapSizeX,     g_BitmapSizeY     );

    // extract the position of the title
    Scrooby::Text* text = GetTitleText();
    text->GetOriginPosition( g_TitlePositionX, g_TitlePositionY );
}

//===========================================================================
// CGuiScreenMissionLoad::HandleEvent
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
void
CGuiScreenMissionLoad::HandleEvent( EventEnum id, void* pEventData )
{
    if( id == EVENT_GUI_MISSION_LOAD_COMPLETE )
    {
        // mission load completed, show "Press Start" message
        //
        m_LoadIsDone = true;
        g_LoadCompletedShow.Activate();

        Mission* currentMission = GetGameplayManager()->GetCurrentMission();
        rAssert( currentMission != NULL );
        if( currentMission->IsWagerMission() )
        {
            this->UpdateGamblingInfo();
        }

        // hide loading text
        //
        if( m_loadingText != NULL )
        {
            m_loadingText->SetVisible( false );
        }
    }
}

//===========================================================================
// CGuiScreenMissionSuccess::HandleMessage
//===========================================================================
// Description: Handles messages, and passes them up the hierarchy when done
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMissionLoad::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2 
)
{
    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        switch( message )
        {
            case GUI_MSG_UPDATE:
            {
                m_elapsedIdleTime += param1;

                if( m_loadingText != NULL && !m_LoadIsDone )
                {
                    // blink loading text if idling here on this screen to satisfy
                    // TRC/TCR requirements
                    //
                    const unsigned int BLINKING_PERIOD = 250;
                    bool isBlinked = GuiSFX::Blink( m_loadingText,
                                                    static_cast<float>( m_elapsedIdleTime ),
                                                    static_cast<float>( BLINKING_PERIOD ) );
                    if( isBlinked )
                    {
                        m_elapsedIdleTime %= BLINKING_PERIOD;
                    }
                }

                break;
            }
            case GUI_MSG_CONTROLLER_SELECT:
            {
                if( m_LoadIsDone && this->IsButtonVisible( BUTTON_ICON_ACCEPT ) )
                {
                    SetPlayAnimatedCamera( true );

                    // resume game and start mission
                    //
                    m_pParent->HandleMessage( GUI_MSG_RESUME_INGAME );

                    // trigger this event ONLY for wager race missions
                    //
                    Mission* currentMission = GetGameplayManager()->GetCurrentMission();
                    rAssert( currentMission != NULL );
                    if( currentMission->IsWagerMission() )
                    {
                        GetEventManager()->TriggerEvent( EVENT_ATTEMPT_TO_ENTER_GAMBLERACE );
                    }

                    GetEventManager()->TriggerEvent( EVENT_FE_CONTINUE );
                    GetEventManager()->TriggerEvent( EVENT_MISSION_BRIEFING_ACCEPTED );
                }
                break;
            }
            case GUI_MSG_CONTROLLER_BACK:
            {
                if( m_LoadIsDone && this->IsButtonVisible( BUTTON_ICON_BACK ) )
                {
                    // resume game and abort mission
                    //
                    SetPlayAnimatedCamera( false );
                    AnimatedCam::SetCamera( "" );
                    m_pParent->HandleMessage( GUI_MSG_RESUME_INGAME, ON_HUD_ENTER_ABORT_MISSION );
                    
                    GetEventManager()->TriggerEvent(EVENT_USER_CANCEL_MISSION_BRIEFING);
                    GetEventManager()->TriggerEvent( EVENT_FE_CANCEL );
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }
    CGuiScreenMissionBase::HandleMessage( message, param1, param2 );
}

//===========================================================================
// CGuiScreenMissionLoad::InitializePermanentVariables
//===========================================================================
// Description: Stuff that goes into permanent memory gets initialized in 
//              here
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMissionLoad::InitializePermanentVariables()
{
    p3d::inventory->AddSection( "DynamicHud" );
}

//===========================================================================
// CGuiScreenMissionLoad::InitIntro
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
void CGuiScreenMissionLoad::InitIntro()
{
    CGuiScreenMissionBase::InitIntro();

    Scrooby::Text* missionTitle = GetTitleText();
    rAssert( missionTitle != NULL );

#ifdef MISSION_TITLE_ALL_CAPS
    // convert mission title to all caps
    //
    HeapMgr()->PushHeap( GMA_LEVEL_HUD );

    UnicodeString unicodeString;
    unicodeString.ReadUnicode( missionTitle->GetStringBuffer() );
    p3d::UnicodeStrUpr( unicodeString.GetBuffer() );

    missionTitle->SetString( missionTitle->GetIndex(), unicodeString );

    HeapMgr()->PopHeap( GMA_LEVEL_HUD );
#endif // MISSION_TITLE_ALL_CAPS

    //
    // Turn on the titles
    //
    missionTitle->SetVisible( true );
    GetMissionInfoText()->SetVisible( true );

    Scrooby::Group* loadCompleted = GetLoadCompletedGroup();
    loadCompleted->SetVisible( false );
    m_LoadIsDone = false;

    if( m_loadingText != NULL )
    {
        // hide loading text
        //
        m_loadingText->SetVisible( false );

        this->SetAlphaForLayers( 1.0f, m_foregroundLayers, m_numForegroundLayers );
    }

    m_elapsedIdleTime = 0;

    //
    // is this a normal mission or a wager mission
    //
    Mission* currentMission = GetGameplayManager()->GetCurrentMission();
    rAssert( currentMission != NULL );
    if( currentMission->IsWagerMission() )
    {
        InitPositionsWager();
    }
    else
    {
        InitPositionsNormal();
    }

    // only show "cancel" button for wager missions
    //
    this->SetButtonVisible( BUTTON_ICON_BACK, currentMission->IsWagerMission() );
}

//===========================================================================
// CGuiScreenMissionLoad::InitOutro
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
void CGuiScreenMissionLoad::InitOutro()
{
    CGuiScreenMissionBase::InitOutro();
    rWarning( m_LoadIsDone );
    m_LoadIsDone = false;

    if( m_loadingText != NULL )
    {
        // hide loading text
        //
        m_loadingText->SetVisible( false );
    }
}

//===========================================================================
// CGuiScreenMissionLoad::InitPositionsNormal
//===========================================================================
// Description: Move scroobyelements to where they need to be for normal
//              missions
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMissionLoad::InitPositionsNormal()
{
    Scrooby::BoundedDrawable* missionStart = GetMissionStartBitmap();
    missionStart->SetPosition       ( g_BitmapPositionX, g_BitmapPositionY );
    missionStart->SetBoundingBoxSize( g_BitmapSizeX,     g_BitmapSizeY     );

    Scrooby::Text* title = GetTitleText();
#ifndef RAD_WIN32  // temp fix.
    title->SetPosition( g_TitlePositionX, g_TitlePositionY );
#endif

    Scrooby::Text* info = GetMissionInfoText();
    info->SetHorizontalJustification( Scrooby::Center );
}

//===========================================================================
// CGuiScreenMissionLoad::InitPositionsWager
//===========================================================================
// Description: Move scrooby elements to where they need to be for wager
//              missions
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMissionLoad::InitPositionsWager()
{
    Scrooby::BoundedDrawable* missionStart = GetMissionStartBitmap();
    missionStart->SetPosition(        g_BitmapPositionWagerX, g_BitmapPositionWagerY );
    missionStart->SetBoundingBoxSize( g_BitmapSizeWagerX,     g_BitmapSizeWagerY     );

    Scrooby::Text* title = GetTitleText();
    title->SetPosition( g_TitlePositionWagerX, g_TitlePositionWagerY );

    // turn on the flag bitmap
    Scrooby::Drawable* flag = GetFlag();
    flag->SetVisible( true );

    Scrooby::Text* info = GetMissionInfoText();
    info->SetHorizontalJustification( Scrooby::Left );
}

//===========================================================================
// CGuiScreenMissionLoad::RemoveListeners
//===========================================================================
// Description: removes all the listeners that have been set up in this class
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMissionLoad::RemoveListeners()
{
}

//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

