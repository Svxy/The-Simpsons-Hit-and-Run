//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   GameDataManager
//
// Description: Implementation of the GameDataManager class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/09/16      TChu        Created for SRR2
//
//===========================================================================

//#define PRINT_RAW_DATA // to debug output
 
//===========================================================================
// Includes
//===========================================================================
#include <data/gamedatamanager.h>
#include <data/savegameinfo.h>
#include <data/memcard/memorycardmanager.h>
#include <string.h>

#ifndef WORLD_BUILDER
#include <memory/srrmemory.h>
#else
#define MEMTRACK_PUSH_GROUP(x)
#define MEMTRACK_POP_GROUP(x)
#define GMA_PERSISTENT 0
#define GMA_DEFAULT 0
#define GMA_TEMP 0
#endif

#include <radfile.hpp>
#include <raddebugwatch.hpp>
#include <radtime.hpp>

#ifdef RAD_PS2
    #include <radstring.hpp>
#endif
#ifdef RAD_PS2
const unsigned int MAX_GAME_DATA_SIZE = 8600; // in bytes (since ps2 have different byte packing)
#else
const unsigned int MAX_GAME_DATA_SIZE = 7500; // in bytes
#endif

// Static pointer to instance of singleton.
GameDataManager* GameDataManager::spInstance = NULL;

//===========================================================================
// Local Constants
//===========================================================================

#ifdef RAD_PS2
    const char* SAVED_GAME_TITLE = "The Simpsons:Hit & Run"; // there should be no space betw. "Simpsons:" and "Hit"
                                                             // due to line break inserted there
#endif

#ifdef RAD_XBOX
    // Xbox TCR Requirement!!!
    //
    #define ENABLE_MINIMUM_LOAD_SAVE_TIME
    const unsigned int MINIMUM_LOAD_SAVE_TIME = 2000; // in msec
#else
    #define ENABLE_MINIMUM_LOAD_SAVE_TIME
    const unsigned int MINIMUM_LOAD_SAVE_TIME = 1000; // in msec
#endif

#ifdef RAD_PS2
    const unsigned int MINIMUM_DELETE_TIME = 3000; // in msec
#else
    const unsigned int MINIMUM_DELETE_TIME = 1000; // in msec
#endif

#ifdef RAD_PS2
    /* Filename for PS2 saved games must be prefixed with the following:
     *
     * (BISLPS | BISLPM | BISCPS | BASLUS | BASCUS | BESLES | BESCES)-#####
     *
     * Example: "BASCUS-12345savegame.dat"
     *
     */
  #ifdef PAL
    #define PS2_FILENAME_PREFIX "BESLES-51897"
  #else
    #define PS2_FILENAME_PREFIX "BASLUS-20624"
  #endif
#else
    #define PS2_FILENAME_PREFIX ""
#endif

#ifdef DEBUGWATCH
    const char* WATCHER_NAMESPACE = "Game Data Manager";

    unsigned int g_wCurrentSlot = 0;

    static void LoadGameFromWatcher()
    {
        if( GetMemoryCardManager()->GetCurrentDrive() != NULL )
        {
            GetGameDataManager()->LoadGame( g_wCurrentSlot );
        }
        else
        {
            rTunePrintf( "*** Can't load game! No memory device selected!\n" );
        }
    }

    static void SaveGameFromWatcher()
    {
        if( GetMemoryCardManager()->GetCurrentDrive() != NULL )
        {
            GetGameDataManager()->SaveGame( g_wCurrentSlot );
        }
        else
        {
            rTunePrintf( "*** Can't save game! No memory device selected!\n" );
        }
    }
#endif

//===========================================================================
// Public Member Functions
//===========================================================================

//==============================================================================
// GameDataManager::CreateInstance
//==============================================================================
//
// Description: - Creates the Game Data Manager.
//
// Parameters:	None.
//
// Return:      Pointer to the GameDataManager.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
GameDataManager* GameDataManager::CreateInstance()
{
MEMTRACK_PUSH_GROUP( "GameDataManager" );
    spInstance = new GameDataManager;
    rAssert( spInstance != NULL );
MEMTRACK_POP_GROUP( "GameDataManager" );

    // create other singletons owned by GameDataManager
    //
    MemoryCardManager* pMemoryCardManager = MemoryCardManager::CreateInstance();
    rAssert( pMemoryCardManager != NULL );

    return spInstance;
}

//==============================================================================
// GameDataManager::DestroyInstance
//==============================================================================
//
// Description: Destroy the Game Data Manager.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void GameDataManager::DestroyInstance()
{
    // destroy other singletons owned by GameDataManager
    //
    MemoryCardManager::DestroyInstance();

    rAssert( spInstance != NULL );

    delete spInstance;
    spInstance = NULL;
}

//==============================================================================
// GameDataManager::GetInstance
//==============================================================================
//
// Description: - Access point for the GameDataManager singleton.  
//              - Creates the GameDataManager if needed.
//
// Parameters:	None.
//
// Return:      Pointer to the GameDataManager.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
GameDataManager* GameDataManager::GetInstance()
{
    rAssert( spInstance != NULL );

    return spInstance;
}

//===========================================================================
// GameDataManager::GameDataManager
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
GameDataManager::GameDataManager()
:   m_numRegisteredGameData( 0 ),
    m_gameDataBuffer( NULL ),
    m_gameDataSize( 0 ),
    m_gameDataLoadCallback( NULL ),
    m_gameDataSaveCallback( NULL ),
    m_gameDataDeleteCallback( NULL ),
#ifdef ENABLE_MINIMUM_LOAD_SAVE_TIME
    m_minimumLoadSaveTime( MINIMUM_LOAD_SAVE_TIME ),
#else
    m_minimumLoadSaveTime( 0 ),
#endif
    m_elapsedOperationTime( 0 ),
    m_isGameLoaded( false ),
    m_saveGameInfoHandler( NULL ),
    m_radFile( NULL ),
    m_currentFileOperation( FILE_OP_NONE )
{
    rAssertMsg( sizeof( GameDataByte ) == 1,
                "WARNING: *** Size of GameDataByte is not 1 byte! Is that OK??" );

    for( unsigned int i = 0; i < sizeof( m_registeredGameData ) / 
                                 sizeof( m_registeredGameData[ 0 ] ); i++ )
    {
        m_registeredGameData[ i ] = NULL;
    }

    // create the save game info handler, and register it before anything else
    //
    m_saveGameInfoHandler = new SaveGameInfo;
    rAssert( m_saveGameInfoHandler );
    this->RegisterGameData( m_saveGameInfoHandler,
                            SaveGameInfo::GetSize(),
                            "Save Game Info" );
}

//===========================================================================
// GameDataManager::~GameDataManager
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
GameDataManager::~GameDataManager()
{
#ifdef DEBUGWATCH
    ::radDbgWatchDelete( &g_wCurrentSlot );
#endif

    if( m_saveGameInfoHandler != NULL )
    {
        delete m_saveGameInfoHandler;
        m_saveGameInfoHandler = NULL;
    }

    for( unsigned int i = 0; i < m_numRegisteredGameData; i++ )
    {
        if( m_registeredGameData[ i ] != NULL )
        {
            delete m_registeredGameData[ i ];
            m_registeredGameData[ i ] = NULL;
        }
    }
}


void
GameDataManager::Init()
{
MEMTRACK_PUSH_GROUP( "GameDataManager" );
    // initialize memory card manager
    //
    GetMemoryCardManager()->Init( this );

#ifdef DEBUGWATCH
    ::radDbgWatchAddUnsignedInt( &g_wCurrentSlot,
                                 "Game Slot",
                                 WATCHER_NAMESPACE,
                                 NULL, NULL, 0, NUM_GAME_SLOTS - 1 );

    ::radDbgWatchAddFunction( "Load Game",
                              (RADDEBUGWATCH_CALLBACK)LoadGameFromWatcher,
                              NULL,
                              WATCHER_NAMESPACE );

    ::radDbgWatchAddFunction( "Save Game",
                              (RADDEBUGWATCH_CALLBACK)SaveGameFromWatcher,
                              NULL,
                              WATCHER_NAMESPACE );
#endif
MEMTRACK_POP_GROUP( "GameDataManager" );
}

void
GameDataManager::Update( unsigned int elapsedTime )
{
    if( m_currentFileOperation != FILE_OP_NONE )
    {
#ifdef ENABLE_MINIMUM_LOAD_SAVE_TIME
        if( m_elapsedOperationTime > m_minimumLoadSaveTime )
        {
            if( m_currentFileOperation == FILE_OP_LOAD_COMPLETED )
            {
                if( m_gameDataLoadCallback != NULL )
                {
                    m_gameDataLoadCallback->OnLoadGameComplete( Success );
                }

                m_currentFileOperation = FILE_OP_NONE;
            }

            if( m_currentFileOperation == FILE_OP_SAVE_COMPLETED )
            {
                if( m_gameDataSaveCallback != NULL )
                {
                    m_gameDataSaveCallback->OnSaveGameComplete( Success );
                }

                m_currentFileOperation = FILE_OP_NONE;
            }
        }
#endif // ENABLE_MINIMUM_LOAD_SAVE_TIME

        if( m_elapsedOperationTime > MINIMUM_DELETE_TIME )
        {
            if( m_currentFileOperation == FILE_OP_DELETE_COMPLETED )
            {
                if( m_gameDataDeleteCallback != NULL )
                {
                    m_gameDataDeleteCallback->OnDeleteGameComplete( m_lastError );
                    m_gameDataDeleteCallback = NULL;
                }

                m_currentFileOperation = FILE_OP_NONE;
            }
        }

        m_elapsedOperationTime += elapsedTime;
    }
}

void
GameDataManager::RegisterGameData( GameDataHandler* gdHandler,
                                   unsigned int numBytes,
                                   const char* name )
{
MEMTRACK_PUSH_GROUP( "GameDataManager" );
    rAssert( gdHandler != NULL );
    rAssert( numBytes >= 0 );

    rAssertMsg( m_numRegisteredGameData < MAX_NUM_GAME_DATA,
                "ERROR: *** Exceeded maximum number of registered game data!" );

#ifndef WORLD_BUILDER
    HeapMgr()->PushHeap( GMA_PERSISTENT );
#endif

    // create new game data
    //
    GameData* newGameData = new GameData;
    rAssert( newGameData );
    newGameData->m_gdHandler = gdHandler;
    newGameData->m_numBytes = numBytes;

#ifdef RAD_DEBUG
    if( name != NULL )
    {
        strncpy( newGameData->m_name, name, sizeof( newGameData->m_name ) );
    }
    else
    {
        strcpy( newGameData->m_name, "" );
    }

    rTunePrintf( ">> Registered Game Data [%d]: %s (%d bytes)\n",
                 m_numRegisteredGameData,
                 newGameData->m_name,
                 newGameData->m_numBytes );
#endif

    // add to registered list of game data
    //
    m_registeredGameData[ m_numRegisteredGameData ] = newGameData;
    m_numRegisteredGameData++;

    // update game data size
    //
    m_gameDataSize += numBytes;

    rReleasePrintf( ">> Total Game Data Size = %d bytes\n", m_gameDataSize );

//    rReleaseAssertMsg( m_gameDataSize <= MAX_GAME_DATA_SIZE,
//                       "ERROR: *** Max Game Data Size Exceeded!" );

#ifndef WORLD_BUILDER
    HeapMgr()->PopHeap( GMA_PERSISTENT );
#endif

MEMTRACK_POP_GROUP( "GameDataManager" );
}

void
GameDataManager::LoadGame( unsigned int slot, GameDataLoadCallback* callback, const char *load_filename )
{
    rAssertMsg( slot < NUM_GAME_SLOTS, "ERROR: *** Invalid game slot!" );

    rReleasePrintf( ">> Loading game from slot %d ... ...\n", slot + 1 );

    m_gameDataLoadCallback = callback;

    // open save game file for reading
    //
    char filename[ radFileFilenameMax + 1 ];
	if (load_filename==NULL) // set up predefined filename
		this->FormatSavedGameFilename( filename,
									sizeof( filename ),
									slot );
	else
		strcpy( filename, load_filename ); // filename is passed in for xbox
    IRadDrive* currentDrive = GetMemoryCardManager()->GetCurrentDrive();
    rAssert( currentDrive );
#ifndef RAD_WIN32
    currentDrive->SaveGameOpenAsync( &m_radFile,
                                     filename,
                                     false,
                                     OpenExisting,
                                     NULL,
                                     m_gameDataSize,
									 true);
#else
    currentDrive->FileOpenAsync( &m_radFile,
                                 filename,
                                 false,
                                 OpenExisting );
#endif // ~RAD_WIN32
    rAssert( m_radFile );
    m_radFile->AddCompletionCallback( this, NULL );

    // allocate temp memory for game data buffer
    //
    rAssert( m_gameDataBuffer == NULL );

#ifndef WORLD_BUILDER
    HeapMgr()->PushHeap( GMA_TEMP );
#endif

    m_gameDataBuffer = new GameDataByte[ m_gameDataSize ];

#ifndef WORLD_BUILDER
    HeapMgr()->PopHeap( GMA_TEMP );
#endif

    rAssert( m_gameDataBuffer != NULL );

    m_currentFileOperation = FILE_OP_OPEN_FOR_READING;
    m_elapsedOperationTime = 0;
}

radFileError GameDataManager::DeleteGame( const char *fileName,
                                          bool async,
                                          GameDataDeleteCallback* callback )
{
	rAssert(m_currentFileOperation == FILE_OP_NONE);
	m_currentFileOperation = FILE_OP_DELETE;
	
	IRadDrive* currentDrive = GetMemoryCardManager()->GetCurrentDrive();

	rAssert( currentDrive );

    m_lastError = Success;

    if( async )
    {
    	currentDrive->DestroyFileAsync( fileName, true );
        currentDrive->AddCompletionCallback( this, NULL );

        rAssert( callback != NULL );
        m_gameDataDeleteCallback = callback;

        m_elapsedOperationTime = 0;
    }
    else
    {
    	currentDrive->DestroyFileSync( fileName, true );
	    m_currentFileOperation = FILE_OP_NONE;
    }

	return m_lastError;
}

void
GameDataManager::SaveGame( unsigned int slot, GameDataSaveCallback* callback )
{
    rAssertMsg( slot < NUM_GAME_SLOTS, "ERROR: *** Invalid game slot!" );

    rReleasePrintf( ">> Saving game to slot %d ... ...\n", slot + 1 );

    m_gameDataSaveCallback = callback;

    // allocate temp memory for game data buffer
    //
    rAssert( m_gameDataBuffer == NULL );
#ifndef WORLD_BUILDER
    HeapMgr()->PushHeap( GMA_TEMP );
#endif

    m_gameDataBuffer = new GameDataByte[ m_gameDataSize ];
#ifndef WORLD_BUILDER
    HeapMgr()->PopHeap( GMA_TEMP );
#endif
    rAssert( m_gameDataBuffer != NULL );

    this->SaveAllData();

    // update saved game title in memcard info before saving
    //
#ifdef RAD_GAMECUBE
    char levelMissionInfo[ 32 ];
    m_saveGameInfoHandler->FormatLevelMissionInfo( levelMissionInfo );

    char savedGameTitle[ 32 ];
    sprintf( savedGameTitle, "Slot %d %s", slot + 1, levelMissionInfo );

    GetMemoryCardManager()->UpdateMemcardInfo( savedGameTitle );
#endif
#ifdef RAD_PS2
    char levelMissionInfo[ 32 ];
    m_saveGameInfoHandler->FormatLevelMissionInfo( levelMissionInfo );

    char savedGameTitle[ 32 ];
    sprintf( savedGameTitle, "%s (%d)", SAVED_GAME_TITLE, slot + 1 );

    GetMemoryCardManager()->UpdateMemcardInfo( savedGameTitle, 13 );
#endif
#ifdef RAD_XBOX
    GetMemoryCardManager()->UpdateMemcardInfo( NULL );
#endif
#ifdef RAD_WIN32
    GetMemoryCardManager()->UpdateMemcardInfo( NULL );
#endif

    // open save game file for writing
    //
    char filename[ radFileFilenameMax + 1 ];
    this->FormatSavedGameFilename( filename,
                                   sizeof( filename ),
                                   slot );

	IRadDrive* currentDrive = GetMemoryCardManager()->GetCurrentDrive();
	rAssert( currentDrive );

#ifdef RAD_XBOX
	m_saveGameInfoHandler->FormatDisplay(filename, sizeof(filename)); // define filename for xbox
#endif

#ifndef RAD_WIN32
    const radMemcardInfo* memcardInfo = GetMemoryCardManager()->GetMemcardInfo();
	bool simpleName = false;
#ifdef RAD_XBOX
	simpleName = true; // set the flag so radcore doesn't process ':' and '/'
#endif

#ifdef RAD_PS2
    radFileOpenFlags fileOpenFlags = OpenAlways;
#else
    radFileOpenFlags fileOpenFlags = CreateAlways;
#endif
    currentDrive->SaveGameOpenAsync( &m_radFile,
                                     filename,
                                     true,
                                     fileOpenFlags,
                                     const_cast<radMemcardInfo*>( memcardInfo ),
                                     m_gameDataSize, simpleName );
#else
    currentDrive->FileOpenAsync( &m_radFile,
                                 filename,
                                 true,
                                 CreateAlways );
#endif // ~RAD_WIN32


    rAssert( m_radFile );
    m_radFile->AddCompletionCallback( this, NULL );

    m_currentFileOperation = FILE_OP_OPEN_FOR_WRITING;
    m_elapsedOperationTime = 0;
}

void
GameDataManager::ResetGame()
{
    // tell all game data handlers to reset their data to defaults
    //
    for( unsigned int i = 0; i < m_numRegisteredGameData; i++ )
    {
        rAssert( m_registeredGameData[ i ] != NULL &&
                 m_registeredGameData[ i ]->m_gdHandler != NULL );

        m_registeredGameData[ i ]->m_gdHandler->ResetData();
    }

    // game is no longer loaded
    //
    m_isGameLoaded = false; 
}

void
GameDataManager::SetMinimumLoadSaveTime( unsigned int minimumTime )
{
    m_minimumLoadSaveTime = minimumTime;
}

void
GameDataManager::RestoreDefaultMinimumLoadSaveTime()
{
#ifdef ENABLE_MINIMUM_LOAD_SAVE_TIME
    m_minimumLoadSaveTime = MINIMUM_LOAD_SAVE_TIME;
#else
    m_minimumLoadSaveTime = 0;
#endif
}

bool
GameDataManager::GetSaveGameInfo( IRadDrive* pDrive, unsigned int slot, 
                                 SaveGameInfo* saveGameInfo,
                                 bool  *file_corrupt_flag)
{
    rAssert( pDrive != NULL );
    rAssert( saveGameInfo != NULL );
	// open save game file for checking if it exists
	//
	char filename[ radFileFilenameMax + 1 ];

#ifdef RAD_XBOX
	// xbox, we get the filename from the drive instead of predefined slot based filename
	IRadDrive::DirectoryInfo dir_info;
    dir_info.m_Type = IRadDrive::DirectoryInfo::IsDone; // initialize first, in case it errs out

 
	pDrive->FindFirstSync("*",&dir_info);

	if (dir_info.m_Type==IRadDrive::DirectoryInfo::IsDone)
		return false;

	for (unsigned int i = 0; i < slot; i++)
	{
        dir_info.m_Type = IRadDrive::DirectoryInfo::IsDone; // initialize first, in case it errs out
		pDrive->FindNextSync(&dir_info);
		if (dir_info.m_Type==IRadDrive::DirectoryInfo::IsDone)
			break;
	}

	if (dir_info.m_Type==IRadDrive::DirectoryInfo::IsDone)
		return false;
	strcpy(filename,dir_info.m_Name);

#else
    this->FormatSavedGameFilename( filename,
                                   sizeof( filename ),
                                   slot );
#endif

    m_lastError = Success; // want to know what is the error from opensync
    if (file_corrupt_flag)
        *file_corrupt_flag = false;


#ifndef RAD_WIN32
	bool simpleName = false;
#ifdef RAD_XBOX
	simpleName = true; // set up flag for xbox so radcore doesn't process ':' '/' character
#endif
    pDrive->SaveGameOpenSync( &m_radFile,
                              filename,
                              false,
                              OpenExisting,
							  0,
							  0,
							  simpleName);
#else

    eFileOperation temp = m_currentFileOperation;
    m_currentFileOperation = FILE_OP_FILE_CHECK;
    pDrive->FileOpenSync( &m_radFile,
                           filename,
                           false,
                           OpenExisting );

    m_currentFileOperation = temp;

#endif // ~RAD_WIN32

    rAssert( m_radFile );
    bool saveGameExists = false;
// since for xbox we know the file is definitely there
// we could get FileNotFound because of old files, just to cut down
// on bug report, let's treat that as corrupt file
#ifdef RAD_XBOX
    if (m_lastError==FileNotFound)
        m_lastError = DataCorrupt;
#endif

    if (m_lastError==DataCorrupt && file_corrupt_flag)
    {
        *file_corrupt_flag = true;
        saveGameExists = true;
    }
    else
    {
        saveGameExists = m_radFile->IsOpen();
        if( saveGameExists )
        {
            // read save game info from file (synchronously)
            //
            unsigned int numBytes = SaveGameInfo::GetSize();

            rAssert( m_gameDataBuffer == NULL );
    #ifndef WORLD_BUILDER
        HeapMgr()->PushHeap( GMA_TEMP );
    #endif
            m_gameDataBuffer = new GameDataByte[ numBytes ];
    #ifndef WORLD_BUILDER
        HeapMgr()->PopHeap( GMA_TEMP );
    #endif
            rAssert( m_gameDataBuffer != NULL );

            m_radFile->ReadSync( m_gameDataBuffer, numBytes );
            saveGameInfo->LoadData( m_gameDataBuffer, numBytes );

            if( m_gameDataBuffer != NULL )
            {
                delete [] m_gameDataBuffer;
                m_gameDataBuffer = NULL;
            }
        }
    }

#ifdef RAD_XBOX
    strcpy(saveGameInfo->m_displayFilename, filename); // use the filename as the display name
#else
    saveGameInfo->FormatDisplay( saveGameInfo->m_displayFilename, sizeof(saveGameInfo->m_displayFilename) );
#endif
 
    m_radFile->Release();
    m_radFile = NULL;

    return saveGameExists;
}

bool
GameDataManager::DoesSaveGameExist( IRadDrive* pDrive, bool check_valid/* = true*/, bool forAllSlots )
{
    bool saveGameExists = false;
    unsigned int numSavedGameExists = 0;

    unsigned int to_check_file = NUM_GAME_SLOTS;
#ifdef RAD_XBOX
    to_check_file = 1; // on xbox since we query the drive for the file,
                       // just check first one
#endif

    // check if at least one saved game file exists on the drive
    //
    for( unsigned int i = 0; i < to_check_file; i++ )
    {
        SaveGameInfo saveGameInfo;
        bool corrupt = false;
        if( this->GetSaveGameInfo( pDrive, i, &saveGameInfo, &corrupt ) )
        {
            if( !check_valid ) // no need to check validity
            {
                saveGameExists = true;

                if( forAllSlots )
                {
                    numSavedGameExists++;
                    continue;
                }

                break;
            }
            // ok, we found a saved game, but let's make sure the saved game
            // info data are valid so we at least know the file size is correct
            //
            else if( !corrupt && saveGameInfo.CheckData() )
            {
                // everything's cool, we have a valid saved game
                //
                saveGameExists = true;

                if( forAllSlots )
                {
                    numSavedGameExists++;
                    continue;
                }

                break;
            }
        }
        else
        {
            // no save game file for current slot
            //
            if( forAllSlots )
            {
                // if checking for save game file exists in all slots, then
                // we've found a slot w/out a save game file, so stop checking
                //
                break;
            }
        }
    }

    if( forAllSlots )
    {
        saveGameExists = (numSavedGameExists == NUM_GAME_SLOTS);
    }

    return saveGameExists;
}

bool
GameDataManager::FindMostRecentSaveGame( IRadDrive* pDrive,
                                         unsigned int& slot,
                                         radDate& timeStamp )
{
    bool saveGameExists = false;

    timeStamp.m_Year = 0;

    // search for most recent save game file on the drive
    //
    for( unsigned int i = 0; i < NUM_GAME_SLOTS; i++ )
    {
        SaveGameInfo saveGameInfo;
        if( this->GetSaveGameInfo( pDrive, i, &saveGameInfo ) )
        {
            if( saveGameInfo.CheckData() )
            {
                // everything's cool, we have a valid saved game
                //
                saveGameExists = true;

                // now let's check if this is the most recent one thus far
                //
                const SaveGameInfoData* pData = saveGameInfo.GetData();
                rAssert( pData != NULL );
                if( SaveGameInfo::CompareTimeStamps( pData->m_timeStamp, timeStamp ) > 0 )
                {
                    memcpy( &timeStamp, &pData->m_timeStamp, sizeof( radDate ) );

                    slot = i; // update most recent saved game slot
                }
            }
        }
    }

    return saveGameExists;
}

void
GameDataManager::FormatSavedGameFilename( char* filename,
                                          unsigned int filenameLength,
                                          unsigned int slot )
{
    rAssert( filename != NULL );
    rAssert( filenameLength >= 32 );

#ifdef RAD_XBOX
    sprintf( filename, "Saved Game (Slot %d)", slot + 1 );
#else
    sprintf( filename, "%sSave%d", PS2_FILENAME_PREFIX, slot + 1 );
#endif
}

void
GameDataManager::OnFileOperationsComplete( void* pUserData )
{
    switch( m_currentFileOperation )
    {
        // cases during loading
        //

        case FILE_OP_OPEN_FOR_READING: // done opening file for reading
        {
            rAssert( m_radFile );
            rAssert( m_radFile->IsOpen() );

            m_radFile->ReadAsync( m_gameDataBuffer,
                                  m_gameDataSize );

            m_radFile->AddCompletionCallback( this, NULL );
            m_currentFileOperation = FILE_OP_READ;

            break;
        }

        case FILE_OP_READ: // done reading data from file
        {
            rAssert( m_radFile );

            m_radFile->Release();
            m_radFile = NULL;

            bool loadOK = this->LoadAllData();
            if( loadOK )
            {
#ifdef ENABLE_MINIMUM_LOAD_SAVE_TIME
                m_currentFileOperation = FILE_OP_LOAD_COMPLETED;
#else
                if( m_gameDataLoadCallback != NULL )
                {
                    m_gameDataLoadCallback->OnLoadGameComplete( Success );
                }

                m_currentFileOperation = FILE_OP_NONE;
#endif
                rReleasePrintf( ">> Load game completed successfully. (%d ms)\n\n",
                                m_elapsedOperationTime );
            }
            else
            {
                if( m_gameDataLoadCallback != NULL )
                {
                    m_gameDataLoadCallback->OnLoadGameComplete( DataCorrupt );
                }

                m_currentFileOperation = FILE_OP_NONE;
            }

            // free up temp memory allocated for game data buffer
            //
            if( m_gameDataBuffer != NULL )
            {
                delete [] m_gameDataBuffer;
                m_gameDataBuffer = NULL;
            }

            break;
        }

        // cases during saving
        //

        case FILE_OP_OPEN_FOR_WRITING: // done opening file for writing
        {
            rAssert( m_radFile );
            rAssert( m_radFile->IsOpen() );

            m_radFile->WriteAsync( m_gameDataBuffer,
                                   m_gameDataSize );

            m_radFile->AddCompletionCallback( this, NULL );
            m_currentFileOperation = FILE_OP_WRITE;

            break;
        }

        case FILE_OP_WRITE: // done writing data to file
        {
            rAssert( m_radFile );

            m_radFile->CommitAsync();

            m_radFile->AddCompletionCallback( this, NULL );
            m_currentFileOperation = FILE_OP_COMMIT;

            break;
        }

        case FILE_OP_COMMIT: // done committing all data writes to file
        {
            rAssert( m_radFile );

            m_radFile->Release();
            m_radFile = NULL;

#ifdef ENABLE_MINIMUM_LOAD_SAVE_TIME
            m_currentFileOperation = FILE_OP_SAVE_COMPLETED;
#else
            if( m_gameDataSaveCallback != NULL )
            {
                m_gameDataSaveCallback->OnSaveGameComplete( Success );
            }

            m_currentFileOperation = FILE_OP_NONE;
#endif
            rReleasePrintf( ">> Save game completed successfully. (%d ms)\n\n",
                            m_elapsedOperationTime );

            // free up temp memory allocated for game data buffer
            //
            if( m_gameDataBuffer != NULL )
            {
                delete [] m_gameDataBuffer;
                m_gameDataBuffer = NULL;
            }

            break;
        }

        default:
        {
            rAssertMsg( m_currentFileOperation == FILE_OP_NONE,
                        "ERROR: *** Invalid file operation!" );

            break;
        }
    }
}

void
GameDataManager::OnDriveOperationsComplete( void* pUserData )
{
    switch( m_currentFileOperation )
    {
        case FILE_OP_DELETE:
        {
            m_currentFileOperation = FILE_OP_DELETE_COMPLETED;

            break;
        }
        default:
        {
            break;
        }
    }
}

bool
GameDataManager::OnDriveError( radFileError error,
                               const char* pDriveName,
                               void* pUserData )
{
    m_lastError = error; // cache error for synchronous call 
    switch( m_currentFileOperation )
    {
        case FILE_OP_OPEN_FOR_READING:
        case FILE_OP_READ:
        {
            rTunePrintf( "*** Error [%d] occurred on drive [%s] during loading!\n",
                         error, pDriveName );

#ifdef RAD_XBOX
            // on xbox we know the filename already
            //  this could happen because of old version file format

            if (m_lastError==FileNotFound)
                m_lastError = DataCorrupt;
#endif

            if( m_gameDataLoadCallback != NULL )
            {
                m_gameDataLoadCallback->OnLoadGameComplete( error );
            }

            if( m_radFile != NULL )
            {
                m_radFile->Release();
                m_radFile = NULL;
            }

            // free up temp memory allocated for game data buffer
            //
            if( m_gameDataBuffer != NULL )
            {
                delete [] m_gameDataBuffer;
                m_gameDataBuffer = NULL;
            }

            m_currentFileOperation = FILE_OP_NONE;

            break;
        }
        case FILE_OP_OPEN_FOR_WRITING:
        case FILE_OP_WRITE:
        case FILE_OP_COMMIT:
        {
            rTunePrintf( "*** Error [%d] occurred on drive [%s] during saving!\n",
                         error, pDriveName );

            if( m_gameDataSaveCallback != NULL )
            {
                m_gameDataSaveCallback->OnSaveGameComplete( error );
            }

            if( m_radFile != NULL )
            {
                m_radFile->Release();
                m_radFile = NULL;
            }

            // free up temp memory allocated for game data buffer
            //
            if( m_gameDataBuffer != NULL )
            {
                delete [] m_gameDataBuffer;
                m_gameDataBuffer = NULL;
            }

            m_currentFileOperation = FILE_OP_NONE;

            break;
        }
        case FILE_OP_DELETE:
        {
            rTunePrintf( "*** Error [%d] occurred on drive [%s] during deleting!\n",
                         error, pDriveName );

            if( m_gameDataDeleteCallback != NULL )
            {
                m_gameDataDeleteCallback->OnDeleteGameComplete( error );
                m_gameDataDeleteCallback = NULL;
            }

            m_currentFileOperation = FILE_OP_NONE;

            break;
        }
        default:
        {
//            rDebugPrintf( "*** WARNING: Unhandled drive error [%d] occurred on drive: %s\n",
//                          error, pDriveName );

            break;
        }
    }

    // we should always return false, since we're always going to prompt
    // the user first before attempting to retry any operation
    //
    return false;
}

bool GameDataManager::IsUsingDrive() const
{
    return m_currentFileOperation != FILE_OP_NONE;
}

//===========================================================================
// Private Member Functions
//===========================================================================

bool
GameDataManager::LoadAllData()
{
    GameDataByte* currentGameDataBuffer = m_gameDataBuffer;

    for( unsigned int i = 0; i < m_numRegisteredGameData; i++ )
    {
        rAssert( m_registeredGameData[ i ] );

        // get number of bytes for current game data
        //
        unsigned int numDataBytes = m_registeredGameData[ i ]->m_numBytes;
        rAssert( numDataBytes >= 0 );

#ifdef PRINT_RAW_DATA
        rDebugPrintf( "Game Data Load (%s): ",
                      m_registeredGameData[ i ]->m_name );

        this->PrintRawData( currentGameDataBuffer, numDataBytes );

        rDebugPrintf( "\n" );
#endif

        // call current game data handler to load data
        //
        rAssert( m_registeredGameData[ i ]->m_gdHandler );
        m_registeredGameData[ i ]->m_gdHandler->LoadData( currentGameDataBuffer,
                                                          numDataBytes );

        if( i == 0 ) // 0 = saved game info data handler
        {
            bool isDataValid = m_saveGameInfoHandler->CheckData();
            if( !isDataValid )
            {
                // saved game info data is not valid, assume file corrupted
                //
                return false;
            }
        }

        // advance reference to current game data buffer
        //
        currentGameDataBuffer += numDataBytes;
    }

    // set flag indicating that a saved game has been loaded
    //
    m_isGameLoaded = true;

    // sanity check
    //
    rAssert( currentGameDataBuffer - m_gameDataBuffer == static_cast<int>( m_gameDataSize ) );

    return true;
}

bool
GameDataManager::SaveAllData()
{
MEMTRACK_PUSH_GROUP( "GameDataManager" );
    GameDataByte* currentGameDataBuffer = m_gameDataBuffer;

    for( unsigned int i = 0; i < m_numRegisteredGameData; i++ )
    {
        rAssert( m_registeredGameData[ i ] );

        // get number of bytes for current game data
        //
        unsigned int numDataBytes = m_registeredGameData[ i ]->m_numBytes;
        rAssert( numDataBytes >= 0 );
/*
        // create temporary buffer for client to write in
        //
        GameDataByte* tempDataBuffer = new( GMA_TEMP ) GameDataByte[ numDataBytes ];
        rAssert( tempDataBuffer );
*/
        // call current game data handler to save data
        //
        rAssert( m_registeredGameData[ i ]->m_gdHandler );
        m_registeredGameData[ i ]->m_gdHandler->SaveData( currentGameDataBuffer,
                                                          numDataBytes );
/*
        // copy data from temporary buffer to current buffer
        //
        memcpy( currentGameDataBuffer, tempDataBuffer, numDataBytes );

        // and destroy temporary data buffer
        //
        if( tempDataBuffer != NULL )
        {
            delete tempDataBuffer;
            tempDataBuffer = NULL;
        }
*/
#ifdef PRINT_RAW_DATA
        rDebugPrintf( "Game Data Save (%s): ",
                      m_registeredGameData[ i ]->m_name );

        this->PrintRawData( currentGameDataBuffer, numDataBytes );

        rDebugPrintf( "\n" );
#endif

        // advance reference to current game data buffer
        //
        currentGameDataBuffer += numDataBytes;
    }

    // sanity check
    //
    rAssert( currentGameDataBuffer - m_gameDataBuffer == static_cast<int>( m_gameDataSize ) );
    MEMTRACK_POP_GROUP("GameDataManager");
    return true;
}

void
GameDataManager::PrintRawData( GameDataByte* dataBuffer,
                               unsigned int numBytes )
{
    rAssert( dataBuffer != NULL );

    for( unsigned int i = 0; i < numBytes; i++ )
    {
        rDebugPrintf( "[ %02X ]", dataBuffer[ i ] );
    }
}

