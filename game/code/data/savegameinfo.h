//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   SaveGameInfo
//
// Description: Interface for the SaveGameInfo class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/10/18      TChu        Created for SRR2
//
//===========================================================================

#ifndef SAVEGAMEINFO_H
#define SAVEGAMEINFO_H

//===========================================================================
// Nested Includes
//===========================================================================

#include <data/gamedata.h>

#include <radtime.hpp>
#include <radfile.hpp>

//===========================================================================
// Forward References
//===========================================================================

struct SaveGameInfoData
{
    unsigned short m_magicNumber;   // used for data verification
    radDate m_timeStamp;
    GameDataByte m_level;
    GameDataByte m_mission;
    unsigned int m_fileSize;        // used for file version verification
};

//===========================================================================
// Interface Definitions
//===========================================================================

class SaveGameInfo : public GameDataHandler
{
public:
    enum{  MAGIC_NUMBER = 1978 }; 

	SaveGameInfo();
    virtual ~SaveGameInfo();

    static const unsigned int GetSize() { return sizeof( SaveGameInfoData ); }

    // Implements Game Data Handler
    //
    virtual void LoadData( const GameDataByte* dataBuffer,
                           unsigned int numBytes );
    virtual void SaveData( GameDataByte* dataBuffer,
                           unsigned int numBytes );

    virtual void ResetData();
    
    void FormatLevelMissionInfo(char levelMissionInfo[32]) const;

    // Data accessors
    //
    const SaveGameInfoData* GetData() const { return &m_data; }

    // Verify data
    //
    bool CheckData();

    // Format display string for viewing in frontend screen
    //
    void FormatDisplay( char* displayBuffer,
                        unsigned int bufferLength ) const;

    static int CompareTimeStamps( const radDate& date1,
                                  const radDate& date2 );

    char    m_displayFilename[radFileFilenameMax+1];

private:
    //---------------------------------------------------------------------
    // Private Functions
    //---------------------------------------------------------------------

    // No copying or assignment. Declare but don't define.
    //
    SaveGameInfo( const SaveGameInfo& );
    SaveGameInfo& operator= ( const SaveGameInfo& );

    //---------------------------------------------------------------------
    // Private Data
    //---------------------------------------------------------------------

    SaveGameInfoData m_data;

};

#endif // SAVEGAMEINFO_H
