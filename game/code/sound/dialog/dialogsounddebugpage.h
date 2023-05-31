//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        dialogsounddebugpage.h
//
// Description: Declares a class for displaying dialog-related debug info 
//              on the screen, triggered using Watcher
//
// History:     1/20/2003 + Created -- Darren
//
//=============================================================================

#ifndef DIALOGSOUNDDEBUGPAGE_H
#define DIALOGSOUNDDEBUGPAGE_H

//========================================
// Nested Includes
//========================================
#include <sound/sounddebug/sounddebugpage.h>

#include <events/eventenum.h>
#include <sound/dialog/dialogqueueelement.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    DialogSoundDebugPage
//
//=============================================================================

class DialogSoundDebugPage : public SoundDebugPage
{
    public:
        DialogSoundDebugPage( unsigned int pageNum, SoundDebugDisplay* master );
        virtual ~DialogSoundDebugPage();

        void SetQueueLength( unsigned int size );

        void SetQueueEntry( unsigned int position,
                            EventEnum event,
                            unsigned int mission,
                            unsigned int level,
                            tUID theCharacter,
                            DialogPriority priority,
                            unsigned int msecsRemaining );

    protected:
        //
        // Pure virtual functions from SoundDebugPage
        //
        void fillLineBuffer( int lineNum, char* buffer );
        int getNumLines();

    private:
        //Prevent wasteful constructor creation.
        DialogSoundDebugPage();
        DialogSoundDebugPage( const DialogSoundDebugPage& dialogsounddebugpage );
        DialogSoundDebugPage& operator=( const DialogSoundDebugPage& dialogsounddebugpage );

        void fillQueueText( char* buffer, unsigned int index );

        //
        // Structure for holding queue information
        //
        struct QueueInfo
        {
            EventEnum eventID;
            unsigned int mission;
            unsigned int level;
            tUID theCharacter;
            DialogPriority priority;
            unsigned int msecsRemaining;
        };

        static const unsigned int MAX_QUEUE_SIZE = 10;

        QueueInfo m_queueData[MAX_QUEUE_SIZE];
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

#endif //DIALOGSOUNDDEBUGPAGE_H
