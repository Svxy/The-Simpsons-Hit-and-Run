//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiManagerLanguage
//
// Description: Implementation of the CGuiManagerLanguage class.
//
// Authors:     Tony Chu
//
// Revisions        Date            Author      Revision
//                  2003/07/08      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/bootup/guimanagerlanguage.h>
#include <presentation/gui/bootup/guiscreenlanguage.h>
#include <presentation/gui/guiwindow.h> // for window IDs
#include <presentation/gui/guisystem.h>
#include <presentation/gui/guitextbible.h>
#include <presentation/language.h>

#include <memory/srrmemory.h>
#include <render/rendermanager/rendermanager.h>
#include <render/rendermanager/renderlayer.h>

#include <raddebug.hpp>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiManagerLanguage::CGuiManagerLanguage
//===========================================================================
// Description: Constructor.
//
// Constraints: None.
//
// Parameters:  None.
//
// Return:      N/A.
//
//===========================================================================
CGuiManagerLanguage::CGuiManagerLanguage
(
    Scrooby::Project* pProject,
    CGuiEntity* pParent
)
:   CGuiManager( pProject, pParent )
{
}


//===========================================================================
// CGuiManagerLanguage::~CGuiManagerLanguage
//===========================================================================
// Description: Destructor.
//
// Constraints: None.
//
// Parameters:  None.
//
// Return:      N/A.
//
//===========================================================================
CGuiManagerLanguage::~CGuiManagerLanguage()
{
    for( int i = 0; i < CGuiWindow::NUM_GUI_WINDOW_IDS; i++ )
    {
        if( m_windows[ i ] != NULL )
        {
            delete m_windows[ i ];
            m_windows[ i ] = NULL;
        }
    }
}


//===========================================================================
// CGuiManagerLanguage::Populate
//===========================================================================
// Description:
//
// Constraints: None.
//
// Parameters:  None.
//
// Return:
//
//===========================================================================
void CGuiManagerLanguage::Populate()
{
MEMTRACK_PUSH_GROUP( "CGuiManagerLanguage" );
    Scrooby::Screen* pScroobyScreen;
    CGuiScreen* pScreen;

    pScroobyScreen = m_pScroobyProject->GetScreen( "Language" );
    if( pScroobyScreen != NULL )
    {
        pScreen = new CGuiScreenLanguage( pScroobyScreen, this );
        this->AddWindow( CGuiWindow::GUI_SCREEN_ID_LANGUAGE, pScreen );
    }
MEMTRACK_POP_GROUP("CGuiManagerLanguage");
}

void
CGuiManagerLanguage::Start( CGuiWindow::eGuiWindowID initialWindow )
{
    rAssert( m_state == GUI_FE_UNINITIALIZED );

    rAssertMsg( initialWindow == CGuiWindow::GUI_WINDOW_ID_UNDEFINED,
                "Can't specify another initial window! Not supported by this manager." );

    bool isLanguageSupported = this->CheckLanguage();

    if( !isLanguageSupported || CommandLineOptions::Get( CLO_LANG_PROMPT ) )
    {
        m_nextScreen = CGuiWindow::GUI_SCREEN_ID_LANGUAGE;

        m_state = GUI_FE_CHANGING_SCREENS; // must be set before calling GotoScreen()

        CGuiScreen* nextScreen = static_cast< CGuiScreen* >( this->FindWindowByID( m_nextScreen ) );
        rAssert( nextScreen != NULL );
        m_pScroobyProject->GotoScreen( nextScreen->GetScroobyScreen(), this );

        // thaw frontend render layer
        //
        GetRenderManager()->mpLayer(RenderEnums::GUI)->Thaw();
    }
    else
    {
        m_state = GUI_FE_TERMINATED;

        GetGuiSystem()->HandleMessage( GUI_MSG_INIT_BOOTUP );
    }
}

//===========================================================================
// CGuiManagerLanguage::HandleMessage
//===========================================================================
// Description:
//
// Constraints: None.
//
// Parameters:  None.
//
// Return:
//
//===========================================================================
void CGuiManagerLanguage::HandleMessage
(
    eGuiMessage message,
    unsigned int param1,
    unsigned int param2
)
{
    switch( message )
    {
        case GUI_MSG_LANGUAGE_SELECTION_DONE:
        {
            rAssert( GUI_FE_SCREEN_RUNNING == m_state );

            m_state = GUI_FE_SHUTTING_DOWN;

            // Tell the current screen to shut down.
            //
            this->FindWindowByID( m_currentScreen )->HandleMessage( GUI_MSG_WINDOW_EXIT );

            break;
        }
        case GUI_MSG_WINDOW_FINISHED:
        {
            if( GUI_FE_CHANGING_SCREENS == m_state )
            {
                m_currentScreen = m_nextScreen;
                CGuiScreen* nextScreen = static_cast< CGuiScreen* >( this->FindWindowByID( m_nextScreen ) );
                rAssert( nextScreen != NULL );
                m_pScroobyProject->GotoScreen( nextScreen->GetScroobyScreen(), this );
            }
            else if( GUI_FE_SHUTTING_DOWN == m_state )
            {
                m_state = GUI_FE_TERMINATED;

                GetGuiSystem()->HandleMessage( GUI_MSG_INIT_BOOTUP );
            }

            break;
        }
        default:
        {
            if( m_state != GUI_FE_UNINITIALIZED &&
                m_currentScreen != CGuiWindow::GUI_WINDOW_ID_UNDEFINED )
            {
                // Send the messages down to the current screen.
                //
                CGuiWindow* pScreen = this->FindWindowByID( m_currentScreen );
                rAssert( pScreen );

                pScreen->HandleMessage( message, param1, param2 );
            }
        }
    }

    // propogate message up the hierarchy
    CGuiManager::HandleMessage( message, param1, param2 );
}

//===========================================================================
// Private Member Functions
//===========================================================================

bool
CGuiManagerLanguage::CheckLanguage()
{
    bool isLanguageSupported = true;

    switch( Language::GetHardwareLanguage() )
    {
        case Language::ENGLISH:
        {
            CGuiTextBible::SetCurrentLanguage( Scrooby::XL_ENGLISH );

            rReleasePrintf( "Localization language automatically set to: ENGLISH\n" );

            break;
        }
        case Language::FRENCH:
        {
            CGuiTextBible::SetCurrentLanguage( Scrooby::XL_FRENCH );

            rReleasePrintf( "Localization language automatically set to: FRENCH\n" );

            break;
        }
        case Language::GERMAN:
        {
            CGuiTextBible::SetCurrentLanguage( Scrooby::XL_GERMAN );

            rReleasePrintf( "Localization language automatically set to: GERMAN\n" );

            break;
        }
/*
        case Language::ITALIAN:
        {
            CGuiTextBible::SetCurrentLanguage( Scrooby::XL_ITALIAN );

            rReleasePrintf( "Localization language automatically set to: ITALIAN\n" );

            break;
        }
*/
#ifndef RAD_GAMECUBE
        case Language::SPANISH:
        {
            CGuiTextBible::SetCurrentLanguage( Scrooby::XL_SPANISH );

            rReleasePrintf( "Localization language automatically set to: SPANISH\n" );

            break;
        }
#endif // !RAD_GAMECUBE
        default:
        {
#ifdef RAD_XBOX
            // default to English, if unsupported language detected
            //
            CGuiTextBible::SetCurrentLanguage( Scrooby::XL_ENGLISH );
#else
            isLanguageSupported = false;
#endif
            rReleasePrintf( "Unsupported hardware language detected.\n" );

            break;
        }
    }

    return isLanguageSupported;
}

