//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        dialogsounddebugpage.cpp
//
// Description: Implements a class for displaying dialog-related debug info 
//              on the screen, triggered using Watcher
//
// History:     1/20/2003 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <sound/dialog/dialogsounddebugpage.h>

#include <sound/dialog/dialogline.h>

//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// DialogSoundDebugPage::DialogSoundDebugPage
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
DialogSoundDebugPage::DialogSoundDebugPage( unsigned int pageNum, SoundDebugDisplay* master ) :
    SoundDebugPage( pageNum, master )
{
    unsigned int i;

    //
    // Mark queue entries as empty by setting event ID to NUM_EVENTS
    //
    for( i = 0; i < MAX_QUEUE_SIZE; i++ )
    {
        m_queueData[i].eventID = NUM_EVENTS;
    }
}

//=============================================================================
// DialogSoundDebugPage::~DialogSoundDebugPage
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
DialogSoundDebugPage::~DialogSoundDebugPage()
{
}

//=============================================================================
// DialogSoundDebugPage::SetQueueLength
//=============================================================================
// Description: Given queue length, clear out entries that are no longer in
//              the queue
//
// Parameters:  size - current size of queue
//
// Return:      void 
//
//=============================================================================
void DialogSoundDebugPage::SetQueueLength( unsigned int size )
{
    unsigned int i;

    for( i = size; i < MAX_QUEUE_SIZE; i++ )
    {
        m_queueData[i].eventID = NUM_EVENTS;
    }
}

//=============================================================================
// DialogSoundDebugPage::SetQueueEntry
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int position, EventEnum event, unsigned int mission, unsigned int level, tUID theCharacter, DialogPriority priority, unsigned int msecsRemaining )
//
// Return:      void 
//
//=============================================================================
void DialogSoundDebugPage::SetQueueEntry( unsigned int position,
                                          EventEnum event,
                                          unsigned int mission,
                                          unsigned int level,
                                          tUID theCharacter,
                                          DialogPriority priority,
                                          unsigned int msecsRemaining )
{
    QueueInfo* data;

    if( position >= MAX_QUEUE_SIZE )
    {
        //
        // Don't bother displaying too much stuff
        //
        return;
    }

    data = &m_queueData[position];
    data->eventID = event;
    data->level = level;
    data->mission = mission;
    data->theCharacter = theCharacter;
    data->priority = priority;
    data->msecsRemaining = msecsRemaining;
}

//*****************************************************************************
//
// Protected Member Functions
//
//*****************************************************************************

//=============================================================================
// DialogSoundDebugPage::fillLineBuffer
//=============================================================================
// Description: Fill the given buffer with text to display on the screen
//              at the given line
//
// Parameters:  lineNum - line number on screen where buffer will be displayed
//              buffer - to be filled in with text to display
//
// Return:      void 
//
//=============================================================================
void DialogSoundDebugPage::fillLineBuffer( int lineNum, char* buffer )
{
    switch( lineNum )
    {
        case 0:
            strcpy( buffer, "Now playing:" );
            break;

        case 1:
            fillQueueText( buffer, 0 );
            break;

        case 3:
            strcpy( buffer, "Queued:" );
            break;

        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
            fillQueueText( buffer, lineNum - 3 );
            break;

        default:
            buffer[0] = '\0';
            break;
    }
}

//=============================================================================
// DialogSoundDebugPage::getNumLines
//=============================================================================
// Description: Returns number of lines that we'll display on screen
//
// Parameters:  None
//
// Return:      Line count 
//
//=============================================================================
int DialogSoundDebugPage::getNumLines()
{
    return( 13 );
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************

//=============================================================================
// DialogSoundDebugPage::fillQueueText
//=============================================================================
// Description: Fill the text buffer with information about one particular
//              dialog queue entry
//
// Parameters:  buffer - text buffer to fill
//              index - index of m_queueData entry to fill out with
//
// Return:      void 
//
//=============================================================================
void DialogSoundDebugPage::fillQueueText( char* buffer, unsigned int index )
{
    QueueInfo* info;
    char eventName[30];
    char characterName[10];

    rAssert( index >= 0 );
    rAssert( index <= MAX_QUEUE_SIZE );
    rAssert( buffer != NULL );

    info = &m_queueData[index];
    if( info->eventID != NUM_EVENTS )
    {
        DialogLine::FillEventName( eventName, 30, info->eventID );
        DialogLine::FillCharacterName( characterName, 10, info->theCharacter );
        sprintf( buffer, "Event: %s Char: %s Pri: %d L%dM%d",
                 eventName,
                 characterName,
                 info->priority,
                 info->level,
                 info->mission );
    }
    else
    {
        buffer[0] = '\0';
    }
}
