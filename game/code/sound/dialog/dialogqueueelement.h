//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        dialogqueueelement.h
//
// Description: Wrapper for SelectableDialog objects in the dialog queue,
//              to associate them with a timer and to allow reception of
//              playback completion callbacks without having to bother
//              the queue class with partially-complete conversations.
//
// History:     04/09/2002 + Created -- Darren
//
//=============================================================================

#ifndef DIALOGQUEUEELEMENT_H
#define DIALOGQUEUEELEMENT_H

//========================================
// Nested Includes
//========================================
#include <radtime.hpp>

#include <sound/simpsonssoundplayer.h>
#include <sound/dialog/dialogqueuetype.h>
#include <events/eventenum.h>

//========================================
// Forward References
//========================================
class SelectableDialog;
class SimpsonsSoundPlayer;
class DialogSoundDebugPage;
class Character;

//
// Four levels of playback priority
//
enum DialogPriority
{
    UnknownPriority,

    OccasionalPlayLine,
    ShouldPlayLine,
    MustPlayLine,
    MustPlayImmediately
};

//=============================================================================
//
// Synopsis:    DialogLineCompleteCallback - callback interface, triggered 
//                  when a line of dialog is finished playing, but more
//                  dialog remains to be played
//
//=============================================================================

class DialogLineCompleteCallback
{
    public:
        virtual void OnDialogLineComplete() = 0;
};

//=============================================================================
//
// Synopsis:    DialogCompleteCallback - callback interface, triggered 
//                  when all of the dialog has been played
//
//=============================================================================

class DialogCompleteCallback
{
    public:
        virtual void OnDialogComplete() = 0;
};

//=============================================================================
//
// Synopsis:    DialogQueueElement
//
//=============================================================================

class DialogQueueElement : public IRadTimerCallback,
                           public SimpsonsSoundPlayerCallback,
                           public radRefCount
{
    public:
        IMPLEMENT_REFCOUNTED( "DialogQueueElement" );

        DialogQueueElement( SelectableDialog* dialog );
        virtual ~DialogQueueElement();

        static void CreateTimerList();

        void OnTimerDone( unsigned int elapsedTime, void * pUserData );

        void AddToQueue( DialogQueueType* queue, rmt::Vector* posn );
        void RemoveSelfFromList();

        void PlayDialog( SimpsonsSoundPlayer& player1,
                         SimpsonsSoundPlayer& player2,
                         DialogLineCompleteCallback* lineCallback,
                         DialogCompleteCallback* dialogCallback );
        void StopDialog();

        DialogPriority GetPriority() { return( CalculateDialogPriority( *m_dialog ) ); }

        bool DialogMatches( SelectableDialog* dialog ) { return( dialog == m_dialog ); }

        rmt::Vector* GetPosition();

        void FillDebugInfo( DialogSoundDebugPage& debugInfo, unsigned int lineNum );

        //
        // SimpsonsSoundPlayer callbacks
        //
        void OnPlaybackComplete();
        void OnSoundReady();

        static DialogPriority CalculateDialogPriority( const SelectableDialog& dialog );
        static unsigned int CalculateDialogProbability( const SelectableDialog& dialog );
        static void Service();

        static const unsigned int MAX_QUEUE_ELEMENTS = 16;

    private:
        //Prevent wasteful constructor creation.
        DialogQueueElement();
        DialogQueueElement( const DialogQueueElement& original );
        DialogQueueElement& operator=( const DialogQueueElement& rhs );

        bool dialogLineIsWalker( unsigned int lineNum );
        Character* dialogLineIsNPC( unsigned int lineNum );

        bool isMouthFlappingEvent( EventEnum theEvent );

        static IRadTimerList* s_timerList;

        static bool s_watcherInitialized;
        
        SelectableDialog* m_dialog;
        IRadTimer* m_timer;

        SimpsonsSoundPlayer* m_player1;
        SimpsonsSoundPlayer* m_player2;

        DialogLineCompleteCallback* m_lineDoneCallback;
        DialogCompleteCallback* m_dialogDoneCallback;

        unsigned int m_linesPlayed;

        DialogQueueType* m_queue;

        rmt::Vector m_position;
        bool m_hasPosition;
};


#endif // DIALOGQUEUEELEMENT_H

