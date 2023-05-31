//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenSkinGallery
//
// Description: Implementation of the CGuiScreenSkinGallery class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/03/21      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/frontend/guiscreenskingallery.h>
#include <presentation/gui/frontend/guiscreenscrapbookcontents.h>
#include <presentation/gui/utility/specialfx.h>
#include <presentation/gui/guimanager.h>
#include <presentation/gui/guitextbible.h>
#include <presentation/gui/guimenu.h>

#include <cheats/cheatinputsystem.h>
#include <memory/srrmemory.h>
#include <mission/rewards/rewardsmanager.h>
#include <mission/rewards/reward.h>
#include <events/eventmanager.h>

#include <p3d/inventory.hpp>

#include <raddebug.hpp> // Foundation

#include <screen.h>
#include <page.h>
#include <layer.h>
#include <group.h>
#include <text.h>

#include <string.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

const char* SKIN_GALLERY_INVENTORY_SECTION = "FE_SkinGallery";
const char* SKIN_GALLERY_IMAGES_DIR = "art\\frontend\\dynaload\\images\\skins2d\\";

const float SKIN_VIEW_TRANSITION_TIME = 250.0f; // in msec
const float SKIN_VIEW_PROJECTILE_GRAVITY = 0.005f; // in m/ms/ms
const int SKIN_VIEW_POS_X = 250;
const int SKIN_VIEW_POS_Y = 130;

#ifdef RAD_WIN32
const float SKIN_BASE_SCALE = 1.0f / 2.5f;
const float SKIN_IMAGE_SCALE = 0.5f / 2.5f;
#else
const float SKIN_BASE_SCALE = 1.0f;
const float SKIN_IMAGE_SCALE = 1.5f - 1.0f;
#endif

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenSkinGallery::CGuiScreenSkinGallery
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
CGuiScreenSkinGallery::CGuiScreenSkinGallery
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_SKIN_GALLERY ),
    m_pMenu( NULL ),
    m_numSelections( 0 ),
    m_isSkinsLoaded( false ),
    m_screenState( SCREEN_STATE_NORMAL ),
    m_elapsedTime( 0 ),
    m_projectileVelocity( 0.0f, 0.0f, 0.0f ),
    m_skinInfo( NULL ),
#ifdef RAD_WIN32
    m_selectedSkin(0),
#endif
    m_skinName( NULL )
{
    memset( m_rewardSelections, 0, sizeof( m_rewardSelections ) );

    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "SkinGallery" );
    rAssert( pPage != NULL );

    // create a 2D sprite menu
    //
    m_pMenu = new CGuiMenu2D( this, MAX_NUM_SKINS_PER_LEVEL, 3, GUI_SPRITE_MENU, MENU_SFX_NONE );
    rAssert( m_pMenu != NULL );
    m_pMenu->SetGreyOutEnabled( false );

    Scrooby::Group* pGroup = pPage->GetGroup( "Menu" );
    rAssert( pGroup != NULL );

    // add sprites to menu
    //
    for( int i = 0; i < MAX_NUM_SKINS_PER_LEVEL; i++ )
    {
        char name[ 16 ];
        sprintf( name, "Skin%d", i );

        m_pMenu->AddMenuItem( pGroup->GetSprite( name ) );
    }

    // add menu cursor
    //
    m_pMenu->SetCursor( pGroup->GetSprite( "SkinCursor" ) );

    // get skin info layer and name
    //
    m_skinInfo = pPage->GetLayer( "ViewSkin" );
    m_skinName = pPage->GetText( "SkinName" );

    // hide skin info layer by default
    //
    rAssert( m_skinInfo != NULL );
    m_skinInfo->SetVisible( false );
    m_skinInfo->SetAlpha( 0.0f );

    if( this->IsWideScreenDisplay() )
    {
        m_skinInfo->ResetTransformation();
        this->ApplyWideScreenCorrectionScale( m_skinInfo );
    }

    // wrap skin name
    //
//    rAssert( m_skinName != NULL );
//    m_skinName->SetTextMode( Scrooby::TEXT_WRAP );

    // add outline to skin name
    //
    m_skinName->SetDisplayOutline( true );

    // create inventory section for skin galllery resources
    //
    p3d::inventory->AddSection( SKIN_GALLERY_INVENTORY_SECTION );
}


//===========================================================================
// CGuiScreenSkinGallery::~CGuiScreenSkinGallery
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
CGuiScreenSkinGallery::~CGuiScreenSkinGallery()
{
    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }

    // destroy skin gallery inventory section
    //
    p3d::inventory->DeleteSection( SKIN_GALLERY_INVENTORY_SECTION );
}


//===========================================================================
// CGuiScreenSkinGallery::HandleMessage
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
void CGuiScreenSkinGallery::HandleMessage
(
	eGuiMessage message,
	unsigned int param1,
	unsigned int param2
)
{
    if( m_screenState == SCREEN_STATE_GOTO_VIEW ||
        m_screenState == SCREEN_STATE_BACK_VIEW )
    {
        if( message == GUI_MSG_UPDATE )
        {
            this->OnUpdate( param1 );
        }

        return;
    }

    if( m_screenState == SCREEN_STATE_VIEWING )
    {
        if( message == GUI_MSG_UPDATE )
        {
            this->OnUpdate( param1 );
        }
        else if( message == GUI_MSG_CONTROLLER_BACK )
        {
            m_screenState = SCREEN_STATE_BACK_VIEW;
            m_elapsedTime = 0;

            GetEventManager()->TriggerEvent( EVENT_FE_MENU_BACK ); // sound effect
        }

        return;
    }

    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        switch( message )
        {
            case GUI_MSG_UPDATE:
            {
                this->OnUpdate( param1 );

                break;
            }
            case GUI_MSG_MENU_SELECTION_CHANGED:
            {
                this->OnMenuSelectionChange( static_cast<int>( param1 ) );

                break;
            }
            case GUI_MSG_MENU_SELECTION_MADE:
            {
                this->OnMenuSelectionMade( static_cast<int>( param1 ) );
#ifdef RAD_WIN32
                m_selectedSkin = static_cast<int>( param1 );
                // Hide/disable all other menu items.
                this->SetVisibilityForAllOtherMenuItems( false );
#endif

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
// CGuiScreenSkinGallery::OnProcessRequestsComplete
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
void
CGuiScreenSkinGallery::OnProcessRequestsComplete( void* pUserData )
{
    m_numTransitionsPending--;

    p3d::pddi->DrawSync();

    // push and select inventory section for searching
    //
    p3d::inventory->PushSection();
    p3d::inventory->SelectSection( SKIN_GALLERY_INVENTORY_SECTION );
    bool currentSectionOnly = p3d::inventory->GetCurrentSectionOnly();
    p3d::inventory->SetCurrentSectionOnly( true );

    // update all 3D models
    //
    CGuiScreenScrapBookContents* pScreen = static_cast<CGuiScreenScrapBookContents*>( m_guiManager->FindWindowByID( GUI_SCREEN_ID_SCRAP_BOOK_CONTENTS ) );
    rAssert( pScreen != NULL );

    for( int i = 0; i < m_numSelections; i++ )
    {
        if( (m_pMenu->GetMenuItem( i )->m_attributes & SELECTION_ENABLED) > 0 )
        {
            char name[ 16 ];
            sprintf( name, "%s.png", m_rewardSelections[ i ]->GetName() );
            tSprite* pSprite = p3d::find<tSprite>( name );
            if( pSprite != NULL )
            {
                Scrooby::Sprite* skinImage = dynamic_cast<Scrooby::Sprite*>( m_pMenu->GetMenuItem( i )->GetItem() );
                rAssert( skinImage != NULL );
                skinImage->SetRawSprite( pSprite, true );
            }
            else
            {
                rAssertMsg( false, "Skin image not found!" );
            }
        }
    }

    // pop inventory section and restore states
    //
    p3d::inventory->SetCurrentSectionOnly( currentSectionOnly );
    p3d::inventory->PopSection();

    m_isSkinsLoaded = true;
}

//===========================================================================
// CGuiScreenSkinGallery::InitIntro
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
void CGuiScreenSkinGallery::InitIntro()
{
    if( !m_isSkinsLoaded )
    {
        this->Load2DImages();
    }

    // set level-specific silhouettes
    //
    CGuiScreenScrapBookContents* scrapBookContents = static_cast<CGuiScreenScrapBookContents*>( m_guiManager->FindWindowByID( GUI_SCREEN_ID_SCRAP_BOOK_CONTENTS ) );
    rAssert( scrapBookContents != NULL );
    for( int i = 0; i < MAX_NUM_SKINS_PER_LEVEL; i++ )
    {
        Scrooby::Sprite* skinImage = dynamic_cast<Scrooby::Sprite*>( m_pMenu->GetMenuItem( i )->GetItem() );
        rAssert( skinImage != NULL );
        skinImage->SetIndex( scrapBookContents->GetCurrentLevel() );
    }

    this->OnMenuSelectionChange( m_pMenu->GetSelection() );

    GetEventManager()->TriggerEvent( EVENT_PLAY_MUZAK );
}


//===========================================================================
// CGuiScreenSkinGallery::InitRunning
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
void CGuiScreenSkinGallery::InitRunning()
{
}


//===========================================================================
// CGuiScreenSkinGallery::InitOutro
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
void CGuiScreenSkinGallery::InitOutro()
{
    if( m_isSkinsLoaded )
    {
        this->Unload2DImages();
    }

    GetEventManager()->TriggerEvent( EVENT_PLAY_FE_MUSIC );
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

void
CGuiScreenSkinGallery::OnUpdate( unsigned int elapsedTime )
{
    switch( m_screenState )
    {
        case SCREEN_STATE_NORMAL:
        {
            // pulse cursor alpha
            //
            Scrooby::Drawable* cursor = m_pMenu->GetCursor();
            if( cursor != NULL )
            {
                const unsigned int PULSE_PERIOD = 1000;

                float alpha = GuiSFX::Pulse( (float)m_elapsedTime,
                                             (float)PULSE_PERIOD,
                                             0.75f,
                                             0.25f,
                                             -rmt::PI_BY2 );

                cursor->SetAlpha( alpha );

                m_elapsedTime += elapsedTime;
                m_elapsedTime %= PULSE_PERIOD;
            }

            break;
        }
        case SCREEN_STATE_GOTO_VIEW:
        {
            m_elapsedTime += elapsedTime;

            rAssert( m_pMenu != NULL );
            int currentSelection = m_pMenu->GetSelection();

            Scrooby::BoundedDrawable* pDrawable = m_pMenu->GetMenuItem( currentSelection )->GetItem();
            rAssert( pDrawable != NULL );

            pDrawable->ResetTransformation();

            if( m_elapsedTime < (unsigned int)SKIN_VIEW_TRANSITION_TIME )
            {
                float percentageDone = m_elapsedTime / SKIN_VIEW_TRANSITION_TIME;

                pDrawable->ScaleAboutCenter( SKIN_BASE_SCALE + percentageDone * SKIN_IMAGE_SCALE );

                GuiSFX::Projectile( pDrawable,
                                    (float)m_elapsedTime,
                                    SKIN_VIEW_TRANSITION_TIME,
                                    m_projectileVelocity,
                                    false,
                                    SKIN_VIEW_PROJECTILE_GRAVITY );

                // fade out rest of the menu items
                //
                this->SetMenuAlpha( 1.0f - rmt::Sqrt( percentageDone ) );

                // fade in skin info layer
                //
                rAssert( m_skinInfo != NULL );
                m_skinInfo->SetAlpha( percentageDone * percentageDone );
            }
            else
            {
                pDrawable->ScaleAboutCenter( SKIN_BASE_SCALE + SKIN_IMAGE_SCALE );

                GuiSFX::Projectile( pDrawable,
                                    SKIN_VIEW_TRANSITION_TIME,
                                    SKIN_VIEW_TRANSITION_TIME,
                                    m_projectileVelocity,
                                    false,
                                    SKIN_VIEW_PROJECTILE_GRAVITY );

                this->SetMenuAlpha( 0.0f );

                rAssert( m_skinInfo != NULL );
                m_skinInfo->SetAlpha( 1.0f );

                m_screenState = SCREEN_STATE_VIEWING;
            }

            break;
        }
        case SCREEN_STATE_VIEWING:
        {

            break;
        }
        case SCREEN_STATE_BACK_VIEW:
        {
            m_elapsedTime += elapsedTime;

            rAssert( m_pMenu != NULL );
            int currentSelection = m_pMenu->GetSelection();

            Scrooby::BoundedDrawable* pDrawable = m_pMenu->GetMenuItem( currentSelection )->GetItem();
            rAssert( pDrawable != NULL );

            pDrawable->ResetTransformation();

            if( m_elapsedTime < (unsigned int)SKIN_VIEW_TRANSITION_TIME )
            {
                float percentageDone = m_elapsedTime / SKIN_VIEW_TRANSITION_TIME;

                pDrawable->ScaleAboutCenter( SKIN_BASE_SCALE + (1.0f - percentageDone) * SKIN_IMAGE_SCALE );

                GuiSFX::Projectile( pDrawable,
                                    (float)m_elapsedTime,
                                    SKIN_VIEW_TRANSITION_TIME,
                                    m_projectileVelocity,
                                    true,
                                    SKIN_VIEW_PROJECTILE_GRAVITY );

                // fade back in rest of the menu items
                //
                this->SetMenuAlpha( percentageDone * percentageDone );

                // fade in skin info layer
                //
                rAssert( m_skinInfo != NULL );
                m_skinInfo->SetAlpha( 1.0f - rmt::Sqrt( percentageDone ) );
            }
            else
            {
                pDrawable->ScaleAboutCenter( SKIN_BASE_SCALE );

                GuiSFX::Projectile( pDrawable,
                                    SKIN_VIEW_TRANSITION_TIME,
                                    SKIN_VIEW_TRANSITION_TIME,
                                    m_projectileVelocity,
                                    true,
                                    SKIN_VIEW_PROJECTILE_GRAVITY );

                // show menu cursor
                //
                rAssert( m_pMenu != NULL );
                m_pMenu->GetCursor()->SetVisible( true );

                // show level bar
                //
                CGuiScreenScrapBookContents* scrapBookContents = static_cast<CGuiScreenScrapBookContents*>( m_guiManager->FindWindowByID( GUI_SCREEN_ID_SCRAP_BOOK_CONTENTS ) );
                rAssert( scrapBookContents != NULL );
                scrapBookContents->SetLevelBarVisible( true );

                this->SetMenuAlpha( 1.0f );

                rAssert( m_skinInfo != NULL );
                m_skinInfo->SetAlpha( 0.0f );

                this->SetButtonVisible( BUTTON_ICON_ACCEPT, true );

                // hide skin info layer
                //
                rAssert( m_skinInfo != NULL );
                m_skinInfo->SetVisible( false );

                m_elapsedTime = 0;
                m_screenState = SCREEN_STATE_NORMAL;
#ifdef RAD_WIN32
                // Show/enable all hidden menu items.
                this->SetVisibilityForAllOtherMenuItems( true );
#endif
            }

            break;
        }
        default:
        {
            rAssertMsg( false, "Invalid screen state!" );

            break;
        }
    }
}

void
CGuiScreenSkinGallery::OnMenuSelectionChange( int selection )
{
    // scale up new selection
    //
    for( int i = 0; i < m_numSelections; i++ )
    {
        Scrooby::BoundedDrawable* drawable = m_pMenu->GetMenuItem( i )->GetItem();
        rAssert( drawable != NULL );
        drawable->ResetTransformation();
        drawable->ScaleAboutCenter( SKIN_BASE_SCALE );

        if( i != selection )
        {
            drawable->ScaleAboutCenter( 0.9f );
        }
    }
}

void
CGuiScreenSkinGallery::OnMenuSelectionMade( int selection )
{
    if( m_numSelections > 0 )
    {
        // hide level bar
        //
        CGuiScreenScrapBookContents* scrapBookContents = static_cast<CGuiScreenScrapBookContents*>( m_guiManager->FindWindowByID( GUI_SCREEN_ID_SCRAP_BOOK_CONTENTS ) );
        rAssert( scrapBookContents != NULL );
        scrapBookContents->SetLevelBarVisible( false );

        // hide menu cursor
        //
        rAssert( m_pMenu != NULL );
        m_pMenu->GetCursor()->SetVisible( false );

        // show skin info layer
        //
        rAssert( m_skinInfo != NULL );
        m_skinInfo->SetVisible( true );

        // update skin name
        //
        HeapMgr()->PushHeap( GMA_LEVEL_FE );

        char stringID[ 16 ];
        rAssert( m_rewardSelections[ m_pMenu->GetSelection() ] != NULL );
        strcpy( stringID, m_rewardSelections[ m_pMenu->GetSelection() ]->GetName() );

        UnicodeString unicodeString;
        unicodeString.ReadUnicode( GetTextBibleString( strupr( stringID ) ) );

        rAssert( m_skinName != NULL );
        m_skinName->SetString( 0, unicodeString );

        HeapMgr()->PopHeap( GMA_LEVEL_FE );

        this->SetButtonVisible( BUTTON_ICON_ACCEPT, false );

        // calculate the initial projectile velocity
        //
        Scrooby::BoundedDrawable* pDrawable = m_pMenu->GetMenuItem( selection )->GetItem();
        rAssert( pDrawable != NULL );

        int startPosX = 0;
        int startPosY = 0;
        pDrawable->GetOriginPosition( startPosX, startPosY );

        m_projectileVelocity.x = (SKIN_VIEW_POS_X - startPosX) / SKIN_VIEW_TRANSITION_TIME;
        m_projectileVelocity.y = (SKIN_VIEW_POS_Y - startPosY - 0.5f * SKIN_VIEW_PROJECTILE_GRAVITY * SKIN_VIEW_TRANSITION_TIME * SKIN_VIEW_TRANSITION_TIME) / SKIN_VIEW_TRANSITION_TIME;

        m_screenState = SCREEN_STATE_GOTO_VIEW;
        m_elapsedTime = 0;
    }
}

void
CGuiScreenSkinGallery::SetMenuAlpha( float alpha )
{
    rAssert( m_pMenu != NULL );
    int currentSelection = m_pMenu->GetSelection();

    for( int i = 0; i < MAX_NUM_SKINS_PER_LEVEL; i++ )
    {
        if( i != currentSelection )
        {
            Scrooby::Drawable* skinImage = m_pMenu->GetMenuItem( i )->GetItem();
            rAssert( skinImage != NULL );
            skinImage->SetAlpha( alpha );
        }
    }
}

void
CGuiScreenSkinGallery::Load2DImages()
{
    // load 2D images for current level
    //
    CGuiScreenScrapBookContents* pScreen = static_cast<CGuiScreenScrapBookContents*>( m_guiManager->FindWindowByID( GUI_SCREEN_ID_SCRAP_BOOK_CONTENTS ) );
    rAssert( pScreen != NULL );

    m_numSelections = 0;

    for( Reward* pReward = GetRewardsManager()->FindFirstMerchandise( pScreen->GetCurrentLevel(), Merchandise::SELLER_INTERIOR );
         pReward != NULL;
         pReward = GetRewardsManager()->FindNextMerchandise( pScreen->GetCurrentLevel(), Merchandise::SELLER_INTERIOR ) )
    {
        rAssert( m_numSelections < MAX_NUM_SKINS_PER_LEVEL );

        // store reference to reward
        //
        m_rewardSelections[ m_numSelections ] = pReward;

        char filename[ 64 ];
        sprintf( filename, "%s%s.p3d", SKIN_GALLERY_IMAGES_DIR, pReward->GetName() );
        rAssert( strlen( filename) < sizeof( filename ) );

        GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D,
                                         filename,
                                         GMA_LEVEL_FE,
                                         SKIN_GALLERY_INVENTORY_SECTION,
                                         SKIN_GALLERY_INVENTORY_SECTION );

        m_numSelections++;
    }

    rWarningMsg( m_numSelections > 0, "No model selections available!" );

    if( m_numSelections > 0 )
    {
        GetLoadingManager()->AddCallback( this );

        m_numTransitionsPending++;
    }

    this->SetButtonVisible( BUTTON_ICON_ACCEPT, false ); // hide by default

    for( int i = 0; i < MAX_NUM_SKINS_PER_LEVEL; i++ )
    {
        if( i < m_numSelections )
        {
            bool isUnlocked = m_rewardSelections[ i ]->RewardStatus();
            if( GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_UNLOCK_SKINS ) )
            {
                isUnlocked = true;
            }

            m_pMenu->SetMenuItemEnabled( i, isUnlocked );

            if( isUnlocked )
            {
                this->SetButtonVisible( BUTTON_ICON_ACCEPT, true );
            }
        }
        else
        {
            m_pMenu->SetMenuItemEnabled( i, false );
        }
    }
}

void
CGuiScreenSkinGallery::Unload2DImages()
{
    p3d::pddi->DrawSync();

    // clear all drawables
    //
    for( int i = 0; i < MAX_NUM_SKINS_PER_LEVEL; i++ )
    {
        Scrooby::Sprite* skinImage = dynamic_cast<Scrooby::Sprite*>( m_pMenu->GetMenuItem( i )->GetItem() );
        rAssert( skinImage != NULL );
        skinImage->SetRawSprite( NULL, true );

        m_pMenu->SetMenuItemEnabled( i, false );
    }

    // unload 2D images
    //
    p3d::inventory->RemoveSectionElements( SKIN_GALLERY_INVENTORY_SECTION );

    m_isSkinsLoaded = false;
}

#ifdef RAD_WIN32
void CGuiScreenSkinGallery::SetVisibilityForAllOtherMenuItems( bool bVisible )
{
    for( int i = 0; i < MAX_NUM_SKINS_PER_LEVEL; i++ )
    {
        if( i != m_selectedSkin )
            m_pMenu->GetMenuItem(i)->GetItem()->SetVisible( bVisible );
    }
}
#endif