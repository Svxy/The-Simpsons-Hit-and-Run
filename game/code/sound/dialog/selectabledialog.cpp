//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        selectabledialog.cpp
//
// Description: Abstract base class for groups of one or more lines of dialog,
//              any of which can be selected randomly in response to a single
//              sound event.
//
// History:     02/09/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <raddebug.hpp>

//========================================
// Project Includes
//========================================
#include <sound/dialog/selectabledialog.h>



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
// SelectableDialog::SelectableDialog
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SelectableDialog::SelectableDialog() :
    m_missionNum( NO_MISSION ),
    m_levelNum( NO_LEVEL ),
    m_event( NUM_EVENTS ),
#ifndef RAD_RELEASE
    m_hasBeenPlayed( false ),
#endif
    m_nextListObject( NULL )
{
}

//==============================================================================
// SelectableDialog::SelectableDialog
//==============================================================================
// Description: Constructor.
//
// Parameters:	mission - mission number for dialog
//              level - level number
//              event - event that this dialog is played in response to
//
// Return:      N/A.
//
//==============================================================================
SelectableDialog::SelectableDialog( unsigned int level, unsigned int mission, EventEnum event ) :
    m_missionNum( mission ),
    m_levelNum( level ),
    m_event( event ),
#ifndef RAD_RELEASE
    m_hasBeenPlayed( false ),
#endif
    m_nextListObject( NULL )
{
}

//==============================================================================
// SelectableDialog::~SelectableDialog
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SelectableDialog::~SelectableDialog()
{
}

//=============================================================================
// SelectableDialog::AddToDialogList
//=============================================================================
// Description: Place this object on the head of the list supplied
//
// Parameters:  list - list to add to
//
// Return:      void 
//
//=============================================================================
void SelectableDialog::AddToDialogList( SelectableDialog** list )
{
    m_nextListObject = *list;
    *list = this;
}

//=============================================================================
// SelectableDialog::AddToDialogList
//=============================================================================
// Description: Place this object in the list after the object supplied
//
// Parameters:  listObj - object to add after
//
// Return:      void 
//
//=============================================================================
void SelectableDialog::AddToDialogList( SelectableDialog* listObj )
{
    m_nextListObject = listObj->m_nextListObject;
    listObj->m_nextListObject = this;
}

//=============================================================================
// SelectableDialog::RemoveNextFromList
//=============================================================================
// Description: Pull out the object after this one in the list
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SelectableDialog::RemoveNextFromList()
{
    rAssert( m_nextListObject != NULL );
    m_nextListObject = m_nextListObject->m_nextListObject;
}

//=============================================================================
// SelectableDialog::GetMission
//=============================================================================
// Description: Returns mission number associated with this dialog
//
// Parameters:  None
//
// Return:      unsigned int with the mission number, NO_MISSION for no mission
//
//=============================================================================
unsigned int SelectableDialog::GetMission() const
{
    return( m_missionNum );
}

//=============================================================================
// SelectableDialog::GetLevel
//=============================================================================
// Description: Returns level number associated with this dialog
//
// Parameters:  None
//
// Return:      unsigned int with the level number, NO_LEVEL for no level
//
//=============================================================================
unsigned int SelectableDialog::GetLevel() const
{
    return( m_levelNum );
}

//=============================================================================
// SelectableDialog::GetEvent
//=============================================================================
// Description: Returns event associated with this dialog
//
// Parameters:  None
//
// Return:      EventEnum
//
//=============================================================================
EventEnum SelectableDialog::GetEvent() const
{
    return( m_event );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

#ifndef RAD_RELEASE

void SelectableDialog::MarkAsPlayed()
{
    m_hasBeenPlayed = true;
}

void SelectableDialog::PrintPlayedStatus()
{
    if( m_hasBeenPlayed )
    {
        rTuneString( "Played\n" );
    }
    else
    {
        rTuneString( "NOT PLAYED\n" );
    }
}

#endif
