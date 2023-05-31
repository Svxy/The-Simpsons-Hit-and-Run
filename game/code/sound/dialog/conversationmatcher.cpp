//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        conversationmatcher.cpp
//
// Description: Takes sound resource names which form individual dialog lines
//              and groups them into the intended conversations.
//
// History:     02/09/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <sound/dialog/conversationmatcher.h>

#include <sound/dialog/conversation.h>
#include <sound/dialog/dialogline.h>
#include <sound/dialog/dialoglist.h>

#include <memory/srrmemory.h>

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
// ConversationMatcher::ConversationMatcher
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
ConversationMatcher::ConversationMatcher() :
    m_conversationList( NULL )
{
}

//==============================================================================
// ConversationMatcher::~ConversationMatcher
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
ConversationMatcher::~ConversationMatcher()
{
}

//=============================================================================
// ConversationMatcher::AddNewLine
//=============================================================================
// Description: Make a new DialogLine object for the resource and add it to
//              the list of conversations to be grouped together.
//
// Parameters:  resource - conversation sound resource
//
// Return:      void 
//
//=============================================================================
void ConversationMatcher::AddNewLine( IDaSoundResource* resource )
{
    DialogLine* line;
    Conversation* conversationObj;
    Conversation* newConversation;
        
    //
    // Create a DialogLine object
    //
#ifdef RAD_GAMECUBE
    line = new( GMA_GC_VMM ) DialogLine( resource );
#else
    line = new( GMA_PERSISTENT ) DialogLine( resource );
#endif
    rAssert( line != NULL );

    //
    // Work through the conversation list, trying to find an existing
    // conversation that matches this line
    //
    conversationObj = m_conversationList;

    while( conversationObj != NULL )
    {
        if( conversationObj->LineFits( *line ) )
        {
            conversationObj->AddToConversation( *line );
            break;
        }

        //
        // Argh!  This must go.  Stinky downcast.
        //
        conversationObj = reinterpret_cast<Conversation*>(conversationObj->GetNextInList());
    }

    if( conversationObj == NULL )
    {
        //
        // No conversation matched, create a new conversation
        //
#ifdef RAD_GAMECUBE
        newConversation = new( GMA_GC_VMM ) Conversation( *line );
#else
        newConversation = new( GMA_PERSISTENT ) Conversation( *line );
#endif
        newConversation->AddToDialogList( reinterpret_cast<SelectableDialog**>(&m_conversationList) );
    }
}

//=============================================================================
// ConversationMatcher::AreAllConversationsComplete
//=============================================================================
// Description: Determine whether all the conversations have been filled out.
//              Sanity check function.
//
// Parameters:  None
//
// Return:      true if all conversations complete, false otherwise 
//
//=============================================================================
bool ConversationMatcher::AreAllConversationsComplete()
{
    Conversation* current;
    bool allComplete = true;

    current = m_conversationList;
    while( current != NULL )
    {
        if( !(current->IsComplete()) )
        {
            allComplete = false;
            current->PrintDialogLineNames();

            //
            // We know we're incomplete at this point, but keep going so that
            // we get everything printed out in one go.
            //
        }

        current = reinterpret_cast<Conversation*>(current->GetNextInList());
    }

    return( allComplete );
}

//=============================================================================
// ConversationMatcher::AddConversationsToList
//=============================================================================
// Description: Transfer all of our conversations from our own list to the
//              list given that match the specification
//
// Parameters:  level - level of conversations for list (NO_LEVEL if N/A)
//              mission - mission of conversations for list (NO_MISSION if N/A)
//              list - list to add to
//
// Return:      void 
//
//=============================================================================
void ConversationMatcher::AddConversationsToList( unsigned int level, 
                                                  unsigned int mission, 
                                                  SelectableDialogList& list )
{
    HeapMgr()->PushHeap( GMA_AUDIO_PERSISTENT );

    Conversation* conversationObj;
    Conversation* nextConversation = NULL;
    Conversation* temp;

    //
    // First, deal with the head of the list as a special case.  This can
    // probably be cleaned up
    //
    while( ( m_conversationList != NULL )
           && ( m_conversationList->GetLevel() == level )
           && ( m_conversationList->GetMission() == mission ) )
    {
        temp = m_conversationList;
        m_conversationList = reinterpret_cast<Conversation*>( m_conversationList->GetNextInList() );
        list.push_back( temp );
    }

    //
    // Now, the first one in the list, if it exists, doesn't match the spec
    //
    conversationObj = m_conversationList;
    if( conversationObj != NULL )
    {
        nextConversation = reinterpret_cast<Conversation*>( conversationObj->GetNextInList() );
    }
    
    while( ( conversationObj != NULL )
           && ( nextConversation != NULL ) )
    {
        if( ( nextConversation->GetLevel() == level )
            && ( nextConversation->GetMission() == mission ) )
        {
            //
            // Remove conversation from our list and add it to the one supplied
            //
            conversationObj->RemoveNextFromList();
            list.push_back( nextConversation );
        }
        else
        {
            conversationObj = nextConversation;
        }

        nextConversation = reinterpret_cast<Conversation*>( conversationObj->GetNextInList() );
    }
    HeapMgr()->PopHeap( GMA_AUDIO_PERSISTENT );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
