#include <input/FEMouse.h>
#include <presentation/gui/guimanager.h>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/guiuserinputhandler.h>
#include <gameflow/gameflow.h>
#include <contexts/contextenum.h>
#include <contexts/gameplay/gameplaycontext.h>
#include <main/win32platform.h>

#ifdef ENABLE_DYNA_LOADED_IMAGES
const char* DYNAMIC_RESOURCES_DIR = "art\\frontend\\dynaload\\images\\";
#endif

const float WIN_TO_P3D_POINT_SCALE_FACTOR = 2.0f;

/******************************************************************************
    Construction/Destruction
*****************************************************************************/

FEMouse::FEMouse()
:   m_bMoved( false ),
    m_bClickable( true ),
    m_bSelectable( true ),
    m_bMovable( true ),
    m_oldPositionX(0),
    m_oldPositionY(0),
    m_hotSpotType( HOTSPOT_BUTTON ),
    m_horzDir( NO_HORZ_MOVEMENT ),
    m_bClickAndStop(false),
    m_bInGame( false ),
    m_bInGameOverride( false ),
    m_inGamePosX( 0 ),
    m_inGamePosY( 0 ),
    m_buttonDownSelection( 0 )
{
    m_pCursor = new MouseCursor();
    memset( m_button, 0, sizeof(m_button));
}

FEMouse::~FEMouse()
{
    delete m_pCursor;
    m_pCursor = NULL;
}

void FEMouse::InitMouseCursor( tDrawable* pCursor )
{
    m_pCursor->Set( pCursor );
}

eFEMouseHorzDir FEMouse::OnSliderHorizontalClickDrag() const
{
    if (IsLeftButtonDown() &&
        m_hotSpotType == HOTSPOT_SLIDER &&
        m_horzDir != NO_HORZ_MOVEMENT   && 
        m_bMoved == true )
    {
        return m_horzDir;
    }
    else return NO_HORZ_MOVEMENT;
}

bool FEMouse::DidWeMove( int newX, int newY ) const
{
    return ( (newX != m_oldPositionX) || (newY != m_oldPositionY) );
}

void FEMouse::Move( int mouseX, int mouseY, long screenWidth, long screenHeight )
{
    if( !m_bMovable ) return;
    m_bMoved = true;

    // let us know what direction it moved horizontally.
    if( m_oldPositionX < mouseX ) m_horzDir = MDIR_RIGHT;
    else if( m_oldPositionX > mouseX ) m_horzDir = MDIR_LEFT;
    else m_horzDir = NO_HORZ_MOVEMENT;


    m_oldPositionX = mouseX;
    m_oldPositionY = mouseY;

    // Normalize the windows mouse coordinates that are in pixel units, to 2d world units.
    // From ( -1 to 1 ) on the major axis and ( -height/width to -height/width ) on the minor.
    // We must scale the incoming coordinates and offset them to the center.  
    // We must also handle the difference in width and height, to compensate for the aspect 
    // ratio of the display.

    const float ASPECT = (float)screenWidth/(float)screenHeight; // The screen aspect ratio.

    float deltaX = ( (mouseX/( screenWidth/WIN_TO_P3D_POINT_SCALE_FACTOR )) - 1.0f );
    float deltaY = ( 1.0f - (mouseY/(screenHeight/WIN_TO_P3D_POINT_SCALE_FACTOR)))/ASPECT;

    m_pCursor->SetPos( deltaX/WIN_TO_P3D_POINT_SCALE_FACTOR, deltaY/WIN_TO_P3D_POINT_SCALE_FACTOR );
    //rDebugPrintf("Mouse moved to X: %g  Y: %g \n", deltaX, deltaY);
}

void FEMouse::Update()
{
    bool ingame = m_bInGame && ! m_bInGameOverride;
    bool bLoading = (GetGameFlow()->GetCurrentContext() == CONTEXT_LOADING_GAMEPLAY) ||
                    (GetGameFlow()->GetCurrentContext() == CONTEXT_LOADING_SUPERSPRINT) ||
                    (GetGameFlow()->GetCurrentContext() == CONTEXT_EXIT);
    if( ingame || bLoading ) return;
    if( !m_pCursor->IsVisible() ) return;

    CGuiWindow* pCurrentWindow = GetGuiSystem()->GetCurrentManager()->GetCurrentWindow();
    CGuiMenu* pCurrentMenu = pCurrentWindow->HasMenu();
    if( !pCurrentMenu || (pCurrentMenu && !pCurrentMenu->HasSelectionBeenMade()) )//m_bSelectable )
    {

        // Dusit here,
        //   Took out the update only on mouse moved test. 
        //   This is a problem when a new front-end screen item appears underneath
        //   the mouse cursor, but the mouse hasn't moved yet, so it can't click 
        //   on the front-end item. So we either need to get notified when that 
        //   happens and manually update the mouse focus via callback, or just
        //   do it every frame. I think it's reasonable to do it every frame; otherwise, if
        //   the call is really expensive, we'd end up with low framerate while mouse is 
        //   moving and good framerate when mouse is stationary (which is not
        //   a good optimization).
        //
        //if( m_bMoved ) //Only update the hotspot checking if the mouse moved!
        //{
        //
        m_hotSpotType = pCurrentWindow->CheckCursorAgainstHotspots( m_pCursor->XPos()*WIN_TO_P3D_POINT_SCALE_FACTOR, 
                                                                    m_pCursor->YPos()*WIN_TO_P3D_POINT_SCALE_FACTOR);
        if( m_hotSpotType == HOTSPOT_NONE )
        {
            // if the user is still holding onto the button, don't tamper with its state.
            // we deal with that case later (just below).
            if( m_button[ BUTTON_LEFT ] != BUTTON_CLICKHOLD )
            {
                m_button[ BUTTON_LEFT ] = BUTTON_IDLE;  
            }
        }
        m_bMoved = false;
        rAssert( !m_bMoved );

        if( pCurrentMenu )
        {
            if( m_button[ BUTTON_LEFT ] == BUTTON_CLICKHOLD )
            {
                // deal with highlighting the hotspot...
                CGuiUserInputHandler* userInputHandler = GetGuiSystem()->GetUserInputHandler( 0 );
                switch( m_hotSpotType ) 
                {
                case HOTSPOT_BUTTON:
                    if( m_buttonDownSelection == pCurrentMenu->GetSelection() && m_bClickable )
                    {
                        pCurrentMenu->HandleMessage( GUI_MSG_MOUSE_LCLICKHOLD, 1 );
                    }
                    break;
                case HOTSPOT_NONE:
                    if( m_bClickable )
                    {
                        pCurrentMenu->HandleMessage( GUI_MSG_MOUSE_LCLICKHOLD, 0 );
                    }
                    break;
                default: break;
                }
            }
            else
            {
                // If nothing is being held down we should eliminate the highlighting.
                // This fixes the problem where if you hold down LMB then right-click to
                // escape to previous screen, when you go back to the first screen, the
                // highlighting will still be in effect.
                //
                if( m_bClickable && m_bSelectable )
                {
                    pCurrentMenu->HandleMessage( GUI_MSG_MOUSE_LCLICKHOLD, 0 );
                }
            }
        }
    }
    m_pCursor->Render();
}

void FEMouse::ButtonDown( eFEMouseButton buttonType ) 
{ 
    // don't allow clicking when selectable not on
    CGuiMenu* pCurrentMenu = GetGuiSystem()->GetCurrentManager()->GetCurrentWindow()->HasMenu();
    if( !(!pCurrentMenu || (pCurrentMenu && !pCurrentMenu->HasSelectionBeenMade())) )//!m_bSelectable )
    {
        return;
    }

    m_button[buttonType] = BUTTON_CLICKHOLD; 

    // what follows concerns only left mouse button
    if( buttonType != BUTTON_LEFT )
    {
        return;
    }

    // remember what menu item it was that we pressed down on...
    m_buttonDownSelection = pCurrentMenu ? pCurrentMenu->GetSelection() : -1;

    // deal with highlighting the hotspot...
    if( pCurrentMenu )
    {
        CGuiUserInputHandler* userInputHandler = GetGuiSystem()->GetUserInputHandler( 0 );
        switch( m_hotSpotType ) 
        {
        case HOTSPOT_BUTTON:
            if( m_bClickable )
            {
                pCurrentMenu->HandleMessage( GUI_MSG_MOUSE_LCLICKHOLD, 1 );
            }
            break;
        case HOTSPOT_NONE:
            if( m_bClickable )
            {
                pCurrentMenu->HandleMessage( GUI_MSG_MOUSE_LCLICKHOLD, 0 );
            }
            break;
        default: break;
        }
    }
}

void FEMouse::ButtonUp( enum eFEMouseButton buttonType )
{
    // don't allow clicking when selectable not on
    CGuiMenu* pCurrentMenu = GetGuiSystem()->GetCurrentManager()->GetCurrentWindow()->HasMenu();
    if( !(!pCurrentMenu || (pCurrentMenu && !pCurrentMenu->HasSelectionBeenMade())) )//!m_bSelectable )
    {
        return;
    }

    m_button[ buttonType ] = BUTTON_CLICKED;

    // what follows concerns only left mouse button
    if( buttonType != BUTTON_LEFT )
    {
        return;
    }

    /////////////////////////////////////////////////////////////////
    // Deal with the consequence of left-clicking a menu button

    //
    // if we release mouse button on something other than the item we 
    // were on when we pressed the mouse button, we don't do anything
    //
    int currSelection = pCurrentMenu ? pCurrentMenu->GetSelection() : -1;
    if( currSelection != m_buttonDownSelection )
    {
        return; 
    }

    // at this point, the hotspot we're currently on matches the
    // one we were at when we pressed the left mouse button... so click!
    CGuiUserInputHandler* userInputHandler = GetGuiSystem()->GetUserInputHandler( 0 );
    switch( m_hotSpotType ) 
    {
    case HOTSPOT_BUTTON:
        userInputHandler->Select();
        m_bSelectable = false; // need to disable selectable IMMEDIATELY (not when screen exits)
        break;
    case HOTSPOT_ARROWLEFT:
        userInputHandler->Left();
        break;
    case HOTSPOT_ARROWRIGHT:
        userInputHandler->Right();
        break;
    case HOTSPOT_ARROWUP:
        userInputHandler->Up();
        break;
    case HOTSPOT_ARROWDOWN:
        userInputHandler->Down();
        break;
    case HOTSPOT_LTRIGGER:
        userInputHandler->L1();
        break;
    case HOTSPOT_RTRIGGER:
        userInputHandler->R1();
        break;
    default: break;
    }

    // reset left button state
    m_button[ BUTTON_LEFT ] = BUTTON_IDLE;
}

void FEMouse::SetInGameMode( bool ingame )
{
    m_bInGame = ingame;
    SetupInGameMode();
}

void FEMouse::SetInGameOverride( bool override )
{
    m_bInGameOverride = override;
    SetupInGameMode();
}

void FEMouse::SetupInGameMode()
{
    bool ingame = m_bInGame && ! m_bInGameOverride;

    m_pCursor->SetVisible( !ingame );

    if( ingame )
    {
        RECT windowRect;
        GetWindowRect( Win32Platform::GetInstance()->GetHwnd(), &windowRect );

        // Save the old cursor position.
        POINT oldPos;
        GetCursorPos( & oldPos );
        m_inGamePosX = oldPos.x;
        m_inGamePosY = oldPos.y;

        // Center the cursor.
        SetCursorPos( ( windowRect.left + windowRect.right ) / 2,
                      ( windowRect.top + windowRect.bottom ) / 2 );

        // Set up the clipping rectangle.
        windowRect.top += 30;
        windowRect.bottom -= 10;
        windowRect.left += 10;
        windowRect.right -= 10;
        ClipCursor( &windowRect );
    }
    else
    {
        // Remove the clipping rectangle.
        ClipCursor( NULL );

        // Restore the old cursor position.
        if( m_inGamePosX != -1 )
        {
            SetCursorPos( m_inGamePosX, m_inGamePosY );
            m_inGamePosX = -1;
        }
    }
}