//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   XboxPlatform   
//
// Description: Abstracts the differences for setting up and shutting down
//              the different platforms.
//
// History:     + Stolen and cleaned up from Svxy -- Darwin Chau
//
//=============================================================================

#ifndef XBOXPLATFORM_H
#define XBOXPLATFORM_H

//========================================
// Nested Includes
//========================================
#include "platform.h" // base class

//========================================
// Forward References
//========================================
struct IRadMemoryHeap;
class tPlatform;
class tContext;

//=============================================================================
//
// Synopsis:    Provides abstraction for setting up and closing down the XBox.
//
//=============================================================================
class XboxPlatform : public Platform
{
    public:
    
        // Static Methods for accessing this singleton.
        static XboxPlatform* CreateInstance();
        static XboxPlatform* GetInstance();
        static void DestroyInstance();

        // Had to workaround our nice clean design cause FTech must be init'ed
        // before anything else is done.
        static void InitializeFoundation();
        static void InitializeMemory();
        
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

	
    protected:

        virtual void InitializeFoundationDrive();
        virtual void ShutdownFoundation();
        
        virtual void InitializePure3D();
        virtual void ShutdownPure3D();
    
    private:
        
        // Constructors, Destructors, and Operators
        XboxPlatform();
        virtual ~XboxPlatform();

        // Unused Constructors, Destructors, and Operators
        XboxPlatform( const XboxPlatform& aPlatform );
        XboxPlatform& operator=( const XboxPlatform& aPlatform );

        // Pointer to the one and only instance of this singleton.
        static XboxPlatform* spInstance;

        // Pure 3D attributes
        tPlatform* mpPlatform; 
        tContext* mpContext;
};

#endif // XBOXPLATFORM_H
