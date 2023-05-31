//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   GCPlatform   
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
#include <dolphin/vi.h>
#include <dolphin/gx.h>
#include <dolphin/os.h>
#include <dolphin/lg.h>
#include <dolphin/dvd.h>
#include <dolphin.h>
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
#include <render/Loaders/BillboardWrappedLoader.h>
#include <render/Loaders/instparticlesystemloader.h>
#include <render/Loaders/breakableobjectloader.h>
#include <render/Loaders/lensflareloader.h>
#include <render/Loaders/AnimDynaPhysLoader.h>
#include <p3d/shadow.hpp>
#include <p3d/anim/vertexanimkey.hpp>
#include <p3d/anim/animatedobject.hpp>
#include <p3d/effects/particleloader.hpp>
#include <p3d/effects/opticloader.hpp>
// Foundation Tech
#include <raddebug.hpp>
#include <raddebugcommunication.hpp>
#include <raddebugwatch.hpp>
#include <radfile.hpp>
#include <radmemorymonitor.hpp>
#include <radplatform.hpp>
#include <radthread.hpp>
#include <radtime.hpp>
#include <radmovie2.hpp>

//This is so we can get the name of the file that's failing.
#include <../src/radfile/common/requests.hpp>

// sim - for InstallSimLoaders
#include <simcommon/simutility.hpp>

// Stateprops
#include <stateprop/statepropdata.hpp>

// To turn off movies during an error.
#include <presentation/fmvplayer/fmvplayer.h>
#include <presentation/presentation.h>

//========================================
// Project Includes
//========================================
#include <input/inputmanager.h>
#include <main/gcplatform.h>
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

#include <main/gamecube_extras/gcmanager.h>
#include <debug/debuginfo.h>

#include <radload/radload.hpp>

#include <main/errorsgc.h>
#include <presentation/gui/guitextbible.h>

#define GC_SECTION "GC_SECTION"

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

// Static pointer to instance of singleton.
GCPlatform* GCPlatform::spInstance = NULL;

//Add baked in loadable headers here.
static unsigned char gLicenseP3D[] =
#include <main/gamecube_extras/license.h>

//The Adlib font.  <sigh>
unsigned char gFont[] = 
#include <font/defaultfont.h>

//GX Warning Levels.
//GX_WARN_NONE
//GX_WARN_SEVERE
//GX_WARN_MEDIUM
//GX_WARN_ALL

const GXWarningLevel gxWarningLevel = GX_WARN_NONE;

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
// GCPlatform::CreateInstance
//==============================================================================
//
// Description: Creates the GCPlatform.
//
// Parameters:	None.
//
// Return:      Pointer to the GCPlatform.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
GCPlatform* GCPlatform::CreateInstance()
{
MEMTRACK_PUSH_GROUP( "GCPlatform" );
    rAssert( spInstance == NULL );

    spInstance = new(GMA_PERSISTENT) GCPlatform;
    rAssert( spInstance );
MEMTRACK_POP_GROUP( "GCPlatform" );
    
    return spInstance;
}

//==============================================================================
// GCPlatform::GetInstance
//==============================================================================
//
// Description: - Access point for the GCPlatform singleton.  
//
// Parameters:	None.
//
// Return:      Pointer to the GCPlatform.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
GCPlatform* GCPlatform::GetInstance()
{
    rAssert( spInstance != NULL );
    
    return spInstance;
}


//==============================================================================
// GCPlatform::DestroyInstance
//==============================================================================
//
// Description: Destroy the GCPlatform.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void GCPlatform::DestroyInstance()
{
    rAssert( spInstance != NULL );

    delete( GMA_PERSISTENT, spInstance );
    spInstance = NULL;
}



//==============================================================================
// GCPlatform::InitializeFoundation
//==============================================================================
// Description: Get FTech ready to go.
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: The FTech systems must be initialized in a particular order.
//              Consult their documentation before changing.  Also, we need 
//              to only init the quick things to allow time for drawing the
//              first license screens.
//
//==============================================================================
void GCPlatform::InitializeFoundation() 
{
    //
    // Initialize the memory heaps
    //
    GCPlatform::InitializeMemory();
    
#ifndef FINAL
    //
    // Register an out-of-memory display handler in case something goes bad
    // while allocating the heaps
    //
    ::radMemorySetOutOfMemoryCallback( PrintOutOfMemoryMessage, NULL );
#endif

#ifndef RAD_RELEASE
    //
    // Initialize memory monitor by JamesCo. TM.
    //
    if( CommandLineOptions::Get( CLO_MEMORY_MONITOR ) )
    {
        const int KB = 1024;
        ::radMemoryMonitorInitialize( 2048 * KB, GMA_DEBUG );
    }
#endif 

    // Setup the memory heaps
    //
    HeapMgr()->PrepareHeapsStartup ();

    // Seed the heap stack
    //
    HeapMgr()->PushHeap (GMA_PERSISTENT);

    //
    // Initilalize the platform system
    // 
    ::radPlatformInitialize();

    //
    // Initialize the timer system
    //
    ::radTimeInitialize();

#ifndef RAD_RELEASE
    //
    // Initialize the debug communication system.
    //
    ::radDbgComTargetInitialize( HostIO, 
                                 radDbgComDefaultPort, // Default
                                 NULL,                 // Default
                                 GMA_DEBUG );

    //
    // Initialize the Watcher.
    //
   ::radDbgWatchInitialize( "SRR2",
                             16384 * 16, // Default
                             GMA_DEBUG );
#endif

    //
    // Initialize the file system.
    //
    ::radFileInitialize( 50, // Default
                         32, // Default
                         GMA_PERSISTENT );

    ::radLoadInitialize();

    ::radDriveMount(0, GMA_PERSISTENT);

    //
    // Initialize the new movie player
    //
    ::radMovieInitialize2( GMA_PERSISTENT );

}


//==============================================================================
// GCPlatform::InitializeMemory
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void GCPlatform::InitializeMemory()
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
    ::radMemoryInitialize( VMM_MAIN_RAM, VMM_ARAM );
}

//==============================================================================
// GCPlatform::InitializePlatform
//==============================================================================
// Description: Get the GC ready to go.
//
// Parameters:	None.
//
// Return:      None.
//
//=============================================================================
void GCPlatform::InitializePlatform() 
{
    HeapMgr()->PushHeap (GMA_PERSISTENT);

    //
    // Get some rendering action setup.
    //
    InitializePure3D();

    //
    // Add anything here that needs to be before the drive is opened.
    //

    //This is slow.
    InitializeFoundationDrive();

    //
    // Initialize the controller.
    //
    GetInputManager()->Init();

#ifndef PAL
    // no progressive scan on PAL builds, only on NTSC
    //
    GCManager::GetInstance()->DoProgressiveScanTest();
#endif // !PAL

    //TRC: The license screen needs to be
    DisplaySplashScreen( License, NULL );


    //Set the serial cable debug verbosity level
    GXSetVerifyLevel( gxWarningLevel );

    HeapMgr()->PopHeap (GMA_PERSISTENT);
}

void GCPlatform::OnControllerError(const char *msg)
{
    DisplaySplashScreen( Error, msg, 0.7f, 0.0f, 0.0f, tColour(255, 255, 255), 0 );
    mErrorState = CTL_ERROR;
    mPauseForError = true;

}


//==============================================================================
// GCPlatform::ShutdownPlatform
//==============================================================================
// Description: Shut down the GC.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void GCPlatform::ShutdownPlatform()
{
    ShutdownPure3D();
    ShutdownFoundation();
    
    //Shutdown the steering wheel system.
    ::radControllerTerminate();
}

//=============================================================================
// GCPlatform::ResetMachine
//=============================================================================
// Description: resets the machine
//
// Parameters:  none
//
// Return:      void 
//
//=============================================================================
void GCPlatform::ResetMachine()
{
    LaunchDashboard();
}

//=============================================================================
// GCPlatform::LaunchDashboard
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GCPlatform::LaunchDashboard()
{
    GCManager::GetInstance()->Reset();
    GCManager::GetInstance()->PerformReset( false );
}


//=============================================================================
// GCPlatform::DisplaySplashScreen
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
void GCPlatform::DisplaySplashScreen( SplashScreen screenID, 
                                       const char* overlayText, 
                                       float fontScale, 
                                       float textPosX, 
                                       float textPosY, 
                                       tColour textColour,
                                       int fadeFrames )
{
    //Just in case this was called during a draw!
    GXWaitDrawDone();

    HeapMgr()->PushHeap( GMA_TEMP );

    p3d::inventory->PushSection();
    p3d::inventory->AddSection( GC_SECTION );
    p3d::inventory->SelectSection( GC_SECTION );

    unsigned char* screen = NULL;
    const char* texname = NULL;
    int size = 0;
    tTexture* texture = NULL;

    P3D_UNICODE unicodeText[256];


    //Create shader
    tShader* splashShader = new tShader("simple");
    splashShader->SetInt(PDDI_SP_FILTER, PDDI_FILTER_NONE);
    splashShader->AddRef();

    if ( screenID == FadeToBlack )
    {
        splashShader->SetInt(PDDI_SP_BLENDMODE,PDDI_BLEND_ALPHA);
        splashShader->SetInt(PDDI_SP_FILTER,PDDI_FILTER_BILINEAR);
    }
    else if ( screenID == Error )
    {
        splashShader->SetColour(PDDI_SP_DIFFUSE, tColour(0, 0, 0));
        splashShader->SetColour(PDDI_SP_AMBIENT, tColour(0, 0, 0));
        splashShader->SetColour(PDDI_SP_SPECULAR, tColour(0, 0, 0));
        splashShader->SetColour(PDDI_SP_EMISSIVE, tColour(0, 0, 0));
    }
    else
    {
        //This is so we can handle more baked in screens if we want
        switch (screenID)
        {
            case License:
            {
                screen  = gLicenseP3D;
                texname = LICENSE_NAME;
                size    = LICENSE_SIZE;
            }
            break;
            default:
            rAssert( false);
        }

        // Load the in screen texture file from memory
        p3d::load(screen, size, HeapMgr()->GetCurrentHeap());

        // Get the texture to draw
        texture = p3d::find<tTexture>(texname);
        rAssert( texture );

        // Put it in our shader
        splashShader->SetTexture(PDDI_SP_BASETEX, texture);
    }

    // Save the current Projection mode so I can restore it later
    pddiProjectionMode pm = p3d::pddi->GetProjectionMode();
    p3d::pddi->SetProjectionMode(PDDI_PROJECTION_DEVICE);

    pddiCullMode cm = p3d::pddi->GetCullMode();
    p3d::pddi->SetCullMode(PDDI_CULL_NONE);

    //CREATE THE FONT
    tTextureFont* thisFont = NULL;

    if ( overlayText != NULL )
    {
        // Convert memory buffer into a texturefont.
        //
        //p3d::load(gFont, DEFAULTFONT_SIZE, HeapMgr()->GetCurrentHeap() );
        LoadMemP3DFile( gFont, DEFAULTFONT_SIZE, p3d::inventory );

        thisFont = p3d::find<tTextureFont>("adlibn_20");
        rAssert( thisFont );

        thisFont->AddRef();
        tShader* fontShader = thisFont->GetShader();
        fontShader->SetInt(PDDI_SP_BLENDMODE,PDDI_BLEND_ALPHA);
        fontShader->SetInt(PDDI_SP_FILTER,PDDI_FILTER_BILINEAR);

        p3d::AsciiToUnicode( overlayText, unicodeText, 256 );

        // Make the missing letter into somthing I can see
        //
        thisFont->SetMissingLetter(p3d::ConvertCharToUnicode('j'));
    }

    // Fade up the screen
    int a = 0;
    do
    {
        if ( screenID != FadeToBlack )
        {
            p3d::pddi->SetColourWrite(true, true, true, true);
            p3d::pddi->SetClearColour( pddiColour(0,0,0) );
        }

        p3d::pddi->BeginFrame();

        p3d::pddi->SetProjectionMode(PDDI_PROJECTION_DEVICE);

        GXSetScissorBoxOffset(0, 0);
#ifdef PAL
        GXSetScissor(0, 0, 640, 528);
#else
        GXSetScissor(0, 0, 640, 480);
#endif

        if ( screenID != FadeToBlack )
        {
            p3d::pddi->Clear(PDDI_BUFFER_ALL);
        }

        int bright = 255;
        if (a < fadeFrames) bright = (a * 255) / fadeFrames / 10;
        if ( bright > 255 ) bright = 255;

        tColour c;
        if ( screenID == FadeToBlack)
        {
            c.Set(0, 0, 0, bright);
        }
        else
        {
            c.Set(bright, bright, bright, 255);
        }

        if ( screenID != Error )
        {
            p3d::pddi->PushMatrix(PDDI_MATRIX_MODELVIEW);
            p3d::pddi->IdentityMatrix(PDDI_MATRIX_MODELVIEW);

            p3d::stack->Push();
            p3d::stack->LoadIdentity();

            pddiPrimStream* stream;

            if ( screenID == FadeToBlack )
            {
                stream = p3d::pddi->BeginPrims(splashShader->GetShader(), PDDI_PRIM_TRISTRIP, PDDI_V_C, 4);
            }
            else
            {
                stream = p3d::pddi->BeginPrims(splashShader->GetShader(), PDDI_PRIM_TRISTRIP, PDDI_V_CT, 4);
            }

#ifdef PAL
            stream->Colour(c); stream->UV(0.0F, 0.0F); stream->Coord(  0.0F, 528.0F, 2.01F);
            stream->Colour(c); stream->UV(1.0F, 0.0F); stream->Coord(640.0F, 528.0F, 2.01F);         
#else
            stream->Colour(c); stream->UV(0.0F, 0.0F); stream->Coord(  0.0F, 480.0F, 2.01F);
            stream->Colour(c); stream->UV(1.0F, 0.0F); stream->Coord(640.0F, 480.0F, 2.01F);         
#endif
            stream->Colour(c); stream->UV(0.0F, 1.0F); stream->Coord(  0.0F,   0.0F, 2.01F);         
            stream->Colour(c); stream->UV(1.0F, 1.0F); stream->Coord(640.0F,   0.0F, 2.01F);         
  
            p3d::pddi->EndPrims(stream);

            p3d::stack->Pop();
    
            p3d::pddi->PopMatrix(PDDI_MATRIX_MODELVIEW);
        }

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

        if (screenID == FadeToBlack && bright >= 255)
        {
            //This is a sucky hack...
            break;
        }

    } while (a <= fadeFrames);

    // [ps]: flush out this screen now.
    if ( screenID == FadeToBlack )
    {
        GXSetScissorBoxOffset(0, 0);
#ifdef PAL
        GXSetScissor(0, 0, 640, 528);
#else
        GXSetScissor(0, 0, 640, 480);
#endif

        p3d::pddi->Clear(PDDI_BUFFER_ALL);
    }

    // Remove the texture from the shader
    splashShader->SetTexture(PDDI_SP_BASETEX, NULL);
    splashShader->Release();

    // remove the texture from the inventory
    if ( texture )
    {
        p3d::inventory->Remove(texture);
    }

    p3d::pddi->SetCullMode(cm);
    p3d::pddi->SetProjectionMode(pm);

    if ( thisFont )
    {
        thisFont->Release();
        p3d::inventory->Remove(thisFont);
    }

    p3d::inventory->RemoveSectionElements(GC_SECTION);
    p3d::inventory->DeleteSection(GC_SECTION);
    p3d::inventory->PopSection();

    HeapMgr()->PopHeap( GMA_TEMP );
}

//=============================================================================
// GCPlatform::DisplaySplashScreen
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* overlayText = NULL, 
//                float fontScale = 1.0f, 
//                float textPosX = 0.0f, 
//                float textPosY = 0.0f, 
//                tColour textColour,
//                int fadeFrames = 3 )
//
// Return:      void 
//
//=============================================================================
void GCPlatform::DisplaySplashScreen( const char* textureName,
                                      const char* overlayText, 
                                      float fontScale, 
                                      float textPosX, 
                                      float textPosY, 
                                      tColour textColour,
                                      int fadeFrames )
{
    //Just in case this was called during a draw!
    GXWaitDrawDone();

    HeapMgr()->PushHeap( GMA_TEMP );

    p3d::inventory->PushSection();
    p3d::inventory->AddSection( GC_SECTION );
    p3d::inventory->SelectSection( GC_SECTION );

    tTexture* texture = NULL;

    P3D_UNICODE unicodeText[256];

    rAssertMsg( textureName, "There must be a texture, use the other one with Error otherwise." );

    //Create shader
    tShader* splashShader = new tShader("simple");
    splashShader->SetInt(PDDI_SP_FILTER, PDDI_FILTER_NONE);
    splashShader->AddRef();


    // Get the texture to draw
    texture = p3d::find<tTexture>(textureName);
    rAssert( texture );

    // Put it in our shader
    splashShader->SetTexture(PDDI_SP_BASETEX, texture);


    // Save the current Projection mode so I can restore it later
    pddiProjectionMode pm = p3d::pddi->GetProjectionMode();
    p3d::pddi->SetProjectionMode(PDDI_PROJECTION_DEVICE);

    pddiCullMode cm = p3d::pddi->GetCullMode();
    p3d::pddi->SetCullMode(PDDI_CULL_NONE);

    //CREATE THE FONT
    tTextureFont* thisFont = NULL;

    if ( overlayText != NULL )
    {
        // Convert memory buffer into a texturefont.
        //
        //p3d::load(gFont, DEFAULTFONT_SIZE, HeapMgr()->GetCurrentHeap());
        LoadMemP3DFile( gFont, DEFAULTFONT_SIZE, p3d::inventory );

        thisFont = p3d::find<tTextureFont>("adlibn_20");
        rAssert( thisFont );

        thisFont->AddRef();
        tShader* fontShader = thisFont->GetShader();
        fontShader->SetInt(PDDI_SP_BLENDMODE,PDDI_BLEND_ALPHA);
        fontShader->SetInt(PDDI_SP_FILTER,PDDI_FILTER_BILINEAR);


        p3d::AsciiToUnicode( overlayText, unicodeText, 256 );

        // Make the missing letter into somthing I can see
        //
        thisFont->SetMissingLetter(p3d::ConvertCharToUnicode('j'));
    }

    // Fade up the screen
    int a = 0;

    do
    {
    	p3d::pddi->SetColourWrite(true, true, true, true);
        p3d::pddi->SetClearColour( pddiColour(0,0,0) );
        p3d::pddi->BeginFrame();
        p3d::pddi->Clear(PDDI_BUFFER_ALL);
        p3d::pddi->SetProjectionMode(PDDI_PROJECTION_DEVICE);

        GXSetScissorBoxOffset(0, 0);
#ifdef PAL
        GXSetScissor(0, 0, 640, 528);
#else
        GXSetScissor(0, 0, 640, 480);
#endif

        p3d::pddi->PushMatrix(PDDI_MATRIX_MODELVIEW);
        p3d::pddi->IdentityMatrix(PDDI_MATRIX_MODELVIEW);

        int bright = 255;
        if (a < fadeFrames) bright = (a * 255) / fadeFrames;
        if ( bright > 255 ) bright = 255;
        tColour c(bright, bright, bright, 255);

        p3d::stack->Push();
        p3d::stack->LoadIdentity();

        pddiPrimStream* stream = p3d::pddi->BeginPrims(splashShader->GetShader(), PDDI_PRIM_TRISTRIP, PDDI_V_CT, 4);

#ifdef PAL
        stream->Colour(c); stream->UV(0.0F, 0.0F); stream->Coord(  0.0F, 528.0F, 2.01F);
        stream->Colour(c); stream->UV(1.0F, 0.0F); stream->Coord(640.0F, 528.0F, 2.01F);         
#else
        stream->Colour(c); stream->UV(0.0F, 0.0F); stream->Coord(  0.0F, 480.0F, 2.01F);
        stream->Colour(c); stream->UV(1.0F, 0.0F); stream->Coord(640.0F, 480.0F, 2.01F);         
#endif
        stream->Colour(c); stream->UV(0.0F, 1.0F); stream->Coord(  0.0F,   0.0F, 2.01F);         
        stream->Colour(c); stream->UV(1.0F, 1.0F); stream->Coord(640.0F,   0.0F, 2.01F);         
  
        p3d::pddi->EndPrims(stream);

        p3d::stack->Pop();

        p3d::pddi->PopMatrix(PDDI_MATRIX_MODELVIEW);

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

            thisFont->DisplayText( unicodeText );

            p3d::stack->Pop();
        }

        p3d::pddi->EndFrame();
        p3d::context->SwapBuffers();

        ++a;

    } while (a <= fadeFrames);

    // Remove the texture from the shader
    splashShader->SetTexture(PDDI_SP_BASETEX, NULL);
    splashShader->Release();

    p3d::pddi->SetCullMode(cm);
    p3d::pddi->SetProjectionMode(pm);

    if ( overlayText != NULL )
    {
        thisFont->Release();
        p3d::inventory->Remove(thisFont);
    }

    p3d::inventory->RemoveSectionElements(GC_SECTION);
    p3d::inventory->DeleteSection(GC_SECTION);
    p3d::inventory->PopSection();

    HeapMgr()->PopHeap( GMA_TEMP );
}

//=============================================================================
// GCPlatform::DisplayYesNo
//=============================================================================
// Description: Comment
//
// Parameters:  ( float fontScale = 1.0f, 
//                float yesPosX = 0.0f, 
//                float yesPosY = 0.0f, 
//                float noPosX = 0.0f, 
//                float noPosY = 0.0f, 
//                int fadeFrames = 3 )
//
// Return:      bool 
//
//=============================================================================
bool GCPlatform::DisplayYesNo( float fontScale, 
                               float yesPosX, 
                               float yesPosY, 
                               float noPosX, 
                               float noPosY, 
                               int fadeFrames )
{
    //Just in case this was called during a draw!
    GXWaitDrawDone();

    HeapMgr()->PushHeap( GMA_TEMP );

    p3d::inventory->PushSection();
    p3d::inventory->AddSection( GC_SECTION );
    p3d::inventory->SelectSection( GC_SECTION );

    // Save the current Projection mode so I can restore it later
    pddiProjectionMode pm = p3d::pddi->GetProjectionMode();

    pddiCullMode cm = p3d::pddi->GetCullMode();
    p3d::pddi->SetCullMode(PDDI_CULL_NONE);

    //CREATE THE FONT
    tTextureFont* thisFont = NULL;

    // Convert memory buffer into a texturefont.
    //
    //p3d::load(gFont, DEFAULTFONT_SIZE, HeapMgr()->GetCurrentHeap());
    LoadMemP3DFile( gFont, DEFAULTFONT_SIZE, p3d::inventory );

    thisFont = p3d::find<tTextureFont>("adlibn_20");
    rAssert( thisFont );

    thisFont->AddRef();
    tShader* fontShader = thisFont->GetShader();
    fontShader->SetInt(PDDI_SP_BLENDMODE,PDDI_BLEND_ALPHA);
    fontShader->SetInt(PDDI_SP_FILTER,PDDI_FILTER_BILINEAR);

    //This is the text!
    const char* yes = "Yes";
    const char* no = "No";
    
    P3D_UNICODE unicodeYes[256];
    P3D_UNICODE unicodeNo[256];

    p3d::AsciiToUnicode( yes, unicodeYes, 256 );
    p3d::AsciiToUnicode( no, unicodeNo, 256 );

    // Make the missing letter into somthing I can see
    //
    thisFont->SetMissingLetter(p3d::ConvertCharToUnicode('j'));

    unsigned int inputCount = 0;
    PADStatus controllers[PAD_MAX_CONTROLLERS];

    for ( inputCount = 0; inputCount < PAD_MAX_CONTROLLERS; ++inputCount )
    {
        controllers[ inputCount ].button = 0;
    }

    LGPosition lgStatus[ SI_MAX_CHAN ];
    memset( lgStatus, 0, sizeof( LGPosition )*SI_MAX_CHAN  );

    bool yesSelected = true;
    bool buttonAPressed[ PAD_MAX_CONTROLLERS ];
    bool buttonAReleased = false;

    // Fade up the screen
    int a = 0;
    tColour colour;

    unsigned int start = OSTicksToMilliseconds( OSGetTime() );

    bool buttonDown[ PAD_MAX_CONTROLLERS ];
    
    unsigned int i;

    for ( i = 0; i < PAD_MAX_CONTROLLERS; ++i )
    {
        buttonDown[ i ] = false;
        buttonAPressed[ i ] = false;
    }

    do
    {
        unsigned int end = OSTicksToMilliseconds( OSGetTime() );
        unsigned int milliseconds = end - start;
        start = end;

        p3d::pddi->BeginFrame();

        GXSetScissorBoxOffset(0, 0);
#ifdef PAL
        GXSetScissor(0, 0, 640, 528);
#else
        GXSetScissor(0, 0, 640, 480);
#endif
        int bright = 255;
        if (a < fadeFrames) bright = (a * 255) / fadeFrames;
        if ( bright > 255 ) bright = 255;
        tColour c(bright, bright, bright, 255);

        p3d::pddi->SetProjectionMode(PDDI_PROJECTION_ORTHOGRAPHIC);
        
        float scaleSize = 1.0f / 480.0f;  //This is likely good for 528 also.

        //Draw Yes
        p3d::stack->Push();
        p3d::stack->LoadIdentity();

        p3d::stack->Translate( yesPosX, yesPosY, 1.5f);
        p3d::stack->Scale(scaleSize * fontScale, scaleSize* fontScale , 1.0f);

        if ( yesSelected )
        {
            colour = tColour(255, 255, 255, bright);
        }
        else
        {
            colour = tColour(100, 100, 100, bright);
        }

        thisFont->SetColour( colour );
        thisFont->DisplayText( unicodeYes );

        p3d::stack->Pop();

        //Draw No
        p3d::stack->Push();
        p3d::stack->LoadIdentity();

        p3d::stack->Translate( noPosX, noPosY, 1.5f);
        p3d::stack->Scale(scaleSize * fontScale, scaleSize* fontScale , 1.0f);

        if ( !yesSelected )
        {
            colour = tColour(255, 255, 255, bright);
        }
        else
        {
            colour = tColour(100, 100, 100, bright);
        }

        thisFont->SetColour( colour );
        thisFont->DisplayText( unicodeNo );

        p3d::stack->Pop();

        p3d::pddi->EndFrame();
        p3d::context->SwapBuffers();

        if ( bright < 255 )
        {
            ++a;
        }

        ::radControllerSystemService();
        ::radThreadSleep( 32 );

        PADRead(controllers);
        LGRead( lgStatus );

        rAssert( PAD_MAX_CONTROLLERS == SI_MAX_CHAN );
        for ( i = 0; i < PAD_MAX_CONTROLLERS; ++i )
        {
            bool start = controllers[ i ].err == PAD_ERR_NONE &&
                         ( controllers[ i ].button & PAD_BUTTON_MENU );

            start = start ||
                    (lgStatus[ i ].err == LG_ERR_NONE && (lgStatus[ i ].button & LG_BUTTON_START )); 
            
            bool x = controllers[ i ].err == PAD_ERR_NONE &&
                     ( controllers[ i ].button & PAD_BUTTON_A );

            x = x ||
                (lgStatus[ i ].err == LG_ERR_NONE && (lgStatus[ i ].button & LG_BUTTON_A )); 
            
            if ( x || start )
            {
                buttonAPressed[ i ] = true;
            }
            else if ( buttonAPressed[ i ] )
            {
                buttonAReleased = true;
                break;
            }

            bool dLeft = ( controllers[ i ].err == PAD_ERR_NONE &&
                           ( ( controllers[ i ].button & PAD_BUTTON_LEFT ) ||
                             ( controllers[ i ].stickX < -50 ) ) );
            bool dRight = ( controllers[ i ].err == PAD_ERR_NONE &&
                            ( ( controllers[ i ].button & PAD_BUTTON_RIGHT ) ||
                              ( controllers[ i ].stickX > 50 ) ) );

            dLeft = dLeft ||
                    (lgStatus[ i ].err == LG_ERR_NONE && ((lgStatus[ i ].button & LG_BUTTON_LEFT) || (lgStatus[ i ].wheel < -50 )));

            dRight = dRight ||
                     (lgStatus[ i ].err == LG_ERR_NONE && ((lgStatus[ i ].button & LG_BUTTON_RIGHT) || (lgStatus[ i ].wheel > 50 )));

            if ( ( dLeft || dRight ) && !buttonDown[ i ] )
            {
                yesSelected = !yesSelected;
                buttonDown[ i ] = true;
            }
            else if ( !dLeft && !dRight )
            {
                buttonDown[ i ] = false;
            }
        }

        GXWaitDrawDone();
    } while ( !buttonAReleased );

    p3d::pddi->SetCullMode(cm);
    p3d::pddi->SetProjectionMode(pm);

    thisFont->Release();

    p3d::inventory->Remove(thisFont);

    p3d::inventory->RemoveSectionElements(GC_SECTION);
    p3d::inventory->DeleteSection(GC_SECTION);
    p3d::inventory->PopSection();

    HeapMgr()->PopHeap( GMA_TEMP );

    return yesSelected;
}

//=============================================================================
// GCPlatform::OnDriveError
//=============================================================================
// Description: Comment
//
// Parameters:  ( radFileError error, const char* pDriveName, void* pUserData )
//
// Return:      bool 
//
//=============================================================================
bool GCPlatform::OnDriveError( radFileError error, const char* pDriveName, void* pUserData )
{
    GCManager::GetInstance()->OnTimerDone( 16, NULL );

    // Halt rumbling
    GCManager::GetInstance()->StopRumble();

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
                error = NoMedia;
                //Fall through.
            }
        }
    case ShellOpen:
    case WrongMedia:
    case NoMedia:
    case MediaCorrupt:
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
// Protected Member Functions
//
//******************************************************************************


//==============================================================================
// GCPlatform::InitializeFoundationDrive
//==============================================================================
// Description: Get FTech ready to go.
//
// Parameters:	None.
//
// Return:      None.
//
// Constraints: The FTech systems must be initialized in a particular order.
//              Consult their documentation before changing.  This is all the
//              slower elements of FTech
//
//==============================================================================
void GCPlatform::InitializeFoundationDrive() 
{
    //
    // Get the DVD drive and hold it open for the life of the game.
    // This is a costly operation so we only want to do it once.
    //

    ::radDriveOpen( &mpIRadDrive, 
                    "DVD:",
                    NormalPriority, // Default
                    GMA_PERSISTENT );

    rAssert( mpIRadDrive != NULL );

    mpIRadDrive->RegisterErrorHandler( this, NULL );

    //
    // Set the read-write granulatity to prevent operations from
    // being partitioned.
    //
//    const int GRANULARITY = 20 * 1024 * 1024;
//    mpIRadDrive->SetReadWriteGranularity( GRANULARITY );

    DVDSetAutoFatalMessaging( true );
}


//==============================================================================
// GCPlatform::ShutdownFoundation
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
void GCPlatform::ShutdownFoundation()
{
    //
    // Release the drive we've held open since the begining.
    //
    mpIRadDrive->Release();
    mpIRadDrive = NULL;
     
    //
    // Shutdown the systems in the reverse order.
    //
    ::radDriveUnmount();
    ::radFileTerminate();

#ifndef RAD_RELEASE
    ::radDbgWatchTerminate();
    if( CommandLineOptions::Get( CLO_MEMORY_MONITOR ) )
    {
        ::radMemoryMonitorTerminate();
    }
    ::radDbgComTargetTerminate();
#endif

    ::radTimeTerminate();
    ::radPlatformTerminate();
    ::radMemoryTerminate();
    ::radThreadTerminate();
}


//==============================================================================
// GCPlatform::InitializePure3D
//==============================================================================
// Description: Get Pure3D ready to go.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void GCPlatform::InitializePure3D() 
{
MEMTRACK_PUSH_GROUP( "GCPlatform" );
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

    //tContextInitData init;  We're storing this as a member var called mInitData.

    //
    // These values only take effect in fullscreen mode.  In windowed mode, the
    // dimensions of the window define the rendering area.  We'll define them
    // anyway for completeness sake.
    //
    mInitData.xsize = WindowSizeX;
    mInitData.ysize = WindowSizeY;

    //
    // Depth of the rendering buffer.  Again, this value only works in
    // fullscreen mode.  In window mode, the depth of the desktop is used.
    // This value should be either 16 or 32.  Depths of 4, 8, and 24 are not
    // supported by most 3D hardware, and not by Pure3D.
    //
//    mInitData.bpp = WindowBPP;
    mInitData.bufferMask  = PDDI_BUFFER_COLOUR | PDDI_BUFFER_DEPTH;
    //
    // Rendering to NTSC or PAL.
    //
#ifdef PAL    
    mInitData.pal = true;
#else
    mInitData.pal = false;

    //Progressive only available in NTSC
//    mInitData.progressive = OSGetProgressiveMode() && VIGetDTVStatus();
    mInitData.progressive = false;
#endif
    
    //TODO: Investigate VSync
    mInitData.lockToVsync = false;
    //
    // Create the context.
    //
    mpContext = mpPlatform->CreateContext( &mInitData );
    rAssert( mpContext != NULL );

    VISetBlack(FALSE);
    VIFlush();
    VIWaitForRetrace();

    //
    // Assign this context to the platform.
    //
    mpPlatform->SetActiveContext( mpContext );
    p3d::pddi->EnableZBuffer( true );

    //
    // This call installs chunk handlers for all the primary chunk types that
    // Pure3D supports.  This includes textures, materials, geometries, and the
    // like.
    //
//    p3d::InstallDefaultLoaders();
        P3DASSERT(p3d::context);
    tP3DFileHandler* p3d = new(GMA_GC_VMM) tP3DFileHandler;
//    p3d::loadManager->AddHandler(p3d);
    p3d::context->GetLoadManager()->AddHandler(p3d, "p3d");
    p3d::context->GetLoadManager()->AddHandler(new(GMA_GC_VMM) tPNGHandler, "png");

    if( CommandLineOptions::Get( CLO_FE_UNJOINED ) )
    {
        p3d::context->GetLoadManager()->AddHandler(new(GMA_GC_VMM) tBMPHandler, "bmp");
        p3d::context->GetLoadManager()->AddHandler(new(GMA_GC_VMM) tTargaHandler, "tga");
    }
    else
    {
        p3d::context->GetLoadManager()->AddHandler(new(GMA_GC_VMM) tBMPHandler, "p3d");
        p3d::context->GetLoadManager()->AddHandler(new(GMA_GC_VMM) tPNGHandler, "p3d");
        p3d::context->GetLoadManager()->AddHandler(new(GMA_GC_VMM) tTargaHandler, "p3d");
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

    p3d->AddHandler(new(GMA_GC_VMM) tTextureLoader);
    p3d->AddHandler( new(GMA_GC_VMM) tSetLoader );
    p3d->AddHandler(new(GMA_GC_VMM) tShaderLoader);
    p3d->AddHandler(new(GMA_GC_VMM) tCameraLoader);
    p3d->AddHandler(new(GMA_GC_VMM) tGameAttrLoader);
    p3d->AddHandler(new(GMA_GC_VMM) tLightLoader);
  
    p3d->AddHandler(new(GMA_GC_VMM) tLocatorLoader);
    p3d->AddHandler(new(GMA_GC_VMM) tLightGroupLoader);
    p3d->AddHandler(new(GMA_GC_VMM) tImageLoader);
    p3d->AddHandler(new(GMA_GC_VMM) tTextureFontLoader);
    p3d->AddHandler(new(GMA_GC_VMM) tImageFontLoader);
    p3d->AddHandler(new(GMA_GC_VMM) tSpriteLoader);
    //p3d->AddHandler(new(GMA_GC_VMM) tBillboardQuadGroupLoader);
    p3d->AddHandler(new(GMA_GC_VMM) tSkeletonLoader);
    p3d->AddHandler(new(GMA_GC_VMM) tPolySkinLoader);
    p3d->AddHandler(new(GMA_GC_VMM) tCompositeDrawableLoader);
    p3d->AddHandler(new(GMA_GC_VMM) tAnimationLoader);
    p3d->AddHandler(new(GMA_GC_VMM) tFrameControllerLoader);
    p3d->AddHandler(new(GMA_GC_VMM) tMultiControllerLoader);
    p3d->AddHandler(new(GMA_GC_VMM) tAnimatedObjectFactoryLoader);
    p3d->AddHandler(new(GMA_GC_VMM) tAnimatedObjectLoader);
    p3d->AddHandler(new(GMA_GC_VMM) tParticleSystemFactoryLoader);
    p3d->AddHandler(new(GMA_GC_VMM) tParticleSystemLoader);
    p3d->AddHandler(new(GMA_GC_VMM) tLensFlareGroupLoader);
    p3d->AddHandler(new(GMA_GC_VMM) sg::Loader);

    p3d->AddHandler(new(GMA_GC_VMM) tExpressionGroupLoader);
    p3d->AddHandler(new(GMA_GC_VMM) tExpressionMixerLoader);
    p3d->AddHandler(new(GMA_GC_VMM) tExpressionLoader);

    p3d->AddHandler(new(GMA_GC_VMM) tVertexAnimKeyLoader);

    //p3d->AddHandler(new p3d::tIgnoreLoader);

    tSEQFileHandler* sequencerFileHandler = new(GMA_GC_VMM) tSEQFileHandler;
    p3d::loadManager->AddHandler(sequencerFileHandler, "seq");

     // sim lib
    sim::InstallSimLoaders();

    p3d->AddHandler(new(GMA_GC_VMM) CameraDataLoader, SRR2::ChunkID::WALKERCAM );    
    p3d->AddHandler(new(GMA_GC_VMM) CameraDataLoader, SRR2::ChunkID::FOLLOWCAM );    
    p3d->AddHandler(new(GMA_GC_VMM) IntersectionLoader);    
//    p3d->AddHandler(new(GMA_GC_VMM) RoadLoader);    
    p3d->AddHandler(new(GMA_GC_VMM) RoadDataSegmentLoader);    
    p3d->AddHandler(new(GMA_GC_VMM) ATCLoader);
    p3d->AddHandler(new(GMA_GC_VMM) CStatePropDataLoader);
MEMTRACK_POP_GROUP( "GCPlatform" );
}


//==============================================================================
// GCPlatform::ShutdownPure3D
//==============================================================================
// Description: Clean up and shut down Pure3D.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void GCPlatform::ShutdownPure3D()
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


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************


//==============================================================================
// GCPlatform::GCPlatform
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
GCPlatform::GCPlatform() :
    mpPlatform( NULL ),
    mpContext( NULL )
{
}


//==============================================================================
// GCPlatform::~GCPlatform
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
GCPlatform::~GCPlatform()
{
}
