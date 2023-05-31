//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenPhoneBooth
//
// Description: Implementation of the CGuiScreenPhoneBooth class.
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
#include <presentation/gui/ingame/guiscreenphonebooth.h>
#include <presentation/gui/ingame/guiscreenhud.h>
#include <presentation/gui/utility/specialfx.h>
#include <presentation/gui/guimenu.h>
#include <presentation/gui/guitextbible.h>

#include <ai/actionbuttonhandler.h>
#include <camera/relativeanimatedcam.h>
#include <camera/supercam.h>
#include <camera/supercammanager.h>
#include <cheats/cheatinputsystem.h>
#include <events/eventmanager.h>
#include <memory/srrmemory.h>
#include <mission/gameplaymanager.h>
#include <mission/missionmanager.h>
#include <mission/rewards/rewardsmanager.h>
#include <mission/rewards/reward.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <sound/soundmanager.h>
#include <worldsim/character/charactermanager.h>
#include <worldsim/character/character.h>
#include <worldsim/coins/coinmanager.h>
#include <worldsim/vehiclecentral.h>

#include <screen.h>
#include <page.h>
#include <layer.h>
#include <group.h>
#include <sprite.h>
#include <text.h>
#include <pure3dobject.h>

#include <p3d/anim/cameraanimation.hpp>

#include <raddebug.hpp> // Foundation

#include <string.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================
#ifdef RAD_WIN32
const float VEHICLE_CORRECTION_SCALE = 0.93f;
const float VEHICLE_DAMAGED_CORRECTION_SCALE = 0.93f;
const float HUSK_CORRECTION_SCALE = 1.4f;
#else
const float VEHICLE_CORRECTION_SCALE = 1.0f / 0.75f;
const float VEHICLE_DAMAGED_CORRECTION_SCALE = 1.0f / 0.5f;
const float HUSK_CORRECTION_SCALE = 1.0f / 0.5f;
#endif

const float VEHICLE_LOCKED_IMAGE_ALPHA = 0.5f;

const tUID CHARACTER_VEHICLE_NAMES[] =
{
    tEntity::MakeUID( "apu_v" ),
    tEntity::MakeUID( "bart_v" ),
    tEntity::MakeUID( "comic_v" ),
    tEntity::MakeUID( "cletu_v" ),
    tEntity::MakeUID( "frink_v" ),
    tEntity::MakeUID( "gramp_v" ),
    tEntity::MakeUID( "homer_v" ),
    tEntity::MakeUID( "lisa_v" ),
    tEntity::MakeUID( "marge_v" ),
    tEntity::MakeUID( "skinn_v" ),
    tEntity::MakeUID( "smith_v" ),
    tEntity::MakeUID( "snake_v" ),
    tEntity::MakeUID( "wiggu_v" ),

    0 // dummy terminator
};

const int NUM_CHARACTER_VEHICLE_NAMES =
    sizeof( CHARACTER_VEHICLE_NAMES ) / sizeof( CHARACTER_VEHICLE_NAMES[ 0 ] ) - 1;

static int GetVehicleIndex( const char* vehicleName )
{
    int vehicleIndex = -1;

    tUID vehicleNameUID = tEntity::MakeUID( vehicleName );

    for( int i = 0; i < NUM_CHARACTER_VEHICLE_NAMES; i++ )
    {
        if( CHARACTER_VEHICLE_NAMES[ i ] == vehicleNameUID )
        {
            // found it!
            //
            vehicleIndex = i;

            // stop searching
            //
            break;
        }
    }

    return vehicleIndex;
}

#ifdef SRR2_OVERRIDE_CAR_SELECTION
    const char* OVERRIDE_VEHICLE_NAMES[] =
    {
        "snake_v", 
        "bookb_v", 
        "marge_v", 
        "carhom_v",
		"krust_v", 
        "bbman_v", 
        "elect_v", 
        "famil_v", 
        "bart_v",  
		"scorp_v", 
		"honor_v", 
        "hbike_v", 
        "frink_v", 
 		"comic_v", 
        "lisa_v",  
        "smith_v", 
        "mrplo_v", 
        "fone_v",  
		"cletu_v", 
		"apu_v",   
		"plowk_v", 
       	"wiggu_v", 
		"otto_v",  
        "moe_v",   
		"skinn_v", 
		"homer_v", 
        "zombi_v", 
        "burns_v", 
		"willi_v", 
        "gramp_v", 
        "gramR_v", 

        "atv_v",   
        "knigh_v", 
        "mono_v",  
	    "oblit_v", 
        "hype_v",  
        "dune_v",  
        "rocke_v", 

		"cArmor",  
        "cCellA",  
        "cCellB",  
        "cCellC",  
        "cCellD",  
		"cSedan",  
		"cCola",   
        "cCube",   
        "cCurator",
        "cDonut",  
        "cDuff",   
        "cBlbart", 
		"cHears",  
        "cKlimo",  
        "cLimo",   
		"cMilk",   
        "cNerd",   
        "cNonup",  
        "cPolice", 
        "cVan",    

        //"huskA",   
        "compactA",
        "minivanA",
        "pickupA", 
        "sedanA",  
        "sedanB",  
        "sportsA", 
        "sportsB", 
        "wagonA",  
		"SUVA",    
        "taxiA",   
        "coffin",  
        "ship",    
        "hallo",   
        "witchcar",

        "ambul",   
        "burnsarm",
        "fishtruc",
        "garbage", 
        "icecream",
        "IStruck", 
        "nuctruck",
        "pizza",   
        "schoolbu",
        "votetruc",
        "glastruc",
        "cFire_v",
        "cBone",
        "redbrick",

        "" // dummy terminator
    };

    const unsigned int NUM_OVERRIDE_VEHICLES =
        sizeof( OVERRIDE_VEHICLE_NAMES ) / sizeof( OVERRIDE_VEHICLE_NAMES[ 0 ] ) - 1;

    const char* VEHICLES_DIR = "art\\cars\\";

    int CGuiScreenPhoneBooth::s_currentDebugVehicleSelection = 0;
    unsigned CGuiScreenPhoneBooth::s_currentTeleportSelection = 0;
#endif

//===========================================================================
// CGuiScreenPhoneBooth::
//===========================================================================

//===========================================================================
// CGuiScreenPhoneBooth::CGuiScreenPhoneBooth
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
CGuiScreenPhoneBooth::CGuiScreenPhoneBooth
(
    Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   IGuiScreenRewards( pScreen,
                       pScreen->GetPage( "PhoneBooth" ),
                       pParent,
                       GUI_SCREEN_ID_PHONE_BOOTH ),
    m_damagedInfo( NULL ),
    m_vehicleDamaged( NULL ),
    m_repairCostInfo( NULL ),
    m_vehicleRepairCost( NULL )
{
MEMTRACK_PUSH_GROUP( "CGUIScreenPhoneBooth" );
    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "PhoneBooth" );
	rAssert( pPage != NULL );

#ifdef RAD_WIN32
    m_leftArrow = pPage->GetGroup( "Arrows" )->GetSprite( "LeftArrow" );
    m_rightArrow = pPage->GetGroup( "Arrows" )->GetSprite( "RightArrow" );
    m_leftArrow->ScaleAboutCenter( 1.3f );
    m_rightArrow->ScaleAboutCenter( 1.3f );
#endif

    // get vehicle damaged info
    //
    m_damagedInfo = pPage->GetGroup( "DamagedInfo" );
    rAssert( m_damagedInfo != NULL );
    m_vehicleDamaged = m_damagedInfo->GetText( "VehicleDamaged_Value" );

    m_repairCostInfo = m_damagedInfo->GetGroup( "RepairCostInfo" );
    rAssert( m_repairCostInfo != NULL );
    m_vehicleRepairCost = m_repairCostInfo->GetText( "CostToFix_Value" );

    // always show vehicle stats
    //
    rAssert( m_statsOverlay != NULL );
    m_statsOverlay->SetVisible( true );

#ifdef PAL
    // re-position damaged info text
    //
    const int DAMAGED_INFO_TRANSLATE_X = +60;

    m_vehicleDamaged->ResetTransformation();
    m_vehicleDamaged->Translate( DAMAGED_INFO_TRANSLATE_X, 0 );

    m_vehicleRepairCost->ResetTransformation();
    m_vehicleRepairCost->Translate( DAMAGED_INFO_TRANSLATE_X, 0 );

    Scrooby::Text* repairCostCoins = m_repairCostInfo->GetText( "Coins" );
    if( repairCostCoins != NULL )
    {
        repairCostCoins->ResetTransformation();
        repairCostCoins->Translate( DAMAGED_INFO_TRANSLATE_X, 0 );
#ifdef RAD_WIN32
        repairCostCoins->Translate( 70, 0 );
        repairCostCoins->ScaleAboutCenter( 0.5f );
#endif
    }

    // move stats over to the left a bit to avoid safe zone
    // boundary clipping
    //
    m_statsOverlay->ResetTransformation();
    m_statsOverlay->Translate( -15, 0 );

    // scale-down vehicle name a bit
    //
    m_previewName->ResetTransformation();
    m_previewName->ScaleAboutCenter( 0.9f );
#endif // PAL

    // apply correction scale to spotlight
    //
    Scrooby::Sprite* spotLight = pPage->GetSprite( "Spotlight" );
    if( spotLight != NULL )
    {
        spotLight->ResetTransformation();
        spotLight->ScaleAboutCenter( 4.0f );
    }

    // apply correction scale to spotlight overlay
    //
    spotLight = pPage->GetSprite( "Overlay" );
    if( spotLight != NULL )
    {
        spotLight->ResetTransformation();
        spotLight->ScaleAboutCenter( 4.0f );
    }

    rAssert( m_pRewardsMenu != NULL );
    m_pRewardsMenu->SetHighlightColour( false, tColour( 255, 255, 255 ) );
    m_pRewardsMenu->Reset();

    this->AutoScaleFrame( pPage );

#ifdef SRR2_OVERRIDE_CAR_SELECTION
    pPage = m_pScroobyScreen->GetPage( "CarSelect" );
    if( pPage != NULL )
    {
        m_carSelectOverlay = pPage->GetLayerByIndex( 0 );
        rAssert( m_carSelectOverlay );
    }

    // Create a menu.
    //
    m_pMenu = new CGuiMenu( this );
    rAssert( m_pMenu != NULL );

    // Add menu items
    //
    m_pMenu->AddMenuItem( pPage->GetText( "Car" ),
                          pPage->GetText( "Car_Value" ) );

    // clamp number of vehicle selections w/ only what's available
    //
    m_pMenu->SetSelectionValueCount( 0, NUM_OVERRIDE_VEHICLES );

    // wrap vehicle names
    //
    Scrooby::Text* vehicleNames = dynamic_cast<Scrooby::Text*>( m_pMenu->GetMenuItem( 0 )->GetItemValue() );
    rAssert( vehicleNames != NULL );
    vehicleNames->SetTextMode( Scrooby::TEXT_WRAP );

    m_menuTeleport = false;
#endif
MEMTRACK_POP_GROUP("CGUIScreenPhoneBooth");
}

//===========================================================================
// CGuiScreenPhoneBooth::~CGuiScreenPhoneBooth
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
CGuiScreenPhoneBooth::~CGuiScreenPhoneBooth()
{
#ifdef SRR2_OVERRIDE_CAR_SELECTION
    if( m_pMenu != NULL )
    {
        delete m_pMenu;
        m_pMenu = NULL;
    }
#endif
}

//===========================================================================
// CGuiScreenPhoneBooth::HandleMessage
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
void CGuiScreenPhoneBooth::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2 
)
{
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
    if( m_pRewardsMenu->HasSelectionBeenMade() || m_isLoadingReward )
    {
        if( this->IsControllerMessage( message ) )
        {
            // selection has already been made or reward is currently loading,
            // ignore all controller inputs
            //
            return;
        }
    }

#ifdef SRR2_OVERRIDE_CAR_SELECTION
    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        if( message == GUI_MSG_CONTROLLER_L1 || message == GUI_MSG_CONTROLLER_R1 )
        {
#ifdef FINAL
            if( GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_UNLOCK_VEHICLES ) &&
                GetCharacterSheetManager()->QueryPercentGameCompleted() > 99.999f )
#endif
            {
                rAssert( m_carSelectOverlay != NULL );

                // toggle visibility of car selection overlay
                //
                m_carSelectOverlay->SetVisible( !m_carSelectOverlay->IsVisible() );

                if( m_carSelectOverlay->IsVisible() )
                {
                    this->SetButtonVisible( BUTTON_ICON_ACCEPT, true );

// Teleportation menu disabled in final build
#ifndef FINAL
                    m_menuTeleport = (message == GUI_MSG_CONTROLLER_L1);
#endif
                    Scrooby::Text* menuLabel = dynamic_cast<Scrooby::Text*>( m_pMenu->GetMenuItem( 0 )->GetItem() );
                    rAssert( menuLabel != NULL );

                    HeapMgr()->PushHeap( GMA_LEVEL_HUD );
                    if( !m_menuTeleport )
                    {
                        menuLabel->SetString( 0, "Vehicle:" );
                        m_pMenu->SetSelectionValue( 0, s_currentDebugVehicleSelection );
                    }
                    else
                    {
                        menuLabel->SetString( 0, "Teleport:" );
                        m_pMenu->SetSelectionValue( 0, s_currentTeleportSelection );
                    }
                    HeapMgr()->PopHeap( GMA_LEVEL_HUD );
                }
                else
                {
                    this->SetButtonVisible( BUTTON_ICON_ACCEPT,
                                            this->GetCurrentPreviewObject()->isUnlocked );
                }
            }
        }
    }

    if( m_carSelectOverlay->IsVisible() )
    {
        this->HandleMessageForCar( message, param1, param2 );
    }
    else
#endif
    {
        if( m_state == GUI_WINDOW_STATE_RUNNING )
        {
            switch( message )
            {
                case GUI_MSG_CONTROLLER_SELECT:
                {
                    const PreviewObject* currentPreviewObject = this->GetCurrentPreviewObject();

                    if( currentPreviewObject->isUnlocked )
                    {
                        // check if selecting a damaged out vehicle; if so, reduce current bank
                        // value by repair cost amount
                        //
                        int carIndex = GetCharacterSheetManager()->GetCarIndex( currentPreviewObject->name );
                        if( carIndex >= 0 )
                        {
                            bool isVehicleDamagedOut = ( GetCharacterSheetManager()->GetCarDamageState( carIndex ) == eHusk );
                            if( isVehicleDamagedOut )
                            {
                                int repairCost = currentPreviewObject->pReward->GetRepairCost();

                                // TC: *** Joe says to always allow car repair, even if user
                                //         doesn't quite have enough money
/*
                                if( GetCoinManager()->GetBankValue() < repairCost )
                                {
                                    // doh... not enough money in the bank to repair damaged vehicle
                                    //
                                    return;
                                }
*/
                                GetCoinManager()->AdjustBankValue( -repairCost );

                                // trigger HUD event
                                //
                                GetEventManager()->TriggerEvent( EVENT_LOST_COINS );

                                //Chuck: Adding Record that the car has full hitpoints restored                               
                                GetCharacterSheetManager()->UpdateCarHealth(carIndex,1.0f);
                            }
                        }

                        Character* pWalkerCharacter = GetCharacterManager()->GetCharacter( 0 );
                        rWarning( pWalkerCharacter != NULL );

                        GetEventManager()->TriggerEvent( EVENT_PHONE_BOOTH_RIDE_REQUEST, reinterpret_cast<void*>( pWalkerCharacter ) );

                        bool isBonusVehicle = (m_previewVehicles[ m_currentPreviewVehicle ].pReward->GetQuestType() == Reward::eBonusMission);
                        if( isBonusVehicle )
                        {
                            Character* pDriverCharacter = NULL;

                            Vehicle* pCurrentVehicle = GetGameplayManager()->GetCurrentVehicle();
                            rAssert( pCurrentVehicle != NULL );

                            bool isNewVehicle = ( strcmp( pCurrentVehicle->GetName(), currentPreviewObject->name ) != 0 );
                            if( !isNewVehicle )
                            {
                                //
                                // Re-requesting old vehicle.  Send some dialogue if there's a driver.
                                //
                                pDriverCharacter = pCurrentVehicle->GetDriver();
                                if( pDriverCharacter != NULL )
                                {
                                    GetEventManager()->TriggerEvent( EVENT_PHONE_BOOTH_OLD_VEHICLE_RESELECTED, pDriverCharacter );
                                }
                            }
                        }
                    }
                    else
                    {
                        GetEventManager()->TriggerEvent( EVENT_FE_LOCKED_OUT );
                        return;
                    }

                    break;
                }
                case GUI_MSG_MENU_SELECTION_MADE:
                {
                    // load selected reward item
                    //
                    this->LoadSelectedReward();

                    break;
                }
                case GUI_MSG_CONTROLLER_LEFT:
                {
                    m_currentPreviewVehicle = (m_currentPreviewVehicle - 1 + m_numPreviewVehicles) % m_numPreviewVehicles;

                    this->On3DModelSelectionChange( &m_previewVehicles[ m_currentPreviewVehicle ] );
                    this->UpdateDamagedInfo();

                    break;
                }
                case GUI_MSG_CONTROLLER_RIGHT:
                {
                    m_currentPreviewVehicle = (m_currentPreviewVehicle + 1 + m_numPreviewVehicles) % m_numPreviewVehicles;

                    this->On3DModelSelectionChange( &m_previewVehicles[ m_currentPreviewVehicle ] );
                    this->UpdateDamagedInfo();

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
        IGuiScreenRewards::HandleMessage( message, param1, param2 );
    }
}

#ifdef RAD_WIN32
//===========================================================================
// CGuiScreenPhoneBooth::CheckCursorAgainstHotspots
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
eFEHotspotType CGuiScreenPhoneBooth::CheckCursorAgainstHotspots( float x, float y )
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
// CGuiScreenPhoneBooth::InitIntro
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
void CGuiScreenPhoneBooth::InitIntro()
{
    IGuiScreenRewards::InitIntro();

    // hide lock by default
    //
    rAssert( m_lockedOverlay != NULL );
    m_lockedOverlay->SetVisible( false );

    // set level default vehicle as default garage vehicle selection
    //
    Reward* pDefaultCar = GetRewardsManager()->GetReward( GetGameplayManager()->GetCurrentLevelIndex(),
                                                          Reward::eDefaultCar );
    if( pDefaultCar != NULL )
    {
        for( int i = 0; i < m_numPreviewVehicles; i++ )
        {
            if( m_previewVehicles[ i ].pReward == pDefaultCar )
            {
                m_currentPreviewVehicle = i;

                break;
            }
        }
    }

    this->UpdateDamagedInfo();

    // load current 3D model object
    //
    this->On3DModelSelectionChange( this->GetCurrentPreviewObject() );

    // notify sound manager that game is paused
    //
    GetSoundManager()->OnStoreScreenStart();
}

//===========================================================================
// CGuiScreenPhoneBooth::InitRunning
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
void CGuiScreenPhoneBooth::InitRunning()
{
    IGuiScreenRewards::InitRunning();

    CGuiScreenHud::UpdateNumCoinsDisplay( GetCoinManager()->GetBankValue() );
}

//===========================================================================
// CGuiScreenPhoneBooth::InitOutro
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
void CGuiScreenPhoneBooth::InitOutro()
{
    if( m_isLoadingReward )
    {
        RelativeAnimatedCam::AllowSkipping( true );
        RelativeAnimatedCam::Reset();
        RelativeAnimatedCam::SetCamera( "phonecameraShape" );
        RelativeAnimatedCam::SetMulticontroller( "phonecamera" );
        tCameraAnimationController* camController = p3d::find< tCameraAnimationController >( "CAM_phonecameraShape" );
        
        SuperCam* sc = GetSuperCamManager()->GetSCC( 0 )->GetSuperCam( SuperCam::RELATIVE_ANIMATED_CAM );
        RelativeAnimatedCam* rac = dynamic_cast< RelativeAnimatedCam* >( sc );
        rac->SetCameraAnimationController( camController );
        GetSuperCamManager()->GetSCC( 0 )->SelectSuperCam( SuperCam::RELATIVE_ANIMATED_CAM );
        RelativeAnimatedCam::CheckPendingCameraSwitch();

        GetMissionManager()->Update( 16 );
        GetSuperCamManager()->GetSCC( 0 )->NoTransition();
        GetSuperCamManager()->Update( 16, true );
    }

    CGuiScreenHud::UpdateNumCoinsDisplay( 0, false );

    IGuiScreenRewards::InitOutro();

    // notify sound manager that game is un-paused
    //
    GetSoundManager()->OnStoreScreenEnd();
}

void
CGuiScreenPhoneBooth::On3DModelLoaded( const PreviewObject* previewObject )
{
    p3d::pddi->DrawSync();

    rAssert( m_previewImage != NULL );
    m_previewImage->SetVisible( true );
    m_previewImage->ResetTransformation();

    // apply alpha to locked vehicles
    //
    m_previewImage->SetAlpha( previewObject->isUnlocked ? 1.0f : VEHICLE_LOCKED_IMAGE_ALPHA );

    CarDamageStateEnum damageState = eNull;
    if( previewObject->isUnlocked )
    {
        int carIndex = GetCharacterSheetManager()->GetCarIndex( previewObject->name );
        if( carIndex >= 0 )
        {
            damageState = GetCharacterSheetManager()->GetCarDamageState( carIndex );
        }
    }

    if( damageState == eHusk )
    {
        m_previewImage->ScaleAboutCenter( HUSK_CORRECTION_SCALE );

        // show generic husk
        //
        m_previewImage->SetRawSprite( NULL, true );
    }
    else
    {
        // search for 2D image(s) in inventory
        //
        char name[ 16 ];

        if( damageState == eDamaged )
        {
            // show damaged vehicle
            //
#ifdef LOAD_DAMAGED_VEHICLE_IMAGES
            sprintf( name, "%sD.png", previewObject->nameModel );
            m_previewImage->ScaleAboutCenter( VEHICLE_DAMAGED_CORRECTION_SCALE );
#else
            sprintf( name, "%s.png", previewObject->nameModel );
            m_previewImage->ScaleAboutCenter( VEHICLE_CORRECTION_SCALE );
#endif
        }
        else
        {
            // show normal vehicle
            //
            sprintf( name, "%s.png", previewObject->nameModel );

            m_previewImage->ScaleAboutCenter( VEHICLE_CORRECTION_SCALE );
        }

        tSprite* pSprite = p3d::find<tSprite>( name );
        if( pSprite != NULL )
        {
            m_previewImage->SetRawSprite( pSprite, true );
        }
        else
        {
            rAssertMsg( false, "*** Can't find 2D vehicle image!!" );
        }
    }
}

const PreviewObject*
CGuiScreenPhoneBooth::GetCurrentPreviewObject() const
{
    const PreviewObject* previewObject = NULL;

    if( m_numPreviewVehicles > 0 )
    {
        previewObject = &m_previewVehicles[ m_currentPreviewVehicle ];
    }

    return previewObject;
}

void
CGuiScreenPhoneBooth::InitMenu()
{
    const int highestLevelPlayed = GetCharacterSheetManager()->QueryHighestMission().mLevel;

    for( int i = 0; i <= highestLevelPlayed; i++ )
    {
        Reward* pReward = NULL;

        for( int j = (Reward::eBlank + 1); j < Reward::NUM_QUESTS; j++ )
        {
            pReward = GetRewardsManager()->GetReward( i, (Reward::eQuestType)j );
            if( pReward != NULL &&
                pReward->GetRewardType() == Reward::ALT_PLAYERCAR )
            {
                rAssert( m_numPreviewVehicles < MAX_NUM_PREVIEW_VEHICLES - 1 );
                m_numPreviewVehicles = this->InsertPreviewObject( m_previewVehicles,
                                                                  m_numPreviewVehicles,
                                                                  pReward,
                                                                  true );
            }
        }

        for( pReward = GetRewardsManager()->FindFirstMerchandise( i, Merchandise::SELLER_SIMPSON );
             pReward != NULL;
             pReward = GetRewardsManager()->FindNextMerchandise( i, Merchandise::SELLER_SIMPSON ) )
        {
            rAssert( m_numPreviewVehicles < MAX_NUM_PREVIEW_VEHICLES - 1 );
            m_numPreviewVehicles = this->InsertPreviewObject( m_previewVehicles,
                                                              m_numPreviewVehicles,
                                                              pReward,
                                                              true );
        }

        for( pReward = GetRewardsManager()->FindFirstMerchandise( i, Merchandise::SELLER_GIL );
             pReward != NULL;
             pReward = GetRewardsManager()->FindNextMerchandise( i, Merchandise::SELLER_GIL ) )
        {
            rAssert( m_numPreviewVehicles < MAX_NUM_PREVIEW_VEHICLES - 1 );
            m_numPreviewVehicles = this->InsertPreviewObject( m_previewVehicles,
                                                              m_numPreviewVehicles,
                                                              pReward,
                                                              true );
        }
    }

    rAssertMsg( m_numPreviewVehicles > 0, "No vehicle selections!" );

    // apply cheats, if enabled
    //
    if( GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_UNLOCK_VEHICLES ) )
    {
        for( int i = 0; i < m_numPreviewVehicles; i++ )
        {
            m_previewVehicles[ i ].isUnlocked = true;
        }
    }
}

void CGuiScreenPhoneBooth::OnUpdate( unsigned int elapsedTime )
{
    IGuiScreenRewards::OnUpdate( elapsedTime );
}

void
CGuiScreenPhoneBooth::LoadSelectedReward()
{
    // unload 3D model first before loading reward
    //
    this->Unload3DModel();

    m_isLoading = true; // set loading flag
    m_isLoadingReward = true; // we're loading the selected reward

    const PreviewObject* currentPreviewObject = this->GetCurrentPreviewObject();

    VehicleCentral::DriverInit driverInit = VehicleCentral::ALLOW_DRIVER;
    ActionButton::SummonVehiclePhone::LoadVehicle( currentPreviewObject->name,
                                                   currentPreviewObject->filename,
                                                   driverInit );
}

void
CGuiScreenPhoneBooth::UpdateDamagedInfo()
{
    if( this->GetCurrentPreviewObject()->isUnlocked )
    {
        m_damagedInfo->SetVisible( true );

        HeapMgr()->PushHeap( GMA_LEVEL_HUD );

        Reward* pReward = this->GetCurrentPreviewObject()->pReward;
        rAssert( pReward != NULL );

        char buffer[ 16 ];

        int carIndex = GetCharacterSheetManager()->GetCarIndex( pReward->GetName() );
        if( carIndex != -1 )
        {
            float damagedAmount = 1.0f - GetCharacterSheetManager()->GetCarHealth( carIndex );
            if( damagedAmount >= 0.0f && damagedAmount <= 1.0f )
            {
                sprintf( buffer, "%.0f %%", damagedAmount * 100 );
                rAssert( m_vehicleDamaged != NULL );
                m_vehicleDamaged->SetString( 0, buffer );
            }
            else
            {
                rAssertMsg( false, "Why is the damage amount not between 0 and 1??" );
            }

            rAssert( m_repairCostInfo != NULL );
            m_repairCostInfo->SetVisible( damagedAmount == 1.0f );

            sprintf( buffer, "%d", pReward->GetRepairCost() );
            rAssert( m_vehicleRepairCost != NULL );
            m_vehicleRepairCost->SetString( 0, buffer );
        }
        else
        {
            rAssertMsg( false, "Why is the car index -1??" );
        }

        HeapMgr()->PopHeap( GMA_LEVEL_HUD );
    }
    else
    {
        m_damagedInfo->SetVisible( false );
    }
}

#ifdef SRR2_OVERRIDE_CAR_SELECTION

void
CGuiScreenPhoneBooth::HandleMessageForCar( eGuiMessage message, 
                                           unsigned int param1,
                                           unsigned int param2 )
{
    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        switch( message )
        {
            case GUI_MSG_MENU_SELECTION_MADE:
            {
                rAssert( param1 == 0 );

                if( m_menuTeleport )
                {
                    if( s_currentTeleportSelection >= 0 &&
                        s_currentTeleportSelection < CharacterManager::GetNumTeleportDests() )
                    {
                        CharacterManager::Teleport(s_currentTeleportSelection );

                        m_pParent->HandleMessage( GUI_MSG_BACK_SCREEN );
                    }
                    else
                    {
                        rAssertMsg( 0, "WARNING: *** Invalid teleport destination selection!" );
                    }
                }
                else
                {
                    if( s_currentDebugVehicleSelection >= 0 &&
                        s_currentDebugVehicleSelection < static_cast<int>( NUM_OVERRIDE_VEHICLES ) )
                    {
                        char vehicleFileName[ 32 ];
                        sprintf( vehicleFileName, "%s%s.p3d",
                                 VEHICLES_DIR,
                                 OVERRIDE_VEHICLE_NAMES[ s_currentDebugVehicleSelection ] );

                        ActionButton::SummonVehiclePhone::CarSelectionInfo* pInfo =
                            ActionButton::SummonVehiclePhone::GetDebugCarSelectInfo();

                        rAssert( pInfo );
	                    pInfo->AddDebugVehicleSelectionInfo( vehicleFileName,
                                                             OVERRIDE_VEHICLE_NAMES[ s_currentDebugVehicleSelection ],
                                                             "" );

                        this->Unload3DModel(); // unload 3D model first

    	                ActionButton::SummonVehiclePhone::LoadDebugVehicle();

                        m_state = GUI_WINDOW_STATE_IDLE;
                    }
                    else
                    {
                        rAssertMsg( 0, "WARNING: *** Invalid vehicle selection!" );
                    }
                }

                break;
            }
            case GUI_MSG_MENU_SELECTION_VALUE_CHANGED:
            {
                rAssert( param1 == 0 );

                HeapMgr()->PushHeap( GMA_LEVEL_HUD );

                Scrooby::Text* vehicleNames = dynamic_cast<Scrooby::Text*>( m_pMenu->GetMenuItem( 0 )->GetItemValue() );
                rAssert( vehicleNames != NULL );

                if( m_menuTeleport )
                {
                    s_currentTeleportSelection = static_cast<int>( param2 );

                    if( s_currentTeleportSelection == 99 )
                    {
                        s_currentTeleportSelection = CharacterManager::GetNumTeleportDests() - 1;
                        m_pMenu->SetSelectionValue( 0, s_currentTeleportSelection);
                    }

                    if( s_currentTeleportSelection >= CharacterManager::GetNumTeleportDests() )
                    {
                        s_currentTeleportSelection = 0;
                        m_pMenu->SetSelectionValue( 0, s_currentTeleportSelection);
                    }

                    vehicleNames->SetString( s_currentTeleportSelection, CharacterManager::GetTeleportDest(s_currentTeleportSelection) );
                }
                else
                {
                    s_currentDebugVehicleSelection = static_cast<int>( param2 );

                    if( param2 < NUM_OVERRIDE_VEHICLES )
                    {
                        char textBibleEntry[ 16 ];
                        strcpy( textBibleEntry, OVERRIDE_VEHICLE_NAMES[ s_currentDebugVehicleSelection ] );
                        UnicodeChar* textBibleString = GetTextBibleString( strupr( textBibleEntry ) );
                        UnicodeString unicodeString;
                        if( textBibleString != NULL )
                        {
                            unicodeString.ReadUnicode( textBibleString );
                        }
                        else
                        {
                            unicodeString.ReadAscii( OVERRIDE_VEHICLE_NAMES[ s_currentDebugVehicleSelection ] );
                        }

                        vehicleNames->SetString( s_currentDebugVehicleSelection, unicodeString );
                    }
                }

                HeapMgr()->PopHeap( GMA_LEVEL_HUD );

                break;
            }
            default:
            {
                break;
            }
        }

        if( m_pMenu != NULL )
        {
            m_pMenu->HandleMessage( message, param1, param2 );
        }
    }

    CGuiScreen::HandleMessage( message, param1, param2 );
}

#endif

