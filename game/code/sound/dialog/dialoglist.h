//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        dialoglist.h
//
// Description: Loads and maintains the list of dialog lines and conversations
//              (which group multiple dialog lines, and potentially link 
//              conversations to other conversations that occur later).
//
// History:     01/09/2002 + Created -- Darren
//
//=============================================================================

#ifndef DIALOGLIST_H
#define DIALOGLIST_H

//========================================
// Nested Includes
//========================================
#include <mission/gameplaymanager.h>
#include <events/eventenum.h>
#include <sound/dialog/selectabledialoglist.h>

//========================================
// Forward References
//========================================
struct IRadNameSpace;
struct IDaSoundResource;
class SelectableDialog;
class Character;

//=============================================================================
//
// Synopsis:    DialogList
//
//=============================================================================

class DialogList
{
    public:
        DialogList();
        virtual ~DialogList();

        void OrganizeDialog( IRadNameSpace* namespaceObj );
        
        SelectableDialog* FindDialogForEvent( EventEnum id, Character* character1, Character* character2, 
                                              tUID charUID1, tUID charUID2, radKey32 convKey, bool isVillain );

        static Character* GetStinkySkinPointer( tUID charUID );

    private:
        //Prevent wasteful constructor creation.
        DialogList( const DialogList& original );
        DialogList& operator=( const DialogList& rhs );

        //
        // Sound file naming convention tests
        //
        bool isIndividualLine( IDaSoundResource* resource );
        bool isConversationLine( IDaSoundResource* resource );

        bool hasConversationPrefix( const char* name ) 
            { return( ( name[0] == 'C' ) && ( name[1] == '_' ) ); }
        bool hasOneLinerPrefix( const char* name );
        unsigned int underscoreCount( const char* name );

        SelectableDialog* searchDialogList( EventEnum id, tUID characterUID1, tUID characterUID2,
                                            SelectableDialogList& list, radKey32 convName, bool isVillain,
                                            bool fuzzyPedMatch );

        SelectableDialog* searchDialogList( EventEnum id, tUID characterUID1, tUID characterUID2,
                                            SelectableDialogList& list, radKey32 convName, bool isVillain );

        SelectableDialog* searchDialogList( EventEnum id, Character* character1,
                                            Character* character2, SelectableDialogList& list,
                                            radKey32 convName, bool isVillain );

        bool characterMatches( tUID characterUID, SelectableDialog* dialog, bool fuzzyPedMatch );

        tUID getPuppetUID( Character* characterPtr );

        static void dumpDialogCoverage( void* userData );

        //
        // List of level/mission dialogs
        //
        SelectableDialogList m_missionLists[GameplayManager::MAX_LEVELS][GameplayManager::MAX_MISSIONS+1];

        //
        // Generic dialog list
        //
        SelectableDialogList m_genericDialogList;

        static radKey32 s_introKey;
        static radKey32 s_aztecKey;
        static tUID s_milhouseKey;
        static tUID s_nelsonKey;
        static tUID s_raceZombie1;
        static tUID s_raceZombie2;

        //
        // Debug flag
        //
        static bool s_showDialogSpew;
};


#endif // DIALOGLIST_H

