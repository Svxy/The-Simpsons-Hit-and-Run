//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        dialogpriorityqueue.h
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

#ifndef DIALOGPRIORITYQUEUE_H
#define DIALOGPRIORITYQUEUE_H

//========================================
// Nested Includes
//========================================
#include <sound/simpsonssoundplayer.h>
#include <sound/positionalsoundplayer.h>
#include <sound/dialog/dialogqueueelement.h>
#include <sound/dialog/dialogqueuetype.h>
#include <sound/dialog/dialogsounddebugpage.h>

//========================================
// Forward References
//========================================
class SelectableDialog;

//=============================================================================
//
// Synopsis:    DialogPriorityQueue
//
//=============================================================================

class DialogPriorityQueue : public DialogLineCompleteCallback,
                            public DialogCompleteCallback
{
    public:
        DialogPriorityQueue();
        virtual ~DialogPriorityQueue();

        void AddDialogToQueue( SelectableDialog& dialog, rmt::Vector* posn = NULL );
        
        bool StopCurrentDialog();
        bool StopAllDialog();

        void PauseDialog();
        void UnpauseDialog();

        void OnDialogLineComplete();
        void OnDialogComplete();

        void ServiceOncePerFrame();

    private:
        //Prevent wasteful constructor creation.
        DialogPriorityQueue( const DialogPriorityQueue& original );
        DialogPriorityQueue& operator=( const DialogPriorityQueue& rhs );

        void advanceQueue();
        void serviceDebugPage();
        void playDialog( rmt::Vector* posn );

#ifdef SOUND_DEBUG_INFO_ENABLED
        DialogSoundDebugPage m_debugPage;
#endif

        //
        // Dialog sound players.  Two needed so that we can queue up a second
        // line while the first is playing.
        //
        SimpsonsSoundPlayer m_player1;
        SimpsonsSoundPlayer m_player2;

        PositionalSoundPlayer m_positionalPlayer1;
        PositionalSoundPlayer m_positionalPlayer2;

        //
        // Points to whatever's playing right this instant.  Stored separately
        // from the waiting queue objects.
        //
        DialogQueueElement* m_nowPlaying;

        //
        // The queue
        //
        DialogQueueType m_dialogQueue;

        bool m_permitQueueAdvance;
};


#endif // DIALOGPRIORITYQUEUE_H

