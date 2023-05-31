//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenDisplay
//
// Description: Implementation of the CGuiScreenDisplay class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/06/16      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/frontend/guiscreendisplay.h>
#include <presentation/gui/guimenu.h>

#include <data/config/gameconfigmanager.h>
#include <main/win32platform.h>
#include <memory/srrmemory.h>
#include <render/RenderFlow/renderflow.h>

#include <raddebug.hpp> // Foundation
#include <screen.h>
#include <page.h>
#include <group.h>
#include <text.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================


const char* DISPLAY_MENU_ITEMS[] =
{
    "Resolution",
    "ColourDepth",
    "DisplayMode",
    "Gamma",
    "ApplyChanges",

    ""
};

const float SLIDER_ICON_SCALE = 0.5f;

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenDisplay::CGuiScreenDisplay
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
CGuiScreenDisplay::CGuiScreenDisplay
(
    Scrooby::Screen* pScreen,
    CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_DISPLAY ),
    m_pMenu( NULL ),
    m_changedGamma( false )
{
MEMTRACK_PUSH_GROUP( "CGuiScreenDisplay" );
    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "Display" );
    rAssert( pPage != NULL );

    // Create a menu.
    //
    m_pMenu = new CGuiMenu( this, NUM_MENU_ITEMS );
    rAssert( m_pMenu != NULL );

    // Add menu items
    //
    char itemName[ 32 ];

    for( int i = 0; i < MENU_ITEM_GAMMA; i++ )
    {
        Scrooby::Group* group = pPage->GetGroup( DISPLAY_MENU_ITEMS[ i ] );
        rAssert( group != NULL );

        sprintf( itemName, "%s_Value", DISPLAY_MENU_ITEMS[ i ] );
        Scrooby::Text* pTextValue = group->GetText( itemName );

        sprintf( itemName, "%s_ArrowL", DISPLAY_MENU_ITEMS[ i ] );
        Scrooby::Sprite* pLArrow = group->GetSprite( itemName );

        sprintf( itemName, "%s_ArrowR", DISPLAY_MENU_ITEMS[ i ] );
        Scrooby::Sprite* pRArrow = group->GetSprite( itemName );

        m_pMenu->AddMenuItem( group->GetText( DISPLAY_MENU_ITEMS[ i ] ),
                              pTextValue,
                              NULL,
                              NULL,
                              pLArrow,
                              pRArrow,
                              SELECTION_ENABLED | VALUES_WRAPPED | TEXT_OUTLINE_ENABLED );
    }

    // Add the gamma slider
    Scrooby::Group* pgroup = pPage->GetGroup( "Gamma" );
    rAssert(pgroup  != NULL );

    Scrooby::Text* pText = pgroup->GetText( "Gamma" );

    Scrooby::Group* sliderGroup = pgroup->GetGroup( "Gamma_Slider" );
    rAssert( sliderGroup != NULL );

    sliderGroup->ResetTransformation();

    m_pMenu->AddMenuItem( pText,
                          NULL,
                          NULL,
                          sliderGroup->GetSprite( "Gamma_Slider" ),
                          NULL,
                          NULL,
                          SELECTION_ENABLED | VALUES_WRAPPED | TEXT_OUTLINE_ENABLED );

    m_pMenu->GetMenuItem( MENU_ITEM_GAMMA )->m_slider.m_type = Slider::HORIZONTAL_SLIDER_ABOUT_CENTER;

    Scrooby::Sprite* soundOnIcon = pgroup->GetSprite( "Gamma_Icon" );
    soundOnIcon->ScaleAboutCenter( SLIDER_ICON_SCALE );

    // Add the apply changes button

    pgroup = pPage->GetGroup( "Menu" );
    rAssert( pgroup != NULL );

    m_pMenu->AddMenuItem( pgroup->GetText( "ApplyChanges" ) );

MEMTRACK_POP_GROUP("CGuiScreenDisplay");
}


//===========================================================================
// CGuiScreenDisplay::~CGuiScreenDisplay
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
CGuiScreenDisplay::~CGuiScreenDisplay()
{
    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }
}


//===========================================================================
// CGuiScreenDisplay::HandleMessage
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
void CGuiScreenDisplay::HandleMessage
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
            case GUI_MSG_MENU_SELECTION_MADE:
            {
                switch( param1 )
                {
                    case MENU_ITEM_APPLY_CHANGES:
                    {
                        ApplySettings();
                        break;
                    }
                }
                break;
            }
            case GUI_MSG_MENU_SELECTION_VALUE_CHANGED:
            {
                rAssert( m_pMenu );
                GuiMenuItem* currentItem = m_pMenu->GetMenuItem( param1 );
                rAssert( currentItem );

                switch( param1 )
                {
                    case MENU_ITEM_GAMMA:
                    {
                        float gamma = 2 * currentItem->m_slider.m_value + 0.5f;
                        GetRenderFlow()->SetGamma( gamma );
                        m_changedGamma = true;

                        break;
                    }
                }
                break;
            }
        }

        // relay message to menu
        if( m_pMenu != NULL )
        {
            m_pMenu->HandleMessage( message, param1, param2 );
        }
    }

	// Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );
}

//===========================================================================
// CGuiScreenDisplay::InitIntro
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
void CGuiScreenDisplay::InitIntro()
{
    // update settings
    //
    Win32Platform* plat = Win32Platform::GetInstance();

    Win32Platform::Resolution res = plat->GetResolution();
    m_pMenu->SetSelectionValue( MENU_ITEM_RESOLUTION,
                                res );

    int bpp = plat->GetBPP();
    m_pMenu->SetSelectionValue( MENU_ITEM_COLOUR_DEPTH,
                                bpp == 16 ? 0: 1 );

    bool fullscreen = plat->IsFullscreen();
    m_pMenu->SetSelectionValue( MENU_ITEM_DISPLAY_MODE,
                                fullscreen ? 1 : 0 );

    GuiMenuItem* menuItem = m_pMenu->GetMenuItem( MENU_ITEM_GAMMA );
    rAssert( menuItem );
    menuItem->m_slider.SetValue( ( GetRenderFlow()->GetGamma() - 0.5f ) / 2.0f );
}


//===========================================================================
// CGuiScreenDisplay::InitRunning
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
void CGuiScreenDisplay::InitRunning()
{
}


//===========================================================================
// CGuiScreenDisplay::InitOutro
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
void CGuiScreenDisplay::InitOutro()
{
    // Save the config if we've changed the gamma settings
    if( m_changedGamma )
    {
        GetGameConfigManager()->SaveConfigFile();
        m_changedGamma = false;
    }
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

//===========================================================================
// CGuiScreenDisplay::ApplySettings
//===========================================================================
// Description: Applies the current display settings to teh game. 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenDisplay::ApplySettings()
{
    // Retrieve the settings.
    //
    Win32Platform::Resolution res = static_cast< Win32Platform::Resolution >( m_pMenu->GetSelectionValue( MENU_ITEM_RESOLUTION ) );

    int bpp = m_pMenu->GetSelectionValue( MENU_ITEM_COLOUR_DEPTH ) ? 32: 16;

    bool fullscreen = m_pMenu->GetSelectionValue( MENU_ITEM_DISPLAY_MODE ) == 1;

    // Set the resolution.
    Win32Platform::GetInstance()->SetResolution( res, bpp, fullscreen );

    // Save the change to the config file.
    GetGameConfigManager()->SaveConfigFile();
    m_changedGamma = false;
}
