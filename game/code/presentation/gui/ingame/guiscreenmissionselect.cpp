//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMissionSelect
//
// Description: Implementation of the CGuiScreenMissionSelect class.
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
#include <presentation/gui/ingame/guiscreenmissionselect.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/guimenu.h>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/guiuserinputhandler.h>

#include <cheats/cheatinputsystem.h>
#include <events/eventmanager.h>
#include <memory/srrmemory.h>
#include <mission/gameplaymanager.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <mission/charactersheet/charactersheet.h>
#include <render/enums/renderenums.h>

#include <raddebug.hpp> // Foundation
#include <page.h>
#include <polygon.h>
#include <screen.h>
#include <sprite.h>
#include <string.h>


//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

#ifdef RAD_WIN32
const float LEVEL_BAR_CORRECTION_SCALE = 1.0f;
#else
const float LEVEL_BAR_CORRECTION_SCALE = 2.0f;
#endif

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenMissionSelect::CGuiScreenMissionSelect
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
CGuiScreenMissionSelect::CGuiScreenMissionSelect
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_MISSION_SELECT ),
    m_pMenuLevel( NULL ),
    m_pMenu( NULL ),
    m_numLevelSelections( 0 )
{
MEMTRACK_PUSH_GROUP( "CGUIScreenMissionSelect" );
    HeapMgr()->PushHeap (GMA_LEVEL_HUD);

    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "MissionSelect" );
    rAssert( pPage );

    Scrooby::Group* levelBar = pPage->GetGroup( "Level" );
    rAssert( levelBar != NULL );

#ifdef RAD_WIN32
    m_leftArrow = levelBar->GetSprite( "LArrowBgd" );
    rAssert( m_leftArrow != NULL );
    m_leftArrow->ScaleAboutCenter( 1.7f );
    m_leftArrow->Translate( -5, 3 );

    m_rightArrow = levelBar->GetSprite( "RArrowBgd" );
    rAssert( m_rightArrow != NULL );
    m_rightArrow->ScaleAboutCenter( 1.7f );
    m_rightArrow->Translate( -6, 3 );
#endif

    Scrooby::Sprite* levelBarBgd = levelBar->GetSprite( "LevelBar" );
    if( levelBarBgd != NULL )
    {
        levelBarBgd->ResetTransformation();
        levelBarBgd->ScaleAboutCenter( LEVEL_BAR_CORRECTION_SCALE );
    }

    // Create menu for level selection.
    //
    m_pMenuLevel = new CGuiMenu( this, 1, GUI_TEXT_MENU, MENU_SFX_NONE );
    rAssert( m_pMenuLevel != NULL );
    m_pMenuLevel->SetHighlightColour( false, tColour( 0, 0, 0 ) );

    m_pMenuLevel->AddMenuItem( levelBar->GetText( "Level" ),
                               levelBar->GetText( "Level" ),
                               NULL,
                               NULL,
                               levelBar->GetSprite( "LArrowBgd" ),
                               levelBar->GetSprite( "RArrowBgd" ),
                               SELECTION_ENABLED | VALUES_WRAPPED );

    // Create a menu.
    //
    m_pMenu = new CGuiMenu( this, MAX_NUM_REGULAR_MISSIONS, GUI_TEXT_MENU, MENU_SFX_NONE );
    rAssert( m_pMenu != NULL );

    // Add menu items
    //
    Scrooby::Group* missions    = pPage->GetGroup( "Missions" );
    Scrooby::Group* missionMenu = pPage->GetGroup( "Menu" );
    Scrooby::Group* status      = pPage->GetGroup( "Status" );
    Scrooby::Group* initials    = pPage->GetGroup( "Initials" );
    Scrooby::Group* times       = pPage->GetGroup( "Times" );
    rAssert( missions    != NULL );
    rAssert( missionMenu != NULL );
    rAssert( status      != NULL );

    for( int i = 0; i < MAX_NUM_REGULAR_MISSIONS; i++ )
    {
        char name[ 32 ];

        // mission number and titles
        //
        sprintf( name, "MissionNum%d", i );
        m_missionInfo[ i ].m_number = missions->GetText( name );

        sprintf( name, "Mission%d", i );
        Scrooby::Text* pText = missionMenu->GetText( name );
        if( pText != NULL )
        {
            m_pMenu->AddMenuItem( pText );
        }

        m_missionInfo[ i ].m_title = pText;

        // mission status
        //
        sprintf( name, "MissionStatus%d", i );
        m_missionInfo[ i ].m_status = status->GetSprite( name );
    }

    Scrooby::Group* highlightBar = pPage->GetGroup( "HighlightBar" );
    if( highlightBar != NULL )
    {
        m_pMenu->SetCursor( highlightBar );

#ifdef PAL
        highlightBar->ResetTransformation();
        highlightBar->ScaleAboutCenter( 1.035f, 1.0f, 1.0f );
#endif // PAL
    }

    this->AutoScaleFrame( m_pScroobyScreen->GetPage( "BigBoard" ) );

    HeapMgr()->PopHeap (GMA_LEVEL_HUD);
MEMTRACK_POP_GROUP("CGUIScreenMissionSelect");
}


//===========================================================================
// CGuiScreenMissionSelect::~CGuiScreenMissionSelect
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
CGuiScreenMissionSelect::~CGuiScreenMissionSelect()
{
    if( m_pMenuLevel != NULL )
    {
        delete m_pMenuLevel;
        m_pMenuLevel = NULL;
    }

    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }
}


//===========================================================================
// CGuiScreenMissionSelect::HandleMessage
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
void CGuiScreenMissionSelect::HandleMessage
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
            case GUI_MSG_CONTROLLER_START:
            {
                if( !m_pMenu->HasSelectionBeenMade() )
                {
                    // resume game
                    m_pParent->HandleMessage( GUI_MSG_UNPAUSE_INGAME );
                }

                break;
            }
            case GUI_MSG_MENU_SELECTION_VALUE_CHANGED:
            {
                rAssert( param1 == 0 );

                this->OnLevelSelectionChange( static_cast<int>( param2 ) );

                break;
            }
            case GUI_MSG_MENU_SELECTION_MADE:
            {
                rAssert( m_pMenuLevel != NULL );
                int currentLevel = m_pMenuLevel->GetSelectionValue( 0 );

                // special case for level 1: all missions are offset by 1
                // due to the tutorial mission treated as mission 0
                //
                unsigned int selectedMission = currentLevel == 0 ? param1 + 1 : param1;

                m_pParent->HandleMessage( GUI_MSG_QUIT_INGAME_FOR_RELOAD,
                                          currentLevel,
                                          selectedMission );

                // stop any dialog that may still be in progress
                //
                GetEventManager()->TriggerEvent( EVENT_DIALOG_SHUTUP );

                break;
            }
            default:
            {
                break;
            }
        }

#ifdef RAD_DEMO
        // can't change level in demo builds; only missions in current level
        // are selectable
        //
        if( message == GUI_MSG_CONTROLLER_LEFT || message == GUI_MSG_CONTROLLER_RIGHT )
        {
            // ignore left/right controller inputs
            //
            return;
        }
#endif

        // relay message to level menu
        //
        if( m_pMenuLevel != NULL )
        {
            m_pMenuLevel->HandleMessage( message, param1, param2 );
        }

        // relay message to menu
        //
        if( m_pMenu != NULL )
        {
            m_pMenu->HandleMessage( message, param1, param2 );
        }
    }

	// Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );
}

#ifdef RAD_WIN32
//===========================================================================
// CGuiScreenMissionSelect::CheckCursorAgainstHotspots
//===========================================================================
// Description: Checks cursor position against its list of hotspots.
//
// Constraints: None.
//
// Parameters:  float x - The x position of cursor in P3D coordinates.
//              float y - The y position of cursor in P3D coordinates.
//
// Return:      N/A.
//
//===========================================================================
eFEHotspotType CGuiScreenMissionSelect::CheckCursorAgainstHotspots( float x, float y )
{
    eFEHotspotType hotSpotType = CGuiScreen::CheckCursorAgainstHotspots( x, y );
    if( hotSpotType == HOTSPOT_NONE )
    {
        if( m_leftArrow )
        {
            if( m_leftArrow->IsPointInBoundingRect( x, y ) )
            {
                hotSpotType = HOTSPOT_ARROWLEFT;
            }
        }
        if( m_rightArrow )
        {        
            if( m_rightArrow->IsPointInBoundingRect( x, y ) )
            {
                hotSpotType = HOTSPOT_ARROWRIGHT;
            }
        }

    }
    return hotSpotType;
}
#endif

//===========================================================================
// CGuiScreenMissionSelect::InitIntro
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
void CGuiScreenMissionSelect::InitIntro()
{
    m_numLevelSelections = GetCharacterSheetManager()->QueryHighestMission().mLevel + 1;
    if( GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_UNLOCK_MISSIONS ) )
    {
        m_numLevelSelections = RenderEnums::numLevels;
    }

    rAssert( m_pMenuLevel != NULL );
    m_pMenuLevel->SetSelectionValueCount( 0, m_numLevelSelections );

    int currentLevel = GetGameplayManager()->GetCurrentLevelIndex();
    m_pMenuLevel->SetSelectionValue( 0, currentLevel );
    this->OnLevelSelectionChange( currentLevel );
}


//===========================================================================
// CGuiScreenMissionSelect::InitRunning
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
void CGuiScreenMissionSelect::InitRunning()
{
}


//===========================================================================
// CGuiScreenMissionSelect::InitOutro
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
void CGuiScreenMissionSelect::InitOutro()
{
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

void
CGuiScreenMissionSelect::OnLevelSelectionChange( int currentLevel )
{
    rAssert( m_pMenu != NULL );
    m_pMenu->Reset();

    // update mission info for new level
    //
    for( int i = 0; i < MAX_NUM_REGULAR_MISSIONS; i++ )
    {
        // mission title
        //
        Scrooby::Text* missionTitle = dynamic_cast<Scrooby::Text*>( m_pMenu->GetMenuItem( i )->GetItem() );
        rAssert( missionTitle != NULL );
        missionTitle->SetIndex( currentLevel );

        // mission info (status, best time, initials)
        //
        MissionRecord* missionRecord = GetCharacterSheetManager()->QueryMissionStatus( static_cast<RenderEnums::LevelEnum>( currentLevel ),
                                                                                       currentLevel == 0 ? i + 1 : i );
        rAssert( missionRecord != NULL );

        bool isMissionUnlocked = true;
        if( currentLevel == (m_numLevelSelections - 1) )
        {
            int highestMissionPlayed = GetCharacterSheetManager()->QueryHighestMission().mMissionNumber;
            if( currentLevel == 0 )
            {
                highestMissionPlayed--;
            }

            if( GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_UNLOCK_MISSIONS ) )
            {
                highestMissionPlayed = RenderEnums::M7;
            }

#ifdef RAD_DEMO
            highestMissionPlayed = (currentLevel == RenderEnums::L7) ? RenderEnums::M4 : RenderEnums::M7;
#endif // RAD_DEMO

            isMissionUnlocked = ( i <= highestMissionPlayed );
        }

        if( isMissionUnlocked )
        {
            this->UpdateMissionStatus( i, missionRecord );
        }

        // show unlocked missions only
        //
        m_pMenu->SetMenuItemEnabled( i, isMissionUnlocked );

        m_missionInfo[ i ].m_number->SetVisible( isMissionUnlocked );
        m_missionInfo[ i ].m_title->SetVisible( isMissionUnlocked );
        m_missionInfo[ i ].m_status->SetVisible( isMissionUnlocked );
    }
}

void
CGuiScreenMissionSelect::UpdateMissionStatus( int index,
                                              MissionRecord* missionRecord )
{
    rAssert( missionRecord != NULL );
    rAssert( m_missionInfo[ index ].m_status );

    if( missionRecord->mCompleted )
    {
        m_missionInfo[ index ].m_status->SetIndex( 2 ); // 2 = green check
    }
    else
    {
        if( missionRecord->mNumAttempts > 0 &&
            !missionRecord->mSkippedMission )
        {
            m_missionInfo[ index ].m_status->SetIndex( 1 ); // 1 = red cross
        }
        else
        {
            m_missionInfo[ index ].m_status->SetIndex( 0 ); // 0 = yellow dash
        }
    }
}

