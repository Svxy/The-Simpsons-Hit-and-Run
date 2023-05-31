//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenTutorial
//
// Description: Implementation of the CGuiScreenTutorial class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/04/15      TChu        Created
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/ingame/guiscreentutorial.h>
#include <presentation/gui/guitextbible.h>
#include <presentation/gui/utility/specialfx.h>

#include <presentation/tutorialmanager.h>

#include <events/eventmanager.h>
#include <main/commandlineoptions.h>
#include <sound/soundmanager.h>

// Scrooby
#include <screen.h>
#include <page.h>
#include <layer.h>
#include <group.h>
#include <sprite.h>

// ATG
#include <raddebug.hpp>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

#define WAIT_UNTIL_DIALOG_DONE

const float BART_ICON_CORRECTION_SCALE = 1.0f;

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenTutorial::CGuiScreenTutorial
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
CGuiScreenTutorial::CGuiScreenTutorial
(
    Scrooby::Screen* pScreen,
    CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_TUTORIAL, SCREEN_FX_ZOOM ),
    m_tutorialMessage( NULL ),
    m_bartsHead( NULL ),
    m_disableTutorial( NULL ),
    m_elapsedDialogTime( 0 )
{
    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "Tutorial" );
    rAssert( pPage != NULL );

    Scrooby::Group* iconAndMessage = pPage->GetGroup( "IconAndMessage" );
    rAssert( iconAndMessage != NULL );

    m_bartsHead = iconAndMessage->GetSprite( "BartIcon" );

    m_tutorialMessage = iconAndMessage->GetSprite( "TutorialMessage" );
    rAssert( m_tutorialMessage != NULL );
    m_tutorialMessage->SetSpriteMode( Scrooby::SPRITE_BITMAP_TEXT );
    m_tutorialMessage->CreateBitmapTextBuffer( MAX_TUTORIAL_MESSAGE_LENGTH );
#ifdef RAD_WIN32
    m_tutorialMessage->ScaleAboutCenter( 0.25f );
#else
    m_tutorialMessage->ScaleAboutCenter( 0.5f );
#endif

#ifdef PAL
    iconAndMessage->ResetTransformation();
    iconAndMessage->Translate( 0, 15 );
#endif // PAL

    m_disableTutorial = pPage->GetGroup( "DisableTutorial" );
    rAssert( m_disableTutorial != NULL );

    Scrooby::Text* disableTutorial = m_disableTutorial->GetText( "DisableTutorial" );
    if( disableTutorial != NULL )
    {
        disableTutorial->SetDisplayOutline( true );
        disableTutorial->SetTextMode( Scrooby::TEXT_WRAP );
    }

    Scrooby::Page* pPageSmallBoard = m_pScroobyScreen->GetPage( "SmallBoard" );
    if( pPageSmallBoard != NULL )
    {
        this->AutoScaleFrame( pPageSmallBoard );

#ifdef PAL
        Scrooby::Group* frame = pPageSmallBoard->GetGroup( "Frame" );
        if( frame != NULL )
        {
            frame->ResetTransformation();
            frame->ScaleAboutCenter( 1.0f, 1.2f, 1.0f );
        }
#endif // PAL
    }

/*
    // get continue icon button
    //
    pPage = m_pScroobyScreen->GetPage( "Continue" );
    if( pPage != NULL )
    {
        m_buttonIcons[ BUTTON_ICON_ACCEPT ] = pPage->GetGroup( "Continue" );
        rAssert( m_buttonIcons[ BUTTON_ICON_ACCEPT ] != NULL );
    }
*/
}

//===========================================================================
// CGuiScreenTutorial::~CGuiScreenTutorial
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
CGuiScreenTutorial::~CGuiScreenTutorial()
{
}

//===========================================================================
// CGuiScreenTutorial::HandleMessage
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
void CGuiScreenTutorial::HandleMessage(	eGuiMessage message,
                                        unsigned int param1,
                                        unsigned int param2 )
{
    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        switch( message )
        {
            case GUI_MSG_UPDATE:
            {
                // pulse bart's head
                //
                m_elapsedDialogTime += param1;

                float currentScale = GuiSFX::Pulse( (float)m_elapsedDialogTime,
                                                    600.0f,
                                                    BART_ICON_CORRECTION_SCALE,
                                                    0.1f );

                rAssert( m_bartsHead != NULL );
                m_bartsHead->ResetTransformation();
                m_bartsHead->ScaleAboutCenter( currentScale );

#ifdef WAIT_UNTIL_DIALOG_DONE
/*
    #ifndef FINAL
                if( m_elapsedDialogTime > MAX_TUTORIAL_DIALOG_TIME )
                {
                    this->SetButtonVisible( BUTTON_ICON_ACCEPT, true );

                    rAssert( m_disableTutorial != NULL );
                    m_disableTutorial->SetVisible( true );
                }
                else
    #endif // !FINAL
*/
                {
                    this->SetButtonVisible( BUTTON_ICON_ACCEPT, !GetTutorialManager()->IsDialogPlaying() );

//                    rAssert( m_disableTutorial != NULL );
//                    m_disableTutorial->SetVisible( !GetTutorialManager()->IsDialogPlaying() );
                }
#endif // WAIT_UNTIL_DIALOG_DONE

                break;
            }
            case GUI_MSG_CONTROLLER_AUX_X:
            {
                rAssert( m_disableTutorial != NULL );
                if( m_disableTutorial->IsVisible() )
                {
                    // disable tutorial events
                    //
                    GetTutorialManager()->EnableTutorialEvents( false );

                    GetEventManager()->TriggerEvent( EVENT_FE_MENU_BACK ); // sound effect

                    if( GetTutorialManager()->IsDialogPlaying() &&
                        !CommandLineOptions::Get( CLO_MUTE ) )
                    {
                        // if dialog is still playing, stop it!
                        // (unless all sounds are muted)
                        //
                        GetEventManager()->TriggerEvent( EVENT_CONVERSATION_SKIP );
                    }

                    m_pParent->HandleMessage( GUI_MSG_RESUME_INGAME );
                }

                break;
            }
            case GUI_MSG_CONTROLLER_SELECT:
            {
#ifdef WAIT_UNTIL_DIALOG_DONE
                if( !this->IsButtonVisible( BUTTON_ICON_ACCEPT ) )
                {
                    // ignore input
                    //
                    return;
                }
#endif
                GetEventManager()->TriggerEvent( EVENT_FE_MENU_SELECT ); // sound effect

                m_pParent->HandleMessage( GUI_MSG_RESUME_INGAME );

                break;
            }
            case GUI_MSG_CONTROLLER_START:
            {
                // only pause if dialog is still playing
                //
                if( GetTutorialManager()->IsDialogPlaying() )
                {
                    m_pParent->HandleMessage( GUI_MSG_PAUSE_INGAME );
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
	CGuiScreen::HandleMessage( message, param1, param2 );
}

//===========================================================================
// CGuiScreenTutorial::InitIntro
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
void CGuiScreenTutorial::InitIntro()
{
    int index = GetTutorialManager()->GetCurrentEventID();

    char textBibleID[ 32 ];
    sprintf( textBibleID, "TUTORIAL_%03d", index );
    P3D_UNICODE* text = GetTextBibleString( textBibleID );

    // check for platform-specific text; if found, override default text
    //
#ifdef RAD_GAMECUBE
    strcat( textBibleID, "_(GC)" );
#endif
#ifdef RAD_PS2
    strcat( textBibleID, "_(PS2)" );
#endif
#ifdef RAD_XBOX
    strcat( textBibleID, "_(XBOX)" );
#endif
    P3D_UNICODE* platformText = GetTextBibleString( textBibleID );
    if( platformText != NULL )
    {
        text = platformText;

        rTunePrintf( "Replacing default tutorial text with entry: %s\n", textBibleID );
    }

    rAssert( m_tutorialMessage != NULL );
    m_tutorialMessage->SetBitmapText( text );

#ifdef WAIT_UNTIL_DIALOG_DONE
    this->SetButtonVisible( BUTTON_ICON_ACCEPT, false ); // hide by default

//    rAssert( m_disableTutorial != NULL );
//    m_disableTutorial->SetVisible( false ); // hide by default

    rAssert( m_bartsHead != NULL );
    m_bartsHead->ResetTransformation();
    m_bartsHead->ScaleAboutCenter( BART_ICON_CORRECTION_SCALE );

    m_elapsedDialogTime = 0;
#endif

    //
    // Inform the sound manager that it's time to turn the sound down a bit
    //
    GetSoundManager()->OnMissionBriefingStart();
}

//===========================================================================
// CGuiScreenTutorial::InitRunning
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
void CGuiScreenTutorial::InitRunning()
{
}

//===========================================================================
// CGuiScreenTutorial::InitOutro
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
void CGuiScreenTutorial::InitOutro()
{
    //
    // Turn the sound back up
    //
    GetSoundManager()->OnMissionBriefingEnd();
}

//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

