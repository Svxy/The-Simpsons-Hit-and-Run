//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   MemoryCardManager										
//
// Description: Implementation of the MemoryCardManager class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/09/16      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <data/memcard/memorycardmanager.h>
#include <data/gamedatamanager.h>
#include <data/savegameinfo.h>

#ifndef WORLD_BUILDER
#include <gameflow/gameflow.h>
#endif

#include <radfile.hpp>

#ifndef WORLD_BUILDER
#include <memory/srrmemory.h>
#else
#define MEMTRACK_PUSH_GROUP(x)
#define MEMTRACK_POP_GROUP(x)
#define GMA_PERSISTENT 0
#define GMA_DEFAULT 0
#define GMA_TEMP 0
#endif

#include <string.h>

// Static pointer to instance of singleton.
MemoryCardManager* MemoryCardManager::spInstance = NULL;

//===========================================================================
// Local Constants
//===========================================================================

#ifdef RAD_PS2
  #ifdef PAL
    const unsigned int MINIMUM_MEMCARD_CHECK_TIME = 3000; // in msec
  #else
    const unsigned int MINIMUM_MEMCARD_CHECK_TIME = 0; // in msec
  #endif
#elif RAD_WIN32
    const unsigned int MINIMUM_MEMCARD_CHECK_TIME = 0; // in msec
#else
    const unsigned int MINIMUM_MEMCARD_CHECK_TIME = 1000; // in msec
#endif

const int MAX_SAVED_GAME_TITLE_LENGTH = 32; // # chars

#ifdef RAD_WIN32
char DEFAULT_GAME_DRIVE[radFileDrivenameMax+1]; // for win32, need to store the default.
#endif

const char* SAVE_GAME_DRIVE[] =
{
#ifdef RAD_GAMECUBE
    "MEMCARDCHANNELA:",
    "MEMCARDCHANNELB:",
#endif

#ifdef RAD_PS2
    "MEMCARD1A:",
    "MEMCARD1B:",
    "MEMCARD1C:",
    "MEMCARD1D:",
    "MEMCARD2A:",
    "MEMCARD2B:",
    "MEMCARD2C:",
    "MEMCARD2D:",
#endif

#ifdef RAD_XBOX
//    "T:",
    "U:",
    "MEMCARD1A:",
    "MEMCARD1B:",
    "MEMCARD2A:",
    "MEMCARD2B:",
    "MEMCARD3A:",
    "MEMCARD3B:",
    "MEMCARD4A:",
    "MEMCARD4B:",
#endif

#ifdef RAD_WIN32
    DEFAULT_GAME_DRIVE,
#endif

    "" // dummy terminator
};

const unsigned int NUM_SAVE_GAME_DRIVES =
    sizeof( SAVE_GAME_DRIVE ) / sizeof( SAVE_GAME_DRIVE[ 0 ] ) - 1;

#ifdef RAD_GAMECUBE
    const char* GC_BANNER_FILE = "opening.bnr";
    const char* GC_TPL_FILE = "icon.tpl";
#endif

#ifdef RAD_PS2
    const char* PS2_LIST_ICON_FILE = "list.ico";
    const char* PS2_COPY_ICON_FILE = "copy.ico";
    const char* PS2_DELETE_ICON_FILE = "delete.ico";

    // this will use the 'list' icon file for all three icons
    //
#define USE_ONE_ICON_FILE_ONLY
#endif

#ifdef RAD_XBOX
    const char* XBOX_ICON_FILE = "saveimg.xbx";

    // this will use the default title and save game image icons
    //
    #define USE_DEFAULT_ICONS
#endif

//===========================================================================
// Public Member Functions
//===========================================================================

//==============================================================================
// MemoryCardManager::CreateInstance
//==============================================================================
//
// Description: - Creates the Game Data Manager.
//
// Parameters:	None.
//
// Return:      Pointer to the MemoryCardManager.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
MemoryCardManager* MemoryCardManager::CreateInstance()
{
    spInstance = new MemoryCardManager;
    rAssert( spInstance != NULL );

    return spInstance;
}

//==============================================================================
// MemoryCardManager::DestroyInstance
//==============================================================================
//
// Description: Destroy the Game Data Manager.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void MemoryCardManager::DestroyInstance()
{
    rAssert( spInstance != NULL );

    delete spInstance;
    spInstance = NULL;
}

//==============================================================================
// MemoryCardManager::GetInstance
//==============================================================================
//
// Description: - Access point for the MemoryCardManager singleton.  
//              - Creates the MemoryCardManager if needed.
//
// Parameters:	None.
//
// Return:      Pointer to the MemoryCardManager.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
MemoryCardManager* MemoryCardManager::GetInstance()
{
    rAssert( spInstance != NULL );

    return spInstance;
}

//===========================================================================
// MemoryCardManager::MemoryCardManager
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
MemoryCardManager::MemoryCardManager()
:   m_currentState( STATE_UNINITIALIZED ),
    m_numDrivesOpened( 0 ),
    m_memcardInfo( NULL ),
    m_memcardInfoLoadState( MEMCARD_INFO_NOT_LOADED ),
    m_memcardInfoLoadCallback( NULL ),
    m_radFile( NULL ),
    m_elapsedMemcardInfoLoadTime( 0 ),
    m_pDrives( NULL ),
    m_currentDrive( NULL ),
    m_radDriveErrorCallback( NULL ),
    m_mediaInfos( NULL ),
    m_currentMediaInfo( -1 ),
    m_nextMediaInfo( 0 ),
	m_formatDriveState( false ),
    m_formatCallback( NULL ),
    m_memcardCheckCallback( NULL ),
    m_memcardCheckingState( MEMCARD_CHECK_NOT_DONE ),
    m_elapsedMemcardCheckTime( 0 ),
#ifdef RAD_XBOX
    m_savedGameCreationSizeHD( 0 ),
#endif
    m_savedGameCreationSize( 0 )
{
    m_pDrives = new IRadDrive*[ NUM_SAVE_GAME_DRIVES ];
    rAssert( m_pDrives != NULL );

    m_mediaInfos = new IRadDrive::MediaInfo[ NUM_SAVE_GAME_DRIVES ];
    rAssert( m_mediaInfos != NULL );

    for( unsigned int i = 0; i < NUM_SAVE_GAME_DRIVES; i++ )
    {
        m_pDrives[ i ] = NULL;

        m_mediaInfos[ i ].m_FreeFiles = 0;
        m_mediaInfos[ i ].m_FreeSpace = 0;
        m_mediaInfos[ i ].m_MediaState = IRadDrive::MediaInfo::MediaNotPresent;
        m_mediaInfos[ i ].m_SectorSize = 0;
        m_mediaInfos[ i ].m_VolumeName[ 0 ] = '\0';
    }

/*
#ifdef RAD_WIN32
    // Algorithm fix which is only needed on PC.  If there is only one card,
    // then having m_nextmediainfo = m_currentMediaInfo = 0 won't load anything.
    // We start currentmediainfo at -1 to indicate that nothing has been loaded.
    m_currentMediaInfo = -1;
#endif
*/
}

//===========================================================================
// MemoryCardManager::~MemoryCardManager
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
MemoryCardManager::~MemoryCardManager()
{
    if( m_pDrives != NULL )
    {
        // release and un-mount drives
        //
        for( unsigned int i = 0; i < NUM_SAVE_GAME_DRIVES; i++ )
        {
            if( m_pDrives[ i ] != NULL )
            {
#ifndef RAD_WIN32
                m_pDrives[ i ]->UnregisterErrorHandler( this );
#endif

                m_pDrives[ i ]->Release();
                m_pDrives[ i ] = NULL;

                m_numDrivesOpened--;
            }

#ifndef RAD_WIN32
            radDriveUnmount( SAVE_GAME_DRIVE[ i ] );
#endif
        }

        rAssert( m_numDrivesOpened == 0 );

        delete [] m_pDrives;
        m_pDrives = NULL;
    }

    if( m_mediaInfos != NULL )
    {
        delete [] m_mediaInfos;
        m_mediaInfos = NULL;
    }

    m_currentState = STATE_UNINITIALIZED;
}

void
MemoryCardManager::Init( IRadDriveErrorCallback* radDriveErrorCallback )
{
    m_radDriveErrorCallback = radDriveErrorCallback;

#ifdef RAD_WIN32
    radGetDefaultDrive( DEFAULT_GAME_DRIVE );
#endif

    // mount and open drives
    //
    for( unsigned int i = 0; i < NUM_SAVE_GAME_DRIVES; i++ )
    {
#ifndef RAD_WIN32
        bool driveAlreadyMounted = radDriveMount( SAVE_GAME_DRIVE[ i ], GMA_PERSISTENT );
        rAssert( !driveAlreadyMounted );
#endif

        radDriveOpenAsync( &m_pDrives[ i ], SAVE_GAME_DRIVE[ i ] );
        rAssert( m_pDrives[ i ] );
        m_pDrives[ i ]->AddCompletionCallback( this, reinterpret_cast<void*>( i ) );

        // register error handler
        //
#ifndef RAD_WIN32
        m_pDrives[ i ]->RegisterErrorHandler( this, NULL );
#endif
    }

    m_currentState = STATE_OPENING_DRIVES;
}

void
MemoryCardManager::Update( unsigned int elapsedTime )
{
    if( m_currentState == STATE_READY )
    {
        if( m_currentMediaInfo != m_nextMediaInfo )
        {
            m_currentMediaInfo = m_nextMediaInfo;

            // get the next media info asynchronously
            //
            rAssert( m_pDrives[ m_currentMediaInfo ] != NULL );
            m_pDrives[ m_currentMediaInfo ]->GetMediaInfoAsync( &m_mediaInfos[ m_currentMediaInfo ] );
            m_pDrives[ m_currentMediaInfo ]->AddCompletionCallback( this, NULL );
        }
    }
}

void
MemoryCardManager::StartMemoryCardCheck( IMemoryCardCheckCallback* callback )
{
    m_memcardCheckCallback = callback;
    m_elapsedMemcardCheckTime = 0;

    m_currentMediaInfo = -1;
    m_nextMediaInfo = 0;

    m_memcardCheckingState = MEMCARD_CHECK_IN_PROGRESS;
}

void
MemoryCardManager::UpdateMemoryCardCheck( unsigned int elapsedTime )
{
    m_elapsedMemcardCheckTime += elapsedTime;
    if( m_elapsedMemcardCheckTime > MINIMUM_MEMCARD_CHECK_TIME )
    {
        if( m_memcardCheckingState == MEMCARD_CHECK_COMPLETED )
        {
            if( this->IsMemcardInfoLoaded() )
            {
                this->OnMemoryCardCheckCompleted();
            }
            else
            {
                // memory card info must be loaded/loading during memory card check,
                // and should be unloaded when check is completed
                //
                rAssertMsg( m_memcardInfoLoadState != MEMCARD_INFO_NOT_LOADED,
                            "ERROR: *** Memory card info not loaded during memory card check!" );
            }
        }
    }

    if( m_memcardCheckingState == MEMCARD_CHECK_IN_PROGRESS )
    {
        this->Update( elapsedTime );
    }
}

const IRadDrive::MediaInfo*
MemoryCardManager::GetMediaInfo( unsigned int driveIndex ) const
{
    rAssert( driveIndex < NUM_SAVE_GAME_DRIVES );

    return &(m_mediaInfos[ driveIndex ] );
}

void
MemoryCardManager::LoadMemcardInfo( IMemoryCardInfoLoadCallback* callback )
{
#ifndef WORLD_BUILDER
    if( m_memcardInfoLoadState == MEMCARD_INFO_NOT_LOADED )
    {
MEMTRACK_PUSH_GROUP( "MemcardInfo" );
        bool isIngame = GetGameFlow()->GetCurrentContext() == CONTEXT_GAMEPLAY ||
                        GetGameFlow()->GetCurrentContext() == CONTEXT_PAUSE;

        GameMemoryAllocator heap = isIngame ? GMA_LEVEL_MISSION : GMA_LEVEL_MOVIE;

        HeapMgr()->PushHeap( heap );

        m_memcardInfoLoadCallback = callback;

        rAssert( m_memcardInfo == NULL );
        m_memcardInfo = new radMemcardInfo;
        rAssert( m_memcardInfo != NULL );

        rTunePrintf( ">> Loading memory card info ... ...\n" );

        m_elapsedMemcardInfoLoadTime = radTimeGetMilliseconds();

#ifdef RAD_GAMECUBE
        this->LoadMemcardInfo_GC( heap );
#endif

#ifdef RAD_PS2
        this->LoadMemcardInfo_PS2( heap );
#endif

#ifdef RAD_XBOX
        this->LoadMemcardInfo_XBOX( heap );
#endif

#ifdef RAD_WIN32
        // go straight to the requests complete method; there is no
        // memcard info.
        OnProcessRequestsComplete( NULL );
#endif

        HeapMgr()->PopHeap( heap );
MEMTRACK_POP_GROUP( "MemcardInfo" );
    }
    else
    {
        rTuneWarningMsg( false, "WARNING: *** Memory card info already loaded! Ignoring request to load again.\n" );
    }
#endif
}

void
MemoryCardManager::UnloadMemcardInfo()
{
    if( m_memcardInfoLoadState == MEMCARD_INFO_LOAD_COMPLETED )
    {
#ifdef RAD_GAMECUBE
        this->UnloadMemcardInfo_GC();
#endif

#ifdef RAD_PS2
        this->UnloadMemcardInfo_PS2();
#endif

#ifdef RAD_XBOX
        this->UnloadMemcardInfo_XBOX();
#endif

        if( m_memcardInfo != NULL )
        {
            delete m_memcardInfo;
            m_memcardInfo = NULL;
        }

        rTunePrintf( ">> Memory card info unloaded.\n" );

        m_memcardInfoLoadState = MEMCARD_INFO_NOT_LOADED;
    }
    else
    {
        rTuneWarningMsg( false, "WARNING: *** Memory card info not completely loaded yet! Ignoring request to unload.\n" );
    }
}

bool
MemoryCardManager::IsMemcardInfoLoaded() const
{
    return (m_memcardInfoLoadState == MEMCARD_INFO_LOAD_COMPLETED);
}

void
MemoryCardManager::SetMemcardIconData( char* dataBuffer,
                                       unsigned int dataSize )
{
    rAssert( dataBuffer != NULL && dataSize > 0 );
    rAssert( m_memcardInfo != NULL );

    switch( m_memcardInfoLoadState )
    {
#ifdef RAD_GAMECUBE
        case MEMCARD_INFO_LOADING_BANNER:
        {
            m_dvdBanner = reinterpret_cast<DVDBanner*>( dataBuffer );

            // This banner is in RGB format
            //
            m_memcardInfo->m_Banner = m_dvdBanner->image;
            m_memcardInfo->m_BannerFormat = radMemcardInfo::RGB5A3;

            // Take the comment from the disk too.
            //
            memcpy( m_memcardInfo->m_CommentLine1, m_dvdBanner->shortTitle, 32 );
            strcpy( m_memcardInfo->m_CommentLine2, "" );

            break;
        }
        case MEMCARD_INFO_LOADING_TEXPALETTE:
        {
            m_texPalette = reinterpret_cast<TEXPalette*>( dataBuffer );

            // We have a TEXPalette, we need to parse it.
            //
            UnpackTexPalette( m_texPalette );

            //
            // Set the animation to loop at find number of frames
            //
            m_memcardInfo->m_AnimType = radMemcardInfo::Loop;

            m_memcardInfo->m_NumFrames = m_texPalette->numDescriptors > 8 ? 8 : m_texPalette->numDescriptors;

            //
            // Set the info for all frames
            //
            for( unsigned int i = 0; i < m_memcardInfo->m_NumFrames; i++ )
            {
                TEXDescriptorPtr tdp = TEXGet( m_texPalette, (u32) i );

                rAssertMsg( tdp->textureHeader->height == 32 && tdp->textureHeader->width == 32,
                            "Icon is the wrong dimension. Should be 32 by 32." );

                //
                // Set format and data
                //
                m_memcardInfo->m_pIcon[ i ].m_CLUT = NULL;

                switch( (GXTexFmt)(tdp->textureHeader->format) )
                {
                    case GX_TF_RGB5A3:
                    {
                        m_memcardInfo->m_pIcon[ i ].m_Format = radMemcardIconData::RGB5A3;
                        m_memcardInfo->m_pIcon[ i ].m_Data = (void*) tdp->textureHeader->data;

                        break;
                    }
                    case GX_TF_C8:
                    {
                        m_memcardInfo->m_pIcon[ i ].m_Format = radMemcardIconData::C8;
                        m_memcardInfo->m_pIcon[ i ].m_Data = (void*) tdp->textureHeader->data;

                        rAssert( tdp->CLUTHeader );
                        rAssert( (GXTlutFmt) tdp->CLUTHeader->format == GX_TL_RGB5A3 );
                        rAssert( tdp->CLUTHeader->numEntries == 256 );

                        m_memcardInfo->m_pIcon[ i ].m_CLUT = (void*) tdp->CLUTHeader->data;

                        break;
                    }
                    default:
                    {
                        rAssertMsg( false, "Unsupported icon texture format." );
                        break;
                    }
                }

                // Set speed
                //
                m_memcardInfo->m_pIcon[ i ].m_Speed = radMemcardIconData::Slow;
            }

            break;
        }
#endif // RAD_GAMECUBE

#ifdef RAD_PS2
        case MEMCARD_INFO_LOADING_ICON_LIST:
        {
            m_memcardInfo->m_ListIcon = dataBuffer;
            m_memcardInfo->m_ListIconSize = dataSize;

#ifdef USE_ONE_ICON_FILE_ONLY
            m_memcardInfo->m_CopyIcon = dataBuffer;
            m_memcardInfo->m_CopyIconSize = dataSize;

            m_memcardInfo->m_DelIcon = dataBuffer;
            m_memcardInfo->m_DelIconSize = dataSize;

            m_memcardInfoLoadState = MEMCARD_INFO_LOADING_ICON_DELETE;
#endif

            break;
        }
        case MEMCARD_INFO_LOADING_ICON_COPY:
        {
            m_memcardInfo->m_CopyIcon = dataBuffer;
            m_memcardInfo->m_CopyIconSize = dataSize;

            break;
        }
        case MEMCARD_INFO_LOADING_ICON_DELETE:
        {
            m_memcardInfo->m_DelIcon = dataBuffer;
            m_memcardInfo->m_DelIconSize = dataSize;

            break;
        }
#endif // RAD_PS2

#ifdef RAD_XBOX
        case MEMCARD_INFO_LOADING_ICON:
        {
            m_memcardInfo->m_Icon = dataBuffer;
            m_memcardInfo->m_IconSize = dataSize;

            break;
        }
#endif // RAD_XBOX

        case MEMCARD_INFO_NOT_LOADED:
        {
            rAssertMsg( false, "*** ERROR: Invalid MemcardInfo load state!" );

            break;
        }

        default:
        {
            break;
        }
    }
}

void
MemoryCardManager::OnProcessRequestsComplete( void* pUserData )
{
    m_memcardInfoLoadState++;

    if( m_memcardInfoLoadState == MEMCARD_INFO_LOAD_COMPLETED )
    {
        // determine amount of time it took to load memcard info
        //
        m_elapsedMemcardInfoLoadTime = radTimeGetMilliseconds() - m_elapsedMemcardInfoLoadTime;

        rTunePrintf( ">> Memory card info loaded. (%d msec)\n",
                     m_elapsedMemcardInfoLoadTime );

        if( m_memcardInfoLoadCallback != NULL )
        {
            m_memcardInfoLoadCallback->OnMemcardInfoLoadComplete();
            m_memcardInfoLoadCallback = NULL;
        }
    }
}

void
MemoryCardManager::SetCurrentDrive( unsigned int driveIndex )
{
    rAssert( driveIndex < NUM_SAVE_GAME_DRIVES );

    this->SetCurrentDrive( m_pDrives[ driveIndex ] );
}
void
MemoryCardManager::ClearCurrentDrive(  )
{
	m_currentDrive = NULL;
}

int
MemoryCardManager::GetCurrentDriveIndex() const
{
    return( this->GetDriveIndex( m_currentDrive ) );
}
const char *
MemoryCardManager::GetDriveName(unsigned int driveIndex) const
{
	rAssert(driveIndex < NUM_SAVE_GAME_DRIVES);
	rAssert(m_pDrives[ driveIndex ]);
	return m_pDrives[driveIndex]->GetDriveName();
}
const char *
MemoryCardManager::GetCurrentDriveVolumeName() const
{
	return m_mediaInfos[ GetCurrentDriveIndex() ].m_VolumeName;
}

bool
MemoryCardManager::IsCurrentDrivePresent( unsigned int currentDriveIndex )
{
	bool isMissing = true;
	if( static_cast<int>( currentDriveIndex ) != -1 )
	{
		isMissing = (m_mediaInfos[ currentDriveIndex ].m_MediaState == IRadDrive::MediaInfo::MediaNotPresent);
	}
	return !isMissing;
}
bool
MemoryCardManager::IsCurrentDriveReady( bool forceSyncUpdate, bool *unformatted , IRadDrive::MediaInfo::MediaState *errorOut)
{
    bool isReady = false;
    bool card_unformatted = false;

    int currentDriveIndex = this->GetCurrentDriveIndex();
	if (unformatted!=NULL)
		*unformatted = false;
    if( currentDriveIndex != -1 )
    {
        if( forceSyncUpdate )
        {
            // force synchronous update on current drive's media info
            //
            m_pDrives[ currentDriveIndex ]->GetMediaInfoSync( &m_mediaInfos[ currentDriveIndex ] );
        }
        if (errorOut)
            *errorOut = m_mediaInfos[ currentDriveIndex ].m_MediaState;

        if (  m_mediaInfos[ currentDriveIndex ].m_MediaState==IRadDrive::MediaInfo::MediaNotFormatted )
        {
            card_unformatted = true;
        }
#ifdef RAD_GAMECUBE
        if (  m_mediaInfos[ currentDriveIndex ].m_MediaState==IRadDrive::MediaInfo::MediaEncodingErr )
            card_unformatted = true;
#endif
        isReady = (m_mediaInfos[ currentDriveIndex ].m_MediaState == IRadDrive::MediaInfo::MediaPresent);
		if (unformatted)
			*unformatted = card_unformatted;
        
        if( !isReady 
			&& card_unformatted==false )
              // we need to keep track of unformatted drive so we can format it
		{
            // memory card must have been pulled out, reset current drive
            //
            m_currentDrive = NULL;
        }
    }

    return isReady;
}
 
int
MemoryCardManager::GetAvailableDrives( IRadDrive** pDrives,
                                       IRadDrive::MediaInfo** mediaInfos,
                                       IRadDrive **drive_mounted)
{
    int numAvailableDrives = 0;

    // query for all drives currently present
    //
    for( unsigned int i = 0; i < NUM_SAVE_GAME_DRIVES; i++ )
    {
        rAssert( m_pDrives[ i ] );

        if( m_mediaInfos[ i ].m_MediaState != IRadDrive::MediaInfo::MediaNotPresent )
        {
            if( pDrives != NULL )
            {
                pDrives[ numAvailableDrives ] = m_pDrives[ i ];
            }

            if( mediaInfos != NULL )
            {
                mediaInfos[ numAvailableDrives ] = &m_mediaInfos[ i ];
            }
            if (drive_mounted)
            {
                drive_mounted[i] = m_pDrives[ i ];
            }
            numAvailableDrives++;
        }
        else
        {
            if (drive_mounted)
            {
                drive_mounted[i] = NULL;
            }
        }

    }


    // terminate w/ NULL
    //
    if( pDrives != NULL )
    {
        pDrives[ numAvailableDrives ] = NULL;
    }

    return numAvailableDrives;
}

bool
MemoryCardManager::EnoughFreeSpace( unsigned int driveIndex ) const
{
    rAssert( driveIndex < NUM_SAVE_GAME_DRIVES );

    const unsigned int NUM_FREE_FILES_REQUIRED = 1;

    return( m_mediaInfos[ driveIndex ].m_FreeFiles >= NUM_FREE_FILES_REQUIRED &&
            m_mediaInfos[ driveIndex ].m_FreeSpace >= this->GetSavedGameCreationSize( driveIndex ) );
}

void MemoryCardManager::FormatDrive(unsigned int driveIndex, IMemoryCardFormatCallback *callback)
{ 
	SetCurrentDrive(driveIndex);
	m_formatDriveState = true;
	rAssert(callback);
	m_formatCallback = callback;
	m_currentDrive->FormatAsync();
	m_currentDrive->AddCompletionCallback( this , NULL);
}
void
MemoryCardManager::UpdateMemcardInfo( const char* savedGameTitle, int lineBreak )
{
    rAssert( m_memcardInfo != NULL );

#ifdef RAD_GAMECUBE
    rAssert( savedGameTitle != NULL );
    strncpy( m_memcardInfo->m_CommentLine2,
             savedGameTitle,
             MAX_SAVED_GAME_TITLE_LENGTH );
#endif

#ifdef RAD_PS2
    rAssert( savedGameTitle != NULL );
    rAssert( lineBreak != -1 );

    radSJISChar title[ MAX_SAVED_GAME_TITLE_LENGTH ];
    radAsciiToSjis( title, savedGameTitle );
    radSetIconSysTitle( &(m_memcardInfo->m_IconSys),
                        title,
                        static_cast<unsigned short>( lineBreak ) );
#endif

#ifdef RAD_XBOX
    // TC: stinky XBox! there's is no way to specify a descriptive name
    //     that's different from the file name
    //
    rWarningMsg( savedGameTitle == NULL, "Can't specify save game title on Xbox!" );
#endif
}

void
MemoryCardManager::OnDriveOperationsComplete( void* pUserData )
{ 
	if ( m_formatDriveState )
	{
		m_formatCallback->OnFormatOperationComplete(Success);
		m_formatDriveState = false;
	}
    else if( m_currentState == STATE_OPENING_DRIVES )
    {
        unsigned int driveIndex = reinterpret_cast<unsigned int>( pUserData );
        rAssert( driveIndex < NUM_SAVE_GAME_DRIVES );

        m_numDrivesOpened++;

        if( m_numDrivesOpened == NUM_SAVE_GAME_DRIVES )
        {
            // done opening all save game drives
            //
            m_currentState = STATE_READY;
        }
    }
    else // checking memory cards during bootup
    {
        rAssert( m_currentMediaInfo == m_nextMediaInfo );
        m_nextMediaInfo = (m_currentMediaInfo + 1) % NUM_SAVE_GAME_DRIVES;

        if( m_memcardCheckingState == MEMCARD_CHECK_IN_PROGRESS )
        {
            if( m_nextMediaInfo == 0 )
            {
#if defined( RAD_GAMECUBE) || defined( RAD_PS2 )
                // continue to poll for media info from all drives
                // during the entire minimum checking time
                //
                if( m_elapsedMemcardCheckTime > MINIMUM_MEMCARD_CHECK_TIME )
#endif
                {
                    // ok, we finished querying media info from all drives
                    //
                    m_memcardCheckingState = MEMCARD_CHECK_COMPLETED;
                }
            }
        }
    }
}

bool 
MemoryCardManager::HasSaveGame(unsigned int driveIndex)
{
    rAssert( driveIndex < NUM_SAVE_GAME_DRIVES );
    return  GetGameDataManager()->DoesSaveGameExist( m_pDrives[ driveIndex ], false );
}

bool
MemoryCardManager::OnDriveError( radFileError error,
                                 const char* pDriveName,
                                 void* pUserData )
{

    rDebugPrintf( "*** MEMCARD: ERROR [%d] occurred on drive [%s]! ***\n",
                  error,
                  pDriveName );

	if ( m_formatDriveState )
	{
		m_formatCallback->OnFormatOperationComplete(error);
	}
	else if( m_radDriveErrorCallback != NULL )
	{
		m_radDriveErrorCallback->OnDriveError( error, pDriveName, pUserData );
	}

    // we should always return false, since we're always going to prompt
    // the user first before attempting to retry any operation
    //
    return false;
}

int
MemoryCardManager::GetDriveIndex( IRadDrive* pDrive ) const
{
    int driveIndex = -1;

    if( pDrive != NULL )
    {
        for( unsigned int i = 0; i < NUM_SAVE_GAME_DRIVES; i++ )
        {
            if( strcmp( pDrive->GetDriveName(), SAVE_GAME_DRIVE[ i ] ) == 0 )
            {
                driveIndex = static_cast<int>( i );

                break;
            }
        }
    }

    return driveIndex;
}

//===========================================================================
// Private Member Functions
//===========================================================================

void
MemoryCardManager::DetermineSavedGameCreationSize( unsigned int driveIndex )
{
#ifdef RAD_XBOX
    if( driveIndex == 0 ) // Xbox hard disk
    {
        rAssert( driveIndex < NUM_SAVE_GAME_DRIVES );
        m_savedGameCreationSizeHD = m_pDrives[ driveIndex ]->GetCreationSize( m_memcardInfo, GetGameDataManager()->GetGameDataSize() );

        rReleasePrintf( "The Simpsons Hit & Run Saved Game File Size (on HD) = %.2f KB (%d bytes)\n",
                        m_savedGameCreationSizeHD / 1024.0f, m_savedGameCreationSizeHD );
    }
    else
#endif
    {
        rAssert( driveIndex < NUM_SAVE_GAME_DRIVES );
        m_savedGameCreationSize = m_pDrives[ driveIndex ]->GetCreationSize( m_memcardInfo, GetGameDataManager()->GetGameDataSize() );

        rReleasePrintf( "The Simpsons Hit & Run Saved Game File Size = %.2f KB (%d bytes)\n",
                        m_savedGameCreationSize / 1024.0f, m_savedGameCreationSize );
    }
}

void
MemoryCardManager::OnMemoryCardCheckCompleted()
{
    radFileError errorCode = Success;
    IRadDrive::MediaInfo::MediaState mediaState = IRadDrive::MediaInfo::MediaPresent;
    int driveIndex = -1;

    // search for a good memory card w/ enough free space to save games
    // 
    bool goodCardExists = false;
    bool fullCardExists = false;

    // and, at the same time, search for most recent save game among
    // all drives
    // 
    int mostRecentSaveGameDriveIndex = -1;
    int mostRecentSaveGameSlot = -1;
    radDate mostRecentTimeStamp;
    mostRecentTimeStamp.m_Year = 0;

    for( unsigned int i = 0; i < NUM_SAVE_GAME_DRIVES; i++ )
    {
        // determine saved game creation size
        //
        this->DetermineSavedGameCreationSize( i );

        if( m_mediaInfos[ i ].m_MediaState == IRadDrive::MediaInfo::MediaPresent )
        {
            unsigned int slot = 0;
            radDate timeStamp;
            bool saveGameExists = GetGameDataManager()->FindMostRecentSaveGame( m_pDrives[ i ],
                                                                                slot,
                                                                                timeStamp );
            if( saveGameExists )
            {
                if( SaveGameInfo::CompareTimeStamps( timeStamp, mostRecentTimeStamp ) > 0 )
                {
                    mostRecentSaveGameDriveIndex = static_cast<int>( i );
                    mostRecentSaveGameSlot = static_cast<int>( slot );
                    memcpy( &mostRecentTimeStamp, &timeStamp, sizeof( radDate ) );
                }
            }

            if( this->EnoughFreeSpace( i ) )
            {
                goodCardExists = true; // we found good card, continue looping to find the latest save game
            }
            else
            {
#ifdef RAD_GAMECUBE
                // Nintendo TRC: if there is a full memory card, we need to check later to see if
                // all game slots are taken up; if not, then we must display a "full memory card" error message
                //
                fullCardExists = true;
#endif
                if( saveGameExists )
                {
                    goodCardExists = true; // we found good card, continue looping to find the latest save game
                }
            }
        }
    }

#ifdef RAD_XBOX
    // only need to check xbox hard disk, no need to check memory units
    //
    rAssertMsg( m_mediaInfos[ 0 ].m_MediaState == IRadDrive::MediaInfo::MediaPresent,
                "ERROR: *** WTF? The Xbox Hard Disk is not present??" );

    if( !GetGameDataManager()->DoesSaveGameExist( m_pDrives[ 0 ] ) &&
        !this->EnoughFreeSpace( 0 ) )
    {
        driveIndex = 0;
        errorCode = NoFreeSpace;
    }
#endif // RAD_XBOX

#ifdef RAD_WIN32
    rAssertMsg( m_mediaInfos[ 0 ].m_MediaState == IRadDrive::MediaInfo::MediaPresent,
                "ERROR: Default hard drive didn't mount." );
#endif // RAD_WIN32

#if defined( RAD_GAMECUBE ) || defined( RAD_PS2 )
    if( !goodCardExists || fullCardExists )
    {
        // no good card exists; now search for first card w/ an error
        //
        bool errorExists = false;
        for( unsigned int i = 0; i < NUM_SAVE_GAME_DRIVES; i++ )
        {
            if( m_mediaInfos[ i ].m_MediaState == IRadDrive::MediaInfo::MediaPresent )
            {
#ifdef RAD_GAMECUBE
                bool saveGameExists = GetGameDataManager()->DoesSaveGameExist( m_pDrives[ i ], true, true );
#else
                bool saveGameExists = GetGameDataManager()->DoesSaveGameExist( m_pDrives[ i ] );
#endif
                if( !saveGameExists && !this->EnoughFreeSpace( i ) )
                {
                    errorExists = true;

                    errorCode = NoFreeSpace;
                    driveIndex = static_cast<int>( i );

                    break;
                }
            }
            else if( m_mediaInfos[ i ].m_MediaState != IRadDrive::MediaInfo::MediaNotPresent )
            {
                errorExists = true;

                errorCode = m_pDrives[ i ]->GetLastError();
                mediaState = m_mediaInfos[ i ].m_MediaState;
                driveIndex = static_cast<int>( i );

                break;
            }
        }

        if( !errorExists && !fullCardExists )
        {
            // hmmm... no error exists, that means there are no memory cards attached
            //
            mediaState = IRadDrive::MediaInfo::MediaNotPresent;
            driveIndex = -1;

#ifdef RAD_DEBUG
            // let's double check that there are, in fact, no memory cards
            //
            for( unsigned int i = 0; i < NUM_SAVE_GAME_DRIVES; i++ )
            {
                rAssert( m_mediaInfos[ i ].m_MediaState == IRadDrive::MediaInfo::MediaNotPresent );
            }
#endif
        }
    }
#endif // RAD_GAMECUBE || RAD_PS2

    if( m_memcardCheckCallback != NULL )
    {
        m_memcardCheckCallback->OnMemoryCardCheckDone( errorCode,
                                                       mediaState,
                                                       driveIndex,
                                                       mostRecentSaveGameDriveIndex,
                                                       mostRecentSaveGameSlot );
        m_memcardCheckCallback = NULL;
    }
}


#ifdef RAD_GAMECUBE
    //
    // I have no idea what this does ...
    //
    void
    MemoryCardManager::UnpackTexPalette( TEXPalettePtr pal )
    {
        u16 i;
    
        rAssertMsg( pal->versionNumber == 2142000, "Invalid version number for texture palette" );
    
        pal->descriptorArray = (TEXDescriptorPtr)(((u32)(pal->descriptorArray)) + ((u32)pal));

        //
        // Go through each of the palette descriptors
        //
        for ( i = 0; i < pal->numDescriptors; i++ )
        {
            if( pal->descriptorArray[ i ].textureHeader )
            {
                //
                // Fill in the texture header
                //
                pal->descriptorArray[ i ].textureHeader =
                    reinterpret_cast< TEXHeaderPtr >
                    (
                        ( (u32) pal->descriptorArray[i].textureHeader ) +
                        ( (u32) pal )
                    );
            
                //
                // If it is not unpacked, unpack it
                //
                if
                (
                    !( pal->descriptorArray[i].textureHeader->unpacked )
                )
                {
                    pal->descriptorArray[i].textureHeader->data =
                        reinterpret_cast< Ptr >
                        (
                            ( (u32) pal->descriptorArray[ i ].textureHeader->data ) +
                            ( (u32) pal )
                        );
                    pal->descriptorArray[i].textureHeader->unpacked = 1;
                }
            }

            if(pal->descriptorArray[i].CLUTHeader)
            {
                pal->descriptorArray[i].CLUTHeader = (CLUTHeaderPtr)((u32)(pal->descriptorArray[i].CLUTHeader) + (u32)pal);     
            
                if(!(pal->descriptorArray[i].CLUTHeader->unpacked))
                {
                    pal->descriptorArray[i].CLUTHeader->data = (Ptr)((u32)(pal->descriptorArray[i].CLUTHeader->data) + (u32)pal);
                    pal->descriptorArray[i].CLUTHeader->unpacked = 1;
                }
            }
        }
    }

    void
    MemoryCardManager::LoadMemcardInfo_GC( GameMemoryAllocator heap )
    {
        m_dvdBanner = NULL;
        m_texPalette = NULL;

        GetLoadingManager()->AddRequest( FILEHANDLER_ICON,
                                         GC_BANNER_FILE,
                                         heap,
                                         this );

        GetLoadingManager()->AddRequest( FILEHANDLER_ICON,
                                         GC_TPL_FILE,
                                         heap,
                                         this );

        m_memcardInfoLoadState = MEMCARD_INFO_LOADING_BANNER;
    }

    void
    MemoryCardManager::UnloadMemcardInfo_GC()
    {
        if( m_dvdBanner != NULL )
        {
            delete m_dvdBanner;
            m_dvdBanner = NULL;
        }

        if( m_texPalette != NULL )
        {
            delete [] m_texPalette;
            m_texPalette = NULL;
        }
    }
#endif // RAD_GAMECUBE

#ifdef RAD_PS2
    void
    MemoryCardManager::LoadMemcardInfo_PS2( GameMemoryAllocator heap )
    {
        // make a header structure
        //
        radSJISChar title[ MAX_SAVED_GAME_TITLE_LENGTH ];
        radAsciiToSjis( title, "" );
        rAssert( m_memcardInfo != NULL );
        radMakeIconSys( &(m_memcardInfo->m_IconSys), title, 0 );

        m_memcardInfo->m_ListIcon = NULL;
        m_memcardInfo->m_CopyIcon = NULL;
        m_memcardInfo->m_DelIcon = NULL;

        GetLoadingManager()->AddRequest( FILEHANDLER_ICON,
                                         PS2_LIST_ICON_FILE,
                                         heap,
                                         this );

#ifndef USE_ONE_ICON_FILE_ONLY
        GetLoadingManager()->AddRequest( FILEHANDLER_ICON,
                                         PS2_COPY_ICON_FILE,
                                         heap,
                                         this );

        GetLoadingManager()->AddRequest( FILEHANDLER_ICON,
                                         PS2_DELETE_ICON_FILE,
                                         heap,
                                         this );
#endif

        m_memcardInfoLoadState = MEMCARD_INFO_LOADING_ICON_LIST;
    }

    void
    MemoryCardManager::UnloadMemcardInfo_PS2()
    {
        if( m_memcardInfo != NULL )
        {
            if( m_memcardInfo->m_ListIcon != NULL )
            {
                delete [] reinterpret_cast<char*>( m_memcardInfo->m_ListIcon );
                m_memcardInfo->m_ListIcon = NULL;
            }

#ifdef USE_ONE_ICON_FILE_ONLY
			m_memcardInfo->m_CopyIcon = NULL;
			m_memcardInfo->m_DelIcon = NULL;
#else
            if( m_memcardInfo->m_CopyIcon != NULL )
            {
                delete [] reinterpret_cast<char*>( m_memcardInfo->m_CopyIcon );
                m_memcardInfo->m_CopyIcon = NULL;
            }

            if( m_memcardInfo->m_DelIcon != NULL )
            {
                delete [] reinterpret_cast<char*>( m_memcardInfo->m_DelIcon );
                m_memcardInfo->m_DelIcon = NULL;
            }
#endif
        }
    }
#endif // RAD_PS2

#ifdef RAD_XBOX
    void
    MemoryCardManager::LoadMemcardInfo_XBOX( GameMemoryAllocator heap )
    {
        rAssert( m_memcardInfo != NULL );
        m_memcardInfo->m_Icon = NULL;

        m_memcardInfoLoadState = MEMCARD_INFO_LOADING_ICON;

#ifdef USE_DEFAULT_ICONS
        this->OnProcessRequestsComplete( NULL );
#else
        GetLoadingManager()->AddRequest( FILEHANDLER_ICON,
                                         XBOX_ICON_FILE,
                                         heap,
                                         this );
#endif
    }

    void
    MemoryCardManager::UnloadMemcardInfo_XBOX()
    {
        if( m_memcardInfo != NULL )
        {
            if( m_memcardInfo->m_Icon != NULL )
            {
                delete [] reinterpret_cast<char*>( m_memcardInfo->m_Icon );
                m_memcardInfo->m_Icon = NULL;
            }
        }
    }
#endif // RAD_XBOX

