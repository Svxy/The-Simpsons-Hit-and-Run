//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        conversationmatcher.h
//
// Description: Takes sound resource names which form individual dialog lines
//              and groups them into the intended conversations.
//
// History:     02/09/2002 + Created -- Darren
//
//=============================================================================

#ifndef CONVERSATIONMATCHER_H
#define CONVERSATIONMATCHER_H

//========================================
// Nested Includes
//========================================
//
// I'd like a forward declaration, but it's a little complicated with
// STL lists
//
#include <sound/dialog/selectabledialoglist.h>

//========================================
// Forward References
//========================================
struct IDaSoundResource;
class Conversation;

//=============================================================================
//
// Synopsis:    ConversationMatcher
//
//=============================================================================

class ConversationMatcher
{
    public:
        ConversationMatcher();
        virtual ~ConversationMatcher();

        void AddNewLine( IDaSoundResource* resource );
        bool AreAllConversationsComplete();
        void AddConversationsToList( unsigned int level, unsigned int mission, SelectableDialogList& list );

    private:
        //Prevent wasteful constructor creation.
        ConversationMatcher( const ConversationMatcher& original );
        ConversationMatcher& operator=( const ConversationMatcher& rhs );

        Conversation* m_conversationList;
};


#endif // CONVERSATIONMATCHER_H

