//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   SaveGameInfo
//
// Description: Implementation of the SaveGameInfo class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/08/06      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <data/savegameinfo.h>
#include <data/gamedatamanager.h>

#include <main/commandlineoptions.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <presentation/gui/guitextbible.h>

#include <p3d/unicode.hpp>

#include <string.h>

//===========================================================================
// Local Constants
//===========================================================================

//===========================================================================
// Public Member Functions
//===========================================================================

//===========================================================================
// SaveGameInfo::SaveGameInfo
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
SaveGameInfo::SaveGameInfo()
{
    this->ResetData();
}

//===========================================================================
// SaveGameInfo::~SaveGameInfo
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
SaveGameInfo::~SaveGameInfo()
{
}

//===========================================================================
// SaveGameInfo::LoadData
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void
SaveGameInfo::LoadData( const GameDataByte* dataBuffer,
                        unsigned int numBytes )
{
    // copy data from buffer
    //
    memcpy( &m_data, dataBuffer, sizeof( m_data ) );

//    rWarningMsg( m_data.m_fileSize == GetGameDataManager()->GetGameDataSize(),
//                 "*** ERROR: File size mis-match that is quite possibly due to loading an old saved game file." );
}

//===========================================================================
// SaveGameInfo::SaveData
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void
SaveGameInfo::SaveData( GameDataByte* dataBuffer,
                        unsigned int numBytes )
{
#ifndef WORLD_BUILDER
    // store magic number
    //
    m_data.m_magicNumber = MAGIC_NUMBER;

    // get current time stamp
    //
    radTimeGetDate( &(m_data.m_timeStamp) );

    if( CommandLineOptions::Get( CLO_MEMCARD_CHEAT ) )
    {
        // TC: if saving w/ memory card cheat, set the date to be my birthday so
        //     it can be more easily distinguished from a regular saved game
        //
        m_data.m_timeStamp.m_Month = 6;
        m_data.m_timeStamp.m_Day = 27;
        m_data.m_timeStamp.m_Year = 1978;
        m_data.m_timeStamp.m_Hour = 23;
        m_data.m_timeStamp.m_Minute = 0;
        m_data.m_timeStamp.m_Second = 0;
    }

    // get current level and mission
    //
    CurrentMissionStruct currentMission = GetCharacterSheetManager()->QueryCurrentMission();
    m_data.m_level = static_cast<GameDataByte>( currentMission.mLevel + 1 );
    m_data.m_mission = static_cast<GameDataByte>( currentMission.mMissionNumber + 1 );

    // special case for level 1 due to tutorial mission being mission #0
    //
    if( currentMission.mLevel == 0 )
    {
        m_data.m_mission--;
    }

    // get file size (in bytes)
    //
    m_data.m_fileSize = GetGameDataManager()->GetGameDataSize();

    // copy data to buffer
    //
    memcpy( dataBuffer, &m_data, sizeof( m_data ) );
#endif
}

void
SaveGameInfo::ResetData()
{
    memset( &m_data, 0, sizeof( m_data ) );
}

bool
SaveGameInfo::CheckData()
{
    // do some rudimentary data checking to verify that file is not corrupted
    // and that all expected data make sense

    // check file size against expected file size (*** SHOULD BE CHECKED FIRST! ***)
    //
    if( m_data.m_fileSize != GetGameDataManager()->GetGameDataSize() )
    {
        rTunePrintf( "ERROR: *** Loaded game data size = %d bytes (Expected data size = %d bytes)!\n",
                     m_data.m_fileSize, GetGameDataManager()->GetGameDataSize() );

        return false;
    }

    // check magic number
    //
    if( m_data.m_magicNumber != MAGIC_NUMBER )
    {
        rTunePrintf( "ERROR: *** Loaded game data contains magic number = %d (Expected magic number = %d)!\n",
                     m_data.m_magicNumber, MAGIC_NUMBER );

        return false;
    }

    // check timestamp for valid date
    //
    if( m_data.m_timeStamp.m_Month < 1 || m_data.m_timeStamp.m_Month > 12 ||
        m_data.m_timeStamp.m_Day < 1 || m_data.m_timeStamp.m_Day > 31 )
    {
        rTunePrintf( "ERROR: *** Loaded game data contains invalid timestamp information!\n" );

        return false;
    }

    // everything's cool, return 'OK' status
    //
    return true;
}
void CopyUnicodeToCharString(char *str, P3D_UNICODE* uni_str, int max_char)
{
    int i = 0;
    while (*uni_str && i < max_char)
    {
        *str++ = (char)*uni_str++;
		i++;
    }
    *str++ = 0;
}
void SaveGameInfo::FormatLevelMissionInfo(char levelMissionInfo[32]) const
{
#define MAX_LEVEL_MISSION_STRING 10
    char tut_string[MAX_LEVEL_MISSION_STRING+1];
    char l_string[MAX_LEVEL_MISSION_STRING+1];
    char m_string[MAX_LEVEL_MISSION_STRING+1];

    P3D_UNICODE* uni_string = GetTextBibleString( "TUTORIAL_ABBREVIATION" );
    rTuneAssert(uni_string);
    CopyUnicodeToCharString(tut_string, uni_string, MAX_LEVEL_MISSION_STRING);

    uni_string = GetTextBibleString( "LEVEL_ABBREVIATION" );
    rTuneAssert(uni_string);
    CopyUnicodeToCharString(l_string, uni_string, MAX_LEVEL_MISSION_STRING);

    uni_string = GetTextBibleString( "MISSION_ABBREVIATION" );
    rTuneAssert(uni_string);
    CopyUnicodeToCharString(m_string, uni_string, MAX_LEVEL_MISSION_STRING);

    if( m_data.m_level == 1 && m_data.m_mission == 0 )
    {
        // level 1 tutorial mission
        //
        sprintf( levelMissionInfo, "(%s) ", tut_string );
    }
    else
    {
        sprintf( levelMissionInfo, "(%s%d %s%d) ",
            l_string,
            m_data.m_level,
            m_string,
            m_data.m_mission );
    }
}

void
SaveGameInfo::FormatDisplay( char* displayBuffer,
                             unsigned int bufferLength ) const
{
    char dateBuffer[ 64 ];
    rAssert( displayBuffer != NULL );
    rAssert( bufferLength >= 64 );

    // format time and date display
    //
	sprintf( dateBuffer, "%02d/%02d/%02d %02d:%02d:%02d",
#if defined( PAL ) && defined( RAD_PS2 )    // English date format
             m_data.m_timeStamp.m_Day,
             m_data.m_timeStamp.m_Month,
#else                                       // American date format
             m_data.m_timeStamp.m_Month,
             m_data.m_timeStamp.m_Day,
#endif
             m_data.m_timeStamp.m_Year % 100,
             m_data.m_timeStamp.m_Hour,
			 m_data.m_timeStamp.m_Minute,
			 m_data.m_timeStamp.m_Second
			 );

    // format level and mission display
    //
    char levelMissionInfo[ 32 ];
    FormatLevelMissionInfo( levelMissionInfo );

#ifdef RAD_GAMECUBE
    // add number of blocks to display
    //
    P3D_UNICODE* unicodeString = GetTextBibleString( "BLOCKS" );
    rAssert( unicodeString != NULL );

    char numBlocksBuffer[ 32 ];
    p3d::UnicodeToAscii( unicodeString, numBlocksBuffer, sizeof( numBlocksBuffer ) );

    sprintf( displayBuffer, "%s %s (2 %s)", levelMissionInfo, dateBuffer, numBlocksBuffer );
#else
    sprintf( displayBuffer, "%s %s", levelMissionInfo, dateBuffer );
#endif

    // sanity check
    //
    rAssertMsg( strlen( displayBuffer ) < bufferLength, "*** Buffer Overrun!" );
}

int
SaveGameInfo::CompareTimeStamps( const radDate& date1, const radDate& date2 )
{
    // compares two timestamps, and returns:
    //
    // - a positive integer if date1 is later than date2
    // - zero if date1 is the same as date2
    // - a negative integer if date1 is earlier than date2
    //

    int diff = 0;

    if( date1.m_Year != date2.m_Year )          // compare years
    {
        diff = date1.m_Year - date2.m_Year;
    }
    else if( date1.m_Month != date2.m_Month )   // compare months
    {
        diff = date1.m_Month - date2.m_Month;
    }
    else if( date1.m_Day != date2.m_Day )       // compare days
    {
        diff = date1.m_Day - date2.m_Day;
    }
    else if( date1.m_Hour != date2.m_Hour )     // compare hours
    {
        diff = date1.m_Hour - date2.m_Hour;
    }
    else if( date1.m_Minute != date2.m_Minute ) // compare minutes
    {
        diff = date1.m_Minute - date2.m_Minute;
    }
    else if( date1.m_Second != date2.m_Second ) // compare seconds
    {
        diff = date1.m_Second - date2.m_Second;
    }

    return diff;
}

//===========================================================================
// Private Member Functions
//===========================================================================

