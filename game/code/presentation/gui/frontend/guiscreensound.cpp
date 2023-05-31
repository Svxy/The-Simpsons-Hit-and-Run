//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenSound
//
// Description: Implementation of the CGuiScreenSound class.
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
#include <presentation/gui/frontend/guiscreensound.h>
#include <presentation/gui/guimenu.h>

#include <memory/srrmemory.h>
#include <sound/soundmanager.h>

#include <raddebug.hpp> // Foundation
#include <page.h>
#include <screen.h>
#include <group.h>
#include <sprite.h>
#include <text.h>

#include <string.h>

#ifdef RAD_WIN32
#include <data/config/gameconfigmanager.h>
#endif

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

const char* SOUND_MENU_ITEMS[] = 
{
    "Music",
    "Effects",
    "Engine",
    "Voice",

#ifdef INCLUDE_SOUND_MODE
    "SurroundSound",
#endif

    "" // dummy terminator
};

const float SLIDER_CORRECTION_SCALE = 4.0f; // in x-direction only

#ifdef RAD_WIN32
const float SLIDER_ICON_SCALE = 0.5f;
#endif
//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenSound::CGuiScreenSound
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
CGuiScreenSound::CGuiScreenSound
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_SOUND ),
    m_pMenu( NULL )
{
MEMTRACK_PUSH_GROUP( "CGUIScreenSound" );
    memset( m_soundOffIcons, 0, sizeof( m_soundOffIcons ) );

    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage;
	pPage = m_pScroobyScreen->GetPage( "Sound" );
	rAssert( pPage );

    // Create a menu.
    //
    m_pMenu = new CGuiMenu( this, NUM_MENU_ITEMS );
    rAssert( m_pMenu != NULL );

    // Add menu items
    //
    Scrooby::Text* pText = NULL;
    Scrooby::Text* pTextValue = NULL;
    Scrooby::Sprite* pArrowL = NULL;
    Scrooby::Sprite* pArrowR = NULL;

    char itemName[ 32 ];

    for( int i = 0; i < NUM_MENU_ITEMS; i++ )
    {
        Scrooby::Group* group = pPage->GetGroup( SOUND_MENU_ITEMS[ i ] );
        rAssert( group  != NULL );

        pText = group->GetText( SOUND_MENU_ITEMS[ i ] );

        // if text value exists
        //
        sprintf( itemName, "%s_Value", SOUND_MENU_ITEMS[ i ] );
        pTextValue = group->GetText( itemName );

#ifdef INCLUDE_SOUND_MODE
        if( i == MENU_ITEM_SURROUND_SOUND )
        {
/*
            // get "dolby prologic #" text and set: # = 1 for GC
            //                                      # = 2 for PS2
            //
            P3D_UNICODE* unicodeString = static_cast<P3D_UNICODE*>( pTextValue->GetStringBuffer( SURROUND_SOUND ) );
            if( unicodeString != NULL )
            {
                int stringLength = p3d::UnicodeStrLen( unicodeString );
                for( int j = 0; j < stringLength; j++ )
                {
                    if( unicodeString[ j ] == '#' )
                    {
#ifdef RAD_GAMECUBE
                        unicodeString[ j ] = '1';
#endif
#ifdef RAD_PS2
                        unicodeString[ j ] = '2';
#endif
                    }
                }
            }
*/

            // add menu item for sound mode setting
            //
            pText = pTextValue;

            sprintf( itemName, "%s_LArrow", SOUND_MENU_ITEMS[ i ] );
            pArrowL = group->GetSprite( itemName );

            sprintf( itemName, "%s_RArrow", SOUND_MENU_ITEMS[ i ] );
            pArrowR = group->GetSprite( itemName );

            m_pMenu->AddMenuItem( pText,
                                  pTextValue,
                                  NULL,
                                  NULL,
                                  pArrowL,
                                  pArrowR,
                                  SELECTION_ENABLED | VALUES_WRAPPED | TEXT_OUTLINE_ENABLED );
        }
#endif

        // if slider exists
        //
        sprintf( itemName, "%s_Slider", SOUND_MENU_ITEMS[ i ] );
        Scrooby::Group* sliderGroup = group->GetGroup( itemName );
        if( sliderGroup != NULL )
        {
            sliderGroup->ResetTransformation();
            sliderGroup->ScaleAboutCenter( SLIDER_CORRECTION_SCALE, 1.0f, 1.0f );

            m_pMenu->AddMenuItem( pText,
                                  pTextValue,
                                  NULL,
                                  sliderGroup->GetSprite( itemName ),
                                  pArrowL,
                                  pArrowR,
                                  SELECTION_ENABLED | VALUES_WRAPPED | TEXT_OUTLINE_ENABLED );

            m_pMenu->GetMenuItem( i )->m_slider.m_type = Slider::HORIZONTAL_SLIDER_ABOUT_CENTER;

            // get sound off icon
            //
            sprintf( itemName, "%s_Off", SOUND_MENU_ITEMS[ i ] );
            m_soundOffIcons[ i ] = group->GetSprite( itemName );
#ifdef RAD_WIN32
            m_soundOffIcons[ i ]->ScaleAboutCenter( SLIDER_ICON_SCALE );

            sprintf( itemName, "%s_Icon", SOUND_MENU_ITEMS[ i ] );
            Scrooby::Sprite* soundOnIcon = group->GetSprite( itemName );
            soundOnIcon->ScaleAboutCenter( SLIDER_ICON_SCALE );
#endif
        }
    }

#ifdef INCLUDE_SOUND_MODE
    m_pMenu->SetSelectionValueCount( MENU_ITEM_SURROUND_SOUND, NUM_SOUND_SETTINGS );
#else
    // hide surround sound setting
    //
    Scrooby::Group* surroundSoundSetting = pPage->GetGroup( "SurroundSound" );
    if( surroundSoundSetting != NULL )
    {
        surroundSoundSetting->SetVisible( false );
    }

  #ifndef RAD_WIN32 // for PC don't shift the pixels... essential for the mouse cursor.
    // and move regular sound menu down a bit to re-center vertically
    //
    Scrooby::Group* soundMenu = pPage->GetGroup( "Menu" );
    rAssert( soundMenu != NULL );
    soundMenu->Translate( 0, -30 );
  #endif
#endif

MEMTRACK_POP_GROUP("CGUIScreenSound");
}


//===========================================================================
// CGuiScreenSound::~CGuiScreenSound
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
CGuiScreenSound::~CGuiScreenSound()
{
    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }
}


//===========================================================================
// CGuiScreenSound::HandleMessage
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
void CGuiScreenSound::HandleMessage
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
            case GUI_MSG_MENU_SELECTION_VALUE_CHANGED:
            {
                rAssert( m_pMenu );
                GuiMenuItem* currentItem = m_pMenu->GetMenuItem( param1 );
                rAssert( currentItem );

                switch( param1 )
                {
                    case MENU_ITEM_MUSIC:
                    {
                        GetSoundManager()->SetMusicVolume( currentItem->m_slider.m_value );

                        break;
                    }
                    case MENU_ITEM_EFFECTS:
                    {
                        GetSoundManager()->SetSfxVolume( currentItem->m_slider.m_value );

                        //
                        // HACK!!
                        //
                        // Oh boy, this is ugly.  We've got four sliders and five sets of volumes.
                        // Sfx needs to adjust ambience as well.  In a stroke of genius, I didn't
                        // provide a separate set of volumes for the designers.  So, the sfx needs
                        // to adjust ambience without upsetting whatever ambience volume they choose
                        // in the scripts.  I'm not proud of this.
                        //
                        // Esan
                        //
                        GetSoundManager()->SetAmbienceVolume( GetSoundManager()->GetCalculatedAmbienceVolume() );

                        break;
                    }
                    case MENU_ITEM_ENGINE:
                    {
                        GetSoundManager()->SetCarVolume( currentItem->m_slider.m_value );

                        break;
                    }
                    case MENU_ITEM_VOICE:
                    {
                        GetSoundManager()->SetDialogueVolume( currentItem->m_slider.m_value );

                        break;
                    }
#ifdef INCLUDE_SOUND_MODE
                    case MENU_ITEM_SURROUND_SOUND:
                    {
                        if( param2 == MONO_SOUND )
                        {
                            GetSoundManager()->SetSoundMode( SOUND_MONO );
                        }
                        else if( param2 == STEREO_SOUND )
                        {
                            GetSoundManager()->SetSoundMode( SOUND_STEREO );
                        }
                        else if( param2 == SURROUND_SOUND )
                        {
                            GetSoundManager()->SetSoundMode( SOUND_SURROUND );
                        }
                        else
                        {
                            rAssert( false );
                        }

                        break;
                    }
#endif
                    default:
                    {
                        break;
                    }
                }

                // show 'sound off' icon if volume is completely turn off
                //
                if( m_soundOffIcons[ param1 ] != NULL )
                {
                    m_soundOffIcons[ param1 ]->SetVisible( currentItem->m_slider.m_value == 0 );
                }

                // set flag indicating slider value has changed
                //
                m_hasSliderValueChanged[ param1 ] = true;

                break;
            }
            case GUI_MSG_MENU_SLIDER_NOT_CHANGING:
            {
                // play stinger if music slider has recently changed
                //
                if( m_hasSliderValueChanged[ MENU_ITEM_MUSIC ] )
                {
                    m_hasSliderValueChanged[ MENU_ITEM_MUSIC ] = false;

                    GetSoundManager()->PlayMusicOptionMenuStinger();
                }

                // play stinger if effects slider has recently changed
                //
                if( m_hasSliderValueChanged[ MENU_ITEM_EFFECTS ] )
                {
                    m_hasSliderValueChanged[ MENU_ITEM_EFFECTS ] = false;

                    GetSoundManager()->PlaySfxOptionMenuStinger();
                }

                // play stinger if engine slider has recently changed
                //
                if( m_hasSliderValueChanged[ MENU_ITEM_ENGINE ] )
                {
                    m_hasSliderValueChanged[ MENU_ITEM_ENGINE ] = false;

                    GetSoundManager()->PlayCarOptionMenuStinger();
                }

                // play stinger if voic slider has recently changed
                //
                if( m_hasSliderValueChanged[ MENU_ITEM_VOICE ] )
                {
                    m_hasSliderValueChanged[ MENU_ITEM_VOICE ] = false;

                    GetSoundManager()->PlayDialogueOptionMenuStinger();
                }

                break;
            }
            case GUI_MSG_CONTROLLER_BACK:
            {
                this->StartTransitionAnimation( 690, 720 );

                break;
            }
            default:
            {
                break;
            }
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

//===========================================================================
// CGuiScreenSound::InitIntro
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
void CGuiScreenSound::InitIntro()
{
    // Set slider values to current volume settings
    //
    GuiMenuItem* menuItem = NULL;
    rAssert( m_pMenu );

    // music
    //
    menuItem = m_pMenu->GetMenuItem( MENU_ITEM_MUSIC );
    rAssert( menuItem );
    menuItem->m_slider.SetValue( GetSoundManager()->GetMusicVolume() );

    rAssert( m_soundOffIcons[ MENU_ITEM_MUSIC ] != NULL );
    m_soundOffIcons[ MENU_ITEM_MUSIC ]->SetVisible( menuItem->m_slider.m_value == 0 );

    // effects
    //
    menuItem = m_pMenu->GetMenuItem( MENU_ITEM_EFFECTS );
    rAssert( menuItem );
    menuItem->m_slider.SetValue( GetSoundManager()->GetSfxVolume() );

    rAssert( m_soundOffIcons[ MENU_ITEM_EFFECTS ] != NULL );
    m_soundOffIcons[ MENU_ITEM_EFFECTS ]->SetVisible( menuItem->m_slider.m_value == 0 );

    // engine
    //
    menuItem = m_pMenu->GetMenuItem( MENU_ITEM_ENGINE );
    rAssert( menuItem );
    menuItem->m_slider.SetValue( GetSoundManager()->GetCarVolume() );

    rAssert( m_soundOffIcons[ MENU_ITEM_ENGINE ] != NULL );
    m_soundOffIcons[ MENU_ITEM_ENGINE ]->SetVisible( menuItem->m_slider.m_value == 0 );

    // voice
    //
    menuItem = m_pMenu->GetMenuItem( MENU_ITEM_VOICE );
    rAssert( menuItem );
    menuItem->m_slider.SetValue( GetSoundManager()->GetDialogueVolume() );

    rAssert( m_soundOffIcons[ MENU_ITEM_VOICE ] != NULL );
    m_soundOffIcons[ MENU_ITEM_VOICE ]->SetVisible( menuItem->m_slider.m_value == 0 );

    // surround sound
    //
#ifdef INCLUDE_SOUND_MODE
    if( GetSoundManager()->GetSoundMode() == SOUND_MONO )
    {
        m_pMenu->SetSelectionValue( MENU_ITEM_SURROUND_SOUND, MONO_SOUND );
    }
    else if( GetSoundManager()->GetSoundMode() == SOUND_STEREO )
    {
        m_pMenu->SetSelectionValue( MENU_ITEM_SURROUND_SOUND, STEREO_SOUND );
    }
    else if( GetSoundManager()->GetSoundMode() == SOUND_SURROUND )
    {
        m_pMenu->SetSelectionValue( MENU_ITEM_SURROUND_SOUND, SURROUND_SOUND );
    }
#endif

    // reset slider value changed flags
    //
    for( int i = 0; i < NUM_MENU_ITEMS; i++ )
    {
        m_hasSliderValueChanged[ i ] = false;
    }
}


//===========================================================================
// CGuiScreenSound::InitRunning
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
void CGuiScreenSound::InitRunning()
{
}


//===========================================================================
// CGuiScreenSound::InitOutro
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
void CGuiScreenSound::InitOutro()
{
#ifdef RAD_WIN32
    // Save the new controller mappings to the config file.
    GetGameConfigManager()->SaveConfigFile();
#endif
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

