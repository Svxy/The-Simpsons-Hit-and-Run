//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenVehicleGallery
//
// Description: Implementation of the CGuiScreenVehicleGallery class.
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
#include <presentation/gui/frontend/guiscreenvehiclegallery.h>
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

const char* VEHICLE_GALLERY_INVENTORY_SECTION = "FE_VehicleGallery";
const char* VEHICLE_GALLERY_IMAGES_DIR = "art\\frontend\\dynaload\\images\\cars2d\\";

const float VEHICLE_VIEW_TRANSITION_TIME = 250.0f; // in msec
const float VEHICLE_VIEW_PROJECTILE_GRAVITY = 0.005f; // in m/ms/ms
const int VEHICLE_VIEW_POS_X = 250;
const int VEHICLE_VIEW_POS_Y = 140;
#ifdef RAD_WIN32
const float VEHICLE_BASE_SCALE = 0.66f;
const float VEHICLE_IMAGE_SCALE = 1.33f - 0.66f;
#else
const float VEHICLE_BASE_SCALE = 1.0f;
const float VEHICLE_IMAGE_SCALE = 2.0f - 1.0f;
#endif

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenVehicleGallery::CGuiScreenVehicleGallery
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
CGuiScreenVehicleGallery::CGuiScreenVehicleGallery
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_VEHICLE_GALLERY ),
    m_pMenu( NULL ),
    m_numSelections( 0 ),
    m_isVehiclesLoaded( false ),
    m_screenState( SCREEN_STATE_NORMAL ),
    m_elapsedTime( 0 ),
    m_projectileVelocity( 0.0f, 0.0f, 0.0f ),
    m_vehicleInfo( NULL ),
#ifdef RAD_WIN32
    m_selectedVehicle(0),
#endif
    m_vehicleName( NULL )
{
    memset( m_rewardSelections, 0, sizeof( m_rewardSelections ) );

    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "VehicleGallery" );
    rAssert( pPage != NULL );

    // create a 2D sprite menu
    //
    m_pMenu = new CGuiMenu2D( this, MAX_NUM_VEHICLES_PER_LEVEL, 3, GUI_SPRITE_MENU, MENU_SFX_NONE );
    rAssert( m_pMenu != NULL );
    m_pMenu->SetGreyOutEnabled( false );

    Scrooby::Group* pGroup = pPage->GetGroup( "Menu" );
    rAssert( pGroup != NULL );

    // add sprites to menu
    //
    for( int i = 0; i < MAX_NUM_VEHICLES_PER_LEVEL; i++ )
    {
        char name[ 16 ];
        sprintf( name, "Vehicle%d", i );

        m_pMenu->AddMenuItem( pGroup->GetSprite( name ) );
    }

    // add menu cursor
    //
    Scrooby::Sprite* pCursor = pGroup->GetSprite( "VehicleCursor" );
    if( pCursor != NULL )
    {
        // scale up cursor a bit for vehicles
        //
        pCursor->ScaleAboutCenter( 2.0f );

        m_pMenu->SetCursor( pCursor );
    }

    // get vehicle info layer and name
    //
    m_vehicleInfo = pPage->GetLayer( "ViewVehicle" );
    m_vehicleName = pPage->GetText( "VehicleName" );

    // hide vehicle info layer by default
    //
    rAssert( m_vehicleInfo != NULL );
    m_vehicleInfo->SetVisible( false );
    m_vehicleInfo->SetAlpha( 0.0f );

    if( this->IsWideScreenDisplay() )
    {
        m_vehicleInfo->ResetTransformation();
        this->ApplyWideScreenCorrectionScale( m_vehicleInfo );
    }

    // wrap vehicle name
    //
//    rAssert( m_vehicleName != NULL );
//    m_vehicleName->SetTextMode( Scrooby::TEXT_WRAP );

    // add outline to vehicle name
    //
    m_vehicleName->SetDisplayOutline( true );

    // create inventory section for vehicle galllery resources
    //
    p3d::inventory->AddSection( VEHICLE_GALLERY_INVENTORY_SECTION );
}


//===========================================================================
// CGuiScreenVehicleGallery::~CGuiScreenVehicleGallery
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
CGuiScreenVehicleGallery::~CGuiScreenVehicleGallery()
{
    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }

    // destroy vehicle gallery inventory section
    //
    p3d::inventory->DeleteSection( VEHICLE_GALLERY_INVENTORY_SECTION );
}


//===========================================================================
// CGuiScreenVehicleGallery::HandleMessage
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
void CGuiScreenVehicleGallery::HandleMessage
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
                m_selectedVehicle = static_cast<int>( param1 );
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
// CGuiScreenVehicleGallery::OnProcessRequestsComplete
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
CGuiScreenVehicleGallery::OnProcessRequestsComplete( void* pUserData )
{
    m_numTransitionsPending--;

    p3d::pddi->DrawSync();

    // push and select inventory section for searching
    //
    p3d::inventory->PushSection();
    p3d::inventory->SelectSection( VEHICLE_GALLERY_INVENTORY_SECTION );
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
                Scrooby::Sprite* vehicleImage = dynamic_cast<Scrooby::Sprite*>( m_pMenu->GetMenuItem( i )->GetItem() );
                rAssert( vehicleImage != NULL );
                vehicleImage->SetRawSprite( pSprite, true );
            }
            else
            {
                rAssertMsg( false, "Vehicle image not found!" );
            }
        }
    }

    // pop inventory section and restore states
    //
    p3d::inventory->SetCurrentSectionOnly( currentSectionOnly );
    p3d::inventory->PopSection();

    m_isVehiclesLoaded = true;
}

//===========================================================================
// CGuiScreenVehicleGallery::InitIntro
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
void CGuiScreenVehicleGallery::InitIntro()
{
    if( !m_isVehiclesLoaded )
    {
        this->Load2DImages();
    }

    this->OnMenuSelectionChange( m_pMenu->GetSelection() );

    GetEventManager()->TriggerEvent( EVENT_PLAY_MUZAK );
}


//===========================================================================
// CGuiScreenVehicleGallery::InitRunning
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
void CGuiScreenVehicleGallery::InitRunning()
{
}


//===========================================================================
// CGuiScreenVehicleGallery::InitOutro
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
void CGuiScreenVehicleGallery::InitOutro()
{
    if( m_isVehiclesLoaded )
    {
        this->Unload2DImages();
    }

    GetEventManager()->TriggerEvent( EVENT_PLAY_FE_MUSIC );
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

void
CGuiScreenVehicleGallery::OnUpdate( unsigned int elapsedTime )
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

            if( m_elapsedTime < (unsigned int)VEHICLE_VIEW_TRANSITION_TIME )
            {
                float percentageDone = m_elapsedTime / VEHICLE_VIEW_TRANSITION_TIME;

                pDrawable->ScaleAboutCenter( VEHICLE_BASE_SCALE + percentageDone * VEHICLE_IMAGE_SCALE );

                GuiSFX::Projectile( pDrawable,
                                    (float)m_elapsedTime,
                                    VEHICLE_VIEW_TRANSITION_TIME,
                                    m_projectileVelocity,
                                    false,
                                    VEHICLE_VIEW_PROJECTILE_GRAVITY );

                // fade out rest of the menu items
                //
                this->SetMenuAlpha( 1.0f - rmt::Sqrt( percentageDone ) );

                // fade in vehicle info layer
                //
                rAssert( m_vehicleInfo != NULL );
                m_vehicleInfo->SetAlpha( percentageDone * percentageDone );
            }
            else
            {
                pDrawable->ScaleAboutCenter( VEHICLE_BASE_SCALE + VEHICLE_IMAGE_SCALE );

                GuiSFX::Projectile( pDrawable,
                                    VEHICLE_VIEW_TRANSITION_TIME,
                                    VEHICLE_VIEW_TRANSITION_TIME,
                                    m_projectileVelocity,
                                    false,
                                    VEHICLE_VIEW_PROJECTILE_GRAVITY );

                this->SetMenuAlpha( 0.0f );

                rAssert( m_vehicleInfo != NULL );
                m_vehicleInfo->SetAlpha( 1.0f );

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

            if( m_elapsedTime < (unsigned int)VEHICLE_VIEW_TRANSITION_TIME )
            {
                float percentageDone = m_elapsedTime / VEHICLE_VIEW_TRANSITION_TIME;

                pDrawable->ScaleAboutCenter( VEHICLE_BASE_SCALE + (1.0f - percentageDone) * VEHICLE_IMAGE_SCALE );

                GuiSFX::Projectile( pDrawable,
                                    (float)m_elapsedTime,
                                    VEHICLE_VIEW_TRANSITION_TIME,
                                    m_projectileVelocity,
                                    true,
                                    VEHICLE_VIEW_PROJECTILE_GRAVITY );

                // fade back in rest of the menu items
                //
                this->SetMenuAlpha( percentageDone * percentageDone );

                // fade in vehicle info layer
                //
                rAssert( m_vehicleInfo != NULL );
                m_vehicleInfo->SetAlpha( 1.0f - rmt::Sqrt( percentageDone ) );
            }
            else
            {
                pDrawable->ScaleAboutCenter( VEHICLE_BASE_SCALE );

                GuiSFX::Projectile( pDrawable,
                                    VEHICLE_VIEW_TRANSITION_TIME,
                                    VEHICLE_VIEW_TRANSITION_TIME,
                                    m_projectileVelocity,
                                    true,
                                    VEHICLE_VIEW_PROJECTILE_GRAVITY );

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

                rAssert( m_vehicleInfo != NULL );
                m_vehicleInfo->SetAlpha( 0.0f );

                this->SetButtonVisible( BUTTON_ICON_ACCEPT, true );

                // hide vehicle info layer
                //
                rAssert( m_vehicleInfo != NULL );
                m_vehicleInfo->SetVisible( false );

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
CGuiScreenVehicleGallery::OnMenuSelectionChange( int selection )
{
    // scale up new selection
    //
    for( int i = 0; i < m_numSelections; i++ )
    {
        Scrooby::BoundedDrawable* drawable = m_pMenu->GetMenuItem( i )->GetItem();
        rAssert( drawable != NULL );
        drawable->ResetTransformation();
        drawable->ScaleAboutCenter( VEHICLE_BASE_SCALE );

        if( i != selection )
        {
            drawable->ScaleAboutCenter( 0.9f );
        }
    }
}

void
CGuiScreenVehicleGallery::OnMenuSelectionMade( int selection )
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

        // show vehicle info layer
        //
        rAssert( m_vehicleInfo != NULL );
        m_vehicleInfo->SetVisible( true );

        // update vehicle name
        //
        HeapMgr()->PushHeap( GMA_LEVEL_FE );

        char stringID[ 16 ];
        rAssert( m_rewardSelections[ m_pMenu->GetSelection() ] != NULL );
        strcpy( stringID, m_rewardSelections[ m_pMenu->GetSelection() ]->GetName() );

        UnicodeString unicodeString;
        unicodeString.ReadUnicode( GetTextBibleString( strupr( stringID ) ) );

        rAssert( m_vehicleName != NULL );
        m_vehicleName->SetString( 0, unicodeString );

        HeapMgr()->PopHeap( GMA_LEVEL_FE );

        this->SetButtonVisible( BUTTON_ICON_ACCEPT, false );

        // calculate the initial projectile velocity
        //
        Scrooby::BoundedDrawable* pDrawable = m_pMenu->GetMenuItem( selection )->GetItem();
        rAssert( pDrawable != NULL );

        int startPosX = 0;
        int startPosY = 0;
        pDrawable->GetOriginPosition( startPosX, startPosY );

        m_projectileVelocity.x = (VEHICLE_VIEW_POS_X - startPosX) / VEHICLE_VIEW_TRANSITION_TIME;
        m_projectileVelocity.y = (VEHICLE_VIEW_POS_Y - startPosY - 0.5f * VEHICLE_VIEW_PROJECTILE_GRAVITY * VEHICLE_VIEW_TRANSITION_TIME * VEHICLE_VIEW_TRANSITION_TIME) / VEHICLE_VIEW_TRANSITION_TIME;

        m_screenState = SCREEN_STATE_GOTO_VIEW;
        m_elapsedTime = 0;
    }
}

void
CGuiScreenVehicleGallery::SetMenuAlpha( float alpha )
{
    rAssert( m_pMenu != NULL );
    int currentSelection = m_pMenu->GetSelection();

    for( int i = 0; i < MAX_NUM_VEHICLES_PER_LEVEL; i++ )
    {
        if( i != currentSelection )
        {
            Scrooby::Drawable* vehicleImage = m_pMenu->GetMenuItem( i )->GetItem();
            rAssert( vehicleImage != NULL );
            vehicleImage->SetAlpha( alpha );
        }
    }
}

void
CGuiScreenVehicleGallery::Load2DImages()
{
    // load 2D images for current level
    //
    CGuiScreenScrapBookContents* pScreen = static_cast<CGuiScreenScrapBookContents*>( m_guiManager->FindWindowByID( GUI_SCREEN_ID_SCRAP_BOOK_CONTENTS ) );
    rAssert( pScreen != NULL );

    Reward* pReward = NULL;
    char filename[ 64 ];
    m_numSelections = 0;

    // load earnable reward and default vehicles
    //
    for( int i = Reward::eBlank + 1; i < Reward::NUM_QUESTS; i++ )
    {
        pReward = GetRewardsManager()->GetReward( pScreen->GetCurrentLevel(), static_cast<Reward::eQuestType>( i ) );
        if( pReward != NULL )
        {
            if( pReward->GetRewardType() == Reward::ALT_PLAYERCAR )
            {
                rAssert( m_numSelections < MAX_NUM_VEHICLES_PER_LEVEL );

                // store reference to reward
                //
                m_rewardSelections[ m_numSelections ] = pReward;

                sprintf( filename, "%s%s.p3d", VEHICLE_GALLERY_IMAGES_DIR, pReward->GetName() );
                rAssert( strlen( filename) < sizeof( filename ) );

                GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D,
                                                filename,
                                                GMA_LEVEL_FE,
                                                VEHICLE_GALLERY_INVENTORY_SECTION,
                                                VEHICLE_GALLERY_INVENTORY_SECTION );

                m_numSelections++;
            }
        }
    }

    // load merchandise vehicles (from GIL)
    //
    for( pReward = GetRewardsManager()->FindFirstMerchandise( pScreen->GetCurrentLevel(), Merchandise::SELLER_GIL );
         pReward != NULL;
         pReward = GetRewardsManager()->FindNextMerchandise( pScreen->GetCurrentLevel(), Merchandise::SELLER_GIL ) )
    {
        rAssert( m_numSelections < MAX_NUM_VEHICLES_PER_LEVEL );

        // store reference to reward
        //
        m_rewardSelections[ m_numSelections ] = pReward;

        sprintf( filename, "%s%s.p3d", VEHICLE_GALLERY_IMAGES_DIR, pReward->GetName() );
        rAssert( strlen( filename) < sizeof( filename ) );

        GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D,
                                         filename,
                                         GMA_LEVEL_FE,
                                         VEHICLE_GALLERY_INVENTORY_SECTION,
                                         VEHICLE_GALLERY_INVENTORY_SECTION );

        m_numSelections++;
    }

    // load merchandise vehicles (from SIMPSON CHARACTER)
    //
    for( pReward = GetRewardsManager()->FindFirstMerchandise( pScreen->GetCurrentLevel(), Merchandise::SELLER_SIMPSON );
         pReward != NULL;
         pReward = GetRewardsManager()->FindNextMerchandise( pScreen->GetCurrentLevel(), Merchandise::SELLER_SIMPSON ) )
    {
        rAssert( m_numSelections < MAX_NUM_VEHICLES_PER_LEVEL );

        // store reference to reward
        //
        m_rewardSelections[ m_numSelections ] = pReward;

        sprintf( filename, "%s%s.p3d", VEHICLE_GALLERY_IMAGES_DIR, pReward->GetName() );
        rAssert( strlen( filename) < sizeof( filename ) );

        GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D,
                                         filename,
                                         GMA_LEVEL_FE,
                                         VEHICLE_GALLERY_INVENTORY_SECTION,
                                         VEHICLE_GALLERY_INVENTORY_SECTION );

        m_numSelections++;
    }

    rWarningMsg( m_numSelections > 0, "No model selections available!" );

    if( m_numSelections > 0 )
    {
        GetLoadingManager()->AddCallback( this );

        m_numTransitionsPending++;
    }

    this->SetButtonVisible( BUTTON_ICON_ACCEPT, false ); // hide by default

    for( int i = 0; i < MAX_NUM_VEHICLES_PER_LEVEL; i++ )
    {
        if( i < m_numSelections )
        {
            bool isUnlocked = m_rewardSelections[ i ]->RewardStatus();
            if( GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_UNLOCK_VEHICLES ) )
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
CGuiScreenVehicleGallery::Unload2DImages()
{
    p3d::pddi->DrawSync();

    // clear all drawables
    //
    for( int i = 0; i < MAX_NUM_VEHICLES_PER_LEVEL; i++ )
    {
        Scrooby::Sprite* vehicleImage = dynamic_cast<Scrooby::Sprite*>( m_pMenu->GetMenuItem( i )->GetItem() );
        rAssert( vehicleImage != NULL );
        vehicleImage->SetRawSprite( NULL, true );

        m_pMenu->SetMenuItemEnabled( i, false );
    }

    // unload 2D images
    //
    p3d::inventory->RemoveSectionElements( VEHICLE_GALLERY_INVENTORY_SECTION );

    m_isVehiclesLoaded = false;
}
#ifdef RAD_WIN32
void CGuiScreenVehicleGallery::SetVisibilityForAllOtherMenuItems( bool bVisible )
{
    for( int i = 0; i < MAX_NUM_VEHICLES_PER_LEVEL; i++ )
    {
        if( i != m_selectedVehicle )
            m_pMenu->GetMenuItem(i)->GetItem()->SetVisible( bVisible );
    }
}
#endif
