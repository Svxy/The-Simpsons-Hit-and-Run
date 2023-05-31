//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   Platform
//
// Description: Pure virtual interface that abstracts the differences for
//              setting up and shutting down the different platforms.
//
// History:     + Stolen and cleaned up from Svxy -- Darwin Chau
//              + Added the Splash screens + dashboard -- Cary Brisebois
//
//=============================================================================

#ifndef PLATFORM_H
#define PLATFORM_H

//========================================
// Nested Includes
//========================================
#include <p3d/p3dtypes.hpp> // tColour

#include <radfile.hpp>

struct IRadDrive;

//=============================================================================
//
// Synopsis:    Implement this interface for each platform that the game will
//              support (e.g. PS2, GameCube, Xbox, etc.).
//
//=============================================================================
class Platform : public IRadDriveErrorCallback
{
    public:
        
        // NOTE: Each derived platform must also implement the following
        //       static function.  This has to be static because we need
        //       to init FTech before anything is allocated.
        //
        // static void InitializeFoundation();

        virtual void InitializePlatform() = 0;    
        virtual void ShutdownPlatform() = 0;

        virtual void LaunchDashboard() = 0;
        virtual void ResetMachine() = 0;

        //These are the baked in screens
        enum SplashScreen
        {
            License,    //Shows the baked in license screen as the background
            Error,      //Displays a blank black background
            FadeToBlack
        };
        
        virtual void DisplaySplashScreen( SplashScreen screenID, 
                                          const char* overlayText = NULL, 
                                          float fontScale = 1.0f, 
                                          float textPosX = 0.0f,
                                          float textPosY = 0.0f,
                                          tColour textColour = tColour( 255,255,255 ),
                                          int fadeFrames = 3 ) = 0;

        virtual void DisplaySplashScreen( const char* textureName,
                                          const char* overlayText = NULL, 
                                          float fontScale = 1.0f, 
                                          float textPosX = 0.0f,
                                          float textPosY = 0.0f,
                                          tColour textColour = tColour( 255,255,255 ),
                                          int fadeFrames = 3 ) = 0;
        virtual void OnControllerError(const char *msg) = 0;

        //Override this if you wanna support it.
        virtual bool OnDriveError( radFileError error, const char* pDriveName, void* pUserData ) { return false; };  
        void ClearControllerError() { OnDriveError(Success, NULL, NULL);}
        bool PausedForErrors() const { return mPauseForError; };
        bool IsControllerError() const {  return mErrorState==CTL_ERROR; };
        IRadDrive* GetHostDrive( void ) const { return mpIRadDrive; }
        
    protected:
        enum ErrorState 
        { 
            NONE,
            P_ERROR,
            CTL_ERROR
        };

        ErrorState mErrorState;

        virtual void InitializeFoundationDrive() = 0;
        virtual void ShutdownFoundation() = 0;
        
        virtual void InitializePure3D() = 0;
        virtual void ShutdownPure3D() = 0;

        bool mPauseForError;

        // Foundation attributes
        IRadDrive* mpIRadDrive;

        Platform() : mPauseForError( false ), mpIRadDrive( 0 ) {};
};

#endif // PLATFORM_H
