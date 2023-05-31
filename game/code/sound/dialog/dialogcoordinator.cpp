//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        dialogcoordinator.cpp
//
// Description: The main interface class for the dialog system.  It listens 
//              for game events and cues the appropriate line of dialog.
//              Objects representing lines or sets of lines are obtained 
//              from the SpeechCoordinator, and are passed to the 
//              DialogPriorityQueue, which determines when playback is 
//              ready to begin.
//
// History:     30/08/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <sound/dialog/dialogcoordinator.h>

#include <sound/dialog/dialoglist.h>
#include <sound/dialog/dialogpriorityqueue.h>
#include <sound/dialog/dialogline.h>

#include <memory/srrmemory.h>
#include <events/eventmanager.h>
#include <events/eventdata.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/character/charactermanager.h>
#include <mission/objectives/missionobjective.h>
#include <mission/conditions/missioncondition.h>
#include <gameflow/gameflow.h>



//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

struct TutorialConversationData
{
    const char* convName;
    radKey32 convNameKey;
    bool platformSpecificLine;
};

//
// IMPORTANT: needs to line up with TutorialMode enumeration in tutorialmode.h.
// Not ideal, but we're three days from alpha.
//
TutorialConversationData tutorialConvNames[] =
{
    { "camera", 0, false },
    { "bonus", 0, false },
    { "start", 0, true },
    { "drive", 0, true },
    { "traffic", 0, false },
    { "gag", 0, false },
    { "race", 0, false },
    { "card", 0, false },
    { "coin", 0, false },
    { "reward", 0, true },
    { "outcar", 0, true },
    { "break", 0, true },
    { "broken", 0, false },
    { "interior", 0, true },
    { "unknown", 0, false },
    { "damaged", 0, false },
    { "Gil", 0, false },
    { "wrench", 0, false },
    { "incar", 0, true },
    { "unknown", 0, false },
    { "timetrial", 0, false }
};

static unsigned int tutorialTableLength = sizeof( tutorialConvNames ) / sizeof( TutorialConversationData );

static tUID genericTrafficUIDs[] =
{
    tEntity::MakeUID( "traffic1" ),
    tEntity::MakeUID( "traffic2" ),
    tEntity::MakeUID( "traffic3" ),
    tEntity::MakeUID( "traffic4" )
};

static radKey32 s_failDialog1 = ::radMakeKey32( "fail1" );
static radKey32 s_failDialog2 = ::radMakeKey32( "fail2" );

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// DialogCoordinator::DialogCoordinator
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
DialogCoordinator::DialogCoordinator( IRadNameSpace* namespaceObj ) :
    m_dialogNamespace( namespaceObj ),
    m_dialogList( new( GMA_PERSISTENT ) DialogList() ),
    m_playbackQueue( new( GMA_PERSISTENT ) DialogPriorityQueue() ),
    m_dialogOn( true ),
    m_phoneBoothRequestMade( false )
{
    unsigned int i;
    char buffer[50];

    rAssert( m_dialogList != NULL );
    rAssert( m_dialogNamespace != NULL );
    rAssert( m_playbackQueue != NULL );

    //
    // Initialize the tutorial dialog table
    //
    for( i = 0; i < tutorialTableLength; i++ )
    {
        if( tutorialConvNames[i].platformSpecificLine )
        {
#ifdef RAD_PS2
            sprintf( buffer, "%sps2", tutorialConvNames[i].convName );
#elif RAD_GAMECUBE
            sprintf( buffer, "%sngc", tutorialConvNames[i].convName );
#else
            sprintf( buffer, "%sxbx", tutorialConvNames[i].convName );
#endif

            tutorialConvNames[i].convNameKey = ::radMakeKey32( buffer );
        }
        else
        {
            tutorialConvNames[i].convNameKey = ::radMakeKey32( tutorialConvNames[i].convName );
        }
    }
}

//==============================================================================
// DialogCoordinator::~DialogCoordinator
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
DialogCoordinator::~DialogCoordinator()
{
    GetEventManager()->RemoveAll( this );
}

//=============================================================================
// DialogCoordinator::HandleEvent
//=============================================================================
// Description: Pass the event to the DialogList, which will find a suitable
//              SelectableDialog for us, and give it to the priority queue
//              for playback.
//
// Parameters:  id - event ID
//              pEventData - user data, unused here
//
// Return:      void 
//
//=============================================================================
void DialogCoordinator::HandleEvent( EventEnum id, void* pEventData )
{
    unsigned int tutorialIndex;
    SelectableDialog* dialog;
    DialogEventData* eventData;
    DialogEventData raceEventData;
    bool dialogWasPlaying;
    AvatarManager* avatarMgr;
    Vehicle* vehicleEventData = NULL;
    Vehicle* minigameVehicle;
    Vehicle* avatarVehicle;
    tUID charUID1 = 0;
    tUID charUID2 = 0;
    int i;
    int numPlayers;
    const char* charName;
    rmt::Vector dlgPosition;
    rmt::Vector* dlgPositionPtr = NULL;
    radKey32 convName = 0;
    Character* character1 = NULL;
    Character* character2 = NULL;
    Avatar* playerAvatar = GetAvatarManager()->GetAvatarForPlayer( 0 );
    bool overridePositional = false;
    int coinToss;

    if( GetGameplayManager() != NULL )
    {
        //
        // Filter out the crap we don't want in supersprint
        //
        if( GetGameplayManager()->IsSuperSprint()
            && ( id != EVENT_BIG_VEHICLE_CRASH ) 
            && ( id != EVENT_BIG_BOOM_SOUND )
            && ( id != EVENT_DEATH_VOLUME_SOUND ) )
        {
            return;
        }

        //
        // Filter out the crap we don't want in race missions
        //
        if( ( GetGameplayManager()->GetCurrentMission() != NULL )
            && ( GetGameplayManager()->GetCurrentMission()->IsRaceMission() ) )
        {
            if( id == EVENT_MISSION_FAILURE )
            {
                const char* modelName;

                //
                // Hack!
                // Turn this into a Patty/walker failure conversation.
                // Randomly choose between fail1 and fail2.
                //
                rAssert( playerAvatar != NULL );
                modelName = GetCharacterManager()->GetModelName( playerAvatar->GetCharacter() );
                raceEventData.charUID1 = tEntity::MakeUID( modelName );
                raceEventData.charUID2 = tEntity::MakeUID( "patty" );
                if( rand() % 2 == 0 )
                {
                    raceEventData.dialogName = s_failDialog1;
                }
                else
                {
                    raceEventData.dialogName = s_failDialog2;
                }

                pEventData = &raceEventData;
                id = EVENT_IN_GAMEPLAY_CONVERSATION;
            }
            else if( id == EVENT_MISSION_BRIEFING_ACCEPTED )
            {
                return;
            }
        }
    }

    //**********************************************************
    //
    // TODO: this is becoming a bit of an if-statement mess.  We're getting close
    // to alpha, so I'm not changing it now.  In the sequel, pEventData should
    // point to a parameter object which can be created in a variety of ways with
    // a variety of data, and you should be able to ask it for a tUID or
    // something when it gets here.
    //
    //**********************************************************

    //
    // Hack!!
    //
    if( id == EVENT_PHONE_BOOTH_RIDE_REQUEST )
    {
        m_phoneBoothRequestMade = true;
    }
    else if( ( id == EVENT_PHONE_BOOTH_NEW_VEHICLE_SELECTED )
             || ( id == EVENT_PHONE_BOOTH_OLD_VEHICLE_RESELECTED ) )
    {
        if( !m_phoneBoothRequestMade )
        {
            //
            // This isn't a phone booth car request, it's just a regular
            // vehicle load.  Throw the event out.
            //
            return;
        }
        else
        {
            m_phoneBoothRequestMade = false;
        }
    }
    else if( id == EVENT_PHONE_BOOTH_CANCEL_RIDEREPLY_LINE )
    {
        m_phoneBoothRequestMade = false;
        return;
    }
    else
    {
        //
        // We expect those requests to be made back-to-back.  Anything else,
        // and we're done with the phone booth.
        //
        m_phoneBoothRequestMade = false;
    }

    //
    // Filter out the reverse burnouts we don't want
    //
    if( id == EVENT_BURNOUT )
    {
        //
        // Need to check the mBrake value, calling IsInReverse() or
        // IsMovingBackward() filters out the small velocities we're
        // getting at the start of the burnout
        //
        if( playerAvatar->IsInCar()
            && ( playerAvatar->GetVehicle()->mBrake > 0.0f ) )
        {
            return;
        }
    }

    //
    // Turn wasp zaps into hit by car to play HitByC dialog lines
    //
    if( id == EVENT_WASP_BULLET_HIT_CHARACTER_STYLIZED_VIOLENCE_FOLLOWS )
    {
        id = EVENT_PLAYER_CAR_HIT_NPC;
        overridePositional = true;
    }

    if( id == EVENT_DIALOG_SHUTUP )
    {
        m_playbackQueue->StopAllDialog();
    }
    else if( id == EVENT_CONVERSATION_SKIP )
    {
        //
        // Stop anything that might be playing
        //
        dialogWasPlaying = m_playbackQueue->StopAllDialog();
        if( dialogWasPlaying )
        {
            GetEventManager()->TriggerEvent( EVENT_CONVERSATION_DONE );
        }
    }
    else if( m_dialogOn )
    {
        if( id == EVENT_TUTORIAL_DIALOG_PLAY )
        {
            tutorialIndex = *(reinterpret_cast<unsigned int*>( pEventData ));
            if( tutorialIndex < tutorialTableLength )
            {
                //
                // It's always Bart.  Specify him by UID (his Character
                // object isn't loaded for these)
                //
                character1 = NULL;
                character2 = NULL;
                charUID1 = tEntity::MakeUID( "bart" );
                charUID2 = tEntity::MakeUID( "homer" );
                convName = tutorialConvNames[tutorialIndex].convNameKey;
            }
            else
            {
                //
                // No line exists here, play nothing
                //
                rDebugString( "No tutorial dialog exists for event data\n" );
                return;
            }
        }
        else if( ( id == EVENT_CONVERSATION_INIT_DIALOG )
                 || ( id == EVENT_IN_GAMEPLAY_CONVERSATION ) )
        {
            //
            // Get characters to match
            //
            eventData = static_cast<DialogEventData*>( pEventData );
            character1 = eventData->char1;
            character2 = eventData->char2;
            charUID1 = eventData->charUID1;
            charUID2 = eventData->charUID2;

            //
            // Assumption: if eventData->dialogNameis zero,
            // that's an indication that no name is to be matched
            //
            convName = eventData->dialogName;
        }
        else if( id == EVENT_TRAFFIC_IMPEDED )
        {
            //
            // Almost-final hack!
            //
            // Special case, of course.  For this one, toss a coin between
            // playing the avatar line and the traffic line.  For the 
            // traffic, play one of the four generic vehicles.
            //
            if( playerAvatar->IsInCar() )
            {
                coinToss = rand() % 2;
            }
            else
            {
                coinToss = 1;
            }

            if( coinToss == 0 )
            {
                character1 = playerAvatar->GetCharacter();
                overridePositional = true;
            }
            else
            {
                coinToss = rand() % 4;
                charUID1 = genericTrafficUIDs[coinToss];
                vehicleEventData = static_cast<Vehicle*>( pEventData );
            }
        }
        else if( ( pEventData != NULL )
                 && ( id != EVENT_LOCATOR + LocatorEvent::BOUNCEPAD ) )
        {
            if( GetGameplayManager()->IsSuperSprint() )
            {
                minigameVehicle = static_cast<Vehicle*>(pEventData);

                avatarMgr = GetAvatarManager();
                numPlayers = GetGameplayManager()->GetNumPlayers();
                for( i = 0; i < numPlayers; i++ )
                {
                    playerAvatar = avatarMgr->GetAvatarForPlayer( i );
                    if( playerAvatar->GetVehicle() == minigameVehicle )
                    {
                        character1 = playerAvatar->GetCharacter();
                        break;
                    }
                }

                if( i >= numPlayers )
                {
                    //
                    // Not the player, so use the driver name
                    //
                    charUID1 = tEntity::MakeUID( minigameVehicle->mDriverName );
                }
            }
            else if( id == EVENT_WRONG_SIDE_DUMBASS )
            {
                //
                // Stinky special case.  pEventData is actually a Vehicle in this case, since
                // we can't use the character, since drivers have different names
                //
                vehicleEventData = static_cast<Vehicle*>(pEventData);
                dialog = m_dialogList->FindDialogForEvent( id, NULL, NULL,
                                                           tEntity::MakeUID( vehicleEventData->mDriverName ), 0, convName, false );
                if( dialog == NULL )
                {
                    rDebugPrintf( "No driver dialog found for event %d\n", static_cast<int>( id ) );
                }
                else
                {
                    m_playbackQueue->AddDialogToQueue( *dialog );
                }

                return;
            }
            else if( id == EVENT_DING_DONG )
            {
                //
                // Yet Another Special Case.  Doorbells play dialog for unloaded characters.
                // We're getting a string with the character name
                //
                charName = static_cast<char*>(pEventData);
                dialog = m_dialogList->FindDialogForEvent( id, NULL, NULL, tEntity::MakeUID( charName ), 0, 0, false );

                if( dialog == NULL )
                {
                    rDebugPrintf( "No doorbell dialog found for character %s\n", charName );
                }
                else
                {
                    m_playbackQueue->AddDialogToQueue( *dialog );
                }

                return;
            }
            else if( id == EVENT_BIG_BOOM_SOUND )
            {
                Vehicle* blowedUpCar = static_cast<Vehicle*>(pEventData);

                //
                // Can't handle below because we can get thrown out of the car before the
                // explosion.
                //
                if( playerAvatar->IsInCar() && ( blowedUpCar != playerAvatar->GetVehicle() ) )
                {
                    return;
                }

                character1 = playerAvatar->GetCharacter();
                rAssert( character1 != NULL );

                //
                // Play funny driver line
                //
                if( blowedUpCar->GetDriver() != NULL )
                {
                    dialog = m_dialogList->FindDialogForEvent( id, NULL, NULL, 
                                                               tEntity::MakeUID(blowedUpCar->mDriverName), 0, 0, false );
                    if( dialog == NULL )
                    {
                        rDebugPrintf( "No driver dialog found for event %d\n", static_cast<int>( id ) );
                    }
                    else
                    {
                        m_playbackQueue->AddDialogToQueue( *dialog );
                    }
                }
            }
            else if( eventHasVehicleData( id ) 
                     || ( id == EVENT_MISSION_FAILURE ) 
                     || ( id == EVENT_HIT_BREAKABLE )
                     || ( id == EVENT_DEATH_VOLUME_SOUND ) 
                     || ( id == EVENT_CARD_COLLECTED ) )
            {
                if( eventHasVehicleData( id ) 
                    && ( !(playerAvatar->IsInCar())
                         || ( static_cast<Vehicle*>(pEventData) != playerAvatar->GetVehicle() ) ) )
                {
                    //
                    // We're only interested in our own vehicle
                    //
                    return;
                }

                //
                // Another stinky special case.  Event data is used in the call 
                // to queueVillainDialog below.
                //
                character1 = playerAvatar->GetCharacter();
                rAssert( character1 != NULL );
            }
            else
            {
                //
                // We expect a Character* as the event data,
                //
                character1 = static_cast<Character*>( pEventData );
            }
        }
        else
        {
            //
            // If nothing is supplied as event data, we assume that the character
            // referred to is avatar 0
            //
            character1 = playerAvatar->GetCharacter();
            rAssert( character1 != NULL );
        }

        //
        // Check for events where we want to queue up a villain line before the walker/driver
        //
        queueVillainDialog( id, pEventData );

        //
        // Driver lines
        //
        if( ( playerAvatar != NULL )
            && playerAvatar->IsInCar() )
        {
            avatarVehicle = playerAvatar->GetVehicle();
            if( ( avatarVehicle->mpDriver != NULL ) && ( avatarVehicle->mpDriver != character1 ) )
            {
                tUID driverUID;

                //
                // Character is in car and not driving.  Trigger driver dialog
                // as well as character dialog below
                //
                driverUID = tEntity::MakeUID(avatarVehicle->mDriverName);

                dialog = m_dialogList->FindDialogForEvent( id, NULL, NULL, driverUID, charUID2, convName, false );
                if( dialog == NULL )
                {
                    rDebugPrintf( "No driver dialog found for event %d\n", static_cast<int>( id ) );
                }
                else
                {
                    m_playbackQueue->AddDialogToQueue( *dialog );
                }
            }
        }

        dialog = m_dialogList->FindDialogForEvent( id, character1, character2, charUID1, charUID2, convName, false );

        if( dialog == NULL )
        {
            rDebugPrintf( "No dialog found for event %d\n", static_cast<int>( id ) );
        }
        else
        {
            if( playLinePositionally( id ) && !overridePositional )
            {
                if( character1 != NULL )
                {
                    getCharacterPosition( character1, dlgPosition );
                }
                else
                {
                    vehicleEventData->GetPosition( &dlgPosition );
                }
                dlgPositionPtr = &dlgPosition;
            }
            m_playbackQueue->AddDialogToQueue( *dialog, dlgPositionPtr );
        }
    }
}

//=============================================================================
// DialogCoordinator::Initialize
//=============================================================================
// Description: Parse the namespace for dialog resources and register for
//              dialog-related events
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void DialogCoordinator::Initialize()
{
    m_dialogList->OrganizeDialog( m_dialogNamespace );

    registerDialogEvents();
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// DialogCoordinator::registerDialogEvents
//=============================================================================
// Description: Register as a listener for the dialog-related events with
//              the event manager.
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void DialogCoordinator::registerDialogEvents()
{
    unsigned int tableSize = DialogLine::GetEventTableSize();
    unsigned int i;

    for( i = 0; i < tableSize; i++ )
    {
        GetEventManager()->AddListener( this, DialogLine::GetEventTableEntry( i )->event );
    }
    
    GetEventManager()->AddListener( this, EVENT_CONVERSATION_SKIP );
    GetEventManager()->AddListener( this, EVENT_TUTORIAL_DIALOG_PLAY );
    GetEventManager()->AddListener( this, EVENT_DIALOG_SHUTUP );
    GetEventManager()->AddListener( this, EVENT_WASP_BULLET_HIT_CHARACTER_STYLIZED_VIOLENCE_FOLLOWS );
    GetEventManager()->AddListener( this, EVENT_PHONE_BOOTH_CANCEL_RIDEREPLY_LINE );
}

//=============================================================================
// DialogCoordinator::queueVillainDialog
//=============================================================================
// Description: If this is a villain event, queue up some villain dialog
//
// Parameters:  id - walker/driver event to be mapped to villain event
//              eventData - user data passed in with event
//
// Return:      void 
//
//=============================================================================
void DialogCoordinator::queueVillainDialog( EventEnum id, void* eventData )
{
    EventEnum villainID = NUM_EVENTS;  // Invalid placeholder value, shouldn't be used
    Mission* currentMission;
    MissionStage* currentStage;
    MissionObjective* currentObjective;
    MissionCondition* failureCondition;
    Vehicle* eventCar;
    Vehicle* AICar;
    VehicleAI* AICarAI;
    const char* villainName;
    SelectableDialog* dialog;
    GameplayManager* gameplayMgr;
    rmt::Vector villainPosn;
    rmt::Vector avatarPosn;
    rmt::Vector diff;
    bool noVillainLine = false;

    if( GetGameFlow()->GetCurrentContext() == CONTEXT_FRONTEND )
    {
        //
        // No villains in the FE
        //
        return;
    }

    gameplayMgr = GetGameplayManager();
    if( ( gameplayMgr == NULL ) || ( gameplayMgr->IsSuperSprint() ) )
    {
        return;
    }

    //
    // See if the given event ID is one that has corresponding villain dialog
    //
    switch( id )
    {
        case EVENT_TAIL_LOST_DIALOG:
            villainID = EVENT_VILLAIN_TAIL_EVADE;
            break;

        case EVENT_MINOR_VEHICLE_CRASH:
        case EVENT_BIG_VEHICLE_CRASH:
            eventCar = static_cast<Vehicle*>(eventData);
            if( ( eventCar != NULL )
                && ( gameplayMgr->GetCurrentMission() != NULL )
                && ( gameplayMgr->GetCurrentMission()->GetCurrentStage() != NULL )
                && ( gameplayMgr->GetCurrentMission()->GetCurrentStage()->GetMainAIVehicleForThisStage() != NULL ) )
            {
                //
                // Hack!
                //
                // Ideally, we should only play villain car crash dialogue if it's involved in the
                // collision, apparently.  Since we're trying to go final, just check to see if 
                // it's close by.  Good enough.
                //
                AICar = gameplayMgr->GetCurrentMission()->GetCurrentStage()->GetMainAIVehicleForThisStage();
                AICar->GetPosition( &villainPosn );
                GetAvatarManager()->GetAvatarForPlayer( 0 )->GetPosition( avatarPosn );
                diff.Sub( villainPosn, avatarPosn );
                if( diff.MagnitudeSqr() > 75.0f )
                {
                    noVillainLine = true;
                }
                else
                {
                    //
                    // Dig through the GameplayManager stuff to find out who is
                    // attacking who
                    //
                    currentMission = gameplayMgr->GetCurrentMission();
                    rAssert( currentMission != NULL );
                    currentStage = currentMission->GetCurrentStage();
                    if (currentStage != NULL)
                    {
                        currentObjective = currentStage->GetObjective();
                        rAssert( currentObjective != NULL );

                        if( currentObjective->GetObjectiveType() == MissionObjective::OBJ_DESTROY )
                        {
                            villainID = EVENT_BIG_CRASH;
                        }
                        else
                        {
                            villainID = EVENT_VILLAIN_CAR_HIT_PLAYER;
                        }
                    }
                    else
                    {
                        noVillainLine = true;
                    }
                }
            }
            else
            {
                noVillainLine = true;
            }
            break;

        case EVENT_MISSION_FAILURE:
            failureCondition = static_cast<MissionCondition*>(eventData);

            //
            // We only play villain dialog on mission failure
            // when the player blew a tail mission
            //
            if( failureCondition->GetType() == MissionCondition::COND_FOLLOW_DISTANCE )
            {
                villainID = EVENT_TAIL_LOST_DIALOG;
            }
            else if( failureCondition->IsChaseCondition() )
            {
                villainID = EVENT_MISSION_FAILURE;
            }
            else
            {
                //
                // No villain involved
                //
                noVillainLine = true;
            }
            break;

        case EVENT_MISSION_SUCCESS_DIALOG:
            currentMission = gameplayMgr->GetCurrentMission();
            rAssert( currentMission != NULL );
            currentStage = currentMission->GetCurrentStage();
            if( currentStage != NULL )
            {
                currentObjective = currentStage->GetObjective();
                rAssert( currentObjective != NULL );

                if( currentObjective->GetObjectiveType() == MissionObjective::OBJ_DESTROY )
                {
                    villainID = EVENT_MISSION_SUCCESS_DIALOG;
                }
                else
                {
                    noVillainLine = true;
                }
            }
            else
            {
                noVillainLine = true;
            }
            break;

        default:
            //
            // No villain line needed for this type of event
            //
            noVillainLine = true;
            break;
    }

    if( noVillainLine )
    {
        return;
    }

    //
    // Find out who the driver of the AI vehicle is and play the line
    // if the dialog exists
    //
   // AICar = gameplayMgr->GetVehicleInSlot( GameplayManager::eAICar );

    //Chuck: Esan use this method for getting the mainAI for a stage. It should work

    AICar = gameplayMgr->GetCurrentMission()->GetCurrentStage()->GetMainAIVehicleForThisStage();

    if( AICar != NULL )
    {
        if( ( id != EVENT_MISSION_FAILURE )
            && ( id != EVENT_MISSION_SUCCESS_DIALOG ) )
        {
            // HACK:
            // [Dusit Matthew Eakkachaichanvet: July 3rd, 2003]
            // When a vehicle is destroyed and we ask for the AI, well, 
            // it wont' find that vehicle in the database... So.. we have some
            // possible REAL fixes:
            // - When a car is destroyed, I should send you an event and you should remember the new husk (or lack thereof). 
            //   For this particular dialogue it doesn't matter. But y'know... maybe there are other bugs associated with this.
            // - We check it here. I would prefer we STILL play the sound, but it's really your call. 
            if( !AICar->mVehicleDestroyed )
            {
                AICarAI = GetVehicleCentral()->GetVehicleAI( AICar );

                // At the end of the mission, we don't seem to have AI anymore...
                if( ( AICarAI == NULL )
                    || ( AICarAI->GetState() == VehicleAI::STATE_WAITING ) )
                {
                    //
                    // Don't play dialogue for inactive AI vehicles
                    //
                    return;
                }
            }
        }
        villainName = AICar->GetDriverName();
        rAssert( villainName != NULL );

        //
        // If the villain isn't there, then we're probably in a race mission or something
        // where it isn't loaded
        //
        if( villainName != NULL )
        {
            dialog = m_dialogList->FindDialogForEvent( villainID, NULL, NULL, tEntity::MakeUID( villainName ), 0, 0, true );

            if( dialog == NULL )
            {
                rDebugPrintf( "No dialog found for villain event %d\n", static_cast<int>( villainID ) );
            }
            else
            {
                m_playbackQueue->AddDialogToQueue( *dialog );
            }
        }
    }
}

//=============================================================================
// DialogCoordinator::playLinePositionally
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id )
//
// Return:      bool 
//
//=============================================================================
bool DialogCoordinator::playLinePositionally( EventEnum id )
{
    return( ( id == EVENT_KICK_NPC_SOUND )
            || ( id == EVENT_PEDESTRIAN_DODGE )
            || ( id == EVENT_PLAYER_CAR_HIT_NPC )
            || ( id == EVENT_TRAFFIC_IMPEDED ) );
}

//=============================================================================
// DialogCoordinator::getCharacterPosition
//=============================================================================
// Description: Comment
//
// Parameters:  thePed - character to get position for.
//
// Return:      position
//
//=============================================================================
void DialogCoordinator::getCharacterPosition( Character* thePed, rmt::Vector& posn )
{
    rAssert( thePed != NULL );

    //
    // Get position from the character
    //
    thePed->GetPosition( posn );
}

//=============================================================================
// DialogCoordinator::eventHasVehicleData
//=============================================================================
// Description: Indicate whether the given event has a vehicle as a data
//              parameter (stinky void*'s)
//
// Parameters:  id - event ID to check
//
// Return:      true if vehicle parameter expected, false otherwise
//
//=============================================================================
bool DialogCoordinator::eventHasVehicleData( EventEnum id )
{
    bool retVal = false;

    switch( id )
    {
        case EVENT_MINOR_VEHICLE_CRASH:
        case EVENT_BIG_VEHICLE_CRASH:
        case EVENT_WRONG_SIDE_DUMBASS:
        case EVENT_TRAFFIC_IMPEDED:
        case EVENT_BIG_BOOM_SOUND:
        case EVENT_DEATH_VOLUME_SOUND:
            retVal = true;
            break;

        default:
            break;
    }

    return( retVal );
}
