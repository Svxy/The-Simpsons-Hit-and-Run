//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenCreditsPostFMV
//
// Description: Implementation of the CGuiScreenCreditsPostFMV class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/06/02      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/ingame/guiscreencreditspostfmv.h>
#include <presentation/gui/guimanager.h>

#include <events/eventmanager.h>
#include <sound/soundmanager.h>

// Scrooby
#include <screen.h>
#include <page.h>
#include <group.h>
#include <sprite.h>

// ATG
#include <p3d/utility.hpp>
#include <raddebug.hpp>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

const float KANG_KODOS_IMAGE_CORRECTION_SCALE = 1.5f;

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenCreditsPostFMV::CGuiScreenCreditsPostFMV
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
CGuiScreenCreditsPostFMV::CGuiScreenCreditsPostFMV
(
    Scrooby::Screen* pScreen,
    CGuiEntity* pParent
)
:   CGuiScreenViewCredits( pScreen, pParent ),
    m_kang( NULL ),
    m_kodos( NULL )
{
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "CreditsKK" );
    rAssert( pPage != NULL );

    m_kang = pPage->GetSprite( "Kang" );
    rAssert( m_kang != NULL );
    m_kang->SetVisible( false ); // hide by default

    m_kodos = pPage->GetSprite( "Kodos" );
    rAssert( m_kodos != NULL );
    m_kodos->SetVisible( false ); // hide by default
}


//===========================================================================
// CGuiScreenCreditsPostFMV::~CGuiScreenCreditsPostFMV
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
CGuiScreenCreditsPostFMV::~CGuiScreenCreditsPostFMV()
{
    if( m_kang != NULL )
    {
        m_kang->SetRawSprite( NULL );
    }

    if( m_kodos != NULL )
    {
        m_kodos->SetRawSprite( NULL );
    }
}


//===========================================================================
// CGuiScreenCreditsPostFMV::HandleMessage
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
void CGuiScreenCreditsPostFMV::HandleMessage
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
                this->OnScrollingDone();

                break;
            }
            case GUI_MSG_CONTROLLER_BACK:
            {
                // ignore back controller inputs
                //
                return;
            }
            default:
            {
                break;
            }
        }
    }

	// Propogate the message up the hierarchy.
	//
	CGuiScreenViewCredits::HandleMessage( message, param1, param2 );
}


//===========================================================================
// CGuiScreenCreditsPostFMV::InitIntro
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
void CGuiScreenCreditsPostFMV::InitIntro()
{
    GetEventManager()->TriggerEvent( EVENT_PLAY_CREDITS );

    m_playKKDialog = true;

    this->ResetScrolling();

    GetSoundManager()->DuckForInGameCredits();

    rAssert( m_creditsGroup != NULL );
    m_creditsGroup->SetVisible( false );

    //
    // TC: [TODO] remove the loading of these images from the mission script
    //            for level 7 - mission 8 - sunday drive
    //
/*
    // search for kang and kodos images
    //
    tSprite* pSprite = NULL;

    pSprite = p3d::find<tSprite>( "kang.png" );
    if( pSprite != NULL )
    {
        rAssert( m_kang != NULL );
        m_kang->SetRawSprite( pSprite );
        m_kang->SetVisible( true );

        m_kang->ResetTransformation();
        m_kang->ScaleAboutCenter( KANG_KODOS_IMAGE_CORRECTION_SCALE );
    }
    else
    {
        rAssertMsg( false, "Can't find kang image!" );
    }

    pSprite = p3d::find<tSprite>( "kodos.png" );
    if( pSprite != NULL )
    {
        rAssert( m_kodos != NULL );
        m_kodos->SetRawSprite( pSprite );
        m_kodos->SetVisible( true );

        m_kodos->ResetTransformation();
        m_kodos->ScaleAboutCenter( KANG_KODOS_IMAGE_CORRECTION_SCALE );
    }
    else
    {
        rAssertMsg( false, "Can't find kodos image!" );
    }
*/
}


//===========================================================================
// CGuiScreenCreditsPostFMV::InitRunning
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
void CGuiScreenCreditsPostFMV::InitRunning()
{
    rAssert( m_creditsGroup != NULL );
    m_creditsGroup->SetVisible( true );
}


//===========================================================================
// CGuiScreenCreditsPostFMV::InitOutro
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
void CGuiScreenCreditsPostFMV::InitOutro()
{
    GetEventManager()->TriggerEvent( EVENT_DIALOG_SHUTUP );
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

void
CGuiScreenCreditsPostFMV::OnScrollingDone()
{
    m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN,
                              GUI_SCREEN_ID_LEVEL_END,
                              CLEAR_WINDOW_HISTORY );
}

