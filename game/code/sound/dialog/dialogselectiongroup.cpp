//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        dialogselectiongroup.cpp
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
#define DONTCHECKVECTORRESIZING
//========================================
// System Includes
//========================================
#include <radtime.hpp>

//========================================
// Project Includes
//========================================
#include <sound/dialog/dialogselectiongroup.h>

#include <sound/simpsonssoundplayer.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//
// Typical max number of elements in the vector, so as to not waste space
//
static const int TYPICAL_VECTOR_SIZE = 4;

//
// Value for in-use dialog index when nothing being played
//
static const short DIALOG_NOT_IN_USE = -1;

static const short DIALOG_NOT_YET_USED = -1;



//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// DialogSelectionGroup::DialogSelectionGroup
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
DialogSelectionGroup::DialogSelectionGroup( SelectableDialog& dialog1, 
                                            SelectableDialog& dialog2 ) :
    m_currentlyPlayingDialog( DIALOG_NOT_IN_USE ),
    m_lastSelection( DIALOG_NOT_YET_USED )
{
    HeapMgr()->PushHeap( GMA_AUDIO_PERSISTENT );

    m_dialogVector.reserve( TYPICAL_VECTOR_SIZE );

    m_dialogVector.push_back( &dialog1 );
    m_dialogVector.push_back( &dialog2 );

    HeapMgr()->PopHeap( GMA_AUDIO_PERSISTENT );
}

//==============================================================================
// DialogSelectionGroup::~DialogSelectionGroup
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
DialogSelectionGroup::~DialogSelectionGroup()
{
}

//==============================================================================
// DialogSelectionGroup::PlayLine
//==============================================================================
// Description: Select a dialog if we haven't done it yet and play it.
//
// Parameters:	lineIndex - index of dialog line to play
//              player - sound player
//              callback - callback for when the dialog line is complete
//
// Return:      N/A.
//
//==============================================================================
void DialogSelectionGroup::PlayLine( unsigned int lineIndex,
                                     SimpsonsSoundPlayer& player,
                                     SimpsonsSoundPlayerCallback* callback )
{
    if( lineIndex == 0 )
    {
        makeRandomSelection();
    }
    else
    {
        rAssert( m_currentlyPlayingDialog != DIALOG_NOT_IN_USE );
    }

    m_dialogVector[m_currentlyPlayingDialog]->PlayLine( lineIndex, player, callback );
}

//=============================================================================
// DialogSelectionGroup::QueueLine
//=============================================================================
// Description: Select a dialog if we haven't done it yet and queue it for
//              playback
//
// Parameters:	lineIndex - index of dialog line to play
//              player - sound player
//
// Return:      void 
//
//=============================================================================
void DialogSelectionGroup::QueueLine( unsigned int lineIndex, SimpsonsSoundPlayer& player )
{
    if( lineIndex == 0 )
    {
        makeRandomSelection();
    }
    else
    {
        rAssert( m_currentlyPlayingDialog != DIALOG_NOT_IN_USE );
    }

    m_dialogVector[m_currentlyPlayingDialog]->QueueLine( lineIndex, player );
}

//=============================================================================
// DialogSelectionGroup::PlayQueuedLine
//=============================================================================
// Description: Play line of dialog that we queued earlier
//
// Parameters:	player - sound player
//              callback - callback for when the dialog line is complete
//
// Return:      void 
//
//=============================================================================
void DialogSelectionGroup::PlayQueuedLine( SimpsonsSoundPlayer& player,
                                           SimpsonsSoundPlayerCallback* callback )
{
    m_dialogVector[m_currentlyPlayingDialog]->PlayQueuedLine( player, callback );
}

//=============================================================================
// DialogSelectionGroup::GetNumDialogLines
//=============================================================================
// Description: Return the number of dialog lines in the currently selected 
//              SelectableDialog object
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
unsigned int DialogSelectionGroup::GetNumDialogLines() const
{
    if( m_currentlyPlayingDialog == DIALOG_NOT_IN_USE )
    {
        //
        // Assume shortest case
        //
        return( 1 );
    }
    else
    {
        return( m_dialogVector[m_currentlyPlayingDialog]->GetNumDialogLines() );
    }
}

//=============================================================================
// DialogSelectionGroup::UsesCharacter
//=============================================================================
// Description: Indicate if any of the dialogs use the given character
//
// Parameters:  characterObj - character to match
//
// Return:      true if character used, false otherwise
//
//=============================================================================
bool DialogSelectionGroup::UsesCharacter( tUID characterUID )
{
    if( m_dialogVector.empty() )
    {
        return( false );
    }
    else
    {
        return( m_dialogVector[0]->UsesCharacter( characterUID ) );
    }
}

//=============================================================================
// DialogSelectionGroup::IsVillainLine
//=============================================================================
// Description: Indicate if the dialogs are villain
//
// Parameters:  None
//
// Return:      true if villain, false otherwise 
//
//=============================================================================
bool DialogSelectionGroup::IsVillainLine()
{
    if( m_dialogVector.empty() )
    {
        return( false );
    }
    else
    {
        return( m_dialogVector[0]->IsVillainLine() );
    }
}

//=============================================================================
// DialogSelectionGroup::GetDialogLineCharacterUID
//=============================================================================
// Description: Indicate which character is speaking with the given line
//
// Parameters:  lineNum - indicates which dialog line we want the UID for
//
// Return:      tUID of character associated with dialog line, zero if we
//              haven't selected a character
//
//=============================================================================
tUID DialogSelectionGroup::GetDialogLineCharacterUID( unsigned int lineNum )
{
    if( m_currentlyPlayingDialog == DIALOG_NOT_IN_USE )
    {
        return( m_dialogVector[0]->GetDialogLineCharacterUID( lineNum ) );
    }
    else
    {
        return( m_dialogVector[m_currentlyPlayingDialog]->GetDialogLineCharacterUID( lineNum ) );
    }
}

//=============================================================================
// DialogSelectionGroup::AddMatchingDialog
//=============================================================================
// Description: Push given dialog onto list
//
// Parameters:  newDialog - dialog to add
//
// Return:      void 
//
//=============================================================================
void DialogSelectionGroup::AddMatchingDialog( SelectableDialog& newDialog, 
                                              SelectableDialogList& list )
{
    HeapMgr()->PushHeap( GMA_AUDIO_PERSISTENT );
    m_dialogVector.push_back( &newDialog );
    HeapMgr()->PopHeap( GMA_AUDIO_PERSISTENT );
}

//=============================================================================
// DialogSelectionGroup::GetEvent
//=============================================================================
// Description: Returns event associated with this group, which we'll get
//              from a dialog in the list.
//
// Parameters:  None
//
// Return:      EventEnum representing event
//
//=============================================================================
EventEnum DialogSelectionGroup::GetEvent() const
{
    rAssert( m_dialogVector.size() > 0 );

    return( m_dialogVector[0]->GetEvent() );
}

//=============================================================================
// DialogSelectionGroup::GetLevel
//=============================================================================
// Description: Returns level associated with this group, which we'll get
//              from a dialog in the list.
//
// Parameters:  None
//
// Return:      index of level if one exists, NO_LEVEL otherwise 
//
//=============================================================================
unsigned int DialogSelectionGroup::GetLevel() const
{
    rAssert( m_dialogVector.size() > 0 );

    return( m_dialogVector[0]->GetLevel() );
}

//=============================================================================
// DialogSelectionGroup::GetMission
//=============================================================================
// Description: Returns event associated with this group, which we'll get
//              from a dialog in the list.
//
// Parameters:  None
//
// Return:      index of mission if one exists, NO_MISSION otherwise 
//
//=============================================================================
unsigned int DialogSelectionGroup::GetMission() const
{
    rAssert( m_dialogVector.size() > 0 );

    return( m_dialogVector[0]->GetMission() );
}

//=============================================================================
// DialogSelectionGroup::GetConversationName
//=============================================================================
// Description: Not relevant in this class.
//
// Parameters:  None
//
// Return:      Zero, indicating we aren't a conversation 
//
//=============================================================================
radKey32 DialogSelectionGroup::GetConversationName()
{
    return( 0 );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// DialogSelectionGroup::makeRandomSelection
//=============================================================================
// Description: Select dialog to play
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void DialogSelectionGroup::makeRandomSelection()
{
    if( m_lastSelection == DIALOG_NOT_YET_USED )
    {
        //
        // No real need to bother with true pseudo-randomization, just
        // use the clock
        //
        m_lastSelection = static_cast<short>( radTimeGetMilliseconds() % m_dialogVector.size() );
    }

    m_currentlyPlayingDialog = m_lastSelection;
    m_lastSelection = ( m_lastSelection + 1 ) % m_dialogVector.size();
}

#undef DONTCHECKVECTORRESIZING