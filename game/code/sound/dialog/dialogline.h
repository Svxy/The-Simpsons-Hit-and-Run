//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        dialogline.h
//
// Description: Atomic unit of dialog.  A DialogLine object represents a 
//              complete line of dialog spoken by a single character.
//
// History:     02/09/2002 + Created -- Darren
//
//=============================================================================

#ifndef DIALOGLINE_H
#define DIALOGLINE_H

//========================================
// Nested Includes
//========================================
#include <p3d/p3dtypes.hpp>

#include <sound/dialog/playabledialog.h>
#include <sound/dialog/selectabledialoglist.h>

#include <sound/soundrenderer/soundsystem.h>
#include <sound/soundrenderer/soundresource.h>
//========================================
// Forward References
//========================================

class Character;

//
// Table entry structure for mapping between event strings in filenames
// and events responded to in dialog system
//
struct EventTableEntry
{
    const char* eventString;
    radKey32 eventKey;
    EventEnum event;
    unsigned int lifeInMsecs;
};

//
// Table entry structure for mapping between character strings in filenames
// and tUIDs that we pull out of the Character objects.
//
struct CharacterTableEntry
{
    radInt64 realCharacterUID;
    const char* characterString;
    radKey32 characterKey;
    const char* realCharacterName;
};

const char ROLE_NONE = 0;
const char ROLE_WALKER = 1;
const char ROLE_DRIVER = 2;
const char ROLE_PEDESTRIAN = 3;
const char ROLE_VILLAIN = 4;

typedef char DialogRole;

//=============================================================================
//
// Synopsis:    DialogLine
//
//=============================================================================

class DialogLine : public PlayableDialog
{
    public:
        DialogLine( IDaSoundResource* resource );
        virtual ~DialogLine();

        static const int NOT_CONVERSATION_LINE = -1;

        static const int BONUS_MISSION_NUMBER = 8;
        static const int FIRST_RACE_MISSION_NUMBER = 9;
        static const int TUTORIAL_MISSION_NUMBER = 12;

        int GetConversationPosition() { return( m_conversationPosition ); }
        tUID GetCharacterUID();
        tUID GetDialogLineCharacterUID( unsigned int lineNum );
        bool IsVillainLine() { return( m_role == ROLE_VILLAIN ); }

        inline radKey32 GetConversationName( );

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

        unsigned int GetNumDialogLines() const { return( 1 ); }
        bool UsesCharacter( tUID characterUID );
        void AddMatchingDialog( SelectableDialog& newDialog, SelectableDialogList& list );

        static bool IsFoodCharacter( Character* theGuy );
        static unsigned int GetLifeInMsecsForEvent( EventEnum eventID );
        
        //
        // Utility for stripping the directory crud off of filenames
        //
        static void StripDirectoryCrud( const char* filename, char* buffer, int bufferLen );

        //
        // Accessors for tables
        //
        static const EventTableEntry* GetEventTableEntry( unsigned int index );
        static unsigned int GetEventTableSize();
        
        static const CharacterTableEntry* GetCharacterTableEntry( unsigned int index );
        static unsigned int GetCharacterTableSize();

        static void FillEventName( char* buffer, unsigned int bufferSize, EventEnum eventID );
        static void FillCharacterName( char* buffer, unsigned int bufferSize, tUID characterUID );

        //
        // For debugging
        //
        void PrintResourceName();

    private:
        //Prevent wasteful constructor creation.
        DialogLine();
        DialogLine( const DialogLine& original );
        DialogLine& operator=( const DialogLine& rhs );

        void parseResourceFilename();
        bool getNameField( const char* filename, int field, char* buffer, int bufferLen );
        void initializeTables();

        void matchRoleField( const char* filename, int field );
        void matchOrderField( const char* filename, int field );
        void matchEventField( const char* filename, int field );
        void matchCharacterField( const char* filename, int field );
        void matchLevelField( const char* filename, int field );
        void parseConversationName( );
        //
        // Sound resource to play
        //
        IDaSoundResource* m_resource;

        radKey32 m_ConversationName;

        DialogRole /* (char)*/ m_role;

        //
        // Position within conversation if this is a conversation line
        //
        
        char m_conversationPosition;
        
        //
        // Character that this line belongs to
        //
        char m_characterIndex;        
};

inline radKey32 DialogLine::GetConversationName( )
{
    return m_ConversationName;
}

#endif // DIALOGLINE_H

