//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMissionGallery
//
// Description: Implementation of the CGuiScreenMissionGallery class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/03/19      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/frontend/guiscreenmissiongallery.h>
#include <presentation/gui/frontend/guiscreenscrapbookcontents.h>
#include <presentation/gui/utility/specialfx.h>
#include <presentation/gui/guimanager.h>
#include <presentation/gui/guimenu.h>
#include <presentation/gui/guiscreenprompt.h>

#include <cheats/cheatinputsystem.h>
#include <events/eventmanager.h>
#include <input/inputmanager.h>
#include <memory/srrmemory.h>
#include <mission/charactersheet/charactersheetmanager.h>

#include <p3d/inventory.hpp>
#include <raddebug.hpp> // Foundation

#include <screen.h>
#include <page.h>
#include <layer.h>
#include <group.h>
#include <sprite.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

const char* MISSION_GALLERY_IMAGES_DIR = "art\\frontend\\dynaload\\images\\scrapbook\\";
const char* MISSION_GALLERY_INVENTORY_SECTION = "FE_MissionGallery";
const float MISSION_IMAGE_SCALE = 0.4f;
const float MISSION_VIEW_TRANSITION_TIME = 250.0f; // in msec
const float MISSION_VIEW_PROJECTILE_GRAVITY = 0.005f; // in m/ms/ms
const int MISSION_VIEW_POS_X = 180;
const int MISSION_VIEW_POS_Y = 120;

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenMissionGallery::CGuiScreenMissionGallery
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
CGuiScreenMissionGallery::CGuiScreenMissionGallery
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_MISSION_GALLERY ),
    m_pMenu( NULL ),
    m_menuGroup( NULL ),
    m_isMissionImagesLoaded( false ),
    m_screenState( SCREEN_STATE_NORMAL ),
    m_elapsedTime( 0 ),
    m_projectileVelocity( 0.0f, 0.0f, 0.0f ),
    m_missionInfo( NULL ),
    m_missionNum( NULL ),
    m_missionTitle( NULL ),
    m_selectedMission( -1 )
{
    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "MissionGallery" );
    rAssert( pPage != NULL );

    // create a 2D sprite menu
    //
    m_pMenu = new CGuiMenu2D( this, NUM_MISSIONS_PER_LEVEL, 4, GUI_SPRITE_MENU, MENU_SFX_NONE );
    rAssert( m_pMenu != NULL );
    m_pMenu->SetGreyOutEnabled( false );

    m_menuGroup = pPage->GetGroup( "Menu" );
    rAssert( m_menuGroup != NULL );

    // add sprites to menu
    //
    for( int i = 0; i < NUM_MISSIONS_PER_LEVEL; i++ )
    {
        char name[ 16 ];
        sprintf( name, "Mission%d", i );

        m_pMenu->AddMenuItem( m_menuGroup->GetSprite( name ) );
    }

    // add menu cursor
    //
    m_pMenu->SetCursor( m_menuGroup->GetSprite( "MissionFrame" ) );

    // TC: [TEMP] due to placeholder image
    //
    m_menuGroup->GetSprite( "MissionFrame" )->RotateAboutCenter( 90.0f );

    // get view mission layer and text drawables
    //
    m_missionInfo = pPage->GetLayer( "ViewMission" );
    m_missionNum = pPage->GetText( "MissionNum" );
    m_missionTitle = pPage->GetText( "MissionTitle" );

    // add view mission layer to list of foreground layers
    //
    if( m_numForegroundLayers < MAX_FOREGROUND_LAYERS )
    {
        m_foregroundLayers[ m_numForegroundLayers ] = m_missionInfo;
        m_numForegroundLayers++;
    }

    if( this->IsWideScreenDisplay() )
    {
        m_missionInfo->ResetTransformation();
        this->ApplyWideScreenCorrectionScale( m_missionInfo );
    }

    // wrap mission title
    //
    rAssert( m_missionTitle != NULL );
    m_missionTitle->SetTextMode( Scrooby::TEXT_WRAP );

    // add outline to mission number and title
    //
    Scrooby::Text* missionNum = pPage->GetText( "MissionNum" );
    if( missionNum != NULL )
    {
        missionNum->SetDisplayOutline( true );
    }
    m_missionTitle->SetDisplayOutline( true );

    // create inventory section for mission galllery resources
    //
    p3d::inventory->AddSection( MISSION_GALLERY_INVENTORY_SECTION );
}


//===========================================================================
// CGuiScreenMissionGallery::~CGuiScreenMissionGallery
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
CGuiScreenMissionGallery::~CGuiScreenMissionGallery()
{
    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }

    // destroy mission gallery inventory section
    //
    p3d::inventory->DeleteSection( MISSION_GALLERY_INVENTORY_SECTION );
}


//===========================================================================
// CGuiScreenMissionGallery::HandleMessage
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
void CGuiScreenMissionGallery::HandleMessage
(
	eGuiMessage message,
	unsigned int param1,
	unsigned int param2
)
{
    if( message == GUI_MSG_MENU_PROMPT_RESPONSE )
    {
        rAssert( param1 == PROMPT_CONFIRM_START_MISSION );

        if( param2 == CGuiMenuPrompt::RESPONSE_YES )
        {
            this->OnStartMission();
        }
        else
        {
            rAssert( param2 == CGuiMenuPrompt::RESPONSE_NO );

//            m_selectedMission = -1;
            GetInputManager()->UnregisterControllerID( 0 );
            this->ReloadScreen();
        }
    }

    if( m_screenState == SCREEN_STATE_GOTO_VIEW ||
        m_screenState == SCREEN_STATE_BACK_VIEW )
    {
        if( message == GUI_MSG_UPDATE )
        {
            this->OnUpdate( param1 );
        }

        return;
    }

    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        if( m_screenState == SCREEN_STATE_VIEWING )
        {
            if( message == GUI_MSG_CONTROLLER_SELECT )
            {
                if( this->IsButtonVisible( BUTTON_ICON_ACCEPT ) )
                {
                    GetInputManager()->RegisterControllerID( 0, param1 );

                    m_guiManager->DisplayPrompt( PROMPT_CONFIRM_START_MISSION, this );

                    GetEventManager()->TriggerEvent( EVENT_FE_MENU_SELECT ); // sound effect
                }
            }
            else if( message == GUI_MSG_CONTROLLER_BACK )
            {
                m_screenState = SCREEN_STATE_BACK_VIEW;
                m_elapsedTime = 0;

                m_selectedMission = -1;

                GetEventManager()->TriggerEvent( EVENT_FE_MENU_BACK ); // sound effect
            }

            if( this->IsControllerMessage( message ) )
            {
                return;
            }
        }

        switch( message )
        {
            case GUI_MSG_UPDATE:
            {
                this->OnUpdate( param1 );

                break;
            }
/*
            case GUI_MSG_MENU_SELECTION_CHANGED:
            {
                this->SetButtonVisible( BUTTON_ICON_ACCEPT, static_cast<int>( param1 ) != MISSION_8 );

                break;
            }
*/
            case GUI_MSG_MENU_SELECTION_MADE:
            {
                m_selectedMission = static_cast<int>( param1 );

//                m_guiManager->DisplayPrompt( PROMPT_CONFIRM_START_MISSION, this );
                this->OnMenuSelectionMade( param1 );
#ifdef RAD_WIN32
                // Hide/disable all other menu items.
                this->SetVisibilityForAllOtherMenuItems( false );
#endif
                break;
            }
/*
            case GUI_MSG_CONTROLLER_SELECT:
            {
                if( this->IsButtonVisible( BUTTON_ICON_ACCEPT ) )
                {
                    GetInputManager()->RegisterControllerID( 0, param1 );
                }
                else
                {
                    return;
                }

                break;
            }
*/
            default:
            {
                break;
            }
        }

        // relay message to menu
        //
        if( m_pMenu != NULL && m_screenState == SCREEN_STATE_NORMAL )
        {
            m_pMenu->HandleMessage( message, param1, param2 );
        }
    }
    else if( m_state == GUI_WINDOW_STATE_OUTRO )
    {
        if( message == GUI_MSG_UPDATE && m_numTransitionsPending <= 0 )
        {
            if( m_isMissionImagesLoaded && m_selectedMission == -1 )
            {
                this->UnloadMissionImages();
            }
        }
    }

	// Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );
}


//===========================================================================
// CGuiScreenMissionGallery::OnProcessRequestsComplete
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
CGuiScreenMissionGallery::OnProcessRequestsComplete( void* pUserData )
{
    m_numTransitionsPending--;

    // show mission images
    //
    m_menuGroup->SetAlpha( 1.0f );

    p3d::pddi->DrawSync();

    // push and select inventory section for searching
    //
    p3d::inventory->PushSection();
    p3d::inventory->SelectSection( MISSION_GALLERY_INVENTORY_SECTION );
    bool currentSectionOnly = p3d::inventory->GetCurrentSectionOnly();
    p3d::inventory->SetCurrentSectionOnly( true );

    // update all mission images
    //
    CGuiScreenScrapBookContents* scrapBookContents = static_cast<CGuiScreenScrapBookContents*>( m_guiManager->FindWindowByID( GUI_SCREEN_ID_SCRAP_BOOK_CONTENTS ) );
    rAssert( scrapBookContents != NULL );

    rAssert( m_pMenu != NULL );
    for( int i = 0; i < NUM_MISSIONS_PER_LEVEL; i++ )
    {
        if( (m_pMenu->GetMenuItem( i )->m_attributes & SELECTION_ENABLED) > 0 )
        {
            char name[ 16 ];
            sprintf( name, "mis%02d_%02d.png", scrapBookContents->GetCurrentLevel() + 1, i + 1 );
            tSprite* pSprite = p3d::find<tSprite>( name );
            if( pSprite != NULL )
            {
                Scrooby::Sprite* missionImage = dynamic_cast<Scrooby::Sprite*>( m_pMenu->GetMenuItem( i )->GetItem() );
                rAssert( missionImage != NULL );
                missionImage->SetRawSprite( pSprite, true );

                missionImage->ResetTransformation();
                missionImage->ScaleAboutCenter( MISSION_IMAGE_SCALE );
            }
            else
            {
                rAssertMsg( false, "Mission bitmap not found!" );
            }
        }
    }

    // pop inventory section and restore states
    //
    p3d::inventory->SetCurrentSectionOnly( currentSectionOnly );
    p3d::inventory->PopSection();

    m_isMissionImagesLoaded = true;
}

//===========================================================================
// CGuiScreenMissionGallery::InitIntro
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
void CGuiScreenMissionGallery::InitIntro()
{
    if( !m_isMissionImagesLoaded )
    {
        int numImagesToLoad = this->LoadMissionImages();
    }
/*
    // reset menu to first selection
    //
    if( numImagesToLoad > 0 )
    {
        rAssert( m_pMenu != NULL );
        m_pMenu->Reset();
    }
*/
    m_elapsedTime = 0;
}


//===========================================================================
// CGuiScreenMissionGallery::InitRunning
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
void CGuiScreenMissionGallery::InitRunning()
{
    if( m_screenState != SCREEN_STATE_VIEWING )
    {
        // hide mission info layer by default
        //
        rAssert( m_missionInfo != NULL );
        m_missionInfo->SetVisible( false );
        m_missionInfo->SetAlpha( 0.0f );
    }
}


//===========================================================================
// CGuiScreenMissionGallery::InitOutro
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
void CGuiScreenMissionGallery::InitOutro()
{
//    this->UnloadMissionImages();
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

void
CGuiScreenMissionGallery::OnUpdate( unsigned int elapsedTime )
{
    if( m_screenState == SCREEN_STATE_NORMAL )
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
    }
    else if( m_screenState == SCREEN_STATE_GOTO_VIEW )
    {
        // transition mission image from normal menu position
        // to viewing position
        //
        m_elapsedTime += elapsedTime;

        rAssert( m_pMenu != NULL );
        int currentSelection = m_pMenu->GetSelection();

        Scrooby::BoundedDrawable* pDrawable = m_pMenu->GetMenuItem( currentSelection )->GetItem();
        rAssert( pDrawable != NULL );

        pDrawable->ResetTransformation();

        if( m_elapsedTime < (unsigned int)MISSION_VIEW_TRANSITION_TIME )
        {
            float percentageDone = m_elapsedTime / MISSION_VIEW_TRANSITION_TIME;

            pDrawable->ScaleAboutCenter( MISSION_IMAGE_SCALE + percentageDone * (1.0f - MISSION_IMAGE_SCALE) );

            GuiSFX::Projectile( pDrawable,
                                (float)m_elapsedTime,
                                MISSION_VIEW_TRANSITION_TIME,
                                m_projectileVelocity,
                                false,
                                MISSION_VIEW_PROJECTILE_GRAVITY );

            // fade out rest of the menu items
            //
            this->SetMenuAlpha( 1.0f - rmt::Sqrt( percentageDone ) );

            // fade in mission info layer
            //
            rAssert( m_missionInfo != NULL );
            m_missionInfo->SetAlpha( percentageDone * percentageDone );
        }
        else
        {
            GuiSFX::Projectile( pDrawable,
                                MISSION_VIEW_TRANSITION_TIME,
                                MISSION_VIEW_TRANSITION_TIME,
                                m_projectileVelocity,
                                false,
                                MISSION_VIEW_PROJECTILE_GRAVITY );

            this->SetMenuAlpha( 0.0f );

            rAssert( m_missionInfo != NULL );
            m_missionInfo->SetAlpha( 1.0f );

            m_screenState = SCREEN_STATE_VIEWING;
        }
    }
    else if( m_screenState == SCREEN_STATE_BACK_VIEW )
    {
        // transition mission image from viewing position back to
        // normal menu position
        //
        m_elapsedTime += elapsedTime;

        rAssert( m_pMenu != NULL );
        int currentSelection = m_pMenu->GetSelection();

        Scrooby::BoundedDrawable* pDrawable = m_pMenu->GetMenuItem( currentSelection )->GetItem();
        rAssert( pDrawable != NULL );

        pDrawable->ResetTransformation();

        if( m_elapsedTime < (unsigned int)MISSION_VIEW_TRANSITION_TIME )
        {
            float percentageDone = m_elapsedTime / MISSION_VIEW_TRANSITION_TIME;

            pDrawable->ScaleAboutCenter( MISSION_IMAGE_SCALE + (1.0f - percentageDone) * (1.0f - MISSION_IMAGE_SCALE) );

            GuiSFX::Projectile( pDrawable,
                                (float)m_elapsedTime,
                                MISSION_VIEW_TRANSITION_TIME,
                                m_projectileVelocity,
                                true,
                                MISSION_VIEW_PROJECTILE_GRAVITY );

            // fade back in rest of the menu items
            //
            this->SetMenuAlpha( percentageDone * percentageDone );

            // fade in mission info layer
            //
            rAssert( m_missionInfo != NULL );
            m_missionInfo->SetAlpha( 1.0f - rmt::Sqrt( percentageDone ) );
        }
        else
        {
            pDrawable->ScaleAboutCenter( MISSION_IMAGE_SCALE );

            GuiSFX::Projectile( pDrawable,
                                MISSION_VIEW_TRANSITION_TIME,
                                MISSION_VIEW_TRANSITION_TIME,
                                m_projectileVelocity,
                                true,
                                MISSION_VIEW_PROJECTILE_GRAVITY );

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

            rAssert( m_missionInfo != NULL );
            m_missionInfo->SetAlpha( 0.0f );

            this->SetButtonVisible( BUTTON_ICON_ACCEPT, true );

            // hide mission info layer
            //
            rAssert( m_missionInfo != NULL );
            m_missionInfo->SetVisible( false );

            m_elapsedTime = 0;
            m_screenState = SCREEN_STATE_NORMAL;

#ifdef RAD_WIN32
            // Show/enable all hidden menu items.
            this->SetVisibilityForAllOtherMenuItems( true );
#endif
        }
    }
}

void
CGuiScreenMissionGallery::SetMenuAlpha( float alpha )
{
    rAssert( m_pMenu != NULL );
    int currentSelection = m_pMenu->GetSelection();

    for( int i = 0; i < NUM_MISSIONS_PER_LEVEL; i++ )
    {
        if( i != currentSelection )
        {
            Scrooby::Drawable* missionImage = m_pMenu->GetMenuItem( i )->GetItem();
            rAssert( missionImage != NULL );
            missionImage->SetAlpha( alpha );
        }
    }
}

void
CGuiScreenMissionGallery::OnMenuSelectionMade( int selection )
{
    m_screenState = SCREEN_STATE_GOTO_VIEW;
    m_elapsedTime = 0;

    CGuiScreenScrapBookContents* scrapBookContents = static_cast<CGuiScreenScrapBookContents*>( m_guiManager->FindWindowByID( GUI_SCREEN_ID_SCRAP_BOOK_CONTENTS ) );
    rAssert( scrapBookContents != NULL );

    // hide menu cursor
    //
    rAssert( m_pMenu != NULL );
    m_pMenu->GetCursor()->SetVisible( false );

    // hide level bar
    //
    scrapBookContents->SetLevelBarVisible( false );

    // show mission info layer
    //
    rAssert( m_missionInfo != NULL );
    m_missionInfo->SetVisible( true );

    if( selection == MISSION_8 ) // bonus mission
    {
        this->SetButtonVisible( BUTTON_ICON_ACCEPT, false );
    }

    // update mission number and title
    //
    rAssert( m_missionNum != NULL );
    m_missionNum->SetIndex( selection );

    int missionTitleIndex = scrapBookContents->GetCurrentLevel() * NUM_MISSIONS_PER_LEVEL + selection;
    rAssert( m_missionTitle != NULL );
    m_missionTitle->SetIndex( missionTitleIndex );

    // calculate the initial projectile velocity
    //
    Scrooby::BoundedDrawable* pDrawable = m_pMenu->GetMenuItem( selection )->GetItem();
    rAssert( pDrawable != NULL );

    int startPosX = 0;
    int startPosY = 0;
    pDrawable->GetOriginPosition( startPosX, startPosY );

    m_projectileVelocity.x = (MISSION_VIEW_POS_X - startPosX) / MISSION_VIEW_TRANSITION_TIME;
    m_projectileVelocity.y = (MISSION_VIEW_POS_Y - startPosY - 0.5f * MISSION_VIEW_PROJECTILE_GRAVITY * MISSION_VIEW_TRANSITION_TIME * MISSION_VIEW_TRANSITION_TIME) / MISSION_VIEW_TRANSITION_TIME;
}

void
CGuiScreenMissionGallery::OnStartMission()
{
    CGuiScreenScrapBookContents* scrapBookContents = static_cast<CGuiScreenScrapBookContents*>( m_guiManager->FindWindowByID( GUI_SCREEN_ID_SCRAP_BOOK_CONTENTS ) );
    rAssert( scrapBookContents != NULL );
    unsigned int level = scrapBookContents->GetCurrentLevel();

    int mission = m_selectedMission;

    // special case for level 1 due to tutorial mission being m0
    //
    if( level == 0 )
    {
        mission++;
    }

    GetCharacterSheetManager()->SetCurrentMission( static_cast<RenderEnums::LevelEnum>( level ),
                                                   static_cast<RenderEnums::MissionEnum>( mission ) );

    // hide mission info layer
    //
    rAssert( m_missionInfo != NULL );
    m_missionInfo->SetVisible( false );

    if( m_isMissionImagesLoaded )
    {
        this->UnloadMissionImages();
    }

    // send message to front-end manager to quit front-end and
    // start single player story mode
    //
    m_pParent->HandleMessage( GUI_MSG_QUIT_FRONTEND, 1 ); // 1 = one player
}

int
CGuiScreenMissionGallery::LoadMissionImages()
{
    // load mission bitmaps for current level
    //
    CGuiScreenScrapBookContents* scrapBookContents = static_cast<CGuiScreenScrapBookContents*>( m_guiManager->FindWindowByID( GUI_SCREEN_ID_SCRAP_BOOK_CONTENTS ) );
    rAssert( scrapBookContents != NULL );

    int numImagesToLoad = 0;

    rAssert( m_pMenu != NULL );

    for( int i = 0; i < NUM_MISSIONS_PER_LEVEL; i++ )
    {
        Scrooby::BoundedDrawable* missionImage = m_pMenu->GetMenuItem( i )->GetItem();
        rAssert( missionImage != NULL );
        missionImage->SetAlpha( 1.0f ); // restore alpha by default

        bool isUnlocked = false;

        if( GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_UNLOCK_MISSIONS ) )
        {
            isUnlocked = true;
        }
        else
        {
            if( i == NUM_MISSIONS_PER_LEVEL - 1 )
            {
                isUnlocked = GetCharacterSheetManager()->QueryBonusMissionCompleted( static_cast<RenderEnums::LevelEnum>( scrapBookContents->GetCurrentLevel() ) );
            }
            else
            {
                int missionIndex = scrapBookContents->GetCurrentLevel() == 0 ? i + 1 : i;
                MissionRecord* pMissionRecord = GetCharacterSheetManager()->QueryMissionStatus( static_cast<RenderEnums::LevelEnum>( scrapBookContents->GetCurrentLevel() ), missionIndex );
                rAssert( pMissionRecord != NULL );
                if( pMissionRecord->mCompleted )
                {
                    isUnlocked = true;
                }
                else if( pMissionRecord->mSkippedMission )
                {
                    isUnlocked = true;

                    missionImage->SetAlpha( 0.5f ); // 50% alpha for skipped missions
                }
            }
        }

        m_pMenu->SetMenuItemEnabled( i, isUnlocked );

        if( isUnlocked )
        {
            char filename[ 64 ];
            sprintf( filename, "%smis%02d_%02d.p3d",
                     MISSION_GALLERY_IMAGES_DIR,
                     scrapBookContents->GetCurrentLevel() + 1,
                     i + 1 );

            GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D,
                                             filename,
                                             GMA_LEVEL_FE,
                                             MISSION_GALLERY_INVENTORY_SECTION,
                                             MISSION_GALLERY_INVENTORY_SECTION );

            numImagesToLoad++;
        }
    }

    if( numImagesToLoad > 0 )
    {
        GetLoadingManager()->AddCallback( this );

        // hide images until they're loaded
        //
        m_menuGroup->SetAlpha( 0.0f );

        m_numTransitionsPending++;
    }

    this->SetButtonVisible( BUTTON_ICON_ACCEPT, numImagesToLoad > 0 );

    return numImagesToLoad;
}

void
CGuiScreenMissionGallery::UnloadMissionImages()
{
    p3d::pddi->DrawSync();

    // clear all mission images
    //
    rAssert( m_pMenu != NULL );
    for( int i = 0; i < NUM_MISSIONS_PER_LEVEL; i++ )
    {
        Scrooby::Sprite* missionImage = dynamic_cast<Scrooby::Sprite*>( m_pMenu->GetMenuItem( i )->GetItem() );
        rAssert( missionImage != NULL );
        missionImage->SetRawSprite( NULL, true );
        missionImage->ResetTransformation();

        m_pMenu->SetMenuItemEnabled( i, false );
    }

    // unload mission bitmaps
    //
    p3d::inventory->RemoveSectionElements( MISSION_GALLERY_INVENTORY_SECTION );

    m_isMissionImagesLoaded = false;
}

#ifdef RAD_WIN32
void CGuiScreenMissionGallery::SetVisibilityForAllOtherMenuItems( bool bVisible )
{
    for( int i = 0; i < NUM_MISSIONS_PER_LEVEL; i++ )
    {
        if( i != m_selectedMission )
            m_pMenu->GetMenuItem(i)->GetItem()->SetVisible( bVisible );
    }
}
#endif
