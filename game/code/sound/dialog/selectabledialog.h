//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        selectabledialog.h
//
// Description: Abstract base class for groups of one or more lines of dialog,
//              any of which can be selected randomly in response to a single
//              sound event.
//
// History:     02/09/2002 + Created -- Darren
//
//=============================================================================

#ifndef SELECTABLEDIALOG_H
#define SELECTABLEDIALOG_H

//========================================
// Nested Includes
//========================================
#include <events/eventenum.h>
#include <worldsim/character/character.h>
#include <sound/dialog/selectabledialoglist.h>

//========================================
// Forward References
//========================================
class SimpsonsSoundPlayer;
struct SimpsonsSoundPlayerCallback;

//=============================================================================
//
// Synopsis:    SelectableDialog
//
//=============================================================================

class SelectableDialog
{
    public:
        SelectableDialog();
        SelectableDialog( unsigned int level, unsigned int mission, EventEnum event );
        virtual ~SelectableDialog();

        static const int NO_MISSION = 0;
        static const int NO_LEVEL = 0;

        bool IsLevelSpecific() { return( m_levelNum != NO_LEVEL ); }

        virtual unsigned int GetMission() const;
        virtual unsigned int GetLevel() const;
        virtual EventEnum GetEvent() const;
        
        virtual bool UsesCharacter( Character* characterObj ) 
            { return( UsesCharacter( characterObj->GetUID() ) ); }
        virtual bool UsesCharacter( tUID characterUID ) = 0;

        virtual tUID GetDialogLineCharacterUID( unsigned int lineNum ) = 0;

        virtual bool IsVillainLine() = 0;

        void AddToDialogList( SelectableDialog** list );
        void AddToDialogList( SelectableDialog* listObj );
        SelectableDialog* GetNextInList() const { return( m_nextListObject ); }
        void RemoveNextFromList();

        virtual void AddMatchingDialog( SelectableDialog& newDialog, SelectableDialogList& list ) = 0;

        virtual void PlayLine( unsigned int lineIndex,
                               SimpsonsSoundPlayer& player,
                               SimpsonsSoundPlayerCallback* callback ) = 0;
        virtual void QueueLine( unsigned int lineIndex,
                                SimpsonsSoundPlayer& player ) = 0;
        virtual void PlayQueuedLine( SimpsonsSoundPlayer& player,
                                     SimpsonsSoundPlayerCallback* callback ) = 0;

        virtual unsigned int GetNumDialogLines() const = 0;

        virtual radKey32 GetConversationName() = 0;

#ifndef RAD_RELEASE
        void MarkAsPlayed();
        void PrintPlayedStatus();
#endif

    protected:
        //
        // Level, mission, and event values.  Since these aren't really used
        // in DialogSelectionGroup and Conversation objects, they should
        // probably be separated out of this object someday if we want to
        // save a few bytes and clean up the design a bit.
        //

        //
        // Level and mission number for this line.  Set to NO_MISSION or
        // NO_LEVEL if not applicable.
        //
        int m_missionNum;
        int m_levelNum;

        //
        // Event that this dialog is played in response to
        //
        EventEnum m_event;

    private:
        //Prevent wasteful constructor creation.
        SelectableDialog( const SelectableDialog& original );
        SelectableDialog& operator=( const SelectableDialog& rhs );

#ifndef RAD_RELEASE
        bool m_hasBeenPlayed;
#endif

        SelectableDialog* m_nextListObject;
};


#endif // SELECTABLEDIALOG_H

