//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        tunerdebugpage.cpp
//
// Description: Displays debug info for the sound tuner
//
// History:     6/11/2003 + Created -- NAME
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <sound/soundrenderer/tunerdebugpage.h>

const char* s_duckNames[Sound::NUM_DUCK_SITUATIONS] =
{
    "Full fade",
    "Pause",
    "Mission",
    "Letterbox",
    "Dialogue",
    "Store",
    "On foot",
    "Minigame",
    "Just Music",
    "Credits"
};

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
// TunerDebugPage::TunerDebugPage
//=============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
TunerDebugPage::TunerDebugPage()
{
    unsigned int i, j;

    //
    // Zero out the tuner values
    //
    for( i = 0; i < Sound::NUM_DUCK_SITUATIONS; i++ )
    {
        for( j = 0; j < Sound::NUM_DUCK_VOLUMES; j++ )
        {
            m_duckLevels[i].duckVolume[j] = 0.0f;
        }
    }

    for( i = 0; i < Sound::NUM_DUCK_VOLUMES; i++ )
    {
        m_userVolumes.duckVolume[i] = 0.0f;
    }
}

//=============================================================================
// TunerDebugPage::~TunerDebugPage
//=============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//=============================================================================
TunerDebugPage::~TunerDebugPage()
{
}

//=============================================================================
// TunerDebugPage::SetDuckLevel
//=============================================================================
// Description: Comment
//
// Parameters:  ( DuckSituations situation, DuckVolumes volumeType, float volume )
//
// Return:      void 
//
//=============================================================================
void TunerDebugPage::SetDuckLevel( Sound::DuckSituations situation,
                                   Sound::DuckVolumes volumeType,
                                   float volume )
{
    m_duckLevels[situation].duckVolume[volumeType] = volume;
}

//=============================================================================
// TunerDebugPage::SetFinalDuckLevel
//=============================================================================
// Description: Comment
//
// Parameters:  ( DuckVolumes volumeType, float volume )
//
// Return:      void 
//
//=============================================================================
void TunerDebugPage::SetFinalDuckLevel( Sound::DuckVolumes volumeType, float volume )
{
    m_finalDuckLevel.duckVolume[volumeType] = volume;
}

//=============================================================================
// TunerDebugPage::SetUserVolume
//=============================================================================
// Description: Comment
//
// Parameters:  ( DuckVolumes volumeType, float volume )
//
// Return:      void 
//
//=============================================================================
void TunerDebugPage::SetUserVolume( Sound::DuckVolumes volumeType, float volume )
{
    m_userVolumes.duckVolume[volumeType] = volume;
}

//*****************************************************************************
//
// Protected Member Functions
//
//*****************************************************************************

//=============================================================================
// TunerDebugPage::fillLineBuffer
//=============================================================================
// Description: Fill the given buffer with text to display on the screen
//              at the given line
//
// Parameters:  lineNum - line number on screen where buffer will be displayed
//              buffer - to be filled in with text to display
//
// Return:      void 
//
//=============================================================================
void TunerDebugPage::fillLineBuffer( int lineNum, char* buffer )
{
    switch( lineNum )
    {
    case 0:
        strcpy( buffer, "Ducking volumes:" );
        break;

    case 1:
        strcpy( buffer, "          SFX   Car   Music   Dialog   Ambience" );
        break;

    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
        sprintf( buffer, "%s: %0.2f  %0.2f  %0.2f  %0.2f  %0.2f",
                 s_duckNames[lineNum-2],
                 m_duckLevels[lineNum-2].duckVolume[0],
                 m_duckLevels[lineNum-2].duckVolume[1],
                 m_duckLevels[lineNum-2].duckVolume[2],
                 m_duckLevels[lineNum-2].duckVolume[3],
                 m_duckLevels[lineNum-2].duckVolume[4] );
        break;

    case 13:
        sprintf( buffer, "Final duck volumes: %0.2f  %0.2f  %0.2f  %0.2f  %0.2f",
                 m_finalDuckLevel.duckVolume[0],
                 m_finalDuckLevel.duckVolume[1],
                 m_finalDuckLevel.duckVolume[2],
                 m_finalDuckLevel.duckVolume[3],
                 m_finalDuckLevel.duckVolume[4] );
        break;

    case 15:
        sprintf( buffer, "User volumes: %0.2f  %0.2f  %0.2f  %0.2f %0.2f",
            m_userVolumes.duckVolume[0],
            m_userVolumes.duckVolume[1],
            m_userVolumes.duckVolume[2],
            m_userVolumes.duckVolume[3],
            m_userVolumes.duckVolume[4] );
        break;

    default:
        buffer[0] = '\0';
        break;
    }
}

//=============================================================================
// DialogSoundDebugPage::getNumLines
//=============================================================================
// Description: Returns number of lines that we'll display on screen
//
// Parameters:  None
//
// Return:      Line count 
//
//=============================================================================
int TunerDebugPage::getNumLines()
{
    return( 16 );
}

//*****************************************************************************
//
// Private Member Functions
//
//*****************************************************************************
