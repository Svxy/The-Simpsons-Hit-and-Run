//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenPurchaseRewards
//
// Description: Implementation of the CGuiScreenPurchaseRewards class.
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
#include <presentation/gui/ingame/guiscreenpurchaserewards.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>
#include <presentation/gui/ingame/hudevents/hudcoincollected.h>
#include <presentation/gui/guimenu.h>
#include <presentation/gui/guitextbible.h>

#include <ai/actionbuttonhandler.h>
#include <cheats/cheatinputsystem.h>
#include <mission/gameplaymanager.h>
#include <mission/rewards/rewardsmanager.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <mission/rewards/reward.h>
#include <sound/soundmanager.h>

#include <worldsim/character/charactermanager.h>
#include <worldsim/character/character.h>
#include <worldsim/coins/coinmanager.h>

#include <p3d/anim/animate.hpp>
#include <p3d/anim/poseanimation.hpp>
#include <p3d/anim/skeleton.hpp>
#include <raddebug.hpp> // Foundation

// Scrooby Header Files
//
#include <screen.h>
#include <page.h>
#include <group.h>
#include <sprite.h>
#include <pure3dobject.h>

#include <string.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

const float PREVIEW_CHARACTER_SCALE = 2.0f;
const float PREVIEW_PEDESTAL_SCALE_FOR_SKINS = 0.75f;

const float PREVIEW_VEHICLE_RADIUS = 3.6f;

const tUID CHARACTER_NAME_HOMER = tEntity::MakeUID( "homer" );
const tUID CHARACTER_NAME_BART = tEntity::MakeUID( "bart" );
const tUID CHARACTER_NAME_LISA = tEntity::MakeUID( "lisa" );
const tUID CHARACTER_NAME_MARGE = tEntity::MakeUID( "marge" );
const tUID CHARACTER_NAME_APU = tEntity::MakeUID( "apu" );

const char* PURCHASE_REWARDS_INVENTORY_SECTION = "FE_PurchaseRewardsScreen";
const char* PURCHASE_REWARDS_BGD = "art\\frontend\\scrooby\\resource\\pure3d\\rewardbg.p3d";
const char* PURCHASE_REWARDS_BGD_DRAWABLE = "Pedestal_Scenegraph";
const char* PURCHASE_REWARDS_BGD_CAMERA = "Pedestal_Camera";
const char* PURCHASE_REWARDS_BGD_MULTICONTROLLER = "Pedestal_MasterController";

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// CGuiScreenPurchaseRewards::CGuiScreenPurchaseRewards
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
CGuiScreenPurchaseRewards::CGuiScreenPurchaseRewards
(
    Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   IGuiScreenRewards( pScreen,
                       pScreen->GetPage( "Rewards" ),
                       pParent,
                       GUI_SCREEN_ID_PURCHASE_REWARDS ),
    m_currentType( Merchandise::INVALID_SELLER_TYPE ),
    m_isPurchasingReward( false ),
    m_elapsedCoinDecrementTotalTime( 0 ),
    m_elapsedCoinDecrementTime( 0 ),
    m_bankValueBeforePurchase( 0 ),
    m_purchaseLabel( NULL ),
    m_skinMultiController( NULL ),
    m_skinAnimController( NULL )
{
    this->SetFadingEnabled( false );
    this->SetIrisWipeEnabled( true );

    // Retrieve the Scrooby drawing elements (from Rewards page).
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "Rewards" );
    rAssert( pPage != NULL );

#ifdef RAD_WIN32
    m_leftArrow = pPage->GetGroup( "Arrows" )->GetSprite( "LeftArrow" );
    m_rightArrow = pPage->GetGroup( "Arrows" )->GetSprite( "RightArrow" );
    m_leftArrow->ScaleAboutCenter( 1.3f );
    m_rightArrow->ScaleAboutCenter( 1.3f );
#endif

    // Retrieve the Scrooby drawing elements (from Coins page).
    //
    pPage = m_pScroobyScreen->GetPage( "Coins" );
    rAssert( pPage != NULL );
    CGuiScreenHud::SetNumCoinsDisplay( pPage->GetSprite( "NumCoins" ) );

//    m_numCoins.SetScroobyText( pPage->GetGroup( "NumCoins" ), "NumCoins" );

    // get purchase button label
    //
    rAssert( m_buttonIcons[ BUTTON_ICON_ACCEPT ] != NULL );
    m_purchaseLabel = m_buttonIcons[ BUTTON_ICON_ACCEPT ]->GetText( "Accept" );
    rAssert( m_purchaseLabel != NULL );
/*
    pPage = m_pScroobyScreen->GetPage( "Buy" );
    if( pPage != NULL )
    {
        m_buttonIcons[ BUTTON_ICON_ACCEPT ] = pPage->GetGroup( "AcceptLabel" );
        rAssert( m_buttonIcons[ BUTTON_ICON_ACCEPT ] != NULL );

        m_purchaseLabel = m_buttonIcons[ BUTTON_ICON_ACCEPT ]->GetText( "Accept" );
        rAssert( m_purchaseLabel != NULL );
    }
*/

    // create and setup controllers for character animations
    //
    m_skinMultiController = new tMultiController( 1, 90.0f );
    rAssert( m_skinMultiController != NULL );
    m_skinMultiController->AddRef();
    m_skinMultiController->SetFramerate( 30.0f );

    m_skinAnimController = new tPoseAnimationController;
    rAssert( m_skinAnimController != NULL );
    m_skinAnimController->AddRef();
    m_skinMultiController->SetTrack( 0, m_skinAnimController );

    m_skinTrackInfo.startTime = 0.0f;
    m_skinTrackInfo.endTime = 0.0f;
    m_skinTrackInfo.offset = 0.0f;
    m_skinTrackInfo.scale = 1.0f;
    m_skinMultiController->SetTrackInfo( 0, &m_skinTrackInfo );

    rAssert( m_previewWindow != NULL );
    m_previewWindow->SetMultiController( m_skinMultiController );

    p3d::inventory->AddSection( PURCHASE_REWARDS_INVENTORY_SECTION );
}


//===========================================================================
// CGuiScreenPurchaseRewards::~CGuiScreenPurchaseRewards
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
CGuiScreenPurchaseRewards::~CGuiScreenPurchaseRewards()
{
    p3d::pddi->DrawSync();

    p3d::inventory->DeleteSection( PURCHASE_REWARDS_INVENTORY_SECTION );

    if( m_skinAnimController != NULL )
    {
        m_skinAnimController->Release();
        m_skinAnimController = NULL;
    }

    if( m_skinMultiController != NULL )
    {
        m_skinMultiController->Release();
        m_skinMultiController = NULL;
    }
}


//===========================================================================
// CGuiScreenPurchaseRewards::HandleMessage
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

void CGuiScreenPurchaseRewards::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2 
)
{
    if( message == GUI_MSG_WINDOW_ENTER )
    {
        // set current purchase center type
        //
        m_currentType = static_cast<Merchandise::eSellerType>( param1 );
        rAssert( m_currentType != Merchandise::INVALID_SELLER_TYPE );
    }

    if( m_isLoading )
    {
        if( message == GUI_MSG_CONTROLLER_SELECT ||
            message == GUI_MSG_CONTROLLER_BACK )
        {
            // don't allow user to select or back out of screen
            // during loading
            //
            return;
        }
    }

    rAssert( m_pRewardsMenu != NULL );
    if( m_pRewardsMenu->HasSelectionBeenMade() || m_isLoadingReward || m_isPurchasingReward )
    {
        if( this->IsControllerMessage( message ) )
        {
            // selection has already been made or reward is currently loading,
            // ignore all controller inputs
            //
            return;
        }
    }

    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        int selectionDelta = 0;

        switch( message )
        {
            case GUI_MSG_CONTROLLER_SELECT:
            {
                if( !this->IsButtonVisible( BUTTON_ICON_ACCEPT ) )
                {
                    // not enough coins to purchase reward
                    //
                    GetEventManager()->TriggerEvent( EVENT_FE_LOCKED_OUT );

                    return;
                }

                break;
            }
            case GUI_MSG_CONTROLLER_BACK:
            {
                if( !m_pRewardsMenu->HasSelectionBeenMade() )
                {
                    GetEventManager()->TriggerEvent( EVENT_FE_MENU_BACK );

                    m_pParent->HandleMessage( GUI_MSG_RESUME_INGAME );
                }

                break;
            }
            case GUI_MSG_MENU_SELECTION_MADE:
            {
                // load selected reward item
                //
                this->PurchaseReward();

                break;
            }
            case GUI_MSG_CONTROLLER_LEFT:
            {
                selectionDelta = -2;

                // follow-thru
                //
            }
            case GUI_MSG_CONTROLLER_RIGHT:
            {
                selectionDelta++;

                int previousSelection = 0;

                if( m_currentType == Merchandise::SELLER_INTERIOR )
                {
                    if( m_numPreviewClothing > 1 )
                    {
                        previousSelection = m_currentPreviewClothing;

                        m_currentPreviewClothing = (m_currentPreviewClothing + m_numPreviewClothing + selectionDelta) % m_numPreviewClothing;

                        this->On3DModelSelectionChange( &m_previewClothing[ m_currentPreviewClothing ] );
                    }
                }
                else
                {
                    if( m_numPreviewVehicles > 1 )
                    {
                        previousSelection = m_currentPreviewVehicle;

                        m_currentPreviewVehicle = (m_currentPreviewVehicle + m_numPreviewVehicles + selectionDelta) % m_numPreviewVehicles;

                        this->On3DModelSelectionChange( &m_previewVehicles[ m_currentPreviewVehicle ] );
                    }
                }

                this->UpdateRewardPrice();

                break;
            }
            default:
            {
                break;
            }
        }
    }
    else if( m_state == GUI_WINDOW_STATE_INTRO )
    {
        if( message == GUI_MSG_UPDATE )
        {
            if( m_numTransitionsPending == 1 && m_currentIrisState == IRIS_STATE_CLOSED )
            {
                // resume iris wipe
                //
                this->IrisWipeOpen();

                // show the preview pedestal
                //
                rAssert( m_previewPedestal != NULL );
                m_previewPedestal->SetVisible( true );
                rAssert( m_previewBgd != NULL );
                m_previewBgd->SetVisible( true );
            }
        }
    }
    else if( m_state == GUI_WINDOW_STATE_OUTRO )
    {
        if( message == GUI_MSG_UPDATE )
        {
            if( m_numTransitionsPending == 1 && m_currentIrisState == IRIS_STATE_CLOSED )
            {
                // resume iris wipe
                //
                this->IrisWipeOpen();

                p3d::pddi->DrawSync();

                // hide the preview pedestal
                //
                rAssert( m_previewPedestal != NULL );
                m_previewPedestal->SetVisible( false );
                rAssert( m_previewBgd != NULL );
                m_previewBgd->SetVisible( false );

                m_previewBgd->SetDrawable( NULL );
                m_previewBgd->SetCamera( NULL );
                m_previewBgd->SetMultiController( NULL );

                p3d::inventory->RemoveSectionElements( PURCHASE_REWARDS_INVENTORY_SECTION );
            }
        }
    }

	// Propogate the message up the hierarchy.
	//
    IGuiScreenRewards::HandleMessage( message, param1, param2 );
}

void
CGuiScreenPurchaseRewards::OnProcessRequestsComplete( void* pUserData )
{
    if( reinterpret_cast<Scrooby::Pure3dObject*>( pUserData ) == m_previewBgd )
    {
        m_numTransitionsPending--;

        // push and select inventory section for searching
        //
        p3d::inventory->PushSection();
        p3d::inventory->SelectSection( PURCHASE_REWARDS_INVENTORY_SECTION );
        bool currentSectionOnly = p3d::inventory->GetCurrentSectionOnly();
        p3d::inventory->SetCurrentSectionOnly( true );

        // search for pedestal drawable
        //
        tDrawable* drawable = p3d::find<tDrawable>( PURCHASE_REWARDS_BGD_DRAWABLE );
        if( drawable != NULL )
        {
            rAssert( m_previewBgd != NULL );
            m_previewBgd->SetDrawable( drawable );

            tCamera* camera = p3d::find<tCamera>( PURCHASE_REWARDS_BGD_CAMERA );
            rAssert( camera != NULL );
            m_previewBgd->SetCamera( camera );
            m_previewPedestal->SetCamera( camera );
            m_previewWindow->SetCamera( camera );

            tMultiController* multiController = p3d::find<tMultiController>( PURCHASE_REWARDS_BGD_MULTICONTROLLER );
            m_previewBgd->SetMultiController( multiController );
        }
        else
        {
            rAssertMsg( false, "*** Can't find 3D pedestal drawable!" );
        }

        // pop inventory section and restore states
        //
        p3d::inventory->SetCurrentSectionOnly( currentSectionOnly );
        p3d::inventory->PopSection();
    }
    else if( m_isLoadingReward )
    {
        // this means we were waiting for the selected character skin to load;
        // so, now that it's loaded, let's exit outa here
        //
        m_pParent->HandleMessage( GUI_MSG_RESUME_INGAME );
    }
    else
    {
        IGuiScreenRewards::OnProcessRequestsComplete( pUserData );
    }
}

#ifdef RAD_WIN32
//===========================================================================
// CGuiScreenPurchaseRewards::CheckCursorAgainstHotspots
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
eFEHotspotType CGuiScreenPurchaseRewards::CheckCursorAgainstHotspots( float x, float y )
{
    eFEHotspotType hotSpotType = CGuiScreen::CheckCursorAgainstHotspots( x, y );
    if( hotSpotType == HOTSPOT_NONE )
    {
        if( m_leftArrow )
        {
            if( m_leftArrow->IsPointInBoundingRect( x, y ) )
            {
                hotSpotType = HOTSPOT_ARROWLEFT;
            }
        }
        if( m_rightArrow )
        {        
            if( m_rightArrow->IsPointInBoundingRect( x, y ) )
            {
                hotSpotType = HOTSPOT_ARROWRIGHT;
            }
        }

    }
    return hotSpotType;
}
#endif

//===========================================================================
// CGuiScreenPurchaseRewards::InitIntro
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
void CGuiScreenPurchaseRewards::InitIntro()
{
    // load 3D pedestal
    //
    GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D,
                                     PURCHASE_REWARDS_BGD,
                                     GMA_LEVEL_MISSION,
                                     PURCHASE_REWARDS_INVENTORY_SECTION,
                                     PURCHASE_REWARDS_INVENTORY_SECTION,
                                     this,
                                     reinterpret_cast<void*>( m_previewBgd ) );
    m_numTransitionsPending++;

    IGuiScreenRewards::InitIntro();

    // load current 3D model object
    //
    this->On3DModelSelectionChange( this->GetCurrentPreviewObject() );

    // show and update reward price
    //
    rAssert( m_rewardPrice != NULL );
    m_rewardPrice->SetVisible( true );
    this->UpdateRewardPrice();

    // show/hide stats button label
    //
    rAssert( m_statsOverlayButton );
    m_statsOverlayButton->SetVisible( m_currentType != Merchandise::SELLER_INTERIOR );

    // hide vehicle stats by default
    //
    this->SetVehicleStatsVisible( false );

    // adjust pedestal scale
    //
    rAssert( m_previewPedestal != NULL );
    if( m_currentType == Merchandise::SELLER_INTERIOR )
    {
        // scale down pedestal a bit
        //
        m_previewPedestal->SetDrawableScale( PREVIEW_PEDESTAL_SCALE_FOR_SKINS );

        // notify sound manager that game is paused, include dialogue in
        // ducking to kill gag sounds -- Esan
        //
        GetSoundManager()->DuckEverythingButMusicBegin( true );
    }
    else
    {
        m_previewPedestal->SetDrawableScale( 1.0f );

        // notify sound manager that game is paused, don't duck dialogue
        // so we can still hear ol' Gil -- Esan
        //
        GetSoundManager()->OnStoreScreenStart( true );
    }

    // enable/disable L/R arrows
    //
    if( m_numPreviewClothing > 1 || m_numPreviewVehicles > 1 )
    {
        m_pRewardsMenu->SetSelectionValueCount( 0, 2 );
    }
    else
    {
        m_pRewardsMenu->SetSelectionValueCount( 0, 1 );
    }

    m_bankValueBeforePurchase = GetCoinManager()->GetBankValue();
}

//===========================================================================
// CGuiScreenPurchaseRewards::InitRunning
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
void CGuiScreenPurchaseRewards::InitRunning()
{
    IGuiScreenRewards::InitRunning();

    CGuiScreenHud::UpdateNumCoinsDisplay( m_bankValueBeforePurchase );
}

//===========================================================================
// CGuiScreenPurchaseRewards::InitOutro
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
void CGuiScreenPurchaseRewards::InitOutro()
{
    // hide 3D coin on FE render layer
    //
    CGuiScreenHud::UpdateNumCoinsDisplay( 0, false );

    IGuiScreenRewards::InitOutro();

    if( m_currentType == Merchandise::SELLER_INTERIOR )
    {
        // notify sound manager that game is paused
        //
        GetSoundManager()->DuckEverythingButMusicEnd( true );
    }
    else
    {
        GetSoundManager()->OnStoreScreenEnd();
    }
}

void
CGuiScreenPurchaseRewards::On3DModelLoaded( const PreviewObject* previewObject )
{
    p3d::pddi->DrawSync();

    // search for 3D model in inventory
    //
    tDrawable* drawable = p3d::find<tDrawable>( previewObject->nameModel );
    if( drawable != NULL )
    {
        rAssert( m_previewWindow != NULL );
        m_previewWindow->SetDrawable( drawable );

        if( m_currentType != Merchandise::SELLER_INTERIOR )
        {
            // place vehicles (w/ wheels) on the ground (y = 0)
            //
            rmt::Box3D box3D;
            drawable->GetBoundingBox( &box3D );
            float groundOffset = box3D.low.y;
            m_previewWindow->SetDrawableTranslation( 0.0f, -groundOffset, 0.0f );

            // scale vehicles so that they fit nicely on pedestal
            //
            rmt::Sphere sphere;
            drawable->GetBoundingSphere( &sphere );
            m_previewWindow->SetDrawableScale( PREVIEW_VEHICLE_RADIUS / sphere.radius );
        }
        else
        {
            // assume characters are already placed on ground (y = 0)
            //
            m_previewWindow->SetDrawableTranslation( 0.0f, 0.0f, 0.0f );

            // scale characters larger to fill the screen
            //
            m_previewWindow->SetDrawableScale( PREVIEW_CHARACTER_SCALE );

            // set character animation
            //
            HeapMgr()->PushHeap( GMA_LEVEL_MISSION );
			tDrawablePose *drawable_pose = dynamic_cast<tDrawablePose *>(drawable);
		    rAssert( drawable_pose != NULL );

            m_skinAnimController->SetPose( drawable_pose->GetPose() );

            p3d::inventory->SetCurrentSectionOnly( false );
			char anim_name[32];
			switch(GetGameplayManager()->GetCurrentLevelIndex())
			{
			case 0:
			case 6:
				strcpy(anim_name,"hom");
				break;
			case 1:
			case 5:
				strcpy(anim_name,"brt");
				break;
			case 2:
				strcpy(anim_name,"lsa");
				break;
			case 3:
				strcpy(anim_name,"mrg");
				break;
			case 4:
				strcpy(anim_name,"apu");
				break;
            default:
                strcpy( anim_name,"hom" );
                break;
			}
			strcat(anim_name, "_loco_idle_rest");
            tAnimation* anim = p3d::find<tAnimation>( anim_name );
            rAssert( anim != NULL );
            anim->SetCyclic( true );

            rAssert( m_skinAnimController != NULL );
            m_skinAnimController->SetAnimation( anim, 0.0f, 0.0f );

            m_skinTrackInfo.endTime = anim->GetNumFrames();

            rAssert( m_skinMultiController != NULL );
            m_skinMultiController->Reset();
            m_skinMultiController->SetFrame( 0.0f );
            m_skinMultiController->SetTrackInfo( 0, &m_skinTrackInfo );

            HeapMgr()->PopHeap( GMA_LEVEL_MISSION );

        }
    }
    else
    {
        rAssertMsg( false, "*** Can't find drawable for 3D model!!" );
    }
}

const PreviewObject*
CGuiScreenPurchaseRewards::GetCurrentPreviewObject() const
{
    const PreviewObject* previewObject = NULL;

    switch( m_currentType )
    {
        case Merchandise::SELLER_INTERIOR:
        {
            if( m_numPreviewClothing > 0 )
            {
                previewObject = &m_previewClothing[ m_currentPreviewClothing ];
            }

            break;
        }
        case Merchandise::SELLER_SIMPSON:
        case Merchandise::SELLER_GIL:
        {
            if( m_numPreviewVehicles > 0 )
            {
                previewObject = &m_previewVehicles[ m_currentPreviewVehicle ];
            }

            break;
        }
        default:
        {
            rAssert( false );
            break;
        }
    }

    return previewObject;
}

void
CGuiScreenPurchaseRewards::InitMenu()
{
    int currentLevel = GetGameplayManager()->GetCurrentLevelIndex();
    Reward* pReward = NULL;

    m_numPreviewClothing = 0;
    m_numPreviewVehicles = 0;

    switch( m_currentType )
    {
        case Merchandise::SELLER_INTERIOR:
        {
            Character* currentCharacter = GetCharacterManager()->GetCharacter( 0 );
            rAssert( currentCharacter != NULL );
            const char* currentSkinName = GetCharacterManager()->GetModelName( currentCharacter );

            // insert default character skin first
            //
            pReward = GetRewardsManager()->GetReward( currentLevel, Reward::eDefaultSkin );
            if( pReward != NULL )
            {
                if( strcmp( currentSkinName, pReward->GetName() ) != 0 )
                {
                    m_numPreviewClothing = this->InsertPreviewObject( m_previewClothing,
                                                                      m_numPreviewClothing,
                                                                      pReward );
                }
            }

            // insert all other purchasable skins
            //
            for( pReward = GetRewardsManager()->FindFirstMerchandise( currentLevel, m_currentType );
                 pReward != NULL;
                 pReward = GetRewardsManager()->FindNextMerchandise( currentLevel, m_currentType ) )
            {
                if( strcmp( currentSkinName, pReward->GetName() ) != 0 )
                {
                    rAssert( m_numPreviewClothing < MAX_NUM_PREVIEW_CLOTHING );
                    m_numPreviewClothing = this->InsertPreviewObject( m_previewClothing,
                                                                      m_numPreviewClothing,
                                                                      pReward );
                }
            }

            if( GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_UNLOCK_SKINS ) )
            {
                for( int i = 0; i < m_numPreviewClothing; i++ )
                {
                    m_previewClothing[ i ].isUnlocked = true;
                }
            }

            break;
        }
        case Merchandise::SELLER_SIMPSON:
        case Merchandise::SELLER_GIL:
        {
            for( pReward = GetRewardsManager()->FindFirstMerchandise( currentLevel, m_currentType );
                 pReward != NULL;
                 pReward = GetRewardsManager()->FindNextMerchandise( currentLevel, m_currentType ) )
            {
                // only display locked rewards
                //
                if( !pReward->RewardStatus() )
                {
                    rAssert( m_numPreviewVehicles < MAX_NUM_PREVIEW_VEHICLES );
                    m_numPreviewVehicles = this->InsertPreviewObject( m_previewVehicles,
                                                                      m_numPreviewVehicles,
                                                                      pReward,
                                                                      true );
                }
            }

            break;
        }
        default:
        {
            rAssert( false );
            break;
        }
    }
}

void
CGuiScreenPurchaseRewards::OnUpdate( unsigned int elapsedTime )
{
/*
    // always show accept button
    //
    this->SetButtonVisible( BUTTON_ICON_ACCEPT, true );
*/
/*
    // rotate reward object
    //
    const unsigned int ROTATION_PERIOD = 3000;

    m_elapsedRotationTime = (m_elapsedRotationTime + elapsedTime) % ROTATION_PERIOD;

    rAssert( m_previewWindow != NULL );
    m_previewWindow->ResetTransformation();

    float rotation = (m_elapsedRotationTime / (float)ROTATION_PERIOD) * 360.0f;
    m_previewWindow->RotateAboutCenter( rotation,
                                        rmt::Vector( 0.0f, 1.0f, 0.0f ) );
*/
    if( m_isPurchasingReward )
    {
        const unsigned int COINS_DECREMENT_DURATION = 1000; // in msec
        const unsigned int COINS_DECREMENT_PERIOD = 100; // in msec

        m_elapsedCoinDecrementTotalTime += elapsedTime;
        if( m_elapsedCoinDecrementTotalTime < COINS_DECREMENT_DURATION )
        {
            m_elapsedCoinDecrementTime += elapsedTime;
            if( m_elapsedCoinDecrementTime > COINS_DECREMENT_PERIOD )
            {
                int deltaCoins = m_bankValueBeforePurchase - GetCoinManager()->GetBankValue();
                int bankValue = m_bankValueBeforePurchase - (int)( m_elapsedCoinDecrementTotalTime / (float)COINS_DECREMENT_DURATION * deltaCoins );

                CGuiScreenHud::UpdateNumCoinsDisplay( bankValue );

                GetCoinManager()->AddFlyDownCoin();

                m_elapsedCoinDecrementTime %= COINS_DECREMENT_PERIOD;
            }
        }
        else
        {
            CGuiScreenHud::UpdateNumCoinsDisplay( GetCoinManager()->GetBankValue() );

            m_isPurchasingReward = false;

            // ok, we've reached the real current bank value;
            // now load the purchased reward now
            //
            this->LoadSelectedReward();
/*
            if( m_currentType == Merchandise::SELLER_INTERIOR )
            {
                this->LoadSelectedReward();
            }
            else
            {
                // don't load vehicles, user must go to the phone
                // booth to do that
                //
                m_pParent->HandleMessage( GUI_MSG_BACK_SCREEN );
            }
*/
        }
    }

    IGuiScreenRewards::OnUpdate( elapsedTime );
}

void
CGuiScreenPurchaseRewards::UpdateRewardPrice()
{
    const PreviewObject* currentPreviewObject = this->GetCurrentPreviewObject();
    rAssert( currentPreviewObject != NULL );

    if( currentPreviewObject->isUnlocked )
    {
        this->SetButtonVisible( BUTTON_ICON_ACCEPT, true );
    }
    else
    {
        Merchandise* pMerchandise = GetRewardsManager()->GetMerchandise( GetGameplayManager()->GetCurrentLevelIndex(),
                                                                        currentPreviewObject->name );
        if( pMerchandise != NULL )
        {
            HeapMgr()->PushHeap( GMA_LEVEL_HUD );

            int rewardCost = pMerchandise->GetCost();
            rAssertMsg( rewardCost > 0, "Reward price should be a positive integer!" );

            // get "coins" text
            //
            char coinsText[ 32 ];
            p3d::UnicodeToAscii( GetTextBibleString( "COINS" ), coinsText, sizeof( coinsText ) );

            char buffer[ 32 ];
#ifdef PAL
            if( CGuiTextBible::GetCurrentLanguage() == Scrooby::XL_GERMAN )
            {
                // special case for German for proper grammar
                //
                sprintf( buffer, "Für %d %s", rewardCost, coinsText );
            }
            else
#endif // PAL
            {
                sprintf( buffer, "%d %s", rewardCost, coinsText );
            }

            rAssert( strlen( buffer ) < sizeof( buffer ) );

            rAssert( m_rewardPrice != NULL );
            m_rewardPrice->SetString( 0, buffer );

            // only show accept label if user has enough money to purchase OR
            // if reward is already unlocked
            //
            this->SetButtonVisible( BUTTON_ICON_ACCEPT, GetCoinManager()->GetBankValue() >= rewardCost );

            HeapMgr()->PopHeap( GMA_LEVEL_HUD );
        }
    }

    // update purchase label
    //
    rAssert( m_purchaseLabel != NULL );
    m_purchaseLabel->SetIndex( currentPreviewObject->isUnlocked ? 1 : 0 );
}

void
CGuiScreenPurchaseRewards::PurchaseReward()
{
    const PreviewObject* currentPreviewObject = this->GetCurrentPreviewObject();
    rAssert( currentPreviewObject != NULL );

    if( currentPreviewObject->isUnlocked )
    {
        // reward is already unlocked, so just load it
        //
        this->LoadSelectedReward();
    }
    else
    {
        bool boughtReward = GetRewardsManager()->BuyMerchandise( GetGameplayManager()->GetCurrentLevelIndex(),
                                                                 currentPreviewObject->name );

        if( boughtReward )
        {
            // hide lock overlay
            //
            rAssert( m_lockedOverlay != NULL );
            m_lockedOverlay->SetVisible( false );

            // update hud coins current item count
            //
            CGuiScreenHud* currentHud = GetCurrentHud();
            if( currentHud != NULL )
            {
                HudCoinCollected* hudCoinCollected = static_cast<HudCoinCollected*>( currentHud->GetEventHandler( CGuiScreenHud::HUD_EVENT_HANDLER_COIN_COLLECTED ) );
                rAssert( hudCoinCollected != NULL );
                hudCoinCollected->SetCurrentItemCount( GetCoinManager()->GetBankValue() );
                hudCoinCollected->Start();
            }

            m_isPurchasingReward = true;
            m_elapsedCoinDecrementTotalTime = 0;
            m_elapsedCoinDecrementTime = 0;
        }
        else
        {
            rAssertMsg( false, "This should not happen!" );
        }
    }
}

void
CGuiScreenPurchaseRewards::LoadSelectedReward()
{
    // unload 3D model first before loading reward
    //
    this->Unload3DModel();

    m_isLoading = true; // set loading flag
    m_isLoadingReward = true; // we're loading the selected reward

    if( m_currentType == Merchandise::SELLER_INTERIOR )
    {
        Character* currentCharacter = GetCharacterManager()->GetCharacter( 0 );

        char characterName[ 16 ];
        tUID characterUID = currentCharacter->GetUID();

        if( characterUID == CHARACTER_NAME_HOMER )
        {
            strcpy( characterName, "homer" );
        }
        else if( characterUID == CHARACTER_NAME_BART )
        {
            strcpy( characterName, "bart" );
        }
        else if( characterUID == CHARACTER_NAME_LISA )
        {
            strcpy( characterName, "lisa" );
        }
        else if( characterUID == CHARACTER_NAME_MARGE )
        {
            strcpy( characterName, "marge" );
        }
        else if( characterUID == CHARACTER_NAME_APU )
        {
            strcpy( characterName, "apu" );
        }
        else
        {
            rAssert( false );
        }

        GetCharacterManager()->SwapData( currentCharacter,
                                         m_previewClothing[ m_currentPreviewClothing ].name,
                                         characterName );

       //Chuck: Save the skin they are buy/changing into as the current skin
        GetCharacterSheetManager()->SetCurrentSkin(GetGameplayManager()->GetCurrentLevelIndex(),m_previewClothing[ m_currentPreviewClothing ].name);

        GetCharacterManager()->PreloadCharacter( m_previewClothing[ m_currentPreviewClothing ].name,
                                                 characterName,
                                                 this );

        //for the HitnRun system
        GetEventManager()->TriggerEvent( EVENT_SWITCH_SKIN, NULL );

    }
    else
    {
        ActionButton::SummonVehiclePhone::LoadVehicle( m_previewVehicles[ m_currentPreviewVehicle ].name,
                                                       m_previewVehicles[ m_currentPreviewVehicle ].filename,
                                                       VehicleCentral::ALLOW_DRIVER );
    }
}

