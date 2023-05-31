//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        dialogselectiongroup.h
//
// Description: Represents a set of dialog lines or conversations that can 
//              be randomly selected without affecting gameplay.  For example, 
//              if Homer had three generic lines for playback whenever he is 
//              hit by a car, and all three can be substituted for each other 
//              freely, then those lines would be grouped by a DialogSelectionGroup 
//              object.
//
// History:     02/09/2002 + Created -- Darren
//
//=============================================================================

#ifndef DIALOGSELECTIONGROUP_H
#define DIALOGSELECTIONGROUP_H

//========================================
// Nested Includes
//========================================
#include <vector>

#include <sound/dialog/selectabledialog.h>

//========================================
// Forward References
//========================================
class SimpsonsSoundPlayer;
struct SimpsonsSoundPlayerCallback;

//=============================================================================
//
// Synopsis:    DialogSelectionGroup
//
//=============================================================================

class DialogSelectionGroup : public SelectableDialog
{
    public:
        DialogSelectionGroup( SelectableDialog& dialog1, SelectableDialog& dialog2 );
        virtual ~DialogSelectionGroup();
        
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

        //
        // Overriding SelectableDialog methods
        //
        unsigned int GetMission() const;
        unsigned int GetLevel() const;
        EventEnum GetEvent() const;

    private:
        //Prevent wasteful constructor creation.
        DialogSelectionGroup();
        DialogSelectionGroup( const DialogSelectionGroup& original );
        DialogSelectionGroup& operator=( const DialogSelectionGroup& rhs );

        void makeRandomSelection();

        //
        // List of SelectableDialog objects to choose from.  Use std::vector
        // for space conservation
        //
        typedef std::vector< SelectableDialog*, s2alloc<SelectableDialog*> > DialogVector;

        DialogVector m_dialogVector;

        short m_currentlyPlayingDialog;
        short m_lastSelection;
};


#endif // DIALOGSELECTIONGROUP_H

