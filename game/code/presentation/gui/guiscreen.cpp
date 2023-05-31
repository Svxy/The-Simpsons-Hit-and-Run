//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreen
//
// Description: Implementation of the CGuiScreen class.
//
// Authors:     Darwin Chau
//              Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2000/09/21		DChau		Created
//                  2002/06/06      TChu        Modified for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <memory/classsizetracker.h>
#include <presentation/gui/guiscreen.h>
#include <presentation/gui/guimanager.h>
#include <presentation/gui/utility/specialfx.h>

#include <events/eventmanager.h>

#include <app.h>
#include <screen.h>
#include <page.h>
#include <layer.h>
#include <group.h>
#include <pure3dobject.h>
#include <sprite.h>
#include <text.h>

#include <p3d/anim/multicontroller.hpp>
#include <p3d/utility.hpp>
#include <pddi/pddi.hpp>

#include <raddebug.hpp>

#include <string.h>

#ifdef RAD_WIN32
#include <input/inputmanager.h>
#endif

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//#define PRINTSCREENUPDATES

const char* FOREGROUND_LAYER = "Foreground";
const char* BACKGROUND_LAYER = "Background";
const char* FE_PURE3D_OBJECT = "CamAndSet";

const float DEFAULT_SCREEN_FADE_TIME = 250.0f;  // in msec
const float DEFAULT_SCREEN_ZOOM_TIME = 250.0f;  // in msec
const float DEFAULT_SCREEN_SLIDE_TIME = 250.0f; // in msec
const float DEFAULT_IRIS_WIPE_SPEED = 0.5f;

const float WIDE_SCREEN_CORRECTION_SCALE = (4.0f / 3.0f) * (9.0f / 16.0f);

const char* ACCEPT_PAGES[] =
{
    "Accept",
    "Accept2",
    "Accept3",
    "Buy",
    "Continue",

    "" // dummy terminator
};

const int NUM_ACCEPT_PAGES = sizeof( ACCEPT_PAGES ) / sizeof( ACCEPT_PAGES[ 0 ] );

const char* BACK_PAGES[] =
{
    "Back",
    "Back2",
    "Cancel",

    "" // dummy terminator
};

const int NUM_BACK_PAGES = sizeof( BACK_PAGES ) / sizeof( BACK_PAGES[ 0 ] );

tMultiController* CGuiScreen::s_p3dMultiController = NULL;
float CGuiScreen::s_numIrisFrames = 0.0f;

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreen::CGuiScreen
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
CGuiScreen::CGuiScreen
(
	Scrooby::Screen* pScroobyScreen,
	CGuiEntity* pParent,
    eGuiWindowID id,
    unsigned int screenFX
)
:   CGuiWindow( id, pParent ),	
	m_pScroobyScreen( pScroobyScreen ),
    m_screenCover( NULL ),
    m_p3dObject( NULL ),
    m_p3dIris( NULL ),
    m_irisController( NULL ),
    m_currentIrisState( IRIS_STATE_IDLE ),
    m_autoOpenIris( false ),
    m_numForegroundLayers( 0 ),
    m_numBackgroundLayers( 0 ),
    m_ignoreControllerInputs( false ),
    m_inverseFading( false ),
    m_screenFX( screenFX ),
    m_fadeTime( DEFAULT_SCREEN_FADE_TIME ),
    m_elapsedFadeTime( -1 ),
    m_zoomTime( DEFAULT_SCREEN_ZOOM_TIME ),
    m_elapsedZoomTime( -1 ),
    m_slideTime( DEFAULT_SCREEN_SLIDE_TIME ),
    m_elapsedSlideTime( -1 ),
    m_playTransitionAnimationLast( false )
{
    CLASSTRACKER_CREATE( CGuiScreen );
    memset( m_foregroundLayers, 0, sizeof( m_foregroundLayers ) );
    memset( m_backgroundLayers, 0, sizeof( m_backgroundLayers ) );
    memset( m_buttonIcons, 0, sizeof( m_buttonIcons ) );

    m_guiManager = static_cast<CGuiManager*>( m_pParent );
    rAssert( m_guiManager );

    rAssert( m_pScroobyScreen != NULL );

    Scrooby::Page* pPage = NULL;

    // Get foreground and background layers of all pages in screen
    //
    int numPages = m_pScroobyScreen->GetNumberOfPages();
    for( int i = 0; i < numPages; i++ )
    {
        pPage = m_pScroobyScreen->GetPageByIndex( i );
        rAssert( pPage );

        Scrooby::Layer* pLayer = pPage->GetLayer( FOREGROUND_LAYER );
        if( pLayer != NULL )
        {
            rAssert( m_numForegroundLayers < MAX_FOREGROUND_LAYERS );
            m_foregroundLayers[ m_numForegroundLayers ] = pLayer;
            m_numForegroundLayers++;

            if( this->IsWideScreenDisplay() )
            {
                pLayer->ResetTransformation();
                this->ApplyWideScreenCorrectionScale( pLayer );
            }
        }

        pLayer = pPage->GetLayer( BACKGROUND_LAYER );
        if( pLayer != NULL )
        {
            rAssert( m_numBackgroundLayers < MAX_BACKGROUND_LAYERS );
            m_backgroundLayers[ m_numBackgroundLayers ] = pLayer;
            m_numBackgroundLayers++;
        }
    }

    this->RestoreScreenCover();

    // Get pure3d object from "3dFE" page, if included
    //
    pPage = m_pScroobyScreen->GetPage( "3dFE" );
    if( pPage != NULL )
    {
        m_p3dObject = pPage->GetPure3dObject( FE_PURE3D_OBJECT );
        rAssert( m_p3dObject );
    }

    // Get iris pure3d object from "IrisCover" page, if included
    //
    pPage = m_pScroobyScreen->GetPage( "IrisCover" );
    if( pPage != NULL )
    {
        // set iris layer visible
        //
        Scrooby::Layer* irisLayer = pPage->GetLayer( "IrisCover" );
        rAssert( irisLayer != NULL );
        irisLayer->SetVisible( true );

        m_p3dIris = pPage->GetPure3dObject( "3dIris" );
        rAssert( m_p3dIris != NULL );
        m_p3dIris->SetZBufferEnabled( false );

        m_irisController = p3d::find<tMultiController>( "IrisController" );
        rAssert( m_irisController != NULL );
        m_irisController->AddRef();

        s_numIrisFrames = m_irisController->GetNumFrames();
    }

    // Get button icons
    //
    for( int j = 0; j < NUM_ACCEPT_PAGES; j++ )
    {
        pPage = m_pScroobyScreen->GetPage( ACCEPT_PAGES[ j ] );
        if( pPage != NULL )
        {
            m_buttonIcons[ BUTTON_ICON_ACCEPT ] = pPage->GetGroup( "AcceptLabel" );
            rAssert( m_buttonIcons[ BUTTON_ICON_ACCEPT ] != NULL );

#ifndef RAD_WIN32
            // add text outline to accept text
            //
            Scrooby::Text* accept = m_buttonIcons[ BUTTON_ICON_ACCEPT ]->GetText( "Accept" );
            rAssert( accept != NULL );
            accept->SetDisplayOutline( true );
#endif

            break;
        }
    }

    for( int k = 0; k < NUM_BACK_PAGES; k++ )
    {
        pPage = m_pScroobyScreen->GetPage( BACK_PAGES[ k ] );
        if( pPage != NULL )
        {
            m_buttonIcons[ BUTTON_ICON_BACK ] = pPage->GetGroup( "BackLabel" );
            rAssert( m_buttonIcons[ BUTTON_ICON_BACK ] != NULL );

#ifndef RAD_WIN32
            // add text outline to accept text
            //
            Scrooby::Text* back = m_buttonIcons[ BUTTON_ICON_BACK ]->GetText( "Back" );
            rAssert( back != NULL );
            back->SetDisplayOutline( true );
#endif

            break;
        }
    }
}

//===========================================================================
// CGuiScreen::~CGuiScreen
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
CGuiScreen::~CGuiScreen()
{
    CLASSTRACKER_DESTROY( CGuiScreen );
    if( m_irisController != NULL )
    {
        m_irisController->Release();
        m_irisController = NULL;
    }
}


//===========================================================================
// CGuiScreen::HandleMessage
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void CGuiScreen::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2
)
{
    if( m_ignoreControllerInputs &&
        this->IsControllerMessage( message ) )
    {
        // ignore controller messages
        return;
    }


#ifdef RAD_WIN32
    if( message == GUI_MSG_WINDOW_ENTER )
    {
        // just entered screen, so re-enable mouse
        GetInputManager()->GetFEMouse()->SetSelectable( true );
    }
    else if( message == GUI_MSG_WINDOW_EXIT )
    {
        // exiting screen, so disable mouse
        GetInputManager()->GetFEMouse()->SetSelectable( false );
    }
#endif

    switch( message )
    {
        case GUI_MSG_WINDOW_ENTER:
        {
            // reset any pending transitions
            m_numTransitionsPending = 0;

            if( !m_inverseFading )
            {
                if( m_screenFX & SCREEN_FX_FADE )
                {
                    this->SetAlphaForLayers( 0.0f,
                                             m_foregroundLayers,
                                             m_numForegroundLayers );
                }
            }

            if( m_screenFX & SCREEN_FX_ZOOM )
            {
                this->SetAlphaForLayers( 0.0f,
                                         m_backgroundLayers,
                                         m_numBackgroundLayers );
            }

            if( m_screenFX & SCREEN_FX_IRIS )
            {
                this->SetAlphaForLayers( 0.0f,
                                         m_foregroundLayers,
                                         m_numForegroundLayers );

                this->SetAlphaForLayers( 0.0f,
                                         m_backgroundLayers,
                                         m_numBackgroundLayers );
            }

            // restore all button icon visibilities
            this->RestoreButtons();

            // follow-through
            //
        }
        case GUI_MSG_WINDOW_EXIT:
        {
            if( m_screenFX & SCREEN_FX_FADE )
            {
                if( m_numForegroundLayers > 0 || m_screenCover != NULL )
                {
                    // reset fade elapsed time
                    m_elapsedFadeTime = 0;

                    // increment number of pending transitions
                    m_numTransitionsPending++;
                }
            }

            if( m_screenFX & SCREEN_FX_ZOOM )
            {
                // reset zoom elapsed time
                m_elapsedZoomTime = 0;

                // increment number of pending transitions
                m_numTransitionsPending++;
            }

			if( m_screenFX & SCREEN_FX_SLIDE_X ||
                m_screenFX & SCREEN_FX_SLIDE_Y )
			{
                m_elapsedSlideTime = 0;
				m_numTransitionsPending++;
			}

            if( m_screenFX & SCREEN_FX_IRIS )
            {
                rAssert( m_irisController != NULL );
                m_irisController->Reset();
                m_irisController->SetRelativeSpeed( DEFAULT_IRIS_WIPE_SPEED );
                m_irisController->SetFrameRange( 0.0f, s_numIrisFrames );
                m_irisController->SetFrame( 0.0f );

                m_currentIrisState = IRIS_STATE_CLOSING;

                m_numTransitionsPending++;
            }
            break;
        }

        case GUI_MSG_UPDATE:
        {
            #ifdef PRINTSCREENUPDATES
                const char* screenName = GetWatcherName();
                rDebugPrintf( "Screen - %s\n", screenName );
            #endif

            switch( m_state )
            {
                case GUI_WINDOW_STATE_INTRO:
                {
                    // update screen fade in
                    //
                    if( (m_screenFX & SCREEN_FX_FADE) &&
                        m_elapsedFadeTime != -1 )
                    {
                        if( m_inverseFading )
                        {
                            this->FadeOut( static_cast<float>( param1 ) );
                        }
                        else
                        {
                            this->FadeIn( static_cast<float>( param1 ) );
                        }
                    }

                    // update screen zoom in
                    //
                    if( (m_screenFX & SCREEN_FX_ZOOM) &&
                        m_elapsedZoomTime != -1 )
                    {
                        this->ZoomIn( static_cast<float>( param1 ) );
                    }

					// update screen slide in
					//
					if( ((m_screenFX & SCREEN_FX_SLIDE_X) || (m_screenFX & SCREEN_FX_SLIDE_Y)) &&
                        m_elapsedSlideTime != -1 )
                    {
                        this->SlideIn( static_cast<float>( param1 ) );
                    }

                    // update transition in animation
                    //
                    if( m_p3dObject != NULL )
                    {
                        tMultiController* controller = m_p3dObject->GetMultiController();
                        if( controller != NULL )
                        {
                            if( controller->GetFrame() >= controller->GetNumFrames() )
                            {
                                // nullify multicontroller
                                m_p3dObject->SetMultiController( NULL );

                                // decrement number of pending transitions
                                m_numTransitionsPending--;
                            }
                        }
                    }

                    // update iris wipe
                    //
                    if( m_screenFX & SCREEN_FX_IRIS )
                    {
                        rAssert( m_irisController != NULL );
                        if( m_currentIrisState == IRIS_STATE_CLOSING )
                        {
                            if( m_irisController->GetFrame() > m_irisController->GetNumFrames() / 2 )
                            {
                                this->OnIrisWipeClosed();

                                this->SetAlphaForLayers( 1.0f,
                                                         m_foregroundLayers,
                                                         m_numForegroundLayers );

                                this->SetAlphaForLayers( 1.0f,
                                                         m_backgroundLayers,
                                                         m_numBackgroundLayers );
                            }
                        }
                        else if( m_currentIrisState == IRIS_STATE_OPENING )
                        {
                            if( m_irisController->LastFrameReached() )
                            {
                                m_numTransitionsPending--;

                                m_currentIrisState = IRIS_STATE_IDLE;
                            }
                        }
                    }

                    // get p3d multicontroller upon first time entering screen
                    // and store reference to it
                    //
                    if( m_firstTimeEntered )
                    {
                        if( m_p3dObject != NULL &&
                            s_p3dMultiController == NULL )
                        {
                            s_p3dMultiController = m_p3dObject->GetMultiController();
                            if( s_p3dMultiController != NULL )
                            {
                                s_p3dMultiController->AddRef();

                                m_p3dObject->SetMultiController( NULL );
                            }
                        }
                    }

                    break;
                }
                case GUI_WINDOW_STATE_OUTRO:
                {
                    // update screen fade out
                    //
                    if( (m_screenFX & SCREEN_FX_FADE) &&
                        m_elapsedFadeTime != -1 )
                    {
                        if( m_inverseFading )
                        {
                            this->FadeIn( static_cast<float>( param1 ) );
                        }
                        else
                        {
                            this->FadeOut( static_cast<float>( param1 ) );
                        }
                    }

                    // update screen zoom out
                    //
                    if( (m_screenFX & SCREEN_FX_ZOOM) &&
                        m_elapsedZoomTime != -1 )
                    {
                        this->ZoomOut( static_cast<float>( param1 ) );
                    }

					// update screen slide out
					//
					if( ((m_screenFX & SCREEN_FX_SLIDE_X) || (m_screenFX & SCREEN_FX_SLIDE_Y)) &&
                        m_elapsedSlideTime != -1 )
                    {
                        this->SlideOut( static_cast<float>( param1 ) );
                    }

                    // update transition out animation
                    //
                    if( m_p3dObject != NULL )
                    {
                        if( m_playTransitionAnimationLast )
                        {
                            if( m_numTransitionsPending == 1 )
                            {
                                s_p3dMultiController->Reset();
                                m_p3dObject->SetMultiController( s_p3dMultiController );
                                m_playTransitionAnimationLast = false;
                            }
                        }
                        else
                        {
                            tMultiController* controller = m_p3dObject->GetMultiController();
                            if( controller != NULL )
                            {
                                if( controller->GetFrame() >= controller->GetNumFrames() )
                                {
                                    // nullify multicontroller
                                    m_p3dObject->SetMultiController( NULL );

                                    // decrement number of pending transitions
                                    m_numTransitionsPending--;
                                }
                            }
                        }
                    }

                    // update iris wipe
                    //
                    if( m_screenFX & SCREEN_FX_IRIS )
                    {
                        rAssert( m_irisController != NULL );
                        if( m_currentIrisState == IRIS_STATE_CLOSING )
                        {
                            if( m_irisController->GetFrame() > m_irisController->GetNumFrames() / 2 )
                            {
                                this->OnIrisWipeClosed();

                                this->SetAlphaForLayers( 0.0f,
                                                         m_foregroundLayers,
                                                         m_numForegroundLayers );

                                this->SetAlphaForLayers( 0.0f,
                                                         m_backgroundLayers,
                                                         m_numBackgroundLayers );

                                m_currentIrisState = IRIS_STATE_CLOSED;
                            }
                        }
                        else if( m_currentIrisState == IRIS_STATE_OPENING )
                        {
                            if( m_irisController->LastFrameReached() )
                            {
                                m_numTransitionsPending--;

                                m_currentIrisState = IRIS_STATE_IDLE;
                            }
                        }
                    }

                    break;
                }

                default:
                {
                    break;
                }
            }

            break;
        }

        case GUI_MSG_CONTROLLER_BACK:
        {
            if( m_state == GUI_WINDOW_STATE_RUNNING )
            {
                if( m_guiManager->GetPreviousScreen() != GUI_WINDOW_ID_UNDEFINED )
                {
                    m_pParent->HandleMessage( GUI_MSG_BACK_SCREEN );
                    GetEventManager()->TriggerEvent( EVENT_FE_MENU_BACK );
                }
            }

            break;
        }

        default:
        {
            break;
        }
    };

    // Pass the message up the heirarchy.
    //
    CGuiWindow::HandleMessage( message, param1, param2 );
}

void
CGuiScreen::SetFadingEnabled( bool enable )
{
    if( enable )
    {
        m_screenFX |= SCREEN_FX_FADE;
    }
    else
    {
        m_screenFX &= ~SCREEN_FX_FADE;

        this->SetAlphaForLayers( 1.0f,
                                 m_foregroundLayers,
                                 m_numForegroundLayers );

        if( m_screenCover != NULL )
        {
            m_screenCover->SetVisible( false );
        }
    }
}

void
CGuiScreen::SetZoomingEnabled( bool enable )
{
    if( enable )
    {
        m_screenFX |= SCREEN_FX_ZOOM;
    }
    else
    {
        m_screenFX &= ~SCREEN_FX_ZOOM;

        m_pScroobyScreen->SetScale( 1.0f );

        this->SetAlphaForLayers( 1.0f,
                                 m_backgroundLayers,
                                 m_numBackgroundLayers );
    }
}

void
CGuiScreen::SetSlidingEnabled( eScreenEffect slideType, bool enable )
{
    rAssert( slideType == SCREEN_FX_SLIDE_X ||
             slideType == SCREEN_FX_SLIDE_Y );

    rAssertMsg( !this->IsWideScreenDisplay() || slideType != SCREEN_FX_SLIDE_X,
                "Horizontal screen sliding currently not supported for widescreen display!" );

    if( enable )
    {
        m_screenFX |= slideType;
    }
    else
    {
        m_screenFX &= ~slideType;
    }
}

void
CGuiScreen::SetIrisWipeEnabled( bool enable, bool autoOpenIris )
{
    if( enable )
    {
        m_screenFX |= SCREEN_FX_IRIS;

        m_autoOpenIris = autoOpenIris;
    }
    else
    {
        m_screenFX &= ~SCREEN_FX_IRIS;
    }
}

bool
CGuiScreen::IsEffectEnabled( eScreenEffect effect ) const
{
    return ((m_screenFX & effect) > 0);
}

void
CGuiScreen::Reset3dFEMultiController()
{
    if( s_p3dMultiController != NULL )
    {
        s_p3dMultiController->Release();
        s_p3dMultiController = NULL;
    }
}

void
CGuiScreen::SetButtonVisible( eButtonIcon button, bool isVisible )
{
    if( m_buttonIcons[ button ] != NULL )
    {
        m_buttonIcons[ button ]->SetVisible( isVisible );
    }
}

bool
CGuiScreen::IsButtonVisible( eButtonIcon button ) const
{
    if( m_buttonIcons[ button ] != NULL )
    {
        return m_buttonIcons[ button ]->IsVisible();
    }

    return false;
}

void
CGuiScreen::StartTransitionAnimation( int startFrame,
                                      int endFrame,
                                      bool lastTransition )
{
    if( m_ignoreControllerInputs )
    {
        return;
    }

    if( m_p3dObject != NULL )
    {
        rAssert( s_p3dMultiController != NULL );

        // if start and end frames are specified, set them for multicontroller
        if( startFrame > -1 && endFrame > -1 )
        {
            s_p3dMultiController->SetFrameRange( (float)startFrame, (float)endFrame );
        }

        m_playTransitionAnimationLast = lastTransition;
        if( !m_playTransitionAnimationLast )
        {
            // set multicontroller for p3dobject
            s_p3dMultiController->Reset();
            m_p3dObject->SetMultiController( s_p3dMultiController );
        }

        // increment number of pending transitions
        m_numTransitionsPending++;
    }
}

void
CGuiScreen::ReloadScreen()
{
    m_pParent->HandleMessage( GUI_MSG_GOTO_SCREEN,
                              this->m_ID,
                              KEEP_WINDOW_HISTORY );
}

void
CGuiScreen::RestoreScreenCover()
{
    // Get screen cover, if Cover page included
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "COVER" );
    if( pPage != NULL )
    {
        m_screenCover = pPage->GetLayerByIndex( 0 );
        rAssert( m_screenCover != NULL );

        m_screenCover->SetVisible( true );
        m_screenCover->SetAlpha( 1.0f );
    }
}

void
CGuiScreen::RestoreButtons()
{
    for( int i = 0; i < NUM_BUTTON_ICONS; i++ )
    {
        if( m_buttonIcons[ i ] != NULL )
        {
            m_buttonIcons[ i ]->SetVisible( true );
        }
    }
}

bool
CGuiScreen::IsWideScreenDisplay()
{
#ifdef RAD_XBOX
    return p3d::display->IsWidescreen();
#else
    return false;
#endif
}

void
CGuiScreen::ApplyWideScreenCorrectionScale( Scrooby::Drawable* drawable )
{
    // Assumes drawable is currently screen-centered!
    //
    int screenWidthBy2 = static_cast<int>( Scrooby::App::GetInstance()->GetScreenWidth() ) / 2;

    rAssert( drawable != NULL );
    drawable->Translate( -screenWidthBy2, 0 );
    drawable->Scale( WIDE_SCREEN_CORRECTION_SCALE, 1.0f, 1.0f );
    drawable->Translate( screenWidthBy2, 0 );
}

#ifdef RAD_WIN32
//===========================================================================
// CGuiScreen::CheckCursorAgainstHotspots
//===========================================================================
// Description: Checks cursor position against its list of hotspots.
//
// Constraints: None.
//
// Parameters:  float x - The x position of cursor in P3D coordinates.
//              float y - The y position of cursor in P3D coordinates.
//
// Return:      N/A.
//
//===========================================================================
eFEHotspotType CGuiScreen::CheckCursorAgainstHotspots( float x, float y )
{
    eFEHotspotType hotSpotType = HOTSPOT_NONE;
    CGuiMenu* pCurrentMenu = HasMenu();
    int numMenuItems = 0;
    GuiMenuItem* pMenuItem = NULL;


    if( pCurrentMenu )
    {
        numMenuItems = pCurrentMenu->GetNumItems();

        for( int i = 0; i < numMenuItems; i++ )
        {
            bool bIsMenuItemEnabled = pCurrentMenu->IsMenuItemEnabled(i);
            pMenuItem = pCurrentMenu->GetMenuItem( i );

            if( pMenuItem && bIsMenuItemEnabled )
            {
                if( pMenuItem->GetItem()->IsVisible() )
                {
                    // Just tests if the point is in the bounding rect of the sprite.
                    if( pMenuItem->GetItem()->IsPointInBoundingRect( x, y ) )
                    {
                        //rDebugPrintf( "Cursor is inside Sprite %d rectangle!\n", i );
                        pCurrentMenu->HandleMessage( GUI_MSG_MOUSE_OVER, i );
                        hotSpotType = HOTSPOT_BUTTON;

                        //After taking care of all the events for this menu item, just bail out.
                        break;
                    }
                    else if( pCurrentMenu->GetSelection() == i )
                    {
                        if( pMenuItem->m_itemValueArrowL )
                        {
                            if( pMenuItem->m_itemValueArrowL->IsPointInBoundingRect( x, y ) )
                            {
                                hotSpotType = HOTSPOT_ARROWLEFT;
                                break;
                            }
                        }

                        if( pMenuItem->m_itemValueArrowR )
                        {
                            if( pMenuItem->m_itemValueArrowR->IsPointInBoundingRect( x, y ) )
                            {
                                hotSpotType = HOTSPOT_ARROWRIGHT;
                                break;
                            }
                        }

                        if( pMenuItem->m_slider.m_pImage )
                        {
                            if( pMenuItem->m_slider.m_pImage->IsPointInBoundingRect( x, y ) )
                            {
                                hotSpotType = HOTSPOT_SLIDER;
                                break;
                            }
                        }
                    }
                }
            }
        }           
    }

    return hotSpotType;
}
#endif

//===========================================================================
// Protected Member Functions
//===========================================================================

void
CGuiScreen::RestoreDefaultFadeTime()
{
    m_fadeTime = DEFAULT_SCREEN_FADE_TIME;
}

void
CGuiScreen::RestoreDefaultZoomTime()
{
    m_zoomTime = DEFAULT_SCREEN_ZOOM_TIME;
}

void
CGuiScreen::IrisWipeOpen()
{
    rAssert( !m_autoOpenIris );

    rAssert( m_irisController != NULL );
    m_irisController->SetRelativeSpeed( DEFAULT_IRIS_WIPE_SPEED );

    m_currentIrisState = IRIS_STATE_OPENING;
}

void CGuiScreen::SetAlphaForLayers( float alpha, Scrooby::Layer** layers, int numLayers )
{
    rAssert( layers );
    rAssert( numLayers >= 0 );

    for( int i = 0; i < numLayers; i++ )
    {
        layers[ i ]->SetAlpha( alpha );
    }
}

void
CGuiScreen::AutoScaleFrame( Scrooby::Page* pPage )
{
    if( pPage != NULL )
    {
        Scrooby::Group* pFrameGroup = pPage->GetGroup( "Frame" );
        if( pFrameGroup != NULL )
        {
            const int SEGMENT_LENGTH = 40; // in pixels

            Scrooby::Sprite* pFrameTop = pFrameGroup->GetSprite( "Frame_Top" );
            rAssert( pFrameTop != NULL );
            Scrooby::Sprite* pFrameBottom = pFrameGroup->GetSprite( "Frame_Bottom" );
            rAssert( pFrameBottom != NULL );
            Scrooby::Sprite* pFrameLeft = pFrameGroup->GetSprite( "Frame_Left" );
            rAssert( pFrameLeft != NULL );
            Scrooby::Sprite* pFrameRight = pFrameGroup->GetSprite( "Frame_Right" );
            rAssert( pFrameRight != NULL );

            int width = 0;
            int height = 0;

            // scale top and bottom frames horizontally
            //
            pFrameTop->GetBoundingBoxSize( width, height );
            float scaleX = (width - SEGMENT_LENGTH) / (float)SEGMENT_LENGTH;

            pFrameTop->ResetTransformation();
            pFrameTop->ScaleAboutCenter( scaleX, 1.0f, 1.0f );
            pFrameTop->Translate( +5, 0 );

            pFrameBottom->ResetTransformation();
            pFrameBottom->ScaleAboutCenter( scaleX, 1.0f, 1.0f );
            pFrameBottom->Translate( +5, 0 );

            // scale left and right frames vertically
            //
            pFrameLeft->GetBoundingBoxSize( width, height );
            float scaleY = (height - SEGMENT_LENGTH) / (float)SEGMENT_LENGTH;

            pFrameLeft->ResetTransformation();
            pFrameLeft->ScaleAboutCenter( 1.0f, scaleY, 1.0f );

            pFrameRight->ResetTransformation();
            pFrameRight->ScaleAboutCenter( 1.0f, scaleY, 1.0f );

#ifdef PAL
            if( pPage == m_pScroobyScreen->GetPage( "BigBoard" ) )
            {
                pFrameGroup->ResetTransformation();
                pFrameGroup->ScaleAboutCenter( 1.035f, 1.0f, 1.0f );
            }
#endif // PAL
        }
    }
}

//===========================================================================
// Private Member Functions
//===========================================================================

void
CGuiScreen::FadeIn( float elapsedTime )
{
    float alpha = (m_elapsedFadeTime + elapsedTime) / m_fadeTime;

    if( alpha < 1.0f )
    {
        // for non-linear fading
        //
        alpha *= alpha;

        this->SetAlphaForLayers( alpha,
                                 m_foregroundLayers,
                                 m_numForegroundLayers );

        if( m_screenCover != NULL )
        {
            m_screenCover->SetVisible( true );
            m_screenCover->SetAlpha( 1.0f - alpha );
        }
    }
    else
    {
        this->SetAlphaForLayers( 1.0f,
                                 m_foregroundLayers,
                                 m_numForegroundLayers );

        if( m_screenCover != NULL )
        {
            m_screenCover->SetAlpha( 0.0f );
        }
    }

    if( m_elapsedFadeTime < m_fadeTime )
    {
        m_elapsedFadeTime += elapsedTime;
    }
    else
    {
        m_elapsedFadeTime = -1;

        // decrement number of pending transitions
        m_numTransitionsPending--;
    }
}

void
CGuiScreen::FadeOut( float elapsedTime )
{
    float alpha = 1.0f - (m_elapsedFadeTime + elapsedTime) / m_fadeTime;

    if( alpha > 0.0f )
    {
        // for non-linear fading
        //
        alpha *= alpha;

        this->SetAlphaForLayers( alpha,
                                 m_foregroundLayers,
                                 m_numForegroundLayers );

        if( m_screenCover != NULL )
        {
            m_screenCover->SetVisible( true );
            m_screenCover->SetAlpha( 1.0f - alpha );
        }
    }
    else
    {
        this->SetAlphaForLayers( 0.0f,
                                 m_foregroundLayers,
                                 m_numForegroundLayers );

        if( m_screenCover != NULL )
        {
            m_screenCover->SetAlpha( 1.0f );
        }
    }

    if( m_elapsedFadeTime < m_fadeTime )
    {
        m_elapsedFadeTime += elapsedTime;
    }
    else
    {
        m_elapsedFadeTime = -1;

        // decrement number of pending transitions
        m_numTransitionsPending--;
    }
}

void
CGuiScreen::ZoomIn( float elapsedTime )
{
    rAssert( m_pScroobyScreen );

    float zoomValue = (m_elapsedZoomTime + elapsedTime) / m_zoomTime;
    rAssert( zoomValue >= 0.0f );

    if( zoomValue < 1.0f )
    {
        // scale screen
        m_pScroobyScreen->SetScale( zoomValue );

        // apply alpha to background layers
        this->SetAlphaForLayers( zoomValue,
                                 m_backgroundLayers,
                                 m_numBackgroundLayers );
    }
    else
    {
        m_pScroobyScreen->SetScale( 1.0f );

        this->SetAlphaForLayers( 1.0f,
                                 m_backgroundLayers,
                                 m_numBackgroundLayers );
    }

    if( m_elapsedZoomTime < m_zoomTime )
    {
        m_elapsedZoomTime += elapsedTime;
    }
    else
    {
        m_elapsedZoomTime = -1;

        // decrement number of pending transitions
        m_numTransitionsPending--;
    }
}

void
CGuiScreen::ZoomOut( float elapsedTime )
{
    rAssert( m_pScroobyScreen );

    float zoomValue = 1.0f - (m_elapsedZoomTime + elapsedTime) / m_zoomTime;
    rAssert( zoomValue <= 1.0f );

    if( zoomValue > 0.0f )
    {
        // scale screen
        m_pScroobyScreen->SetScale( zoomValue );

        // apply alpha to background layers
        this->SetAlphaForLayers( zoomValue,
                                 m_backgroundLayers,
                                 m_numBackgroundLayers );
    }
    else
    {
        m_pScroobyScreen->SetScale( 0.0f );

        this->SetAlphaForLayers( 0.0f,
                                 m_backgroundLayers,
                                 m_numBackgroundLayers );
    }

    if( m_elapsedZoomTime < m_zoomTime )
    {
        m_elapsedZoomTime += elapsedTime;
    }
    else
    {
        m_elapsedZoomTime = -1;

        // decrement number of pending transitions
        m_numTransitionsPending--;
    }
}

void
CGuiScreen::SlideIn( float elapsedTime )
{
    bool done = false;

    for( int i = 0; i < m_numForegroundLayers; i++ )
    {
        rAssert( m_foregroundLayers[ i ] );

        if( m_screenFX & SCREEN_FX_SLIDE_X )
        {
            done = GuiSFX::SlideX( m_foregroundLayers[ i ],
                                   m_elapsedSlideTime,
                                   m_slideTime,
                                   true,
                                   GuiSFX::SLIDE_BORDER_RIGHT );
        }

        if( m_screenFX & SCREEN_FX_SLIDE_Y )
        {
            done = GuiSFX::SlideY( m_foregroundLayers[ i ],
                                   m_elapsedSlideTime,
                                   m_slideTime,
                                   true,
                                   GuiSFX::SLIDE_BORDER_TOP );

            if( this->IsWideScreenDisplay() )
            {
                this->ApplyWideScreenCorrectionScale( m_foregroundLayers[ i ] );
            }
        }
    }

    if( done )
    {
        m_elapsedSlideTime = -1;
        m_numTransitionsPending--;
    }
    else
    {
        m_elapsedSlideTime += elapsedTime;
    }
}

void
CGuiScreen::SlideOut( float elapsedTime )
{
    bool done = false;

    for( int i = 0; i < m_numForegroundLayers; i++ )
    {
        rAssert( m_foregroundLayers[ i ] );

        if( m_screenFX & SCREEN_FX_SLIDE_X )
        {
            done = GuiSFX::SlideX( m_foregroundLayers[ i ],
                                   m_elapsedSlideTime,
                                   m_slideTime,
                                   false,
                                   GuiSFX::SLIDE_BORDER_LEFT );
        }

        if( m_screenFX & SCREEN_FX_SLIDE_Y )
        {
            done = GuiSFX::SlideY( m_foregroundLayers[ i ],
                                   m_elapsedSlideTime,
                                   m_slideTime,
                                   false,
                                   GuiSFX::SLIDE_BORDER_TOP );

            if( this->IsWideScreenDisplay() )
            {
                this->ApplyWideScreenCorrectionScale( m_foregroundLayers[ i ] );
            }
        }
    }

    if( done )
    {
        m_elapsedSlideTime = -1;
        m_numTransitionsPending--;
    }
    else
    {
        m_elapsedSlideTime += elapsedTime;
    }
}

void
CGuiScreen::OnIrisWipeClosed()
{
    if( m_autoOpenIris )
    {
        this->IrisWipeOpen();
    }
    else
    {
        rAssert( m_irisController != NULL );
        m_irisController->SetRelativeSpeed( 0.0f );

        m_currentIrisState = IRIS_STATE_CLOSED;
    }
}

#ifdef DEBUGWATCH
const char* CGuiScreen::GetWatcherName() const
{
    return "No Name";
}
#endif