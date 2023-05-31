//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        conversation.h
//
// Description: Conversation objects aggregate groups of DialogLine objects 
//              which are intended to be played without interruption, like 
//              a conversation.  Hence the name.
//
// History:     02/09/2002 + Created -- Darren
//
//=============================================================================

#ifndef CONVERSATION_H
#define CONVERSATION_H

//========================================
// Nested Includes
//========================================
#include <sound/dialog/playabledialog.h>

//========================================
// Forward References
//========================================
class DialogLine;

//=============================================================================
//
// Synopsis:    Conversation
//
//=============================================================================

class Conversation : public PlayableDialog
{
    public:
        Conversation( DialogLine& line );
        virtual ~Conversation();

        bool LineFits( DialogLine& line );
        void AddToConversation( DialogLine& line );
        bool IsComplete();

        //
        // Pure virtual functions from SelectableDialog
        //
        void PlayLine( unsigned int lineIndex,
                       SimpsonsSoundPlayer& player,
                       SimpsonsSoundPlayerCallback* callback );
        void QueueLine( unsigned int lineIndex,
                        SimpsonsSoundPlayer& player );
        void PlayQueuedLine( SimpsonsSoundPlayer& player,
                             SimpsonsSoundPlayerCallback* callback );

        unsigned int GetNumDialogLines() const;
        bool UsesCharacter( tUID characterUID );
        tUID GetDialogLineCharacterUID( unsigned int lineNum );
        radKey32 GetConversationName();
        bool IsVillainLine();

        void AddMatchingDialog( SelectableDialog& newDialog, SelectableDialogList& list );
        
        void PrintDialogLineNames();

    private:
        //Prevent wasteful constructor creation.
        Conversation();
        Conversation( const Conversation& original );
        Conversation& operator=( const Conversation& rhs );

        DialogLine* findDialogLineByIndex( unsigned int lineIndex );

        unsigned int m_maxOrderNumber;
        DialogLine* m_dialogList;
        DialogLine* m_currentLine;
};


#endif // CONVERSATION_H

