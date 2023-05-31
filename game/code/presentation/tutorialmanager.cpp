//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        presentation.cpp
//
// Description: Implement PresentationManager
//
// History:     16/04/2002 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <data/gamedatamanager.h>
#include <events/eventmanager.h>
#include <gameflow/gameflow.h>
#include <main/commandlineoptions.h>
#include <memory/srrmemory.h>
#include <meta/locatorevents.h>
#include <mission/mission.h>
#include <mission/missionmanager.h>
#include <mission/gameplaymanager.h>
#include <mission/objectives/missionobjective.h>
#include <presentation/gui/guisystem.h>
#include <presentation/gui/ingame/guimanageringame.h>
#include <presentation/gui/ingame/guiscreenhud.h>
#include <presentation/tutorialmanager.h>
#include <presentation/tutorialmode.h>
#include <worldsim/redbrick/vehicle.h>

#include <string.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

// Static pointer to instance of singleton.
TutorialManager* TutorialManager::spInstance = NULL;
//#define NUMBER_OF_MISSION_OBJECTIVE_MESSAGES 300

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//=============================================================================
// TutorialManager::CreateInstance
//=============================================================================
// Description: creats an instance of the singleton
//
// Parameters:  ()
//
// Return:      TutorialManager
//
//=============================================================================
TutorialManager* TutorialManager::CreateInstance()
{
    MEMTRACK_PUSH_GROUP( "TutorialManager" );
    if( spInstance == NULL )
    {
        spInstance = new TutorialManager;
        rAssert( spInstance );
    }
    size_t size = sizeof( TutorialManager );
    MEMTRACK_POP_GROUP( "TutorialManager" );
    return spInstance;
}

//=============================================================================
// TutorialManager::GetInstance
//=============================================================================
// Description: gets the instance of the singleton
//
// Parameters:  ()
//
// Return:      TutorialManager
//
//=============================================================================
TutorialManager* TutorialManager::GetInstance()
{
    return spInstance;
}

//=============================================================================
// TutorialManager::AddToQueue
//=============================================================================
// Description: adds an item to the queue of events to process
//
// Parameters:  event - the tutorial event that we want to show as soon as 
//                      possible
//
// Return:      void 
//
//=============================================================================
void TutorialManager::AddToQueue( TutorialMode event )
{
    m_Queue.push_back( event );
}

//=============================================================================
// TutorialManager::DestroyInstance
//=============================================================================
// Description: destroys the only instance of the singleton
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void TutorialManager::DestroyInstance()
{
    if( spInstance != NULL )
    {
        delete spInstance;
        spInstance = NULL;
    }
}

//==============================================================================
// TutorialManager::TutorialManager
//==============================================================================
// Description: Constructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
TutorialManager::TutorialManager() :
    m_EnableTutorialMode( false ),
    m_EnableTutorialEvents( true ),
    m_DialogCurrentlyPlaying( false ),
    m_TimeSinceDialogStart( 0.0f ),
    m_tutorialsSeen( 0 )
{
#ifndef FINAL
    // We're going to be passing enums in void* messages - they'd better be the
    // same size or else we're in big trouble
    size_t sizeOfEnum = sizeof( TutorialMode );
    size_t sizeOfVoid = sizeof( void* );
    rReleaseAssert( sizeOfEnum == sizeOfVoid );
#endif
    m_Queue.reserve( 16 );

    GetGameDataManager()->RegisterGameData( this, 1 + sizeof( m_tutorialsSeen ), "Tutorial Manager" );

    if( CommandLineOptions::Get( CLO_NO_TUTORIAL ) )
    {
        m_EnableTutorialEvents = false;
    }
}

//==============================================================================
// TutorialManager::~TutorialManager
//==============================================================================
// Description: Destructor.
//
// Parameters: None.
//
// Return:      N/A.
//
//==============================================================================
TutorialManager::~TutorialManager()
{
    GetEventManager()->RemoveAll( this );
}

//==============================================================================
// TutorialManager::HandleEvent
//==============================================================================
// Description: Handles events that are passed to this listener
//
// Parameters:  id - the event id
//              pEventData - any data that tags along for the ride
//
// Return:      N/A.
//
//==============================================================================
void TutorialManager::HandleEvent( EventEnum id, void* pEventData )
{
    if( id == EVENT_TUTORIAL_DIALOG_DONE )
    {
        MarkDialogFinished();

        return;
    }

    if( !m_EnableTutorialMode || !m_EnableTutorialEvents )
    {
        return;
    }

    switch( id )
    {
        case EVENT_CARD_COLLECTED:
        {
            AddToQueue( TUTORIAL_COLLECTOR_CARD );
            GetEventManager()->RemoveListener( this, EVENT_CARD_COLLECTED );
            break;
        }
        case EVENT_COLLECTED_COINS:
        {
            AddToQueue( TUTORIAL_COLLECTOR_COIN );
            GetEventManager()->RemoveListener( this, EVENT_COLLECTED_COINS );
            break;
        }
        case static_cast< EventEnum >( EVENT_LOCATOR + LocatorEvent::INTERIOR_ENTRANCE ):
        {
            AddToQueue( TUTORIAL_INTERIOR_ENTERED );
            GetEventManager()->RemoveListener( this, static_cast< EventEnum >( EVENT_LOCATOR + LocatorEvent::INTERIOR_ENTRANCE ) );
            break;
        }
        case static_cast< EventEnum >( EVENT_LOCATOR + LocatorEvent::CAR_DOOR ):
        {
            AddToQueue( TUTORIAL_AT_CAR_DOOR );
            GetEventManager()->RemoveListener( this, static_cast< EventEnum >( EVENT_LOCATOR + LocatorEvent::CAR_DOOR ) );
            break;
        }
        case static_cast< EventEnum >( EVENT_INTERACTIVE_GAG ):
        {
            AddToQueue( TUTORIAL_INTERACTIVE_GAG );
            GetEventManager()->RemoveListener( this, static_cast< EventEnum >( EVENT_INTERACTIVE_GAG ) );
            break;
        }
        case EVENT_ENTERING_PLAYER_CAR:
        {
            AddToQueue( TUTORIAL_GETTING_INTO_PLAYER_CAR );
            GetEventManager()->RemoveListener( this, EVENT_ENTERING_PLAYER_CAR );
    	    break;
        }
        case EVENT_ENTERING_TRAFFIC_CAR:
        {
            //
            // Have we played the player car tutorial message yet?
            //
            bool seenDrivingTutorial = QueryTutorialSeen( TUTORIAL_GETTING_INTO_PLAYER_CAR );
            if( seenDrivingTutorial )
            {
                AddToQueue( TUTORIAL_GETTING_INTO_TRAFFIC_CAR );
                GetEventManager()->RemoveListener( this, EVENT_ENTERING_TRAFFIC_CAR );
            }
            else
            {
                 AddToQueue( TUTORIAL_GETTING_INTO_PLAYER_CAR );
                 SetTutorialSeen( TUTORIAL_GETTING_INTO_TRAFFIC_CAR, true );
                 GetEventManager()->RemoveListener( this, EVENT_ENTERING_TRAFFIC_CAR );
                 GetEventManager()->RemoveListener( this, EVENT_ENTERING_PLAYER_CAR );
            }
            break;
        }
        case EVENT_ANIMATED_CAM_SHUTDOWN:
        {
            AddToQueue( TUTORIAL_START_GAME );
            GetEventManager()->RemoveListener( this, EVENT_ANIMATED_CAM_SHUTDOWN );
            break;
        }
        case EVENT_BONUS_MISSION_CHARACTER_APPROACHED:
        {
            int missionNum = reinterpret_cast< int >( pEventData );
            Mission* mission = GetMissionManager()->GetMission( missionNum );
            bool raceMission = mission->IsRaceMission();
            bool wagerMission = mission->IsWagerMission();

            if( wagerMission )
            {
                AddToQueue( TUTORIAL_WAGER_MISSION );
            }
            else if( raceMission ) 
            {
                AddToQueue( TUTORIAL_RACE );
            }
            else
            {
                AddToQueue( TUTORIAL_BONUS_MISSION );
            }

            bool seenRace  = QueryTutorialSeen( TUTORIAL_RACE );
            bool seenBonus = QueryTutorialSeen( TUTORIAL_BONUS_MISSION );
            bool seenWager = QueryTutorialSeen( TUTORIAL_WAGER_MISSION );
            if( seenRace && seenBonus && seenWager )
            {
                GetEventManager()->RemoveListener( this, EVENT_MISSION_START );
            }
            break;
        }
        case EVENT_HIT_BREAKABLE:
        {
            AddToQueue( TUTORIAL_BREAKABLE_DESTROYED );
            GetEventManager()->RemoveListener( this, EVENT_HIT_BREAKABLE );
            break;
        }
        case EVENT_UNLOCKED_CAR:
        {
            AddToQueue( TUTORIAL_UNLOCKED_CAR );
            GetEventManager()->RemoveListener( this, EVENT_UNLOCKED_CAR );
            break;
        }
        case EVENT_VEHICLE_DESTROYED:
        {
            //
            // Check if this is a user vehicle
            //
            Vehicle* vehicle = reinterpret_cast< Vehicle* >( pEventData );
            rAssert( vehicle != NULL );
            if( vehicle->mVehicleType == VT_USER )
            {
                AddToQueue( TUTORIAL_VEHICLE_DESTROYED );
                GetEventManager()->RemoveListener( this, EVENT_VEHICLE_DESTROYED );
            }
            break;
        }
        case EVENT_COLLECTED_WRENCH:
        {
            AddToQueue( TUTORIAL_WRENCH );
            GetEventManager()->RemoveListener( this, EVENT_COLLECTED_WRENCH );
            break;
        }
        case EVENT_WASP_APPROACHED:
        {
            AddToQueue( TUTORIAL_BREAK_CAMERA );
            GetEventManager()->RemoveListener( this, EVENT_WASP_APPROACHED );
            break;
        }
        default:
        {
            rAssertMsg( false, "TutorialManager: why are we registered for messages we don't care about?" );
            break;
        }
    }

//    ProcessQueue();
}

//==============================================================================
// TutorialManager::Initialize
//==============================================================================
// Description: Initializes this manager as a listener to various event types
//
// Parameters:  None
//
// Return:      N/A.
//
//==============================================================================
void TutorialManager::Initialize()
{
    while( !m_Queue.empty() )
    {
        m_Queue.pop_back();
    }

    GetEventManager()->AddListener( this, EVENT_CARD_COLLECTED                );
    GetEventManager()->AddListener( this, EVENT_COLLECTED_COINS               );
    GetEventManager()->AddListener( this, EVENT_COLLECTED_WRENCH              );
    GetEventManager()->AddListener( this, static_cast< EventEnum >( EVENT_LOCATOR + LocatorEvent::INTERIOR_ENTRANCE ) );
    GetEventManager()->AddListener( this, static_cast< EventEnum >( EVENT_LOCATOR + LocatorEvent::CAR_DOOR          ) );
    GetEventManager()->AddListener( this, EVENT_INTERACTIVE_GAG               );
    GetEventManager()->AddListener( this, EVENT_ENTERING_PLAYER_CAR           );
    GetEventManager()->AddListener( this, EVENT_ENTERING_TRAFFIC_CAR          );
    GetEventManager()->AddListener( this, EVENT_ANIMATED_CAM_SHUTDOWN         );
    GetEventManager()->AddListener( this, EVENT_BONUS_MISSION_CHARACTER_APPROACHED );
    GetEventManager()->AddListener( this, EVENT_HIT_BREAKABLE              );
    GetEventManager()->AddListener( this, EVENT_TUTORIAL_DIALOG_DONE          );
    GetEventManager()->AddListener( this, EVENT_UNLOCKED_CAR                  );
    GetEventManager()->AddListener( this, EVENT_VEHICLE_DESTROYED             );
    GetEventManager()->AddListener( this, EVENT_WASP_APPROACHED               );
}

//==============================================================================
// TutorialManager::MarkDialogFinished
//==============================================================================
// Description: when a piece of dialog is finished playing, you may need to play
//              another
//
// Parameters:  None
//
// Return:      N/A.
//
//==============================================================================
void TutorialManager::MarkDialogFinished()
{
/*
    CGuiScreenHud* hud = GetCurrentHud();
    if( hud != NULL )
    {
        hud->TutorialBitmapInitOutro();
    }
*/
    m_DialogCurrentlyPlaying = false;
//    ProcessQueue();
}

//==============================================================================
// TutorialManager::ProcessQueue
//==============================================================================
// Description: takes an item out of the queue, and processes it
//
// Parameters:  None
//
// Return:      N/A.
//
//==============================================================================
void TutorialManager::ProcessQueue()
{
/*
    CGuiScreenHud* hud = GetCurrentHud();
    if( hud == NULL )
    {
        return;
    }
*/
    if( m_DialogCurrentlyPlaying )
    {
        return;
    }

    //
    // return if there's nothing in the queue
    //
    size_t size = m_Queue.size();
    if( size <= 0 )
    {
        return;
    }

    TutorialMode event = m_Queue.front();

    //
    // Check if the event is marked in the character sheet as "already played"
    //
    bool alreadySeen = this->QueryTutorialSeen( event );
    if( !alreadySeen )
    {
        //
        // Make sure we never see this message again
        //
        this->SetTutorialSeen( event, true );

#ifndef RAD_WIN32
        switch( event )
        {
        case TUTORIAL_BREAK_CAMERA:
            {
                m_DialogCurrentlyPlaying = true;
                m_TimeSinceDialogStart = 0;
                TutorialMode mode = TUTORIAL_BREAK_CAMERA;
                GetEventManager()->TriggerEvent( EVENT_TUTORIAL_DIALOG_PLAY, &mode );
                break;
            }
        case TUTORIAL_BONUS_MISSION:
            {
                m_DialogCurrentlyPlaying = true;
                m_TimeSinceDialogStart = 0;
                TutorialMode mode = TUTORIAL_BONUS_MISSION;
                GetEventManager()->TriggerEvent( EVENT_TUTORIAL_DIALOG_PLAY, &mode );
                break;
            }
        case TUTORIAL_BREAKABLE_DESTROYED:
            {
                m_DialogCurrentlyPlaying = true;
                m_TimeSinceDialogStart = 0;
                TutorialMode mode = TUTORIAL_BREAKABLE_DESTROYED;
                GetEventManager()->TriggerEvent( EVENT_TUTORIAL_DIALOG_PLAY, &mode );
                break;
            }
        case TUTORIAL_COLLECTOR_CARD: 
            {
                m_DialogCurrentlyPlaying = true;
                m_TimeSinceDialogStart = 0;
                TutorialMode mode = TUTORIAL_COLLECTOR_CARD;
                GetEventManager()->TriggerEvent( EVENT_TUTORIAL_DIALOG_PLAY, &mode );
                break;
            }
        case TUTORIAL_COLLECTOR_COIN:
            {
                m_DialogCurrentlyPlaying = true;
                m_TimeSinceDialogStart = 0;
                TutorialMode mode = TUTORIAL_COLLECTOR_COIN;
                GetEventManager()->TriggerEvent( EVENT_TUTORIAL_DIALOG_PLAY, &mode );
                break;
            }
        case TUTORIAL_INTERACTIVE_GAG:
            {
                m_DialogCurrentlyPlaying = true;
                m_TimeSinceDialogStart = 0;
                TutorialMode mode = TUTORIAL_INTERACTIVE_GAG;
                GetEventManager()->TriggerEvent( EVENT_TUTORIAL_DIALOG_PLAY, &mode );
                break;
            }
        case TUTORIAL_INTERIOR_ENTERED:
            {
                m_DialogCurrentlyPlaying = true;
                m_TimeSinceDialogStart = 0;
                TutorialMode mode = TUTORIAL_INTERIOR_ENTERED;
                GetEventManager()->TriggerEvent( EVENT_TUTORIAL_DIALOG_PLAY, &mode);
                break;
            }
        case TUTORIAL_GETTING_INTO_PLAYER_CAR:
            {
                m_DialogCurrentlyPlaying = true;
                m_TimeSinceDialogStart = 0;
                TutorialMode mode = TUTORIAL_GETTING_INTO_PLAYER_CAR;
                GetEventManager()->TriggerEvent( EVENT_TUTORIAL_DIALOG_PLAY, &mode );
                break;
            }
        case TUTORIAL_GETTING_INTO_TRAFFIC_CAR:
            {
                m_DialogCurrentlyPlaying = true;
                m_TimeSinceDialogStart = 0;
                TutorialMode mode = TUTORIAL_GETTING_INTO_TRAFFIC_CAR;
                GetEventManager()->TriggerEvent( EVENT_TUTORIAL_DIALOG_PLAY, &mode );
                break;
            }
        case TUTORIAL_GETTING_OUT_OF_CAR:
            {
                m_DialogCurrentlyPlaying = true;
                m_TimeSinceDialogStart = 0;
                TutorialMode mode = TUTORIAL_GETTING_OUT_OF_CAR;
                GetEventManager()->TriggerEvent( EVENT_TUTORIAL_DIALOG_PLAY, &mode );
                break;
            }
        case TUTORIAL_RACE:
            {
                m_DialogCurrentlyPlaying = true;
                m_TimeSinceDialogStart = 0;
                TutorialMode mode = TUTORIAL_RACE;
                GetEventManager()->TriggerEvent( EVENT_TUTORIAL_DIALOG_PLAY, &mode );
                break;
            }
        case TUTORIAL_START_GAME:
            {
                m_DialogCurrentlyPlaying = true;
                m_TimeSinceDialogStart = 0;
                TutorialMode mode = TUTORIAL_START_GAME;
                GetEventManager()->TriggerEvent( EVENT_TUTORIAL_DIALOG_PLAY, &mode );
                break;
            }
        case TUTORIAL_VEHICLE_DESTROYED:
            {
                m_DialogCurrentlyPlaying = true;
                m_TimeSinceDialogStart = 0;
                TutorialMode mode = TUTORIAL_VEHICLE_DESTROYED;
                GetEventManager()->TriggerEvent( EVENT_TUTORIAL_DIALOG_PLAY, &mode );
                break;
            }
        case TUTORIAL_REWARD:
            {
                m_DialogCurrentlyPlaying = true;
                m_TimeSinceDialogStart = 0;
                TutorialMode mode = TUTORIAL_REWARD;
                GetEventManager()->TriggerEvent( EVENT_TUTORIAL_DIALOG_PLAY, &mode );
                break;
            }
        case TUTORIAL_UNLOCKED_CAR:
            {
                m_DialogCurrentlyPlaying = true;
                m_TimeSinceDialogStart = 0;
                TutorialMode mode = TUTORIAL_UNLOCKED_CAR;
                GetEventManager()->TriggerEvent( EVENT_TUTORIAL_DIALOG_PLAY, &mode );
                break;
            }
        case TUTORIAL_WAGER_MISSION:
            {
                m_DialogCurrentlyPlaying = true;
                m_TimeSinceDialogStart = 0;
                TutorialMode mode = TUTORIAL_WAGER_MISSION;
                GetEventManager()->TriggerEvent( EVENT_TUTORIAL_DIALOG_PLAY, &mode );
                break;
            }
        case TUTORIAL_WRENCH:
            {
                m_DialogCurrentlyPlaying = true;
                m_TimeSinceDialogStart = 0;
                TutorialMode mode = TUTORIAL_WRENCH;
                GetEventManager()->TriggerEvent( EVENT_TUTORIAL_DIALOG_PLAY, &mode );
                break;
            }
        case TUTORIAL_AT_CAR_DOOR:
            {
                m_DialogCurrentlyPlaying = true;
                m_TimeSinceDialogStart = 0;
                TutorialMode mode = TUTORIAL_AT_CAR_DOOR;
                GetEventManager()->TriggerEvent( EVENT_TUTORIAL_DIALOG_PLAY, &mode );
                break;
            }
        case TUTORIAL_INTERACTIVE_GAG_APPROACHED:
            {
                m_DialogCurrentlyPlaying = true;
                m_TimeSinceDialogStart = 0;
                TutorialMode mode = TUTORIAL_INTERACTIVE_GAG_APPROACHED;
                GetEventManager()->TriggerEvent( EVENT_TUTORIAL_DIALOG_PLAY, &mode );
                break;
            }
        default:
            {
                rAssertMsg( false, "Why don't we process this message" );
                return;

                break;
            }
        }
#endif

//        hud->SetTutorialMessage( event );
//        hud->TutorialBitmapShow();

        GetGuiSystem()->GotoScreen( CGuiWindow::GUI_SCREEN_ID_TUTORIAL, 0, 0,
                                    CLEAR_WINDOW_HISTORY | FORCE_WINDOW_CHANGE_IMMEDIATE );

#ifdef RAD_WIN32
        m_DialogCurrentlyPlaying = false;
#endif
        GetGameFlow()->SetContext( CONTEXT_PAUSE );

    }

    //
    // Pop an item off the queue
    //
    m_Queue.erase( m_Queue.begin() );
}

//==============================================================================
// TutorialManager::Update
//==============================================================================
// Description: takes an item out of the queue, and processes it
//
// Parameters:  None
//
// Return:      N/A.
//
//==============================================================================
void TutorialManager::Update( const float deltaT )
{
    CGuiScreenHud* currentHud = GetCurrentHud();
    if( currentHud != NULL && currentHud->IsActive() )
    {
        // only update tutorial manager if the HUD is currently active
        //
//        m_TimeSinceDialogStart += deltaT;
//        if( m_TimeSinceDialogStart > 10000.0f )
        {
            m_TimeSinceDialogStart = 0;
            m_DialogCurrentlyPlaying = false;
/*
            CGuiScreenHud* hud = GetCurrentHud();
            if( hud != NULL )
            {
                hud->TutorialBitmapInitOutro();
            }
*/
        }

        ProcessQueue();
    }
}

void
TutorialManager::LoadData( const GameDataByte* dataBuffer, unsigned int numBytes )
{
    m_EnableTutorialEvents = ( dataBuffer[ 0 ] != 0 );

    memcpy( &m_tutorialsSeen, dataBuffer + 1, sizeof( m_tutorialsSeen ) );
}

void
TutorialManager::SaveData( GameDataByte* dataBuffer, unsigned int numBytes )
{
    dataBuffer[ 0 ] = m_EnableTutorialEvents ? ~0 : 0;

    memcpy( dataBuffer + 1, &m_tutorialsSeen, sizeof( m_tutorialsSeen ) );
}

void
TutorialManager::ResetData()
{
#ifdef RAD_WIN32

    if( !GetInputManager()->GetController(0)->IsTutorialDisabled() )
    {
#endif       
        m_EnableTutorialEvents = true;
        m_tutorialsSeen = 0;
#ifdef RAD_WIN32
    }
#endif 

#ifndef FINAL
    if( CommandLineOptions::Get( CLO_NO_TUTORIAL ) )
    {
        m_EnableTutorialEvents = false;
    }
#endif

    // re-add event listeners
    //
    GetEventManager()->RemoveAll( this );
    this->Initialize();
}

//=============================================================================
// TutorialManager::QueryTutorialSeen
//=============================================================================
// Description: checks to see if a given tutorial has already been played
//
// Parameters:  tutorial - enum of the tutorial in question
//
// Return:      bool - has the tutorial been shown
//
//=============================================================================
bool TutorialManager::QueryTutorialSeen( const TutorialMode tutorial )
{
    return (m_tutorialsSeen & (1 << tutorial)) > 0;

}

//=============================================================================
// TutorialManager::SetTutorialSeen
//=============================================================================
// Description: sets the status of a specific tutorial
//
// Parameters:  tutorial - enum of the tutorial in question
//
// Return:      bool - has the tutorial been shown
//
//=============================================================================
void TutorialManager::SetTutorialSeen( const TutorialMode tutorial, const bool seen )
{
    if ( seen )
    {
        m_tutorialsSeen |= (1 << tutorial);
    }
    else
    {
        m_tutorialsSeen &= ~(1 << tutorial);
    }
}

