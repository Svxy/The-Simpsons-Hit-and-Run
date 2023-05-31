//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenPlayMovie
//
// Description: Implementation of the CGuiScreenPlayMovie class.
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
#include <presentation/gui/frontend/guiscreenplaymovie.h>
#include <presentation/gui/guimanager.h>
#include <presentation/gui/guitextbible.h>
#include <presentation/gui/utility/specialfx.h>

#include <presentation/presentation.h>
#include <presentation/fmvplayer/fmvplayer.h>
#include <presentation/fmvplayer/fmvuserinputhandler.h>

#include <memory/srrmemory.h>
#include <render/rendermanager/rendermanager.h>
#include <render/rendermanager/renderlayer.h>

#include <screen.h>
#include <page.h>
#include <text.h>

#include <p3d/view.hpp>

#include <raddebug.hpp> // Foundation
#include <radmovie2.hpp>

#include <sound/soundmanager.h>

#include <input/inputmanager.h>


//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenPlayMovie::CGuiScreenPlayMovie
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
CGuiScreenPlayMovie::CGuiScreenPlayMovie
(
    Scrooby::Screen* pScreen,
    CGuiEntity* pParent,
    eGuiWindowID windowID
)
:   CGuiScreen( pScreen, pParent, windowID ),
    m_demoText( NULL ),
    m_elapsedTime( 0 ),
	m_IsSkippable(true)
//    m_tvFrame( NULL )
{
    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "Demo" );
    if( pPage != NULL )
    {
        m_demoText = pPage->GetText( "Demo" );
        rAssert( m_demoText != NULL );
    }

/*
    // background TV frame layer
    //
    m_tvFrame = pPage->GetLayer( "TVFrame" );
*/

    if( windowID == GUI_SCREEN_ID_PLAY_MOVIE )
    {
        // invert fading; ie. fade out during transition in, and vice versa
        //
        m_inverseFading = true;
    }
}


//===========================================================================
// CGuiScreenPlayMovie::~CGuiScreenPlayMovie
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
CGuiScreenPlayMovie::~CGuiScreenPlayMovie()
{
}


//===========================================================================
// CGuiScreenPlayMovie::HandleMessage
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
void CGuiScreenPlayMovie::HandleMessage
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
                GetPresentationManager()->Update( param1 );

                if( m_demoText != NULL )
                {
                    m_elapsedTime += param1;

                    const unsigned int BLINK_PERIOD = 250;
                    bool blinked = GuiSFX::Blink( m_demoText,
                                                (float)m_elapsedTime,
                                                (float)BLINK_PERIOD );
                    if( blinked )
                    {
                        m_elapsedTime %= BLINK_PERIOD;
                    }
                }
/*
                if( m_state == GUI_WINDOW_STATE_OUTRO )
                {
                    if( m_numTransitionsPending <= 0 )
                    {
                        // restore the background TV frame
                        //
                        if( m_tvFrame != NULL )
                        {
                            m_tvFrame->SetVisible( true );
                        }
                    }
                }
*/
                break;
            }

            case GUI_MSG_CONTROLLER_BACK:
            {
                // don't allow user to back out of screen, must be done
                // by skipping movie
                //
                return;

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


void
CGuiScreenPlayMovie::SetMovieToPlay( const char* movieName, bool IsSkippable, bool KillMusic, bool IsLocalized )
{
    rAssert( movieName != NULL );

    FMVEvent* pEvent = NULL;
    GetPresentationManager()->QueueFMV( &pEvent, this );

    strcpy( pEvent->fileName, movieName );
    pEvent->SetRenderLayer( RenderEnums::PresentationSlot );
    pEvent->SetAutoPlay( false );
    pEvent->SetClearWhenDone( true );
	pEvent->SetSkippable(IsSkippable);

#ifdef PAL
    if( IsLocalized )
    {
        switch( CGuiTextBible::GetCurrentLanguage() )
        {
            case Scrooby::XL_FRENCH:
            {
                pEvent->SetAudioIndex( FMVEvent::AUDIO_INDEX_FRENCH );

                break;
            }
            case Scrooby::XL_GERMAN:
            {
                pEvent->SetAudioIndex( FMVEvent::AUDIO_INDEX_GERMAN );

                break;
            }
            case Scrooby::XL_SPANISH:
            {
                pEvent->SetAudioIndex( FMVEvent::AUDIO_INDEX_SPANISH );

                break;
            }
            default:
            {
                rAssert( CGuiTextBible::GetCurrentLanguage() == Scrooby::XL_ENGLISH );
                pEvent->SetAudioIndex( FMVEvent::AUDIO_INDEX_ENGLISH );

                break;
            }
        }
    }
    else
#endif // PAL
    {
        pEvent->SetAudioIndex( FMVEvent::AUDIO_INDEX_ENGLISH );
    }

    if( KillMusic )
    {
        pEvent->KillMusic();
    }
}


//=============================================================================
// CGuiScreenPlayMovie::OnPresentationEventBegin
//=============================================================================
// Description: Comment
//
// Parameters:  ( PresentationEvent* pEvent )
//
// Return:      void 
//
//=============================================================================
void CGuiScreenPlayMovie::OnPresentationEventBegin( PresentationEvent* pEvent )
{
    GetSoundManager()->StopForMovie();
}


//=============================================================================
// CGuiScreenPlayMovie::OnPresentationEventLoadComplete
//=============================================================================
// Description: Comment
//
// Parameters:  ( PresentationEvent* pEvent )
//
// Return:      void 
//
//=============================================================================
void CGuiScreenPlayMovie::OnPresentationEventLoadComplete( PresentationEvent* pEvent )
{
}


//=============================================================================
// CGuiScreenPlayMovie::OnPresentationEventEnd
//=============================================================================
// Description: Comment
//
// Parameters:  ( PresentationEvent* pEvent )
//
// Return:      void 
//
//=============================================================================
void CGuiScreenPlayMovie::OnPresentationEventEnd( PresentationEvent* pEvent )
{
    if( GetPresentationManager()->IsQueueEmpty() )
    {
        GetSoundManager()->ResumeAfterMovie();

        if( m_ID == GUI_SCREEN_ID_PLAY_MOVIE_INTRO )
        {
            // finished playing intro movie, now go to main menu
            //
            m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN,
                                      GUI_SCREEN_ID_INTRO_TRANSITION,
                                      CLEAR_WINDOW_HISTORY );
        }
        else if( m_ID == GUI_SCREEN_ID_PLAY_MOVIE_NEW_GAME )
        {
            // finished playing new game movie, now quit the FE
            //
            m_pParent->HandleMessage( GUI_MSG_QUIT_FRONTEND, 1 ); // 1 = one player
        }
        else
        {
            rAssert( m_guiManager );
            eGuiWindowID previousScreen = m_guiManager->GetPreviousScreen();
            if( previousScreen == GUI_SCREEN_ID_VIEW_MOVIES )
            {
                this->StartTransitionAnimation( 1005, 1035 );
            }
            else if( previousScreen == GUI_SCREEN_ID_OPTIONS )
            {
                this->StartTransitionAnimation( 630, 660 );
            }

            m_pParent->HandleMessage( GUI_MSG_BACK_SCREEN );
        }
    }
}


//===========================================================================
// CGuiScreenPlayMovie::InitIntro
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
void CGuiScreenPlayMovie::InitIntro()
{
    rAssertMsg( !GetPresentationManager()->IsQueueEmpty(),
                "ERROR: *** No movies were queued for playing!" );

    if( m_demoText != NULL )
    {
        m_demoText->SetVisible( false );
        m_elapsedTime = 0;
    }

#ifdef RAD_WIN32
    GetInputManager()->GetFEMouse()->SetInGameMode( true );
#endif

/*
    // hide background TV frame so we can fade in/out the foreground one
    //
    if( m_tvFrame != NULL )
    {
        m_tvFrame->SetVisible( false );
    }
*/
}


//===========================================================================
// CGuiScreenPlayMovie::InitRunning
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
void CGuiScreenPlayMovie::InitRunning()
{
    if( m_ID == GUI_SCREEN_ID_PLAY_MOVIE_DEMO )
    {
        // disable screen clearing for GUI render layer
        //
        GetRenderManager()->mpLayer( RenderEnums::GUI )->pView( 0 )->SetClearMask( PDDI_BUFFER_DEPTH | PDDI_BUFFER_STENCIL );
    }
    else
    {
        GetRenderManager()->mpLayer( RenderEnums::GUI )->Chill();
    }

    GetPresentationManager()->GetFMVPlayer()->Play();

#ifdef FINAL
    if( m_ID == GUI_SCREEN_ID_PLAY_MOVIE_NEW_GAME )
    {
        // disable skipping of new game movie
        //
        FMVUserInputHandler* userInputHandler = GetPresentationManager()->GetFMVPlayer()->GetUserInputHandler();
        rAssert( userInputHandler != NULL );
        userInputHandler->SetEnabled( false );
    }
#endif
}


//===========================================================================
// CGuiScreenPlayMovie::InitOutro
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
void CGuiScreenPlayMovie::InitOutro()
{
    if( m_ID == GUI_SCREEN_ID_PLAY_MOVIE_DEMO )
    {
        // enable screen clearing
        //
        GetRenderManager()->mpLayer(RenderEnums::GUI)->pView( 0 )->SetClearMask( PDDI_BUFFER_ALL );
    }
    else
    {
        GetRenderManager()->mpLayer( RenderEnums::GUI )->Warm();
    }

#ifdef RAD_WIN32
    GetInputManager()->GetFEMouse()->SetInGameMode( false );
#endif

#ifdef FINAL
    if( m_ID == GUI_SCREEN_ID_PLAY_MOVIE_NEW_GAME )
    {
        // re-enable FMV user inputs
        //
        FMVUserInputHandler* userInputHandler = GetPresentationManager()->GetFMVPlayer()->GetUserInputHandler();
        rAssert( userInputHandler != NULL );
        userInputHandler->SetEnabled( true );
    }
#endif
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

