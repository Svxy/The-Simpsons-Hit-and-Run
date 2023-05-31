//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenViewCards
//
// Description: Implementation of the CGuiScreenViewCards class.
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
#include <presentation/gui/ingame/guiscreenviewcards.h>

#include <memory/srrmemory.h>
#include <mission/gameplaymanager.h>

// Scrooby
//
#include <screen.h>
#include <page.h>
#include <layer.h>

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
// CGuiScreenViewCards::CGuiScreenViewCards
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
CGuiScreenViewCards::CGuiScreenViewCards
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreenCardGallery( pScreen, pParent, GUI_SCREEN_ID_VIEW_CARDS ),
    m_pauseFgdLayer( NULL ),
    m_bigBoardFgdLayer( NULL )
{
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "PauseFgd" );
    if( pPage != NULL )
    {
        m_pauseFgdLayer = pPage->GetLayerByIndex( 0 );
    }

    pPage = m_pScroobyScreen->GetPage( "BigBoard" );
    if( pPage != NULL )
    {
        m_bigBoardFgdLayer = pPage->GetLayerByIndex( 0 );
    }

#ifndef RAD_WIN32
    m_cardScaleLarge = (1.0f / m_cardScaleSmall) * 0.8f;
    m_cardScaleSmall = 1.0f;
#endif
}


//===========================================================================
// CGuiScreenViewCards::~CGuiScreenViewCards
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
CGuiScreenViewCards::~CGuiScreenViewCards()
{
}


//===========================================================================
// CGuiScreenViewCards::HandleMessage
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
void CGuiScreenViewCards::HandleMessage
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
                if( m_pauseFgdLayer != NULL )
                {
                    m_pauseFgdLayer->SetVisible( m_cardGalleryState == STATE_BROWSING_CARDS );
                }

                if( m_bigBoardFgdLayer != NULL )
                {
                    m_bigBoardFgdLayer->SetVisible( m_cardGalleryState == STATE_BROWSING_CARDS );
                }

                break;
            }
            case GUI_MSG_CONTROLLER_START:
            {
                if( !m_pMenu->HasSelectionBeenMade() && m_cardGalleryState == STATE_BROWSING_CARDS )
                {
                    // resume game
                    m_pParent->HandleMessage( GUI_MSG_UNPAUSE_INGAME );
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
	CGuiScreenCardGallery::HandleMessage( message, param1, param2 );
}


//===========================================================================
// CGuiScreenViewCards::InitIntro
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
void CGuiScreenViewCards::InitIntro()
{
    this->UpdateCards( GetGameplayManager()->GetCurrentLevelIndex() );
}


//===========================================================================
// CGuiScreenViewCards::InitRunning
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
void CGuiScreenViewCards::InitRunning()
{
}


//===========================================================================
// CGuiScreenViewCards::InitOutro
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
void CGuiScreenViewCards::InitOutro()
{
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

