//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   GameDataManager
//
// Description: Interface for the GameDataManager class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/09/16      TChu        Created for SRR2
//
//===========================================================================

#ifndef GAMEDATAMANAGER_H
#define GAMEDATAMANAGER_H

#ifndef NULL
    #define NULL 0
#endif

//===========================================================================
// Nested Includes
//===========================================================================

#include <data/gamedata.h>
#include <radfile.hpp>

//===========================================================================
// Forward References
//===========================================================================

const unsigned int MAX_NUM_GAME_DATA = 16; // maximum number of
                                           // client-registered game data

const unsigned int KB = 1024;
const unsigned int MB = 1024 * KB;

struct GameDataLoadCallback
{
    virtual void OnLoadGameComplete( radFileError errorCode ) = 0;
};

struct GameDataSaveCallback
{
    virtual void OnSaveGameComplete( radFileError errorCode ) = 0;
};

struct GameDataDeleteCallback
{
    virtual void OnDeleteGameComplete( radFileError errorCode ) = 0;
};

struct IRadFile;
struct radMemcardInfo;
struct radDate;

class SaveGameInfo;

//===========================================================================
// Interface Definitions
//===========================================================================

class GameDataManager : public IRadFileCompletionCallback,
                        public IRadDriveCompletionCallback,
                        public IRadDriveErrorCallback
{
public:
    // Static Methods for accessing this singleton.
    static GameDataManager* CreateInstance();
    static void DestroyInstance();
    static GameDataManager* GetInstance();

	GameDataManager();
    virtual ~GameDataManager();

    // Initialization
    //
    void Init();

    // Update
    //
    void Update( unsigned int elapsedTime );

    // Registration of client game data
    //
    void RegisterGameData( GameDataHandler* gdHandler,
                           unsigned int numBytes,
                           const char* name = NULL );

    unsigned int GetGameDataSize() const { return m_gameDataSize; }

    // Load/Save Game (Asynchronous)
    void LoadGame( unsigned int slot, GameDataLoadCallback* callback = NULL, const char *filename = NULL );
    void SaveGame( unsigned int slot, GameDataSaveCallback* callback = NULL );

	// Delete Game
	radFileError DeleteGame( const char *fileName, bool async = false, GameDataDeleteCallback* callback = NULL );

    // Reset All Game Data (for New Games)
    //
    void ResetGame();

    void SetMinimumLoadSaveTime( unsigned int minimumTime );
    void RestoreDefaultMinimumLoadSaveTime();

    bool IsGameLoaded() const { return m_isGameLoaded; }
    void SetGameLoaded() { m_isGameLoaded = true; }

    bool GetSaveGameInfo( IRadDrive* pDrive,
                          unsigned int slot,
                          SaveGameInfo* saveGameInfo,
                          bool  *file_corrupt_flag = NULL);

    bool DoesSaveGameExist( IRadDrive* pDrive,
                            bool check_valid = true,
                            bool forAllSlots = false );

    bool FindMostRecentSaveGame( IRadDrive* pDrive,
                                 unsigned int& slot,
                                 radDate& timeStamp );

    // saved game filename formatting
    //
    static void FormatSavedGameFilename( char* filename,
                                         unsigned int filenameLength,
                                         unsigned int slot );

    // Implements IRadFileCompletionCallback
    //
    virtual void AddRef() {;}
    virtual void Release() {;}
    virtual void OnFileOperationsComplete( void* pUserData );

    virtual void OnDriveOperationsComplete( void* pUserData );

    // Implements IRadDriveErrorCallback
    //
    virtual bool OnDriveError( radFileError error, const char* pDriveName, void* pUserData );

    bool IsUsingDrive() const;

protected:

    enum eFileOperation
    {
        FILE_OP_NONE,

        FILE_OP_OPEN_FOR_READING,
        FILE_OP_OPEN_FOR_WRITING,
        FILE_OP_READ,
        FILE_OP_WRITE,
        FILE_OP_COMMIT,
        FILE_OP_FILE_CHECK,

        FILE_OP_LOAD_COMPLETED,
        FILE_OP_SAVE_COMPLETED,

		FILE_OP_DELETE,
        FILE_OP_DELETE_COMPLETED,

        NUM_FILE_OPERATIONS
    };

private:
    //---------------------------------------------------------------------
    // Private Functions
    //---------------------------------------------------------------------

    // No copying or assignment. Declare but don't define.
    //
    GameDataManager( const GameDataManager& );
    GameDataManager& operator= ( const GameDataManager& );

    bool LoadAllData();
    bool SaveAllData();

    static void PrintRawData( GameDataByte* dataBuffer,
                              unsigned int numBytes );

    //---------------------------------------------------------------------
    // Private Data
    //---------------------------------------------------------------------

    // Pointer to the one and only instance of this singleton.
    static GameDataManager* spInstance;

    GameData* m_registeredGameData[ MAX_NUM_GAME_DATA ];
    unsigned int m_numRegisteredGameData;

    GameDataByte* m_gameDataBuffer;
    unsigned int m_gameDataSize;

    GameDataLoadCallback* m_gameDataLoadCallback;
    GameDataSaveCallback* m_gameDataSaveCallback;
    GameDataDeleteCallback* m_gameDataDeleteCallback;

    unsigned int m_minimumLoadSaveTime;
    unsigned int m_elapsedOperationTime;

    bool m_isGameLoaded;

    SaveGameInfo* m_saveGameInfoHandler;

    // RadFile Stuff
    //
    IRadFile* m_radFile;
    eFileOperation m_currentFileOperation;

	radFileError m_lastError;

};

// A little syntactic sugar for getting at this singleton.
inline GameDataManager* GetGameDataManager() { return( GameDataManager::GetInstance() ); }

#endif // GAMEDATAMANAGER_H
