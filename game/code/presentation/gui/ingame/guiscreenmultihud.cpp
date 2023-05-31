//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenMultiHud
//
// Description: Implementation of the CGuiScreenMultiHud class.
//
// Authors:     Darwin Chau
//              Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2000/11/20		DChau		Created
//                  2002/06/06      TChu        Modified for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <presentation/tutorialmanager.h>
#include <presentation/gui/ingame/guiscreenmultihud.h>
#include <presentation/gui/utility/hudmap.h>
#include <presentation/gui/utility/specialfx.h>
#include <presentation/gui/utility/transitions.h>
#include <presentation/gui/guitextbible.h>
#include <presentation/presentation.h>
#include <contexts/gameplay/gameplaycontext.h>
#include <input/inputmanager.h>
#include <gameflow/gameflow.h>
#include <main/commandlineoptions.h>
#include <memory/srrmemory.h>
#include <mission/gameplaymanager.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/character/character.h>
#include <worldsim/character/charactertarget.h>
#include <worldsim/redbrick/vehicle.h>

#include <raddebug.hpp> // Foundation

#include <group.h>
#include <screen.h>
#include <page.h>
#include <layer.h>
#include <polygon.h>
#include <sprite.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

#define SRR2_SHOW_SPEEDOMETER
#define WHITE_WASH_IN_TIME 1000.0f

const float ENTER_INGAME_FADE_IN_TIME = 500.0f; // in milliseconds
const float transitionIn = 250.0f;
const float minDisplayTime = 3000.0f;
const float maxDisplayTime = 50000.0f;
const float transitionOut = transitionIn;
Scrooby::Polygon*   g_WhiteWash = NULL;
GuiSFX::Show        g_WhiteWashShow( "WhiteWashShow" );
GuiSFX::ColorChange g_WhiteWashIn( "WhiteWashIn" );
GuiSFX::Pause       g_WhiteWashPause( "WhiteWashPause" );
GuiSFX::Hide        g_WhiteWashHide( "WhiteWashHide" );
GuiSFX::ResumeGame  g_WhiteWashResumeGame( "WhiteWashResumeGame" );
GuiSFX::PulseScale  g_TutorialPulse( "TutorialPulse" );
float               g_TimeSinceLastDeathVolume = 0.0f;
Vehicle*            g_Vehicle = NULL;

//===========================================================================
// Public Member Functions
//===========================================================================

class ResetCar : public GuiSFX::SendEvent
{
public:
    ResetCar();
    void Activate();
    void SetVehicle( Vehicle* vehicle );

protected:
    Vehicle* m_Vehicle;
};
ResetCar    g_ResetCar;

//===========================================================================
// CGuiScreenMultiHud::CGuiScreenMultiHud
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
CGuiScreenMultiHud::CGuiScreenMultiHud
(
    Scrooby::Screen* pScreen,
    CGuiEntity* pParent,
    eGuiWindowID id,
    int numActivePlayers
)
:
    CGuiScreen( pScreen, pParent, id ),
    m_numActivePlayers( numActivePlayers ),
    m_tutorialWalkie( NULL ),
    m_tutorialMessage( NULL ),
    m_isStartButtonPressed( false ),
    m_TutorialBitmapTimeShown( 0.0f )
{
    for( unsigned int i = 0; i < sizeof( m_hudMap ) /
                                 sizeof( m_hudMap[ 0 ] ); i++ )
    {
        m_hudMap[ i ] = NULL;
    }

    Scrooby::Page* pPage;
    pPage = m_pScroobyScreen->GetPage( "Hud" );
    rAssert( pPage );

    m_LetterboxPage = m_pScroobyScreen->GetPage( "LetterBox" );
    m_LetterboxPage->SetVisible( false );
    rAssert( m_LetterboxPage != NULL );

    m_tutorialWalkie     = pPage->GetGroup( "walkietalkie" );
    rAssert( m_tutorialWalkie != NULL );
    m_tutorialWalkie->SetVisible( false );

    Scrooby::Sprite* bartImage = m_tutorialWalkie->GetSprite( "walkietalkie" );
    if( bartImage != NULL )
    {
        bartImage->ResetTransformation();
        bartImage->ScaleAboutCenter( 0.75f );
    }

    m_tutorialMessage = m_tutorialWalkie->GetSprite( "TutorialMessage" );
    rAssert( m_tutorialMessage != NULL );
    m_tutorialMessage->ScaleAboutCenter( 0.4f );
    m_tutorialMessage->SetSpriteMode( Scrooby::SPRITE_BITMAP_TEXT );
    m_tutorialMessage->CreateBitmapTextBuffer( MAX_TUTORIAL_MESSAGE_LENGTH );

    pPage = m_pScroobyScreen->GetPage( "WhiteCover" );
    rAssert( pPage != NULL );

    Scrooby::Layer* whiteOverlay = pPage->GetLayer( "WhiteOverlay" );
    rAssert( whiteOverlay != NULL );
    whiteOverlay->SetVisible( true );

    g_WhiteWash = pPage->GetPolygon( "WhiteOverlay" );
    rAssert( g_WhiteWash != NULL );
    g_WhiteWash->SetVisible( false );

    g_TutorialPulse.SetDrawable( m_tutorialWalkie );
    g_TutorialPulse.SetAmplitude( 0.1f );
    g_TutorialPulse.SetFrequency( 2.5f );
    g_TutorialPulse.Activate();
    g_WhiteWashShow.SetDrawable( g_WhiteWash );
    g_WhiteWashShow.SetNextTransition( g_WhiteWashIn );
    g_WhiteWashIn.SetDrawable( g_WhiteWash );
    g_WhiteWashIn.SetTimeInterval( WHITE_WASH_IN_TIME );
    g_WhiteWashIn.SetStartColour( tColour( 255, 255, 255, 0 ) );
    g_WhiteWashIn.SetEndColour( tColour( 255, 255, 255, 255 ) );
    g_ResetCar.SetEvent( EVENT_DEATH_VOLUME_SCREEN_BLANK );
    g_ResetCar.SetEventData( NULL );
    


    g_WhiteWashIn.SetNextTransition( g_ResetCar );
    g_ResetCar.SetNextTransition( g_WhiteWashResumeGame );
    g_WhiteWashResumeGame.SetNextTransition( g_WhiteWashPause );
    g_WhiteWashPause.SetTimeInterval( 250.0f );
    g_WhiteWashPause.SetNextTransition( g_WhiteWashHide );
    g_WhiteWashHide.SetDrawable( g_WhiteWash );
    g_WhiteWashHide.SetNextTransition( NULL );

    int xMin;
    int xMax;
    int yMin;
    int yMax;
    m_tutorialWalkie->GetBoundingBox( xMin, yMin, xMax, yMax );
    int height = yMax - yMin;

    //
    // Set up transitions for the tutorial bitmap
    //
    m_TutorialBitmapTransitionIn.SetDrawable( m_tutorialWalkie );
    m_TutorialBitmapTransitionIn.SetFrequency( 5.0f );
    m_TutorialBitmapTransitionIn.SetTimeInterval( transitionIn * 2.0f );
    m_TutorialBitmapTransitionIn.SetStartOffscreenTop( m_tutorialWalkie );
    m_TutorialBitmapTransitionIn.SetCoordsEnd( 0, 0 );

    m_TutorialBitmapSteadyState.SetDrawable( m_tutorialWalkie );
    m_TutorialBitmapSteadyState.SetTimeInterval( maxDisplayTime );
    m_TutorialBitmapSteadyState.SetCoordsStart( 0, 0 );
    m_TutorialBitmapSteadyState.SetCoordsEnd( 0, 0 );

    m_TutorialBitmapTransitionOut.SetDrawable( m_tutorialWalkie );
    m_TutorialBitmapTransitionOut.SetTimeInterval( transitionOut );
    m_TutorialBitmapTransitionOut.SetCoordsStart( 0, 0 );
    m_TutorialBitmapTransitionOut.SetEndOffscreenTop( m_tutorialWalkie );

    m_TurorialBitmapStayOut.SetDrawable( m_tutorialWalkie );
    m_TurorialBitmapStayOut.SetTimeInterval( FLT_MAX );
    m_TurorialBitmapStayOut.SetCoordsStart( 0, -height * 2 - yMin );
    m_TurorialBitmapStayOut.SetCoordsEnd( 0, -height * 2 - yMin );

    //
    // Chain the transitions together
    //
    m_TutorialBitmapTransitionIn.SetNextTransition( m_TutorialBitmapSteadyState );
    m_TutorialBitmapSteadyState.SetNextTransition( m_TutorialBitmapTransitionOut );
    m_TutorialBitmapTransitionOut.SetNextTransition( m_TurorialBitmapStayOut );

    // override default fade time
    this->SetFadeTime( ENTER_INGAME_FADE_IN_TIME );
}

CGuiScreenMultiHud::CGuiScreenMultiHud
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:	
	CGuiScreen( pScreen, pParent, GUI_SCREEN_ID_MULTI_HUD ),
    m_numActivePlayers( MAX_PLAYERS )
{
    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage;
	pPage = m_pScroobyScreen->GetPage( "MultiHud" );
	rAssert( pPage );

    // Get dynamic elements
    this->RetrieveElements( pPage );
}


//===========================================================================
// CGuiScreenMultiHud::~CGuiScreenMultiHud
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
CGuiScreenMultiHud::~CGuiScreenMultiHud()
{
    for( unsigned int i = 0; i < sizeof( m_hudMap ) /
                                 sizeof( m_hudMap[ 0 ] ); i++ )
    {
        if( m_hudMap[ i ] != NULL )
        {
            delete m_hudMap[ i ];
            m_hudMap[ i ] = NULL;
        }
    }
}


//===========================================================================
// CGuiScreenMultiHud::HandleMessage
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
void CGuiScreenMultiHud::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2 
)
{
    switch( message )
    {
        case GUI_MSG_UPDATE:
        {
            // update HUD elements for all players
            this->UpdateElements( param1 );

            if( m_isStartButtonPressed )
            {
                m_isStartButtonPressed = false;

                if( m_state == GUI_WINDOW_STATE_RUNNING )
                {
                    m_pParent->HandleMessage( GUI_MSG_PAUSE_INGAME );
                }
            }

            break;
        }

        case GUI_MSG_CONTROLLER_START:
        {
            // queue this up to be processed in the next update frame
            //
            m_isStartButtonPressed = true;

            break;
        }

#ifdef RAD_WIN32
        case GUI_MSG_CONTROLLER_BACK:
        {
            // This is our start button for PC.
            if( GetInputManager()->GetValue( 0, InputManager::KeyboardEsc ) > 0.0f )
            {
                m_isStartButtonPressed = true;
            }
            break;
        }
#endif

        case GUI_MSG_DEATH_VOLUME_START:
        {
            //
            // Need to start the transition for the death volume
            //
            if( g_TimeSinceLastDeathVolume > WHITE_WASH_IN_TIME + 1000.0 && GetGameFlow()->GetCurrentContext() != CONTEXT_PAUSE && GetGameFlow()->GetNextContext() != CONTEXT_PAUSE)
            {
                g_ResetCar.SetEventData( reinterpret_cast< void* >( param1 ) );
                GameplayContext::GetInstance()->PauseAllButPresentation( true );
                g_WhiteWashShow.Reset();
                g_WhiteWashIn.Reset();
                g_WhiteWashHide.Reset();
                g_WhiteWashPause.Reset();
                g_WhiteWashShow.Activate();
                g_TimeSinceLastDeathVolume = 0.0f;
            }
            break;
        }

        case GUI_MSG_MANUAL_RESET:
        {
            //
            // Need to start the transition for the death volume
            //
            if( g_TimeSinceLastDeathVolume > WHITE_WASH_IN_TIME + 1000.0 && GetGameFlow()->GetCurrentContext() != CONTEXT_PAUSE && GetGameFlow()->GetNextContext() != CONTEXT_PAUSE)
            {
                g_ResetCar.SetVehicle( reinterpret_cast< Vehicle* >( param1 ) );
                g_ResetCar.SetEventData( NULL );
                GameplayContext::GetInstance()->PauseAllButPresentation( true );
                g_WhiteWashShow.Reset();
                g_WhiteWashIn.Reset();
                g_WhiteWashHide.Reset();
                g_WhiteWashPause.Reset();
                g_WhiteWashShow.Activate();
                g_TimeSinceLastDeathVolume = 0.0f;
            }
            break;
        }

        default:
        {
            break;
        }
    }

    // Propogate the message up the hierarchy.
	//
	CGuiScreen::HandleMessage( message, param1, param2 );
}


//===========================================================================
// CGuiScreenMultiHud::GetHudMap
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
CHudMap*
CGuiScreenMultiHud::GetHudMap( unsigned int playerID ) const
{
    rAssert( playerID < static_cast<unsigned int>( m_numActivePlayers ) );

    return m_hudMap[ playerID ];
}


//===========================================================================
// CGuiScreenMultiHud::InitIntro
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
void CGuiScreenMultiHud::InitIntro()
{
    for( int i = 0; i < m_numActivePlayers; i++ )
    {
        Avatar* pAvatar = GetAvatarManager()->GetAvatarForPlayer( i );
        rAssert( pAvatar );

        rAssert( m_hudMap[ i ] );
        m_hudMap[ i ]->SetCameraTarget( pAvatar->GetCharacter()->GetTarget() );
        m_hudMap[ i ]->Reset();
/*
        m_hudMap[ i ]->RegisterIcon( HudMapIcon::ICON_PLAYER,
                                     rmt::Vector( 0, 0, 0 ),
                                     pAvatar->GetCharacter()->GetTarget() );
*/
    }
}


//===========================================================================
// CGuiScreenMultiHud::InitRunning
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
void CGuiScreenMultiHud::InitRunning()
{
}


//===========================================================================
// CGuiScreenMultiHud::InitOutro
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
void CGuiScreenMultiHud::InitOutro()
{
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

void
CGuiScreenMultiHud::RetrieveElements( Scrooby::Page* pPage )
{
MEMTRACK_PUSH_GROUP( "CGUIScreenMultiHud" );
    char objectName[ 32 ];
    for( int i = 0; i < m_numActivePlayers; i++ )
    {
#ifdef SRR2_SHOW_SPEEDOMETER
        sprintf( objectName, "Speed%d", i );
        Scrooby::Group* speedGroup = pPage->GetGroup( objectName );

        rAssert( speedGroup != NULL );
        m_speedDigital[ i ].SetScroobyText( speedGroup, objectName );
//        m_speedDigital[ i ].SetColour( tColour( 255, 255, 255 ) );

        // speedometer is turned on w/ commandline option "speedometer"
        //
        m_speedDigital[ i ].SetVisible( CommandLineOptions::Get( CLO_SHOW_SPEED ) );

    // hide speed units in final build
    #ifdef FINAL
        sprintf( objectName, "SpeedUnit%d", i );
        Scrooby::Text* speedUnit = speedGroup->GetText( objectName );
        if( speedUnit != NULL )
        {
            speedUnit->SetVisible( false );
        }
    #endif
#endif

        char p3dFile[ 16 ];
        sprintf( p3dFile, "l%dhudmap", GetGameplayManager()->GetCurrentLevelIndex() + 1 );
        m_hudMap[ i ] = new CHudMap( pPage, i, p3dFile );
        rAssert( m_hudMap[ i ] != NULL );

        // Get Hud Map Icons
        //
        Scrooby::Group* playerCars      = pPage->GetGroup( "PlayerCars" );
        Scrooby::Group* aiCarsGroup     = pPage->GetGroup( "AICars" );
        Scrooby::Group* aiHarassCars    = pPage->GetGroup( "AIHarassCars" );
        Scrooby::Group* waypointFlags   = pPage->GetGroup( "WaypointFlags" );
        Scrooby::Group* checkeredFlags  = pPage->GetGroup( "CheckeredFlags" );
        Scrooby::Group* collectables    = pPage->GetGroup( "Collectibles" );
        Scrooby::Group* icons0          = pPage->GetGroup( "Icons0" );
        Scrooby::Group* missions        = pPage->GetGroup( "Missions" );
        Scrooby::Group* bonusMissions   = pPage->GetGroup( "BonusMissions" );
        Scrooby::Group* streetRaces     = pPage->GetGroup( "StreetRaces" );
        Scrooby::Group* wagerRaces      = pPage->GetGroup( "WagerRaces" );
        Scrooby::Group* purchaseCentres = pPage->GetGroup( "PurchaseCentres" );
        Scrooby::Group* phoneBooths     = pPage->GetGroup( "PhoneBooths" );
        rAssert( playerCars      != NULL );
        rAssert( aiCarsGroup     != NULL );
        rAssert( aiHarassCars    != NULL );
        rAssert( waypointFlags   != NULL );
        rAssert( checkeredFlags  != NULL );
        rAssert( collectables    != NULL );
        rAssert( icons0          != NULL );
        rAssert( missions        != NULL );
        rAssert( bonusMissions   != NULL );
        rAssert( streetRaces     != NULL );
        rAssert( wagerRaces      != NULL );
        rAssert( purchaseCentres != NULL );
        rAssert( phoneBooths     != NULL );

        sprintf( objectName, "IconPlayer%d", i );
        m_hudMap[ i ]->AddIconToInventory( HudMapIcon::ICON_PLAYER, icons0->GetSprite( objectName ) );

        for( unsigned int j = 0; j < MAX_NUM_ICONS_PER_TYPE; j++ )
        {
            sprintf( objectName, "IconMission%d_%d", i, j );
            m_hudMap[ i ]->AddIconToInventory( HudMapIcon::ICON_MISSION, missions->GetSprite( objectName ) );

            sprintf( objectName, "IconCollectible%d_%d", i, j );
            m_hudMap[ i ]->AddIconToInventory( HudMapIcon::ICON_COLLECTIBLE, collectables->GetSprite( objectName ) );

            sprintf( objectName, "IconPlayerCar%d_%d", i, j );
            m_hudMap[ i ]->AddIconToInventory( HudMapIcon::ICON_PLAYER_CAR, playerCars->GetSprite( objectName ) );

            sprintf( objectName, "IconAICar%d_%d", i, j );
            Scrooby::Sprite* aiCarIcon = aiCarsGroup->GetSprite( objectName );
            m_hudMap[ i ]->AddIconToInventory( HudMapIcon::ICON_AI_CHASE, aiCarIcon );
            m_hudMap[ i ]->AddIconToInventory( HudMapIcon::ICON_AI_RACE, aiCarIcon );
            m_hudMap[ i ]->AddIconToInventory( HudMapIcon::ICON_AI_EVADE, aiCarIcon );
            m_hudMap[ i ]->AddIconToInventory( HudMapIcon::ICON_AI_TARGET, aiCarIcon );

            sprintf( objectName, "IconHarass%d_%d", i, j );
            m_hudMap[ i ]->AddIconToInventory( HudMapIcon::ICON_AI_HIT_N_RUN, aiHarassCars->GetSprite( objectName ) );

            sprintf( objectName, "IconWaypoint%d_%d", i, j );
            m_hudMap[ i ]->AddIconToInventory( HudMapIcon::ICON_FLAG_WAYPOINT, waypointFlags->GetSprite( objectName ) );

            sprintf( objectName, "IconChecker%d_%d", i, j );
            m_hudMap[ i ]->AddIconToInventory( HudMapIcon::ICON_FLAG_CHECKERED, checkeredFlags->GetSprite( objectName ) );

            sprintf( objectName, "BonusMission%d_%d", i, j );
            m_hudMap[ i ]->AddIconToInventory( HudMapIcon::ICON_BONUS_MISSION, bonusMissions->GetSprite( objectName ) );

            sprintf( objectName, "StreetRace%d_%d", i, j );
            m_hudMap[ i ]->AddIconToInventory( HudMapIcon::ICON_STREET_RACE, streetRaces->GetSprite( objectName ) );

            sprintf( objectName, "WagerRace%d_%d", i, j );
            m_hudMap[ i ]->AddIconToInventory( HudMapIcon::ICON_WAGER_RACE, wagerRaces->GetSprite( objectName ) );

            sprintf( objectName, "PhoneBooth%d_%d", i, j );
            m_hudMap[ i ]->AddIconToInventory( HudMapIcon::ICON_PHONE_BOOTH, phoneBooths->GetSprite( objectName ) );

            sprintf( objectName, "PurchaseCentre%d_%d", i, j );
            m_hudMap[ i ]->AddIconToInventory( HudMapIcon::ICON_PURCHASE_CENTRE, purchaseCentres->GetSprite( objectName ) );
        }

        m_hudMap[ i ]->RestoreAllRegisteredIcons();
    }
MEMTRACK_POP_GROUP("CGUIScreenMultiHud");
}

void
CGuiScreenMultiHud::UpdateElements( unsigned int elapsedTime )
{
    float elapsedTimeFloat = static_cast< float >( elapsedTime );
    g_TimeSinceLastDeathVolume += elapsedTimeFloat;
    g_WhiteWashIn.Update( elapsedTimeFloat );
    g_WhiteWashPause.Update( elapsedTimeFloat );
    g_TutorialPulse.Update( elapsedTimeFloat );

    for( int i = 0; i < m_numActivePlayers; i++ )
    {
#ifdef SRR2_SHOW_SPEEDOMETER
        // update speedometer
        //
        if( CommandLineOptions::Get( CLO_SHOW_SPEED ) ||
            GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_SHOW_SPEEDOMETER ) )
        {
            unsigned int speed = 0;

            Avatar* pAvatar = GetAvatarManager()->GetAvatarForPlayer( i );
            rAssert( pAvatar );
            if( pAvatar->IsInCar() )
            {
                Vehicle* vehicle = pAvatar->GetVehicle();
                rAssert( vehicle );

                speed = (unsigned int)vehicle->GetSpeedKmh();
            }
            else
            {
                Character* pCharacter = pAvatar->GetCharacter();
                rAssert( pCharacter );

                speed = (unsigned int)pCharacter->GetSpeed();
            }

            m_speedDigital[ i ].SetVisible( true );
            m_speedDigital[ i ].SetValue( speed );
        }
        else
        {
            m_speedDigital[ i ].SetVisible( false );
        }
#endif // SRR2_SHOW_SPEEDOMETER

        // update hud map
        //
        rAssert( m_hudMap[ i ] );
        m_hudMap[ i ]->Update( elapsedTime );
    }
}

//===========================================================================
// CGuiScreenHud::TutorialBitmapHide
//===========================================================================
// Description: hides the tutorial mode bitmap
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMultiHud::TutorialBitmapHide()
{
    m_tutorialWalkie->SetVisible( false );
    m_TutorialBitmapVisible = false;
}

//===========================================================================
// CGuiScreenHud::TutorialBitmapInitOutro
//===========================================================================
// Description: starts the outro transition of the tutorial bitmap
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMultiHud::TutorialBitmapInitOutro()
{
    m_TutorialBitmapTransitionIn.Deactivate();
    m_TutorialBitmapSteadyState.Deactivate();
    m_TutorialBitmapTransitionOut.Activate();
    m_TurorialBitmapStayOut.Deactivate();
    m_TutorialBitmapTimeShown = maxDisplayTime;
}
//===========================================================================
// CGuiScreenHud::TutorialBitmapShow
//===========================================================================
// Description: shows the tutorial mode bitmap.
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMultiHud::TutorialBitmapShow()
{
    m_tutorialWalkie->SetVisible( true );
    m_TutorialBitmapVisible = true;
    m_TutorialBitmapTimeShown = 0.0f;
    m_TutorialBitmapTransitionIn.Reset();
    m_TutorialBitmapTransitionOut.Reset();
    m_TutorialBitmapSteadyState.Reset();
    m_TutorialBitmapTransitionIn.Activate();
    m_TutorialBitmapTransitionOut.Deactivate();
    m_TutorialBitmapSteadyState.Deactivate();
    m_TurorialBitmapStayOut.Deactivate();
}

//===========================================================================
// CGuiScreenHud::TutorialBitmapHide
//===========================================================================
// Description: hides the tutorial mode bitmap
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMultiHud::UpdateTutorialMode( const float deltaT )
{
    m_tutorialWalkie->ResetTransformation();
    m_TutorialBitmapTransitionIn.Update( deltaT );
    m_TutorialBitmapSteadyState.Update( deltaT );
    if( m_TutorialBitmapTransitionOut.IsActive() )
    {
        m_TutorialBitmapTransitionOut.Update( deltaT );
        if( m_TutorialBitmapTransitionOut.IsDone() )
        {
            this->TutorialBitmapHide();
        }
    }
    m_TurorialBitmapStayOut.Update( deltaT );
}

//===========================================================================
// CGuiScreenMultiHud::SetTutorialMessage
//===========================================================================
// Description: hides the tutorial mode bitmap
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMultiHud::SetTutorialMessage( int index )
{
    char textBibleID[ 32 ];
    sprintf( textBibleID, "TUTORIAL_%03d", index );

    rAssert( m_tutorialMessage != NULL );
    m_tutorialMessage->SetBitmapText( GetTextBibleString( textBibleID ) );
}

//===========================================================================
// CGuiScreenMultiHud::ShowLetterBox
//===========================================================================
// Description: shows the letterbox
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenMultiHud::ShowLetterBox()
{
    m_LetterboxPage->SetVisible( true );
}

//===========================================================================
// ResetCar::ResetCar
//===========================================================================
// Description: constructor for the reset car transition
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
ResetCar::ResetCar()
{
}

//===========================================================================
// ResetCar::Activate
//===========================================================================
// Description: Activate - does the work of the reset car trasntition
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void ResetCar::Activate()
{
    if( this->mEventData == NULL )
    {
        bool manualDamageReset = CommandLineOptions::Get( CLO_MANUAL_RESET_DAMAGE );
        rAssert( m_Vehicle != NULL );
        if( m_Vehicle != NULL )
        {
            m_Vehicle->ResetOnSpot( manualDamageReset );
            m_Vehicle = NULL;
        }
    }
    else
    {
        SendEvent::Activate();
    }
    ContinueChain();
}

//===========================================================================
// ResetCar::SetVehicle
//===========================================================================
// Description: sets the vehicle that's going to get reset
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void ResetCar::SetVehicle( Vehicle* vehicle )
{
    m_Vehicle = vehicle;
}
