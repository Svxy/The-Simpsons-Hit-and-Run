//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenLevelEnd
//
// Description: Implementation of the CGuiScreenLevelEnd class.
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
#include <presentation/gui/ingame/guiscreenlevelend.h>

#include <data/memcard/memorycardmanager.h>
#include <events/eventmanager.h>
#include <memory/srrmemory.h>
#include <mission/gameplaymanager.h>

// Scrooby
//
#include <screen.h>
#include <page.h>
#include <text.h>

// ATG
//
#include <raddebug.hpp>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenLevelEnd::CGuiScreenLevelEnd
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
CGuiScreenLevelEnd::CGuiScreenLevelEnd
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreenLevelStats( pScreen, pParent, GUI_SCREEN_ID_LEVEL_END )
{
}


//===========================================================================
// CGuiScreenLevelEnd::~CGuiScreenLevelEnd
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
CGuiScreenLevelEnd::~CGuiScreenLevelEnd()
{
}


//===========================================================================
// CGuiScreenLevelEnd::HandleMessage
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
void CGuiScreenLevelEnd::HandleMessage
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
                m_pParent->HandleMessage( GUI_MSG_QUIT_INGAME_FOR_RELOAD,
                                          GetGameplayManager()->GetCurrentLevelIndex() + 1,
                                          0 );

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
// CGuiScreenLevelEnd::InitIntro
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
void CGuiScreenLevelEnd::InitIntro()
{
    // set heading to "Level Complete"
    //
    rAssert( m_levelStatsHeading != NULL );
    m_levelStatsHeading->SetIndex( 1 );

    // show "use mission select" info text
    //
    if( m_useMissionSelect != NULL )
    {
        m_useMissionSelect->SetVisible( true );
    }

    // update current level stats
    //
    this->UpdateLevelStats();

    // load memory card info (in case user decides to save before advancing
    // to next level)
    //
    GetMemoryCardManager()->LoadMemcardInfo( dynamic_cast<IMemoryCardInfoLoadCallback*>( m_pParent ) );
}


//===========================================================================
// CGuiScreenLevelEnd::InitRunning
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
void CGuiScreenLevelEnd::InitRunning()
{
}


//===========================================================================
// CGuiScreenLevelEnd::InitOutro
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
void CGuiScreenLevelEnd::InitOutro()
{
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

