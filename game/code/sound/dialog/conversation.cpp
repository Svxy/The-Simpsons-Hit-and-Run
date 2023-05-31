//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        conversation.cpp
//
// Description: Conversation objects aggregate groups of DialogLine objects 
//              which are intended to be played without interruption, like 
//              a conversation.  Hence the name.
//
// History:     02/09/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <stdio.h>
#include <raddebug.hpp>

//========================================
// Project Includes
//========================================
#include <sound/dialog/conversation.h>

#include <sound/dialog/dialogline.h>
#include <sound/dialog/dialogselectiongroup.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// Conversation::Conversation
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
Conversation::Conversation( DialogLine& line ) :
    PlayableDialog( line.GetLevel(), line.GetMission(), line.GetEvent() ),
    m_maxOrderNumber( line.GetConversationPosition() ),
    m_dialogList( NULL ),
    m_currentLine( NULL )
{
    line.AddToDialogList( reinterpret_cast<SelectableDialog**>(&m_dialogList) );
}

//==============================================================================
// Conversation::~Conversation
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
Conversation::~Conversation()
{
}

//=============================================================================
// Conversation::LineFits
//=============================================================================
// Description: Determine whether the given line matches this conversation
//
// Parameters:  line - dialog line to test
//
// Return:      true if it fits in this conversation, false otherwise
//
//=============================================================================
bool Conversation::LineFits( DialogLine& line )
{
    bool matches;
    DialogLine* existingLine = m_dialogList;

    rAssert( existingLine != NULL );

    //
    // If the new line matches one dialog line in this conversation, it should
    // match them all
    //
    matches = ( line.GetLevel() == existingLine->GetLevel() )
              && ( line.GetMission() == existingLine->GetMission() )
              && ( line.GetEvent() == existingLine->GetEvent() )
              && ( line.GetConversationName() == existingLine->GetConversationName() );

#ifdef RAD_DEBUG
    if( matches )
    {
        rAssert( line.GetConversationPosition() != existingLine->GetConversationPosition() );
    }
#endif

    return( matches );
}

//=============================================================================
// Conversation::AddToConversation
//=============================================================================
// Description: Add the given dialog line to this conversation
//
// Parameters:  line - line to be added
//
// Return:      void 
//
//=============================================================================
void Conversation::AddToConversation( DialogLine& line )
{
    int position = line.GetConversationPosition();
    DialogLine* listObj;
    DialogLine* nextListObj;

    if( ( m_dialogList == NULL ) 
        || ( position < m_dialogList->GetConversationPosition() ) )
    {
        line.AddToDialogList( reinterpret_cast<SelectableDialog**>(&m_dialogList) );
    }
    else
    {
        listObj = m_dialogList;
        // ARGH!  Stinky downcast!  I hate this!
        nextListObj = reinterpret_cast<DialogLine*>(listObj->GetNextInList());
        while( ( nextListObj != NULL ) 
               && ( nextListObj->GetConversationPosition() < position ) )
        {
            listObj = nextListObj;
            nextListObj = reinterpret_cast<DialogLine*>(listObj->GetNextInList());
        }

        line.AddToDialogList( listObj );
    }
}

//=============================================================================
// Conversation::IsComplete
//=============================================================================
// Description: Returns true if we have a set of consecutively-numbered 
//              dialog lines, starting from 1.
//
// Parameters:  None
//
// Return:      true if conversation valid and complete, false otherwise
//
//=============================================================================
bool Conversation::IsComplete()
{
    bool complete = true;
    int nextPosition = 1;
    DialogLine* nextDialogLine = m_dialogList;

    while( nextDialogLine != NULL )
    {
        if( nextDialogLine->GetConversationPosition() != nextPosition )
        {
            complete = false;
            break;
        }

        // Stinky downcast!
        nextDialogLine = reinterpret_cast<DialogLine*>(nextDialogLine->GetNextInList());
        ++nextPosition;
    }

    return( complete );
}

//=============================================================================
// Conversation::PlayLine
//=============================================================================
// Description: Comment
//
// Parameters:  lineIndex - index for the DialogLine we want to play
//              player - player to play it with
//              callback - callback to call when we're done
//
// Return:      void 
//
//=============================================================================
void Conversation::PlayLine( unsigned int lineIndex, 
                             SimpsonsSoundPlayer& player,
                             SimpsonsSoundPlayerCallback* callback )
{
    DialogLine* currentDialog = findDialogLineByIndex( lineIndex );

    currentDialog->PlayLine( 0, player, callback );
}

void Conversation::QueueLine( unsigned int lineIndex, SimpsonsSoundPlayer& player )
{
    m_currentLine = findDialogLineByIndex( lineIndex );

    m_currentLine->QueueLine( 0, player );
}

void Conversation::PlayQueuedLine( SimpsonsSoundPlayer& player,
                                   SimpsonsSoundPlayerCallback* callback )
{
    m_currentLine->PlayQueuedLine( player, callback );
}

//=============================================================================
// Conversation::GetNumDialogLines
//=============================================================================
// Description: Returns the number of DialogLine objects associated with
//              this conversation.
//
// Parameters:  None
//
// Return:      number of dialog lines
//
//=============================================================================
unsigned int Conversation::GetNumDialogLines() const
{
    unsigned int lineCount = 0;
    DialogLine* lineObj = m_dialogList;

    while( lineObj != NULL )
    {
        ++lineCount;
        lineObj = static_cast<DialogLine*>(lineObj->GetNextInList());
    }

    return( lineCount );
}

//=============================================================================
// Conversation::UsesCharacter
//=============================================================================
// Description: Indicate if any of our dialog lines are from the given
//              character
//
// Parameters:  characterObj - character to match
//
// Return:      true if >=1 dialog line comes from the given character,
//              false otherwise
//
//=============================================================================
bool Conversation::UsesCharacter( tUID characterUID )
{
    bool match = false;
    DialogLine* lineObj = m_dialogList;

    while( lineObj != NULL )
    {
        if( lineObj->UsesCharacter( characterUID ) )
        {
            match = true;
            break;
        }
        lineObj = static_cast<DialogLine*>(lineObj->GetNextInList());
    }

    return( match );
}

//=============================================================================
// Conversation::IsVillainLine
//=============================================================================
// Description: Indicate if this is a villain one-liner (answer: no)
//
// Parameters:  None
//
// Return:      false 
//
//=============================================================================
bool Conversation::IsVillainLine()
{
    //
    // No conversations are villain one-liners
    //
    return( false );
}

//=============================================================================
// Conversation::GetDialogLineCharacterUID
//=============================================================================
// Description: Indicate which character is speaking with the given line
//
// Parameters:  lineNum - indicates which dialog line we want the UID for
//
// Return:      tUID of character associated with dialog line
//
//=============================================================================
tUID Conversation::GetDialogLineCharacterUID( unsigned int lineNum )
{
    unsigned int lineCount = lineNum;
    DialogLine* lineObj = m_dialogList;

    rAssert( lineNum > 0 );

    while( ( --lineCount > 0 ) && ( lineObj != NULL ) )
    {
        lineObj = static_cast<DialogLine*>(lineObj->GetNextInList());
    }

    if( lineObj != NULL )
    {
        return( lineObj->GetCharacterUID() );
    }
    else
    {
        return( 0 );
    }
}

//=============================================================================
// Conversation::GetConversationName
//=============================================================================
// Description: I don't really expect this to be used, but that's no reason
//              to return garbage here.  Find the name stored with one of the
//              individual lines.
//
// Parameters:  None
//
// Return:      radKey32 representing conversation name field
//
//=============================================================================
radKey32 Conversation::GetConversationName()
{
    rAssert( m_dialogList != NULL );

    return( m_dialogList->GetConversationName() );
}

//=============================================================================
// Conversation::AddMatchingDialog
//=============================================================================
// Description: Remove self from the given list and add ourself and the new
//              dialog back in within a DialogSelectionGroup object
//
// Parameters:  newDialog - new dialog with same characteristics as self
//              list - list to add to
//
// Return:      void 
//
//=============================================================================
void Conversation::AddMatchingDialog( SelectableDialog& newDialog, SelectableDialogList& list )
{
    HeapMgr()->PushHeap( GMA_AUDIO_PERSISTENT );

    DialogSelectionGroup* group = new DialogSelectionGroup( *this, newDialog );

    rAssert( group != NULL );

    list.remove( this );
    list.push_back( group );

    HeapMgr()->PopHeap( GMA_AUDIO_PERSISTENT );
}

//=============================================================================
// Conversation::PrintDialogLineNames
//=============================================================================
// Description: For debugging.  Print the names of the dialog lines.
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void Conversation::PrintDialogLineNames()
{
#ifndef RAD_RELEASE
    DialogLine* currentDialog = m_dialogList;

    rDebugString( "Conversation contents:\n" );
    while( currentDialog != NULL )
    {
        currentDialog->PrintResourceName();
        currentDialog = static_cast<DialogLine*>(currentDialog->GetNextInList());
    }
#endif
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

DialogLine* Conversation::findDialogLineByIndex( unsigned int lineIndex )
{
    unsigned int lineCount = lineIndex;
    DialogLine* currentDialog = m_dialogList;

    rAssert( currentDialog != NULL );
    
    while( lineCount > 0 )
    {
        currentDialog = static_cast<DialogLine*>(currentDialog->GetNextInList());
        rAssert( currentDialog != NULL );
        --lineCount;
    }

    return( currentDialog );
}