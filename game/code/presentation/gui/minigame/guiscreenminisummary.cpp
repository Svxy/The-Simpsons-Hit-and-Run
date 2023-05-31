//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMiniSummary
//
// Description: Implementation of the CGuiScreenMiniSummary class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/03/24      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/minigame/guiscreenminisummary.h>
#include <presentation/gui/guimanager.h>
#include <presentation/gui/guiscreenprompt.h>
#include <presentation/gui/guitextbible.h>

#include <contexts/supersprint/supersprintcontext.h>
#include <events/eventmanager.h>
#include <gameflow/gameflow.h>
#include <input/inputmanager.h>
#include <sound/soundmanager.h>

#include <screen.h>
#include <page.h>
#include <group.h>
#include <sprite.h>
#include <text.h>
#include <polygon.h>

#include <supersprint/supersprintmanager.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

const float MINI_SUMMARY_CHARACTER_SCALE_BIG = 0.9f;
const float MINI_SUMMARY_CHARACTER_SCALE_SMALL = 0.7f;
const float MINI_SUMMARY_KING_ICON_SCALE = 0.75f;

void
CGuiScreenMiniSummary::PlayerDisplayInfo::SetVisible( bool isVisible )
{
    rAssert( m_face != NULL );
    m_face->SetVisible( isVisible );
    rAssert( m_ranking != NULL );
    m_ranking->SetVisible( isVisible );
    rAssert( m_numWins != NULL );
    m_numWins->SetVisible( isVisible );
    rAssert( m_numPoints != NULL );
    m_numPoints->SetVisible( isVisible );
    rAssert( m_totalTime != NULL );
    m_totalTime->SetVisible( isVisible );
    rAssert( m_bestLap != NULL );
    m_bestLap->SetVisible( isVisible );
    rAssert( m_rowBgd != NULL );
    m_rowBgd->SetVisible( isVisible );
}

int
CGuiScreenMiniSummary::PlayerRanking::CompareScores( int points1, int wins1,
                                                     int points2, int wins2 )
{
    if( points1 > points2 )
    {
        return 1;
    }
    else if( points1 == points2 )
    {
        if( wins1 > wins2 )
        {
            return 1;
        }
        else if( wins1 == wins2 )
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenMiniSummary::CGuiScreenMiniSummary
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
CGuiScreenMiniSummary::CGuiScreenMiniSummary
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_MINI_SUMMARY ),
    m_totalTimeKingIcon( NULL ),
    m_bestLapKingIcon( NULL )
{
    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "MiniSummary" );
	rAssert( pPage != NULL );

    Scrooby::Text* raceSummary = pPage->GetText( "RaceSummary" );
    if( raceSummary != NULL )
    {
        raceSummary->SetTextMode( Scrooby::TEXT_WRAP );
    }

    Scrooby::Group* pGroupFaces = pPage->GetGroup( "Faces" );
    rAssert( pGroupFaces != NULL );
    Scrooby::Group* pGroupRanks = pPage->GetGroup( "Ranks" );
    rAssert( pGroupRanks != NULL );
    Scrooby::Group* pGroupPlayers = pPage->GetGroup( "Players" );
    rAssert( pGroupPlayers != NULL );
    Scrooby::Group* pGroupWins = pPage->GetGroup( "Wins" );
    rAssert( pGroupWins != NULL );
    Scrooby::Group* pGroupPoints = pPage->GetGroup( "Points" );
    rAssert( pGroupPoints != NULL );
    Scrooby::Group* pGroupTotalTime = pPage->GetGroup( "TotalTime" );
    rAssert( pGroupTotalTime != NULL );
    Scrooby::Group* pGroupBestLap = pPage->GetGroup( "BestLap" );
    rAssert( pGroupBestLap != NULL );
    Scrooby::Group* pGroupRows = pPage->GetGroup( "Rows" );
    rAssert( pGroupRows != NULL );

    char name[ 32 ];
    for( int i = 0; i < SuperSprintData::NUM_PLAYERS; i++ )
    {
        sprintf( name, "Face%d", i );
        m_displayInfo[ i ].m_face = pGroupFaces->GetSprite( name );

        sprintf( name, "Rank%d", i );
        m_displayInfo[ i ].m_ranking = pGroupRanks->GetText( name );

        sprintf( name, "Player%d", i );
        m_displayInfo[ i ].m_playerIndicator = pGroupPlayers->GetText( name );

        sprintf( name, "NumWins%d", i );
        m_displayInfo[ i ].m_numWins = pGroupWins->GetText( name );

        sprintf( name, "NumPoints%d", i );
        m_displayInfo[ i ].m_numPoints = pGroupPoints->GetText( name );

        sprintf( name, "TotalTime%d", i );
        m_displayInfo[ i ].m_totalTime = pGroupTotalTime->GetText( name );

        sprintf( name, "BestLap%d", i );
        m_displayInfo[ i ].m_bestLap = pGroupBestLap->GetText( name );

        sprintf( name, "Row%d", i );
        m_displayInfo[ i ].m_rowBgd = pGroupRows->GetPolygon( name );
    }

    // text wrap "Total Time" and "Best Lap"
    //
    Scrooby::Text* pText = pGroupTotalTime->GetText( "TotalTime" );
    rAssert( pText != NULL );
    pText->SetTextMode( Scrooby::TEXT_WRAP );

    pText = pGroupBestLap->GetText( "BestLap" );
    rAssert( pText != NULL );
    pText->SetTextMode( Scrooby::TEXT_WRAP );

    // get king icons
    //
    m_totalTimeKingIcon = pGroupTotalTime->GetSprite( "TotalTimeKingIcon" );
    m_bestLapKingIcon = pGroupBestLap->GetSprite( "BestLapKingIcon" );
    
#ifdef RAD_WIN32
    m_totalTimeKingIcon->ScaleAboutCenter( 0.5f );
    m_bestLapKingIcon->ScaleAboutCenter( 0.5f );
    pGroupRanks->GetSprite( "RankKingIcon" )->ScaleAboutCenter( 0.5f );
#endif

    // TC: [TEMP] hide these for now until I have time to implement the
    //            functionality
    //
    rAssert( m_totalTimeKingIcon != NULL );
    m_totalTimeKingIcon->SetVisible( false );
    rAssert( m_bestLapKingIcon != NULL );
    m_bestLapKingIcon->SetVisible( false );

    this->AutoScaleFrame( m_pScroobyScreen->GetPage( "BigBoard" ) );
}


//===========================================================================
// CGuiScreenMiniSummary::~CGuiScreenMiniSummary
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
CGuiScreenMiniSummary::~CGuiScreenMiniSummary()
{
}


//===========================================================================
// CGuiScreenMiniSummary::HandleMessage
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
void CGuiScreenMiniSummary::HandleMessage
(
	eGuiMessage message,
	unsigned int param1,
	unsigned int param2
)
{
    if( message == GUI_MSG_MENU_PROMPT_RESPONSE )
    {
        rAssert( param1 == PROMPT_CONFIRM_RACE_AGAIN );

        switch( param2 )
        {
            case (CGuiMenuPrompt::RESPONSE_YES):
            {
                // ok, letz start another race
                //
                GetGameFlow()->GetContext( CONTEXT_SUPERSPRINT )->Resume();

                m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN,
                                          GUI_SCREEN_ID_MINI_HUD,
                                          CLEAR_WINDOW_HISTORY );

                GetSSM()->Reset();

                GetSoundManager()->RestartSupersprintMusic();

                break;
            }
            case (CGuiMenuPrompt::RESPONSE_NO):
            {
                // quit mini-game and return to mini-game FE
                //
                m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN,
                                          GUI_SCREEN_ID_MINI_MENU,
                                          CLEAR_WINDOW_HISTORY );

                GetGameFlow()->SetContext( CONTEXT_SUPERSPRINT_FE );

                break;
            }
            default:
            {
                rAssertMsg( 0, "WARNING: *** Invalid prompt response!\n" );

                break;
            }
        }

        GetSoundManager()->OnStoreScreenEnd();
    }

    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        switch( message )
        {
            case GUI_MSG_CONTROLLER_SELECT:
            {
                // display 'race again' prompt
                //
                m_guiManager->DisplayPrompt( PROMPT_CONFIRM_RACE_AGAIN,
                                             this,
                                             PROMPT_TYPE_YES_NO,
                                             false );

                GetEventManager()->TriggerEvent( EVENT_FE_MENU_SELECT );

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
// CGuiScreenMiniSummary::InitIntro
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
void CGuiScreenMiniSummary::InitIntro()
{
    this->ResetCurrentRankings();

    // rank players by points and wins
    //
    for( int i = 0; i < SuperSprintData::NUM_PLAYERS; i++ )
    {
//        if( GetSSM()->GetVehicleData( i )->mIsHuman )
        {
            this->InsertPlayerRanking( i );
        }
    }

    // update display info
    //
    for( int i = 0; i < SuperSprintData::NUM_PLAYERS; i++ )
    {
        this->UpdateDisplayInfo( i, m_currentRankings[ i ].m_playerID );
    }

    //
    // This isn't a store, but it's a handy set of ducking values
    //
    GetSoundManager()->OnStoreScreenStart( false );
}


//===========================================================================
// CGuiScreenMiniSummary::InitRunning
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
void CGuiScreenMiniSummary::InitRunning()
{
}


//===========================================================================
// CGuiScreenMiniSummary::InitOutro
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
void CGuiScreenMiniSummary::InitOutro()
{
#ifdef RAD_WIN32
    GetInputManager()->GetFEMouse()->SetInGameMode( false );
#endif
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

void
CGuiScreenMiniSummary::UpdateDisplayInfo( int rankIndex, int playerID )
{
    rAssert( rankIndex >= 0 && rankIndex < SuperSprintData::NUM_PLAYERS );

    if( playerID != -1 )
    {
        m_displayInfo[ rankIndex ].SetVisible( true );

        const SuperSprintData::PlayerData* playerData = GetSSM()->GetPlayerData( playerID );
        rAssert( playerData != NULL );

        HeapMgr()->PushHeap( GMA_LEVEL_HUD );

        // set character face
        //
        rAssert( m_displayInfo[ rankIndex ].m_face != NULL );
        m_displayInfo[ rankIndex ].m_face->SetIndex( playerData->mCharacterIndex );
        m_displayInfo[ rankIndex ].m_face->ResetTransformation();
        if( rankIndex == 0 )
        {
            m_displayInfo[ rankIndex ].m_face->ScaleAboutCenter( MINI_SUMMARY_CHARACTER_SCALE_BIG );
        }
        else
        {
            m_displayInfo[ rankIndex ].m_face->ScaleAboutCenter( MINI_SUMMARY_CHARACTER_SCALE_SMALL );
        }

        // set ranking colour
        //
        rAssert( m_displayInfo[ rankIndex ].m_ranking != NULL );
        m_displayInfo[ rankIndex ].m_ranking->SetColour( SuperSprintData::PLAYER_COLOURS[ playerID ] );

        // set player ID indicator and colour
        //
        rAssert( m_displayInfo[ rankIndex ].m_playerIndicator != NULL );
        if( GetSSM()->GetVehicleData( playerID )->mIsHuman )
        {
            m_displayInfo[ rankIndex ].m_playerIndicator->SetVisible( true );
            m_displayInfo[ rankIndex ].m_playerIndicator->SetIndex( playerID );
            m_displayInfo[ rankIndex ].m_playerIndicator->SetColour( SuperSprintData::PLAYER_COLOURS[ playerID ] );
        }
        else
        {
            m_displayInfo[ rankIndex ].m_playerIndicator->SetVisible( false );
        }

        char buffer[ 16 ];
        UnicodeString unicodeString;

        // set num wins
        //
        sprintf( buffer, "%d", playerData->mWins );
        rAssert( m_displayInfo[ rankIndex ].m_numWins != NULL );
        m_displayInfo[ rankIndex ].m_numWins->SetString( 0, buffer );

        // set num points
        //
        sprintf( buffer, "%d", playerData->mPoints );
        rAssert( m_displayInfo[ rankIndex ].m_numPoints != NULL );
        m_displayInfo[ rankIndex ].m_numPoints->SetString( 0, buffer );

        // set total time
        //
        if( playerData->mPosition > 0 )
        {
            sprintf( buffer, "%.2f", playerData->mRaceTime / 1000000.0f );
            unicodeString.ReadAscii( buffer );
        }
        else
        {
            unicodeString.ReadUnicode( GetTextBibleString( "DNF" ) );
        }
        rAssert( m_displayInfo[ rankIndex ].m_totalTime != NULL );
        m_displayInfo[ rankIndex ].m_totalTime->SetString( 0, unicodeString );

        // set best lap
        //
        if( playerData->mRaceTime > 0 )
        {
            sprintf( buffer, "%.2f", playerData->mBestLap / 1000000.0f );
            unicodeString.ReadAscii( buffer );
        }
        else
        {
            unicodeString.ReadUnicode( GetTextBibleString( "DNF" ) );
        }
        rAssert( m_displayInfo[ rankIndex ].m_bestLap != NULL );
        m_displayInfo[ rankIndex ].m_bestLap->SetString( 0, unicodeString );

        HeapMgr()->PopHeap( GMA_LEVEL_HUD );
    }
    else
    {
        m_displayInfo[ rankIndex ].SetVisible( false );
    }
}

void
CGuiScreenMiniSummary::ResetCurrentRankings()
{
    for( int i = 0; i < SuperSprintData::NUM_PLAYERS; i++ )
    {
        m_currentRankings[ i ].m_playerID = -1;
        m_currentRankings[ i ].m_playerPoints = -1;
        m_currentRankings[ i ].m_playerWins = -1;
    }
}

void
CGuiScreenMiniSummary::InsertPlayerRanking( int playerID )
{
    const SuperSprintData::PlayerData* playerData = GetSSM()->GetPlayerData( playerID );
    rAssert( playerData != NULL );

    for( int i = 0; i < SuperSprintData::NUM_PLAYERS; i++ )
    {
        if( PlayerRanking::CompareScores( playerData->mPoints, playerData->mWins,
                                          m_currentRankings[ i ].m_playerPoints, m_currentRankings[ i ].m_playerWins ) > 0 )
        {
            // move everyone else down a spot
            //
            for( int j = SuperSprintData::NUM_PLAYERS - 1; j > i; j-- )
            {
                m_currentRankings[ j ].m_playerID = m_currentRankings[ j - 1 ].m_playerID;
                m_currentRankings[ j ].m_playerPoints = m_currentRankings[ j - 1 ].m_playerPoints;
                m_currentRankings[ j ].m_playerWins = m_currentRankings[ j - 1 ].m_playerWins;
            }

            // insert here
            //
            m_currentRankings[ i ].m_playerID = playerID;
            m_currentRankings[ i ].m_playerPoints = playerData->mPoints;
            m_currentRankings[ i ].m_playerWins = playerData->mWins;

            break;
        }
    }
}

