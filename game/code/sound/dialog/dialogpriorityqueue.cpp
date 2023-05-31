//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        dialogpriorityqueue.cpp
//
// Description: Responsible for managing the outstanding dialog playback requests.
//              When the DialogCoordinator needs to play dialog, it hands the 
//              PlayableDialog object off, and the DialogPriorityQueue determines
//              if it can be played, or if it should wait until some other dialog 
//              completes.  When a PlayableDialog is ready for playback, it gets
//              handed to the SimpsonsSoundPlayer.
//
// History:     04/09/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <radtime.hpp>
#include <radnamespace.hpp>

//========================================
// Project Includes
//========================================
#include <sound/dialog/dialogpriorityqueue.h>

#include <sound/dialog/dialogqueueelement.h>
#include <sound/dialog/selectabledialog.h>
#include <sound/soundrenderer/soundrenderingmanager.h>
#include <sound/soundrenderer/idasoundtuner.h>
#include <sound/soundmanager.h>

#include <memory/srrmemory.h>


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//
// Probability of optional play success as chance in 256
//
static const unsigned int OPL_PROB = 64;

//#define DEBUG_QUEUE_REFCOUNT

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// DialogPriorityQueue::DialogPriorityQueue
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
DialogPriorityQueue::DialogPriorityQueue() :
#ifdef SOUND_DEBUG_INFO_ENABLED
    m_debugPage( 3, GetSoundManager()->GetDebugDisplay() ),
#endif
    m_nowPlaying( NULL ),
    m_permitQueueAdvance( true )
{
}

//==============================================================================
// DialogPriorityQueue::~DialogPriorityQueue
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
DialogPriorityQueue::~DialogPriorityQueue()
{
}

//=============================================================================
// DialogPriorityQueue::AddDialogToQueue
//=============================================================================
// Description: Place dialog on queue and play if possible
//
// Parameters:  dialog - dialog to add to queue
//
// Return:      void 
//
//=============================================================================
void DialogPriorityQueue::AddDialogToQueue( SelectableDialog& dialog, rmt::Vector* posn )
{
    DialogPriority priority;
    DialogQueueElement* queueElement;
    unsigned int diceRoll;

    //
    // Check the priority on the dialog, and use it to find the appropriate
    // spot in the queue.
    //
    priority = DialogQueueElement::CalculateDialogPriority( dialog );

    if( priority == OccasionalPlayLine )
    {
        //
        // Random play
        //
        diceRoll = ( rand() % 100 );
        if( diceRoll >= DialogQueueElement::CalculateDialogProbability( dialog ) )
        {
            return;
        }
    }

    //
    // Ducking for dialog
    //
    if( m_nowPlaying == NULL )
    {
        Sound::daSoundRenderingManager::GetInstance()->GetTuner()->ActivateSituationalDuck( NULL, Sound::DUCK_SIT_DIALOG, NULL, false );
        GetSoundManager()->MuteNISPlayers();
    }

    //
    // Don't bother playing dialog if we're already playing the same thing.
    // This can happen with collision events that get triggered zillions of
    // times
    //
    if( ( m_nowPlaying == NULL )
        || !( m_nowPlaying->DialogMatches( &dialog ) ) )
    {
        queueElement = new ( GMA_TEMP ) DialogQueueElement( &dialog );

        if( priority == MustPlayImmediately )
        {
            //
            // Special case.  Place this guy at the head of the queue and kill
            // whatever's playing right now.
            //
            if( m_nowPlaying )
            {
                //
                // If we don't halt the dialog queue, the stop callback will advance
                // it on us and we get two dialog lines
                //
                m_permitQueueAdvance = false;

                m_nowPlaying->StopDialog();
#ifdef DEBUG_QUEUE_REFCOUNT
                rTunePrintf( "AddDialogToQueue %x: Count %d\n", m_nowPlaying, m_nowPlaying->GetRefCount() );
#endif
                //
                // One more check.  The StopDialog() above may already have made m_nowPlaying
                // go away
                //
                if( m_nowPlaying != NULL )
                {
                    m_nowPlaying->Release();
                }

                m_permitQueueAdvance = true;
            }

            m_nowPlaying = queueElement;
            playDialog( posn );
        }
        else
        {
            //
            // Stick it in the list
            //
            queueElement->AddToQueue( &m_dialogQueue, posn );
        }
    }

#ifndef RAD_RELEASE
    //
    // Mark dialog as played for coverage testing purposes
    //
    dialog.MarkAsPlayed();
#endif
}

//=============================================================================
// DialogPriorityQueue::StopCurrentDialog
//=============================================================================
// Description: If we have something playing, stop it and yank it off the
//              queue
//
// Parameters:  None
//
// Return:      true if there was dialog to stop, false otherwise 
//
//=============================================================================
bool DialogPriorityQueue::StopCurrentDialog()
{
    bool dialogStopped = false;

    if( m_nowPlaying )
    {
        //
        // Stop it and let the callback do the rest
        //
        m_nowPlaying->StopDialog();
        dialogStopped = true;
    }

    return( dialogStopped );
}

//=============================================================================
// DialogPriorityQueue::StopAllDialog
//=============================================================================
// Description: Kill the queue.  This'll most likely be done when gameplay
//              ends.
//
// Parameters:  None
//
// Return:      true if there was dialog to stop, false otherwise  
//
//=============================================================================
bool DialogPriorityQueue::StopAllDialog()
{
    DialogQueueElement* current;
    bool dialogStopped = false;

    //
    // Just in case we're still in a paused state.  Stopping paused dialogue doesn't
    // seem to give us our stop callbacks
    //
    UnpauseDialog(); 

    m_permitQueueAdvance = false;

    while( !m_dialogQueue.empty() )
    {
        current = m_dialogQueue.front();
        current->RemoveSelfFromList();
        current->Release();
    }

    if( m_nowPlaying != NULL )
    {
        m_nowPlaying->StopDialog();

        //
        // Check again to make sure that the dialog didn't remove itself.
        // Since the queue was emptied first, nothing else should replace
        // the currently playing dialog afterward.
        //
        if( m_nowPlaying != NULL )
        {
#ifdef DEBUG_QUEUE_REFCOUNT
            rTunePrintf( "StopAllDialog %x: Count %d\n", m_nowPlaying, m_nowPlaying->GetRefCount() );
#endif
            m_nowPlaying->Release();
            m_nowPlaying = NULL;

            Sound::daSoundRenderingManager::GetInstance()->GetTuner()->ActivateSituationalDuck( NULL, Sound::DUCK_SIT_DIALOG, NULL, true );
            GetSoundManager()->UnmuteNISPlayers();
        }

        dialogStopped = true;
    }

    m_permitQueueAdvance = true;

    //
    // Since we don't seem to get the OnPlaybackComplete callback for the queue
    // element, throw a couple of shutup events in case someone was mouth flapping
    //
    GetEventManager()->TriggerEvent( EVENT_PC_SHUTUP );
    GetEventManager()->TriggerEvent( EVENT_NPC_SHUTUP );

    return( dialogStopped );
}

//=============================================================================
// DialogPriorityQueue::PauseDialog
//=============================================================================
// Description: Pause the dialog players
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void DialogPriorityQueue::PauseDialog()
{
    m_player1.Pause();
    m_player2.Pause();
    m_positionalPlayer1.Pause();
    m_positionalPlayer2.Pause();
}

//=============================================================================
// DialogPriorityQueue::UnpauseDialog
//=============================================================================
// Description: Unpause the dialog players
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void DialogPriorityQueue::UnpauseDialog()
{
    if( m_player1.IsPaused() )
    {
        m_player1.Continue();
    }
    if( m_player2.IsPaused() )
    {
        m_player2.Continue();
    }
    if( m_positionalPlayer1.IsPaused() )
    {
        m_positionalPlayer1.Continue();
    }
    if( m_positionalPlayer2.IsPaused() )
    {
        m_positionalPlayer2.Continue();
    }
}

//=============================================================================
// DialogPriorityQueue::OnDialogLineComplete
//=============================================================================
// Description: Called when a line of dialog in the currently playing
//              conversation is complete
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void DialogPriorityQueue::OnDialogLineComplete()
{
    //
    // TODO: Stop the mouth flapping and eye blinking
    //
}

//=============================================================================
// DialogPriorityQueue::OnDialogComplete
//=============================================================================
// Description: Called when the currently playing SelectableDialog is
//              complete
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void DialogPriorityQueue::OnDialogComplete()
{
    //
    // TODO: Stop the mouth flapping and eye blinking
    //
    
    //
    // Get rid of the currently playing element
    //
#ifdef DEBUG_QUEUE_REFCOUNT
    rTunePrintf( "OnDialogComplete %x: Count %d\n", m_nowPlaying, m_nowPlaying->GetRefCount() );
#endif
    m_nowPlaying->Release();
    if( !m_dialogQueue.empty() && m_permitQueueAdvance )
    {
        m_nowPlaying = m_dialogQueue.front();
        m_nowPlaying->RemoveSelfFromList();
        playDialog( m_nowPlaying->GetPosition() );
    }
    else
    {
        m_nowPlaying = NULL;

        Sound::daSoundRenderingManager::GetInstance()->GetTuner()->ActivateSituationalDuck( NULL, Sound::DUCK_SIT_DIALOG, NULL, true );
        GetSoundManager()->UnmuteNISPlayers();
    }
}

//=============================================================================
// DialogPriorityQueue::Service
//=============================================================================
// Description: Do servicing stuff
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void DialogPriorityQueue::ServiceOncePerFrame()
{
    DialogQueueElement::Service();

    //
    // Just to be safe, don't update the positional players here.  The update
    // function just does stuff for moving sounds and pausing out-of-range
    // stuff, none of which applies here.  It's theoretically safe to do the
    // right thing and service these players, but we're way too close to final
    // to change the status quo.
    //
    //m_positionalPlayer1.ServiceOncePerFrame();
    //m_positionalPlayer2.ServiceOncePerFrame();

    advanceQueue();

    serviceDebugPage();
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// DialogPriorityQueue::advanceQueue
//=============================================================================
// Description: Advance the queue if non-empty and nothing's playing.
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void DialogPriorityQueue::advanceQueue()
{
    if( ( m_nowPlaying == NULL ) && ( !m_dialogQueue.empty() ) && m_permitQueueAdvance )
    {
        m_nowPlaying = m_dialogQueue.front();
        m_nowPlaying->RemoveSelfFromList();
        playDialog( m_nowPlaying->GetPosition() );
    }
}

//=============================================================================
// DialogPriorityQueue::playDialog
//=============================================================================
// Description: Start a dialog line playing with the correct players
//
// Parameters:  posn - position of speaker for positional dialogue.  NULL
//                     if non-positional
//
// Return:      void 
//
//=============================================================================
void DialogPriorityQueue::playDialog( rmt::Vector* posn )
{
    IRadNameSpace* nameSpace;
    IRefCount* nameSpaceObj;
    positionalSoundSettings* parameters;

    if( posn != NULL )
    {
        //
        // Before starting playback, set up the positional stuff
        //
        //
        // Get the positionalSoundSettings object for the wasp sound
        //
        nameSpace = Sound::daSoundRenderingManagerGet()->GetTuningNamespace();
        rAssert( nameSpace != NULL );
        nameSpaceObj = nameSpace->GetInstance( ::radMakeKey32( "posn_dialog_settings" ) );
        if( nameSpaceObj != NULL )
        {
            parameters = reinterpret_cast<positionalSoundSettings*>( nameSpaceObj );
            m_positionalPlayer1.SetParameters( parameters );

            m_positionalPlayer1.SetPosition( posn->x, posn->y, posn->z );

            m_nowPlaying->PlayDialog( m_positionalPlayer1, m_positionalPlayer2, this, this );
        }
        else
        {
            rTuneAssertMsg( false, "No min/max for positional dialogue?  Bad, call Esan." );

            //
            // Handle gracefully in release
            //
            m_nowPlaying->PlayDialog( m_player1, m_player2, this, this );
        }
    }
    else
    {
        m_nowPlaying->PlayDialog( m_player1, m_player2, this, this );
    }
}

//=============================================================================
// DialogPriorityQueue::serviceDebugPage
//=============================================================================
// Description: Update the SoundInfo data available in Watcher
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void DialogPriorityQueue::serviceDebugPage()
{
#ifdef SOUND_DEBUG_INFO_ENABLED
    int i;
    int queueLength;
    DialogQueueType::const_iterator iter;

    if( m_nowPlaying != NULL )
    {
        //
        // For the debug page, nowPlaying is part of the queue
        //
        queueLength = m_dialogQueue.size() + 1;
        m_debugPage.SetQueueLength( queueLength );

        m_nowPlaying->FillDebugInfo( m_debugPage, 0 );
        i = 1;
        for( iter = m_dialogQueue.begin(); iter != m_dialogQueue.end(); ++iter )
        {
            (*iter)->FillDebugInfo( m_debugPage, i++ );
        }
    }
    else
    {
        m_debugPage.SetQueueLength( 0 );
    }
#endif
}
