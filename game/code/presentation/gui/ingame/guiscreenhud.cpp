//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreenHud
//
// Description: Implementation of the CGuiScreenHud class.
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
#include <presentation/gui/ingame/guiscreenhud.h>
#include <presentation/gui/ingame/guihudtextbox.h>
#include <presentation/gui/ingame/hudevents/hudcardcollected.h>
#include <presentation/gui/ingame/hudevents/hudcoincollected.h>
#include <presentation/gui/ingame/hudevents/hudmissionprogress.h>
#include <presentation/gui/ingame/hudevents/hudmissionobjective.h>
#include <presentation/gui/ingame/hudevents/hudcountdown.h>
#include <presentation/gui/ingame/hudevents/hudhitnrun.h>
#include <presentation/gui/ingame/hudevents/hudwaspdestroyed.h>
#include <presentation/gui/ingame/hudevents/huditemdropped.h>
#include <presentation/gui/utility/hudmap.h>
#include <presentation/gui/utility/scrollingtext.h>
#include <presentation/gui/utility/specialfx.h>
#include <presentation/gui/guitextbible.h>
#include <presentation/gui/guisystem.h>

#ifdef SRR2_MOVABLE_HUD_MAP
    #include <presentation/gui/guisystem.h>
    #include <presentation/gui/guiuserinputhandler.h>
#endif

#include <cards/card.h>
#include <contexts/context.h>
#include <contexts/gameplay/gameplaycontext.h>
#include <events/eventmanager.h>
#include <gameflow/gameflow.h>
#include <interiors/interiormanager.h>
#include <mission/gameplaymanager.h>
#include <mission/mission.h>
#include <mission/missionmanager.h>
#include <mission/conditions/getoutofcarcondition.h>
#include <memory/srrmemory.h>
#include <worldsim/hitnrunmanager.h>
#include <worldsim/coins/coinmanager.h>

#include <p3d/unicode.hpp>

// FTech
//
#include <raddebug.hpp>

// Scrooby
//
#include <app.h>
#include <group.h>
#include <layer.h>
#include <page.h>
#include <pure3dobject.h>
#include <screen.h>
#include <sprite.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

// radar fade in/out transition time
//
const unsigned int RADAR_FADE_TIME = 250; // in msec

const float RADAR_SCALE = 0.9f;

//const tColour HNR_METER_EMPTY_COLOUR( 0x02, 0x45, 0x39 );
const short NUMERIC_TEXT_SPACING = -12;

const float HNR_METER_BLINKING_THRESHOLD = 0.78f;
const tColour HNR_METER_DEFAULT_COLOUR( 255, 234, 2 );
const tColour HNR_METER_WARNING_COLOUR( 215, 16, 12 );

const tColour PROXIMITY_METER_RED( 255, 0, 0 );
const tColour PROXIMITY_METER_GREEN( 0, 255, 0 );

// minimum time required for avatar to be off road before updating
// race position to a ?/N display
//
const unsigned int MIN_AVATAR_OFF_ROAD_TIME = 0; // in msec

const unsigned int MESSAGE_DISPLAY_TIME = 3000; // in msec
const unsigned int MESSAGE_TRANSITION_TIME = 200; // in msec

Scrooby::Sprite* CGuiScreenHud::s_numCoinsDisplay = NULL;

//===========================================================================
// Public Member Functions
//===========================================================================

MessageQueue::MessageQueue()
:   m_front( 0 ),
    m_back( 0 ),
    m_isEmpty( true ),
    m_isFull( false )
{
}

MessageQueue::~MessageQueue()
{
}

void
MessageQueue::Enqueue( unsigned int messageID )
{
    rAssertMsg( !m_isFull, "Queue is full!" );

    m_messages[ m_back ] = messageID;
    m_back = (m_back + 1) % MESSAGE_QUEUE_CAPACITY;

    m_isEmpty = false;
    m_isFull = (m_front == m_back);
}

unsigned int
MessageQueue::Dequeue()
{
    // assumes queue is not empty
    //
    rAssertMsg( !m_isEmpty, "Queue is empty!" );

    unsigned int messageID = m_messages[ m_front ];
    m_front = (m_front + 1) % MESSAGE_QUEUE_CAPACITY;

    m_isEmpty = (m_front == m_back);
    m_isFull = false;

    return messageID;
}

void
MessageQueue::Clear()
{
    m_front = 0;
    m_back = 0;
    m_isEmpty = true;
    m_isFull= false;
}

//===========================================================================
// CGuiScreenHud::CGuiScreenHud
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
CGuiScreenHud::CGuiScreenHud
(
	Scrooby::Screen* pScreen,
	CGuiEntity* pParent
)
:   CGuiScreenMultiHud( pScreen, pParent, GUI_SCREEN_ID_HUD, 1 ),
    m_missionOverlays( NULL ),
    m_helpMessage( NULL ),
    m_messageBox( NULL ),
    m_actionButton( NULL ),
    m_missionComplete( NULL ),
    m_timer( NULL ),
    m_defaultTimerColour( 255, 255, 255 ),
    m_timerBlinkingStartTime( 10000 ),
    m_timerBlinkingEndTime( 0 ),
    m_parTime( NULL ),
    m_collectibles( NULL ),
    m_collectiblesUpdated( 0 ),
    m_position( NULL ),
    m_positionOrdinal( NULL ),
    m_lap( NULL ),
    m_lapUpdated( 0 ),
    m_isSlidingDamageMeter( false ),
    m_isFadingInRadar( false ),
    m_isFadingOutRadar( false ),
    m_hnrLights( NULL ),
    m_hnrMeter( NULL ),
    m_hnrMeterBgd( NULL ),
    m_hnrElapsedTime( 0 ),
    m_isFadingIn( false ),
    m_isFadingOut( false ),
    m_elapsedFadeTime( 0.0f ),
    m_isBonusMissionJustCompleted( false ),
    m_isAvatarOffRoad( false ),
    m_avatarOffRoadDurationTime( 0 ),
    m_pGetOutOfCarCondition( NULL ),
    m_elapsedFgdFadeInTime( -1.0f )
{
MEMTRACK_PUSH_GROUP( "CGUIScreenHud" );
    memset( m_overlays, 0, sizeof( m_overlays ) );
    memset( m_hudEventHandlers, 0, sizeof( m_hudEventHandlers ) );
    memset( m_hnrSectors, 0, sizeof( m_hnrSectors ) );

    int i = 0;
/*
    bool enableFadeIn = (GetGameFlow()->GetCurrentContext() == CONTEXT_LOADING_GAMEPLAY );
    this->SetFadingEnabled( enableFadeIn );
*/
    this->SetFadingEnabled( false );

    // Retrieve the Scrooby drawing elements.
    //
    Scrooby::Page* pPage = m_pScroobyScreen->GetPage( "Hud" );
	rAssert( pPage != NULL );

    // Get dynamic elements
    this->RetrieveElements( pPage );

    m_missionOverlays = pPage->GetGroup( "MissionOverlays" );
    rAssert( m_missionOverlays != NULL );

    Scrooby::Group* pGroup = NULL;

    // Retrieve HUD overlays
    //
    pGroup = m_missionOverlays->GetGroup( "Timer" );
    rAssert( pGroup != NULL );
    m_overlays[ HUD_TIMER ] = pGroup;
    m_overlays[ HUD_TIMER_TEMP ] = pGroup;
    m_timer = pGroup->GetSprite( "Timer" );
    rAssert( m_timer != NULL );
    m_timer->SetSpriteMode( Scrooby::SPRITE_BITMAP_TEXT );
    m_timer->CreateBitmapTextBuffer( BITMAP_TEXT_BUFFER_SIZE );
    m_timer->SetBitmapTextSpacing( NUMERIC_TEXT_SPACING );
    m_defaultTimerColour = m_timer->GetColour();
#ifdef RAD_WIN32
    m_timer->Translate( -25, 0 );
    m_timer->ScaleAboutCenter( 0.5f );
#endif

    pGroup = m_missionOverlays->GetGroup( "ParTime" );
    rAssert( pGroup != NULL );
    m_overlays[ HUD_PAR_TIME ] = pGroup;
    m_parTime = pGroup->GetSprite( "ParTime" );
    rAssert( m_parTime != NULL );
    m_parTime->SetSpriteMode( Scrooby::SPRITE_BITMAP_TEXT );
    m_parTime->CreateBitmapTextBuffer( BITMAP_TEXT_BUFFER_SIZE );
    m_parTime->SetBitmapTextSpacing( NUMERIC_TEXT_SPACING );
#ifdef RAD_WIN32
    m_parTime->Translate( -25, 0 );
    m_parTime->ScaleAboutCenter( 0.5f );
#endif

    pGroup = m_missionOverlays->GetGroup( "Collectibles" );
    rAssert( pGroup != NULL );
    m_overlays[ HUD_COLLECTIBLES ] = pGroup;
    m_collectibles = pGroup->GetSprite( "Collectibles" );
    rAssert( m_collectibles != NULL );
    m_collectibles->SetSpriteMode( Scrooby::SPRITE_BITMAP_TEXT );
    m_collectibles->CreateBitmapTextBuffer( BITMAP_TEXT_BUFFER_SIZE );
    m_collectibles->SetBitmapTextSpacing( NUMERIC_TEXT_SPACING );
#ifdef RAD_WIN32
    m_collectibles->Translate( -25, 0 );
    m_collectibles->ScaleAboutCenter( 0.5f );
#endif

    pGroup = m_missionOverlays->GetGroup( "Position" );
    rAssert( pGroup != NULL );
    m_overlays[ HUD_RACE_POSITION ] = pGroup;
    m_positionOrdinal = pGroup->GetText( "PositionOrdinal" );
    m_position = pGroup->GetSprite( "Position" );
    rAssert( m_position != NULL );
    m_position->SetSpriteMode( Scrooby::SPRITE_BITMAP_TEXT );
    m_position->CreateBitmapTextBuffer( BITMAP_TEXT_BUFFER_SIZE );
    m_position->SetBitmapTextSpacing( NUMERIC_TEXT_SPACING );
#ifdef RAD_WIN32
    m_position->Translate( -25, 0 );
    m_position->ScaleAboutCenter( 0.5f );
#endif

    pGroup = m_missionOverlays->GetGroup( "Lap" );
    rAssert( pGroup != NULL );
    m_overlays[ HUD_LAP_COUNTER ] = pGroup;
    m_lap = pGroup->GetSprite( "Lap" );
    rAssert( m_lap != NULL );
    m_lap->SetSpriteMode( Scrooby::SPRITE_BITMAP_TEXT );
    m_lap->CreateBitmapTextBuffer( BITMAP_TEXT_BUFFER_SIZE );
    m_lap->SetBitmapTextSpacing( NUMERIC_TEXT_SPACING );
#ifdef RAD_WIN32
    m_lap->Translate( -25, 0 );
    m_lap->ScaleAboutCenter( 0.5f );
#endif

    pGroup = m_missionOverlays->GetGroup( "DamageMeter" );
    rAssert( pGroup != NULL );
    m_overlays[ HUD_DAMAGE_METER ] = pGroup;
    m_damageMeter.SetScroobyImage( pGroup->GetSprite( "DamageBar" ) );

    pGroup = m_missionOverlays->GetGroup( "ProximityMeter" );
    rAssert( pGroup != NULL );
    m_overlays[ HUD_PROXIMITY_METER ] = pGroup;
    m_proximityMeter.SetScroobyImage( pGroup->GetSprite( "ProximityBar" ) );

    pGroup = m_missionOverlays->GetGroup( "MissionComplete" );
    rAssert( pGroup != NULL );
    m_overlays[ HUD_MISSION_COMPLETE ] = pGroup;

    // get 'mission complete' text
    //
    m_missionComplete = pGroup->GetSprite( "MissionComplete" );
    rAssert( m_missionComplete != NULL );
    m_missionComplete->SetSpriteMode( Scrooby::SPRITE_BITMAP_TEXT );
    m_missionComplete->CreateBitmapTextBuffer( 256 );
    m_missionComplete->SetBitmapText( GetTextBibleString( "MISSION_COMPLETE" ) );
    m_missionComplete->SetBitmapTextLineSpacing( 10 );

    // get 'mission failed' text
    //
    m_missionFailedSprite = pGroup->GetSprite( "MissionFailed" );
    rAssert( m_missionFailedSprite != NULL );
    m_missionFailedSprite->SetSpriteMode( Scrooby::SPRITE_BITMAP_TEXT );
    m_missionFailedSprite->CreateBitmapTextBuffer( 256 );
    m_missionFailedSprite->SetBitmapText( GetTextBibleString( "MISSION_FAILED" ) );
    m_missionFailedSprite->SetBitmapTextLineSpacing( 10 );
#ifdef RAD_WIN32
    m_missionFailedSprite->ScaleAboutCenter( 0.5f );
#endif

    pGroup = pPage->GetGroup( "HudMap0" );
    rAssert( pGroup != NULL );
    m_overlays[ HUD_MAP ] = pGroup;

    // scale entire HUD map (or RADAR, if you'd prefer to call it that)
    //
    m_overlays[ HUD_MAP ]->ResetTransformation();
    m_overlays[ HUD_MAP ]->ScaleAboutCenter( RADAR_SCALE );

    Scrooby::Group* pGroupHNR = pGroup->GetGroup( "HitNRunMeter" );
    if( pGroupHNR != NULL )
    {
        for( i = 0; i < NUM_HIT_N_RUN_SECTORS; i++ )
        {
            char name[ 16 ];
            sprintf( name, "HnRSector%d", i );
            m_hnrSectors[ i ] = pGroupHNR->GetSprite( name );

//            rAssert( m_hnrSectors[ i ] != NULL );
//            m_hnrSectors[ i ]->SetColour( HNR_METER_EMPTY_COLOUR );

        }

        m_hnrLights = pGroupHNR->GetSprite( "HitNRun" );
        m_hnrMeter = pGroupHNR->GetSprite( "HnRMeter" );
        m_hnrMeterBgd = pGroupHNR->GetSprite( "HnRMeterBgd" );
    }

    pGroup = pPage->GetGroup( "Message" );
    rAssert( pGroup != NULL );
    m_overlays[ HUD_MESSAGE ] = pGroup;
    m_helpMessage = pGroup->GetText( "Message" );
    rAssert( m_helpMessage != NULL );
    m_helpMessage->SetTextMode( Scrooby::TEXT_WRAP );
    m_helpMessage->ResetTransformation();
    m_helpMessage->Translate( 0, MESSAGE_TEXT_VERTICAL_TRANSLATION );
    m_helpMessage->ScaleAboutPoint( MESSAGE_TEXT_SCALE * MESSGAE_TEXT_HORIZONTAL_STRETCH,
                                    MESSAGE_TEXT_SCALE,
                                    1.0f, 0, 0 );

    m_messageBox = pGroup->GetSprite( "MessageBox" );
    rAssert( m_messageBox != NULL );
    m_messageBox->ResetTransformation();
    m_messageBox->ScaleAboutCenter( MESSAGE_BOX_CORRECTION_SCALE * MESSAGE_BOX_HORIZONTAL_STRETCH,
                                    MESSAGE_BOX_CORRECTION_SCALE * MESSAGE_BOX_VERTICAL_STRETCH,
                                    1.0f );

    pGroup = pPage->GetGroup( "ActionButton" );
    rAssert( pGroup != NULL );
    m_overlays[ HUD_ACTION_BUTTON ] = pGroup;
#ifdef RAD_WIN32
    m_actionButton = pGroup->GetText( "ActionTextButton" );
    m_actionLabel = pGroup->GetText( "ActionTextLabel" );
#else
    m_actionButton = pGroup->GetSprite( "ActionButton" );
#endif

    // hide all HUD overlays by default
    //
    for( i = 0; i < NUM_HUD_OVERLAYS; i++ )
    {
        if( i == NUM_HUD_MISSION_OVERLAYS )
        {
            continue;
        }

        rAssert( m_overlays[ i ] );
        m_overlays[ i ]->SetVisible( false );

        m_elapsedTime[ i ] = 0;
    }

    Mission* currentMission = GetGameplayManager()->GetCurrentMission();
    if( currentMission != NULL )
    {
        this->SetVisible( currentMission->IsHUDVisible() );
    }

    // create HUD event handlers
    //
    m_hudEventHandlers[ HUD_EVENT_HANDLER_CARD_COLLECTED ] = new HudCardCollected( pPage );
    m_hudEventHandlers[ HUD_EVENT_HANDLER_COIN_COLLECTED ] = new HudCoinCollected( pPage );
    m_hudEventHandlers[ HUD_EVENT_HANDLER_MISSION_PROGRESS ] = new HudMissionProgress( pPage );
    m_hudEventHandlers[ HUD_EVENT_HANDLER_MISSION_OBJECTIVE ] = new HudMissionObjective( pPage );
    m_hudEventHandlers[ HUD_EVENT_HANDLER_COUNTDOWN ] = new HudCountDown( pPage );
    m_hudEventHandlers[ HUD_EVENT_HANDLER_HIT_N_RUN ] = new HudHitNRun( pPage );
    m_hudEventHandlers[ HUD_EVENT_HANDLER_WASP_DESTROYED ] = new HudWaspDestroyed( pPage );
    m_hudEventHandlers[ HUD_EVENT_HANDLER_ITEM_DROPPED ] = new HudItemDropped( pPage );

    // register event listeners
    //
    GetEventManager()->AddListener( this, EVENT_CARD_COLLECTED );
    GetEventManager()->AddListener( this, EVENT_COLLECTED_COINS );
    GetEventManager()->AddListener( this, EVENT_LOST_COINS );
    GetEventManager()->AddListener( this, EVENT_STAGE_COMPLETE );
    GetEventManager()->AddListener( this, EVENT_SHOW_MISSION_OBJECTIVE );
    GetEventManager()->AddListener( this, EVENT_GUI_COUNTDOWN_START );
    GetEventManager()->AddListener( this, EVENT_HIT_AND_RUN_START );
    GetEventManager()->AddListener( this, EVENT_HIT_AND_RUN_CAUGHT );
    GetEventManager()->AddListener( this, EVENT_GAMBLERACE_SUCCESS );
    GetEventManager()->AddListener( this, EVENT_GAMBLERACE_FAILURE );
    GetEventManager()->AddListener( this, EVENT_WASP_BLOWED_UP );
    GetEventManager()->AddListener( this, EVENT_GAG_FOUND );
    GetEventManager()->AddListener( this, EVENT_DUMP_STATUS );
    GetEventManager()->AddListener( this, EVENT_AVATAR_ON_ROAD );
    GetEventManager()->AddListener( this, EVENT_AVATAR_OFF_ROAD );
    GetEventManager()->AddListener( this, EVENT_OUTOFCAR_CONDITION_ACTIVE );
    GetEventManager()->AddListener( this, EVENT_OUTOFCAR_CONDITION_INACTIVE );

MEMTRACK_POP_GROUP("CGUIScreenHud");
}


//===========================================================================
// CGuiScreenHud::~CGuiScreenHud
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
CGuiScreenHud::~CGuiScreenHud()
{
    // destroy hud event handlers
    //
    for( int i = 0; i < NUM_HUD_EVENT_HANDLERS; i++ )
    {
        if( m_hudEventHandlers[ i ] != NULL )
        {
            // stop them first in case they're still in progress
            //
            m_hudEventHandlers[ i ]->Stop();

            delete m_hudEventHandlers[ i ];
            m_hudEventHandlers[ i ] = NULL;
        }
    }

    // unregister event listeners
    //
    GetEventManager()->RemoveAll( this );

    s_numCoinsDisplay = NULL;
}


//===========================================================================
// CGuiScreenHud::HandleMessage
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
void CGuiScreenHud::HandleMessage
(
	eGuiMessage message, 
	unsigned int param1,
	unsigned int param2 
)
{
    if( message == GUI_MSG_MENU_PROMPT_RESPONSE )
    {
        m_pParent->HandleMessage( GUI_MSG_RESUME_INGAME );
    }

    switch( message )
    {
        case GUI_MSG_UPDATE:
        {
            this->Update( param1 );

            break;
        }
        case GUI_MSG_SHOW_HUD_OVERLAY:
        {
            if( param1 == HUD_TIMER_TEMP && m_overlays[ HUD_TIMER ]->IsVisible() )
            {
                // ignore, cuz these are both the same overlay
                //
                break;
            }

            // follow-through
            //
        }
        case GUI_MSG_HIDE_HUD_OVERLAY:
        {
            rAssert( param1 < NUM_HUD_OVERLAYS );
            rAssert( m_overlays[ param1 ] != NULL );

            // show/hide HUD overlay
            //
            m_overlays[ param1 ]->SetVisible( message == GUI_MSG_SHOW_HUD_OVERLAY );
            m_elapsedTime[ param1 ] = 0;

            rAssertMsg( param1 != HUD_MESSAGE, "Call CGuiScreenHud::DisplayMessage() instead!" );

            // for mission-related overlays
            //
            if( message == GUI_MSG_SHOW_HUD_OVERLAY &&
                this->IsMissionOverlay( param1 ) )
            {
                const float SECONDARY_MISSION_OVERLAY_SCALE = 0.75f;
                const int SECONDARY_MISSION_OVERLAY_OFFSET_Y = -45;

                m_overlays[ param1 ]->ResetTransformation();

                unsigned int otherOverlayIndex = 0;
                int numOverlaysEnabled = 0;

                for( unsigned int i = 0; this->IsMissionOverlay( i ); i++ )
                {
                    if( m_overlays[ i ]->IsVisible() && m_overlays[ i ] != m_overlays[ param1 ] )
                    {
                        otherOverlayIndex = i;
                        numOverlaysEnabled++;
                    }
                }

                if( numOverlaysEnabled >= 2 )
                {
                    rAssertMsg( numOverlaysEnabled == 2, "More than two is not handled properly yet!" );

                    // if already two overlays enabled, just push this new one down as the third one
                    // w/out re-ordering anything
                    //
                    m_overlays[ param1 ]->ScaleAboutPoint( SECONDARY_MISSION_OVERLAY_SCALE, 0, 0 );
                    m_overlays[ param1 ]->Translate( 0, SECONDARY_MISSION_OVERLAY_OFFSET_Y * 2 );
                }
                else if( numOverlaysEnabled == 1 )
                {
                    // already another mission overlay enabled, need to
                    // scale and translate the secondary one
                    //
                    unsigned int secondaryOverlay = rmt::Max( otherOverlayIndex, param1 );

                    m_overlays[ secondaryOverlay ]->ResetTransformation();
                    m_overlays[ secondaryOverlay ]->ScaleAboutPoint( SECONDARY_MISSION_OVERLAY_SCALE, 0, 0 );
                    m_overlays[ secondaryOverlay ]->Translate( 0, SECONDARY_MISSION_OVERLAY_OFFSET_Y );
                }
            }

            // special cases
            //
            if( param1 == HUD_PROXIMITY_METER )
            {
                rAssert( m_proximityMeter.m_pImage != NULL );
                if( param2 == 0 ) // 0 = red bar
                {
                    m_proximityMeter.m_pImage->SetColour( PROXIMITY_METER_RED );
                }
                else // 1 = green bar
                {
                    m_proximityMeter.m_pImage->SetColour( PROXIMITY_METER_GREEN );
                }
            }
            else if( param1 == HUD_MISSION_COMPLETE )
            {
                m_isBonusMissionJustCompleted = false;

                Mission* currentMission = GetGameplayManager()->GetCurrentMission();
                rAssert( currentMission != NULL );
                if( !currentMission->IsWagerMission() ) // not a wager race
                {
                    if( currentMission->IsBonusMission() )
                    {
                        rAssert( m_missionComplete != NULL );
                        m_missionComplete->SetBitmapText( GetTextBibleString( "BONUS_MISSION_COMPLETE" ) );

                        m_isBonusMissionJustCompleted = true;
                    }
                    else
                    {
                        rAssert( m_missionComplete != NULL );
                        m_missionComplete->SetBitmapText( GetTextBibleString( "MISSION_COMPLETE" ) );
                    }
                }
            }
            else if( param1 == HUD_LAP_COUNTER )
            {
                m_lapUpdated = 0;
            }
/*
            else if( param1 == HUD_MAP )
            {
                if( param2 != 0 ) // != 0 means w/ fading
                {
                    if( message == GUI_MSG_SHOW_HUD_OVERLAY )
                    {
                        m_isFadingInRadar = true;
                        m_isFadingOutRadar = false;
                    }
                    else
                    {
                        m_isFadingOutRadar = true;
                        m_isFadingInRadar = false;

                        // wait for fade out transition to finish
                        // before turning off radar
                        //
                        m_overlays[ HUD_MAP ]->SetVisible( true );
                    }
                }
            }
            else if( param1 == HUD_DAMAGE_METER )
            {
                m_isSlidingDamageMeter = (message == GUI_MSG_SHOW_HUD_OVERLAY);
            }
*/

            break;
        }
        case GUI_MSG_INGAME_FADE_OUT:
        {
            if( !m_isFadingOut )
            {
                rAssertMsg( !m_isFadingIn, "Fade in currently in progress!" );

                m_isFadingOut = true;
                m_elapsedFadeTime = 0.0f;
                ++m_numTransitionsPending;
            }
            else
            {
                rWarningMsg( false, "Fade out currently in progress! Ignoring redundant request." );
            }

            break;
        }
        case GUI_MSG_INGAME_FADE_IN:
        {
            if( !m_isFadingIn )
            {
                rAssertMsg( !m_isFadingOut, "Fade out currently in progress!" );
                ++m_numTransitionsPending;
                m_isFadingIn = true;
                m_elapsedFadeTime = 0.0f;
            }
            else
            {
                rWarningMsg( false, "Fade in currently in progress! Ignoring redundant request." );
            }

            break;
        }
        case GUI_MSG_INGAME_FADE_OUT_CANCEL:
        {
            if( m_isFadingOut )
            {
                rAssert( m_screenCover != NULL );
                m_screenCover->SetAlpha( 1.0f );

                m_isFadingOut = false;
                --m_numTransitionsPending;

                GetEventManager()->TriggerEvent( EVENT_GUI_FADE_OUT_DONE );
            }

            break;
        }
        case GUI_MSG_INGAME_FADE_IN_CANCEL:
        {
            if( m_isFadingIn )
            {
                rAssert( m_screenCover != NULL );
                m_screenCover->SetAlpha( 0.0f );

                m_isFadingIn = false;
                --m_numTransitionsPending;

                GetEventManager()->TriggerEvent( EVENT_GUI_FADE_IN_DONE );
            }

            break;
        }
        case GUI_MSG_CONTROLLER_START:
        {
            if( m_screenCover != NULL && m_screenCover->GetAlpha() > 0.0f )
            {
                // don't allow pausing during fade in/out
                //
                return;
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
	CGuiScreenMultiHud::HandleMessage( message, param1, param2 );
}

//===========================================================================
// CGuiScreenHud::Update
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	elapsed time
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenHud::Update( unsigned int elapsedTime )
{
    if( m_state == GUI_WINDOW_STATE_RUNNING )
    {
        if( m_elapsedFgdFadeInTime != -1.0f )
        {
            // fade in entire HUD foreground layer(s) to prevent sudden flashes
            // on screen during situations where the HUD gets displayed very
            // briefly before transitioning out to another screen
            //
            const float HUD_FGD_FADE_IN_TIME = 100.0f;

            m_elapsedFgdFadeInTime += elapsedTime;
            if( m_elapsedFgdFadeInTime < HUD_FGD_FADE_IN_TIME )
            {
//                float alpha = m_elapsedFgdFadeInTime / HUD_FGD_FADE_IN_TIME;
                this->SetAlphaForLayers( 0.0f, m_foregroundLayers, m_numForegroundLayers );
            }
            else
            {
                this->SetAlphaForLayers( 1.0f, m_foregroundLayers, m_numForegroundLayers );

                rAssert( m_overlays[ HUD_MAP ] != NULL );
                m_overlays[ HUD_MAP ]->SetAlpha( 1.0f );

                rAssert( m_hudMap[ 0 ] != NULL );
                m_hudMap[ 0 ]->SetVisible( GetGuiSystem()->IsRadarEnabled() );

                m_elapsedFgdFadeInTime = -1.0f; // fade in is done
            }
        }
        else
        {
            rAssert( m_hudMap[ 0 ] != NULL );
            m_hudMap[ 0 ]->SetVisible( GetGuiSystem()->IsRadarEnabled() );
        }
    }

    static float INGAME_FADE_TIME = 250.0f; // in msec

    if( m_isFadingOut )
    {
        rAssert( m_screenCover != NULL );

        m_elapsedFadeTime += (float)elapsedTime;
        if( m_elapsedFadeTime < INGAME_FADE_TIME )
        {
            m_screenCover->SetAlpha( m_elapsedFadeTime / INGAME_FADE_TIME );
        }
        else
        {
            m_screenCover->SetAlpha( 1.0f );

            --m_numTransitionsPending;
            m_isFadingOut = false;

            GetEventManager()->TriggerEvent( EVENT_GUI_FADE_OUT_DONE );
        }
    }

    if( m_isFadingIn )
    {
        rAssert( m_screenCover != NULL );

        m_elapsedFadeTime += (float)elapsedTime;
        if( m_elapsedFadeTime < INGAME_FADE_TIME )
        {
            m_screenCover->SetAlpha( 1.0f - m_elapsedFadeTime / INGAME_FADE_TIME );
        }
        else
        {
            m_screenCover->SetAlpha( 0.0f );

            --m_numTransitionsPending;
            m_isFadingIn = false;

            GetEventManager()->TriggerEvent( EVENT_GUI_FADE_IN_DONE );
        }
    }

    this->UpdateOverlays( elapsedTime );
    this->UpdateEventHandlers( elapsedTime );
    this->UpdateTutorialMode( static_cast<float>( elapsedTime ) );

#ifdef SRR2_MOVABLE_HUD_MAP
    this->UpdateHudMapPosition( elapsedTime );
#endif

/*
    // TC: *** testing only ***
    //
    static float s_elapsedTime = 0.0f;
    s_elapsedTime += static_cast<float>( elapsedTime );
    float value = 0.5f * rmt::Sin( rmt::PI_2 * s_elapsedTime / 2000.0f ) + 0.5f;
    this->SetHitAndRunMeter( value );
*/
    HitnRunManager* hnrManager = GetHitnRunManager();
    rAssert( hnrManager != NULL );

    // blink H&R Meter lights
    //
    const unsigned int HNR_BLINK_PERIOD = 500;

    rAssert( m_hnrLights != NULL );
    rAssert( m_hnrMeterBgd != NULL );
    if( hnrManager->IsHitnRunActive() )
    {
        m_hnrMeterBgd->SetColour( tColour( 17, 31, 161 ) );
    }
    else
    {
        m_hnrMeterBgd->SetColour( tColour( 0, 0, 0 ) );

        m_hnrLights->SetIndex( 2 ); // 2 = lights off
    }

    m_hnrElapsedTime += elapsedTime;
    if( m_hnrElapsedTime > HNR_BLINK_PERIOD / 2 )
    {
        if( hnrManager->IsHitnRunActive() )
        {
            int currentIndex = m_hnrLights->GetIndex();
            m_hnrLights->SetIndex( currentIndex > 1 ? 0 : 1 - currentIndex );

            rAssert( m_hnrMeter != NULL );
            m_hnrMeter->SetVisible( !m_hnrMeter->IsVisible() );
        }
        else if( (hnrManager->GetHitnRunValue() / 100.0f) > HNR_METER_BLINKING_THRESHOLD )
        {
            rAssert( m_hnrMeter != NULL );
            m_hnrMeter->SetVisible( !m_hnrMeter->IsVisible() );

            // TC: [TODO] throw a sound event to go w/ this
            //
        }

        m_hnrElapsedTime %= HNR_BLINK_PERIOD / 2;
    }

    if( m_isAvatarOffRoad )
    {
        // update avatar off-road duration time
        //
        m_avatarOffRoadDurationTime += elapsedTime;
    }
}

//===========================================================================
// CGuiScreenHud::SetVisible
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
void CGuiScreenHud::SetVisible( bool isVisible )
{
    rAssert( m_pScroobyScreen != NULL );

    // for each page in screen
    //
    int numPages = m_pScroobyScreen->GetNumberOfPages();
    for( int i = 0; i < numPages; i++ )
    {
        Scrooby::Page* pPage = m_pScroobyScreen->GetPageByIndex( i );
        rAssert( pPage != NULL );

        // for each layer in page
        //
        int numLayers = pPage->GetNumberOfLayers();
        for( int j = 0; j < numLayers; j++ )
        {
            Scrooby::Layer* pLayer = pPage->GetLayerByIndex( j );
            rAssert( pLayer );

            pLayer->SetVisible( isVisible );
        }
    }
}


//===========================================================================
// CGuiScreenHud::SetTimerBlinkingInterval
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
void CGuiScreenHud::SetTimerBlinkingInterval( int startTime, int endTime )
{
    m_timerBlinkingStartTime = startTime;
    m_timerBlinkingEndTime = endTime;
}


//===========================================================================
// CGuiScreenHud::SetParTime
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
void CGuiScreenHud::SetParTime( int parTime ) // in seconds
{
    char buffer[ BITMAP_TEXT_BUFFER_SIZE ];

    sprintf( buffer, "%d:%02d", parTime / 60, parTime % 60 );

    rAssert( m_parTime != NULL );
    m_parTime->SetBitmapText( buffer );
}


//===========================================================================
// CGuiScreenHud::SetCollectibles
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
void CGuiScreenHud::SetCollectibles( int numCollected, int numToCollect )
{
//    rAssert( m_collectibles != NULL );
//    m_collectibles->SetVisible( numToCollect > 1 ); // only show collectibles if more than 1 to collect

    char buffer[ BITMAP_TEXT_BUFFER_SIZE ];
    sprintf( buffer, "%d/%d", numCollected, numToCollect );

    m_collectibles->SetBitmapText( buffer );
/*
    m_playerCollected.SetValue( numCollected );
    m_numToCollect.SetValue( numToCollect );
*/
    if( numCollected > 0 )
    {
        m_collectiblesUpdated = 1;

        m_elapsedTime[ HUD_COLLECTIBLES ] = 0;

        // TC [TODO]: play sound effect
        //
    }
}


//===========================================================================
// CGuiScreenHud::SetRacePosition
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
void CGuiScreenHud::SetRacePosition( int currentPosition, int numPositions )
{
    char buffer[ BITMAP_TEXT_BUFFER_SIZE ];

    // extra spaces are needed to make more room for the ordinal text
    //
    char extraSpaces[ 4 ];
    strcpy( extraSpaces, "  " );

#ifdef PAL
    if( CGuiTextBible::GetCurrentLanguage() != Scrooby::XL_ENGLISH )
    {
        extraSpaces[ 0 ] = '\0';
    }
#endif // PAL

    if( m_isAvatarOffRoad && m_avatarOffRoadDurationTime > MIN_AVATAR_OFF_ROAD_TIME )
    {
        sprintf( buffer, "?%s/%d", extraSpaces, numPositions );
        m_positionOrdinal->SetVisible( false );
    }
    else
    {
        sprintf( buffer, "%d%s/%d", currentPosition, extraSpaces, numPositions );
        m_positionOrdinal->SetVisible( true );
    }

    rAssert( m_position != NULL );
    m_position->SetBitmapText( buffer );
/*
    m_currentPosition.SetValue( currentPosition );
    m_numPositions.SetValue( numPositions );
*/
    m_positionOrdinal->SetIndex( currentPosition > 3 ? 3 : currentPosition - 1 );
}


//===========================================================================
// CGuiScreenHud::SetLap
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
void CGuiScreenHud::SetLap( int currentLap, int numLaps )
{
    char buffer[ BITMAP_TEXT_BUFFER_SIZE ];
    sprintf( buffer, "%d/%d", currentLap, numLaps );

    rAssert( m_lap != NULL );
    m_lap->SetBitmapText( buffer );
/*
    m_currentLap.SetValue( currentLap );
    m_numLaps.SetValue( numLaps );
*/
    if( currentLap > 1 )
    {
        m_lapUpdated = 8;

        m_elapsedTime[ HUD_LAP_COUNTER ] = 0;

        // play sound effect
        //
        GetEventManager()->TriggerEvent( EVENT_HUD_LAP_UPDATED );
    }
    else
    {
        m_lapUpdated = 0;
    }
}


//===========================================================================
// CGuiScreenHud::SetDamageMeter
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
void CGuiScreenHud::SetDamageMeter( float value )
{
    m_damageMeter.SetValue( value );
}


//===========================================================================
// CGuiScreenHud::SetProximityMeter
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
void CGuiScreenHud::SetProximityMeter( float value )
{
    m_proximityMeter.SetValue( value );
}


//===========================================================================
// CGuiScreenHud::SetHitAndRunMeter
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
void CGuiScreenHud::SetHitAndRunMeter( float value )
{
    rAssert( value >= 0.0f && value <= 1.0f );

    rAssert( m_hnrMeter != NULL );
    if( value > HNR_METER_BLINKING_THRESHOLD || GetHitnRunManager()->IsHitnRunActive() )
    {
        m_hnrMeter->SetColour( HNR_METER_WARNING_COLOUR );
    }
    else
    {
        m_hnrMeter->SetColour( HNR_METER_DEFAULT_COLOUR ); // restore default colour
        m_hnrMeter->SetVisible( true );
    }

    const float TOTAL_AMOUNT_COVERED = 1.0f - value;
    const float SECTOR_AMOUNT = 1.0f / NUM_HIT_N_RUN_SECTORS;
    const float TOTAL_ANGLE = (1.0f - 1 / (float)NUM_HIT_N_RUN_SECTORS) * 360.0f;

    float amountCovered = 0.0f;

    for( int i = 0; i < NUM_HIT_N_RUN_SECTORS; i++ )
    {
        rAssert( m_hnrSectors[ i ] != NULL );
        m_hnrSectors[ i ]->ResetTransformation();

        if( amountCovered < TOTAL_AMOUNT_COVERED )
        {
            float currentCovered = TOTAL_AMOUNT_COVERED - amountCovered;
            if( currentCovered > SECTOR_AMOUNT )
            {
                currentCovered = SECTOR_AMOUNT;
            }

            amountCovered += currentCovered;

            m_hnrSectors[ i ]->RotateAboutCenter( -amountCovered * TOTAL_ANGLE );

            static float correctionScale = 1.02f;
            m_hnrSectors[ i ]->ScaleAboutCenter( correctionScale );
/*
#ifdef RAD_DEBUG
            static tColour sectorColour( HNR_METER_EMPTY_COLOUR );
            m_hnrSectors[ i ]->SetColour( sectorColour );
#endif
*/
        }
    }
}


//===========================================================================
// CGuiScreenHud::DisplayMessage
//===========================================================================
// Description: adds some scrolling text to the queue of text to show
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      N/A.
//
//===========================================================================
void CGuiScreenHud::DisplayMessage( bool show, const int index )
{
    rAssert( m_overlays[ HUD_MESSAGE ] != NULL );

    if( show )
    {
        if( !m_helpMessageQueue.IsEmpty() )
        {
            m_helpMessageQueue.Enqueue( index );
        }
        else
        {
            if( m_overlays[ HUD_MESSAGE ]->IsVisible() )
            {
                if( m_elapsedTime[ HUD_MESSAGE ] > MESSAGE_DISPLAY_TIME - MESSAGE_TRANSITION_TIME )
                {
                    m_elapsedTime[ HUD_MESSAGE ] = MESSAGE_DISPLAY_TIME - m_elapsedTime[ HUD_MESSAGE ];
                }
                else if( m_elapsedTime[ HUD_MESSAGE ] > MESSAGE_TRANSITION_TIME )
                {
                    m_elapsedTime[ HUD_MESSAGE ] = MESSAGE_TRANSITION_TIME;
                }
            }
            else
            {
                m_overlays[ HUD_MESSAGE ]->SetVisible( true );
                m_overlays[ HUD_MESSAGE ]->ScaleAboutCenter( 0.0f );
                m_elapsedTime[ HUD_MESSAGE ] = 0;
            }

            rAssert( index >= 0 );
            m_helpMessage->SetIndex( index );
        }
    }
    else
    {
        m_overlays[ HUD_MESSAGE ]->SetVisible( false );
    }
}


void
CGuiScreenHud::DisplayMissionOverlays( bool show )
{
    rAssert( m_missionOverlays != NULL );
    m_missionOverlays->SetVisible( show );
}


void
CGuiScreenHud::HandleEvent( EventEnum id, void* pEventData )
{
    switch( id )
    {
        case EVENT_CARD_COLLECTED:
        {
            // stop coin presentation in case it's currently active
            //
            rAssert( m_hudEventHandlers[ HUD_EVENT_HANDLER_COIN_COLLECTED ] != NULL );
            m_hudEventHandlers[ HUD_EVENT_HANDLER_COIN_COLLECTED ]->Stop();

            rAssert( m_hudEventHandlers[ HUD_EVENT_HANDLER_CARD_COLLECTED ] != NULL );
            m_hudEventHandlers[ HUD_EVENT_HANDLER_CARD_COLLECTED ]->Start();

            Card* collectedCard = reinterpret_cast<Card*>( pEventData );
            rAssert( collectedCard != NULL );
            HudCardCollected* hudCardCollected = static_cast<HudCardCollected*>( m_hudEventHandlers[ HUD_EVENT_HANDLER_CARD_COLLECTED ] );
            hudCardCollected->SetCurrentCard( collectedCard->GetID() );

            break;
        }
        case EVENT_COLLECTED_COINS:
        case EVENT_LOST_COINS:
        {
            rAssert( m_hudEventHandlers[ HUD_EVENT_HANDLER_CARD_COLLECTED ] != NULL );
            if( !m_hudEventHandlers[ HUD_EVENT_HANDLER_CARD_COLLECTED ]->IsActive() )
            {
                // only show coin presentation if card presentation is not active
                //
                rAssert( m_hudEventHandlers[ HUD_EVENT_HANDLER_COIN_COLLECTED ] != NULL );
                m_hudEventHandlers[ HUD_EVENT_HANDLER_COIN_COLLECTED ]->Start();
            }

            break;
        }
        case EVENT_STAGE_COMPLETE:
        {
            unsigned int showStageComplete = reinterpret_cast<unsigned int>( pEventData );
            if( showStageComplete > 0 )
            {
                rAssert( m_hudEventHandlers[ HUD_EVENT_HANDLER_MISSION_PROGRESS ] != NULL );
                m_hudEventHandlers[ HUD_EVENT_HANDLER_MISSION_PROGRESS ]->Start();
            }

            break;
        }
        case EVENT_SHOW_MISSION_OBJECTIVE:
        {
            int messageID = reinterpret_cast<unsigned int>( pEventData );

            HudMissionObjective* hudMissionObjective = static_cast<HudMissionObjective*>( m_hudEventHandlers[ HUD_EVENT_HANDLER_MISSION_OBJECTIVE ] );
            if( messageID != -1 )
            {
                rAssert( m_hudEventHandlers[ HUD_EVENT_HANDLER_MISSION_OBJECTIVE ] != NULL );
                m_hudEventHandlers[ HUD_EVENT_HANDLER_MISSION_OBJECTIVE ]->Start();

//                this->DisplayMessage( true, messageID );

                hudMissionObjective->SetMessageID( messageID );
            }
            else
            {
                hudMissionObjective->UpdateIcon();
            }

            break;
        }
        case EVENT_GUI_COUNTDOWN_START:
        {
            rAssert( m_hudEventHandlers[ HUD_EVENT_HANDLER_COUNTDOWN ] != NULL );
            m_hudEventHandlers[ HUD_EVENT_HANDLER_COUNTDOWN ]->Start();

            break;
        }
        case EVENT_HIT_AND_RUN_START:
        case EVENT_HIT_AND_RUN_CAUGHT:
        {
            HudHitNRun* hudHitNRun = static_cast<HudHitNRun*>( m_hudEventHandlers[ HUD_EVENT_HANDLER_HIT_N_RUN] );
            rAssert( hudHitNRun != NULL );
            if( id == EVENT_HIT_AND_RUN_START )
            {
                hudHitNRun->SetMessage( HudHitNRun::MSG_HIT_N_RUN );
            }
            else
            {
                rAssert( id == EVENT_HIT_AND_RUN_CAUGHT );
                hudHitNRun->SetMessage( HudHitNRun::MSG_BUSTED );
            }

            rAssert( m_hudEventHandlers[ HUD_EVENT_HANDLER_HIT_N_RUN ] != NULL );
            m_hudEventHandlers[ HUD_EVENT_HANDLER_HIT_N_RUN ]->Start();

            break;
        }
        case EVENT_GAMBLERACE_SUCCESS: // (pEventData != 0) means new best time been set
        {
            if( pEventData != 0 )
            {
                // display "Congratulations! New Best Time!"
                //
                rAssert( m_missionComplete != NULL );
                m_missionComplete->SetBitmapText( GetTextBibleString( "GAMBLING_RACE_SUCCESS_NBT" ) );
            }
            else
            {
                // display "Congratulations!"
                //
                rAssert( m_missionComplete != NULL );
                m_missionComplete->SetBitmapText( GetTextBibleString( "GAMBLING_RACE_SUCCESS" ) );
            }

            break;
        }
        case EVENT_GAMBLERACE_FAILURE:
        {
            rAssert( m_missionComplete != NULL );
            m_missionComplete->SetBitmapText( GetTextBibleString( "GAMBLING_RACE_FAILURE" ) );

            break;
        }
        case EVENT_GAG_FOUND:
        case EVENT_WASP_BLOWED_UP:
        {
            HudWaspDestroyed* hudWaspDestroyed = static_cast<HudWaspDestroyed*>( m_hudEventHandlers[ HUD_EVENT_HANDLER_WASP_DESTROYED] );
            rAssert( hudWaspDestroyed != NULL );
            hudWaspDestroyed->SetGagInsteadOfWasp( id == EVENT_GAG_FOUND );

            rAssert( m_hudEventHandlers[ HUD_EVENT_HANDLER_WASP_DESTROYED ] != NULL );
            m_hudEventHandlers[ HUD_EVENT_HANDLER_WASP_DESTROYED ]->Start();

            break;
        }
        case EVENT_DUMP_STATUS:
        {
            rAssert( m_hudEventHandlers[ HUD_EVENT_HANDLER_ITEM_DROPPED ] != NULL );

            int numActiveItems = reinterpret_cast<int>( pEventData );
            if( numActiveItems > 0 )
            {
                m_hudEventHandlers[ HUD_EVENT_HANDLER_ITEM_DROPPED ]->Start();
            }
            else
            {
                m_hudEventHandlers[ HUD_EVENT_HANDLER_ITEM_DROPPED ]->Stop();
            }

            break;
        }
        case EVENT_AVATAR_ON_ROAD:
        {
            m_isAvatarOffRoad = false;

            break;
        }
        case EVENT_AVATAR_OFF_ROAD:
        {
            if( !m_isAvatarOffRoad ) // if previously on-road
            {
                m_avatarOffRoadDurationTime = 0; // reset off-road duration time
            }

            m_isAvatarOffRoad = true;

            break;
        }
        case EVENT_OUTOFCAR_CONDITION_ACTIVE:
            {
                m_pGetOutOfCarCondition = static_cast<GetOutOfCarCondition*> (pEventData);
                rAssert(m_pGetOutOfCarCondition != NULL);

                break;
            }

        case EVENT_OUTOFCAR_CONDITION_INACTIVE:
            {
                m_pGetOutOfCarCondition = NULL;

                break;
            }


        default:
        {
            rWarningMsg( false, "Unhandled event caught by GUI HUD!" );

            break;
        }
    }
}

void
CGuiScreenHud::SetNumCoinsDisplay( Scrooby::Sprite* pSprite )
{
    if( s_numCoinsDisplay == NULL )
    {
        rAssert( pSprite != NULL );
        pSprite->SetSpriteMode( Scrooby::SPRITE_BITMAP_TEXT );
        pSprite->CreateBitmapTextBuffer( MAX_NUM_COIN_DIGITS );
        pSprite->SetBitmapTextSpacing( NUMERIC_TEXT_SPACING );
        pSprite->SetVisible( false ); // hide by default

#ifdef RAD_WIN32
        pSprite->ResetTransformation();
        pSprite->Translate( 70, 0 );
        pSprite->ScaleAboutCenter( 0.5f );
#endif

        s_numCoinsDisplay = pSprite;
    }
}

void
CGuiScreenHud::UpdateNumCoinsDisplay( int numCoins, bool show )
{
    rAssert( s_numCoinsDisplay != NULL );
    s_numCoinsDisplay->SetVisible( show );

    static int coinPosX = CGuiScreen::IsWideScreenDisplay() ? 540 : 605;
    static int coinPosY = 432;
    GetCoinManager()->SetHUDCoin( coinPosX, coinPosY, show );

    if( show )
    {
        char buffer[ MAX_NUM_COIN_DIGITS ];
        sprintf( buffer, "%d", numCoins );
        s_numCoinsDisplay->SetBitmapText( buffer );
    }
}

//===========================================================================
// CGuiScreenHud::InitIntro
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
void CGuiScreenHud::InitIntro()
{
    Scrooby::Screen* screen = GetScroobyScreen();
    screen->SetAlpha( 1.0f );

    m_missionFailedSprite->SetVisible( false );

/*
    if( GetInteriorManager()->IsInside() )
    {
        this->HandleMessage( GUI_MSG_HIDE_HUD_OVERLAY, HUD_MAP );
    }
    else
*/
    {
        this->HandleMessage( GUI_MSG_SHOW_HUD_OVERLAY, HUD_MAP );
    }

    // hide hud map, by default
    //
    m_overlays[ HUD_MAP ]->SetAlpha( 0.0f );
    rAssert( m_hudMap[ 0 ] != NULL );
    m_hudMap[ 0 ]->SetVisible( false );

    this->SetAlphaForLayers( 0.0f, m_foregroundLayers, m_numForegroundLayers );

    // force update on hit & run meter
    //
    HitnRunManager* hnrManager = GetHitnRunManager();
    rAssert( hnrManager != NULL );
    this->SetHitAndRunMeter( hnrManager->GetHitnRunValue() / 100.0f );

    CGuiScreenMultiHud::InitIntro();
}


//===========================================================================
// CGuiScreenHud::InitRunning
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
void CGuiScreenHud::InitRunning()
{
//    this->SetFadingEnabled( false );

    // for fading in/out ingame
    //
    rAssert( m_screenCover != NULL );
    m_screenCover->SetVisible( true );
    m_screenCover->SetAlpha( 0.0f );

    // reset HUD foreground fade in time
    //
    m_elapsedFgdFadeInTime = 0.0f;

    CGuiScreenMultiHud::InitRunning();
}


//===========================================================================
// CGuiScreenHud::InitOutro
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
void CGuiScreenHud::InitOutro()
{
/*
    this->HideMissionObjective();

    rAssert( m_overlays[ HUD_MESSAGE ] );
    m_overlays[ HUD_MESSAGE ]->SetVisible( false );

    m_helpMessageQueue.Clear();
*/
    // stop coin collected presentation
    //
    rAssert( m_hudEventHandlers[ HUD_EVENT_HANDLER_COIN_COLLECTED ] != NULL );
    m_hudEventHandlers[ HUD_EVENT_HANDLER_COIN_COLLECTED ]->Stop();

    if( m_isFadingIn ) // if we're in the middle of fading back in from blackness, just stop it right here
    {
        this->HandleMessage( GUI_MSG_INGAME_FADE_IN_CANCEL );
    }

    CGuiScreenMultiHud::InitOutro();
}


//---------------------------------------------------------------------
// Private Functions
//---------------------------------------------------------------------

/*
//===========================================================================
// CGuiScreenHud::DisplayMissionObjective
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
void CGuiScreenHud::DisplayMissionObjective( unsigned int messageID )
{
    rAssert( m_helpMessage );

    // display mission objective message
    //
    m_helpMessage->SetTextIndex( messageID );
    m_helpMessage->Start();
}


//===========================================================================
// CGuiScreenHud::HideMissionObjective
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
void CGuiScreenHud::HideMissionObjective()
{
    // hide mission objective message
    //
    rAssert( m_helpMessage );
    if( m_helpMessage->GetCurrentState() != ScrollingText::STATE_IDLE )
    {
        m_helpMessage->Stop();
    }
}
*/

void
CGuiScreenHud::UpdateOverlays( unsigned int elapsedTime )
{
    // update 'timer' overlay
    //
    if( m_overlays[ HUD_TIMER ]->IsVisible() )
    {
        this->UpdateTimer( elapsedTime );
    }
    else
    {
        Mission* currentMission = GetGameplayManager()->GetCurrentMission();
        rAssert( currentMission != NULL );
        {
            // TC: Temporary work-around until I get Cary's help to fix
            //     this properly
            //
            if( currentMission->GetMissionTimeLeftInMilliSeconds() > 1000 )
            {
                this->HandleMessage( GUI_MSG_SHOW_HUD_OVERLAY, HUD_TIMER );

                this->UpdateTimer( elapsedTime );
            }

            rWarningMsg( currentMission->GetMissionTimeLeftInMilliSeconds() <= 0,
                         "Why is there mission time left and the HUD timer is not enabled??" );
        }
    }

    // update 'message' overlay
    //
    if( m_overlays[ HUD_MESSAGE ]->IsVisible() )
    {
        m_elapsedTime[ HUD_MESSAGE ] += elapsedTime;

        if( m_elapsedTime[ HUD_MESSAGE ] > MESSAGE_DISPLAY_TIME )
        {
            if( !m_helpMessageQueue.IsEmpty() )
            {
                unsigned int messageIndex = m_helpMessageQueue.Dequeue();
                m_helpMessage->SetIndex( static_cast<int>( messageIndex ) );

                // reset elapsed message display time
                //
                m_elapsedTime[ HUD_MESSAGE ] = 0;
            }
            else
            {
                m_overlays[ HUD_MESSAGE ]->SetVisible( false );
            }
        }
        else
        {
            m_overlays[ HUD_MESSAGE ]->ResetTransformation();

            if( m_elapsedTime[ HUD_MESSAGE ] < MESSAGE_TRANSITION_TIME )
            {
                // transition in
                //
                float scaleY = m_elapsedTime[ HUD_MESSAGE ] / (float)MESSAGE_TRANSITION_TIME;
                m_overlays[ HUD_MESSAGE ]->ScaleAboutCenter( 1.0f, scaleY, 1.0f );
            }
            else if( m_elapsedTime[ HUD_MESSAGE ] > MESSAGE_DISPLAY_TIME - MESSAGE_TRANSITION_TIME )
            {
                // transition out
                //
                float scaleY = (MESSAGE_DISPLAY_TIME - m_elapsedTime[ HUD_MESSAGE ]) / (float)MESSAGE_TRANSITION_TIME;
                m_overlays[ HUD_MESSAGE ]->ScaleAboutCenter( 1.0f, scaleY, 1.0f );
            }
        }
    }

    // update 'action button' overlay
    //
    if( m_overlays[ HUD_ACTION_BUTTON ]->IsVisible() )
    {
#ifdef RAD_WIN32
        const float PULSE_AMPLITUDE = 0.10f;
        const float PULSE_PERIOD = 600.0f; // in milliseconds
#else
        const float PULSE_AMPLITUDE = 0.25f;
        const float PULSE_PERIOD = 500.0f; // in milliseconds
#endif

        float scale = GuiSFX::Pulse( (float)m_elapsedTime[ HUD_ACTION_BUTTON ],
                                     PULSE_PERIOD,
                                     1.0f,
                                     PULSE_AMPLITUDE );

        m_actionButton->ResetTransformation();
        m_actionButton->ScaleAboutCenter( scale, scale, 1.0f );

#ifdef RAD_WIN32
        m_actionLabel->ResetTransformation();
        m_actionLabel->ScaleAboutCenter( 0.85f, 0.85f, 1.0f );
#endif

        m_elapsedTime[ HUD_ACTION_BUTTON ] += elapsedTime;
    }

    // update 'mission complete' overlay
    //
    if( m_overlays[ HUD_MISSION_COMPLETE ]->IsVisible() )
    {
        static float MISSION_COMPLETE_DISPLAY_TIME = 2500; // in milliseconds

#ifdef RAD_WIN32
        bool done = GuiSFX::Flash( m_missionComplete,
                                   (float)m_elapsedTime[ HUD_MISSION_COMPLETE ],
                                    MISSION_COMPLETE_DISPLAY_TIME,
                                    3, 0.625f, 0.6f );
#else
        bool done = GuiSFX::Flash( m_missionComplete,
                                   (float)m_elapsedTime[ HUD_MISSION_COMPLETE ],
                                   MISSION_COMPLETE_DISPLAY_TIME,
                                   3, 1.25f, 1.2f );
#endif

        if( done )
        {
            m_overlays[ HUD_MISSION_COMPLETE ]->SetVisible( false );

            if( m_isBonusMissionJustCompleted )
            {
                //
                // Different prompt for different levels
                //
                RenderEnums::LevelEnum level = GetGameplayManager()->GetCurrentLevelIndex();
                int levelInt = static_cast< int >( level );
                m_pParent->HandleMessage( GUI_MSG_INGAME_DISPLAY_PROMPT, 12 + levelInt );
            }
        }

        m_elapsedTime[ HUD_MISSION_COMPLETE ] += elapsedTime;
    }

    // update 'damage meter' overlay
    //
    if( m_overlays[ HUD_DAMAGE_METER ]->IsVisible() )
    {
/*
        if( m_isSlidingDamageMeter )
        {
            const float SLIDING_TIME = 1000.0f;

            if( m_elapsedTime[ HUD_DAMAGE_METER ] < SLIDING_TIME )
            {
                int xPos, yPos;
                m_overlays[ HUD_DAMAGE_METER ]->GetOriginPosition( xPos, yPos );

                int screenWidth = (int)Scrooby::App::GetInstance()->GetScreenWidth();

                int xTranslation = (int)( (1.0f - m_elapsedTime[ HUD_DAMAGE_METER ] / SLIDING_TIME) *
                                          (screenWidth - xPos) );

                m_overlays[ HUD_DAMAGE_METER ]->ResetTransformation();
                m_overlays[ HUD_DAMAGE_METER ]->Translate( xTranslation, 0 );

                m_elapsedTime[ HUD_DAMAGE_METER ] += elapsedTime;
            }
            else
            {
                m_overlays[ HUD_DAMAGE_METER ]->ResetTransformation();

                m_isSlidingDamageMeter = false;
            }
        }
*/
        const float DAMAGE_METER_BLINK_THRESHOLD = 0.75f;

        if( m_damageMeter.m_value > DAMAGE_METER_BLINK_THRESHOLD )
        {
            const unsigned int BLINK_PERIOD = 250;

            bool blinked = GuiSFX::Blink( m_damageMeter.m_pImage,
                                          (float)m_elapsedTime[ HUD_DAMAGE_METER ],
                                          (float)BLINK_PERIOD );
            if( blinked )
            {
                m_elapsedTime[ HUD_DAMAGE_METER ] %= BLINK_PERIOD;
            }

            m_elapsedTime[ HUD_DAMAGE_METER ] += elapsedTime;
        }
        else
        {
            m_damageMeter.m_pImage->SetVisible( true );
        }
    }

    // update 'lap counter' overlay
    //
    if( m_overlays[ HUD_LAP_COUNTER ]->IsVisible() || m_lapUpdated > 0 )
    {
        if( m_lapUpdated > 0 )
        {
            const int BLINK_PERIOD = 250; // msec
            bool blinked = GuiSFX::Blink( m_overlays[ HUD_LAP_COUNTER ],
                                          (float)m_elapsedTime[ HUD_LAP_COUNTER ],
                                          (float)BLINK_PERIOD );

            if( blinked )
            {
                m_elapsedTime[ HUD_LAP_COUNTER ] %= BLINK_PERIOD;

                m_lapUpdated--;
            }

            m_elapsedTime[ HUD_LAP_COUNTER ] += elapsedTime;
        }
    }
/*
    // update 'hud map' overlay
    //
    if( m_overlays[ HUD_MAP ]->IsVisible() )
    {
        if( m_isFadingInRadar )
        {
            float radarAlpha = m_overlays[ HUD_MAP ]->GetAlpha() + (elapsedTime / (float)RADAR_FADE_TIME);

            if( radarAlpha < 1.0f )
            {
                m_overlays[ HUD_MAP ]->SetAlpha( radarAlpha );
            }
            else
            {
                m_overlays[ HUD_MAP ]->SetAlpha( 1.0f );

                m_isFadingInRadar = false;
            }
        }

        if( m_isFadingOutRadar )
        {
            float radarAlpha = m_overlays[ HUD_MAP ]->GetAlpha() - (elapsedTime / (float)RADAR_FADE_TIME);

            if( radarAlpha > 0.0f )
            {
                m_overlays[ HUD_MAP ]->SetAlpha( radarAlpha );
            }
            else
            {
                m_overlays[ HUD_MAP ]->SetAlpha( 0.0f );

                m_isFadingInRadar = false;

                // turn off radar
                //
                m_overlays[ HUD_MAP ]->SetVisible( false );
            }
        }
    }
*/
}

void
CGuiScreenHud::UpdateEventHandlers( unsigned int elapsedTime )
{
    // update HUD event handlers
    //
    for( int j = 0; j < NUM_HUD_EVENT_HANDLERS; j++ )
    {
        if( m_hudEventHandlers[ j ] != NULL )
        {
            m_hudEventHandlers[ j ]->Update( (float)elapsedTime );
        }
    }
}

void
CGuiScreenHud::UpdateTimer( unsigned int elapsedTime )
{
    const int TIMER_BLINKING_STARTING_PERIOD = 500;
    const tColour TIMER_BLINKING_COLOUR( 217, 2, 5 );

    int numMillisecondsRemaining = 0;

    Mission* currentMission = GetGameplayManager()->GetCurrentMission();
    if( currentMission != NULL )
    {
        if (m_pGetOutOfCarCondition != NULL)
        {
            //use the condition timer
            numMillisecondsRemaining = m_pGetOutOfCarCondition->GetTimeRemainingTilFailuremilliseconds();
        }
        else
        {
            numMillisecondsRemaining = currentMission->GetMissionTimeLeftInMilliSeconds();
        }
    }

    if( numMillisecondsRemaining > 1000 )
    {
        rAssert( m_timer != NULL );

        float blinkPercentage = (numMillisecondsRemaining - m_timerBlinkingStartTime) /
                                (float)(m_timerBlinkingEndTime - m_timerBlinkingStartTime);

        // blink timer if remaining time is btw. timer blinking start
        // and end time
        //
        if( blinkPercentage > 0.0f )
        {
            float blinkingPeriod = (float)TIMER_BLINKING_STARTING_PERIOD;
            if( blinkPercentage > 0.8f )
            {
                blinkingPeriod *= 0.50f;
            }
            else if( blinkPercentage > 0.5f )
            {
                blinkingPeriod *= 0.67f;
            }

            tColour currentColour;
            GuiSFX::ModulateColour( &currentColour,
                                    (float)numMillisecondsRemaining,
                                    blinkingPeriod,
                                    m_defaultTimerColour,
                                    TIMER_BLINKING_COLOUR,
                                    rmt::PI_BY2 );

            m_timer->SetColour( currentColour );

            if( m_elapsedTime[ HUD_TIMER ] >= blinkingPeriod )
            {
                GetEventManager()->TriggerEvent( EVENT_HUD_TIMER_BLINK );

                m_elapsedTime[ HUD_TIMER ] %= (int)blinkingPeriod;
            }

            m_elapsedTime[ HUD_TIMER ] += elapsedTime;
        }
        else
        {
            // restore timer's default colours
            //
            m_timer->SetColour( m_defaultTimerColour );

            m_elapsedTime[ HUD_TIMER ] = TIMER_BLINKING_STARTING_PERIOD;
        }

        int numSecondsRemaining = numMillisecondsRemaining / 1000;

        char buffer[ BITMAP_TEXT_BUFFER_SIZE ];
        sprintf( buffer, "%d:%02d", numSecondsRemaining / 60, numSecondsRemaining % 60 );

        m_timer->SetBitmapText( buffer );
    }
    else
    {
        this->HandleMessage( GUI_MSG_HIDE_HUD_OVERLAY, HUD_TIMER );
    }
}

#ifdef SRR2_MOVABLE_HUD_MAP

void
CGuiScreenHud::UpdateHudMapPosition( unsigned int elapsedTime )
{
    // update HUD map screen position
    //
    int numUserInputHandlers = GetGuiSystem()->GetNumUserInputHandlers();
    for( int i = 0; i < numUserInputHandlers; i++ )
    {
        CGuiUserInputHandler* userInputHandler = GetGuiSystem()->GetUserInputHandler( i );
        if( userInputHandler != NULL )
        {
#ifdef RAD_PS2
            if( userInputHandler->IsButtonDown( GuiInput::Back ) ) // action button (for PS2)
#else
            if( userInputHandler->IsButtonDown( GuiInput::AuxY ) ) // action button (for Xbox and GC)
#endif
            {
                static int NUM_PIXELS_PER_SECOND = 160;
                int translationAmount = (int)( (elapsedTime / 1000.0f) * (float)NUM_PIXELS_PER_SECOND );

                if( userInputHandler->IsButtonDown( GuiInput::Left ) )
                {
                    this->MoveHudMap( -translationAmount, 0 );
                }

                if( userInputHandler->IsButtonDown( GuiInput::Right ) )
                {
                    this->MoveHudMap( translationAmount, 0 );
                }

                if( userInputHandler->IsButtonDown( GuiInput::Up ) )
                {
                    this->MoveHudMap( 0, translationAmount );
                }

                if( userInputHandler->IsButtonDown( GuiInput::Down ) )
                {
                    this->MoveHudMap( 0, -translationAmount );
                }
            }
        }
    }
}

void
CGuiScreenHud::MoveHudMap( int x, int y )
{
    rAssert( m_hudMap[ 0 ] );

    // clamp HUD map within screen boundaries
    //
    int mapPosX = 0;
    int mapPosY = 0;
    int mapWidth = 0;
    int mapHeight = 0;
    m_hudMap[ 0 ]->GetPure3dObject()->GetOriginPosition( mapPosX, mapPosY );
    m_hudMap[ 0 ]->GetPure3dObject()->GetBoundingBoxSize( mapWidth, mapHeight );

    if( (mapPosX + x) < 0 ||
        (mapPosX + x) > (static_cast<int>( Scrooby::App::GetInstance()->GetScreenWidth() ) - mapWidth) )
    {
        x = 0;
    }

    if( (mapPosY + y) < 0 ||
        (mapPosY + y) > (static_cast<int>( Scrooby::App::GetInstance()->GetScreenHeight() ) - mapHeight) )
    {
        y = 0;
    }

    // move the HUD map
    //
    m_hudMap[ 0 ]->Translate( x, y );
}

#endif

void CGuiScreenHud::AbortFade()
{
    m_elapsedFadeTime = 250.0f;
    m_screenCover->SetAlpha( 0.0f );
    Update( 0 );
}

#ifdef DEBUGWATCH
const char* CGuiScreenHud::GetWatcherName() const
{
    return "CGuiScreenHud";
}
#endif