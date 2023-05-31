//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   IGuiScreenRewards
//
// Description: Implementation of the IGuiScreenRewards class.
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
#include <presentation/gui/ingame/guiscreenrewards.h>
#include <presentation/gui/guimenu.h>
#include <presentation/gui/guitextbible.h>

#include <memory/srrmemory.h>
#include <mission/gameplaymanager.h>
#include <mission/rewards/rewardsmanager.h>
#include <mission/rewards/reward.h>
#include <worldsim/coins/coinmanager.h>

#include <screen.h>
#include <page.h>
#include <sprite.h>
#include <text.h>
#include <pure3dobject.h>
#include <group.h>
#include <polygon.h>

#include <p3d/unicode.hpp>
#include <raddebug.hpp> // Foundation

#include <string.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

const char* REWARDS_INVENTORY_SECTION = "FE_RewardsScreen";

#ifdef RAD_XBOX
    const float PHONE_BOOTH_BGD_CORRECTION_SCALE = 8.0f;
#else
    const float PHONE_BOOTH_BGD_CORRECTION_SCALE = 8.8f;
#endif

const float LIGHT_OPEN_CLOSE_TRANSITION_TIME = 500.0f; // in msec

const char* PHONE_BOOTH_3DMODEL_CARS_DIR = "art\\frontend\\dynaload\\cars\\";
const char* PHONE_BOOTH_2DMODEL_CARS_DIR = "art\\frontend\\dynaload\\images\\cars2D\\";

//===========================================================================
// PhoneBoothStars::
//===========================================================================

PhoneBoothStars::PhoneBoothStars( Scrooby::Page* pPage, const char* namePrefix )
{
    rAssert( pPage );
    for( int i = 0; i < MAX_NUM_STARS; i++ )
    {
        char name[ 32 ];
        sprintf( name, "%s_Stars%d", namePrefix, i );
        m_stars[ i ] = pPage->GetSprite( name );
    }
}

void
PhoneBoothStars::SetNumStars( float numStars )
{
    rWarningMsg( numStars >= 0.0f && numStars <= static_cast<float>( MAX_NUM_STARS ),
                 "Number of stars exceeds boundaries!" );

    for( int i = 0; i < MAX_NUM_STARS; i++ )
    {
        bool isOn = (i < static_cast<int>( numStars ));

        // check if half on
        //
        bool isHalfOn = !isOn && (static_cast<float>( i ) < numStars);

        rAssert( m_stars[ i ] != NULL );
        if( isOn )
        {
            m_stars[ i ]->SetIndex( 2 );
//            m_stars[ i ]->SetAlpha( 1.0f );
        }
        else if( isHalfOn )
        {
            m_stars[ i ]->SetIndex( 1 );
//            m_stars[ i ]->SetAlpha( 1.0f );
        }
        else
        {
            m_stars[ i ]->SetIndex( 0 );
//            m_stars[ i ]->SetAlpha( 0.5f );
        }
    }
}


//===========================================================================
// IGuiScreenRewards::
//===========================================================================

IGuiScreenRewards::IGuiScreenRewards( Scrooby::Screen* pScreen,
                                      Scrooby::Page* pPage,
                                      CGuiEntity* pParent,
                                      eGuiWindowID windowID )
:   CGuiScreen( pScreen, pParent, windowID ),
    m_pRewardsMenu( NULL ),
    m_currentState( REWARDS_SCREEN_LIGHT_CLOSED ),
    m_elapsedTime( 0.0f ),
    m_previewLightCover( NULL ),
    m_previewWindow( NULL ),
    m_previewPedestal( NULL ),
    m_previewBgd( NULL ),
    m_previewImage( NULL ),
    m_previewName( NULL ),
    m_numPreviewVehicles( 0 ),
    m_currentPreviewVehicle( 0 ),
    m_numPreviewClothing( 0 ),
    m_currentPreviewClothing( 0 ),
    m_isLoading( false ),
    m_isLoadingReward( false ),
    m_lockedOverlay( NULL ),
    m_lockedLevel( NULL ),
    m_rewardPrice( NULL ),
    m_statsOverlay( NULL ),
    m_statsOverlayButton( NULL ),
    m_statsOverlayButtonLabel( NULL ),
    m_statsOverlayToggle( false )
{
    // Retrieve the Scrooby drawing elements.
    //
	rAssert( pPage != NULL );

    Scrooby::Sprite* previewLightL = pPage->GetSprite( "PreviewLightL" );
    Scrooby::Sprite* previewLightR = pPage->GetSprite( "PreviewLightR" );
    if( previewLightL != NULL && previewLightR != NULL )
    {
        previewLightL->ResetTransformation();
        previewLightR->ResetTransformation();

        // apply image correction scales
        //
        previewLightL->ScaleAboutCenter( PHONE_BOOTH_BGD_CORRECTION_SCALE );
        previewLightR->ScaleAboutCenter( PHONE_BOOTH_BGD_CORRECTION_SCALE );

        // flip the right light image horizontally to mirror the
        // left light image
        //
        previewLightR->RotateAboutCenter( 180.0f, rmt::Vector( 0.0f, 1.0f, 0.0f ) );
    }

    // get preview light cover (for opening/closing light)
    //
    m_previewLightCover = pPage->GetPolygon( "PreviewLightCover" );

    // get preview image
    //
    m_previewImage = pPage->GetSprite( "PreviewImage" );

    // create rewards menu w/ preview name
    //
    m_previewName = pPage->GetText( "PreviewName" );
    rAssert( m_previewName );
    m_previewName->SetTextMode( Scrooby::TEXT_WRAP );

    m_pRewardsMenu = new CGuiMenu( this, 1, GUI_TEXT_MENU, MENU_SFX_NONE );
    rAssert( m_pRewardsMenu != NULL );

    m_pRewardsMenu->AddMenuItem( m_previewName,
                                 pPage->GetText( "PreviewNameDummy" ),
                                 NULL,
                                 NULL,
                                 pPage->GetSprite( "LeftArrow" ),
                                 pPage->GetSprite( "RightArrow" ) );

    // get preview window and pedestal
    //
    m_previewWindow = pPage->GetPure3dObject( "PreviewWindow" );
    if( m_previewWindow != NULL )
    {
        m_previewWindow->SetVisible( false );
        m_previewWindow->SetClearDepthBuffer( true );
    }

    m_previewPedestal = pPage->GetPure3dObject( "RewardFG" );
    if ( m_previewPedestal != NULL )
    {
        m_previewPedestal->SetVisible( false );
    }

    m_previewBgd = pPage->GetPure3dObject( "RewardBG" );
    if( m_previewBgd != NULL )
    {
        m_previewBgd->SetVisible( false );
        m_previewBgd->SetClearDepthBuffer( true );
    }

    // get lock overlay
    //
    m_lockedOverlay = pPage->GetGroup( "Locked" );
    rAssert( m_lockedOverlay );

    // get locked level
    //
    m_lockedLevel = pPage->GetText( "Level" );

#ifdef RAD_E3
    if( m_lockedLevel != NULL )
    {
        m_lockedLevel->SetVisible( false );  // hide level display on E3 build
    }
#endif

    // get reward price
    //
    m_rewardPrice = m_lockedOverlay->GetText( "Price" );
    if( m_rewardPrice != NULL )
    {
        m_rewardPrice->SetDisplayOutline( true );

        Scrooby::Text* toPurchase = m_lockedOverlay->GetText( "ToPurchase" );
        if( toPurchase != NULL )
        {
            toPurchase->SetDisplayOutline( true );
        }
    }

    // get stats overlay and button label
    //
    m_statsOverlay = pPage->GetGroup( "VehicleStats" );
    m_statsOverlayButton = pPage->GetGroup( "ToggleView" );
    m_statsOverlayButtonLabel = pPage->GetText( "ToggleView" );

    // get vehicle star ratings
    //
    for( int i = 0; i < NUM_VEHICLE_RATINGS; i++ )
    {
        m_vehicleRatings[ i ] = NULL;
    }

    m_vehicleRatings[ VEHICLE_RATING_SPEED ] = new PhoneBoothStars( pPage, "Spe" );
    m_vehicleRatings[ VEHICLE_RATING_ACCELERATION ] = new PhoneBoothStars( pPage, "Acc" );
    m_vehicleRatings[ VEHICLE_RATING_TOUGHNESS ] = new PhoneBoothStars( pPage, "Tou" );
    m_vehicleRatings[ VEHICLE_RATING_STABILITY ] = new PhoneBoothStars( pPage, "Sta" );

    // create inventory section for loading rewards
    //
    p3d::inventory->AddSection( REWARDS_INVENTORY_SECTION );
}

IGuiScreenRewards::~IGuiScreenRewards()
{
    for( int i = 0; i < NUM_VEHICLE_RATINGS; i++ )
    {
        if( m_vehicleRatings[ i ] != NULL )
        {
            delete m_vehicleRatings[ i ];
            m_vehicleRatings[ i ] = NULL;
        }
    }

    if( m_pRewardsMenu != NULL )
    {
        delete m_pRewardsMenu;
        m_pRewardsMenu = NULL;
    }

    // destroy inventory section for loading rewards
    //
    p3d::pddi->DrawSync();

    p3d::inventory->DeleteSection( REWARDS_INVENTORY_SECTION );
}

void
IGuiScreenRewards::HandleMessage( eGuiMessage message,
                                  unsigned int param1,
                                  unsigned int param2 )
{
    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        switch( message )
        {
            case GUI_MSG_UPDATE:
            {
                this->OnUpdate( param1 );

                break;
            }
            case GUI_MSG_CONTROLLER_AUX_X:
            {
                // toggle vehicle stats overlay
                //
                if( m_statsOverlayButton != NULL && m_statsOverlayButton->IsVisible() )
                {
                    this->SetVehicleStatsVisible( !m_statsOverlayToggle );
                }

                break;
            }
            default:
            {
                break;
            }
        }

        rAssert( m_pRewardsMenu != NULL );
        m_pRewardsMenu->HandleMessage( message, param1, param2 );
    }

	// Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );
}

void
IGuiScreenRewards::OnProcessRequestsComplete( void* pUserData )
{
    m_isLoading = false; // reset loading flag

    const PreviewObject* currentPreviewObject = this->GetCurrentPreviewObject();
/*
    if( m_state != GUI_WINDOW_STATE_RUNNING )
    {
        // hmmm... the user must have backed out of this screen before
        // the preview object has been loaded; fine, we'll just unload it then
        //
        this->Unload3DModel();
    }
    else
*/
    {
        if( reinterpret_cast<PreviewObject*>( pUserData ) != currentPreviewObject )
        {
            // selection must have changed while loading, so let's try
            // to catch up
            //
            this->On3DModelSelectionChange( currentPreviewObject );
        }
        else
        {
            // push and select inventory section for searching
            //
            p3d::inventory->PushSection();
            p3d::inventory->SelectSection( REWARDS_INVENTORY_SECTION );
            bool currentSectionOnly = p3d::inventory->GetCurrentSectionOnly();
            p3d::inventory->SetCurrentSectionOnly( true );

            this->On3DModelLoaded( currentPreviewObject );

            // pop inventory section and restore states
            //
            p3d::inventory->SetCurrentSectionOnly( currentSectionOnly );
            p3d::inventory->PopSection();
/*
            // turn on/off lights depending on whether or not reward is unlocked
            //
            rAssert( m_previewBgd );
            tView* pView = m_previewBgd->GetView();
            if( pView != NULL )
            {
                for( unsigned int i = 0; i < MAX_VIEW_LIGHTS; i++ )
                {
                    tLight* pLight = pView->GetLight( i );
                    if( pLight != NULL )
                    {
                        pLight->Enable( currentPreviewObject->isUnlocked );
                    }
                    else
                    {
                        break;
                    }
                }
            }
*/
        }
    }
}

void
IGuiScreenRewards::InitIntro()
{
    m_numPreviewVehicles = 0;
    m_currentPreviewVehicle = 0;

    m_numPreviewClothing = 0;
    m_currentPreviewClothing = 0;

    // close light by default
    //
    this->SetLightOpening( 0.0f );
    m_currentState = REWARDS_SCREEN_LIGHT_CLOSED;

    // initialize menu w/ level-specific selections
    //
    this->InitMenu();

    GetCoinManager()->SetDrawAfterGui(true);

}

void
IGuiScreenRewards::InitRunning()
{
}

void
IGuiScreenRewards::InitOutro()
{
    m_isLoading = false;
    m_isLoadingReward = false;

    // unload current 3D vehicle
    //
    this->Unload3DModel();

    GetCoinManager()->SetDrawAfterGui(false);
}

int
IGuiScreenRewards::InsertPreviewObject( PreviewObject* previewObjects,
                                        int numPreviewObjects,
                                        Reward* pReward,
                                        bool isSorted )
{
    rAssert( previewObjects != NULL );
    rAssert( pReward != NULL );

    int newSlot = numPreviewObjects;

    if( isSorted )
    {
        // *** assumes array is already sorted ***

        // sort by unlock status first (unlocked rewards before locked rewards);
        // then sort unlocked rewards alphabetically, and sort locked rewards by level
        //
        for( int i = 0; i < numPreviewObjects; i++ )
        {
            bool insertHere = false;

            if( static_cast<int>( pReward->RewardStatus() ) > static_cast<int>( previewObjects[ i ].isUnlocked ) )
            {
                insertHere = true;
            }
            else if( pReward->RewardStatus() && previewObjects[ i ].isUnlocked )
            {
                char stringID[ 16 ];
                char string1[ 64 ];
                char string2[ 64 ];

                strcpy( stringID, pReward->GetName() );
                p3d::UnicodeToAscii( GetTextBibleString( strupr( stringID ) ), string1, sizeof( string1 ) );

                strcpy( stringID, previewObjects[ i ].name );
                p3d::UnicodeToAscii( GetTextBibleString( strupr( stringID ) ), string2, sizeof( string2 ) );

                insertHere = ( strcmp( string1, string2 ) < 0 );
            }
            else if( !pReward->RewardStatus() && !previewObjects[ i ].isUnlocked )
            {
                insertHere = ( pReward->GetLevel() < previewObjects[ i ].pReward->GetLevel() );
            }

            if( insertHere )
            {
                // OK, found it!
                //
                newSlot = i;

                // move all remaining preview objects up a slot to make room
                // for the new guy
                //
                for( int j = numPreviewObjects - 1; j >= i; j-- )
                {
                    strcpy( previewObjects[ j + 1 ].name, previewObjects[ j ].name );
                    strcpy( previewObjects[ j + 1 ].filename, previewObjects[ j ].filename );
                    strcpy( previewObjects[ j + 1 ].nameModel, previewObjects[ j ].nameModel );
                    strcpy( previewObjects[ j + 1 ].filenameModel, previewObjects[ j ].filenameModel );
                    previewObjects[ j + 1 ].isUnlocked = previewObjects[ j ].isUnlocked;
                    previewObjects[ j + 1 ].pReward = previewObjects[ j ].pReward;
                }

                // we're done
                //
                break;
            }
        }
    }

    // insert new object into the available slot
    //
    strncpy( previewObjects[ newSlot ].name, pReward->GetName(),
             sizeof( previewObjects[ newSlot ].name ) );

    strncpy( previewObjects[ newSlot ].filename, pReward->GetFile(),
             sizeof( previewObjects[ newSlot ].filename ) );

    if( m_ID == GUI_SCREEN_ID_PHONE_BOOTH )
    {
        // use 2D images for phone booth
        //
        strcpy( previewObjects[ newSlot ].nameModel, pReward->GetName() );

        sprintf( previewObjects[ newSlot ].filenameModel, "%s%s",
                 PHONE_BOOTH_2DMODEL_CARS_DIR,
                 pReward->GetName() );
    }
    else
    {
        if( pReward->GetRewardType() == Reward::ALT_PLAYERCAR )
        {
            sprintf( previewObjects[ newSlot ].nameModel, "%s",
                     pReward->GetName() );

            // 3D models for cars are not the same as the in-game ones
            // (they use less memory than the in-game ones)
            //
            sprintf( previewObjects[ newSlot ].filenameModel, "%s%s.p3d",
                     PHONE_BOOTH_3DMODEL_CARS_DIR,
                     pReward->GetName() );
        }
        else
        {
            sprintf( previewObjects[ newSlot ].nameModel, "%s_h",
                     pReward->GetName() );

            strncpy( previewObjects[ newSlot ].filenameModel, pReward->GetFile(),
                     sizeof( previewObjects[ newSlot ].filenameModel ) );
        }
    }

    previewObjects[ newSlot ].isUnlocked = pReward->RewardStatus();
    previewObjects[ newSlot ].pReward = pReward;

    // return the new number of preview objects
    //
    return (numPreviewObjects + 1);
}

void
IGuiScreenRewards::OnUpdate( unsigned int elapsedTime )
{
    switch( m_currentState )
    {
        case REWARDS_SCREEN_OPENING_LIGHT:
        {
            if( m_elapsedTime < LIGHT_OPEN_CLOSE_TRANSITION_TIME )
            {
                float lightOpening = m_elapsedTime / LIGHT_OPEN_CLOSE_TRANSITION_TIME;

                this->SetLightOpening( lightOpening );
            }
            else
            {
                // set light fully opened
                //
                this->SetLightOpening( 1.0f );

                m_currentState = REWARDS_SCREEN_LIGHT_OPENED;
            }

            break;
        }
        case REWARDS_SCREEN_LIGHT_OPENED:
        {
            // do nothing

            break;
        }
        case REWARDS_SCREEN_CLOSING_LIGHT:
        {
            if( m_elapsedTime < LIGHT_OPEN_CLOSE_TRANSITION_TIME )
            {
                float lightOpening = 1.0f - m_elapsedTime / LIGHT_OPEN_CLOSE_TRANSITION_TIME;

                this->SetLightOpening( lightOpening );
            }
            else
            {
                // set light fully closed
                //
                this->SetLightOpening( 0.0f );

                m_currentState = REWARDS_SCREEN_LIGHT_CLOSED;
            }

            break;
        }
        case REWARDS_SCREEN_LIGHT_CLOSED:
        {
            this->SetLightOpening( 0.0f );

            // check if loading is done yet
            //
            if( !m_isLoading )
            {
                // ok, re-open light
                //
                m_currentState = REWARDS_SCREEN_OPENING_LIGHT;
                m_elapsedTime = 0.0f;

                // show 3D model
                //
                if( m_previewWindow != NULL )
                {
                    m_previewWindow->SetVisible( true );
                }
            }

            break;
        }
        default:
        {
            break;
        }
    }

    m_elapsedTime += static_cast<float>( elapsedTime );
}

void
IGuiScreenRewards::SetLightOpening( float amount )
{
    if( m_previewLightCover != NULL )
    {
        const float MAX_LIGHT_AMOUNT_CLOSED = 0.85f;
        float amountClosed = (1.0f - amount) * MAX_LIGHT_AMOUNT_CLOSED;
        amount = 1.0f - amountClosed;

        m_previewLightCover->SetAlpha( amountClosed );
    }
}

void
IGuiScreenRewards::On3DModelSelectionChange( const PreviewObject* nextModel )
{
    if( nextModel != NULL )
    {
        // update preview object display name
        //
        HeapMgr()->PushHeap( GMA_LEVEL_HUD );

        char textBibleEntry[ sizeof( nextModel->name ) ];
        strcpy( textBibleEntry, nextModel->name );
        UnicodeChar* textBibleString = GetTextBibleString( strupr( textBibleEntry ) );
        UnicodeString unicodeString;
        if( textBibleString != NULL )
        {
            unicodeString.ReadUnicode( textBibleString );
        }
        else
        {
            rAssertMsg( false, "Skin/Vehicle name not found in text bible!" );
            unicodeString.ReadAscii( nextModel->name );
        }

#ifdef PAL
        if( m_ID == GUI_SCREEN_ID_PHONE_BOOTH &&
            CGuiTextBible::GetCurrentLanguage() == Scrooby::XL_GERMAN )
        {
            if( strcmp( textBibleEntry, "BOOKB_V" ) == 0 )
            {
                UnicodeString str1;
                str1.ReadAscii( "mobil" );

                UnicodeString str2;
                str2.ReadAscii( "-\nmobil" );

                unicodeString.Replace( str1, str2 );
            }

            if( strcmp( textBibleEntry, "SCORP_V") == 0 )
            {
                UnicodeString str1;
                str1.ReadAscii( "Auto" );

                UnicodeString str2;
                str2.ReadAscii( "\nAuto" );

                unicodeString.Replace( str1, str2 );
            }
        }
#endif // PAL

        rAssert( m_previewName );
        m_previewName->SetString( 0, unicodeString );

        HeapMgr()->PopHeap( GMA_LEVEL_HUD );

        // update locked/unlocked status
        //
        rAssert( m_lockedOverlay );
        m_lockedOverlay->SetVisible( !nextModel->isUnlocked );

        if( m_ID == GUI_SCREEN_ID_PHONE_BOOTH )
        {
            this->SetButtonVisible( BUTTON_ICON_ACCEPT, nextModel->isUnlocked );
        }

        // update locked level display
        //
        if( m_lockedLevel != NULL )
        {
            m_lockedLevel->SetIndex( nextModel->pReward->GetLevel() );
        }

        // update vehicle stats
        //
        this->UpdateVehicleStats();

        if( !m_isLoading )
        {
            // unload current 3D model
            //
            this->Unload3DModel();

            // load next 3D model
            //
            this->Load3DModel( nextModel );
        }
    }
}

void
IGuiScreenRewards::Load3DModel( const PreviewObject* previewObject )
{
    m_isLoading = true; // set loading flag
    m_isLoadingReward = false; // not loading reward

    rAssert( previewObject != NULL );

    if( m_ID == GUI_SCREEN_ID_PHONE_BOOTH )
    {
        char filename[ 64 ];

        // load normal 2D car image
        //
        sprintf( filename, "%s.p3d", previewObject->filenameModel );
        GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D,
                                         filename,
                                         GMA_LEVEL_OTHER,
                                         REWARDS_INVENTORY_SECTION,
                                         REWARDS_INVENTORY_SECTION );

#ifdef LOAD_DAMAGED_VEHICLE_IMAGES
        // load damaged 2D car image
        //
        sprintf( filename, "%sD.p3d", previewObject->filenameModel );
        GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D,
                                         filename,
                                         GMA_LEVEL_OTHER,
                                         REWARDS_INVENTORY_SECTION,
                                         REWARDS_INVENTORY_SECTION );
#endif
    }
    else
    {
        // add request to load 3D model
        //
        GetLoadingManager()->AddRequest( FILEHANDLER_PURE3D,
                                         previewObject->filenameModel,
                                         GMA_LEVEL_MISSION,
                                         REWARDS_INVENTORY_SECTION,
                                         REWARDS_INVENTORY_SECTION );
    }

    GetLoadingManager()->AddCallback( this, (void*)previewObject );
}

void
IGuiScreenRewards::Unload3DModel()
{
    p3d::pddi->DrawSync();

    if( m_previewWindow != NULL )
    {
        m_previewWindow->SetDrawable( NULL );
        m_previewWindow->SetVisible( false );
    }

    if( m_previewImage != NULL )
    {
        m_previewImage->SetRawSprite( NULL );
        m_previewImage->SetVisible( false );
    }

    // remove 3D model from inventory
    //
    p3d::inventory->RemoveSectionElements( REWARDS_INVENTORY_SECTION );

    // change current state to closing light, if not already closed
    // or in the progress of closing
    //
    if( m_currentState != REWARDS_SCREEN_CLOSING_LIGHT &&
        m_currentState != REWARDS_SCREEN_LIGHT_CLOSED )
    {
        if( m_currentState == REWARDS_SCREEN_OPENING_LIGHT &&
            m_elapsedTime < LIGHT_OPEN_CLOSE_TRANSITION_TIME )
        {
            m_elapsedTime = LIGHT_OPEN_CLOSE_TRANSITION_TIME - m_elapsedTime;
        }
        else
        {
            m_elapsedTime = 0.0f;
        }

        m_currentState = REWARDS_SCREEN_CLOSING_LIGHT;
    }
}

void
IGuiScreenRewards::SetVehicleStatsVisible( bool isVisible )
{
    m_statsOverlayToggle = isVisible;

    rAssert( m_statsOverlay );
    m_statsOverlay->SetVisible( m_statsOverlayToggle );

    rAssert( m_statsOverlayButtonLabel );
    m_statsOverlayButtonLabel->SetIndex( m_statsOverlayToggle ? 1 : 0 );
}

void
IGuiScreenRewards::UpdateVehicleStats()
{
    const PreviewObject* currentPreviewObject = this->GetCurrentPreviewObject();
    CarAttributeRecord* carAttributes = GetRewardsManager()->GetCarAttributeRecord( const_cast<char*>( currentPreviewObject->name ) );
    if( carAttributes != NULL )
    {
        m_vehicleRatings[ VEHICLE_RATING_SPEED ]->SetNumStars( carAttributes->mSpeed );
        m_vehicleRatings[ VEHICLE_RATING_ACCELERATION ]->SetNumStars( carAttributes->mAcceleration );
        m_vehicleRatings[ VEHICLE_RATING_TOUGHNESS ]->SetNumStars( carAttributes->mToughness );
        m_vehicleRatings[ VEHICLE_RATING_STABILITY ]->SetNumStars( carAttributes->mStability );
    }
/*
    else
    {
        rAssert( false );
        rTunePrintf( "*** WARNING: No car attributes found for [%s]!\n",
                     currentPreviewObject->name );
    }
*/
}

