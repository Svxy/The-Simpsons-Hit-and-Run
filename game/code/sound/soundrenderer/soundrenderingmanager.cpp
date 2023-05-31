//=============================================================================
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundrenderingmanager.cpp
//
// Subsystem:   Dark Angel - Sound Manager System
//
// Description: Implementation of the sound manager
//
// Revisions:
//  + Created October 2, 2001 -- breimer
//
//=============================================================================

//=============================================================================
// Included Files
//=============================================================================

#include <radobject.hpp>
#include <raddebug.hpp>
#include <radfile.hpp>


#include <radmusic/radmusic.hpp>
#include <radsound.hpp>

#include <radobjectlist.hpp>
#include <radscript.hpp>
#include <radtypeinfo.hpp>
#include <radtypeinfoutil.hpp>
#include <radfactory.hpp>

#include <memory/srrmemory.h>
#include <loading/loadingmanager.h>
#include <loading/soundfilehandler.h>

#include <sound/soundrenderer/idasoundtuner.h>
#include <sound/soundrenderer/soundresource.h>
#include <sound/soundrenderer/soundresourcemanager.h>
#include <sound/soundrenderer/playermanager.h>

#include <sound/soundrenderer/soundsystem.h>
#include <sound/soundrenderer/soundrenderingmanager.h>
#include <sound/soundrenderer/soundrenderingmanager.h>
#include <sound/soundrenderer/soundallocatedresource.h>
#include <sound/soundrenderer/soundconstants.h>
#include <sound/soundclusternameenum.h>
#include <sound/soundmanager.h>
#include <sound/soundrenderer/soundnucleus.hpp>
#include <sound/dialog/dialogcoordinator.h>
#include <radobjectbtree.hpp>
#include <radmemorymonitor.hpp>

#include <pddi/pddi.hpp>
#include <p3d/utility.hpp>
#include <p3d/p3dtypes.hpp>

//=============================================================================
// Namespace
//=============================================================================

//#define AUDIO_FILE_PERFORMANCE_LOG
//#define AUDIO_FILE_PERFORMANCE_SPEW

bool gDaSoundStats = false;
bool gTuneSound = false;

namespace Sound {

const short MAX_BTREE_NODES = 5100;

radObjectBTreeNode* gpBTreeNodePool = 0;
    
//=============================================================================
// Debug Information
//=============================================================================

//
// Use this if you want to debug the sound tuner
//
#ifndef FINAL
#ifndef NDEBUG

// This is the type of debug information to output
#define DASOUNDMANAGER_OUTPUTDEBUGINFO
#ifdef DASOUNDMANAGER_OUTPUTDEBUGINFO
//static DebugInfoType sg_DebugInfoType = DEBUG_RESOURCEINFO;
#endif //DASOUNDMANAGER_OUTPUTDEBUGINFO

#endif //NDEBUG
#endif //FINAL


//=============================================================================
// Macros and Defines
//=============================================================================

//
// THe sound memory size
//

//
// Some directories
//
#define SOUND_ROOT_DIR "sound"
#define SCRIPT_DIR SOUND_ROOT_DIR"\\scripts"
#define TYPEINFO_DIR SOUND_ROOT_DIR"\\typ"

unsigned int gTotalMicrosecondsWastedParsingScripts = 0;

//=============================================================================
// Constants
//=============================================================================

//
// Custom memory management should not be handled by the sound system!
//
radMemoryAllocator DAMEMORY_ALLOC_SOUND = RADMEMORY_ALLOC_DEFAULT;

//
// The object list page size for the sound objects
//
static const unsigned int SoundObjectsListPageSize = 32;

//
// Flag used for file callback
//
static const bool LastScriptTrue = true;
static const bool LastScriptFalse = false;

//=============================================================================
// Static Variables
//=============================================================================

//
// The sound manager singleton class
//
daSoundRenderingManager* daSoundRenderingManager::s_Singleton = NULL;


//=============================================================================
// Local data
//=============================================================================

//
// The name of our sound resource namespace
//
static const char s_SoundNamespace[ ] = "SoundGlobals";

//
// The name of our tuning object namespace
//
static const char s_TuningNamespace[] = "TuningGlobals";

//
// Base name for our character namespaces
//
static const char s_CharacterNamespace[] = "CharSounds";

//
// The name of the listener object
//
//static const char s_TheListenerName[ ] = "TheListener";

//
// Dialog cement file names
//
#if defined( RAD_XBOX )

static const unsigned int s_NumDialogCementFiles = 3;

static const char s_EnglishDialogue[] = "dialog0?.rcf";
static const char s_FrenchDialogue[] = "dialogf0?.rcf";
static const char s_GermanDialogue[] = "dialogg0?.rcf";
static const char s_SpanishDialogue[] = "dialogs0?.rcf";
static const char s_ItalianDialogue[] = "dialogi0?.rcf";

#else

static const unsigned int s_NumDialogCementFiles = 1;

static const char s_EnglishDialogue[] = "dialog.rcf";
static const char s_FrenchDialogue[] = "dialogf.rcf";
static const char s_GermanDialogue[] = "dialogg.rcf";
static const char s_SpanishDialogue[] = "dialogs.rcf";
static const char s_ItalianDialogue[] = "dialogi.rcf";

#endif

//
// Cement libraries associated with the Dark Angel sound system
//
struct DaSoundCementLibraryData
{
    const char*     m_LibraryIDString;
    const char*     m_Filename;
    unsigned int    m_CacheSize;
};

//
// Type info
//
struct DaSoundTypeinfoData
{
    const char*     m_Filename;
};
static DaSoundTypeinfoData s_DaSoundTypeInfo =
{
    TYPEINFO_DIR"\\srrtypes.typ"
};

//
// Radscripts associated wtih the DA sound system
//
struct DaSoundScriptData
{
    const char*     m_Filename;
};

//
// IMPORTANT CHANGE: The last script files listed will have its contents go
// into the tuning namespace, not the sound namespace.  This is to ensure
// that everything in the sound namespace is a soundResourceData object
// to allow me to search for dialog resources without an illegal downcast
// (stinky downcasts, hate 'em).
//
static DaSoundScriptData s_DaSoundScripts[ ] =
{
    // Character sound scripts
    { "\\Apu.spt" },
    { "\\Bart.spt" },
    { "\\Homer.spt" },
    { "\\Lisa.spt" },
    { "\\Marge.spt" },

    // Level scripts
    { "\\suburbs.spt" },
    { "\\downtown.spt" },
    { "\\seaside.spt" },
    { "\\level1.spt" },
    { "\\level2.spt" },
    { "\\level3.spt" },
    { "\\level4.spt" },
    { "\\level5.spt" },
    { "\\level6.spt" },
    { "\\level7.spt" },
    { "\\minigame.spt" },

    // Sound effect resources
    { "\\frontend.spt" },
    { "\\collide.spt" },
    { "\\carsound.spt" },
    { "\\World.spt" },
    { "\\positionalSounds.spt" },
    { "\\interactive_props.spt" },
   
    // Dialog
    { "\\dialog.spt" },
    { "\\nis.spt" },

    // Cars
    { "\\bart_v.spt" },
    { "\\apu_v.spt" },
    { "\\snake_v.spt" },
    { "\\homer_v.spt" },
    { "\\famil_v.spt" },
    { "\\gramp_v.spt" },
    { "\\cletu_v.spt" },
    { "\\wiggu_v.spt" },
    { "\\empty.spt" },
    { "\\marge_v.spt" },
    { "\\empty.spt" },
    { "\\empty.spt" },
    { "\\smith_v.spt" },
    { "\\empty.spt" },
    { "\\empty.spt" },
    { "\\empty.spt" },
    { "\\zombi_v.spt" },
    { "\\empty.spt" },
    { "\\empty.spt" },
    { "\\cVan.spt" },
    { "\\compactA.spt" },
    { "\\comic_v.spt" },
    { "\\skinn_v.spt" },
    { "\\cCola.spt" },
    { "\\cSedan.spt" },
    { "\\cPolice.spt" },
    { "\\cCellA.spt" },
    { "\\cCellB.spt" },
    { "\\cCellC.spt" },
    { "\\cCellD.spt" },
    { "\\minivanA_v.spt" },
    { "\\pickupA.spt" },
    { "\\taxiA_v.spt" },
    { "\\sportsA.spt" },
    { "\\sportsB.spt" },
    { "\\SUVA.spt" },
    { "\\wagonA.spt" },
    { "\\hbike_v.spt" },
    { "\\burns_v.spt" },
    { "\\honor_v.spt" },
    { "\\cArmor.spt" },
    { "\\cCurator.spt" },
    { "\\cHears.spt" },
    { "\\cKlimo.spt" },
    { "\\cLimo.spt" },
    { "\\cNerd.spt" },
    { "\\frink_v.spt" },
    { "\\cMilk.spt" },
    { "\\cDonut.spt" },
    { "\\bbman_v.spt" },
    { "\\bookb_v.spt" },
    { "\\carhom_v.spt" },
    { "\\elect_v.spt" },
    { "\\fone_v.spt" },
    { "\\gramR_v.spt" },
    { "\\moe_v.spt" },
    { "\\mrplo_v.spt" },
    { "\\otto_v.spt" },
    { "\\plowk_v.spt" },
    { "\\scorp_v.spt" },
    { "\\willi_v.spt" },
    { "\\sedanA.spt" },
    { "\\sedanB.spt" },
    { "\\cBlbart.spt" },
    { "\\cCube.spt" },
    { "\\cDuff.spt" },
    { "\\cNonup.spt" },
    { "\\lisa_v.spt" },
    { "\\krust_v.spt" },
    { "\\coffin.spt" },
    { "\\hallo.spt" },
    { "\\ship.spt" },
    { "\\witchcar.spt" },
    { "\\huska.spt" },
    { "\\atv_v.spt" },
    { "\\dune_v.spt" },
    { "\\hype_v.spt" },
    { "\\knigh_v.spt" },
    { "\\mono_v.spt" },
    { "\\oblit_v.spt" },
    { "\\rocke_v.spt" },
    { "\\ambul.spt" },
    { "\\burnsarm.spt" },
    { "\\fishtruc.spt" },
    { "\\garbage.spt" },
    { "\\icecream.spt" },
    { "\\istruck.spt" },
    { "\\nuctruck.spt" },
    { "\\pizza.spt" },
    { "\\schoolbu.spt" },
    { "\\votetruc.spt" },
    { "\\glastruc.spt" },
    { "\\cfire_v.spt" },
    { "\\cBone.spt" },
    { "\\redbrick.spt" },

    // Tuning
    { "\\car_tune.spt" },
    { "\\positionalSettings.spt" },
    { "\\global.spt" }
};

const unsigned int NumSoundScripts = sizeof( s_DaSoundScripts ) /
                                     sizeof( DaSoundScriptData );

const unsigned int NUM_TUNING_SCRIPTS = 3;
const unsigned int NUM_CHARACTER_SCRIPTS = 5;
const unsigned int INTERACTIVE_PROPS_SCRIPT_POSITION = 21;
const unsigned int DIALOGUE_SCRIPT_POSITION = 22;
const unsigned int NIS_SCRIPT_POSITION = 23;

//
// Array mapping scripts to sound clusters that their sounds should
// be loaded/unloaded with.
//
// IMPORTANT: this array needs to be maintained to match the scripts 
// in s_DaSoundScripts up to the first car script.  After that, we
// do it programatically (I don't think that's actually a word).
//
static SoundClusterName s_ScriptClusters[] =
{
    SC_CHAR_APU,
    SC_CHAR_BART,
    SC_CHAR_HOMER,
    SC_CHAR_LISA,
    SC_CHAR_MARGE,
    SC_LEVEL_SUBURBS,
    SC_LEVEL_DOWNTOWN,
    SC_LEVEL_SEASIDE,
    SC_LEVEL1,
    SC_LEVEL2,
    SC_LEVEL3,
    SC_LEVEL4,
    SC_LEVEL5,
    SC_LEVEL6,
    SC_LEVEL7,
    SC_MINIGAME,
    SC_ALWAYS_LOADED,
    SC_ALWAYS_LOADED,
    SC_ALWAYS_LOADED,
    SC_ALWAYS_LOADED,
    SC_INGAME,
    SC_INGAME,
    SC_NEVER_LOADED,
    SC_NEVER_LOADED,
};
const unsigned int NumClusterNames = sizeof( s_ScriptClusters ) /
                                     sizeof( SoundClusterName );


//=============================================================================
// Class Implementations
//=============================================================================

//=============================================================================
// daSoundRenderingManager Implementation
//=============================================================================

//=============================================================================
// Function:    daSoundRenderingManager::daSoundRenderingManager
//=============================================================================
// Description: Constructor
//
//-----------------------------------------------------------------------------

daSoundRenderingManager::daSoundRenderingManager( )
    :
    radRefCount( 0 ),
    m_pScript( NULL ),
    m_IsInitialized( false ),
    m_pResourceNameSpace( NULL ),
    m_pTuningNameSpace( NULL ),
    m_pDynaLoadManager( NULL ),
    m_pTuner( NULL ),
    m_pResourceManager( NULL ),
    m_pPlayerManager( NULL ),
    m_scriptLoadCount( 0 ),
    m_currentLanguage( DIALOGUE_LANGUAGE_ENGLISH ),
    m_languageSelected( false )
{
    unsigned int i;

    // Create the singleton
    rAssert( s_Singleton == NULL );
    s_Singleton = this;

    for( i = 0; i < NUM_CHARACTER_NAMESPACES; i++ )
    {
        m_pCharacterNameSpace[i] = NULL;
    }

    for( i = 0; i < NUM_SOUND_CEMENT_FILES; i++ )
    {
        m_soundCementFileHandles[i] = 0;
    }
    
    m_LastPerformanceEventTime = radTimeGetMilliseconds( );
    ::RadSoundSetFilePerformanceCallback( & daSoundRenderingManager::FilePerformanceEvent );
        
    radDbgWatchAddBoolean(
        & gDaSoundStats,
        "DaSound Stats",
        "Sound" );          
}

//=============================================================================
// Function:    daSoundRenderingManager::~daSoundRenderingManager
//=============================================================================
// Description: Destructor
//
//-----------------------------------------------------------------------------

daSoundRenderingManager::~daSoundRenderingManager( )
{
  
    radDbgWatchDelete( & gDaSoundStats );
    ::RadSoundSetFilePerformanceCallback( NULL );    
    
    rAssert( !m_IsInitialized );

    // Clear the singleton
    s_Singleton = NULL;
}

//=============================================================================
// Function:    daSoundRenderingManager::GetInstance
//=============================================================================
// Description: Get the singleton instance of this class
//
//-----------------------------------------------------------------------------

daSoundRenderingManager* daSoundRenderingManager::GetInstance( void )
{
    return s_Singleton;
}

//=============================================================================
// Function:    daSoundRenderingManager::Initialize
//=============================================================================
// Description: Initialize the sound manager with the stuff that we don't
//              need to do if we're muted.
//
//-----------------------------------------------------------------------------

void daSoundRenderingManager::Initialize
(
    void
)
{
    unsigned int i;
    char nameBuffer[50];
    int nameLength;

    //
    // Create namespaces
    //
    m_pResourceNameSpace = ::radNameSpaceCreate( GetThisAllocator( ) );
    m_pResourceNameSpace->AddRef( );
    m_pResourceNameSpace->SetName( s_SoundNamespace );

    m_pTuningNameSpace = ::radNameSpaceCreate( GetThisAllocator( ) );
    m_pTuningNameSpace->AddRef( );
    m_pTuningNameSpace->SetName( s_TuningNamespace );

    strcpy( nameBuffer, s_CharacterNamespace );
    nameLength = strlen( s_CharacterNamespace );
    for( i = 0; i < NUM_CHARACTER_NAMESPACES; i++ )
    {
        m_pCharacterNameSpace[i] = ::radNameSpaceCreate( GetThisAllocator() );
        rAssert( m_pCharacterNameSpace[i] != NULL );

        m_pCharacterNameSpace[i]->AddRef();

        //
        // Create a unique name
        //
        nameBuffer[nameLength] = static_cast<char>( '0' + i );
        nameBuffer[nameLength+1] = '\0';
        m_pCharacterNameSpace[i]->SetName( nameBuffer );
    }

    //
    // Spawn other elements of the sound system (some of these depend on the namespace)
    //
    m_pDynaLoadManager = new ( GetThisAllocator( ) ) daSoundDynaLoadManager( );
    m_pDynaLoadManager->AddRef( );
    m_pResourceManager = new ( GetThisAllocator( ) ) daSoundResourceManager( );
    m_pResourceManager->AddRef( );
    m_pPlayerManager = new ( GetThisAllocator( ) ) daSoundPlayerManager( );
    m_pPlayerManager->AddRef( );
    
    Sound::daSoundTunerCreate					( &m_pTuner, GetThisAllocator( ) );

    m_pPlayerManager->UglyHackPostInitialize( m_pTuner );

    //
    // Register some factories and some objects.  Some of these object use
    // the spawned systems.
    //
    ::radFactoryRegister(
        "daSoundResourceData",
        (radFactoryProc*) daSoundResourceManager::CreateResourceData );
    rAssert( GetSoundNamespace( ) != NULL );
    rAssert( GetTuningNamespace() != NULL );
}

//=============================================================================
// Function:    daSoundRenderingManager::IsInitialized
//=============================================================================
// Description: Use this to pull for the end of initialization
//
//-----------------------------------------------------------------------------

bool daSoundRenderingManager::IsInitialized( void )
{
    return m_IsInitialized;
}

//=============================================================================
// Function:    daSoundRenderingManager::Terminate
//=============================================================================
// Description: Terminate the sound manager
//
//-----------------------------------------------------------------------------

void daSoundRenderingManager::Terminate( void )
{
    unsigned int i;

    // Stop all the sounds in the game
    GetPlayerManager( )->CancelPlayers( );

    // Release the namespaces, must do this before killing resource manager
    // some of the reference counting is not per-object.
    
    m_pResourceNameSpace->Release( );
    m_pTuningNameSpace->Release();

    for( i = 0; i < NUM_CHARACTER_NAMESPACES; i++ )
    {
        m_pCharacterNameSpace[i]->Release();
    }
        
    // Destroy the sound systems created by the initalize
    if( IsInitialized( ) )
    {
        //
        // Detach the various systems spawned on creation
        //
        if( m_pPlayerManager != NULL )
        {
            m_pPlayerManager->Release( );
            m_pPlayerManager = NULL;
        }
        if( m_pResourceManager != NULL )
        {
            m_pResourceManager->Release( );
            m_pResourceManager = NULL;
        }
        if( m_pTuner != NULL )
        {
            m_pTuner->Release( );
            m_pTuner = NULL;
        }
        if( m_pDynaLoadManager != NULL )
        {
            m_pDynaLoadManager->Release( );
            m_pDynaLoadManager = NULL;
        }

#ifndef FINAL
        if ( gTuneSound )
        {
            ::radRemoteScriptTerminate( );
        }
#endif
        rAssert( m_pScript == NULL );
        ::radScriptTerminate( );

        //
        // Release the cement file
        //
        for( i = 0; i < NUM_SOUND_CEMENT_FILES; i++ )
        {
            GetLoadingManager()->UnregisterCementLibrary( m_soundCementFileHandles[i] );
        }

        // Uninitialize
        m_IsInitialized = false;
    }
}

//=============================================================================
// Function:    daSoundRenderingManager::Service
//=============================================================================
// Description: Service the Dark Angel sound system.  This call should be made
//              as often as possible.
//
// Parameters:  none
//
// Returns:     n/a
//
//-----------------------------------------------------------------------------

void daSoundRenderingManager::Service( void )
{
    //
    // Service the radsound system
    //
    ::radSoundHalSystemGet( )->Service( );
    SoundNucleusServiceClipLoad( );
}

//=============================================================================
// Function:    daSoundRenderingManager::ServiceOncePerFrame
//=============================================================================
// Description: Service the Dark Angel sound system.  This call should be made
//              no more than once per frame or else performance may suffer.
//
// Parameters:  none
//
// Returns:     n/a
//
//-----------------------------------------------------------------------------

void daSoundRenderingManager::ServiceOncePerFrame( unsigned int elapsedTime )
{
    //
    // Service the radsound system
    //
    ::radSoundHalSystemGet( )->ServiceOncePerFrame( );

    //
    // Do nothing else until we're fully initialized
    //
    if( !IsInitialized( ) )
    {
        return;
    }

    // Service the dynamic loading system
    if( GetDynaLoadManager( ) != NULL )
    {
        GetDynaLoadManager( )->ServiceOncePerFrame( );
    }

    // Service the tuner
    if( GetTuner( ) != NULL )
    {
        GetTuner( )->ServiceOncePerFrame( elapsedTime );
    }

    // Service the player manager
    if( GetPlayerManager( ) != NULL )
    {
        GetPlayerManager( )->ServiceOncePerFrame( );
    }
}

//=============================================================================
// daSoundRenderingManager::QueueCementFileRegistration
//=============================================================================
// Description: Set up the cement file registration with the loading manager
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void daSoundRenderingManager::QueueCementFileRegistration()
{
    HeapMgr()->PushHeap (GMA_AUDIO_PERSISTENT);

    //
    // Queue requests for sound cement file registration
    //
    int i = s_NumDialogCementFiles;

    if( !m_languageSelected )
    {
        registerDialogueCementFiles( s_EnglishDialogue );
        m_languageSelected = true;
    }

#ifdef RAD_XBOX
    //
    // Register the music rcfs -- no localization needed.
    //
    m_soundCementFileHandles[i++] = GetLoadingManager()->RegisterCementLibrary( "music00.rcf" );
    m_soundCementFileHandles[i++] = GetLoadingManager()->RegisterCementLibrary( "music01.rcf" );
    m_soundCementFileHandles[i++] = GetLoadingManager()->RegisterCementLibrary( "music02.rcf" );
    m_soundCementFileHandles[i++] = GetLoadingManager()->RegisterCementLibrary( "music03.rcf" );
#else
    m_soundCementFileHandles[i++] = GetLoadingManager()->RegisterCementLibrary( "music.rcf" );
#endif

    m_soundCementFileHandles[i++] = GetLoadingManager()->RegisterCementLibrary( "carsound.rcf" );
    m_soundCementFileHandles[i++] = GetLoadingManager()->RegisterCementLibrary( "ambience.rcf" );
    m_soundCementFileHandles[i++] = GetLoadingManager()->RegisterCementLibrary( "nis.rcf" );
    m_soundCementFileHandles[i++] = GetLoadingManager()->RegisterCementLibrary( "soundfx.rcf" );
    m_soundCementFileHandles[i++] = GetLoadingManager()->RegisterCementLibrary( "scripts.rcf" );

    HeapMgr()->PopHeap (GMA_AUDIO_PERSISTENT);
}

//=============================================================================
// daSoundRenderingManager::SwitchDialogueCementFile
//=============================================================================
// Description: Switch the current language to the given language.
//
// Parameters:  language - new language to use
//
// Return:      void 
//
//=============================================================================
void daSoundRenderingManager::SetLanguage( Scrooby::XLLanguage language )
{
    unsigned int i;
    const char* cementFileName = s_EnglishDialogue;
    DialogueLanguage oldLanguage = m_currentLanguage;

    switch( language )
    {
        case Scrooby::XL_ENGLISH:
            cementFileName = s_EnglishDialogue;
            m_currentLanguage = DIALOGUE_LANGUAGE_ENGLISH;
            break;

        //**************************************************************************
        // TEMPORARY: all dialog is English until we actually get localized dialogue
        //**************************************************************************
        case Scrooby::XL_FRENCH:
            cementFileName = s_FrenchDialogue;
            m_currentLanguage = DIALOGUE_LANGUAGE_FRENCH;
            break;

        case Scrooby::XL_GERMAN:
            cementFileName = s_GermanDialogue;
            m_currentLanguage = DIALOGUE_LANGUAGE_GERMAN;
            break;

        case Scrooby::XL_SPANISH:
            cementFileName = s_SpanishDialogue;
            m_currentLanguage = DIALOGUE_LANGUAGE_SPANISH;
            break;

        default:
            rAssertMsg( false, "Language not supported by sound system" );
            break;
    }

    if( m_currentLanguage == oldLanguage )
    {
        //
        // Nothing needs to be done
        //
        return;
    }

    for( i = 0; i < s_NumDialogCementFiles; i++ )
    {
        GetLoadingManager()->UnregisterCementLibrary( m_soundCementFileHandles[i] );
    }

    registerDialogueCementFiles( cementFileName );

    m_languageSelected = true;
}

//=============================================================================
// daSoundRenderingManager::QueueRadscriptFileLoads
//=============================================================================
// Description: Set up the RadScript file loads with the loading manager
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void daSoundRenderingManager::QueueRadscriptFileLoads()
{
    HeapMgr()->PushHeap (GMA_AUDIO_PERSISTENT);

    unsigned int i;
    char filename[100];
    const char* scriptName;
    
    //
    // Queue up the RadScript file loading requests
    //
    GetLoadingManager()->AddRequest( FILEHANDLER_SOUND, s_DaSoundTypeInfo.m_Filename, GMA_AUDIO_PERSISTENT );

    for( i = 0; i < NumSoundScripts; i++ )
    {
        //
        // Hack!
        //
        // Oops.  Need to load the correct dialogue script depending on which language has been
        // selected
        //
        if( i == DIALOGUE_SCRIPT_POSITION )
        {
            switch( m_currentLanguage )
            {
                case DIALOGUE_LANGUAGE_ENGLISH:
                    scriptName = s_DaSoundScripts[i].m_Filename;
                    break;

                case DIALOGUE_LANGUAGE_FRENCH:
                    scriptName = "\\dialogfr.spt";
                    break;

                case DIALOGUE_LANGUAGE_GERMAN:
                    scriptName = "\\dialogge.spt";
                    break;

                case DIALOGUE_LANGUAGE_SPANISH:
                    scriptName = "\\dialogsp.spt";
                    break;

                default:
                    rAssert( false );
                    scriptName = s_DaSoundScripts[i].m_Filename;
                    break;
            }
        }
        else if( i == NIS_SCRIPT_POSITION )
        {
            switch( m_currentLanguage )
            {
                case DIALOGUE_LANGUAGE_ENGLISH:
                    scriptName = s_DaSoundScripts[i].m_Filename;
                    break;

                case DIALOGUE_LANGUAGE_FRENCH:
                    scriptName = "\\nisfr.spt";
                    break;

                case DIALOGUE_LANGUAGE_GERMAN:
                    scriptName = "\\nisge.spt";
                    break;

                case DIALOGUE_LANGUAGE_SPANISH:
                    scriptName = "\\nissp.spt";
                    break;

                default:
                    rAssert( false );
                    scriptName = s_DaSoundScripts[i].m_Filename;
                    break;
            }
        }
        else if( i == INTERACTIVE_PROPS_SCRIPT_POSITION )
        {
            switch( m_currentLanguage )
            {
                case DIALOGUE_LANGUAGE_ENGLISH:
                    scriptName = s_DaSoundScripts[i].m_Filename;
                    break;

                case DIALOGUE_LANGUAGE_FRENCH:
                    scriptName = "\\interactive_propsfr.spt";
                    break;

                case DIALOGUE_LANGUAGE_GERMAN:
                    scriptName = "\\interactive_propsge.spt";
                    break;

                case DIALOGUE_LANGUAGE_SPANISH:
                    scriptName = "\\interactive_propssp.spt";
                    break;

                default:
                    rAssert( false );
                    scriptName = s_DaSoundScripts[i].m_Filename;
                    break;
            }
        }
        else
        {
            scriptName = s_DaSoundScripts[i].m_Filename; 
        }
        sprintf( filename, "%s%s", SCRIPT_DIR, scriptName );
        GetLoadingManager()->AddRequest( FILEHANDLER_SOUND, filename, GMA_AUDIO_PERSISTENT );
    }

    HeapMgr()->PopHeap (GMA_AUDIO_PERSISTENT);
}

//=============================================================================
// daSoundRenderingManager::LoadTypeInfoFile
//=============================================================================
// Description: Start the loading of the type info
//
// Parameters:  filename - name of type info file
//              fileHandler - completion callback object
//
// Return:      void 
//
//=============================================================================
void daSoundRenderingManager::LoadTypeInfoFile( const char* filename, SoundFileHandler* fileHandler )
{
    m_soundFileHandler = fileHandler;

    IRadTypeInfoSystem* pTypeInfoSystem = ::radTypeInfoSystemGet( );
    rAssert( pTypeInfoSystem != NULL );
    pTypeInfoSystem->AddRef( );
    pTypeInfoSystem->LoadTypeInfo
    (
        gTuneSound ? GMA_AUDIO_PERSISTENT : GMA_TEMP,
        filename,
        daSoundRenderingManager::TypeInfoComplete,
        (void*)false
    );
    pTypeInfoSystem->Release( );
}

//=============================================================================
// daSoundRenderingManager::LoadScriptFile
//=============================================================================
// Description: Start the loading of a RadScript script file
//
// Parameters:  filename - name of script file
//              fileHandler - completion callback object
//
// Return:      void 
//
//=============================================================================
void daSoundRenderingManager::LoadScriptFile( const char* filename, SoundFileHandler* fileHandler )
{
    const bool* lastScript;

    m_soundFileHandler = fileHandler;

    // Create the script object, if it hasn't been done yet
    if( m_pScript == NULL )
    {
        m_pScript = ::radScriptCreateScript
        (
            RADMEMORY_ALLOC_TEMP
        );
        rAssert( m_pScript != NULL );
        m_pScript->AddRef( );
    }

    m_pScript->SetAllocator( GetThisAllocator( ) );

    //
    // Pass a flag indicating if this is the last script as user info
    //
    if( m_scriptLoadCount < NUM_CHARACTER_SCRIPTS )
    {
        m_pScript->SetContext( GetCharacterNamespace( m_scriptLoadCount ) );
    }
    else if( m_scriptLoadCount >= NumSoundScripts - NUM_TUNING_SCRIPTS )
    {
        m_pScript->SetContext( GetTuningNamespace( ) );
    }
    else
    {
        m_pScript->SetContext( GetSoundNamespace( ) );
    }

    if( m_scriptLoadCount == NumSoundScripts - 1 )
    {
        lastScript = &LastScriptTrue;
    }
    else
    {
        lastScript = &LastScriptFalse;
    }

    // Load the script
    m_pScript->Load
    (
        filename,
        daSoundRenderingManager::ScriptComplete,
        (void*)lastScript,
        RADMEMORY_ALLOC_TEMP
    );
 }

//=============================================================================
// Function:    daSoundRenderingManager::GetSoundNamespace
//=============================================================================
// Description: Get the sound resource namespace
//
// Parameters:  none
//
// Returns:     Returns a pointer to the sound resource namespace
//
//-----------------------------------------------------------------------------

IRadNameSpace* daSoundRenderingManager::GetSoundNamespace( void )
{
    return m_pResourceNameSpace;
}

//=============================================================================
// Function:    daSoundRenderingManager::GetTuningNamespace
//=============================================================================
// Description: Get the sound tuning namespace
//
// Parameters:  none
//
// Returns:     Returns a pointer to the sound tuning namespace
//
//-----------------------------------------------------------------------------

IRadNameSpace* daSoundRenderingManager::GetTuningNamespace( void )
{
    return m_pTuningNameSpace;
}

//=============================================================================
// daSoundRenderingManager::GetCharacterNamespace
//=============================================================================
// Description: Get the specified character namespace
//
// Parameters:  index - index into list of namespaces
//
// Return:      Pointer to the desired namespace
//
//=============================================================================
IRadNameSpace* daSoundRenderingManager::GetCharacterNamespace( unsigned int index )
{
    rAssert( index < NUM_CHARACTER_NAMESPACES );

    return( m_pCharacterNameSpace[index] );
}

//=============================================================================
// Function:    daSoundRenderingManager::GetTheListener
//=============================================================================
// Description: Get the listener
//
// Parameters:  none
//
// Returns:     Returns a pointer to the listener
//
//-----------------------------------------------------------------------------

IRadSoundHalListener* daSoundRenderingManager::GetTheListener( void )
{
    return ::radSoundHalListenerGet( );
}

//=============================================================================
// Function:    daSoundRenderingManager::GetDynaLoadManager
//=============================================================================
// Description: Get the dynamic loading manager
//
// Parameters:  none
//
// Returns:     Returns a pointer to the dynamic loading manager
//
//-----------------------------------------------------------------------------

daSoundDynaLoadManager* daSoundRenderingManager::GetDynaLoadManager( void )
{
    return m_pDynaLoadManager;
}

//=============================================================================
// Function:    daSoundRenderingManager::GetDialogManager
//=============================================================================
// Description: Get the dialog manager
//
// Parameters:  none
//
// Returns:     Returns a pointer to the dialog manager
//
//-----------------------------------------------------------------------------

/*IDaSoundDialogManager* daSoundRenderingManager::GetDialogManager( void )
{
    return m_pDialogManager;
}*/

//=============================================================================
// Function:    daSoundRenderingManager::GetTuner
//=============================================================================
// Description: Get the tuner
//
// Parameters:  none
//
// Returns:     Returns a pointer to the tuner
//
//-----------------------------------------------------------------------------

IDaSoundTuner* daSoundRenderingManager::GetTuner( void )
{
    return m_pTuner;
}

//=============================================================================
// Function:    daSoundRenderingManager::GetObjectDataLibrary
//=============================================================================
// Description: Get the object data library
//
// Parameters:  none
//
// Returns:     Returns a pointer to the object data library
//
//-----------------------------------------------------------------------------

/*IDaSoundObjectDataLibrary* daSoundRenderingManager::GetObjectDataLibrary( void )
{
    return m_pObjectDataLibrary;
}*/

//=============================================================================
// Function:    daSoundRenderingManager::GetResourceManager
//=============================================================================
// Description: Get the resource manager
//
// Parameters:  none
//
// Returns:     Returns a pointer to the resource manager
//
//-----------------------------------------------------------------------------

daSoundResourceManager* daSoundRenderingManager::GetResourceManager( void )
{
    return m_pResourceManager;
}

//=============================================================================
// Function:    daSoundRenderingManager::GetPlayerManager
//=============================================================================
// Description: Get the player manager
//
// Parameters:  none
//
// Returns:     Returns a pointer to the player manager
//
//-----------------------------------------------------------------------------

daSoundPlayerManager* daSoundRenderingManager::GetPlayerManager( void )
{
    return m_pPlayerManager;
}

//=============================================================================
// Function:    daSoundRenderingManager::TypeInfoComplete
//=============================================================================
// Description: Asynchronous file load complete callback.  Redundant, but
//              RadScript requires a static function for callback
//
// Parameters:  pUserData - some user data to pass on
//
// Note:        This relies on the sound manager being a singleton
//
//-----------------------------------------------------------------------------
void daSoundRenderingManager::TypeInfoComplete( void* pUserData )
{
    rAssert( s_Singleton != NULL );
    s_Singleton->ProcessTypeInfo( pUserData );
}

//=============================================================================
// daSoundRenderingManager::ScriptComplete
//=============================================================================
// Description: Asynchronous file load complete callback.  Redundant, but
//              RadScript requires a static function for callback
//
// Parameters:  pUserData - some user data to pass on
//
// Note:        This relies on the sound manager being a singleton
//
//=============================================================================
void daSoundRenderingManager::ScriptComplete( void* pUserData )
{
    rAssert( s_Singleton != NULL );
    s_Singleton->ProcessScript( pUserData );
}

//=============================================================================
// daSoundRenderingManager::SoundObjectCreated
//=============================================================================
// Description: RadScript callback function called when an object is created.
//              We use it to register the object for loading/unloading
//
// Parameters:  objName - name of sound resource to load/unload
//
// Return:      void 
//
//=============================================================================
void daSoundRenderingManager::SoundObjectCreated( const char* objName, IRefCount* obj )
{
    bool added;
    rAssert( NULL != dynamic_cast< daSoundResourceData*>( obj ) );
    daSoundResourceData* resourceObj = static_cast<daSoundResourceData*>( obj );

    //
    // We only need to preload clips
    //
    if( resourceObj->GetStreaming() == false )
    {
        added = GetSoundManager()->GetSoundLoader()->AddResourceToCurrentCluster( objName );
        rAssert( added );
    }
    else
    {
        volatile int x = 4;
    }
}

//=============================================================================
// daSoundRenderingManager::ProcessTypeInfo
//=============================================================================
// Description: Asynchronous file load complete callback.  
//
// Parameters:  ( void* pUserData )
//
// Return:      void 
//
//=============================================================================
void daSoundRenderingManager::ProcessTypeInfo( void* pUserData )
{
    m_soundFileHandler->LoadCompleted();
}

//=============================================================================
// daSoundRenderingManager::ScriptComplete
//=============================================================================
// Description: Asynchronous script load callback
//
// Parameters:  pUserData - some user data to pass on
//
// Return:      void 
//
//=============================================================================
void daSoundRenderingManager::ProcessScript( void* pUserData )
{
    ScriptObjCreateCallback* callbackPtr = NULL;
    const bool* lastScript = reinterpret_cast<const bool*>( pUserData );

    rAssert( m_pScript != NULL );

    //
    // Tell the sound manager which sound cluster we'll associate 
    // with the resources in this script
    //
    if( m_scriptLoadCount < NumSoundScripts - NUM_TUNING_SCRIPTS )
    {
        callbackPtr = SoundObjectCreated;

        if( m_scriptLoadCount < NumClusterNames )
        {
            GetSoundManager()->GetSoundLoader()->SetCurrentCluster( s_ScriptClusters[m_scriptLoadCount] );
        }
        else
        {
            GetSoundManager()->GetSoundLoader()->SetCurrentCluster( static_cast<SoundClusterName>(SC_CAR_BASE + m_scriptLoadCount - NumClusterNames) );
        }
    }

    // Execute script file
    
    unsigned int start = radTimeGetMicroseconds( );
    m_pScript->Run( callbackPtr );
    unsigned int finished = radTimeGetMicroseconds( );
    unsigned int dif = finished - start;
    
    gTotalMicrosecondsWastedParsingScripts += dif;
    
    if ( dif > 1000 )
    {
        //rTunePrintf( "\n\nAUDIO: Holy !@#$ script took [%d] ms to parse\n\n", dif / 1000 );
    }
        
    m_pScript->UnLoad( );

    if( *lastScript )
    {
        rReleasePrintf( "\n\nAUDIO: Wasted [%d] ms of load time parsing scripts\n\n", gTotalMicrosecondsWastedParsingScripts / 1000);
           
        // Free the script
        rAssert( m_pScript != NULL );
        m_pScript->Release( );
        m_pScript = NULL;

        //
        // At this point the sound resources should be finalized, lets lock them down
        // and then intialize some systems.
        //

        GetTuner( )->Initialize( );
        
        // Initialize the dialog system
        
        SoundManager::GetInstance( )->m_dialogCoordinator->Initialize( );
            
        Sound::daSoundResourceManager::GetInstance( )->SetResourceLockdown( true );
        
        m_pPlayerManager->Initialize( );

        // We are now fully initialized
        m_IsInitialized = true;
        
        if ( ! gTuneSound )
        {
            radScriptUnLoadAllTypeInfo( );        
        }
    }

    ++m_scriptLoadCount;

    m_soundFileHandler->LoadCompleted();
}


//=============================================================================
// Public Functions
//=============================================================================

//=============================================================================
// Function:    ::daSoundRenderingManagerCreate
//=============================================================================
// Description: Create the sound manager
//
// Parameters:  allocator - the memory allocator to use
//
// Preconditions:
//              In order to manage memory fragmentation issues, and to
//              allow a timely intialization of sound, this command should
//              be executed before any frontend or ingame structures have
//              been created.
//
// Postconditions:
//              This will generate any singleton classes related to the Dark
//              Angel sound system.  It will register any appropriate
//              cement files to give access to sound, and it will parse
//              any sound scripts so that the sound system will be ready
//              to start recieving sound events.
//
//-----------------------------------------------------------------------------

void daSoundRenderingManagerCreate( radMemoryAllocator allocator )
{
    rAssert( daSoundRenderingManager::GetInstance( ) == NULL );

    // Create the sound manager
    daSoundRenderingManager* pSoundManager = new ( allocator ) daSoundRenderingManager( );
    rAssert( pSoundManager != NULL );
    pSoundManager->AddRef( );
    rAssert( pSoundManager == daSoundRenderingManager::GetInstance( ) );

    rDebugChannelInitialize( GMA_MUSIC );
    //rDebugChannelEnable( radmusic::debug_channel );

    //
    // Initialize some usefull systems
    //
    ::radScriptInitialize( allocator );
#ifndef FINAL
    if( gTuneSound )
    {
        ::radRemoteScriptInitialize( allocator );
    }
#endif
    
    gpBTreeNodePool = (radObjectBTreeNode*) radMemoryAlloc(
        allocator,
        MAX_BTREE_NODES * sizeof( radObjectBTreeNode ) );

    ::memset( gpBTreeNodePool, 0, MAX_BTREE_NODES * sizeof( radObjectBTreeNode ) );       
    
    radMemoryMonitorIdentifyAllocation( gpBTreeNodePool, "Global BTree Node Pool" );
        
    radObjectBTree::Initialize( gpBTreeNodePool, MAX_BTREE_NODES );
    // Initialize

    SoundNucleusInitialize( allocator );
}

//=============================================================================
// Function:    ::daSoundRenderingManagerGet
//=============================================================================
// Description: Get a pointer to the sound manager singleton class
//
//-----------------------------------------------------------------------------

daSoundRenderingManager* daSoundRenderingManagerGet( void )
{
    rAssert( daSoundRenderingManager::GetInstance( ) != NULL );
    return
    (
        static_cast< daSoundRenderingManager* >( daSoundRenderingManager::GetInstance( ) )
    );
}

void daSoundRenderingManager::FilePerformanceEvent(
    bool start,
    const char * pFile,
    unsigned int bytes )
{
    daSoundRenderingManager * pThis = daSoundRenderingManager::GetInstance( );

    unsigned int now = radTimeGetMilliseconds( );
        
    if ( start )
    {
        pThis->m_LastPerformanceEventTime = now;

        if ( CommandLineOptions::Get( CLO_AUDIO_LOADING_SPEW ) )
        {
            rTunePrintf( "<<START>> Async Loading: (Audio) %s Bytes: 0x%x\n", pFile, bytes );
        }
    }
    else
    {
        unsigned int dif = now - pThis->m_LastPerformanceEventTime;
        
        if ( CommandLineOptions::Get( CLO_AUDIO_LOADING_SPEW ) )
        {       
            rTunePrintf( "<< END >> Async Loading: (Audio) %s (%d msecs)\n", pFile, dif );
        }
    }
}

void daSoundRenderingManager::Render( void )
{
    if ( gDaSoundStats )
    {
        m_pPlayerManager->Render( );
    }
}

//=============================================================================
// Function:    ::daSoundRenderingManagerTerminate
//=============================================================================
// Description: Terminate the sound system
//
//-----------------------------------------------------------------------------

void daSoundRenderingManagerTerminate( void )
{
    rAssert( daSoundRenderingManager::GetInstance( ) != NULL );
    daSoundRenderingManager::GetInstance( )->Terminate( );
    daSoundRenderingManager::GetInstance( )->Release( );
    rAssert( daSoundRenderingManager::GetInstance( ) == NULL );

    SoundNucleusTerminate( );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//=============================================================================
// daSoundRenderingManager::registerDialogueCementFiles
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* cementFilename )
//
// Return:      void 
//
//=============================================================================
void daSoundRenderingManager::registerDialogueCementFiles( const char* cementFilename )
{
#if defined( RAD_XBOX )
    char dialogNameBuffer[ 16 ];
    int i = 0;

    strcpy( dialogNameBuffer, cementFilename );
    char* numberPosition = strchr( dialogNameBuffer, '?' );
    for ( unsigned int j = 0; j < s_NumDialogCementFiles; j++ )
    {
        *numberPosition = j + '0';
        m_soundCementFileHandles[i++] = GetLoadingManager()->RegisterCementLibrary( dialogNameBuffer );
    }
#else
    rAssert( s_NumDialogCementFiles == 1 );
    m_soundCementFileHandles[0] = GetLoadingManager()->RegisterCementLibrary( cementFilename );
#endif
}

} // Sound Namespace

