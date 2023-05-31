//=============================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// File:        FMVObjective.cpp
//
// Description: Implement FMVObjective
//
// History:     4 Feb 2003 + Created -- James Harrison
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <mission/objectives/FMVObjective.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <mission/gameplaymanager.h>

#include <presentation/presentation.h>

#include <input/inputmanager.h>

//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************

//=============================================================================
// DialogueObjective::DialogueObjective
//=============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
FMVObjective::FMVObjective() :
    mMusicStop( false )
{
    mFileName[ 0 ] = 0;
}

//=============================================================================
// DialogueObjective::~DialogueObjective
//=============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//=============================================================================
FMVObjective::~FMVObjective()
{
}

/*=============================================================================
Description:    Callback when the FMV is finished. We can mark this objective
                as complete.
=============================================================================*/
void FMVObjective::OnPresentationEventEnd( PresentationEvent* pEvent )
{
    SetFinished( true );
    
    //Chuck: After we watch the FMV record it on the charactersheet.
    GetCharacterSheetManager()->SetFMVUnlocked( GetGameplayManager()->GetCurrentLevelIndex() ) ;

    GetInputManager()->SetGameState( Input::ACTIVE_ALL );
}

/*=============================================================================
Description:    Start the FMV playing. We'll get an OnPresentationEventEnd call
                back when it's finished.
=============================================================================*/
void FMVObjective::OnInitialize()
{
    if( mFileName[ 0 ] != 0 )
    {
		bool isSkippable = GetCharacterSheetManager()->QueryFMVUnlocked(GetGameplayManager()->GetCurrentLevelIndex());
        GetPresentationManager()->PlayFMV( mFileName, this, isSkippable, mMusicStop );

        GetInputManager()->SetGameState( Input::ACTIVE_FRONTEND );
    }
}
/*=============================================================================
Description:    Set the file name of the FMV to be played. Note that there isn't
                any directory seperator correction for the different platforms
                (at least not at the time of this writting, ATG was talking
                about putting it in).
=============================================================================*/
void FMVObjective::SetFileName( const char* FileName )
{
    rAssertMsg( strlen( FileName ) < FMV_FILE_NAME_LEN, "File name for FMV objective too long." );
    strcpy( mFileName, FileName );
}