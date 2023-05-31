//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        musicplayer.cpp
//
// Description: Implement MusicPlayer class.  Plays all music in
//              the game using RadMusic.
//
// History:     17/07/2002 + Created -- Darren
//
//=============================================================================


//========================================
// Project Includes
//========================================
#include <sound/music/musicplayer.h>

#include <sound/soundmanager.h>
#include <sound/soundrenderer/idasoundtuner.h>
#include <sound/soundrenderer/dasoundgroup.h>

#include <main/commandlineoptions.h>
#include <memory/srrmemory.h>
#include <events/eventmanager.h>
#include <gameflow/gameflow.h>
#include <loading/loadingmanager.h>
#include <loading/soundfilehandler.h>
#include <mission/missionmanager.h>
#include <worldsim/character/character.h>
#include <worldsim/avatarmanager.h>
#include <meta/eventlocator.h>
#include <worldsim/character/charactermanager.h>
#include <worldsim/character/character.h>

#include <Render/Enums/RenderEnums.h>

#include <interiors/interiormanager.h>

#include <radload/radload.hpp>

//========================================
// System Includes
//========================================
#include <string.h>

#include <radmusic/radmusic.hpp>
#include <p3d/entity.hpp>


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

static const char* AMBIENT_FILE_NAME = "sound\\ambience\\ambience.rms";
static const char* LEVEL_FILE_NAME_PREFIX = "sound\\music\\L";
static const char* LEVEL_FILE_NAME_SUFFIX = "_music.rms";
static const char* MUSIC_SEARCH_PATH      = "sound\\music\\";
static const char* AMBIENCE_SEARCH_PATH   = "sound\\ambience\\";

struct InteriorIDEntry
{
    radInt64 id;
    const char* name;
    MusicEventList musicEvent;
};

static InteriorIDEntry interiorNameTable[] =
{
    { 0, "KwikEMart",             MEVENT_INTERIOR_KWIK_E_MART },
    { 0, "SpringfieldElementary", MEVENT_INTERIOR_SCHOOL      },
    { 0, "SimpsonsHouse",         MEVENT_INTERIOR_HOUSE       },
    { 0, "Krustylu",              MEVENT_INTERIOR_HOUSE       },
    { 0, "dmv",                   MEVENT_INTERIOR_DMV         }
};

static int interiorTableLength = sizeof( interiorNameTable ) / sizeof( InteriorIDEntry );

//
// Tables for mapping music/ambience events to indices into radMusic scripts
//

struct MusicScriptTableEntry
{
    const char* name;
    radKey32 nameKey;
    int scriptIndex;
};
static const int NO_INDEX = -1;

static MusicScriptTableEntry musicEventTable[] =
{
    { "movie", 0, 0 },
    { "pause", 0, 0 },
    { "unpause", 0, 0 },
    { "FE", 0, 0 },
    { "Loading_screen", 0, 0 },
    { "Newspaper_Spin", 0, 0 },
    { "SuperSprint", 0, 0 },
    { "Win_SuperSprint", 0, 0 },
    { "Lose_SuperSprint", 0, 0 },
    { "Level_Intro", 0, 0 },
    { "Sunday_Drive_start", 0, 0 },
    { "Sunday_Drive_Get_out_of_Car", 0, 0 },
    { "Store", 0, 0 },
    { "Enter_StoneCutters_Tunnel", 0, 0 },
    { "Exit_StoneCutters_Tunnel", 0, 0 },
    { "OF_explore_mission", 0, 0 },
    { "OF_found_card", 0, 0 },
    { "OF_time_out", 0, 0 },
    { "OF_Apu_Oasis", 0, 0 },
    { "OF_House", 0, 0 },
    { "OF_KiwkEMart", 0, 0 },
    { "OF_School", 0, 0 },
    { "OF_Moes", 0, 0 },
    { "OF_DMV", 0, 0 },
    { "OF_Android_Dungeon", 0, 0 },
    { "OF_Observatory", 0, 0 },
    { "Win_3Street_Races", 0, 0 },
    { "Level_Completed", 0, 0 },
    { "Destroy_Camera_Bonus", 0, 0 },
    { "StoneCutters", 0, 0 },
    { "Social_Club", 0, 0 },
    { "DuffBeer", 0, 0 },
    { "Hit_and_Run", 0, 0 },
    { "Hit_and_Run_Caught", 0, 0 },
    { "Wasp_Attack", 0, 0 },
    { "Gated_Mission", 0, 0 },
    { "ScaryMusic01", 0, 0 },
    { "Credits", 0, 0 }
};

static unsigned int musicEventTableLength = sizeof( musicEventTable ) / sizeof( MusicScriptTableEntry );

struct MissionNameScriptEntry
{
    radKey32 nameKey;
    int scriptIndex;
};
static const unsigned int MISSION_TABLE_SIZE = 81;
static MissionNameScriptEntry missionScriptTable[MISSION_TABLE_SIZE];
static unsigned int missionScriptTableLength;

static const unsigned int RACE_TABLE_SIZE = 12;
static MissionNameScriptEntry raceScriptTable[RACE_TABLE_SIZE];
static unsigned int raceScriptTableLength;

struct MatrixStateScriptEntry
{
    radKey32 stateNameKey;
    unsigned int stateIndex;
    radKey32 stateValueNameKey;
    unsigned int stateValueIndex;
};
static const unsigned int MATRIX_TABLE_SIZE = 6;
static MatrixStateScriptEntry matrixStateTable[MATRIX_TABLE_SIZE];
static unsigned int matrixStateTableLength;

// Should change to whatever the global constant is
static const int NUM_LEVELS = 7;
static const int NUM_STARTING_MISSIONS = 8;

static AmbientEventList startingAmbiences[NUM_LEVELS][NUM_STARTING_MISSIONS] =
{
    // L1
    {
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START + 
        ( LocatorEvent::AMBIENT_SOUND_SUBURBS - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START + 
        ( LocatorEvent::AMBIENT_SOUND_SUBURBS - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START + 
        ( LocatorEvent::AMBIENT_SOUND_SUBURBS - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START + 
        ( LocatorEvent::AMBIENT_SOUND_SUBURBS - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START + 
        ( LocatorEvent::AMBIENT_SOUND_PP_ROOM_2 - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START + 
        ( LocatorEvent::AMBIENT_SOUND_SUBURBS - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START + 
        ( LocatorEvent::AMBIENT_SOUND_SUBURBS - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START + 
        ( LocatorEvent::AMBIENT_SOUND_COUNTRY_HIGHWAY - LocatorEvent::AMBIENT_SOUND_CITY ) )
    },

    // L2
    {
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_CITY - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_CITY - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_CITY - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_CITY - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_CITY - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_CITY - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_CITY - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    AEVENT_INTERIOR_HOUSE  // unused
    },

    // L3
    {
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_LIGHT_CITY - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_LIGHT_CITY - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_LIGHT_CITY - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_FOREST_HIGHWAY - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_LIGHT_CITY - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_QUAY - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_QUAY - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    AEVENT_INTERIOR_HOUSE  // unused
    },

    // L4
    {
    AEVENT_INTERIOR_HOUSE,
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_COUNTRY_NIGHT - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_SUBURBS_NIGHT - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_SUBURBS_NIGHT - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_SUBURBS_NIGHT - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_SUBURBS_NIGHT - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_SUBURBS_NIGHT - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    AEVENT_INTERIOR_HOUSE  // unused
    },

    // L5
    {
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_CITY - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_CITY - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_CITY - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_CITY - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_CITY - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_CITY - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_CITY - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    AEVENT_INTERIOR_HOUSE  // unused
    },

    // L6
    {
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_PLACEHOLDER10 - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_PLACEHOLDER10 - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_SEASIDE_NIGHT - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_COUNTRY_NIGHT - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_SEASIDE_NIGHT - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_PLACEHOLDER10 - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_PLACEHOLDER10 - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    AEVENT_INTERIOR_HOUSE  // unused
    },

    // L7
    {
    AEVENT_INTERIOR_HOUSE,
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_PLACEHOLDER7 - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_HALLOWEEN1 - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    AEVENT_INTERIOR_HOUSE,
    AEVENT_INTERIOR_HOUSE,
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_PLACEHOLDER7 - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    static_cast<AmbientEventList>( AEVENT_TRIGGER_START +
        ( LocatorEvent::AMBIENT_SOUND_PLACEHOLDER7 - LocatorEvent::AMBIENT_SOUND_CITY ) ),
    AEVENT_INTERIOR_HOUSE  // unused
    }
};

static LocatorEvent::Event startingExteriorAmbiences[NUM_LEVELS] = 
{
    LocatorEvent::AMBIENT_SOUND_SUBURBS,
    LocatorEvent::AMBIENT_SOUND_CITY,
    LocatorEvent::AMBIENT_SOUND_LIGHT_CITY,
    LocatorEvent::AMBIENT_SOUND_SUBURBS_NIGHT,
    LocatorEvent::AMBIENT_SOUND_CITY,
    LocatorEvent::AMBIENT_SOUND_PLACEHOLDER10,
    LocatorEvent::AMBIENT_SOUND_HALLOWEEN1
};

static int s_PostHitAndRunTimer = -99;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// MusicPlayer::MusicPlayer
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
MusicPlayer::MusicPlayer( Sound::IDaSoundTuner& tuner ) :
    m_lastServiceTime( ::radTimeGetMilliseconds( ) ),
    m_isLoadingMusic( false ),
    m_isInCar( false ),
    m_radLoadRequest( NULL ),
    m_musicPerformance( NULL ),
    m_musicComposition( NULL ),
    m_isLoadingAmbient( false ),
    m_ambientPerformance( NULL ),
    m_ambientComposition( NULL ),
    m_currentAmbient( AEVENT_TRIGGER_START ),
    m_ambiencePlaying( false ),
    m_onApuRooftop( false ),
    m_stoneCutterSong( false ),
    m_LBCSong( false ),
    m_delayedMusicStart( false ),
    m_wasp( NULL )
{
    EventManager* eventMgr;
    unsigned int event;

    //
    // Register as an event listener
    //
    
    eventMgr = GetEventManager();

    eventMgr->AddListener( this, EVENT_GETINTOVEHICLE_END );
    eventMgr->AddListener( this, EVENT_GETOUTOFVEHICLE_END );
    eventMgr->AddListener( this, EVENT_INTERIOR_SWITCH );
    eventMgr->AddListener( this, EVENT_MISSION_DRAMA );
    eventMgr->AddListener( this, EVENT_MISSION_FAILURE );
    eventMgr->AddListener( this, EVENT_MISSION_SUCCESS );
    eventMgr->AddListener( this, EVENT_CARD_COLLECTED );
    eventMgr->AddListener( this, EVENT_FE_START_GAME_SELECTED );
    eventMgr->AddListener( this, EVENT_HIT_AND_RUN_START );
    eventMgr->AddListener( this, EVENT_HIT_AND_RUN_CAUGHT );
    eventMgr->AddListener( this, EVENT_HIT_AND_RUN_EVADED );

    //
    // Temporarily remove wasp music
    //
    //eventMgr->AddListener( this, EVENT_WASP_CHARGING );
    //eventMgr->AddListener( this, EVENT_WASP_BLOWED_UP );

    eventMgr->AddListener( this, EVENT_ACTOR_REMOVED );
    eventMgr->AddListener( this, EVENT_CHANGE_MUSIC );
    eventMgr->AddListener( this, EVENT_CHANGE_MUSIC_STATE );
    eventMgr->AddListener( this, EVENT_STAGE_TRANSITION_FAILED );
    eventMgr->AddListener( this, EVENT_COMPLETED_ALLSTREETRACES );
    eventMgr->AddListener( this, EVENT_MISSION_RESET );
    eventMgr->AddListener( this, EVENT_CHARACTER_POS_RESET );
    eventMgr->AddListener( this, EVENT_PLAY_CREDITS );
    eventMgr->AddListener( this, EVENT_PLAY_FE_MUSIC );
    eventMgr->AddListener( this, EVENT_PLAY_MUZAK );
    //eventMgr->AddListener( this, EVENT_PLAY_IDLE_MUSIC );
    eventMgr->AddListener( this, EVENT_SUPERSPRINT_WIN );
    eventMgr->AddListener( this, EVENT_SUPERSPRINT_LOSE );
    eventMgr->AddListener( this, EVENT_STOP_THE_MUSIC );

    //
    // Register all of the ambience events
    //
    for( event = EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_CITY;
            event <= EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_PLACEHOLDER16;
            ++event )
    {
        if( ( event < EVENT_LOCATOR + LocatorEvent::PARKED_BIRDS )
            || ( ( event > EVENT_LOCATOR + LocatorEvent::FAR_PLANE )
                 && ( event < EVENT_LOCATOR + LocatorEvent::GOO_DAMAGE ) )
            || ( event > EVENT_LOCATOR + LocatorEvent::TRAP ) )
        {
            eventMgr->AddListener( this, static_cast<EventEnum>(event) );
        }
    }

    //
    // Fill in the script tables with name keys
    //
    initializeTableNameKeys();
}

//==============================================================================
// MusicPlayer::~MusicPlayer
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
MusicPlayer::~MusicPlayer()
{
    //
    // Deregister from EventManager
    //
    GetEventManager()->RemoveAll( this );
    
    if( m_musicPerformance != NULL )
    {
        radmusic::performance_stop( m_musicPerformance );
        radmusic::performance_delete( & m_musicPerformance );
    }
    
    if ( m_musicComposition != NULL )
    {
        radmusic::composition_delete( & m_musicComposition );
    }

    if( m_ambientPerformance != NULL )
    {
        radmusic::performance_stop( m_ambientPerformance );
        radmusic::performance_delete( & m_ambientPerformance );
    }
    
    if ( m_ambientComposition != NULL )
    {
        radmusic::composition_delete( & m_ambientComposition );
    }
}

void MusicPlayer::TriggerMusicEvent( MusicEventList event )
{
    int scriptIndex;
    int tableIndex;

    if( !(CommandLineOptions::Get( CLO_NO_MUSIC )) )
    {
        //
        // Much necessary hack: screen out music we're tired of hearing
        //
        if( CommandLineOptions::Get( CLO_NO_AVRIL ) )
        {
            if( ( event == MEVENT_FE )
                || ( event == MEVENT_SUNDAY_DRIVE_START )
                || ( event == MEVENT_STREETRACE_START ) )
            {
                // Silence
                event = MEVENT_MOVIE;
            }
        }

        //
        // Get the script index for this event from the appropriate table
        //
        if( event < MEVENT_END_STANDARD_EVENTS )
        {
            scriptIndex = musicEventTable[event].scriptIndex;
        }
        else if( event < MEVENT_END_MISSION_EVENTS )
        {
            tableIndex = ( ( event - MEVENT_MISSION_START ) + calculateMissionIndex() );
            scriptIndex = missionScriptTable[tableIndex].scriptIndex;
        }
        else
        {
            tableIndex = ( ( event - MEVENT_STREETRACE_START ) + calculateMissionIndex() );
            scriptIndex = raceScriptTable[tableIndex].scriptIndex;
        }

        //
        // TODO: Uncomment after all the missions are set up and ready to go
        //
        //rAssertMsg( ( scriptIndex != NO_INDEX ), "Music event not found in script? (E-mail Esan, then continue)" );

        if( ( scriptIndex != NO_INDEX )
            && ( m_musicPerformance != NULL ) )
        {
            radmusic::performance_trigger_event(
                m_musicPerformance,
                scriptIndex );
        }
    }
}

void MusicPlayer::TriggerAmbientEvent( unsigned int event )
{
    unsigned int scriptLength;

    if( m_ambientPerformance == NULL )
    {
        return;
    }

    if( !(CommandLineOptions::Get( CLO_NO_MUSIC )) )
    {
        scriptLength = radmusic::performance_num_events( m_ambientPerformance );

        if( event < scriptLength )
        {
            radmusic::performance_trigger_event( m_ambientPerformance, event );
        }
        else
        {
            //
            // TODO: reinstate this after MS 17, get ambience for all interiors
            //

            //rAssertMsg( false, "Invalid ambient event received, ignored\n" );
        }
    }
}
        
//=============================================================================
// MusicPlayer::HandleEvent
//=============================================================================
// Description: Listen for when the player gets in and out of cars
//
// Parameters:  id - event ID
//              pEventData - user data, unused
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::HandleEvent( EventEnum id, void* pEventData )
{
    bool inCar;
    int interiorIndex;
    MusicStateData* musicState;
    EventLocator* pLocator = (EventLocator*)pEventData;
    Character* pCharacter = NULL;
    int levelIndex;
    int missionIndex;

    static bool s_HitAndRun = false;
    static int previousTime = 0, recentTime = 0;
    static EventEnum previousEvent = EVENT_LOCATOR;
    static EventEnum recentEvent = EVENT_LOCATOR;
    static EventLocator* previousLocator = NULL;
    static EventLocator*  recentLocator = NULL;
    static bool bSpecialCaseMusic = false;
    static MusicEventList lastSpecialMusic = MEVENT_MOVIE;  // default

    // bmc: don't pay attention to music events when in hit and run mode
    /*
    if ( s_HitAndRun )
    {
        if ( id != EVENT_HIT_AND_RUN_CAUGHT && id != EVENT_HIT_AND_RUN_EVADED && 
             id != EVENT_MISSION_FAILURE && id != EVENT_MISSION_SUCCESS && id != EVENT_MISSION_RESET )
            return;
        else
            s_HitAndRun = false;
    }
    */

    switch( id )
    {
        case EVENT_FE_START_GAME_SELECTED:
            TriggerMusicEvent( MEVENT_NEWSPAPER_SPIN );
            break;

        case EVENT_GETINTOVEHICLE_END:
            m_isInCar = true;
            
            if ( !s_HitAndRun )
            {
                if ( bSpecialCaseMusic )
                {
                    TriggerMusicEvent( lastSpecialMusic );
                }
                else
                {
                    startMusic();
                }
            }
            break;

        case EVENT_GETOUTOFVEHICLE_END:
            // bmc: have to validate that it's not an ai player getting out of the car...
            pCharacter = static_cast<Character*>( pEventData );
            if ( !pCharacter->IsNPC() )
            {
                m_isInCar = false;

                // if playing special music then we don't want to mess with it...
                if ( !bSpecialCaseMusic )
                {
                    if ( !s_HitAndRun && !musicLockedOnForStage() )
                    {
                        if( currentMissionIsSundayDrive() )
                        {
                            TriggerMusicEvent( MEVENT_SUNDAY_DRIVE_GET_OUT_OF_CAR );
                        }
                        else if( currentMissionIsRace() )
                        {
                            TriggerMusicEvent( MEVENT_STREETRACE_GET_OUT_OF_CAR );
                        }
                        else
                        {
                            TriggerMusicEvent( MEVENT_GET_OUT_OF_CAR );
                        }
                        turnAmbienceOn( m_currentAmbient );
                    }
                }
            }
            break;

        case EVENT_MISSION_RESET:
            {
                s_HitAndRun = false;
                bSpecialCaseMusic = false;
                lastSpecialMusic = MEVENT_MOVIE;
                // bmc: play music if just declined restarting of a mission
                if ( GetGameplayManager()->IsSundayDrive() &&
                    GetInteriorManager()->GetInterior() == static_cast< tUID >( 0 ) && (int)pEventData == 0 )
                {
                    startMusic();
                }

                inCar = GetAvatarManager()->GetAvatarForPlayer( 0 )->IsInCar();
                if( inCar != m_isInCar )
                {
                    m_isInCar = inCar;
                    startMusic();
                }
                else if ( inCar )
                {
                    // bmc: another hack -- if restarting a mission and we're in a car to start it then play the damn music
                    startMusic();
                }

#if defined( RAD_XBOX ) || defined( RAD_WIN32 )
                // XBox seems to like this syntax better.
                bool jumpStage = reinterpret_cast<bool>( pEventData );
#else
                bool jumpStage = (bool)( pEventData );
#endif

                if( jumpStage )
                {
                    //
                    // Make sure we're playing the right ambience
                    //
                    levelIndex = calculateLevelIndex();
                    missionIndex = GetGameplayManager()->GetCurrentMissionIndex();

                    if( startingAmbiences[levelIndex][missionIndex] == AEVENT_INTERIOR_HOUSE )
                    {
                        m_currentAmbient = AEVENT_TRIGGER_START + ( startingExteriorAmbiences[calculateLevelIndex()] - LocatorEvent::AMBIENT_SOUND_CITY );
                    }
                    else
                    {
                        m_currentAmbient = startingAmbiences[levelIndex][missionIndex];
                    }
                    if( !inCar )
                    {
                        turnAmbienceOn( startingAmbiences[levelIndex][missionIndex] );
                    }
                }
            }
            break;

        case EVENT_CHARACTER_POS_RESET:
            if ( pEventData )
                pCharacter = static_cast<Character*>( pEventData );

            if ( pCharacter && pCharacter->IsNPC() )
                break;

            inCar = GetAvatarManager()->GetAvatarForPlayer( 0 )->IsInCar();
            if( inCar != m_isInCar )
            {
                m_isInCar = inCar;
                startMusic();
            }
            
            break;

        case EVENT_INTERIOR_SWITCH:
            //
            // We sometimes get these events on loading screens.  Filter them out.
            //
            if( GetGameFlow()->GetCurrentContext() == CONTEXT_LOADING_GAMEPLAY )
            {
                break;
            }

            if(pEventData)
            {
                // switching to interior
                interiorIndex = calculateInteriorIndex( GetInteriorManager()->GetInterior() );
                turnAmbienceOn( AEVENT_INTERIOR_KWIK_E_MART + interiorIndex );
                if( !musicLockedOnForStage() )
                {
                    TriggerMusicEvent( interiorNameTable[interiorIndex].musicEvent );
                }
            }
            else if( !m_isInCar )
            {
                // switching to exterior
                turnAmbienceOn( m_currentAmbient );
                if( !musicLockedOnForStage() )
                {
                    TriggerMusicEvent( MEVENT_OF_EXPLORE_MISSION );
                }
            }
            break;

        case EVENT_MISSION_DRAMA:
            TriggerMusicEvent( MEVENT_MISSION_DRAMA );
            break;

        case EVENT_CHANGE_MUSIC:
            triggerMusicMissionEventByName( static_cast<radKey32*>( pEventData ) );
            break;

        case EVENT_CHANGE_MUSIC_STATE:
            musicState = static_cast<MusicStateData*>( pEventData );
            triggerMusicStateChange( musicState->stateKey, musicState->stateEventKey );
            break;

        case EVENT_MISSION_SUCCESS:
            s_HitAndRun = false;
            if( currentMissionIsRace() )
            {
                TriggerMusicEvent( MEVENT_STREETRACE_WIN );
            }
            else
            {
                TriggerMusicEvent( MEVENT_WIN_MISSION );
            }
            playPostMissionSounds();
            break;

        case EVENT_MISSION_FAILURE:
            s_HitAndRun = false;
            if( currentMissionIsRace() )
            {
                TriggerMusicEvent( MEVENT_STREETRACE_LOSE );
            }
            else
            {
                TriggerMusicEvent( MEVENT_LOSE_MISSION );
            }

            //playPostMissionSounds();
            break;

        case EVENT_STAGE_TRANSITION_FAILED:
            TriggerMusicEvent( MEVENT_GATED_MISSION );
            break;

        case EVENT_CARD_COLLECTED:
            //
            // If we're in the car, don't interrupt the music, we'll play
            // a sound effect instead
            //
            if( !m_isInCar )
            {
                TriggerMusicEvent( MEVENT_OF_FOUND_CARD );
            }
            break;

        case EVENT_HIT_AND_RUN_START:
            TriggerMusicEvent( MEVENT_HIT_AND_RUN_START );
            s_HitAndRun = true;
            // bmc: queueing this after the hit and run music seems to do the trick when hit and run ends...but only on evade...
            //startMusic();
            break;

        case EVENT_HIT_AND_RUN_CAUGHT:
            TriggerMusicEvent( MEVENT_HIT_AND_RUN_CAUGHT );
            s_HitAndRun = false;
            // begin counter -- (x) ticks until startMusic call...
            s_PostHitAndRunTimer = 110;
            break;

        case EVENT_HIT_AND_RUN_EVADED:
            // bmc: play nothing...
            //TriggerMusicEvent( MEVENT_SUNDAY_DRIVE_START );
            s_HitAndRun = false;
            // begin counter -- (x) ticks until startMusic call...
            s_PostHitAndRunTimer = 10;
            break;

        case EVENT_WASP_CHARGING:
            if( !m_isInCar )
            {
                m_wasp = static_cast<Actor*>( pEventData );
                TriggerMusicEvent( MEVENT_WASP_ATTACK );
            }
            break;

        case EVENT_WASP_BLOWED_UP:
        case EVENT_ACTOR_REMOVED:
            if( static_cast<Actor*>( pEventData ) == m_wasp )
            {
                TriggerMusicEvent( MEVENT_OF_EXPLORE_MISSION );

                m_wasp = NULL;
            }
            break;

        case EVENT_COMPLETED_ALLSTREETRACES:
            TriggerMusicEvent( MEVENT_WIN_3_RACES );
            break;

        case EVENT_DESTROYED_ALLCAMERAS:
            TriggerMusicEvent( MEVENT_DESTROY_CAMERA_BONUS );
            break;

        case EVENT_SUPERSPRINT_WIN:
            TriggerMusicEvent( MEVENT_SUPERSPRINT_WIN );
            break;

        case EVENT_SUPERSPRINT_LOSE:
            TriggerMusicEvent( MEVENT_SUPERSPRINT_LOSE );
            break;

        case EVENT_PLAY_CREDITS:
            TriggerMusicEvent( MEVENT_CREDITS );
            break;

        case EVENT_PLAY_FE_MUSIC:
            TriggerMusicEvent( MEVENT_FE );
            break;

        case EVENT_PLAY_MUZAK:
            TriggerMusicEvent( MEVENT_STORE );
            break;

        case EVENT_PLAY_IDLE_MUSIC:
            // bmc: do nothing here -- it fucks things over anyway
        //    TriggerMusicEvent( MEVENT_OF_TIME_OUT );
            break;

        case EVENT_STOP_THE_MUSIC:
            TriggerMusicEvent( MEVENT_MOVIE );
            break;

        default:
            //
            // This should be triggered with ambience events.  Easier to handle here
            // than writing dozens of cases above
            //
            rAssert( id >= EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_CITY );
            rAssert( id <= EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_PLACEHOLDER16 );
            rAssert( ! ( ( id >= EVENT_LOCATOR + LocatorEvent::PARKED_BIRDS ) 
                          && ( id <= EVENT_LOCATOR + LocatorEvent::FAR_PLANE ) ) );
            rAssert( ! ( ( id >= EVENT_LOCATOR + LocatorEvent::GOO_DAMAGE ) 
                         && ( id <= EVENT_LOCATOR + LocatorEvent::TRAP ) ) );

            // don't play specific music if in hit and run
            if ( s_HitAndRun || !pLocator->GetPlayerEntered() )
                break;


#ifdef RAD_DEBUG
            char temp[256];

            radmusic::performance_event_name( m_ambientPerformance, 
                AEVENT_TRIGGER_START + id - (EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_CITY),
                temp,
                256 );

            rDebugPrintf( "Hit ambient trigger for %s\n", temp );
#endif

            if( m_ambiencePlaying )
            {
                turnAmbienceOn( AEVENT_TRIGGER_START + id - (EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_CITY) );
            }
            else
            {
                m_currentAmbient = AEVENT_TRIGGER_START + id - (EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_CITY);
            }

            //
            // Icky special case
            //
            unsigned int now = ::radTimeGetMilliseconds();

            // bmc: thrashing of events when driving over transition volumes is quite hazardous...
            // implementing a system with a time threshold on it, so for e.g. when thrashing quickly 
            // between two volumes ABBAABABABA etc. will only handle B. ...however still troublesome 
            // if the player continuously drives back and forth over the volumes...may become confused 
            // as to which one to actually choose and end up with the wrong one :(

            if ( id == recentEvent )
            {
                // ignore
                id = id;
            }
            else if ( id == previousEvent && ( ( now - previousTime ) < ( 500 ) ) )
            {
                rDebugPrintf( "Sound ignoring likely thrashing event id = %d with time delay %d\n", id, now-previousTime );
                id = id;
            }
            else
            {
                if( ( id == ( EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_PLACEHOLDER9 ) )
                    && ( GetGameplayManager()->IsSundayDrive() ) )
                {
                    lastSpecialMusic = MEVENT_STONECUTTER_SONG;
                    TriggerMusicEvent( MEVENT_STONECUTTER_SONG );
                    bSpecialCaseMusic = true;
                }
                else if( ( id == ( EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_PLACEHOLDER8 ) )
                        && ( GetGameplayManager()->IsSundayDrive() ) )
                {
                    lastSpecialMusic = MEVENT_LBC_SONG;
                    TriggerMusicEvent( MEVENT_LBC_SONG );
                    bSpecialCaseMusic = true;
                }
                //else if( ( id == ( EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_STONE_CUTTER_TUNNEL ) )
                //        && ( GetGameplayManager()->IsSundayDrive() ) )
                //{
                //    TriggerMusicEvent( MEVENT_STONECUTTER_SONG );
                //    bSpecialCaseMusic = true;
                //}
                else if( ( id == ( EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_STONE_CUTTER_HALL ) )
                        && ( GetGameplayManager()->IsSundayDrive() ) )
                {
                    lastSpecialMusic = MEVENT_ENTER_STONECUTTER_TUNNEL;
                    TriggerMusicEvent( MEVENT_ENTER_STONECUTTER_TUNNEL );
                    bSpecialCaseMusic = true;
                }
                else if( ( id == ( EVENT_LOCATOR + LocatorEvent::AMBIENT_KWIK_E_MART_ROOFTOP ) ) || 
                        ( id == ( EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_PLACEHOLDER3 ) )
                        && ( GetGameplayManager()->IsSundayDrive() ) )
                {
                    lastSpecialMusic = MEVENT_APU_OASIS;
                    TriggerMusicEvent( MEVENT_APU_OASIS );
                    bSpecialCaseMusic = true;
                }
                else if( ( id == ( EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_HALLOWEEN2 ) )
                        && ( GetGameplayManager()->IsSundayDrive() ) )
                {
                    lastSpecialMusic = MEVENT_SCARYMUSIC;
                    TriggerMusicEvent( MEVENT_SCARYMUSIC );
                    bSpecialCaseMusic = true;
                }
                else if( ( id == ( EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_DUFF_EXTERIOR ) ) ||
                          ( id == ( EVENT_LOCATOR + LocatorEvent::AMBIENT_SOUND_BREWERY_NIGHT ) )
                        && ( GetGameplayManager()->IsSundayDrive() ) )
                {
                    lastSpecialMusic = MEVENT_DUFF_SONG;
                    TriggerMusicEvent( MEVENT_DUFF_SONG );
                    bSpecialCaseMusic = true;
                }
                else if( bSpecialCaseMusic )
                {
                    startMusic(); // bmc: test
                    bSpecialCaseMusic = false;
                    lastSpecialMusic = MEVENT_MOVIE;  // default
                }

                previousEvent = recentEvent;
                previousTime = recentTime;
                previousLocator = recentLocator;

                recentEvent = id;
                recentTime = now;
                recentLocator = pLocator;
            }
            break;

    }
}

//=============================================================================
// MusicPlayer::Service
//=============================================================================
// Description: Extract the composition from the radload request, create the
//      performance and hook up the composition.
//              
// Parameters:  None
//
// Return:      void 
//
//=============================================================================

void MusicPlayer::SetUpPerformance(
    radmusic::performance ** ppPerformance,
    radmusic::composition ** ppComposition,
    const char * searchPath )
{
    rAssert( NULL != ppPerformance );
    rAssert( NULL != ppComposition );
    rAssert( NULL == *ppPerformance );
    rAssert( NULL == *ppComposition );
        
    radmusic::radload_composition_adapter * compAdapter =
        radLoadFind< radmusic::radload_composition_adapter >(
            m_radLoadRequest->GetInventory( ), "my_composition" );
            
    rAssert( NULL != compAdapter );
    
    *ppComposition = compAdapter->p_composition;
    
    rAssert( NULL != *ppComposition );
    
    compAdapter->p_composition = NULL;
    
    m_radLoadRequest->Release( );
    m_radLoadRequest = NULL;
            
    if ( CommandLineOptions::Get( CLO_FIREWIRE ) )
    {
        *ppPerformance = radmusic::performance_new( *ppComposition, searchPath, radMemorySpace_Local );    
    }
    else
    {
        *ppPerformance = radmusic::performance_new( *ppComposition, searchPath );    
    }
    
    rAssert( *ppPerformance != NULL );
}
        
//=============================================================================
// MusicPlayer::Service
//=============================================================================
// Description: Service RadMusic, and check the composition loader if we're
//              still in a loading state
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::Service()
{
    HeapMgr()->PushHeap( GMA_AUDIO_PERSISTENT );

    unsigned int now = ::radTimeGetMilliseconds( );
    unsigned int dif = now - m_lastServiceTime;
    
    if ( m_musicPerformance != NULL )
    {
        radmusic::performance_update( m_musicPerformance, dif );
    }
    
    if ( m_ambientPerformance != NULL )
    {
        radmusic::performance_update( m_ambientPerformance, dif );
    }
    
    m_lastServiceTime = now;

    //
    // According to Tim, this can be changed to a synchronous
    // load on a thread using:
    //
    // radmusic::composition_new_from_file( const char * p_file_name );
    //
      
    if( m_isLoadingMusic && ( m_radLoadRequest->IsComplete( ) ) )
    {
        SetUpPerformance( & m_musicPerformance, & m_musicComposition, MUSIC_SEARCH_PATH );
         
        m_isLoadingMusic = false;

        //
        // Construct the event lookup tables
        //
        buildEventTables();

        m_loadCompleteCallback->LoadCompleted();
    }
    else if( m_isLoadingAmbient && m_radLoadRequest->IsComplete( ) )
    {
        SetUpPerformance( & m_ambientPerformance, & m_ambientComposition, AMBIENCE_SEARCH_PATH );
                
        m_isLoadingAmbient = false;

        rAssert( radmusic::performance_num_events( m_ambientPerformance ) == AEVENT_NUM_EVENTS );


        m_loadCompleteCallback->LoadCompleted();
    }

    HeapMgr()->PopHeap( GMA_AUDIO_PERSISTENT );

    // bmc: hit-and-run post music hack
    if ( s_PostHitAndRunTimer > 0 )
    {
        if ( --s_PostHitAndRunTimer == 0 )
        {        
            s_PostHitAndRunTimer = -99;
            startMusic();
        }
    }

    // My own music hack -- DE
    if( m_delayedMusicStart )
    {
        if( m_musicPerformance == NULL )
        {
            m_delayedMusicStart = false;
        }
        else
        {
            //
            // Check to see if the mission success/fail stinger is done yet
            //
            if( radmusic::performance_is_state_steady_idle( m_musicPerformance ) )
            {
                char regionName[ 64 ];
                radmusic::debug_performance_current_region_name( m_musicPerformance, regionName, 64 );

                if( strcmp( regionName, "stopped" ) == 0 )
                {
                    m_delayedMusicStart = false;
                    startMusic();
                }
            }
        }
    }
}

//=============================================================================
// MusicPlayer::QueueRadmusicScriptLoad
//=============================================================================
// Description: Queue the RadMusic composition script files in the loading
//              manager
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::QueueRadmusicScriptLoad()
{
    GetLoadingManager()->AddRequest( FILEHANDLER_SOUND, AMBIENT_FILE_NAME, GMA_LEVEL_AUDIO );

    //
    // For the front end, any music script will do right now
    //
    QueueMusicLevelLoad( RenderEnums::L1 );
}

//=============================================================================
// MusicPlayer::QueueMusicLevelLoad
//=============================================================================
// Description: Queue the music script for the given level in the loading
//              manager
//
// Parameters:  level - enumeration indicating which level we're loading
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::QueueMusicLevelLoad( RenderEnums::LevelEnum level )
{
    char musicScriptName[100];
    int levelNum = level - RenderEnums::L1 + 1;

    //TODO:  Esan are we going to play special music here?
    if ( levelNum > RenderEnums::numLevels )
    {
        levelNum -= RenderEnums::numLevels;
    }

    sprintf( musicScriptName, "%s%d%s", LEVEL_FILE_NAME_PREFIX, levelNum, LEVEL_FILE_NAME_SUFFIX );
    GetLoadingManager()->AddRequest( FILEHANDLER_SOUND, musicScriptName, GMA_LEVEL_AUDIO );
}

//=============================================================================
// MusicPlayer::LoadRadmusicScript
//=============================================================================
// Description: Load the RadMusic script
//
// Parameters:  fileHandler - completion callback object
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::LoadRadmusicScript( const char* filename, SoundFileHandler* fileHandler )
{
    rAssert( NULL == m_radLoadRequest );
    
    if( strcmp( filename, AMBIENT_FILE_NAME ) == 0 )
    {
        m_isLoadingAmbient = true;
    }
    else
    {
        UnloadRadmusicScript();
        m_isLoadingMusic = true;
    }

    m_loadCompleteCallback = fileHandler;
    
    // We have to keep the string around--problem with radload.
    
    strncpy( n_currentLoadName, filename, 64 );

    radLoadOptions options;
    options.filename  = n_currentLoadName; 
    options.allocator = GMA_MUSIC;
    
    HeapMgr()->PushHeap(GMA_MUSIC);
    radLoadInstance()->Load( & options, & m_radLoadRequest );   
    HeapMgr()->PopHeap(GMA_MUSIC);
}

//=============================================================================
// MusicPlayer::UnloadRadmusicScript
//=============================================================================
// Description: Dump the music script, if it's loaded
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::UnloadRadmusicScript()
{
    if( NULL != m_musicPerformance  )
    {
        radmusic::performance_stop( m_musicPerformance );
        radmusic::performance_delete( & m_musicPerformance );

        m_musicPerformance = NULL;
    }
    
    if ( NULL != m_musicComposition )
    {
        radmusic::composition_delete( & m_musicComposition );

        m_musicComposition = NULL;
    }
}

//=============================================================================
// MusicPlayer::OnFrontEndStart
//=============================================================================
// Description: Notify RadMusic composition that we've started the front end
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::OnFrontEndStart()
{
    TriggerMusicEvent( MEVENT_FE );
}

//=============================================================================
// MusicPlayer::OnFrontEndFinish
//=============================================================================
// Description: Notify RadMusic composition that we've finished the front end
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::OnFrontEndFinish()
{
    if( CommandLineOptions::Get( CLO_NO_MUSIC ) )
    {
        return;
    }
}

//=============================================================================
// MusicPlayer::OnGameplayStart
//=============================================================================
// Description: Notify RadMusic composition that we've started gameplay
//
// Parameters:  playerInCar - should be true if player 0 is in the car, meaning
//                            that we play music instead of ambient sound
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::OnGameplayStart( bool playerInCar )
{
    int levelIndex;
    int missionIndex;

    m_isInCar = playerInCar;

    if( !(GetGameplayManager()->IsSuperSprint()) )
    {
        TriggerMusicEvent( MEVENT_LEVEL_INTRO );
    }

    //
    // We want music playing for in-car characters.  SuperSprint is a special
    // case, since the characters can't be placed in cars until they select
    // their cars, but we want music at gameplay start anyway
    //
    if( playerInCar || GetGameplayManager()->IsSuperSprint() )
    {
        startMusic();
    }
    else
    {
        levelIndex = calculateLevelIndex();
        missionIndex = GetGameplayManager()->GetCurrentMissionIndex();

        if( startingAmbiences[levelIndex][missionIndex] == AEVENT_INTERIOR_HOUSE )
        {
            m_currentAmbient = AEVENT_TRIGGER_START + ( startingExteriorAmbiences[calculateLevelIndex()] - LocatorEvent::AMBIENT_SOUND_CITY );
        }
        else
        {
            m_currentAmbient = startingAmbiences[levelIndex][missionIndex];
        }
        turnAmbienceOn( startingAmbiences[levelIndex][missionIndex] );
    }
}

//=============================================================================
// MusicPlayer::OnGameplayFinish
//=============================================================================
// Description: Notify RadMusic composition that we've finished gameplay
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::OnGameplayFinish()
{
    TriggerMusicEvent( MEVENT_LOADING_SCREEN );

    turnAmbienceOff( AEVENT_FRONTEND );

    m_delayedMusicStart = false;
}

//=============================================================================
// MusicPlayer::OnPauseStart
//=============================================================================
// Description: Entering pause menu, pause the music
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::OnPauseStart()
{
    TriggerMusicEvent( MEVENT_PAUSE );
    turnAmbienceOff( AEVENT_PAUSE );
}

//=============================================================================
// MusicPlayer::OnPauseEnd
//=============================================================================
// Description: Leaving pause menu, restart the music
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::OnPauseEnd()
{
    TriggerMusicEvent( MEVENT_UNPAUSE );
    turnAmbienceOn( AEVENT_UNPAUSE );
}

//=============================================================================
// MusicPlayer::OnStoreStart
//=============================================================================
// Description: Triggered when we enter a reward screen, like the clothes
//              shop
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::OnStoreStart()
{
    TriggerMusicEvent( MEVENT_STORE );
}

//=============================================================================
// MusicPlayer::OnStoreEnd
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::OnStoreEnd()
{
    //
    // We actually reuse this for supersprint
    //
    if( !(GetGameplayManager()->IsSuperSprint()) )
    {
        if( GetInteriorManager()->GetInterior() == static_cast< tUID >( 0 ) )
        {
            TriggerMusicEvent( MEVENT_OF_EXPLORE_MISSION );
        }
        else
        {
            TriggerMusicEvent( interiorNameTable[calculateInteriorIndex( GetInteriorManager()->GetInterior() )].musicEvent );
        }
    }
}

//=============================================================================
// MusicPlayer::StopForMovie
//=============================================================================
// Description: Stop the music for a movie
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::StopForMovie()
{
    TriggerMusicEvent( MEVENT_PAUSE );
    turnAmbienceOff( AEVENT_PAUSE );
}

//=============================================================================
// MusicPlayer::ResumeAfterMovie
//=============================================================================
// Description: Start the FE music again after an FMV
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::ResumeAfterMovie()
{
    TriggerMusicEvent( MEVENT_UNPAUSE );
    turnAmbienceOn( AEVENT_UNPAUSE );
}

bool MusicPlayer::IsStoppedForMovie( void )
{
    char regionName[ 64 ];
    if ( !m_musicPerformance )
    {
        return true;
    }
    radmusic::debug_performance_current_region_name( m_musicPerformance, regionName, 64 );

    bool steadyIdle = radmusic::performance_is_state_steady_idle( m_musicPerformance );       

    bool paused = ( strcmp( regionName, "stopped" ) == 0 || strcmp( regionName, "pause_region" ) == 0 );

    return paused && steadyIdle;
}

//=============================================================================
// MusicPlayer::RestartSupersprintMusic
//=============================================================================
// Description: Give the supersprint music a kick
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::RestartSupersprintMusic()
{
    TriggerMusicEvent( MEVENT_SUPERSPRINT );
}

//=============================================================================
// MusicPlayer::GetVolume
//=============================================================================
// Description: Get the volume.  Duh.
//
// Parameters:  None
//
// Return:      Float value for volume 
//
//=============================================================================
float MusicPlayer::GetVolume()
{
    rAssert( NULL != m_musicPerformance );
    if ( NULL == m_musicPerformance )
    {
        return 0.0f;
    }
    else
    {
        return( radmusic::performance_volume( m_musicPerformance ) );
    }
}

//=============================================================================
// MusicPlayer::SetVolume
//=============================================================================
// Description: Set the volume.  Also duh.
//
// Parameters:  volume - new volume setting
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::SetVolume( float volume )
{
    if ( NULL != m_musicPerformance )
    {
        radmusic::performance_volume( m_musicPerformance, volume );
    }
}

//=============================================================================
// MusicPlayer::GetAmbienceVolume
//=============================================================================
// Description: Get the ambience volume.
//
// Parameters:  None
//
// Return:      Float value for volume 
//
//=============================================================================
float MusicPlayer::GetAmbienceVolume()
{
    rAssert( NULL != m_ambientPerformance );
    if ( NULL == m_ambientPerformance )
    {
        return 0.0f;
    }
    else
    {
        return( radmusic::performance_volume( m_ambientPerformance ) );
    }
}

//=============================================================================
// MusicPlayer::SetAmbienceVolume
//=============================================================================
// Description: Set the ambience volume.
//
// Parameters:  volume - new volume setting
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::SetAmbienceVolume( float volume )
{
    rAssert( NULL != m_ambientPerformance );
    if ( NULL != m_ambientPerformance )
    {
        radmusic::performance_volume( m_ambientPerformance, volume );
    }
}

//=============================================================================
// MusicPlayer::GetBeatValue
//=============================================================================
// Description: Pass on the beat from the music performance
//
// Parameters:  None
//
// Return:      float from 0.0f to 4.0f
//
//=============================================================================
float MusicPlayer::GetBeatValue()
{
    float beat = 0.0f;

    if( m_musicPerformance != NULL )
    {
        radmusic::debug_performance_current_beat( m_musicPerformance, &beat );
    }

    return( beat );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// MusicPlayer::calculateLevelIndex
//=============================================================================
// Description: Figure out an index for the current level (0 for L1, 1 for L2...)
//
// Parameters:  None
//
// Return:      Index
//
//=============================================================================
int MusicPlayer::calculateLevelIndex()
{
    RenderEnums::LevelEnum level;

    level = GetGameplayManager()->GetCurrentLevelIndex();

    return( static_cast<int>( level - RenderEnums::L1 ) );
}

//=============================================================================
// MusicPlayer::calculateMissionIndex
//=============================================================================
// Description: Figure out an index for the current mission (0 for M1, 1 for M2...).
//              Use -1 for Sunday Drive.
//
// Parameters:  None
//
// Return:      Index
//
//=============================================================================
int MusicPlayer::calculateMissionIndex()
{
    int index;
    int missionIndex;
    Mission* currentMission;
    GameplayManager* gameplayMgr = GetGameplayManager();
    
    if( gameplayMgr->IsSundayDrive() )
    {
        index = -1;
    }
    else
    {
        currentMission = gameplayMgr->GetCurrentMission();
        rAssert( currentMission != NULL );

        if( currentMission->IsRaceMission() )
        {
            index = ( gameplayMgr->GetCurrentMissionNum() - GameplayManager::MAX_MISSIONS ) * ( MEVENT_END_RACE_EVENTS - MEVENT_STREETRACE_START );
        }
        else if( currentMission->IsBonusMission() )
        {
            index = 8 * ( MEVENT_END_MISSION_EVENTS - MEVENT_MISSION_START );  // 0-7 are regular missions, 8 is bonus
        }
        else
        {
            missionIndex = gameplayMgr->GetCurrentMissionIndex();

            //
            // Stupid dummy level 1 and its special-case training mission
            //
            if( gameplayMgr->GetCurrentLevelIndex() != RenderEnums::L1 )
            {
                missionIndex += 1;
            }

            index = missionIndex * ( MEVENT_END_MISSION_EVENTS - MEVENT_MISSION_START );
        }
    }

    return( index );
}

//=============================================================================
// MusicPlayer::calculateInteriorIndex
//=============================================================================
// Description: Figure out an index for the interior being entered
//
// Parameters:  None
//
// Return:      Index starting from 0, to be added to first interior event
//              in ambient list
//
//=============================================================================
int MusicPlayer::calculateInteriorIndex( tUID interiorID )
{
    int i;
    int retVal = 2;  // Default to Simpsons house sound

    //
    // We should've be calculating this unless we're actually inside
    //
    rAssert( interiorID != static_cast< tUID >( 0 ) );

    if( interiorNameTable[0].id == static_cast< tUID >( 0 ) )
    {
        //
        // Initialize table
        //
        for( i = 0; i < interiorTableLength; i++ )
        {
            interiorNameTable[i].id = tName::MakeUID( interiorNameTable[i].name );
        }
    }

    for( i = 0; i < interiorTableLength; i++ )
    {
        if( interiorID == static_cast< tUID >( interiorNameTable[i].id ) )
        {
            retVal = i;
            break;
        }
    }

    return( retVal );
}

//=============================================================================
// MusicPlayer::musicLockedOnForStage
//=============================================================================
// Description: Checks the current mission stage to see if we need to keep
//              the music playing when we get out of the car
//
// Parameters:  None
//
// Return:      True if music stays on, false otherwise 
//
//=============================================================================
bool MusicPlayer::musicLockedOnForStage()
{
    Mission* mission;
    MissionStage* stage;
    bool musicLocked = false;

    mission = GetGameplayManager()->GetCurrentMission();
    if( mission != NULL )
    {
        stage = mission->GetCurrentStage();
        if( stage != NULL )
        {
            musicLocked = stage->GetMusicAlwaysOnFlag();
        }
    }

    return( musicLocked );
}

//=============================================================================
// MusicPlayer::startMusic
//=============================================================================
// Description: Figure out which music track to play and play it
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::startMusic()
{
    // bmc: if calling startMusic then hit and run *must* be over...clean it up
    s_PostHitAndRunTimer = -99;
    // bmc

    int mission;
    bool onFoot = false;

    mission = calculateMissionIndex();
    if( mission >= 0 )
    {
        if( currentMissionIsRace() )
        {
            TriggerMusicEvent( MEVENT_STREETRACE_START );
        }
        else if ( m_isInCar )
        {
            TriggerMusicEvent( MEVENT_MISSION_START );
        }
        else
        {
            TriggerMusicEvent( MEVENT_OF_EXPLORE_MISSION );
            onFoot = true;
        }

    }
    else if( GetGameplayManager()->IsSuperSprint() )
    {
        //
        // Play Fox's licensed music.  Fox sucks.
        //
        TriggerMusicEvent( MEVENT_SUPERSPRINT );
    }
    else if ( m_isInCar )
    {
        TriggerMusicEvent( MEVENT_SUNDAY_DRIVE_START );
    }
    else
    {
        TriggerMusicEvent( MEVENT_OF_EXPLORE_MISSION );
        onFoot = true;
    }

    if( !onFoot )
    {
        turnAmbienceOff( AEVENT_PAUSE );
    }
    else if( ( GetInteriorManager() != NULL )
             && ( GetInteriorManager()->IsInside() ) )
    {
        triggerCurrentInteriorAmbience();
    }
    else
    {
        turnAmbienceOn( m_currentAmbient );
    }
}

//=============================================================================
// MusicPlayer::playPostMissionSounds
//=============================================================================
// Description: Figure out what to play when the mission ends
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::playPostMissionSounds()
{
    if( !m_isInCar )
    {
        if( ( GetInteriorManager() != NULL )
            && ( GetInteriorManager()->IsInside() ) )
        {
            triggerCurrentInteriorAmbience();
        }
        else
        {
            turnAmbienceOn( m_currentAmbient );
        }
    }

    m_delayedMusicStart = true;
}

//=============================================================================
// MusicPlayer::turnAmbienceOn
//=============================================================================
// Description: Send the given ambience event and mark ambience as not
//              playing
//
// Parameters:  event - event to give to ambient performance object (shouldn't
//                      be pause or stop event)
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::turnAmbienceOn( unsigned int event )
{
    TriggerAmbientEvent( event );

    if( ( event != AEVENT_UNPAUSE ) && ( event <= AEVENT_TRIGGER_END ) )
    {
        m_currentAmbient = event;
    }
    m_ambiencePlaying = true;
}

//=============================================================================
// MusicPlayer::turnAmbienceOff
//=============================================================================
// Description: Send the given ambience event and mark ambience as playing
//
// Parameters:  event - event to give to ambient performance object (should
//                      be pause or stop event)
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::turnAmbienceOff( unsigned int event )
{
    rAssert( ( event == AEVENT_PAUSE ) || ( event == AEVENT_MOVIE )
             || ( event == AEVENT_FRONTEND ) );

    TriggerAmbientEvent( event );
    m_ambiencePlaying = false;
}

//=============================================================================
// MusicPlayer::currentMissionIsRace
//=============================================================================
// Description: As the name says
//
// Parameters:  None
//
// Return:      True if there's a current mission and it's a race, false
//              otherwise
//
//=============================================================================
bool MusicPlayer::currentMissionIsRace()
{
    bool retVal = false;
    Mission* theMission = GetGameplayManager()->GetCurrentMission();

    if( ( theMission != NULL ) && ( theMission->IsRaceMission() ) )
    {
        retVal = true;
    }

    return( retVal );
}

//=============================================================================
// MusicPlayer::currentMissionIsSundayDrive
//=============================================================================
// Description: As the name says
//
// Parameters:  None
//
// Return:      True if there's a current mission and it's Sunday Drive, false
//              otherwise
//
//=============================================================================
bool MusicPlayer::currentMissionIsSundayDrive()
{
    bool retVal = false;
    Mission* theMission = GetGameplayManager()->GetCurrentMission();

    if( ( theMission != NULL ) && ( theMission->IsSundayDrive() ) )
    {
        retVal = true;
    }

    return( retVal );
}

//=============================================================================
// MusicPlayer::initializeTableNameKeys
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::initializeTableNameKeys()
{
    unsigned int i;
    char temp[256];
    unsigned int tableIndex;
    char prefix[10];

    //
    // We've already got strings for the musicEventTable, just make keys
    //
    for( i = 0; i < musicEventTableLength; i++ )
    {
        musicEventTable[i].nameKey = ::radMakeCaseInsensitiveKey32( musicEventTable[i].name );
    }

    //
    // For the mission and race tables, generate strings and store the keys
    //
    tableIndex = 0;
    for( i = 0; i < 9; i++ )  // min. 8 missions per level + bonus
    {
        if( i == 8 )
        {
            strcpy( prefix, "Bonus" );
        }
        else
        {
            sprintf( prefix, "M%d", i );
        }

        sprintf( temp, "%s_start", prefix );
        missionScriptTable[tableIndex++].nameKey = ::radMakeCaseInsensitiveKey32( temp );

        sprintf( temp, "%s_drama", prefix );
        missionScriptTable[tableIndex++].nameKey = ::radMakeCaseInsensitiveKey32( temp );

        sprintf( temp, "%s_win", prefix );
        missionScriptTable[tableIndex++].nameKey = ::radMakeCaseInsensitiveKey32( temp );

        sprintf( temp, "%s_lose", prefix );
        missionScriptTable[tableIndex++].nameKey = ::radMakeCaseInsensitiveKey32( temp );

        sprintf( temp, "%s_get_out_of_car", prefix );
        missionScriptTable[tableIndex++].nameKey = ::radMakeCaseInsensitiveKey32( temp );

        sprintf( temp, "%s_10sec_to_go", prefix );
        missionScriptTable[tableIndex++].nameKey = ::radMakeCaseInsensitiveKey32( temp );
    }
    //
    // Record the length of this table
    //
    rAssert( tableIndex <= MISSION_TABLE_SIZE );
    missionScriptTableLength = tableIndex;
    
    //
    // Just to be safe
    //
    for( i = missionScriptTableLength; i < MISSION_TABLE_SIZE; i++ )
    {
        missionScriptTable[i].nameKey = 0;
        missionScriptTable[i].scriptIndex = NO_INDEX;
    }

    tableIndex = 0;
    for( i = 0; i < 3; i++ )  // 3 race missions
    {
        sprintf( temp, "StreetRace0%d_start", i+1 );
        raceScriptTable[tableIndex++].nameKey = ::radMakeCaseInsensitiveKey32( temp );

        sprintf( temp, "StreetRace0%d_win", i+1 );
        raceScriptTable[tableIndex++].nameKey = ::radMakeCaseInsensitiveKey32( temp );

        sprintf( temp, "StreetRace0%d_lose", i+1 );
        raceScriptTable[tableIndex++].nameKey = ::radMakeCaseInsensitiveKey32( temp );

        sprintf( temp, "StreetRace0%d_getoutofcar", i+1 );
        raceScriptTable[tableIndex++].nameKey = ::radMakeCaseInsensitiveKey32( temp );
    }
    //
    // Record the length of this table
    //
    rAssert( tableIndex <= RACE_TABLE_SIZE );
    raceScriptTableLength = tableIndex;

    //
    // Just to be safe one more time
    //
    for( i = raceScriptTableLength; i < RACE_TABLE_SIZE; i++ )
    {
        raceScriptTable[i].nameKey = 0;
        raceScriptTable[i].scriptIndex = NO_INDEX;
    }
}

//=============================================================================
// MusicPlayer::buildEventTables
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::buildEventTables()
{
    unsigned int i, j;
    unsigned int scriptLength;
    char temp[256];
    radKey32 tempKey;
    bool found;
    unsigned int numStates;
    unsigned int numStateEvents;
    radKey32 stateKey;
    radKey32 stateEventKey;
    unsigned int currentTableLine;

    //
    // First, clear the old values out
    //
    for( i = 0; i < musicEventTableLength; i++ )
    {
        musicEventTable[i].scriptIndex = NO_INDEX;
    }

    for( i = 0; i < MISSION_TABLE_SIZE; i++ )
    {
        missionScriptTable[i].scriptIndex = NO_INDEX;
    }

    for( i = 0; i < RACE_TABLE_SIZE; i++ )
    {
        raceScriptTable[i].scriptIndex = NO_INDEX;
    }

    for( i = 0; i < MATRIX_TABLE_SIZE; i++ )
    {
        matrixStateTable[i].stateNameKey = 0;
        matrixStateTable[i].stateIndex = NO_INDEX;
        matrixStateTable[i].stateValueNameKey = 0;
        matrixStateTable[i].stateValueIndex = NO_INDEX;
    }

    //
    // Now, go through each event in the script and search for a match
    // in the tables.  If one is found, fill in its index
    //
    scriptLength = radmusic::performance_num_events( m_musicPerformance );
    for( i = 0; i < scriptLength; i++ )
    {
        radmusic::performance_event_name( m_musicPerformance, i, temp, 256 );
        tempKey = ::radMakeCaseInsensitiveKey32( temp );

        found = false;

        //
        // Check standard music events first
        //
        for( j = 0; j < musicEventTableLength; j++ )
        {
            if( musicEventTable[j].nameKey == tempKey )
            {
                musicEventTable[j].scriptIndex = i;
                found = true;
                break;
            }
        }

        //
        // If not there, try mission events
        //
        if( !found )
        {
            for( j = 0; j < missionScriptTableLength; j++ )
            {
                if( missionScriptTable[j].nameKey == tempKey )
                {
                    missionScriptTable[j].scriptIndex = i;
                    found = true;
                    break;
                }
            }
        }

        //
        // Finally, try race events
        //
        if( !found )
        {
            for( j = 0; j < raceScriptTableLength; j++ )
            {
                if( raceScriptTable[j].nameKey == tempKey )
                {
                    raceScriptTable[j].scriptIndex = i;
                    found = true;
                    break;
                }
            }
        }

        //
        // If we get here without a match, Marc's got an event we're not using
        // anywhere yet.  Assert on this at some point once the script appears
        // stabilized.
        //
        //rAssert( found );
    }

    //
    // Now, build the matrix event table.  Go through all the states, and make
    // a table of all the state/event pairs
    //
    currentTableLine = 0;

    numStates = radmusic::performance_num_states( m_musicPerformance );
    for( i = 0; i < numStates; i++ )
    {
        radmusic::performance_state_name( m_musicPerformance, i, temp, 256 );
        stateKey = ::radMakeCaseInsensitiveKey32( temp );

        numStateEvents = radmusic::performance_num_state_values( m_musicPerformance, i );
        for( j = 0; j < numStateEvents; j++ )
        {
            radmusic::performance_state_value_name( m_musicPerformance, i, j, temp, 256 );
            stateEventKey = ::radMakeCaseInsensitiveKey32( temp );

            rAssert( currentTableLine < MATRIX_TABLE_SIZE );

            matrixStateTable[currentTableLine].stateIndex = i;
            matrixStateTable[currentTableLine].stateValueIndex = j;
            matrixStateTable[currentTableLine].stateNameKey = stateKey;
            matrixStateTable[currentTableLine++].stateValueNameKey = stateEventKey;
        }
    }

    matrixStateTableLength = currentTableLine;
}

//=============================================================================
// MusicPlayer::triggerMusicMissionEventByName
//=============================================================================
// Description: Trigger a music event by matching the key to the event names
//              in the mission event table
//
// Parameters:  key - radKey for the name of the event to trigger
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::triggerMusicMissionEventByName( radKey32* key )
{
    unsigned int i;

    rAssert( key != NULL );

    if( !(CommandLineOptions::Get( CLO_NO_MUSIC )) )
    {
        for( i = 0; i < missionScriptTableLength; i++ )
        {
            if( missionScriptTable[i].nameKey == *key )
            {
                if( missionScriptTable[i].scriptIndex != NO_INDEX )
                {
                    radmusic::performance_trigger_event( m_musicPerformance,
                                                         missionScriptTable[i].scriptIndex );
                }
                break;
            }
        }
    }
}

//=============================================================================
// MusicPlayer::triggerMusicStateChange
//=============================================================================
// Description: Change the radMusic state
//
// Parameters:  stateKey - name of state
//              stateEventKey - name of event
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::triggerMusicStateChange( radKey32 stateKey, radKey32 stateEventKey )
{
    unsigned int i;

    //
    // Look for a match in the matrix table
    //
    for( i = 0; i < matrixStateTableLength; i++ )
    {
        if( ( matrixStateTable[i].stateNameKey == stateKey )
            && ( matrixStateTable[i].stateValueNameKey == stateEventKey ) )
        {
            //
            // Match, trigger the state change
            //
            radmusic::performance_state_value( m_musicPerformance,
                                               matrixStateTable[i].stateIndex,
                                               matrixStateTable[i].stateValueIndex );

            //
            // We need to trigger an event to prompt the change.  Presumably
            // we do this only for in-car stuff.
            //
            if( m_isInCar )
            {
                TriggerMusicEvent( MEVENT_MISSION_START );
            }
            break;
        }
    }

    if( i == matrixStateTableLength )
    {
        rAssertMsg( false, "Mission script calling non-existent radMusic state" );
    }
}

//=============================================================================
// MusicPlayer::triggerCurrentInteriorAmbience
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void MusicPlayer::triggerCurrentInteriorAmbience()
{
    int interiorIndex;

    interiorIndex = calculateInteriorIndex( GetInteriorManager()->GetInterior() );
    turnAmbienceOn( AEVENT_INTERIOR_KWIK_E_MART + interiorIndex );
}