//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   Win32Platform   
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
// Standard Lib
#include <stdlib.h>
#include <string.h>
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
#include <loading/roaddatasegmentloader.h>
#include <render/Loaders/BillboardWrappedLoader.h>
#include <render/Loaders/instparticlesystemloader.h>
#include <render/Loaders/breakableobjectloader.h>
#include <render/Loaders/AnimDynaPhysLoader.h>
#include <render/Loaders/lensflareloader.h>
#include <p3d/shadow.hpp>
#include <p3d/anim/animatedobject.hpp>
#include <p3d/effects/particleloader.hpp>
#include <p3d/effects/opticloader.hpp>
#include <p3d/anim/vertexanimkey.hpp>
#include <stateprop/statepropdata.hpp>

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

//This is so we can get the name of the file that's failing.
#include <../src/radfile/common/requests.hpp>

// sim - for InstallSimLoaders
#include <simcommon/simutility.hpp>

//========================================
// Project Includes
//========================================
#include <input/inputmanager.h>
#include <main/win32platform.h>
#include <main/commandlineoptions.h>
#include <main/game.h>
#include <render/RenderManager/RenderManager.h>
#include <render/RenderFlow/renderflow.h>
#include <render/Loaders/AllWrappers.h>
#include <memory/srrmemory.h>

#include <loading/locatorloader.h>
#include <loading/cameradataloader.h>
#include <loading/roadloader.h>
#include <loading/pathloader.h>
#include <loading/intersectionloader.h>
#include <loading/roaddatasegmentloader.h>
#include <atc/atcloader.h>
#include <data/gamedatamanager.h>
#include <data/config/gameconfigmanager.h>
#include <debug/debuginfo.h>
#include <constants/srrchunks.h>
#include <gameflow/gameflow.h>
#include <sound/soundmanager.h>
#include <presentation/presentation.h>
#include <presentation/gui/guitextbible.h>
#include <cheats/cheatinputsystem.h>
#include <mission/gameplaymanager.h>




#include <radload/radload.hpp>

#include <main/errorswin32.h>

#define WIN32_SECTION "WIN32_SECTION"
#define TIMER_LEAVE 1

//#define PRINT_WINMESSAGES

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

// Static pointer to instance of singleton.
Win32Platform* Win32Platform::spInstance = NULL;

// Other static members.
HINSTANCE Win32Platform::mhInstance = NULL;
HWND Win32Platform::mhWnd = NULL;
HANDLE Win32Platform::mhMutex = NULL;
bool Win32Platform::mShowCursor = true;

//The Adlib font.  <sigh>
unsigned char gFont[] = 
#include <font/defaultfont.h>

//
// Define the starting resolution.
//
static const Win32Platform::Resolution StartingResolution = Win32Platform::Res_800x600;
static const int StartingBPP = 32;

// This specifies the PDDI DLL to use.  We are using directx8.
#ifdef RAD_DEBUG
static const char d3dLibraryName[] = "pddidx8d.dll";
#endif
#ifdef RAD_TUNE
static const char d3dLibraryName[] = "pddidx8t.dll";
#endif
#ifdef RAD_RELEASE
static const char d3dLibraryName[] = "pddidx8r.dll";
#endif

// Name of the application.  This is the string that appears in the Window's
// title bar.
static const char ApplicationName[] = "The Simpsons: Hit & Run";

// The window style
static const DWORD WndStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

// The gamma of the desktop.. needed to reset it on alt-tabs.
static WORD DesktopGammaRamp[ 3 ][ 256 ] = { 0 };

void LoadMemP3DFile( unsigned char* buffer, unsigned int size, tEntityStore* store )
{
    tFileMem* file = new tFileMem(buffer,size);
    file->AddRef();
    file->SetFilename("memfile.p3d");
    p3d::loadManager->GetP3DHandler()->Load( file, p3d::inventory );
    file->Release();
}

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// Win32Platform::CreateInstance
//==============================================================================
//
// Description: Creates the Win32Platform.
//
// Parameters:	win32 parameters.
//
// Return:      Pointer to the Win32Platform.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
Win32Platform* Win32Platform::CreateInstance( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
MEMTRACK_PUSH_GROUP( "Win32Platform" );
    rAssert( spInstance == NULL );

    spInstance = new(GMA_PERSISTENT) Win32Platform( hInstance, hPrevInstance, lpCmdLine, nCmdShow );
    rAssert( spInstance );
MEMTRACK_POP_GROUP( "Win32Platform" );

    return spInstance;
}

//==============================================================================
// Win32Platform::GetInstance
//==============================================================================
//
// Description: - Access point for the Win32Platform singleton.  
//
// Parameters:	None.
//
// Return:      Pointer to the Win32Platform.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
Win32Platform* Win32Platform::GetInstance()
{
    rAssert( spInstance != NULL );

    return spInstance;
}


//==============================================================================
// Win32Platform::DestroyInstance
//==============================================================================
//
// Description: Destroy the Win32Platform.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void Win32Platform::DestroyInstance()
{
    rAssert( spInstance != NULL );

    delete( GMA_PERSISTENT, spInstance );
    spInstance = NULL;
}



//==============================================================================
// Win32Platform::InitializeWindow
//==============================================================================
// Description: Creates the window class and window instance for the application.
//              We must do this before initializing the platform.
//
// Parameters:	hInstance - the handle to the instance.
//
// Return:      true if successful and the program can run.
//              false if another simpsons window already exists and this
//                instance should terminate.
//
// Constraints: Must be initialized before the platform.
//
//==============================================================================
bool Win32Platform::InitializeWindow( HINSTANCE hInstance ) 
{
    // check to see if another instance is running...
    mhMutex = CreateMutex(NULL, 0, ApplicationName);
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        // simpsons is already running, so lets find the window and give it focus
        HWND hwnd = FindWindow(ApplicationName, NULL);
        if (hwnd != NULL)
        {
            // if window is minimized, restore it
            WINDOWPLACEMENT wndpl;
            if (GetWindowPlacement(hwnd, &wndpl) != 0)
            {
                if ((wndpl.showCmd == SW_MINIMIZE) ||
                    (wndpl.showCmd == SW_SHOWMINIMIZED))
                {
                    ShowWindow(hwnd, SW_RESTORE);
                }
            }

            // activate the window
            SetForegroundWindow(hwnd);

            return false;
        }
    }

    mhInstance = hInstance;

    // Create and resigter an object that defines the window that we will
    // run our game in.
    WNDCLASS Wndclass;
    Wndclass.style = CS_HREDRAW | CS_VREDRAW;
    Wndclass.lpfnWndProc = WndProc;
    Wndclass.cbClsExtra = 0;
    Wndclass.cbWndExtra = 0;
    Wndclass.hInstance = mhInstance;
    Wndclass.hIcon = LoadIcon( mhInstance, "IDI_SIMPSONSICON" );
    Wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    Wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    Wndclass.lpszMenuName= NULL;
    Wndclass.lpszClassName = ApplicationName;
    ::RegisterClass(&Wndclass);

    // Set up the window.
    int x, y;
    TranslateResolution( StartingResolution, x, y );

    RECT clientRect;
    clientRect.left = 0;
    clientRect.top = 0;
    clientRect.right = x;
    clientRect.bottom = y;

    // for windowed mode, center the screen
    int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    while (nScreenWidth > 1600 ) // probably multimonitor
    {
        nScreenWidth /= 2;
    }

    // centre the window
    clientRect.left = (nScreenWidth-x)/2;
    clientRect.top  = (nScreenHeight-y)/2;
    clientRect.right += clientRect.left;
    clientRect.bottom += clientRect.top;

    AdjustWindowRect(&clientRect,WndStyle,FALSE);

    // Create the game's main window.
    mhWnd = ::CreateWindow(ApplicationName,
        ApplicationName,
        WndStyle,
        clientRect.left,
        clientRect.top,
        clientRect.right-clientRect.left, 
        clientRect.bottom-clientRect.top,
        NULL,
        NULL,
        mhInstance,
        NULL);

    rAssert(mhWnd != NULL);

    ShowTheCursor( false );

    return true;
}

//==============================================================================
// Win32Platform::InitializeFoundation
//==============================================================================
// Description: FTech must be setup first so that all the memory services
//              are ready to go before we begin allocating anything.
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: The FTech systems must be initialized in a particular order.
//              Consult their documentation before changing.
//
//==============================================================================
void Win32Platform::InitializeFoundation() 
{
    //
    // Initialize the memory heaps
    // obsolete now.. the heaps initialize memory.
    //
    //InitializeMemory();

    //
    // Register an out-of-memory display handler in case something goes bad
    // while allocating the heaps
    //
    ::radMemorySetOutOfMemoryCallback( PrintOutOfMemoryMessage, NULL );

    //
    // Initialize memory monitor by JamesCo. TM.
    //
    if( CommandLineOptions::Get( CLO_MEMORY_MONITOR ) )
    {
        const int KB = 1024;
        ::radMemoryMonitorInitialize( 64 * KB, GMA_DEBUG );
    }

    // Setup the memory heaps
    //
    HeapMgr()->PrepareHeapsStartup ();

    // Seed the heap stack
    //
    HeapMgr()->PushHeap (GMA_PERSISTENT);

    //
    // Initilalize the platform system
    // 
    ::radPlatformInitialize( mhWnd, mhInstance, 0 );

    //
    // Initialize the timer system
    //
    ::radTimeInitialize();

    //
    // Initialize the debug communication system.
    //
    ::radDbgComTargetInitialize( WinSocket, 
        radDbgComDefaultPort, // Default
        NULL,                 // Default
        GMA_DEBUG );


    //
    // Initialize the Watcher.
    //
    ::radDbgWatchInitialize( "SRR2",
                             32 * 16384, // 2 * Default
                             GMA_DEBUG );

    //
    // Initialize the file system.
    //
    ::radFileInitialize( 50, // Default
        32, // Default
        GMA_PERSISTENT );

    ::radLoadInitialize();
    //radLoad->SetSyncLoading( true );	

    ::radDriveMount( NULL, GMA_PERSISTENT);

    //
    // Initialize the new movie player
    //
    ::radMovieInitialize2( GMA_PERSISTENT );

    HeapMgr()->PopHeap (GMA_PERSISTENT);
}

//==============================================================================
// Win32Platform::InitializeMemory
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Win32Platform::InitializeMemory()
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
    // Initialize the thread system.
    //
    ::radThreadInitialize();

    //
    // Initialize the memory system.
    //
    ::radMemoryInitialize();
}

//==============================================================================
// Win32Platform::ShutdownMemory
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Win32Platform::ShutdownMemory()
{
    if( gMemorySystemInitialized )
    {
        gMemorySystemInitialized = false;

        // No shutdown the memory.  This leads to bad errors when destroying 
        // static variables sprinkled here and there.
        //::radMemoryTerminate();
        
        ::radThreadTerminate();
    }
}

//==============================================================================
// Win32Platform::InitializePlatform
//==============================================================================
// Description: Get the Win32 ready to go.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void Win32Platform::InitializePlatform() 
{
    HeapMgr()->PushHeap (GMA_PERSISTENT);

    //
    // Rendering is good.
    //
    InitializePure3D();

    //
    // Add anything here that needs to be before the drive is opened.
    //
    DisplaySplashScreen( Error ); // blank screen

    //
    // Show the window on the screen.  Must be done before initializing the input manager.
    //
    ShowWindow( mhWnd, mFullscreen ? SW_SHOWMAXIMIZED : SW_SHOW );

    //
    // Opening the drive is SLOW...
    //
    InitializeFoundationDrive();    

    //
    // Initialize the controller.
    //
    GetInputManager()->Init();

    //
    // Register with the game config manager
    //
    GetGameConfigManager()->RegisterConfig( this );

    HeapMgr()->PopHeap (GMA_PERSISTENT);
}


//==============================================================================
// Win32Platform::ShutdownPlatform
//==============================================================================
// Description: Shut down the PS2.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void Win32Platform::ShutdownPlatform()
{
    ShutdownPure3D();
    ShutdownFoundation();
}

//=============================================================================
// Win32Platform::LaunchDashboard
//=============================================================================
// Description: We use this a the emergency exit from the game if we arent in a context that suppose the transition 
//                    to the CONTEXT_EXIT  
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Win32Platform::LaunchDashboard()
{   

    {
        //chuck I copied and pasted from the other platform's implementations

        GetLoadingManager()->CancelPendingRequests();
           //TODO: Make sure sounds shut down too.
        GetSoundManager()->SetMasterVolume( 0.0f );

       // DisplaySplashScreen( FadeToBlack );

        GetPresentationManager()->StopAll();

        //Shouldn't need to do this since, this singleton and the others should get destroyed once we 
        //retrun the main loop
        //GameDataManager::DestroyInstance();  //Get rid of memcards

        p3d::loadManager->CancelAll();

        GetSoundManager()->StopForMovie();

        //Shouldnt need the early destruction of this singleton either
        //SoundManager::DestroyInstance();
        
        //Dont want to shutdown platform early either.
        //ShutdownPlatform();
        //rAssertMsg( false, "Doesn't make sense for win32." );
    }
}

//=============================================================================
// Win32Platform::ResetMachine
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void Win32Platform::ResetMachine()
{
    rAssertMsg( false, "Doesn't make sense for win32." );
}

//=============================================================================
// Win32Platform::DisplaySplashScreen
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
void Win32Platform::DisplaySplashScreen( SplashScreen screenID, 
                                       const char* overlayText, 
                                       float fontScale, 
                                       float textPosX, 
                                       float textPosY,
                                       tColour textColour,
                                       int fadeFrames )
{
    HeapMgr()->PushHeap( GMA_TEMP );

    p3d::inventory->PushSection();
    p3d::inventory->AddSection( WIN32_SECTION );
    p3d::inventory->SelectSection( WIN32_SECTION );

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

    //Should do this after a vsync.
    thisFont->Release();

    p3d::inventory->RemoveSectionElements(WIN32_SECTION);
    p3d::inventory->DeleteSection(WIN32_SECTION);
    p3d::inventory->PopSection();

    HeapMgr()->PopHeap( GMA_TEMP );
}


//=============================================================================
// Win32Platform::DisplaySplashScreen
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
void Win32Platform::DisplaySplashScreen( const char* textureName,
                                       const char* overlayText, 
                                       float fontScale, 
                                       float textPosX, 
                                       float textPosY, 
                                       tColour textColour,
                                       int fadeFrames )
{
}

void Win32Platform::OnControllerError(const char *msg)
{
    DisplaySplashScreen( Error, msg, 0.7f, 0.0f, 0.0f, tColour(255, 255, 255), 0 );
    mErrorState = CTL_ERROR;
    mPauseForError = true;

}


//=============================================================================
// Win32Platform::OnDriveError
//=============================================================================
// Description: Comment
//
// Parameters:  ( radFileError error, const char* pDriveName, void* pUserData )
//
// Return:      bool 
//
//=============================================================================
bool Win32Platform::OnDriveError( radFileError error, const char* pDriveName, void* pUserData )
{
    // First check if the error is related to loading/saving games.
    // We do this here because windows has one drive for all operations.
    // If the game data manager is using the drive, it handles the error.
    GameDataManager* gm = GetGameDataManager();
    if( gm->IsUsingDrive() )
    {
        return gm->OnDriveError( error, pDriveName, pUserData );
    }

    switch ( error )
    {
    case Success:
        {
            if ( mErrorState != NONE )
            {
                DisplaySplashScreen( FadeToBlack );
                mErrorState = NONE;
                mPauseForError = false;
            }

            return true;
            break;
        }
    case FileNotFound:
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

            if( strcmp( fileName, GameConfigManager::ConfigFilename ) == 0 )
            {
                return false;
            }

            char errorString[256];
            sprintf( errorString, "%s:\n%s", ERROR_STRINGS[error], adjustedName );
            DisplaySplashScreen( Error, errorString, 1.0f, 0.0f, 0.0f, tColour(255, 255, 255), 0 );
            mErrorState = P_ERROR;
            mPauseForError = true;

            return true;
        }
    case NoMedia:
    case MediaNotFormatted:
    case MediaCorrupt:
    case NoFreeSpace:
    case HardwareFailure:
        {
            //This could be the wrong disc.
            DisplaySplashScreen( Error, ERROR_STRINGS[error], 1.0f, 0.0f, 0.0f, tColour(255, 255, 255), 0 );
            mErrorState = P_ERROR;
            mPauseForError = true;

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

//=============================================================================
// Win32Platform::SetResolution
//=============================================================================
// Description: Sets the screen resolution
//
// Parameters:  res - desired resolution
//
// Returns:     true if successful
//              false if not supported
//
// Notes:
//=============================================================================

bool Win32Platform::SetResolution( Resolution res, int bpp, bool fullscreen )
{
    // Check if resolution is supported.
    if( !IsResolutionSupported( res, bpp ) )
    {
        return false;
    }

    // Set up the new properties
    mResolution = res;
    mbpp = bpp;
    mFullscreen = fullscreen;

    // Reinitialize the d3d context.
    InitializeContext();

    // Resize the window for the new resolution
    ResizeWindow();

    return true;
}

//=============================================================================
// Win32Platform::GetResolution
//=============================================================================
// Description: Returns the current resolution
//
// Parameters:  n/a
//
// Returns:     resolution
//
// Notes:
//=============================================================================

Win32Platform::Resolution Win32Platform::GetResolution() const
{
    return mResolution;
}

//=============================================================================
// Win32Platform::GetBPP
//=============================================================================
// Description: Returns the current bit depth.
//
// Parameters:  n/a
//
// Returns:     bit depth
//
// Notes:
//=============================================================================

int Win32Platform::GetBPP() const
{
    return mbpp;
}

//=============================================================================
// Win32Platform::IsFullscreen
//=============================================================================
// Description: Returns true if currently in full screen mode
//
// Parameters:  n/a
//
// Returns:     true if in full screen, false if in window
//
// Notes:
//=============================================================================

bool Win32Platform::IsFullscreen() const
{
    return mFullscreen;
}

//=============================================================================
// Win32Platform::GetConfigName
//=============================================================================
// Description: Returns the name of the win32 platform's config
//
// Parameters:  n/a
//
// Returns:     
//
// Notes:
//=============================================================================

const char* Win32Platform::GetConfigName() const
{
    return "System";
}

//=============================================================================
// Win32Platform::GetNumProperties
//=============================================================================
// Description: Returns the number of config properties
//
// Parameters:  n/a
//
// Returns:     
//
// Notes:
//=============================================================================

int Win32Platform::GetNumProperties() const
{
    return 4;
}

//=============================================================================
// Win32Platform::LoadDefaults
//=============================================================================
// Description: Loads the default configuration for the system.
//
// Parameters:  n/a
//
// Returns:     
//
// Notes:
//=============================================================================

void Win32Platform::LoadDefaults()
{
#ifdef RAD_DEBUG
    SetResolution( StartingResolution, StartingBPP, !CommandLineOptions::Get( CLO_WINDOW_MODE ) );
#else
    SetResolution( StartingResolution, StartingBPP, true );
#endif
    

    GetRenderFlow()->SetGamma( 1.0f );
}

//=============================================================================
// Win32Platform::LoadConfig
//=============================================================================
// Description: Loads the platforms configuration
//
// Parameters:  n/a
//
// Returns:     
//
// Notes:
//=============================================================================

void Win32Platform::LoadConfig( ConfigString& config )
{
    char property[ ConfigString::MaxLength ];
    char value[ ConfigString::MaxLength ];

    while ( config.ReadProperty( property, value ) )
    {
        if( _stricmp( property, "display" ) == 0 )
        {
            if( _stricmp( value, "window" ) == 0 )
            {
                mFullscreen = false;
            }
            else if( _stricmp( value, "fullscreen" ) == 0 )
            {
                mFullscreen = true;
            }
        }
        else if( _stricmp( property, "resolution" ) == 0 )
        {
            if( strcmp( value, "640x480" ) == 0 )
            {
                mResolution = Res_640x480;
            }
            else if( strcmp( value, "800x600" ) == 0 )
            {
                mResolution = Res_800x600;
            }
            else if( strcmp( value, "1024x768" ) == 0 )
            {
                mResolution = Res_1024x768;
            }
            else if( strcmp( value, "1152x864" ) == 0 )
            {
                mResolution = Res_1152x864;
            }
            else if( strcmp( value, "1280x1024" ) == 0 )
            {
                mResolution = Res_1280x1024;
            }
            else if( strcmp( value, "1600x1200" ) == 0 )
            {
                mResolution = Res_1600x1200;
            }
        }
        else if( _stricmp( property, "bpp" ) == 0 )
        {
            if( strcmp( value, "16" ) == 0 )
            {
                mbpp = 16;
            }
            else if( strcmp( value, "32" ) == 0 )
            {
                mbpp = 32;
            }
        }
        else if( _stricmp( property, "gamma" ) == 0 )
        {
            float val = (float) atof( value );
            if( val > 0 )
            {
                GetRenderFlow()->SetGamma( val );
            }
        }
    }

    // apply the new settings.
    SetResolution( mResolution, mbpp, mFullscreen );
}

//=============================================================================
// Win32Platform::SaveConfig
//=============================================================================
// Description: Saves the system configuration to the config string.
//
// Parameters:  config string to save to
//
// Returns:     
//
// Notes:
//=============================================================================

void Win32Platform::SaveConfig( ConfigString& config )
{
    config.WriteProperty( "display", mFullscreen ? "fullscreen" : "window" );

    const char* res = "800x600";
    switch( mResolution )
    {
        case Res_640x480:
        {
            res = "640x480";
            break;
        }
        case Res_800x600:
        {
            res = "800x600";
            break;
        }
        case Res_1024x768:
        {
            res = "1024x768";
            break;
        }
        case Res_1152x864:
        {
            res = "1152x864";
            break;
        }
        case Res_1280x1024:
        {
            res = "1280x1024";
            break;
        }
        case Res_1600x1200:
        {
            res = "1600x1200";
            break;
        }
        default:
        {
            rAssert( false );
        }
    }

    config.WriteProperty( "resolution", res );

    config.WriteProperty( "bpp", mbpp == 16 ? "16" : "32" );

    char gamma[20];
    sprintf( gamma, "%f", GetRenderFlow()->GetGamma() );
    config.WriteProperty( "gamma", gamma );
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//==============================================================================
// Win32Platform::Win32Platform
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
Win32Platform::Win32Platform( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) :
    mpPlatform( NULL ),
    mpContext( NULL ),
    mResolution( StartingResolution ),
    mbpp( StartingBPP )
{
    mhInstance = hInstance;
    mFullscreen = false;

    mScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    mScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    while (mScreenWidth > 1600 ) // probably multimonitor
    {
        mScreenWidth /= 2;
    }
}


//==============================================================================
// Win32Platform::~Win32Platform
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
Win32Platform::~Win32Platform()
{
    HeapManager::DestroyInstance();

    CloseHandle( mhMutex );
}

//==============================================================================
// Win32Platform::InitializeFoundationDrive
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
void Win32Platform::InitializeFoundationDrive() 
{
    //
    // Get the default drive and hold it open for the life of the game.
    // This is a costly operation so we only want to do it once.
    //

    char defaultDrive[ radFileDrivenameMax + 1 ];

    ::radGetDefaultDrive( defaultDrive );

    ::radDriveOpenSync( &mpIRadDrive, 
                        defaultDrive,
                        NormalPriority, // Default
                        GMA_PERSISTENT );

    rAssert( mpIRadDrive != NULL );

    mpIRadDrive->RegisterErrorHandler( this, NULL );
}


//==============================================================================
// Win32Platform::ShutdownFoundation
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
void Win32Platform::ShutdownFoundation()
{
    //
    // Release the drive we've held open since the begining.
    //
    mpIRadDrive->Release();
    mpIRadDrive = NULL;

    //
    // Shutdown the systems in the reverse order.
    //
    ::radMovieTerminate2();
    ::radDriveUnmount( NULL );
    ::radLoadTerminate();
    ::radFileTerminate();
    ::radDbgWatchTerminate();
    if( CommandLineOptions::Get( CLO_MEMORY_MONITOR ) )
    {
        ::radMemoryMonitorTerminate();
    }
    ::radDbgComTargetTerminate();
    ::radTimeTerminate();
    ::radPlatformTerminate();
}


//==============================================================================
// Win32Platform::InitializePure3D
//==============================================================================
// Description: Get Pure3D ready to go.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void Win32Platform::InitializePure3D() 
{
MEMTRACK_PUSH_GROUP( "Win32Platform" );
    //    p3d::SetMemAllocator( p3d::ALLOC_DEFAULT, GMA_PERSISTENT );
    //    p3d::SetMemAllocator( p3d::ALLOC_LOADED, GMA_LEVEL );

    //
    // Initialise Pure3D platform object.
    // This call differs between different platforms.  The Win32 version,
    // for example requires the application instance to be passed in.
    //
    mpPlatform = tPlatform::Create( mhInstance );
    rAssert( mpPlatform != NULL );

    //
    // Initialize the d3d context.
    //
    InitializeContext();

    //
    // This call installs chunk handlers for all the primary chunk types that
    // Pure3D supports.  This includes textures, materials, geometries, and the
    // like.
    //
    //    p3d::InstallDefaultLoaders();
    P3DASSERT(p3d::context);
    tP3DFileHandler* p3d = new(GMA_PERSISTENT) tP3DFileHandler;
    //    p3d::loadManager->AddHandler(p3d, "p3d");
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
    p3d->AddHandler(new(GMA_PERSISTENT) tVertexAnimKeyLoader);
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

    //ATCloader, hope this doesnt blow up
    p3d->AddHandler(new(GMA_PERSISTENT) ATCLoader);

    //p3d->AddHandler(new p3d::tIgnoreLoader);

    tSEQFileHandler* sequencerFileHandler = new(GMA_PERSISTENT) tSEQFileHandler;
    p3d::loadManager->AddHandler(sequencerFileHandler, "seq");

    // sim lib
    sim::InstallSimLoaders();

    p3d->AddHandler(new(GMA_PERSISTENT) CameraDataLoader, SRR2::ChunkID::FOLLOWCAM);    
    p3d->AddHandler(new(GMA_PERSISTENT) CameraDataLoader, SRR2::ChunkID::WALKERCAM);    
    p3d->AddHandler(new(GMA_PERSISTENT) IntersectionLoader);    
    //p3d->AddHandler(new(GMA_PERSISTENT) RoadLoader);    
    p3d->AddHandler(new(GMA_PERSISTENT) RoadDataSegmentLoader);    
    p3d->AddHandler(new(GMA_PERSISTENT) CStatePropDataLoader);
MEMTRACK_POP_GROUP( "Win32Platform" );
}


//==============================================================================
// Win32Platform::ShutdownPure3D
//==============================================================================
// Description: Clean up and shut down Pure3D.
//
// Parameters:	None.
// 
// Return:      None.
//
//==============================================================================
void Win32Platform::ShutdownPure3D()
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
// Win32Platform::InitializeContext
//==============================================================================
// Description: Initializes the d3d context for this application according to
//              the class' display settings - resolution, bpp, fullscreen.
//
// Parameters:	n/a
//
// Return:      n/a
//
//==============================================================================

void Win32Platform::InitializeContext()
{
    tContextInitData init;

    //
    // This is the window we want to render into.
    //
    init.hwnd = mhWnd;

    //
    // Set the fullscreen/window mode.
    //
    init.displayMode = mFullscreen ? PDDI_DISPLAY_FULLSCREEN : PDDI_DISPLAY_WINDOW;

    //
    // This the name of the PDDI we will be using for rendering
    //
    strncpy(init.PDDIlib, d3dLibraryName, 128);

    //
    // All applications should supply PDDI_BUFFER_COLOUR.  PDDI_BUFFER_DEPTH
    // specifies that we also want to allocate a Z-buffer.
    //
    init.bufferMask = PDDI_BUFFER_COLOUR | PDDI_BUFFER_DEPTH;
    init.enableSnapshot = false;

    //
    // These values only take effect in fullscreen mode.  In windowed mode, the
    // dimensions of the window define the rendering area.  We'll define them
    // anyway for completeness sake.
    //
    TranslateResolution( mResolution, init.xsize, init.ysize );

    //
    // Depth of the rendering buffer.  Again, this value only works in
    // fullscreen mode.  In window mode, the depth of the desktop is used.
    // This value should be either 16 or 32.
    //
    init.bpp = mbpp;

    init.lockToVsync = false;

    if( mpContext == NULL )
    {
        // Create the context
        mpContext = mpPlatform->CreateContext( &init );
        rAssert( mpContext != NULL );

        //
        // Assign this context to the platform.
        //
        mpPlatform->SetActiveContext( mpContext );
        p3d::pddi->EnableZBuffer( true );
    }
    else
    {
        // Update the display settings.
        mpContext->GetDisplay()->InitDisplay( &init );
    }
}

//==============================================================================
// Win32Platform::TranslateResolution
//==============================================================================
// Description: translates resolution enums to x and y
//
// Parameters:	resolution - the res enum
//              x - corresponding width
//              y - corresponding height
//
// Return:      N/A.
//
//==============================================================================

void Win32Platform::TranslateResolution( Resolution res, int&x, int&y )
{
    switch( res )
    {
        case Res_640x480:
        {
            x = 640;
            y = 480;
            break;
        }
        case Res_800x600:
        {
            x = 800;
            y = 600;
            break;
        }
        case Res_1024x768:
        {
            x = 1024;
            y = 768;
            break;
        }
        case Res_1152x864:
        {
            x = 1152;
            y = 864;
            break;
        }
        case Res_1280x1024:
        {
            x = 1280;
            y = 1024;
            break;
        }
        case Res_1600x1200:
        {
            x = 1600;
            y = 1200;
            break;
        }
        default:
        {
            rAssert( false );
        }
    }
}

//==============================================================================
// Win32Platform::IsResolutionSupported
//==============================================================================
// Description: Determines if a resolution is supported on this pc
//
// Parameters:	resolution - the res enum
//
// Return:      true if supported.
//
//==============================================================================

bool Win32Platform::IsResolutionSupported( Resolution res, int bpp ) const
{
    int x,y;

    TranslateResolution( res, x, y );

    // Get the display info for the device
    pddiDisplayInfo* displays = NULL;
    int num_adapters = mpContext->GetDevice()->GetDisplayInfo( &displays );
    rAssert( num_adapters > 0 );

    // Go through the supported modes and see if we can do it.
    // Ignore the refresh rate - directx uses default.
    for( int i = 0; i < displays[0].nDisplayModes; i++ )
    {
        if( displays[0].modeInfo[i].width == x &&
            displays[0].modeInfo[i].height == y &&
            displays[0].modeInfo[i].bpp == bpp )
        {
            return true;
        }
    }

    return false;
}

//==============================================================================
// Win32Platform::TrackMouseEvent
//==============================================================================
// Description: Determines if a resolution is supported on this pc
//
// Parameters:	pMouseEventTracker - the tracker
//
// Return:      true if succeeded in creating a timer.
//
//==============================================================================
BOOL Win32Platform::TrackMouseEvent( MOUSETRACKER* pMouseEventTracker ) 
{
    if ( !pMouseEventTracker || pMouseEventTracker->cbSize < sizeof(MOUSETRACKER) ) 
        return false;

    if( !IsWindow( pMouseEventTracker->hwndTrack ) ) 
        return false;

    if( !(pMouseEventTracker->dwFlags & TIMER_LEAVE) )
        return false;

    return SetTimer( pMouseEventTracker->hwndTrack, 
                     pMouseEventTracker->dwFlags, 
                     100, (TIMERPROC)TrackMouseTimerProc );
}

//=============================================================================
// Win32Platform::ResizeWindow
//=============================================================================
// Description: Resizes the app's window based on the current resolution.
//
// Parameters:  n/a
//
// Returns:     n/a
//
// Notes:
//=============================================================================

void Win32Platform::ResizeWindow()
{
    // If fullscreen, no need to change the window size.
    if( mFullscreen )
    {
        return;
    }

    int x,y,cx,cy;
    RECT clientRect;

    TranslateResolution( mResolution, cx, cy );

    if( cx < mScreenWidth )     // if the window fits on the desktop
    {
        x = ( mScreenWidth - cx ) / 2;
        y = ( mScreenHeight - cy ) / 2;

        // Adjust the rectangle for the title bar and borders
        clientRect.left = x;
        clientRect.top = y;
        clientRect.right = x+cx;
        clientRect.bottom = y+cy;
        
        AdjustWindowRect(&clientRect,WndStyle,FALSE);
    }
    else    // if the window is bigger than the client area
    {
        clientRect.left = 0;
        clientRect.top = 0;
        clientRect.right = cx;
        clientRect.bottom = cy;
    }

    SetWindowPos( mhWnd, 
                  HWND_TOP, 
                  clientRect.left, 
                  clientRect.top, 
                  clientRect.right-clientRect.left,
                  clientRect.bottom-clientRect.top,
                  0 );
    ShowWindow( mhWnd, SW_SHOW );
}

//=============================================================================
// Win32Platform::ShowTheCursor
//=============================================================================
// Description: Shows or hides the cursor.  Wrapper for the windows ShowCursor
//              function, except it doesn't keep a counter for the number of
//              shows/hides.
//
// Parameters:  show - show cursor
//
// Returns:     n/a
//
// Notes:
//=============================================================================

void Win32Platform::ShowTheCursor( bool show )
{
    if( mShowCursor != show )
    {        
        mShowCursor = show;
        ShowCursor( mShowCursor );
    }
}

//=============================================================================
// GetMessageName
//=============================================================================
// Description: Prints the name of windows messages.
//
// Parameters:  message - message id
//
// Returns:     string name
//
// Notes:
//=============================================================================

#if defined( PRINT_WINMESSAGES ) && defined( RAD_DEBUG )
static const char* GetMessageName( UINT message )
{
    switch ( message )
    {
    case 0x0000: return "WM_NULL";
    case 0x0001: return "WM_CREATE";
    case 0x0002: return "WM_DESTROY";
    case 0x0003: return "WM_MOVE";
    case 0x0005: return "WM_SIZE";
    case 0x0006: return "WM_ACTIVATE";
    case 0x0007: return "WM_SETFOCUS";
    case 0x0008: return "WM_KILLFOCUS";
    case 0x000A: return "WM_ENABLE";
    case 0x000B: return "WM_SETREDRAW";
    case 0x000C: return "WM_SETTEXT";
    case 0x000D: return "WM_GETTEXT";
    case 0x000E: return "WM_GETTEXTLENGTH";
    case 0x000F: return "WM_PAINT";
    case 0x0010: return "WM_CLOSE";
    case 0x0011: return "WM_QUERYENDSESSION";
    case 0x0013: return "WM_QUERYOPEN";
    case 0x0016: return "WM_ENDSESSION";
    case 0x0012: return "WM_QUIT";
    case 0x0014: return "WM_ERASEBKGND";
    case 0x0015: return "WM_SYSCOLORCHANGE";
    case 0x0018: return "WM_SHOWWINDOW";
    case 0x001A: return "WM_WININICHANGE";
    case 0x001B: return "WM_DEVMODECHANGE";
    case 0x001C: return "WM_ACTIVATEAPP";
    case 0x001D: return "WM_FONTCHANGE";
    case 0x001E: return "WM_TIMECHANGE";
    case 0x001F: return "WM_CANCELMODE";
    case 0x0020: return "WM_SETCURSOR";
    case 0x0021: return "WM_MOUSEACTIVATE";
    case 0x0022: return "WM_CHILDACTIVATE";
    case 0x0023: return "WM_QUEUESYNC";
    case 0x0024: return "WM_GETMINMAXINFO";
    case 0x0026: return "WM_PAINTICON";
    case 0x0027: return "WM_ICONERASEBKGND";
    case 0x0028: return "WM_NEXTDLGCTL";
    case 0x002A: return "WM_SPOOLERSTATUS";
    case 0x002B: return "WM_DRAWITEM";
    case 0x002C: return "WM_MEASUREITEM";
    case 0x002D: return "WM_DELETEITEM";
    case 0x002E: return "WM_VKEYTOITEM";
    case 0x002F: return "WM_CHARTOITEM";
    case 0x0030: return "WM_SETFONT";
    case 0x0031: return "WM_GETFONT";
    case 0x0032: return "WM_SETHOTKEY";
    case 0x0033: return "WM_GETHOTKEY";
    case 0x0037: return "WM_QUERYDRAGICON";
    case 0x0039: return "WM_COMPAREITEM";
    case 0x0041: return "WM_COMPACTING";
    case 0x0044: return "WM_COMMNOTIFY";
    case 0x0046: return "WM_WINDOWPOSCHANGING";
    case 0x0047: return "WM_WINDOWPOSCHANGED";
    case 0x0048: return "WM_POWER";
    case 0x004A: return "WM_COPYDATA";
    case 0x004B: return "WM_CANCELJOURNAL";
    case 0x004E: return "WM_NOTIFY";
    case 0x0050: return "WM_INPUTLANGCHANGEREQUEST";
    case 0x0051: return "WM_INPUTLANGCHANGE";
    case 0x0052: return "WM_TCARD";
    case 0x0053: return "WM_HELP";
    case 0x0054: return "WM_USERCHANGED";
    case 0x0055: return "WM_NOTIFYFORMAT";
    case 0x007B: return "WM_CONTEXTMENU";
    case 0x007C: return "WM_STYLECHANGING";
    case 0x007D: return "WM_STYLECHANGED";
    case 0x007E: return "WM_DISPLAYCHANGE";
    case 0x007F: return "WM_GETICON";
    case 0x0080: return "WM_SETICON";
    case 0x0081: return "WM_NCCREATE";
    case 0x0082: return "WM_NCDESTROY";
    case 0x0083: return "WM_NCCALCSIZE";
    case 0x0084: return "WM_NCHITTEST";
    case 0x0085: return "WM_NCPAINT";
    case 0x0086: return "WM_NCACTIVATE";
    case 0x0087: return "WM_GETDLGCODE";
    case 0x0088: return "WM_SYNCPAINT";
    case 0x00A0: return "WM_NCMOUSEMOVE";
    case 0x00A1: return "WM_NCLBUTTONDOWN";
    case 0x00A2: return "WM_NCLBUTTONUP";
    case 0x00A3: return "WM_NCLBUTTONDBLCLK";
    case 0x00A4: return "WM_NCRBUTTONDOWN";
    case 0x00A5: return "WM_NCRBUTTONUP";
    case 0x00A6: return "WM_NCRBUTTONDBLCLK";
    case 0x00A7: return "WM_NCMBUTTONDOWN";
    case 0x00A8: return "WM_NCMBUTTONUP";
    case 0x00A9: return "WM_NCMBUTTONDBLCLK";
    case 0x00AB: return "WM_NCXBUTTONDOWN";
    case 0x00AC: return "WM_NCXBUTTONUP";
    case 0x00AD: return "WM_NCXBUTTONDBLCLK";
    case 0x00FF: return "WM_INPUT";
    case 0x0100: return "WM_KEYDOWN";
    case 0x0101: return "WM_KEYUP";
    case 0x0102: return "WM_CHAR";
    case 0x0103: return "WM_DEADCHAR";
    case 0x0104: return "WM_SYSKEYDOWN";
    case 0x0105: return "WM_SYSKEYUP";
    case 0x0106: return "WM_SYSCHAR";
    case 0x0107: return "WM_SYSDEADCHAR";
    case 0x0109: return "WM_KEYLAST";
    case 0xFFFF: return "UNICODE_NOCHAR";
    case 0x0108: return "WM_KEYLAST";
    case 0x010D: return "WM_IME_STARTCOMPOSITION";
    case 0x010E: return "WM_IME_ENDCOMPOSITION";
    case 0x010F: return "WM_IME_KEYLAST";
    case 0x0110: return "WM_INITDIALOG";
    case 0x0111: return "WM_COMMAND";
    case 0x0112: return "WM_SYSCOMMAND";
    case 0x0113: return "WM_TIMER";
    case 0x0114: return "WM_HSCROLL";
    case 0x0115: return "WM_VSCROLL";
    case 0x0116: return "WM_INITMENU";
    case 0x0117: return "WM_INITMENUPOPUP";
    case 0x011F: return "WM_MENUSELECT";
    case 0x0120: return "WM_MENUCHAR";
    case 0x0121: return "WM_ENTERIDLE";
    case 0x0122: return "WM_MENURBUTTONUP";
    case 0x0123: return "WM_MENUDRAG";
    case 0x0124: return "WM_MENUGETOBJECT";
    case 0x0125: return "WM_UNINITMENUPOPUP";
    case 0x0126: return "WM_MENUCOMMAND";
    case 0x0127: return "WM_CHANGEUISTATE";
    case 0x0128: return "WM_UPDATEUISTATE";
    case 0x0129: return "WM_QUERYUISTATE";
    case 0x0132: return "WM_CTLCOLORMSGBOX";
    case 0x0133: return "WM_CTLCOLOREDIT";
    case 0x0134: return "WM_CTLCOLORLISTBOX";
    case 0x0135: return "WM_CTLCOLORBTN";
    case 0x0136: return "WM_CTLCOLORDLG";
    case 0x0137: return "WM_CTLCOLORSCROLLBAR";
    case 0x0138: return "WM_CTLCOLORSTATIC";
    case 0x0200: return "WM_MOUSEMOVE";
    case 0x0201: return "WM_LBUTTONDOWN";
    case 0x0202: return "WM_LBUTTONUP";
    case 0x0203: return "WM_LBUTTONDBLCLK";
    case 0x0204: return "WM_RBUTTONDOWN";
    case 0x0205: return "WM_RBUTTONUP";
    case 0x0206: return "WM_RBUTTONDBLCLK";
    case 0x0207: return "WM_MBUTTONDOWN";
    case 0x0208: return "WM_MBUTTONUP";
    case 0x0209: return "WM_MBUTTONDBLCLK";
    case 0x020A: return "WM_MOUSEWHEEL";
    case 0x020B: return "WM_XBUTTONDOWN";
    case 0x020C: return "WM_XBUTTONUP";
    case 0x020D: return "WM_MOUSELAST";
    case 0x0210: return "WM_PARENTNOTIFY";
    case 0x0211: return "WM_ENTERMENULOOP";
    case 0x0212: return "WM_EXITMENULOOP";
    case 0x0213: return "WM_NEXTMENU";
    case 0x0214: return "WM_SIZING";
    case 0x0215: return "WM_CAPTURECHANGED";
    case 0x0216: return "WM_MOVING";
    case 0x0218: return "WM_POWERBROADCAST";
    case 0x0219: return "WM_DEVICECHANGE";
    case 0x0220: return "WM_MDICREATE";
    case 0x0221: return "WM_MDIDESTROY";
    case 0x0222: return "WM_MDIACTIVATE";
    case 0x0223: return "WM_MDIRESTORE";
    case 0x0224: return "WM_MDINEXT";
    case 0x0225: return "WM_MDIMAXIMIZE";
    case 0x0226: return "WM_MDITILE";
    case 0x0227: return "WM_MDICASCADE";
    case 0x0228: return "WM_MDIICONARRANGE";
    case 0x0229: return "WM_MDIGETACTIVE";
    case 0x0230: return "WM_MDISETMENU";
    case 0x0231: return "WM_ENTERSIZEMOVE";
    case 0x0232: return "WM_EXITSIZEMOVE";
    case 0x0233: return "WM_DROPFILES";
    case 0x0234: return "WM_MDIREFRESHMENU";
    case 0x0281: return "WM_IME_SETCONTEXT";
    case 0x0282: return "WM_IME_NOTIFY";
    case 0x0283: return "WM_IME_CONTROL";
    case 0x0284: return "WM_IME_COMPOSITIONFULL";
    case 0x0285: return "WM_IME_SELECT";
    case 0x0286: return "WM_IME_CHAR";
    case 0x0288: return "WM_IME_REQUEST";
    case 0x0290: return "WM_IME_KEYDOWN";
    case 0x0291: return "WM_IME_KEYUP";
    case 0x02A1: return "WM_MOUSEHOVER";
    case 0x02A3: return "WM_MOUSELEAVE";
    case 0x02A0: return "WM_NCMOUSEHOVER";
    case 0x02A2: return "WM_NCMOUSELEAVE";
    case 0x02B1: return "WM_WTSSESSION_CHANGE";
    case 0x02c0: return "WM_TABLET_FIRST";
    case 0x02df: return "WM_TABLET_LAST";
    case 0x0300: return "WM_CUT";
    case 0x0301: return "WM_COPY";
    case 0x0302: return "WM_PASTE";
    case 0x0303: return "WM_CLEAR";
    case 0x0304: return "WM_UNDO";
    case 0x0305: return "WM_RENDERFORMAT";
    case 0x0306: return "WM_RENDERALLFORMATS";
    case 0x0307: return "WM_DESTROYCLIPBOARD";
    case 0x0308: return "WM_DRAWCLIPBOARD";
    case 0x0309: return "WM_PAINTCLIPBOARD";
    case 0x030A: return "WM_VSCROLLCLIPBOARD";
    case 0x030B: return "WM_SIZECLIPBOARD";
    case 0x030C: return "WM_ASKCBFORMATNAME";
    case 0x030D: return "WM_CHANGECBCHAIN";
    case 0x030E: return "WM_HSCROLLCLIPBOARD";
    case 0x030F: return "WM_QUERYNEWPALETTE";
    case 0x0310: return "WM_PALETTEISCHANGING";
    case 0x0311: return "WM_PALETTECHANGED";
    case 0x0312: return "WM_HOTKEY";
    case 0x0317: return "WM_PRINT";
    case 0x0318: return "WM_PRINTCLIENT";
    case 0x0319: return "WM_APPCOMMAND";
    case 0x031A: return "WM_THEMECHANGED";
    case 0x0358: return "WM_HANDHELDFIRST";
    case 0x035F: return "WM_HANDHELDLAST";
    case 0x0360: return "WM_AFXFIRST";
    case 0x0380: return "WM_PENWINFIRST";
    case 0x038F: return "WM_PENWINLAST";
    case 0x8000: return "WM_APP";
    default: return "UNKNOWN MESSAGE";
    }
}
#endif

void Win32Platform::TrackMouseTimerProc( HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime ) 
{
    RECT clientRect;
    POINT point;

    GetClientRect( hWnd, &clientRect );
    MapWindowPoints( hWnd, NULL, (LPPOINT)&clientRect, 2 );
    GetCursorPos( &point );
    if( !PtInRect( &clientRect, point ) || (WindowFromPoint(point) != hWnd ) ) 
    {
        if( !KillTimer( hWnd, idEvent ) ) 
        {
            rDebugPrintf( "Couldn't kill the timer" );
        }

        PostMessage( hWnd, WM_MOUSELEAVE, 0, 0 );
    }
    else
    {
        GetInputManager()->GetFEMouse()->getCursor()->SetVisible( true );
    }
}

//=============================================================================
// Win32Platform::WndProc
//=============================================================================
// Description: The windows os messaging callback for the game.
//              Routes messages to pure3d.
//
// Parameters:  hwnd - handle for window
//                message - message ID
//                wParam - word parameter
//                lParam - long parameter
//
// Returns:     windows result
//
// Notes:
//=============================================================================

LRESULT Win32Platform::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#if defined( PRINT_WINMESSAGES ) && defined( RAD_DEBUG )
    rDebugPrintf( "Windows Message: 0x%08x (%s) [0x%08x, 0x%08x]\n", message, GetMessageName(message), wParam, lParam  ); 
#endif
    MOUSETRACKER mouseEventTracker;
    static bool bMouseInWindow; // A flag to poll if you want to check if the mouse is in the clientwindow.

    //
    // Under Win32, Pure3D needs to get a crack at the Windows messages so
    // it can detect window moving, resizing, and activation.
    //
    p3d::platform->ProcessWindowsMessage(hwnd, message, wParam, lParam);

    switch(message)
    {
    case WM_ACTIVATEAPP:
        {
            InputManager* pInputManager = GetInputManager();

            if( spInstance != NULL && spInstance->mpContext != NULL )
            {
                if( wParam ) // Window is being shown (in focus)
                {
                    RenderFlow* rf = GetRenderFlow();

                    rf->SetGamma( rf->GetGamma() );  
                    if( pInputManager )
                    {
                        //GetInputManager()->SetRumbleForDevice(0, true);
                        //rDebugPrintf("Force Effects Started!!! \n");
                    }
                }
                else  // Window is being hidden (not in focus)
                {
                    SetDeviceGammaRamp( GetDC( GetDesktopWindow( ) ), DesktopGammaRamp );
                    if( pInputManager )
                    {
                        //GetInputManager()->SetRumbleForDevice(0, false);
                        //rDebugPrintf("Force Effects Stopped!!! \n");
                    }
                }

                if( GetInputManager() != NULL )
                {
                    GetInputManager()->GetFEMouse()->SetInGameOverride( !wParam );
                }
            }

            break;
        }

    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
        {
            //Ignore Alt and F10 keys.
            switch(wParam) 
            {
            case VK_MENU:
            	return 0;
            case VK_F10:
            	return 0;
            default: break;
            }
        }
    case WM_SHOWWINDOW:
        {
            break;
        }

    case WM_CREATE:
        {
            GetDeviceGammaRamp( GetDC( GetDesktopWindow( ) ), DesktopGammaRamp );
            bMouseInWindow = false;
            break;
        }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;   
    case WM_MOUSELEAVE:
        bMouseInWindow = false;
        GetInputManager()->GetFEMouse()->getCursor()->SetVisible( false );
        break;

    case WM_NCMOUSEMOVE: 
        {
            POINT pPoint;
            GetCursorPos( &pPoint );

            // Convert the absolute screen coordinates from windows to client window absolute coordinates.
            ScreenToClient( hwnd, &pPoint );

            RECT clientRect;
            GetClientRect( hwnd, &clientRect );

            GetInputManager()->GetFEMouse()->Move( pPoint.x, pPoint.y, clientRect.right, clientRect.bottom );

            ShowTheCursor( true );
            break;
        }
    case WM_MOUSEMOVE:  
        {
            POINT pPoint;
            pPoint.x = LOWORD(lParam);
            pPoint.y = HIWORD(lParam);

            // For some reason beyond my comprehension WM_MOUSEMOVE seems to be getting called regardless if the
            // mouse moved or not. So let the FEMouse determine if we moved.
            FEMouse* pFEMouse = GetInputManager()->GetFEMouse();
            if( pFEMouse->DidWeMove( pPoint.x, pPoint.y ) )
            {
                RECT clientRect;
                GetClientRect( hwnd, &clientRect );
                pFEMouse->Move( pPoint.x, pPoint.y, clientRect.right, clientRect.bottom );
            }

            ShowTheCursor( false );

            if( !bMouseInWindow ) 
            {
                bMouseInWindow = true;
                mouseEventTracker.cbSize = sizeof(MOUSETRACKER);
                mouseEventTracker.dwFlags = TIMER_LEAVE;
                mouseEventTracker.hwndTrack = hwnd;
                if( !TrackMouseEvent( &mouseEventTracker ) )
                {
                    rDebugPrintf( "TrackMouseEvent Failed" );
                }
            }

            break;
        }

    case WM_LBUTTONDOWN:
        {
            GetInputManager()->GetFEMouse()->ButtonDown( BUTTON_LEFT );
    //        rDebugPrintf("LEFT MOUSE BUTTON PRESSED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
            break;
        }
    case WM_LBUTTONUP:
        {
            GetInputManager()->GetFEMouse()->ButtonUp( BUTTON_LEFT );
            break;
        }
    case WM_SYSCOMMAND:
        {
            switch (wParam)
            {
            case SC_SCREENSAVE:   // Screensaver Trying To Start?
            case SC_MONITORPOWER: // Monitor Trying To Enter Powersave?
                return 0;
            }
            break;
        }

        // PDDI will sent this message to enable or disable rendering in response to an
        // application level window event.  For example, if the user clicks away from
        // the rendering window, or uses ALT-TAB to select another application, PDDI
        // will tell sent a WM_PDDI_DRAW_ENABLE(0) message.  When the application
        // regains focus, WM_PDDI_DRAW_ENABLE(1) will be sent.
    case WM_PDDI_DRAW_ENABLE:
        //GetApplication()->EnableRendering(wParam == 1);
        break;

    case WM_CHAR:
        {
            break;
        }
    default:
        break;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

