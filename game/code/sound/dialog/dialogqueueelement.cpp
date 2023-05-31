//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        dialogqueueelement.cpp
//
// Description: Implement DialogQueueElement
//
// History:     04/09/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <radtime.hpp>

//========================================
// Project Includes
//========================================
#include <sound/dialog/dialogqueueelement.h>

#include <sound/dialog/selectabledialog.h>
#include <sound/dialog/dialogsounddebugpage.h>
#include <sound/dialog/dialogline.h>
#include <sound/dialog/dialoglist.h>

#include <memory/srrmemory.h>
#include <events/eventmanager.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/character/charactermanager.h>

//#define DEBUG_QUEUE_REFCOUNT


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

IRadTimerList* DialogQueueElement::s_timerList = NULL;
bool DialogQueueElement::s_watcherInitialized = false;

struct LinePriorityTableEntry
{
    EventEnum eventID;
    DialogPriority priority;
    unsigned int probability;
#ifndef RAD_RELEASE
    const char* eventName;
#endif
};

LinePriorityTableEntry priorityTable[] =
{
    { EVENT_GETINTOVEHICLE_START, OccasionalPlayLine, 20
#ifndef RAD_RELEASE
    , "GIC"
#endif
    },
    { EVENT_BURNOUT, OccasionalPlayLine, 15
#ifndef RAD_RELEASE
    , "Burn"
#endif
    },
    { EVENT_DESTINATION_REACHED, OccasionalPlayLine, 20
#ifndef RAD_RELEASE
    , "Arrive"
#endif
    },
    { EVENT_BIG_AIR, OccasionalPlayLine, 20
#ifndef RAD_RELEASE
    , "Air"
#endif
    },
    { EVENT_BIG_CRASH, OccasionalPlayLine, 20
#ifndef RAD_RELEASE
    , "Damage"
#endif
    },
    { EVENT_GETOUTOFVEHICLE_START, OccasionalPlayLine, 20
#ifndef RAD_RELEASE
        , "GOC"
#endif
    },
    { EVENT_RACE_PASSED_AI, OccasionalPlayLine, 20
#ifndef RAD_RELEASE
        , "Pass"
#endif
    },
    { EVENT_RACE_GOT_PASSED_BY_AI, OccasionalPlayLine, 20
#ifndef RAD_RELEASE
        , "Passed"
#endif
    },
    { EVENT_KICK_NPC_SOUND, OccasionalPlayLine, 20
#ifndef RAD_RELEASE
        , "HitByW"
#endif
    },
    { EVENT_PLAYER_CAR_HIT_NPC, OccasionalPlayLine, 20
#ifndef RAD_RELEASE
        , "HitByC"
#endif
    },
    { EVENT_PEDESTRIAN_DODGE, OccasionalPlayLine, 20
#ifndef RAD_RELEASE
        , "NHitByC"
#endif
    },
    { EVENT_PLAYER_MAKES_LIGHT_OF_CAR_HITTING_NPC, OccasionalPlayLine, 20
#ifndef RAD_RELEASE
    , "HitP"
#endif
    },
    { EVENT_PEDESTRIAN_SMACKDOWN, OccasionalPlayLine, 20
#ifndef RAD_RELEASE
    , "Char"
#endif
    },
    { EVENT_MINOR_VEHICLE_CRASH, OccasionalPlayLine, 20
#ifndef RAD_RELEASE
        , "Mcrash"
#endif
    },
    { EVENT_TRAFFIC_IMPEDED, OccasionalPlayLine, 20
#ifndef RAD_RELEASE
        , "CarWay"
#endif
    },
    { EVENT_HIT_BREAKABLE, OccasionalPlayLine, 20
#ifndef RAD_RELEASE
    , "Break"
#endif
    },
    { EVENT_COLLECT_OBJECT, OccasionalPlayLine, 50
#ifndef RAD_RELEASE
    , "ObjectW"
#endif
    },
    { EVENT_BIG_BOOM_SOUND, ShouldPlayLine, 100
#ifndef RAD_RELEASE
    , "Dcar"
#endif
    },
    { EVENT_MISSION_FAILURE, ShouldPlayLine, 100
#ifndef RAD_RELEASE
    , "Mfail"
#endif
    },
    { EVENT_TAIL_LOST_DIALOG, ShouldPlayLine, 100
#ifndef RAD_RELEASE
    , "Tail"
#endif
    },
    { EVENT_MISSION_SUCCESS_DIALOG, ShouldPlayLine, 100
#ifndef RAD_RELEASE
    , "Mvic"
#endif
    },
    { EVENT_CARD_COLLECTED, MustPlayLine, 100
#ifndef RAD_RELEASE
    , "Card"
#endif
    },
    { EVENT_PHONE_BOOTH_RIDE_REQUEST, MustPlayLine, 100
#ifndef RAD_RELEASE
    , "Askride"
#endif
    },
    { EVENT_PHONE_BOOTH_NEW_VEHICLE_SELECTED, MustPlayLine, 100
#ifndef RAD_RELEASE
    , "Ridereply"
#endif
    },
    { EVENT_PHONE_BOOTH_OLD_VEHICLE_RESELECTED, MustPlayLine, 100
#ifndef RAD_RELEASE
    , "Answer"
#endif
    },
    { EVENT_MISSION_BRIEFING_ACCEPTED, MustPlayLine, 100
#ifndef RAD_RELEASE
    , "Mstart"
#endif
    },
    { EVENT_CONVERSATION_INIT_DIALOG, MustPlayImmediately, 100
#ifndef RAD_RELEASE
        , "EVENT_CONVERSATION_INIT_DIALOG"
#endif
    },
    { EVENT_IN_GAMEPLAY_CONVERSATION, MustPlayImmediately, 100
#ifndef RAD_RELEASE
        , "EVENT_IN_GAMEPLAY_CONVERSATION"
#endif
    },
    { EVENT_TUTORIAL_DIALOG_PLAY, MustPlayImmediately, 100
#ifndef RAD_RELEASE
        , "EVENT_TUTORIAL_DIALOG_PLAY"
#endif
    },
    { EVENT_DING_DONG, MustPlayImmediately, 100
#ifndef RAD_RELEASE
        , "EVENT_DING_DONG"
#endif
    }
};

static unsigned int priorityTableLength = sizeof( priorityTable ) / sizeof( LinePriorityTableEntry );

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// DialogQueueElement::DialogQueueElement
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
DialogQueueElement::DialogQueueElement( SelectableDialog* dialog ) :
    m_dialog( dialog ),
    m_player1( NULL ),
    m_player2( NULL ),
    m_lineDoneCallback( NULL ),
    m_dialogDoneCallback( NULL ),
    m_linesPlayed( 0 ),
    m_queue( NULL ),
    m_hasPosition( false )
{
    unsigned int lifeInMsecs;

    rAssert( m_dialog != NULL );

    if( s_timerList == NULL )
    {
        //
        // We need enough timers for each element in the queue, plus one for
        // currently playing dialog and one to create before we test for the
        // queue being full.  Actually, we don't really need one for current
        // dialog, but we'll call that safety margin.
        //
        ::radTimeCreateList( &s_timerList, MAX_QUEUE_ELEMENTS + 2, GMA_AUDIO_PERSISTENT );
    }

    m_timer = NULL;

    lifeInMsecs = DialogLine::GetLifeInMsecsForEvent( dialog->GetEvent() );

    if( lifeInMsecs > 0 )
    {
        s_timerList->CreateTimer( &m_timer, 
                                  lifeInMsecs, 
                                  this, 
                                  NULL, 
                                  true, 
                                  IRadTimer::ResetModeOneShot );
        rAssert( m_timer != NULL );
    }
}

//==============================================================================
// DialogQueueElement::~DialogQueueElement
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
DialogQueueElement::~DialogQueueElement()
{
    if( m_timer )
    {
        m_timer->UnregisterCallback( this );
#ifdef DEBUG_QUEUE_REFCOUNT
        rTunePrintf( "~DialogQueueElement Timer: %d\n", m_timer->GetRefCount() );
#endif
        m_timer->Release();
    }
}

//=============================================================================
// DialogQueueElement::OnTimerDone
//=============================================================================
// Description: If the timer goes off and calls this function, then the
//              lifetime for the dialog has expired and we need to excuse
//              ourselves from the queue.
//
// Parameters:  elapsedTime - ignored
//              pUserData - ignored
//
// Return:      void 
//
//=============================================================================
void DialogQueueElement::OnTimerDone( unsigned int elapsedTime, void * pUserData )
{
    RemoveSelfFromList();

    //
    // We're now expendable.  Delete self.
    //
#ifdef DEBUG_QUEUE_REFCOUNT
    rTunePrintf( "OnTimerDone %x: Count %d\n", this, GetRefCount() );
#endif
    Release();
}

//=============================================================================
// DialogQueueElement::AddToQueue
//=============================================================================
// Description: Add self to the given dialog queue
//
// Parameters:  queue - queue to add self to
//              posn - position of dialog speaker, NULL if non-positional
//
// Return:      none
//
//=============================================================================
void DialogQueueElement::AddToQueue( DialogQueueType* queue, rmt::Vector* posn )
{
    DialogQueueElement* lowerPriorityElement;
    bool duplicateFound = false;
    DialogPriority priority = CalculateDialogPriority( *m_dialog );

    rAssert( queue != NULL );
    m_queue = queue;

    if( posn != NULL )
    {
        m_hasPosition = true;
        m_position = *posn;
    }
    else
    {
        m_hasPosition = false;
    }

    if( m_queue->empty() )
    {
        m_queue->push_front( this );
    }
    else
    {
        DialogQueueType::iterator iter = m_queue->begin();

        //
        // Search the rest of the list to see if we've already got this
        // dialog in it.  It couldn't have been passed over yet because
        // all the earlier stuff has a higher priority
        //
        for( ; iter != m_queue->end(); ++iter )
        {
            if( (*iter)->DialogMatches( m_dialog ) )
            {
                duplicateFound = true;
                break;
            }
        }

        if( !duplicateFound )
        {
            if( m_queue->size() >= MAX_QUEUE_ELEMENTS )
            {
                //
                // Dialog full, something has to go.  Ditch the lowest priority
                // one.  If that's the one we're inserting, don't do it.  If there's
                // a lower-priority one in the queue, ditch that one instead.
                //
                if( iter == m_queue->end() )
                {
                    //
                    // Nothing lower
                    //
#ifdef DEBUG_QUEUE_REFCOUNT
                    rTunePrintf( "AddToQueue %x: Count %d\n", this, GetRefCount() );
#endif
                    Release();
                }
                else
                {
                    lowerPriorityElement = *iter;
                    m_queue->insert( iter, this );
                    m_queue->remove( lowerPriorityElement );
                    lowerPriorityElement->Release();
                }
            }
            else
            {
                //
                // Find spot to insert based on priority
                //
                iter = m_queue->begin();
                while( ( iter != m_queue->end() )
                    && ( (*iter)->GetPriority() >= priority ) )
                {
                    ++iter;
                }

                m_queue->insert( iter, this );
            }
        }
        else
        {
            //
            // No need to add this to the queue, and the caller is now relying
            // on this object for self-management, so delete self
            //
#ifdef DEBUG_QUEUE_REFCOUNT
            rTunePrintf( "AddToQueue %x: Count %d\n", this, GetRefCount() );
#endif
            Release();
        }
    }
}

//=============================================================================
// DialogQueueElement::RemoveSelfFromList
//=============================================================================
// Description: Pull ourself from the queue element list
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void DialogQueueElement::RemoveSelfFromList()
{
    rAssert( m_queue != NULL );

    m_queue->remove( this );
}

//=============================================================================
// DialogQueueElement::GetDialogPriority
//=============================================================================
// Description: Searches the priority table for a priority matching the event
//              ID for this bit of dialog.
//
// Parameters:  dialog - dialog to find priority for
//
// Return:      DialogPriority value for dialog if found in table,
//              UnknownPriority otherwise.
//
//=============================================================================
DialogPriority DialogQueueElement::CalculateDialogPriority( const SelectableDialog& dialog )
{
    EventEnum eventID;
    unsigned int i;
    DialogPriority priority = UnknownPriority;

    eventID = dialog.GetEvent();
    for( i = 0; i < priorityTableLength; i++ )
    {
        if( priorityTable[i].eventID == eventID )
        {
            priority = priorityTable[i].priority;
            break;
        }
    }

    return( priority );
}

//=============================================================================
// DialogQueueElement::CalculateDialogProbability
//=============================================================================
// Description: Searches the priority table for a probability matching the event
//              ID for this bit of dialog.
//
// Parameters:  dialog - dialog to find priority for
//
// Return:      Probability as % for dialog if found in table,
//              100 otherwise.
//
//=============================================================================
unsigned int DialogQueueElement::CalculateDialogProbability( const SelectableDialog& dialog )
{
    EventEnum eventID;
    unsigned int i;
    unsigned int probability = 100;

    eventID = dialog.GetEvent();
    for( i = 0; i < priorityTableLength; i++ )
    {
        if( ( priorityTable[i].priority == OccasionalPlayLine )
            && ( priorityTable[i].eventID == eventID ) )
        {
            probability = priorityTable[i].probability;
            break;
        }
    }

    return( probability );
}

//=============================================================================
// DialogQueueElement::OnPlaybackComplete
//=============================================================================
// Description: Callback function, triggered when currently playing dialog
//              is finished
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void DialogQueueElement::OnPlaybackComplete()
{
    unsigned int numDialogLines = m_dialog->GetNumDialogLines();
    SimpsonsSoundPlayer* player;
    SimpsonsSoundPlayer* queuer;  // I think I made up a word
    Character* npcPtr;

    //
    // Reference self in case one of the callbacks we trigger
    // here tries to delete this object
    //
    AddRef();

    if( ++m_linesPlayed >= numDialogLines )
    {
        if( m_dialogDoneCallback )
        {
            if( isMouthFlappingEvent( m_dialog->GetEvent() ) )
            {
                //
                // Coordinate the mouth flapping.  Find out whether the PC
                // or the NPC just finished talking, and send out the appropriate
                // talk/shutup events.
                //
                if( dialogLineIsWalker( m_linesPlayed ) )
                {
                    GetEventManager()->TriggerEvent( EVENT_PC_SHUTUP );
                }
                else
                {
                    GetEventManager()->TriggerEvent( EVENT_NPC_SHUTUP );
                }
            }

            //
            // Tell the rest of the world that we're done
            //
            if( m_dialog->GetEvent() == EVENT_CONVERSATION_INIT_DIALOG )
            {
/*
                if( m_dialog->GetMission() == static_cast<unsigned int>( DialogLine::TUTORIAL_MISSION_NUMBER ) )
                {
                    GetEventManager()->TriggerEvent( EVENT_TUTORIAL_DIALOG_DONE );
                }
                else
*/
                {
                    GetEventManager()->TriggerEvent( EVENT_CONVERSATION_DONE );
                }
            }
            else if( m_dialog->GetEvent() == EVENT_TUTORIAL_DIALOG_PLAY )
            {
                GetEventManager()->TriggerEvent( EVENT_TUTORIAL_DIALOG_DONE );
            }

            m_dialogDoneCallback->OnDialogComplete();
        }
    }
    else
    {
        if( m_lineDoneCallback )
        {
            m_lineDoneCallback->OnDialogLineComplete();
        }

        if( isMouthFlappingEvent( m_dialog->GetEvent() ) )
        {
            //
            // Coordinate the mouth flapping.  Find out whether the PC
            // or the NPC just finished talking, and send out the appropriate
            // talk/shutup events.
            //
            if( dialogLineIsWalker( m_linesPlayed ) )
            {
                GetEventManager()->TriggerEvent( EVENT_PC_SHUTUP );
            }
            else
            {
                GetEventManager()->TriggerEvent( EVENT_NPC_SHUTUP );
            }

            //
            // Start up the next line
            //
            if( dialogLineIsWalker( m_linesPlayed + 1 ) )
            {
                GetEventManager()->TriggerEvent( EVENT_PC_TALK );
            }
            else
            {
                npcPtr = dialogLineIsNPC( m_linesPlayed + 1 );
                if( npcPtr != NULL )
                {
                    GetEventManager()->TriggerEvent( EVENT_NPC_TALK, npcPtr );
                }
            }
        }

        //
        // Even line numbers on player 1, odd on player 2
        //
        if( m_linesPlayed % 2 == 0 )
        {
            player = m_player1;
            queuer = m_player2;
        }
        else
        {
            player = m_player2;
            queuer = m_player1;
        }
        m_dialog->PlayQueuedLine( *player, this );

        //
        // Queue dialog if necessary
        //
        if( numDialogLines > m_linesPlayed + 1 )
        {
            m_dialog->QueueLine( m_linesPlayed + 1, *queuer );
        }
    }

#ifdef DEBUG_QUEUE_REFCOUNT
    rTunePrintf( "OnPlaybackComplete %x: Count %d\n", this, GetRefCount() );
#endif
    Release();
}

//=============================================================================
// DialogQueueElement::OnSoundReady
//=============================================================================
// Description: Unused, required for SimpsonsSoundPlayerCallback interface
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void DialogQueueElement::OnSoundReady()
{
}

//=============================================================================
// DialogQueueElement::PlayDialog
//=============================================================================
// Description: Comment
//
// Parameters:  player - SimpsonsSoundPlayer to use for playback
//              lineCallback - callback for when an individual line of
//                             dialog is done, but not the entire dialog
//              dialogCallback - callback for when the entire dialog is done
//
// Return:      void 
//
//=============================================================================
void DialogQueueElement::PlayDialog( SimpsonsSoundPlayer& player1,
                                     SimpsonsSoundPlayer& player2,
                                     DialogLineCompleteCallback* lineCallback,
                                     DialogCompleteCallback* dialogCallback )
{
    Character* npcPtr;

    m_lineDoneCallback = lineCallback;
    m_dialogDoneCallback = dialogCallback;

    m_player1 = &player1;
    m_player2 = &player2;
    m_dialog->PlayLine( m_linesPlayed, *m_player1, this );
    if( m_dialog->GetNumDialogLines() >= 2 )
    {
        //
        // Queue up the next line
        //
        m_dialog->QueueLine( m_linesPlayed + 1, *m_player2 );
    }

    if( isMouthFlappingEvent( m_dialog->GetEvent() ) )
    {
        //
        // Start the mouth flapping
        //
        if( dialogLineIsWalker( 1 ) )
        {
            GetEventManager()->TriggerEvent( EVENT_PC_TALK );
        }
        else
        {
            npcPtr = dialogLineIsNPC( 1 );
            if( npcPtr != NULL )
            {
                GetEventManager()->TriggerEvent( EVENT_NPC_TALK, npcPtr );
            }
        }
    }
    
    if( m_timer )
    {
        m_timer->UnregisterCallback( this );
#ifdef DEBUG_QUEUE_REFCOUNT
        rTunePrintf( "PlayDialog Timer: %d\n", m_timer->GetRefCount() );
#endif
        m_timer->Release();
        m_timer = NULL;
    }
}

//=============================================================================
// DialogQueueElement::StopDialog
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void DialogQueueElement::StopDialog()
{
    //
    // Just in case of nasty callback action on the Stop() calls below
    //
    AddRef();

    //
    // Skip to the end of the dialog before triggering any stop callbacks
    //
    m_linesPlayed = m_dialog->GetNumDialogLines();

    m_player1->Stop();
    m_player2->Stop();

    if( m_dialog != NULL )
    {
        if( isMouthFlappingEvent( m_dialog->GetEvent() ) )
        {
            GetEventManager()->TriggerEvent( EVENT_PC_SHUTUP );
            GetEventManager()->TriggerEvent( EVENT_NPC_SHUTUP );
        }
    }

#ifdef DEBUG_QUEUE_REFCOUNT
    rTunePrintf( "StopDialog %x: Count %d\n", this, GetRefCount() );
#endif
    Release();
}

//=============================================================================
// DialogQueueElement::Service
//=============================================================================
// Description: Service the timer list
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void DialogQueueElement::Service()
{
    if( s_timerList != NULL )
    {
        s_timerList->Service();
    }

#ifndef RAD_RELEASE
    if( !s_watcherInitialized )
    {
        //
        // Register probability table in Watcher
        //
        unsigned int i;

        for( i = 0; i < priorityTableLength; i++ )
        {
            if( priorityTable[i].priority == OccasionalPlayLine )
            {
                radDbgWatchAddUnsignedInt( &(priorityTable[i].probability),
                                           priorityTable[i].eventName,
                                           "Dialogue Tuning",
                                           NULL,
                                           NULL,
                                           0,
                                           100 );
            }
        }

        s_watcherInitialized = true;
    }
#endif
}

//=============================================================================
// DialogQueueElement::GetPosition
//=============================================================================
// Description: If this is a positional dialog, return a position
//
// Parameters:  None
//
// Return:      pointer to rmt::Vector with position if positional, NULL otherwise
//
//=============================================================================
rmt::Vector* DialogQueueElement::GetPosition()
{
    rmt::Vector* retVal;

    if( m_hasPosition )
    {
        retVal = &m_position;
    }
    else
    {
        retVal = NULL;
    }

    return( retVal );
}

void DialogQueueElement::FillDebugInfo( DialogSoundDebugPage& debugInfo, unsigned int lineNum )
{
    unsigned int msecs;

    if( m_timer != NULL )
    {
        msecs = m_timer->GetTimeout();
    }
    else
    {
        msecs = 0;
    }

    debugInfo.SetQueueEntry( lineNum,
                             m_dialog->GetEvent(),
                             m_dialog->GetMission(),
                             m_dialog->GetLevel(),
                             m_dialog->GetDialogLineCharacterUID( 1 ),
                             CalculateDialogPriority( *m_dialog ),
                             msecs );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// DialogQueueElement::dialogLineIsWalker
//=============================================================================
// Description: Indicates whether the dialog line with the given number
//              belongs to the player character
//
// Parameters:  lineNum - line number to check
//
// Return:      true if player character, false otherwise
//
//=============================================================================
bool DialogQueueElement::dialogLineIsWalker( unsigned int lineNum )
{
    tUID characterUID;
    Character* walker;
    tUID walkerUID;

    characterUID = m_dialog->GetDialogLineCharacterUID( lineNum );
    walker = GetAvatarManager()->GetAvatarForPlayer( 0 )->GetCharacter();
    rAssert( walker != NULL );
    walkerUID = walker->GetUID();

    return( characterUID == walkerUID );
}

//=============================================================================
// DialogQueueElement::dialogLineIsNPC
//=============================================================================
// Description: Indicates whether the dialog line with the given number
//              belongs to an NPC
//
// Parameters:  lineNum - line number to check
//
// Return:      pointer to character object if found, NULL otherwise
//
//=============================================================================
Character* DialogQueueElement::dialogLineIsNPC( unsigned int lineNum )
{
    tUID speakerUID;
    Character* npc;

    speakerUID = m_dialog->GetDialogLineCharacterUID( lineNum );
    npc = GetCharacterManager()->GetCharacterByName( speakerUID );

    //
    // If the character is an NPC, we'll get a pointer.  If it's some
    // character that doesn't exist in the world (e.g. Brockman in L7M1),
    // then we get NULL.
    //
    if( npc != NULL )
    {
        return( npc );
    }

    //
    // P.S. Not only do we have to check the speaker's UID, we have to
    // check for stinky skins as well
    //
    return( DialogList::GetStinkySkinPointer( speakerUID ) );
}

//=============================================================================
// DialogQueueElement::isMouthFlappingEvent
//=============================================================================
// Description: Indicate whether given dialogue event should trigger mouth
//              flapping
//
// Parameters:  theEvent - event to scrutinize
//
// Return:      true if mouth flapping event, false otherwise 
//
//=============================================================================
bool DialogQueueElement::isMouthFlappingEvent( EventEnum theEvent )
{
    bool retVal = false;

    switch( theEvent )
    {
        case EVENT_CONVERSATION_INIT_DIALOG:
        case EVENT_AMBIENT_ASKFOOD:
        case EVENT_AMBIENT_FOODREPLY:
        case EVENT_AMBIENT_GREETING:
        case EVENT_AMBIENT_RESPONSE:
        case EVENT_PEDESTRIAN_SMACKDOWN:
        case EVENT_CHARACTER_TIRED_NOW:
            retVal = true;
            break;

        default:
            break;
    }

    return( retVal );
}