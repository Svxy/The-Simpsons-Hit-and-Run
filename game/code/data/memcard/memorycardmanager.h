//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.				
//
// Component:   MemoryCardManager
//
// Description: Interface for the MemoryCardManager class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/09/16      TChu        Created for SRR2
//
//===========================================================================

#ifndef MEMORYCARDMANAGER_H
#define MEMORYCARDMANAGER_H

//===========================================================================
// Nested Includes
//===========================================================================
#ifndef WORLD_BUILDER
#include <loading/loadingmanager.h>
#else
namespace LoadingManager
{
    class ProcessRequestsCallback
    {
    };
}

#endif

#include <radfile.hpp>

#ifdef RAD_GAMECUBE
    #include <charPipeline/texPalette.h>
    #include <dolphin/dvd/DVDBanner.h>
#endif

//===========================================================================
// Forward References
//===========================================================================

struct IMemoryCardInfoLoadCallback
{
    virtual void OnMemcardInfoLoadComplete() = 0;
};

struct IMemoryCardFormatCallback
{
	virtual void OnFormatOperationComplete(radFileError err) = 0;
};
struct IMemoryCardCheckCallback
{
    virtual void OnMemoryCardCheckDone( radFileError errorCode,
                                        IRadDrive::MediaInfo::MediaState mediaState,
                                        int driveIndex,
                                        int mostRecentSaveGameDriveIndex,
                                        int mostRecentSaveGameSlot ) = 0;
};



//===========================================================================
// Interface Definitions
//===========================================================================

class MemoryCardManager : public LoadingManager::ProcessRequestsCallback,
                          public IRadDriveCompletionCallback,
                          public IRadDriveErrorCallback
{
public:
    // Static Methods for accessing this singleton.
    static MemoryCardManager* CreateInstance();
    static void DestroyInstance();
    static MemoryCardManager* GetInstance();

    enum eState
    {
        STATE_UNINITIALIZED,
        STATE_OPENING_DRIVES,
        STATE_READY,
        STATE_CHECKING_MEMCARDS,

        NUM_STATES
    };

	MemoryCardManager();
    virtual ~MemoryCardManager();

    virtual void AddRef() {;}
    virtual void Release() {;}

    eState GetCurrentState() const { return m_currentState; }

    // Initialization
    //
    void Init( IRadDriveErrorCallback* radDriveErrorCallback = NULL );

    void Update( unsigned int elapsedTime );

    // Boot-up Memory Card Check
    //
    void StartMemoryCardCheck( IMemoryCardCheckCallback* callback );
    void UpdateMemoryCardCheck( unsigned int elapsedTime );

    // Media Info Accessor
    //
    const IRadDrive::MediaInfo* GetMediaInfo( unsigned int driveIndex ) const;
    unsigned int GetSavedGameCreationSize( unsigned int driveIndex ) const;

    // Loading/Unloading Memory Card Info
    //
    void LoadMemcardInfo( IMemoryCardInfoLoadCallback* callback = NULL );
    void UnloadMemcardInfo();
    bool IsMemcardInfoLoaded() const;

    void SetMemcardIconData( char* dataBuffer, unsigned int dataSize );

    void OnProcessRequestsComplete( void* pUserData );   
    
    bool HasSaveGame(unsigned int driveIndex);

    // Current Drive Accessors
    //
    void SetCurrentDrive( IRadDrive* currentDrive ) { m_currentDrive = currentDrive; }
    void SetCurrentDrive( unsigned int driveIndex );
    IRadDrive* GetCurrentDrive() const { return m_currentDrive; }
	const char *GetDriveName(unsigned int driveIndex) const;
	const char *GetCurrentDriveVolumeName() const;
    int GetCurrentDriveIndex() const;
	void ClearCurrentDrive();

	void FormatDrive(unsigned int driveIndex, IMemoryCardFormatCallback *callback);

	bool IsCurrentDriveReady( bool forceSyncUpdate = false, 
                              bool *unformatted = NULL,
                              IRadDrive::MediaInfo::MediaState *errorOut = NULL);
	bool IsCurrentDrivePresent( unsigned int driveIndex );

    // Query current list of available drives
    //
    int GetAvailableDrives( IRadDrive** pDrives, IRadDrive::MediaInfo** mediaInfos, IRadDrive **drive_mounted = NULL );
    int GetNumAvailableDrives()
    {
        return GetAvailableDrives( NULL, NULL );
    }

    bool EnoughFreeSpace( unsigned int driveIndex ) const;

    // Memory Card Info
    //
    const radMemcardInfo* GetMemcardInfo() const { return m_memcardInfo; }
    void UpdateMemcardInfo( const char* savedGameTitle, int lineBreak = -1 );

    // Implements IRadDriveCompletionCallback
    //
    virtual void OnDriveOperationsComplete( void* pUserData );

    // Implements IRadDriveErrorCallback
    //
    virtual bool OnDriveError( radFileError error, const char* pDriveName, void* pUserData );

    int GetDriveIndex( IRadDrive* pDrive ) const;

private:
    //---------------------------------------------------------------------
    // Private Functions
    //---------------------------------------------------------------------

    // No copying or assignment. Declare but don't define.
    //
    MemoryCardManager( const MemoryCardManager& );
    MemoryCardManager& operator= ( const MemoryCardManager& );

    void DetermineSavedGameCreationSize( unsigned int driveIndex );
    void OnMemoryCardCheckCompleted();

#ifdef RAD_GAMECUBE
    void UnpackTexPalette( TEXPalettePtr pal );

    void LoadMemcardInfo_GC( GameMemoryAllocator heap );
    void UnloadMemcardInfo_GC();
#endif

#ifdef RAD_PS2
    void LoadMemcardInfo_PS2( GameMemoryAllocator heap );
    void UnloadMemcardInfo_PS2();
#endif

#ifdef RAD_XBOX
    void LoadMemcardInfo_XBOX( GameMemoryAllocator heap );
    void UnloadMemcardInfo_XBOX();
#endif

    //---------------------------------------------------------------------
    // Private Data
    //---------------------------------------------------------------------

    // Pointer to the one and only instance of this singleton.
    static MemoryCardManager* spInstance;

    eState m_currentState;
    unsigned int m_numDrivesOpened;

#ifdef RAD_GAMECUBE
    enum eMemcardInfoLoadState
    {
        MEMCARD_INFO_NOT_LOADED,
        MEMCARD_INFO_LOADING_BANNER,
        MEMCARD_INFO_LOADING_TEXPALETTE,

        MEMCARD_INFO_LOAD_COMPLETED
    };

    DVDBanner* m_dvdBanner;
    TEXPalette* m_texPalette;
#endif

#ifdef RAD_PS2
    enum eMemcardInfoLoadState
    {
        MEMCARD_INFO_NOT_LOADED,
        MEMCARD_INFO_LOADING_ICON_LIST,
        MEMCARD_INFO_LOADING_ICON_COPY,
        MEMCARD_INFO_LOADING_ICON_DELETE,

        MEMCARD_INFO_LOAD_COMPLETED
    };
#endif

#ifdef RAD_XBOX
    enum eMemcardInfoLoadState
    {
        MEMCARD_INFO_NOT_LOADED,
        MEMCARD_INFO_LOADING_ICON,

        MEMCARD_INFO_LOAD_COMPLETED
    };
#endif

#ifdef RAD_WIN32
    enum eMemcardInfoLoadState
    {
        MEMCARD_INFO_NOT_LOADED,
        MEMCARD_INFO_LOAD_COMPLETED
    };
#endif

    radMemcardInfo* m_memcardInfo;
    unsigned int m_memcardInfoLoadState;
    IMemoryCardInfoLoadCallback* m_memcardInfoLoadCallback;
    IRadFile* m_radFile;
    unsigned int m_elapsedMemcardInfoLoadTime;

    IRadDrive** m_pDrives;
    IRadDrive* m_currentDrive;
    IRadDriveErrorCallback* m_radDriveErrorCallback;

    IRadDrive::MediaInfo* m_mediaInfos;
    int m_currentMediaInfo;
    int m_nextMediaInfo;
	bool m_formatDriveState;
	IMemoryCardFormatCallback* m_formatCallback;

    enum eMemcardCheckingState
    {
        MEMCARD_CHECK_NOT_DONE,
        MEMCARD_CHECK_IN_PROGRESS,
        MEMCARD_CHECK_COMPLETED
    };

    IMemoryCardCheckCallback* m_memcardCheckCallback;
    eMemcardCheckingState m_memcardCheckingState;
    unsigned int m_elapsedMemcardCheckTime;

#ifdef RAD_XBOX
    unsigned int m_savedGameCreationSizeHD;
#endif
    unsigned int m_savedGameCreationSize;

};

// A little syntactic sugar for getting at this singleton.
inline MemoryCardManager* GetMemoryCardManager() { return( MemoryCardManager::GetInstance() ); }

inline unsigned int
MemoryCardManager::GetSavedGameCreationSize( unsigned int driveIndex ) const
{
#ifdef RAD_XBOX
    if( driveIndex == 0 ) // Xbox hard disk
    {
        return m_savedGameCreationSizeHD;
    }
    else
#endif
    {
        return m_savedGameCreationSize;
    }
}

#endif // MEMORYCARDMANAGER_H
