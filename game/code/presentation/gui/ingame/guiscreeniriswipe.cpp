//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenIrisWipe
//
// Description: Implementation of the CGuiScreenIrisWipe class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/08/21      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/ingame/guiscreeniriswipe.h>
#include <presentation/gui/utility/specialfx.h>

#include <events/eventmanager.h>

#include <screen.h>
#include <page.h>
#include <pure3dobject.h>
#include <text.h>

#include <p3d/anim/multicontroller.hpp>
#include <p3d/utility.hpp>

#include <raddebug.hpp> // Foundation

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================
const float DEFAULT_REL_SPEED = 0.5f;
static bool g_doNotOpenOnNextOutro = false;
bool CGuiScreenIrisWipe::g_IsIrisClosed = false;

const unsigned int MAX_IDLE_TIME_OF_BLACKNESS = 3000; // in msec

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenIrisWipe::CGuiScreenIrisWipe
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
CGuiScreenIrisWipe::CGuiScreenIrisWipe
(
    Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:	CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_IRIS_WIPE ),
    m_pIris( 0 ),
    m_pMultiController( 0 ),
    m_isIrisActive( false ),
    m_loadingText( NULL ),
    m_elapsedIdleTime( 0 ),
    m_elapsedBlinkTime( 0 )
{
    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "3dIris" );
    rAssert( pPage != NULL );

    m_pIris = pPage->GetPure3dObject( "p3d_iris" );
    rAssert( m_pIris );

    // Have to find the multicontroller ourselves because for some stupid reason
    // Scrooby doesn't make it accessable via the Pure3dObject until the first render.
    //
    m_pMultiController = p3d::find<tMultiController>( "IrisController" );
    rAssert( m_pMultiController );

    m_numFrames = m_pMultiController->GetNumFrames();

    // get loading text
    //
    pPage = m_pScroobyScreen->GetPage( "LoadingText" );
    if( pPage != NULL )
    {
        m_loadingText = pPage->GetText( "Loading" );
        rAssert( m_loadingText != NULL );
        m_loadingText->SetVisible( false ); // hide by default
    }

    m_relativeSpeed = DEFAULT_REL_SPEED;
}


//===========================================================================
// CGuiScreenIrisWipe::~CGuiScreenIrisWipe
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
CGuiScreenIrisWipe::~CGuiScreenIrisWipe()
{
}

//===========================================================================
// CGuiScreenIrisWipe::DoNotOpenOnNextOutro
//===========================================================================
// Description: tells the screen not to transition out next time
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenIrisWipe::DoNotOpenOnNextOutro()
{
    g_doNotOpenOnNextOutro = true;
}

//===========================================================================
// CGuiScreenIrisWipe::HandleMessage
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
void CGuiScreenIrisWipe::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2 
)
{
    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        if( message == GUI_MSG_UPDATE )
        {
            m_elapsedIdleTime += param1;

            if( m_elapsedIdleTime > MAX_IDLE_TIME_OF_BLACKNESS )
            {
                m_elapsedBlinkTime += param1;

                if( m_loadingText != NULL )
                {
                    // blink loading text if idling here on this screen to satisfy
                    // TRC/TCR requirements
                    //
                    const unsigned int BLINKING_PERIOD = 250;
                    bool isBlinked = GuiSFX::Blink( m_loadingText,
                                                    static_cast<float>( m_elapsedBlinkTime ),
                                                    static_cast<float>( BLINKING_PERIOD ) );
                    if( isBlinked )
                    {
                        m_elapsedBlinkTime %= BLINKING_PERIOD;
                    }
                }
            }
        }
    }
    else if( m_state == GUI_WINDOW_STATE_INTRO )
    {
        switch( message )
        {
            case GUI_MSG_UPDATE:
            {
                if( m_isIrisActive && m_pMultiController->LastFrameReached() )
                {
                    m_isIrisActive = false;

                    m_numTransitionsPending--;
                    rAssert( m_numTransitionsPending == 0 );

                    GetEventManager()->TriggerEvent( EVENT_GUI_IRIS_WIPE_CLOSED );
                    rReleasePrintf( "CGuiScreenIrisWipe => EVENT_GUI_IRIS_WIPE_CLOSED.\n" );
                }

                break;
            }
            default:
            {
                break;
            }
        }
    }
    else if( m_state == GUI_WINDOW_STATE_OUTRO )
    {
        switch( message )
        {
            case GUI_MSG_UPDATE:
            {
                if( m_isIrisActive && m_pMultiController->LastFrameReached() )
                {
                    m_isIrisActive = false;

                    m_numTransitionsPending--;
                    rAssert( m_numTransitionsPending == 0 );

                    GetEventManager()->TriggerEvent( EVENT_GUI_IRIS_WIPE_OPEN );
                    rReleasePrintf( "CGuiScreenIrisWipe => EVENT_GUI_IRIS_WIPE_OPEN.\n" );
                }

                break;
            }
            case GUI_MSG_WINDOW_EXIT:
            {
                // ignore multiple exit requests
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
	CGuiScreen::HandleMessage( message, param1, param2 );
}


//===========================================================================
// CGuiScreenIrisWipe::InitIntro
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
void CGuiScreenIrisWipe::InitIntro()
{
    if( m_loadingText != NULL )
    {
        // hide loading text
        //
        m_loadingText->SetVisible( false );
    }

    if( !m_isIrisActive )
    {
        m_isIrisActive = true;

        g_IsIrisClosed = true;
        m_pIris->SetVisible( true );
        m_pMultiController->SetRelativeSpeed( m_relativeSpeed );

        rAssertMsg( m_numTransitionsPending == 0, "Bad news if you hit this assert! Please go tell Tony." );
        m_numTransitionsPending++;
        
        m_pMultiController->Reset();
        m_pMultiController->SetFrameRange( 0.0f, m_numFrames * 0.5f );
        m_pMultiController->SetFrame( 0.0f );
    }
}


//===========================================================================
// CGuiScreenIrisWipe::InitRunning
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
void CGuiScreenIrisWipe::InitRunning()
{
    m_elapsedIdleTime = 0;
    m_elapsedBlinkTime = 0;
}


//===========================================================================
// CGuiScreenIrisWipe::InitOutro
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
void CGuiScreenIrisWipe::InitOutro()
{
    if( m_loadingText != NULL )
    {
        // hide loading text
        //
        m_loadingText->SetVisible( false );
    }

    if( g_doNotOpenOnNextOutro )
    {
        g_doNotOpenOnNextOutro = false;
    }
    else
    {
        g_IsIrisClosed = false;
        if( !m_isIrisActive )
        {
            m_isIrisActive = true;

            rAssertMsg( m_numTransitionsPending == 0, "Bad news if you hit this assert! Please go tell Tony." );
            m_numTransitionsPending++;

            m_pMultiController->Reset();
            m_pMultiController->SetFrameRange( m_numFrames * 0.5f, m_numFrames );
            m_pMultiController->SetFrame( m_numFrames * 0.5f );

            //Reset the relative speed to the default
            m_relativeSpeed = DEFAULT_REL_SPEED;
        }
    }
}

//===========================================================================
// CGuiScreenIrisWipe::IsIrisClosed
//===========================================================================
// Description: Lets you know if the iris is closed at the moment
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
bool CGuiScreenIrisWipe::IsIrisClosed()
{
    return g_IsIrisClosed;
}

//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

