//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        gcplatform.h
//
// Description: Abstracts the differences for setting up and shutting down
//              the different platforms.
//
// History:     + Stolen and cleaned up from Svxy -- Darwin Chau
//
//=============================================================================

#ifndef GCPLATFORM_H
#define GCPLATFORM_H

//========================================
// Nested Includes
//========================================
#include <main/platform.h> // base class

#include <p3d/platform.hpp>

//========================================
// Forward References
//========================================
struct IRadMemoryHeap;
//class tPlatform;
class tContext;

//
// This value define the resolution of the rendering area.
// Based on the width, Pure3D figures out the approriate height.
//
const int WindowSizeX = 640;

#ifdef PAL
const int WindowSizeY = 524;
#else
const int WindowSizeY = 480;
#endif

//
// The depth of the rendering area.  This value only has an effect
// when Pure3D has taken over the entire display.  When running in
// a window on the desktop, Pure3D uses the same bit depth as the
// desktop.  Pure3D only supports 16, and 32 rendering depths.
//
static const int WindowBPP = 16;

//Turn this on to disable ARAM use.  Use this to test leaks in the ARAM heap.
//#define NO_ARAM

#ifdef NO_ARAM
const int VMM_MAIN_RAM = 0;
const int VMM_ARAM = 0;
#else
const int VMM_MAIN_RAM = ( 1 * 1024 * 1024 ) + ( 500 * 1024 );
const int VMM_ARAM = 6 * 1024 * 1024;
#endif


//=============================================================================
//
// Synopsis:    Provides abstraction for setting up and closing down the GC.
//
//=============================================================================
class GCPlatform : public Platform
{
    public:
    
        // Static Methods for accessing this singleton.
        static GCPlatform* CreateInstance();
        static GCPlatform* GetInstance();
        static void DestroyInstance();

        // Had to workaround our nice clean design cause FTech must be init'ed
        // before anything else is done.
        static void InitializeFoundation();
        static void InitializeMemory();
        
        // Implement Platform interface.
        virtual void InitializePlatform();
        virtual void ShutdownPlatform();
        
        virtual void InitializeFoundation();

        virtual void LaunchDashboard();
        virtual void ResetMachine();
        virtual void DisplaySplashScreen( SplashScreen screenID, 
                                          const char* overlayText = NULL, 
                                          float fontScale = 1.0f, 
                                          float textPosX = 0.0f,
                                          float textPosY = 0.0f,
                                          tColour textColour = tColour( 255,255,255 ),
                                          int fadeFrames = 3 );

        virtual void DisplaySplashScreen( const char* textureName,
                                          const char* overlayText = NULL, 
                                          float fontScale = 1.0f, 
                                          float textPosX = 0.0f,
                                          float textPosY = 0.0f,
                                          tColour textColour = tColour( 255,255,255 ),
                                          int fadeFrames = 3 );

        bool DisplayYesNo( float fontScale = 1.0f, 
                           float yesPosX = 0.0f, 
                           float yesPosY = 0.0f, 
                           float noPosX = 0.0f, 
                           float noPosY = 0.0f, 
                           int fadeFrames = 3 );

        tContextInitData* GetInitData();

        virtual bool OnDriveError( radFileError error, const char* pDriveName, void* pUserData );  
        virtual void OnControllerError(const char *msg);
  
    protected:

        virtual void InitializeFoundationDrive();
        virtual void ShutdownFoundation();

        virtual void InitializePure3D();
        virtual void ShutdownPure3D();
        
    private:

        // Constructors, Destructors, and Operators
        GCPlatform();
        virtual ~GCPlatform();

        // Unused Constructors, Destructors, and Operators
        GCPlatform( const GCPlatform& aPlatform );
        GCPlatform& operator=( const GCPlatform& aPlatform );

        // Pointer to the one and only instance of this singleton.
        static GCPlatform* spInstance;

        // Pure 3D attributes
        tPlatform* mpPlatform; 
        tContext* mpContext;

        tContextInitData mInitData;
};

//=============================================================================
// GCPlatform::GetInitData
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      tContextInitData
//
//=============================================================================
inline tContextInitData* GCPlatform::GetInitData()
{
    return &mInitData;
}

#endif // GCPLATFORM_H
