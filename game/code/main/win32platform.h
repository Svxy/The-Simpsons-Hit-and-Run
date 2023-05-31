//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   Win32Platform   
//
// Description: Abstracts the differences for setting up and shutting down
//              the different platforms.
//
// History:     + Stolen and cleaned up from Svxy -- Darwin Chau
//
//=============================================================================

#ifndef WIN32PLATFORM_H
#define WIN32PLATFORM_H

//========================================
// System Includes
//========================================

#include <windows.h>

//========================================
// Nested Includes
//========================================
#include "platform.h" // base class
#include <data/config/gameconfig.h> // interface

//========================================
// Forward References
//========================================
struct IRadMemoryHeap;
class tPlatform;
class tContext;


struct MOUSETRACKER 
{
    DWORD cbSize;
    DWORD dwFlags;
    HWND  hwndTrack;
};

//=============================================================================
//
// Synopsis:    Provides abstraction for setting up and closing a win32 exe.
//
//=============================================================================
class Win32Platform : public Platform, public GameConfigHandler
{
public:

    enum Resolution
    {
        Res_640x480 = 0,
        Res_800x600,
        Res_1024x768,
        Res_1152x864,
        Res_1280x1024,
        Res_1600x1200
    };

public:

    // Static Methods for accessing this singleton.
    static Win32Platform* CreateInstance( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow );
    static Win32Platform* GetInstance();
    static void DestroyInstance();

    // Had to workaround our nice clean design cause FTech must be init'ed
    // before anything else is done.
    static bool InitializeWindow( HINSTANCE hInstance );
    static void InitializeFoundation();
    static void InitializeMemory();
    static void ShutdownMemory();

    // Implement Platform interface.
    virtual void InitializePlatform();
    virtual void ShutdownPlatform();

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

    virtual bool OnDriveError( radFileError error, const char* pDriveName, void* pUserData );  
    virtual void OnControllerError(const char *msg);

    HWND GetHwnd() const { return mhWnd; }

    // Set the resolution of the display, or increase the size of the window.
    // Returns true if the change was successful, false if resolution is not supported.
    bool SetResolution( Resolution res, int bpp, bool fullscreen );

    Resolution GetResolution() const;
    int GetBPP() const;
    bool IsFullscreen() const;

    // Implementation of the GameConfigHandler interface
    virtual const char* GetConfigName() const;
    virtual int GetNumProperties() const;
    virtual void LoadDefaults();
    virtual void LoadConfig( ConfigString& config );
    virtual void SaveConfig( ConfigString& config );

private:

    // Constructors, Destructors, and Operators
    Win32Platform( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow );
    virtual ~Win32Platform();

    // Unused Constructors, Destructors, and Operators
    Win32Platform( const Win32Platform& aPlatform );
    Win32Platform& operator=( const Win32Platform& aPlatform );

    // Methods from Platform
    virtual void InitializeFoundationDrive();
    virtual void ShutdownFoundation();

    virtual void InitializePure3D();
    virtual void ShutdownPure3D();

    // Initializes the d3d context
    void InitializeContext();

    // Resolution related methods
    static void TranslateResolution( Resolution res, int&x, int&y );
    bool IsResolutionSupported( Resolution res, int bpp ) const;

    // Mouse tracking method
    static TrackMouseEvent( MOUSETRACKER* pMouseEventTracker );
    static void CALLBACK TrackMouseTimerProc( HWND hWnd, UINT uMsg, UINT idEvent,DWORD dwTime );

    // Windows methods.
    void ResizeWindow();
    static void ShowTheCursor( bool show );
    static LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

private:

    // Pointer to the one and only instance of this singleton.
    static Win32Platform* spInstance;

    // Private Attributes
    // Had to make these static because of the initialization order problem.
    static HINSTANCE mhInstance;
    static HWND mhWnd;
    static HANDLE mhMutex;
    static bool mShowCursor;

    // Pure 3D attributes
    tPlatform* mpPlatform; 
    tContext* mpContext;

    // window properties.
    Resolution mResolution;
    int mbpp;
    bool mFullscreen;
    int mScreenWidth;
    int mScreenHeight;
};

#endif // WIN32PLATFORM_H
