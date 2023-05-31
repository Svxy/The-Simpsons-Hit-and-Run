//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   PS2Platform   
//
// Description: Abstracts the differences for setting up and shutting down
//              the different platforms.
//
// History:     + Stolen and cleaned up from Svxy -- Darwin Chau
//
//===========================================================================

//========================================
// System Includes
//========================================
// Sony
#include <libcdvd.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <sifcmd.h>
#include <eetypes.h>
#include <eekernel.h>
#include <libpad.h>

// Standard Lib
#include <stdlib.h>
#include <string.h>
// SN Systems
#include <libsn.h>
// Pure 3D
#include <p3d/anim/compositedrawable.hpp>
#include <p3d/anim/expression.hpp>
#include <p3d/anim/multicontroller.hpp>
#include <p3d/anim/polyskin.hpp>
#include <p3d/anim/sequencer.hpp>
#include <p3d/anim/skeleton.hpp>
#include <p3d/camera.hpp>
#include <p3d/gameattr.hpp>
#include <p3d/image.hpp>
#include <p3d/imagefont.hpp>
#include <p3d/light.hpp>
#include <p3d/locator.hpp>
#include <p3d/platform.hpp>
#include <p3d/scenegraph/scenegraph.hpp>
#include <p3d/sprite.hpp>
#include <p3d/utility.hpp>
#include <p3d/texture.hpp>
#include <p3d/file.hpp>
#include <p3d/shader.hpp>
#include <p3d/matrixstack.hpp>
#include <p3d/memory.hpp>
#include <p3d/bmp.hpp>
#include <p3d/png.hpp>
#include <p3d/targa.hpp>
#include <p3d/font.hpp>
#include <p3d/texturefont.hpp>
#include <p3d/unicode.hpp>
#include <pddi/pddiext.hpp>
// Pure 3D: Loader-specific
#include <render/Loaders/GeometryWrappedLoader.h>
#include <render/Loaders/StaticEntityLoader.h>
#include <render/Loaders/StaticPhysLoader.h>
#include <render/Loaders/TreeDSGLoader.h>
#include <render/Loaders/FenceLoader.h>
#include <render/Loaders/IntersectLoader.h>
#include <render/Loaders/AnimCollLoader.h>
#include <render/Loaders/AnimDSGLoader.h>
#include <render/Loaders/DynaPhysLoader.h>
#include <render/Loaders/InstStatPhysLoader.h>
#include <render/Loaders/InstStatEntityLoader.h>
#include <render/Loaders/WorldSphereLoader.h>
#include <render/Loaders/BillboardWrappedLoader.h>
#include <render/Loaders/instparticlesystemloader.h>
#include <render/Loaders/breakableobjectloader.h>
#include <render/Loaders/lensflareloader.h>
#include <render/Loaders/AnimDynaPhysLoader.h>
#include <p3d/shadow.hpp>
#include <p3d/anim/animatedobject.hpp>
#include <p3d/anim/vertexanimkey.hpp>
#include <p3d/effects/particleloader.hpp>
#include <p3d/effects/opticloader.hpp>

//This is so we can get the name of the file that's failing.
#include <../src/radfile/common/requests.hpp>

// Foundation Tech
#include <raddebug.hpp>
#include <radthread.hpp>
#include <radplatform.hpp>
#include <radtime.hpp>
#include <radmemorymonitor.hpp>
#include <raddebugcommunication.hpp>
#include <raddebugwatch.hpp>
#include <radfile.hpp>
#include <radmovie2.hpp>
#include <radload/radload.hpp>
#include <radtextdisplay.hpp>

// sim - for InstallSimLoaders
#include <simcommon/simutility.hpp>
// StateProps 
#include <stateprop/statepropdata.hpp>

// To turn off movies during an error.
#include <presentation/fmvplayer/fmvplayer.h>
#include <presentation/presentation.h>

//========================================
// Project Includes
//========================================
#include <input/inputmanager.h>
#include <main/ps2platform.h>
#include <main/game.h>
#include <main/commandlineoptions.h>
#include <memory/srrmemory.h>
#include <render/RenderManager/RenderManager.h>
#include <render/Loaders/AllWrappers.h>

#include <loading/locatorloader.h>
#include <loading/cameradataloader.h>
#include <loading/roadloader.h>
#include <loading/pathloader.h>
#include <loading/intersectionloader.h>
#include <loading/roaddatasegmentloader.h>
#include <atc/atcloader.h>

#include <debug/debuginfo.h>

#include <main/errorsps2.h>

#include <sound/soundmanager.h>

#include <presentation/presentation.h>
#include <presentation/gui/guitextbible.h>
#include <data/gamedatamanager.h>

#include <cheats/cheatinputsystem.h>

#include <mission/gameplaymanager.h>
#include <pddi/pddi.hpp>

#define PS2_SECTION "PS2_SECTION"

// #define IOP_MEMORY_TEST

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

IRadCementLibrary* PS2Platform::s_MainCement = NULL;

// Static pointer to instance of singleton.
PS2Platform* PS2Platform::spInstance = NULL;


//The Adlib font.  <sigh>
unsigned char gFont[] = 
#include <font/defaultfont.h>


//
// Uncomment the define below to run off a DVD 
//
/***** #define DVD_MEDIA *****/
#define DVD_MEDIA

//
// This value define the resolution of the rendering area.
// Based on the width, Pure3D figures out the approriate height.
//
static const int WindowSizeX = 640;

//
// The depth of the rendering area.  This value only has an effect
// when Pure3D has taken over the entire display.  When running in
// a window on the desktop, Pure3D uses the same bit depth as the
// desktop.  Pure3D only supports 16, and 32 rendering depths.
//
static const int WindowBPP = 32;

void LoadMemP3DFile( unsigned char* buffer, unsigned int size, tEntityStore* store )
{
    tFileMem* file = new tFileMem(buffer,size);
    file->AddRef();
    file->SetFilename("memfile.p3d");
    p3d::loadManager->GetP3DHandler()->Load( file, p3d::inventory );
    file->Release();
}

extern bool gIgnoreLastFrameSyncCheck;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// PS2Platform::CreateInstance
//==============================================================================
//
// Description: Creates the PS2Platform.
//
// Parameters:	None.
//
// Return:      Pointer to the PS2Platform.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
PS2Platform* PS2Platform::CreateInstance()
{
MEMTRACK_PUSH_GROUP( "PS2Platform" );
    rAssert( spInstance == NULL );

    spInstance = new(GMA_PERSISTENT) PS2Platform;
    rAssert( spInstance );
MEMTRACK_POP_GROUP("PS2Platform");
    
    return spInstance;
}

//==============================================================================
// PS2Platform::GetInstance
//==============================================================================
//
// Description: - Access point for the PS2Platform singleton.  
//
// Parameters:	None.
//
// Return:      Pointer to the PS2Platform.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
PS2Platform* PS2Platform::GetInstance()
{
    rAssert( spInstance != NULL );
    
    return spInstance;
}


//==============================================================================
// PS2Platform::DestroyInstance
//==============================================================================
//
// Description: Destroy the PS2Platform.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void PS2Platform::DestroyInstance()
{
    rAssert( spInstance != NULL );

    delete( GMA_PERSISTENT, spInstance );
    spInstance = NULL;
}

 
//==============================================================================
// PS2Platform::InitializeFoundation
//==============================================================================
// Description: Get FTech ready to go.
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: The FTech systems must be initialized in a particular order.
//              Consult their documentation before changing.
//
//==============================================================================
void PS2Platform::InitializeFoundation() 
{
    const char IRX_PATH[] = "IRX\\";
    const char SONY_LIB_NAME[] = "IOPRP254.IMG";

    //
    // Initialize the memory heaps
    //
    PS2Platform::InitializeMemory();
    
#ifndef FINAL
    //
    // Register an out-of-memory display handler in case something goes bad
    // while allocating the heaps
    //
    ::radMemorySetOutOfMemoryCallback( PrintOutOfMemoryMessage, NULL );
#endif

    //
    // Initialize memory monitor by JamesCo. TM.
    //
    if( CommandLineOptions::Get( CLO_MEMORY_MONITOR) )
    {
        const int KB = 1024;
        ::radMemoryMonitorInitialize( 100 * KB, GMA_DEBUG );
    }
    
    // Setup the memory heaps
    //
    HeapMgr()->PrepareHeapsStartup ();


    // Seed the heap stack with this
    //
    HeapMgr()->PushHeap (GMA_PERSISTENT);

    //
    // Initilalize the platform system
    // On the PS2, provide the root path for IRXs. 
    // 
#ifdef DVD_MEDIA
    radPlatformGameMediaType mediaType = GameMediaDVD;
#else
    radPlatformGameMediaType mediaType = GameMediaCD;
#endif // DVD_MEDIA

    if( CommandLineOptions::Get( CLO_FIREWIRE ) )
    {
        ::radPlatformInitialize( IRX_PATH, 
                                 IOPMediaCDVD, 
                                 mediaType,
                                 NULL,
                                 GMA_PERSISTENT );
    }
    else if( CommandLineOptions::Get( CLO_CD_FILES_ONLY ) )
    {
        ::radPlatformInitialize( IRX_PATH, 
                                 IOPMediaCDVD, 
                                 mediaType,
                                 SONY_LIB_NAME,
                                 GMA_PERSISTENT );
    }
    else
    {
        ::radPlatformInitialize( IRX_PATH, 
                                 IOPMediaHost, 
                                 mediaType,
                                 SONY_LIB_NAME,
                                 GMA_PERSISTENT );
    }
    


#ifdef IOP_MEMORY_TEST

    IRadTextDisplay * pDisp;

    radTextDisplayGet( & pDisp, RADMEMORY_ALLOC_DEFAULT );
    
    char buf[ 256 ];
    
    unsigned int maxFreeMemSize = sceSifQueryMaxFreeMemSize( );
    unsigned int totalFreeMemSize = sceSifQueryTotalFreeMemSize( );
    unsigned int memSize = sceSifQueryMemSize( );
    
    sprintf(
        buf,
        "\nMemSize: [0x%x]\nTotalFreeMemSize: [0x%x]\nMaxFreeMemSize:[0x%x]\n",
        memSize,
        totalFreeMemSize,
        maxFreeMemSize );
    
    pDisp->TextOut( buf );
    pDisp->SwapBuffers( );
    
    while( 1 ) { }
    
    pDisp->Release( );
#endif    
   
    
    
    // Eat up 6 mb if "TOOL" in command line for testing IOP crashes.
    
    if ( CommandLineOptions::Get( CLO_PS2_TOOL ) )
    {
    
        // Results from above:
        
        // Tool:
        //     MemSize:          [0x7fff00]
        //     TotalFreeMemSize: [0x798400]
        //     MaxFreeMemSize:   [0x798200]
        //
        // Artist Box:
        //     MemSize:          [0x200000]
        //     TotalFreeMemSize: [0x1a3900]
        //     MaxFreeMemSize:   [0x1a3900] 

     
        const unsigned int toolMaxFreeMemSize = 0x798200;
        const unsigned int consumerMaxFreeMemSize = 0x1a3900;
        const unsigned int difMaxFreeMemSize = toolMaxFreeMemSize - consumerMaxFreeMemSize;
 
        const int allocChunkSize = 4096 * 16;
        
        rReleasePrintf( "Consuming [0x%x] bytes on tool:\n", difMaxFreeMemSize );
        
        unsigned int maxFreeMemSize;
        
        do
        {
            void * pMem = sceSifAllocIopHeap( allocChunkSize );
            rReleaseAssertMsg( pMem, "Out of memory consuming extra tool memory" );            
            maxFreeMemSize = sceSifQueryMaxFreeMemSize( );            
            rReleasePrintf( "BURNING IOP: MaxFreeMemSize: [0x%x]\n", maxFreeMemSize );
        } while( maxFreeMemSize > consumerMaxFreeMemSize );
    }
    
    //
    // Initialize the timer system
    //
    ::radTimeInitialize();

    //
    // Initialize the debug communication system.
    //
    if( CommandLineOptions::Get( CLO_FIREWIRE ) )
    {
        ::radDbgComTargetInitialize( FireWire, 
                                     radDbgComDefaultPort, 
                                     (void*)"",
                                     GMA_DEBUG ); 
    }
    else
    {
        ::radDbgComTargetInitialize( Deci,
                                     radDbgComDefaultPort, // Default
                                     NULL,                 // Default
                                     GMA_DEBUG );
    }
    
    //
    // Initialize the Watcher.
    //
#ifdef DEBUGWATCH
        ::radDbgWatchInitialize( "SRR2",
                                 16384 * 32, // Default
                                 GMA_DEBUG );
#endif // DEBUGWATCH

    //
    // Initialize the file system.
    //
    ::radFileInitialize( 50, // Default
                         32, // Default
                         GMA_PERSISTENT );

	::radLoadInitialize( );

    if( CommandLineOptions::Get( CLO_FIREWIRE ) )
    {
        ::radSetDefaultDrive( "REMOTEDRIVE:" );
    }
    else if( CommandLineOptions::Get( CLO_CD_FILES_ONLY ) )
    {
        ::radSetDefaultDrive( "CDROM:" );
    }
    else
    {
        ::radSetDefaultDrive( "HOSTDRIVE:" );
    }

    ::radDriveMount( 0, GMA_PERSISTENT );

    //
    // On PS2, synchronously load the art.rcf file. This is needed before
    // anything else can happen.
    //
    if( CommandLineOptions::Get( CLO_CD_FILES_ONLY ) )
    {
        radFileRegisterCementLibrarySync( & s_MainCement, "art.rcf" );
    }


    //
    // Initialize the new movie player
    //
    ::radMovieInitialize2( GMA_PERSISTENT );

    //
    // Init VU0 for optimizations.
    //
    ::radMathInitialize();

#ifndef FINAL
    //
    // Set up exception handlers, so that when the game crashes we can
    // see something.
    //
    if( CommandLineOptions::Get( CLO_CD_FILES_ONLY ) ||
        CommandLineOptions::Get( CLO_FIREWIRE ) )
    {
        SetDebugHandler( 1, handleTLBChange );
        SetDebugHandler( 2, handleTLBLoadMismatch );
        SetDebugHandler( 3, handleTLBStoreMismatch );
        SetDebugHandler( 4, handleAddressLoadError );
        SetDebugHandler( 5, handleAddressStoreError );
        SetDebugHandler( 6, handleBusFetchError );
        SetDebugHandler( 7, handleBusDataError );

        // #8 is "system call exception", #9 is "breakpoint exception".
        // I don't think we need those.

        SetDebugHandler( 10, handleReservedInstruction );
        SetDebugHandler( 11, handleCoprocessor );
        SetDebugHandler( 12, handleOverflow );
        SetDebugHandler( 13, handleTrap );
    }
#endif

    HeapMgr()->PopHeap (GMA_PERSISTENT);
}



//==============================================================================
// PS2Platform::InitializeMemory
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void PS2Platform::InitializeMemory()
{
    //
    // Only do this once!
    //
    if( gMemorySystemInitialized == true )
    {
        return;
    }

    gMemorySystemInitialized = true;

    //
    // Initialize thread system.
    //
    ::radThreadInitialize();

    //
    // Initialize memory system.
    //
    ::radMemoryInitialize();
}




//==============================================================================
// PS2Platform::InitializePlatform
//==============================================================================
// Description: Get the PS2 ready to go.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void PS2Platform::InitializePlatform() 
{
    HeapMgr()->PushHeap (GMA_PERSISTENT);

    if( CommandLineOptions::Get( CLO_SN_PROFILER ) )
    {
        this->EnableSnProfiler();
    }
    
    InitializePure3D();

    // Add anything here that needs to be before the
    // drive is opened.
    DisplaySplashScreen( Error ); // blank screen

    //
    // This is SLOW so do it last.
    //
    InitializeFoundationDrive();

    //
    // Can only setup the controller after the IOP has been rebooted.
    //
    GetInputManager()->Init();

    HeapMgr()->PopHeap (GMA_PERSISTENT);
}


//==============================================================================
// PS2Platform::ShutdownPlatform
//==============================================================================
// Description: Shut down the PS2.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void PS2Platform::ShutdownPlatform()
{
    ShutdownPure3D();
    ShutdownFoundation();
}

void PS2Platform::ResetMachine()
{
    scePadEnd();
    sceSifExitCmd();

    char build = 0;

#ifdef RAD_DEBUG
    build = 'd';
#elif defined RAD_TUNE
    build = 't';
#else
    build = 'r';
#endif

    char imageName[64];

    if( CommandLineOptions::Get( CLO_CD_FILES_ONLY ) )
    {
#ifndef RAD_E3
        sprintf( imageName, "cdrom0:\\slps123.45" );
#else
        sprintf( imageName, "cdrom0:\\slps123.45" );
#endif
        LoadExecPS2( imageName, 0, NULL );
    }
    else
    {
        char *args[] = { "hostfiles" };
#ifndef RAD_E3
        sprintf( imageName, "hostdrive:\\srr2p%c.elf", build );
#else
        sprintf( imageName, "hostdrive:\\srr2e3p%c.elf", build );
#endif
        LoadExecPS2( imageName, 1, args );
    }
}

//=============================================================================
// PS2Platform::LaunchDashboard
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void PS2Platform::LaunchDashboard()
{
    GetLoadingManager()->CancelPendingRequests();

    //TODO: Make sure sounds shut down too.
    GetSoundManager()->SetMasterVolume( 0.0f );

    DisplaySplashScreen( FadeToBlack );

    GetPresentationManager()->StopAll();

    //Oh boy.
    GameDataManager::DestroyInstance();  //Get rid of memcards

    p3d::loadManager->CancelAll();

    SoundManager::DestroyInstance();    

    ShutdownPlatform();
    ResetMachine();
}


//=============================================================================
// PS2Platform::DisplaySplashScreen
//=============================================================================
// Description: Comment
//
// Parameters:  ( SplashScreen screenID, 
//                const char* overlayText = NULL, 
//                float fontScale = 1.0f, 
//                float textPosX = 0.0f, 
//                float textPosY = 0.0f, 
//                tColour textColour,
//                int fadeFrames = 3 )
//
// Return:      void 
//
//=============================================================================
void PS2Platform::DisplaySplashScreen( SplashScreen screenID, 
                                       const char* overlayText, 
                                       float fontScale, 
                                       float textPosX, 
                                       float textPosY,
                                       tColour textColour,
                                       int fadeFrames )
{
    DisplaySplashScreen( NULL, overlayText, fontScale, textPosX, textPosY, textColour, fadeFrames );
}

//=============================================================================
// PS2Platform::DisplaySplashScreen
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* textureName, 
//                const char* overlayText = NULL, 
//                float fontScale = 1.0f, 
//                float textPosX = 0.0f, 
//                float textPosY = 0.0f, 
//                tColour textColour,
//                int fadeFrames = 3 )
//
// Return:      void 
//
//=============================================================================
void PS2Platform::DisplaySplashScreen( const char* textureName,
                                       const char* overlayText, 
                                       float fontScale, 
                                       float textPosX, 
                                       float textPosY, 
                                       tColour textColour,
                                       int fadeFrames )
{
    p3d::pddi->DrawSync();

    HeapMgr()->PushHeap( GMA_TEMP );
    p3d::inventory->PushSection();
    p3d::inventory->AddSection( PS2_SECTION );
    p3d::inventory->SelectSection( PS2_SECTION );
    
    P3D_UNICODE unicodeText[256];

    // Save the current Projection mode so I can restore it later
    pddiProjectionMode pm = p3d::pddi->GetProjectionMode();
    p3d::pddi->SetProjectionMode(PDDI_PROJECTION_DEVICE);

    pddiCullMode cm = p3d::pddi->GetCullMode();
    p3d::pddi->SetCullMode(PDDI_CULL_NONE);


    //CREATE THE FONT
    tTextureFont* thisFont = NULL;

    // Convert memory buffer into a texturefont.
    //
    //p3d::load(gFont, DEFAULTFONT_SIZE, GMA_TEMP);
    LoadMemP3DFile( gFont, DEFAULTFONT_SIZE, p3d::inventory );

    thisFont = p3d::find<tTextureFont>("adlibn_20");
    rAssert( thisFont );

    thisFont->AddRef();
    tShader* fontShader = thisFont->GetShader();
    //fontShader->SetInt( )


    p3d::AsciiToUnicode( overlayText, unicodeText, 256 );

    // Make the missing letter into somthing I can see
    //
    thisFont->SetMissingLetter(p3d::ConvertCharToUnicode('j'));

    int a = 0;

    do
    {
    	p3d::pddi->SetColourWrite(true, true, true, true);
        p3d::pddi->SetClearColour( pddiColour(0,0,0) );
        p3d::pddi->BeginFrame();
        p3d::pddi->Clear(PDDI_BUFFER_COLOUR);

        //This is for fading in the font and shaders.
        int bright = 255;
        if (a < fadeFrames) bright = (a * 255) / fadeFrames;
        if ( bright > 255 ) bright = 255;
        tColour c(bright, bright, bright, 255);

        //Display font
        if (overlayText != NULL)
        {
            tColour colour = textColour;
            colour.SetAlpha( bright );

            thisFont->SetColour( colour );

            p3d::pddi->SetProjectionMode(PDDI_PROJECTION_ORTHOGRAPHIC);
            p3d::stack->Push();
            p3d::stack->LoadIdentity();

            p3d::stack->Translate( textPosX, textPosY, 1.5f);
            float scaleSize = 1.0f / 480.0f;  //This is likely good for 528 also.
            p3d::stack->Scale(scaleSize * fontScale, scaleSize* fontScale , 1.0f);

            if ( textPosX != 0.0f || textPosY != 0.0f )
            {
                thisFont->DisplayText( unicodeText );
            }
            else
            {
                thisFont->DisplayText( unicodeText, 3 );
            }

            p3d::stack->Pop();
        }

        p3d::pddi->EndFrame();
        p3d::context->SwapBuffers();

        ++a;

    } while (a <= fadeFrames);

    p3d::pddi->SetCullMode(cm);
    p3d::pddi->SetProjectionMode(pm);

    p3d::pddi->DrawSync();

    gIgnoreLastFrameSyncCheck = true;

    //Should do this after a vsync.
    thisFont->Release();

    p3d::inventory->RemoveSectionElements(PS2_SECTION);
    p3d::inventory->DeleteSection(PS2_SECTION);
    p3d::inventory->PopSection();

    gIgnoreLastFrameSyncCheck = false;

    HeapMgr()->PopHeap(GMA_TEMP);
}








//******************************************************************************
//
// Protected Member Functions
//
//******************************************************************************


//==============================================================================
// PS2Platform::InitializeFoundationDrive
//==============================================================================
// Description: Get FTech ready to go.
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: The FTech systems must be initialized in a particular order.
//              Consult their documentation before changing.
//
//==============================================================================
void PS2Platform::InitializeFoundationDrive() 
{
    //
    // No remote drives if we're only allowing files from the CD.
    //

//    if( CommandLineOptions::Get( CLO_CD_FILES_ONLY ) )
//    {
//        ::radDriveSetShadow( false );
//    }

    //
    // Get the CDROM drive and hold it open for the life of the game.
    // This is a costly operation so we only want to do it once.
    //
    if( CommandLineOptions::Get( CLO_CD_FILES_ONLY ) )
    {
        ::radDriveOpen( &mpIRadDrive, 
                        "CDROM:",
                        NormalPriority, // Default
                        GMA_PERSISTENT );

        //Only care about CD drives.
        mpIRadDrive->RegisterErrorHandler( this, NULL );
    }
    else
    {
        ::radDriveOpen( &mpIRadDrive, 
                        "HOSTDRIVE:",
                        NormalPriority, // Default
                        GMA_PERSISTENT );

        //Only care about CD drives.
        mpIRadDrive->RegisterErrorHandler( this, NULL );
    }
    rAssert( mpIRadDrive != NULL );

    //
    // Set the read-write granulatity to prevent operations from
    // being partitioned.
    //
    //const int GRANULARITY = 20 * 1024 * 1024;
    //mpIRadDrive->SetReadWriteGranularity( GRANULARITY );

}


//==============================================================================
// PS2Platform::ShutdownFoundation
//==============================================================================
// Description: Shut down Foundation Tech.
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: The FTech systems must be terminated in the reverse order that
//              they were initialized in.
//
//==============================================================================
void PS2Platform::ShutdownFoundation()
{
    //
    // Release the main cement file.
    //
    s_MainCement->Release();
    s_MainCement = NULL;

    //
    // Release the drive we've held open since the begining.
    //
    mpIRadDrive->Release();
    mpIRadDrive = NULL;

/*
    spIRadMemoryHeapDefault->Release();
    spIRadMemoryHeapDefault = NULL;
    spIRadMemoryHeapTemp->Release();
    spIRadMemoryHeapTemp = NULL;
    spIRadMemoryHeapPersistent->Release();
    spIRadMemoryHeapPersistent = NULL;
    spIRadMemoryHeapLevel->Release();
    spIRadMemoryHeapLevel = NULL;
    spIRadMemoryHeapSwapA->Release();
    spIRadMemoryHeapSwapA = NULL;
    spIRadMemoryHeapSwapB->Release();
    spIRadMemoryHeapSwapB = NULL;
#ifndef RAD_RELEASE
    spIRadMemoryHeapDebug->Release();
    spIRadMemoryHeapDebug = NULL;
#endif // RAD_RELEASE
*/

    //
    // Shutdown the systems in the reverse order.
    //
    ::radDriveUnmount();
    ::radFileTerminate();
    ::radDbgWatchTerminate();
    if( CommandLineOptions::Get( CLO_MEMORY_MONITOR) )
    {
        ::radMemoryMonitorTerminate();
    }
    ::radDbgComTargetTerminate();
    ::radTimeTerminate();
    ::radPlatformTerminate();
    ::radMemoryTerminate();
    ::radThreadTerminate();
    ::radMovieTerminate2( );
}


//==============================================================================
// PS2Platform::InitializePure3D
//==============================================================================
// Description: Get Pure3D ready to go.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void PS2Platform::InitializePure3D() 
{
MEMTRACK_PUSH_GROUP( "PS2Platform" );
//    p3d::SetMemAllocator( p3d::ALLOC_DEFAULT, GMA_PERSISTENT );
//    p3d::SetMemAllocator( p3d::ALLOC_LOADED, GMA_LEVEL );

    //
    // Initialise Pure3D platform object.
    // This call differs between different platforms.  The Win32 version,
    // for example requires the application instance to be passed in.
    //
    mpPlatform = tPlatform::Create();
    rAssert( mpPlatform != NULL );

    //
    // Initialiase the Pure3D context object.
    // We have to create on of these for every window, and for every PDDI
    // instance we use for rendering.  Since almost every application only
    // uses one window and PDDI library at a time, we one need to create one
    // context.
    //
    tContextInitData init;

    //
    // These values only take effect in fullscreen mode.  In windowed mode, the
    // dimensions of the window define the rendering area.  We'll define them
    // anyway for completeness sake.
    //
    init.xsize = WindowSizeX;

    //
    // Rendering to NTSC or PAL.
    //
#ifdef PAL    
    init.pal = true;
#else
    init.pal = false;
#endif

    //for progressive scan
    if( CommandLineOptions::Get( CLO_PROGRESSIVE_SCAN ) )
    {
        init.dtv480 = true;
    }

    //TODO: Investigate VSync
//    init.lockToVsync = true;
    init.lockToVsync = false;

    //
    // Create the context.
    //
    mpContext = mpPlatform->CreateContext( &init );
    rAssert( mpContext != NULL );

    //
    // Assign this context to the platform.
    //
    mpPlatform->SetActiveContext( mpContext );
    
    //((pddiExtPS2Control*)p3d::pddi->GetExtension(PDDI_EXT_PS2_CONTROL))->EnableClipper(false);
    //((pddiExtPS2Control*)p3d::pddi->GetExtension(PDDI_EXT_PS2_CONTROL))->ForceMFIFOSync(true);

    p3d::pddi->EnableZBuffer( true );

    //
    // This call installs chunk handlers for all the primary chunk types that
    // Pure3D supports.  This includes textures, materials, geometries, and the
    // like.
    //
//    p3d::InstallDefaultLoaders();
        P3DASSERT(p3d::context);
    tP3DFileHandler* p3d = new(GMA_PERSISTENT) tP3DFileHandler;
    //p3d::loadManager->AddHandler(p3d, "p3d");
    p3d::context->GetLoadManager()->AddHandler(p3d, "p3d");
    p3d::context->GetLoadManager()->AddHandler(new(GMA_PERSISTENT) tPNGHandler, "png");

    if( CommandLineOptions::Get( CLO_FE_UNJOINED ) )
    {
        p3d::context->GetLoadManager()->AddHandler(new(GMA_PERSISTENT) tBMPHandler, "bmp");
        p3d::context->GetLoadManager()->AddHandler(new(GMA_PERSISTENT) tTargaHandler, "tga");
    }
    else
    {
        p3d::context->GetLoadManager()->AddHandler(new(GMA_PERSISTENT) tBMPHandler, "p3d");
        p3d::context->GetLoadManager()->AddHandler(new(GMA_PERSISTENT) tPNGHandler, "p3d");
        p3d::context->GetLoadManager()->AddHandler(new(GMA_PERSISTENT) tTargaHandler, "p3d");
    }

//    p3d->AddHandler(new tGeometryLoader);
//    GeometryWrappedLoader* pGWL = new GeometryWrappedLoader;
    GeometryWrappedLoader* pGWL = 
       (GeometryWrappedLoader*)GetAllWrappers()->mpLoader( AllWrappers::msGeometry );
    pGWL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pGWL );

    StaticEntityLoader* pSEL = 
       (StaticEntityLoader*)GetAllWrappers()->mpLoader( AllWrappers::msStaticEntity );
    pSEL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pSEL );

    StaticPhysLoader* pSPL = 
       (StaticPhysLoader*)GetAllWrappers()->mpLoader( AllWrappers::msStaticPhys );
    pSPL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pSPL );

    TreeDSGLoader* pTDL = 
       (TreeDSGLoader*)GetAllWrappers()->mpLoader( AllWrappers::msTreeDSG );
    pTDL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pTDL );

    FenceLoader* pFL = 
       (FenceLoader*)GetAllWrappers()->mpLoader( AllWrappers::msFenceEntity );
    pFL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pFL );

    IntersectLoader* pIL = 
       (IntersectLoader*)GetAllWrappers()->mpLoader( AllWrappers::msIntersectDSG );
    pIL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pIL );
    
    AnimCollLoader* pACL = 
        (AnimCollLoader*)GetAllWrappers()->mpLoader( AllWrappers::msAnimCollEntity );
    pACL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pACL );
        
    AnimDSGLoader* pAnimDSGLoader = 
        (AnimDSGLoader*)GetAllWrappers()->mpLoader( AllWrappers::msAnimEntity );
    pAnimDSGLoader->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pAnimDSGLoader );

    DynaPhysLoader* pDPL = 
        (DynaPhysLoader*)GetAllWrappers()->mpLoader( AllWrappers::msDynaPhys );
    pDPL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pDPL );

    InstStatPhysLoader* pISPL = 
        (InstStatPhysLoader*)GetAllWrappers()->mpLoader( AllWrappers::msInstStatPhys );
    pISPL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pISPL );
    
    InstStatEntityLoader* pISEL = 
        (InstStatEntityLoader*)GetAllWrappers()->mpLoader( AllWrappers::msInstStatEntity );
    pISEL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pISEL );
    
    LocatorLoader* pLL = 
        (LocatorLoader*)GetAllWrappers()->mpLoader( AllWrappers::msLocator);
    pLL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pLL );

    RoadLoader* pRL = 
        (RoadLoader*)GetAllWrappers()->mpLoader( AllWrappers::msRoadSegment);
    pRL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pRL );

	PathLoader* pPL = 
        (PathLoader*)GetAllWrappers()->mpLoader( AllWrappers::msPathSegment);
    pPL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pPL );
    
    WorldSphereLoader* pWSL = 
        (WorldSphereLoader*)GetAllWrappers()->mpLoader( AllWrappers::msWorldSphere);
    pWSL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pWSL );

    LensFlareLoader* pLSL = 
        (LensFlareLoader*)GetAllWrappers()->mpLoader( AllWrappers::msLensFlare);
    pLSL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pLSL );

    BillboardWrappedLoader* pBWL = 
        (BillboardWrappedLoader*)GetAllWrappers()->mpLoader( AllWrappers::msBillboard);
    pBWL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pBWL );
    
    InstParticleSystemLoader* pInstParticleSystemLoader = 
        (InstParticleSystemLoader*) GetAllWrappers()->mpLoader( AllWrappers::msInstParticleSystem);
    pInstParticleSystemLoader->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pInstParticleSystemLoader );

    BreakableObjectLoader* pBreakableObjectLoader = 
        (BreakableObjectLoader*) GetAllWrappers()->mpLoader( AllWrappers::msBreakableObject);
    pBreakableObjectLoader->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pBreakableObjectLoader );

	AnimDynaPhysLoader*	pAnimDynaPhysLoader = 
		(AnimDynaPhysLoader*) GetAllWrappers()->mpLoader( AllWrappers::msAnimDynaPhys);
	pAnimDynaPhysLoader->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pAnimDynaPhysLoader );

	AnimDynaPhysWrapperLoader* pAnimWrapperLoader = 
		(AnimDynaPhysWrapperLoader*) GetAllWrappers()->mpLoader( AllWrappers::msAnimDynaPhysWrapper);
	pAnimWrapperLoader->SetRegdListener( GetRenderManager(), 0 );
	p3d->AddHandler( pAnimWrapperLoader );

    p3d->AddHandler(new(GMA_PERSISTENT) tTextureLoader);
    p3d->AddHandler( new(GMA_PERSISTENT) tSetLoader );
    p3d->AddHandler(new(GMA_PERSISTENT) tShaderLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tCameraLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tGameAttrLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tLightLoader);
  
    p3d->AddHandler(new(GMA_PERSISTENT) tLocatorLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tLightGroupLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tImageLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tTextureFontLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tImageFontLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tSpriteLoader);
    //p3d->AddHandler(new(GMA_PERSISTENT) tBillboardQuadGroupLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tSkeletonLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tPolySkinLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tCompositeDrawableLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tAnimationLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tFrameControllerLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tMultiControllerLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tAnimatedObjectFactoryLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tAnimatedObjectLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tParticleSystemFactoryLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tParticleSystemLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tLensFlareGroupLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) sg::Loader);

    p3d->AddHandler(new(GMA_PERSISTENT) tExpressionGroupLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tExpressionMixerLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tExpressionLoader);

    p3d->AddHandler(new(GMA_PERSISTENT) tVertexAnimKeyLoader);

    //ATCloader
    p3d->AddHandler(new(GMA_PERSISTENT) ATCLoader);

    //p3d->AddHandler(new p3d::tIgnoreLoader);


    tSEQFileHandler* sequencerFileHandler = new(GMA_PERSISTENT) tSEQFileHandler;
    p3d::loadManager->AddHandler(sequencerFileHandler, "seq");

       // sim lib
    sim::InstallSimLoaders();

    p3d->AddHandler(new(GMA_PERSISTENT) CameraDataLoader, SRR2::ChunkID::WALKERCAM );    
    p3d->AddHandler(new(GMA_PERSISTENT) CameraDataLoader, SRR2::ChunkID::FOLLOWCAM );    
    p3d->AddHandler(new(GMA_PERSISTENT) IntersectionLoader);    
    //p3d->AddHandler(new(GMA_PERSISTENT) RoadLoader);   
    p3d->AddHandler(new(GMA_PERSISTENT) RoadDataSegmentLoader);  
    p3d->AddHandler(new(GMA_PERSISTENT) CStatePropDataLoader);
MEMTRACK_POP_GROUP("PS2Platform");

    p3d::context->SetClearColour(tColour(0,0,0));
    p3d::pddi->Clear(PDDI_BUFFER_ALL);
    p3d::context->SwapBuffers();
    p3d::pddi->Clear(PDDI_BUFFER_ALL);
}


//==============================================================================
// PS2Platform::ShutdownPure3D
//==============================================================================
// Description: Clean up and shut down Pure3D.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void PS2Platform::ShutdownPure3D()
{
    //
    // Clean-up the Pure3D Inventory
    //
    p3d::inventory->RemoveAllElements();
    p3d::inventory->DeleteAllSections();

    //
    // Clean-up the space taken by the Pure 3D context.
    //
    if( mpContext != NULL )
    {
        mpPlatform->DestroyContext( mpContext );
        mpContext = NULL;
    }

    //
    // Clean-up the space taken by the Pure 3D platform.
    //
    if( mpPlatform != NULL )
    {
        tPlatform::Destroy( mpPlatform );
        mpPlatform = NULL;
    }
}

//==============================================================================
// PS2Platform::SetProgressiveMode
//==============================================================================
// Description: Reinitialize PDDI to change progressive scan mode (480p)
//
// Parameters:	boolean : true means go to 480p mode
//
// Return:      None.
//
//==============================================================================
void PS2Platform::SetProgressiveMode( bool progressiveScan )
{
    pddiDisplayInit init;

    init.xsize = WindowSizeX;

    //
    // Rendering to NTSC or PAL.
    //
#ifdef PAL    
    init.pal = true;
#else
    init.pal = false;
#endif

    init.dtv480 = progressiveScan;

    init.lockToVsync = false;

    p3d::display->InitDisplay(&init);

    mProgressiveMode = progressiveScan;
}

//==============================================================================
// PS2Platform::CheckForStartupButtons
//==============================================================================

bool PS2Platform::CheckForStartupButtons( void )
{
    unsigned char buffer[ 32 ];
    bool buttonsPushed = false;

    for ( int p = 0; p < 2; p++ )
    {
        for ( int s = 0; s < 4; s++ )
        {
            // spin until the system understands that there's a controller around.            
            int state = scePadStateExecCmd;
            do 
            {
                state = scePadGetState( p, s );
            } while( state != scePadStateDiscon && state != scePadStateFindCTP1 && 
                     state != scePadStateStable && state != scePadStateError );

            //
            // Now we have a controller.
            //
            if ( state == scePadStateFindCTP1 || state == scePadStateStable )
            {
                if ( scePadRead( p, s, buffer ) != 0 && // read success
                     buffer[ 0 ] == 0 &&                // buffer fill success
                     ( buffer[ 3 ] & (1<<6) ) == 0 &&   // X pushed
                     ( buffer [ 3 ] & (1<<4) ) == 0     // triangle pushed
                    )
                {
                    buttonsPushed = true;
                    break;
                }
            }
        }
        if ( buttonsPushed ) break;
    }

    return buttonsPushed;
}

void PS2Platform::OnControllerError(const char *msg)
{
    bool inFrame = p3d::context->InFrame();

    if ( inFrame ) p3d::context->EndFrame( true );
    DisplaySplashScreen( Error, msg, 0.7f, 0.0f, 0.0f, tColour(255, 255, 255), 0 );
    if ( inFrame ) p3d::context->BeginFrame( );
    mErrorState = CTL_ERROR;
    mPauseForError = true;

    if ( GetPresentationManager()->GetFMVPlayer()->IsPlaying() )
    {
        GetPresentationManager()->GetFMVPlayer()->Pause( );
    }
    else
    {
        GetSoundManager()->StopForMovie();
    }
}

//=============================================================================
// PS2Platform::OnDriveError
//=============================================================================
// Description: Comment
//
// Parameters:  ( radFileError error, const char* pDriveName, void* pUserData )
//
// Return:      bool 
//
//=============================================================================
bool PS2Platform::OnDriveError( radFileError error, const char* pDriveName, void* pUserData )
{
    bool inFrame = p3d::context->InFrame();

    const int NUM_RADFILE_ERRORS = 13;
    unsigned int errorIndex = error;

#ifdef PAL
    switch( CGuiTextBible::GetCurrentLanguage() )
    {
        case Scrooby::XL_FRENCH:
        {
            errorIndex += 1 * NUM_RADFILE_ERRORS;

            break;
        }
        case Scrooby::XL_GERMAN:
        {
            errorIndex += 2 * NUM_RADFILE_ERRORS;

            break;
        }
        case Scrooby::XL_SPANISH:
        {
            errorIndex += 3 * NUM_RADFILE_ERRORS;

            break;
        }
        default:
        {
            break;
        }
    }
#endif // PAL

    rAssert( errorIndex < sizeof( ERROR_STRINGS ) / sizeof( ERROR_STRINGS[ 0 ] ) );

    switch ( error )
    {
    case Success:
        {
            if ( mErrorState != NONE )
            {
                if ( inFrame ) p3d::context->EndFrame( true );
                DisplaySplashScreen( FadeToBlack );
                if ( inFrame ) p3d::context->BeginFrame( );
                mErrorState = NONE;
                mPauseForError = false;
            }

            if ( GetPresentationManager()->GetFMVPlayer()->IsPlaying() )
            {
                GetPresentationManager()->GetFMVPlayer()->UnPause( );
            }
            else
            {
                GetSoundManager()->ResumeAfterMovie();
            }
            return true;   
            break;
        }
    case FileNotFound:
        {
            if ( CommandLineOptions::Get( CLO_FILE_NOT_FOUND ) )
            {
                rAssert( pUserData != NULL );

                radFileRequest* request = static_cast<radFileRequest*>( pUserData );
                const char* fileName = request->GetFilename();

                //Get rid of the slashes.
                unsigned int i;
                unsigned int lastIndex = 0;
                for ( i = 0; i < strlen( fileName ); ++i )
                {
                    if ( fileName[ i ] == '\\' )
                    {
                        lastIndex = i;
                    }
                }

                unsigned int adjustedIndex = lastIndex == 0 ? lastIndex : lastIndex + 1;

                char adjustedName[32];
                strncpy( adjustedName, &fileName[adjustedIndex], ( strlen( fileName ) - lastIndex ) );
                adjustedName[ strlen( fileName ) - lastIndex ] = '\0';

                char errorString[256];
                sprintf( errorString, "%s:\n%s", ERROR_STRINGS[errorIndex], adjustedName );
                if ( inFrame ) p3d::context->EndFrame( true );
                DisplaySplashScreen( Error, errorString, 1.0f, 0.0f, 0.0f, tColour(255, 255, 255), 0 );
                if ( inFrame ) p3d::context->BeginFrame( );
                mErrorState = P_ERROR;
                mPauseForError = true;

                if ( GetPresentationManager()->GetFMVPlayer()->IsPlaying() )
                {
                    GetPresentationManager()->GetFMVPlayer()->Pause( );
                }
                else
                {
                    GetSoundManager()->StopForMovie();
                }
                return true;
            }
            else
            {
                //Hmmm...  This could be a hack.
                error = WrongMedia;
                //Fall through.
            }
        }
    case ShellOpen:
    case WrongMedia:
    case NoMedia:
    case HardwareFailure:
        {
            //This could be the wrong disc.
            if ( inFrame ) p3d::context->EndFrame( true );
            DisplaySplashScreen( Error, ERROR_STRINGS[errorIndex], 1.0f, 0.0f, 0.0f, tColour(255, 255, 255), 0 );
            if ( inFrame ) p3d::context->BeginFrame( );
            mErrorState = P_ERROR;
            mPauseForError = true;

            if ( GetPresentationManager()->GetFMVPlayer()->IsPlaying() )
            {
                GetPresentationManager()->GetFMVPlayer()->Pause( );
            }
            else
            {
                GetSoundManager()->StopForMovie();
            }
            return true;
        }
    default:
        {
            //Others are not supported.
            rAssert( false );
        }
    }

    return false;
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//==============================================================================
// PS2Platform::PS2Platform
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
PS2Platform::PS2Platform() :
    mpPlatform( NULL ),
    mpContext( NULL )
{
}


//==============================================================================
// PS2Platform::~PS2Platform
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
PS2Platform::~PS2Platform()
{
}


//==============================================================================
// PS2Platform::EnableSnProfiler
//==============================================================================
// Description: Prepare the SN function level profiler.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void PS2Platform::EnableSnProfiler()
{
#ifndef RAD_RELEASE
#ifndef RAD_MW

    //
    // Quadword aligned, can be 2K to 64K bytes
    //
    static u_long128 profdata[2048];
	
    snDebugInit();
	
    sceSifInitRpc( 0 );
	
    //
    // Load the SNProfile module
    //
	if( sceSifLoadModule( "host0:/usr/local/sce/iop/modules/SNProfil.irx", 0, NULL ) < 0 )
	{
	    rDebugString( "Can't load SNProfil.IRX module\n" );
		exit( -1 );
	}

	//
    // Initialize the profiler
    //
    if( snProfInit( _4KHZ, profdata, sizeof(profdata) ) != 0 )
    {
	    //
        // See SN_PRF... in LIBSN.H
        //
        rDebugString( "SN Profiler init failed\n" ); 
    }

    snProfSetFlagValue( 1 );

#endif
#endif
}

#ifndef FINAL

//
// Exception-handling functions.  These functions are set to be called
// whenever the PS2 crashes on a variety of conditions.
//

void PS2Platform::handleTLBChange( unsigned int stat,
                                   unsigned int cause,
                                   unsigned int epc,
                                   unsigned int bva,
                                   unsigned int bpa,
                                   u_long128* registers )
{
    dumpExceptionData( "TLB Change Error", stat, cause, epc, bva, bpa, registers );
}

void PS2Platform::handleTLBLoadMismatch( unsigned int stat,
                                         unsigned int cause,
                                         unsigned int epc,
                                         unsigned int bva,
                                         unsigned int bpa,
                                         u_long128* registers )
{
    dumpExceptionData( "TLB Load Error", stat, cause, epc, bva, bpa, registers );
}

void PS2Platform::handleTLBStoreMismatch( unsigned int stat,
                                          unsigned int cause,
                                          unsigned int epc,
                                          unsigned int bva,
                                          unsigned int bpa,
                                          u_long128* registers )
{
    dumpExceptionData( "TLB Store Error", stat, cause, epc, bva, bpa, registers );
}

void PS2Platform::handleAddressLoadError( unsigned int stat,
                                          unsigned int cause,
                                          unsigned int epc,
                                          unsigned int bva,
                                          unsigned int bpa,
                                          u_long128* registers )
{
    dumpExceptionData( "Address Load Error", stat, cause, epc, bva, bpa, registers );
}

void PS2Platform::handleAddressStoreError( unsigned int stat,
                                           unsigned int cause,
                                           unsigned int epc,
                                           unsigned int bva,
                                           unsigned int bpa,
                                           u_long128* registers )
{
    dumpExceptionData( "Address Store Error", stat, cause, epc, bva, bpa, registers );
}

void PS2Platform::handleBusFetchError( unsigned int stat,
                                       unsigned int cause,
                                       unsigned int epc,
                                       unsigned int bva,
                                       unsigned int bpa,
                                       u_long128* registers )
{
    dumpExceptionData( "Bus Fetch Error", stat, cause, epc, bva, bpa, registers );
}

void PS2Platform::handleBusDataError( unsigned int stat,
                                      unsigned int cause,
                                      unsigned int epc,
                                      unsigned int bva,
                                      unsigned int bpa,
                                      u_long128* registers )
{
    dumpExceptionData( "Bus Data Error", stat, cause, epc, bva, bpa, registers );
}

void PS2Platform::handleReservedInstruction( unsigned int stat,
                                             unsigned int cause,
                                             unsigned int epc,
                                             unsigned int bva,
                                             unsigned int bpa,
                                             u_long128* registers )
{
    dumpExceptionData( "Reserved Instruction Error", stat, cause, epc, bva, bpa, registers );
}

void PS2Platform::handleCoprocessor( unsigned int stat,
                                     unsigned int cause,
                                     unsigned int epc,
                                     unsigned int bva,
                                     unsigned int bpa,
                                     u_long128* registers )
{
    dumpExceptionData( "Coprocessor Error", stat, cause, epc, bva, bpa, registers );
}

void PS2Platform::handleOverflow( unsigned int stat,
                                  unsigned int cause,
                                  unsigned int epc,
                                  unsigned int bva,
                                  unsigned int bpa,
                                  u_long128* registers )
{
    dumpExceptionData( "Overflow Error", stat, cause, epc, bva, bpa, registers );
}

void PS2Platform::handleTrap( unsigned int stat,
                              unsigned int cause,
                              unsigned int epc,
                              unsigned int bva,
                              unsigned int bpa,
                              u_long128* registers )
{
    dumpExceptionData( "Trap Error", stat, cause, epc, bva, bpa, registers );
}

//=============================================================================
// PS2Platform::dumpExceptionData
//=============================================================================
// Description: Dump the parameters from an exception handler to the screen
//
// Parameters:	exceptionName - string describing the exception that was set
//              stat - status register
//              cause - cause register? (I should look this up)
//              epc - program counter
//              bva - address involved in bad access or branch
//              bpa - address involved in bus error
//              registers - array of MIPS general-purpose registers
//
// Return:      Why?
//
//==============================================================================
void PS2Platform::dumpExceptionData( const char* exceptionName,
                                     unsigned int stat,
                                     unsigned int cause,
                                     unsigned int epc,
                                     unsigned int bva,
                                     unsigned int bpa,
                                     u_long128* registers )
{
    IRadTextDisplay* textDisplay;
    char buffer[50];
    int i;
    unsigned int reg1, reg2;

#ifdef RAD_PS2
    //
    // Need to shut down the MFIFO for this to work properly
    //
    ((pddiExtPS2Control*)p3d::pddi->GetExtension(PDDI_EXT_PS2_CONTROL))->MFIFOEnable( false );
#endif

    ::radTextDisplayGet( &textDisplay, GMA_DEFAULT );

    textDisplay->SetBackgroundColor( 0 );
    textDisplay->SetTextColor( 0xffffffff );
    textDisplay->Clear();
    textDisplay->TextOutAt( exceptionName, 15, 1 );
    sprintf( buffer, "PC: 0x%x  Address Value: 0x%x", epc, bva );
    textDisplay->TextOutAt( buffer, 15, 3 );
    sprintf( buffer, "stat: 0x%x  cause: 0x%x", stat, cause );
    textDisplay->TextOutAt( buffer, 15, 5 );
    sprintf( buffer, "Bus error address: 0x%x", bpa );
    textDisplay->TextOutAt( buffer, 15, 7 );

    //
    // GPR printout
    //
    for( i = 0; i < 16; i++ )
    {
        reg1 = (unsigned int)registers[i] & 0xffffffff;
        reg2 = (unsigned int)registers[i+16] & 0xffffffff;
        sprintf( buffer, "GPR%02d: 0x%08x  GPR%02d: 0x%08x", i, reg1, i+16, reg2 );
        textDisplay->TextOutAt( buffer, 15, 9+i );
    }

    textDisplay->TextOutAt( ":-(", 6, 13 );

    if ( CommandLineOptions::Get( CLO_DEMO_TEST ) ||
         GetCheatInputSystem()->IsCheatEnabled( CHEAT_ID_DEMO_TEST ) )
    {
        char buffy[32];
        sprintf( buffy, "Demo Count: %d", GetGame()->GetDemoCount() );
        textDisplay->TextOutAt( buffy, 6, 15 );

        unsigned int time = GetGame()->GetTime();
        unsigned int hours = time / 3600000;
        unsigned int deltaTime = time % 3600000;

        unsigned int minutes = deltaTime / 60000;
        deltaTime = deltaTime % 60000;

        unsigned int seconds = deltaTime / 1000;
        deltaTime = deltaTime % 1000;
        sprintf( buffy, "Time: %d:%d:%d.%d", hours, minutes, seconds, deltaTime );
        textDisplay->TextOutAt( buffy, 6, 17 );

        if ( GetGameplayManager() )
        {
            sprintf( buffy, "Level %d", GetGameplayManager()->GetCurrentLevelIndex() );
            textDisplay->TextOutAt( buffy, 6, 19 );
        }
    }

    textDisplay->SwapBuffers();
    textDisplay->Release();

    rReleaseBreak();
}

void Simpsons2MFIFODisable()
{
#ifdef RAD_PS2
    ((pddiExtPS2Control*)p3d::pddi->GetExtension(PDDI_EXT_PS2_CONTROL))->MFIFOEnable( false );
#endif
}

#endif
