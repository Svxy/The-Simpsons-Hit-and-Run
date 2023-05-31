//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenScrapBookStats
//
// Description: Implementation of the CGuiScreenScrapBookStats class.
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
#include <presentation/gui/frontend/guiscreenscrapbookstats.h>

#include <cards/cardgallery.h>
#include <memory/srrmemory.h>
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
// CGuiScreenScrapBookStats::CGuiScreenScrapBookStats
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
CGuiScreenScrapBookStats::CGuiScreenScrapBookStats
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_SCRAP_BOOK_STATS ),
    m_percentGameComplete( NULL )
{
    memset( m_gameStats, 0, sizeof( m_gameStats ) );

    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "ScrapBookStats" );
	rAssert( pPage != NULL );

    // scale down game stats text bit
    //
    Scrooby::Text* gameStats = pPage->GetText( "GameStats" );
    if( gameStats != NULL )
    {
        gameStats->ResetTransformation();
        gameStats->ScaleAboutCenter( 0.75f );
    }

    Scrooby::Group* pGroup = pPage->GetGroup( "Stats" );
    rAssert( pGroup != NULL );

    m_gameStats[ STAT_STORY_MISSIONS ] = pGroup->GetText( "StoryMissions_Unlocked" );
    m_gameStats[ STAT_BONUS_MISSIONS ] = pGroup->GetText( "BonusMissions_Unlocked" );
    m_gameStats[ STAT_STREET_RACES ] = pGroup->GetText( "StreetRaces_Unlocked" );
    m_gameStats[ STAT_CARDS ] = pGroup->GetText( "Cards_Unlocked" );
    m_gameStats[ STAT_CLOTHING ] = pGroup->GetText( "Clothing_Unlocked" );
    m_gameStats[ STAT_VEHICLES ] = pGroup->GetText( "Vehicles_Unlocked" );
    m_gameStats[ STAT_WASPS ] = pGroup->GetText( "Wasps_Unlocked" );
    m_gameStats[ STAT_GAGS ] = pGroup->GetText( "Gags_Unlocked" );
    m_gameStats[ STAT_MOVIES ] = pGroup->GetText( "Movies_Unlocked" );

    // get game complete value
    //
    m_percentGameComplete = pGroup->GetText( "GameComplete_Value" );
    rAssert( m_percentGameComplete != NULL );

#ifdef RAD_DEBUG
    for( int i = 0; i < NUM_GAME_STATS; i++ )
    {
        rAssert( m_gameStats[ i ] != NULL );
    }
#endif
}


//===========================================================================
// CGuiScreenScrapBookStats::~CGuiScreenScrapBookStats
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
CGuiScreenScrapBookStats::~CGuiScreenScrapBookStats()
{
}


//===========================================================================
// CGuiScreenScrapBookStats::HandleMessage
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
void CGuiScreenScrapBookStats::HandleMessage
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
            case GUI_MSG_CONTROLLER_BACK:
            {
                this->StartTransitionAnimation( 830, 850 );

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
// CGuiScreenScrapBookStats::InitIntro
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
void CGuiScreenScrapBookStats::InitIntro()
{
    // sum game stats totals for all levels
    //
    int numUnlocked[ NUM_GAME_STATS ];
    memset( numUnlocked, 0, sizeof( numUnlocked ) );

    int numTotalGagsInGame = 0;
    int numTotalWaspsInGame = 0;

    for( int i = 0; i < RenderEnums::numLevels; i++ )
    {
        numUnlocked[ STAT_STORY_MISSIONS ] += GetCharacterSheetManager()->QueryNumMissionsCompleted( static_cast<RenderEnums::LevelEnum>( i ) );
        numUnlocked[ STAT_BONUS_MISSIONS ] += GetCharacterSheetManager()->QueryBonusMissionCompleted( static_cast<RenderEnums::LevelEnum>( i ) ) ? 1 : 0;
        numUnlocked[ STAT_STREET_RACES ] += GetCharacterSheetManager()->QueryNumStreetRacesCompleted( static_cast<RenderEnums::LevelEnum>( i ) );
        numUnlocked[ STAT_CARDS ] += GetCardGallery()->GetCollectedCards( i )->m_numCards;
        numUnlocked[ STAT_CLOTHING ] += GetCharacterSheetManager()->QueryNumSkinsUnlocked( static_cast<RenderEnums::LevelEnum>( i ) );
        numUnlocked[ STAT_VEHICLES ] += GetCharacterSheetManager()->QueryNumCarUnlocked( static_cast<RenderEnums::LevelEnum>( i ) );
        numUnlocked[ STAT_WASPS ] += GetCharacterSheetManager()->QueryNumWaspsDestroyed( static_cast<RenderEnums::LevelEnum>( i ) );
        numUnlocked[ STAT_GAGS ] += GetCharacterSheetManager()->QueryNumGagsViewed( static_cast<RenderEnums::LevelEnum>( i ) );
        numUnlocked[ STAT_MOVIES ] += GetCharacterSheetManager()->QueryFMVUnlocked( static_cast<RenderEnums::LevelEnum>( i ) ) ? 1 : 0;

        numTotalGagsInGame += GetRewardsManager()->GetTotalGags( i );
        numTotalWaspsInGame += GetRewardsManager()->GetTotalWasps( i );
    }

    // update game stats
    //
    HeapMgr()->PushHeap( GMA_LEVEL_FE );

    char buffer[ 32 ];

    sprintf( buffer, "%d / %d", numUnlocked[ STAT_STORY_MISSIONS ], 49 );
    m_gameStats[ STAT_STORY_MISSIONS ]->SetString( 0, buffer );

    sprintf( buffer, "%d / %d", numUnlocked[ STAT_BONUS_MISSIONS ], 7 );
    m_gameStats[ STAT_BONUS_MISSIONS ]->SetString( 0, buffer );

    sprintf( buffer, "%d / %d", numUnlocked[ STAT_STREET_RACES ], 21 );
    m_gameStats[ STAT_STREET_RACES ]->SetString( 0, buffer );

    sprintf( buffer, "%d / %d", numUnlocked[ STAT_CARDS ], 49 );
    m_gameStats[ STAT_CARDS ]->SetString( 0, buffer );

    sprintf( buffer, "%d / %d", numUnlocked[ STAT_CLOTHING ], 21 );
    m_gameStats[ STAT_CLOTHING ]->SetString( 0, buffer );

    sprintf( buffer, "%d / %d", numUnlocked[ STAT_VEHICLES ], 35 );
    m_gameStats[ STAT_VEHICLES ]->SetString( 0, buffer );

    sprintf( buffer, "%d / %d", numUnlocked[ STAT_WASPS ], numTotalWaspsInGame );
    m_gameStats[ STAT_WASPS ]->SetString( 0, buffer );

    sprintf( buffer, "%d / %d", numUnlocked[ STAT_GAGS ], numTotalGagsInGame );
    m_gameStats[ STAT_GAGS ]->SetString( 0, buffer );

    sprintf( buffer, "%d / %d", numUnlocked[ STAT_MOVIES ], 7 );
    m_gameStats[ STAT_MOVIES ]->SetString( 0, buffer );

    sprintf( buffer, "%.1f %%", GetCharacterSheetManager()->QueryPercentGameCompleted() );
    m_percentGameComplete->SetString( 0, buffer );

    HeapMgr()->PopHeap( GMA_LEVEL_FE );
}


//===========================================================================
// CGuiScreenScrapBookStats::InitRunning
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
void CGuiScreenScrapBookStats::InitRunning()
{
}


//===========================================================================
// CGuiScreenScrapBookStats::InitOutro
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
void CGuiScreenScrapBookStats::InitOutro()
{
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

