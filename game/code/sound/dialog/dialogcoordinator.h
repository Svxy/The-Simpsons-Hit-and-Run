//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        dialogcoordinator.h
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

#ifndef DIALOGCOORDINATOR_H
#define DIALOGCOORDINATOR_H

//========================================
// Nested Includes
//========================================
#include <sound/dialog/dialogpriorityqueue.h>
#include <events/eventlistener.h>

//========================================
// Forward References
//========================================

struct IRadNameSpace;
class DialogList;
class Character;
class Vehicle;

//=============================================================================
//
// Synopsis:    DialogCoordinator
//
//=============================================================================

class DialogCoordinator : public EventListener
{
    public:
        DialogCoordinator( IRadNameSpace* namespaceObj );
        virtual ~DialogCoordinator();
        
        void Initialize();
        void OnGameplayEnd() { m_playbackQueue->StopAllDialog(); }

        void OnPauseStart() { m_playbackQueue->PauseDialog(); }
        void OnPauseEnd() { m_playbackQueue->UnpauseDialog(); }

        void HandleEvent( EventEnum id, void* pEventData );

        void ServiceOncePerFrame() { m_playbackQueue->ServiceOncePerFrame(); }

    private:
        //Prevent wasteful constructor creation.
        DialogCoordinator();
        DialogCoordinator( const DialogCoordinator& original );
        DialogCoordinator& operator=( const DialogCoordinator& rhs );

        void registerDialogEvents();
        void queueVillainDialog( EventEnum id, void* eventData );
        bool playLinePositionally( EventEnum id );
        void getCharacterPosition( Character* thePed, rmt::Vector& posn );
        bool eventHasVehicleData( EventEnum id );

        IRadNameSpace* m_dialogNamespace;

        DialogList* m_dialogList;
        DialogPriorityQueue* m_playbackQueue;

        bool m_dialogOn;

        //
        // Hack!
        //
        bool m_phoneBoothRequestMade;
};


#endif // DIALOGCOORDINATOR_H

