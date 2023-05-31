//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMissionLoad
//
// Description: Implementation of the CGuiScreenMissionSuccess class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/04/07      ian gipson  Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <mission/charactersheet/charactersheet.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <mission/gameplaymanager.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenmissionsuccess.h>
#include <group.h>
#include <page.h>
#include <screen.h>
#include <text.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================


//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenMissionSuccess::CGuiScreenMissionSuccess
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
CGuiScreenMissionSuccess::CGuiScreenMissionSuccess
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
):
    CGuiScreenMissionBase( pScreen, pParent, GUI_SCREEN_ID_MISSION_SUCCESS )
{
    Scrooby::Page* page  = pScreen->GetPage( "MissionLoad" );     rAssert( page                 != NULL );
    m_PattyAndSelmaTitle = page->GetText( "PattyAndSelmaTitle" ); rAssert( m_PattyAndSelmaTitle != NULL );
    m_PattyAndSelmaTitle->SetTextMode( Scrooby::TEXT_WRAP );
    m_PattyAndSelmaInfo  = page->GetText( "PattyAndSelmaInfo"  ); rAssert( m_PattyAndSelmaInfo  != NULL );
    m_PattyAndSelmaInfo->SetTextMode( Scrooby::TEXT_WRAP );
    m_PattyAndSelmaTitle->SetVisible( false );
    m_PattyAndSelmaInfo->SetVisible( false );
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
void CGuiScreenMissionSuccess::HandleMessage
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
            case GUI_MSG_CONTROLLER_SELECT:
            {
                // resume game and start mission
                //
                m_pParent->HandleMessage( GUI_MSG_RESUME_INGAME );
                break;
            }
            case GUI_MSG_CONTROLLER_BACK:
            {
                // resume game and abort mission
                //
                //m_pParent->HandleMessage( GUI_MSG_RESUME_INGAME, ON_HUD_ENTER_ABORT_MISSION );
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
// CGuiScreenMissionSuccess::InitIntro
//===========================================================================
// Description: sets up the screen when you enter it
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMissionSuccess::InitIntro()
{
    //
    // Check the level - level 7 rerquires the zombie picture
    //
    int level = GetGameplayManager()->GetCurrentLevelIndex();
    if( level == 6 )
    {
        SetBitmapName( "art/frontend/dynaload/images/misXX_HW.p3d" );
    }
    else
    {
        SetBitmapName( "art/frontend/dynaload/images/misXX_PS.p3d" );
    }
    ReplaceBitmap();

    //
    // Call the parent's initIntro function
    //
    CGuiScreenMissionBase::InitIntro();
    
    //
    // Hide the "cancel bitmap"
    //
    Scrooby::Group* abortBitmap = GetAbortBitmap();
    abortBitmap->SetVisible( false );

    //
    // Show the correct titles
    //
    m_PattyAndSelmaTitle->SetVisible( true );
    m_PattyAndSelmaInfo->SetVisible( true );

    //
    // Figure out how many races have been completed, and display it
    //
    CurrentMissionStruct mission = GetCharacterSheetManager()->QueryCurrentMission();
    int racesComplete = GetCharacterSheetManager()->QueryNumStreetRacesCompleted( mission.mLevel );
    char outputString[ 256 ] = "";
    if( racesComplete < 3 )
    {
        m_PattyAndSelmaTitle->SetIndex( 0 );
        m_PattyAndSelmaInfo->SetIndex( 0 );
        UnicodeString title = m_PattyAndSelmaTitle->GetString();
        char numberString[ 256 ] = "";
        sprintf( numberString, "%d", racesComplete );
        title.Replace( "%d", numberString );
        title.Replace( "%d", "3" );
        m_PattyAndSelmaTitle->SetString( 2, title );
        m_PattyAndSelmaTitle->SetIndex( 2 );

        //
        // Check if the info just says "*" if so, then hide it
        //
        UnicodeChar* uc = m_PattyAndSelmaInfo->GetStringBuffer();
        if( uc[ 0 ] == '*' )
        {
            m_PattyAndSelmaInfo->SetVisible( false );
        }
        else
        {
            m_PattyAndSelmaInfo->SetVisible( true );
        }
    }
    else
    {
        int level = GetGameplayManager()->GetCurrentLevelIndex();
        m_PattyAndSelmaTitle->SetIndex( 1 );
        m_PattyAndSelmaInfo->SetIndex( 1 + level );
    }
}

//===========================================================================
// CGuiScreenMissionSuccess::OutroDone
//===========================================================================
// Description: sets up the screen when you enter it
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMissionSuccess::OutroDone()
{
    m_PattyAndSelmaTitle->SetVisible( false );
    m_PattyAndSelmaInfo->SetVisible( false );
}
