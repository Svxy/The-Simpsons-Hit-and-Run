//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenLevelStats
//
// Description: Implementation of the CGuiScreenLevelStats class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/03/10      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/ingame/guiscreenlevelstats.h>

#include <cards/cardgallery.h>
#include <memory/srrmemory.h>
#include <mission/gameplaymanager.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <mission/rewards/rewardsmanager.h>
#include <render/enums/renderenums.h>

// Scrooby
//
#include <screen.h>
#include <page.h>
#include <group.h>
#include <text.h>

// ATG
//
#include <raddebug.hpp>

#include <string.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenLevelStats::CGuiScreenLevelStats
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
CGuiScreenLevelStats::CGuiScreenLevelStats
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent,
    eGuiWindowID windowID
)
:   CGuiScreen( pScreen, pParent, windowID ),
    m_levelStatsHeading( NULL ),
    m_useMissionSelect( NULL )
{
    memset( m_levelStats, 0, sizeof( m_levelStats ) );

    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "LevelStats" );
	rAssert( pPage != NULL );

    this->AutoScaleFrame( pPage );

    m_levelStatsHeading = pPage->GetText( "LevelProgress" );
    rAssert( m_levelStatsHeading != NULL );

    Scrooby::Group* pGroup = pPage->GetGroup( "Stats" );
    rAssert( pGroup != NULL );

    m_levelStats[ STAT_STORY_MISSIONS ] = pGroup->GetText( "StoryMissions_Unlocked" );
    m_levelStats[ STAT_BONUS_MISSIONS ] = pGroup->GetText( "BonusMissions_Unlocked" );
    m_levelStats[ STAT_STREET_RACES ] = pGroup->GetText( "StreetRaces_Unlocked" );
    m_levelStats[ STAT_CARDS ] = pGroup->GetText( "Cards_Unlocked" );
    m_levelStats[ STAT_CLOTHING ] = pGroup->GetText( "Clothing_Unlocked" );
    m_levelStats[ STAT_VEHICLES ] = pGroup->GetText( "Vehicles_Unlocked" );
    m_levelStats[ STAT_WASPS ] = pGroup->GetText( "Wasps_Unlocked" );
    m_levelStats[ STAT_GAGS ] = pGroup->GetText( "Gags_Unlocked" );
    m_levelStats[ STAT_LEVEL_COMPLETE ] = pGroup->GetText( "LevelComplete_Value" );

#ifdef RAD_DEBUG
    for( int i = 0; i < NUM_LEVEL_STATS; i++ )
    {
        rAssert( m_levelStats[ i ] != NULL );
    }
#endif

    m_useMissionSelect = pPage->GetText( "UseMissionSelect" );
    if( m_useMissionSelect != NULL )
    {
        m_useMissionSelect->SetVisible( false ); // hide by default

        m_useMissionSelect->SetTextMode( Scrooby::TEXT_WRAP );
        m_useMissionSelect->SetDisplayOutline( true );
    }
}


//===========================================================================
// CGuiScreenLevelStats::~CGuiScreenLevelStats
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
CGuiScreenLevelStats::~CGuiScreenLevelStats()
{
}


//===========================================================================
// CGuiScreenLevelStats::HandleMessage
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
void CGuiScreenLevelStats::HandleMessage
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
                m_pParent->HandleMessage( GUI_MSG_UNPAUSE_INGAME );

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


//===========================================================================
// CGuiScreenLevelStats::InitIntro
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
void CGuiScreenLevelStats::InitIntro()
{
    // update current level stats
    //
    this->UpdateLevelStats();
}


//===========================================================================
// CGuiScreenLevelStats::InitRunning
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
void CGuiScreenLevelStats::InitRunning()
{
}


//===========================================================================
// CGuiScreenLevelStats::InitOutro
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
void CGuiScreenLevelStats::InitOutro()
{
}


void
CGuiScreenLevelStats::UpdateLevelStats()
{
    HeapMgr()->PushHeap( GMA_LEVEL_HUD );

    char buffer[ 32 ];
    RenderEnums::LevelEnum currentLevel = GetGameplayManager()->GetCurrentLevelIndex();

    sprintf( buffer, "%d / %d", GetCharacterSheetManager()->QueryNumMissionsCompleted( currentLevel ), 7 );
    m_levelStats[ STAT_STORY_MISSIONS ]->SetString( 0, buffer );

    sprintf( buffer, "%d / %d", GetCharacterSheetManager()->QueryBonusMissionCompleted( currentLevel ) ? 1 : 0, 1 );
    m_levelStats[ STAT_BONUS_MISSIONS ]->SetString( 0, buffer );

    sprintf( buffer, "%d / %d", GetCharacterSheetManager()->QueryNumStreetRacesCompleted( currentLevel ), 3 );
    m_levelStats[ STAT_STREET_RACES ]->SetString( 0, buffer );

    sprintf( buffer, "%d / %d", GetCardGallery()->GetCollectedCards( currentLevel )->m_numCards, 7 );
    m_levelStats[ STAT_CARDS ]->SetString( 0, buffer );

    sprintf( buffer, "%d / %d", GetCharacterSheetManager()->QueryNumSkinsUnlocked( currentLevel ), 3 );
    m_levelStats[ STAT_CLOTHING ]->SetString( 0, buffer );

    sprintf( buffer, "%d / %d", GetCharacterSheetManager()->QueryNumCarUnlocked( currentLevel ), 5 );
    m_levelStats[ STAT_VEHICLES ]->SetString( 0, buffer );

    sprintf( buffer, "%d / %d", GetCharacterSheetManager()->QueryNumWaspsDestroyed( currentLevel ),
                                GetRewardsManager()->GetTotalWasps( currentLevel ) );
    m_levelStats[ STAT_WASPS ]->SetString( 0, buffer );

    sprintf( buffer, "%d / %d", GetCharacterSheetManager()->QueryNumGagsViewed( currentLevel ),
                                GetRewardsManager()->GetTotalGags( currentLevel ) );
    m_levelStats[ STAT_GAGS ]->SetString( 0, buffer );

    sprintf( buffer, "%.1f %%", GetCharacterSheetManager()->QueryPercentLevelCompleted( currentLevel ) );
    m_levelStats[ STAT_LEVEL_COMPLETE ]->SetString( 0, buffer );

    HeapMgr()->PopHeap( GMA_LEVEL_HUD );
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

