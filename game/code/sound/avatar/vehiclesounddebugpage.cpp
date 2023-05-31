//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        vehiclesounddebugpage.cpp
//
// Description: Definition for VehicleSoundDebugPage class, which displays
//              vehicle sound info through Watcher
//
// History:     11/22/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <sound/avatar/vehiclesounddebugpage.h>



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
// VehicleSoundDebugPage::VehicleSoundDebugPage
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
VehicleSoundDebugPage::VehicleSoundDebugPage( unsigned int pageNum, SoundDebugDisplay* master ) :
    SoundDebugPage( pageNum, master ),
    m_shiftInProgress( false ),
    m_currentGear( 0 ),
    m_currentSpeed( 0.0f ),
    m_shiftTime( 0 ),
    m_downshiftSpeed( 0.0f ),
    m_upshiftSpeed( 0.0f ),
    m_currentPitch( 0.0f ),
    m_isDamaged( false )
{
}

//==============================================================================
// VehicleSoundDebugPage::~VehicleSoundDebugPage
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
VehicleSoundDebugPage::~VehicleSoundDebugPage()
{
}

//******************************************************************************
//
// Protected Member Functions
//
//******************************************************************************

void VehicleSoundDebugPage::fillLineBuffer( int lineNum, char* buffer )
{
    switch( lineNum )
    {
        case 0:
            sprintf( buffer, "Gear: %d  Speed: %f", m_currentGear, m_currentSpeed );
            break;

        case 2:
            sprintf( buffer, "Shift speeds: %f  %f", m_downshiftSpeed, m_upshiftSpeed );
            break;

        case 4:
            if( m_shiftInProgress )
            {
                sprintf( buffer, "SHIFTING - Time in msecs: %d", m_shiftTime );
            }
            else
            {
                buffer[0] = '\0';
            }
            break;

        case 6:
            sprintf( buffer, "Pitch: %f", m_currentPitch );
            break;

        case 8:
            if( m_isDamaged )
            {
                sprintf( buffer, "Life: %f  Threshold: %f  CAR DAMAGE SOUND",
                         m_vehicleLife, m_damageThreshold );
            }
            else
            {
                sprintf( buffer, "Life: %f  Threshold: %f",
                         m_vehicleLife, m_damageThreshold );
            }
            break;

        default:
            buffer[0] = '\0';
            break;
    }
}

//=============================================================================
// VehicleSoundDebugPage::getNumLines
//=============================================================================
// Description: Returns number of lines that we'll display on screen
//
// Parameters:  None
//
// Return:      Line count 
//
//=============================================================================
int VehicleSoundDebugPage::getNumLines()
{
    return( 9 );
}
