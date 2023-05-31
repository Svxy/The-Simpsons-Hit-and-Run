//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        ps2platform.h
//
// Description: Abstracts the differences for setting up and shutting down
//              the different platforms.
//
// History:     + Stolen and cleaned up from Svxy -- Darwin Chau
//
//=============================================================================

#ifndef PS2PLATFORM_H
#define PS2PLATFORM_H

//========================================
// Nested Includes
//========================================
#include <main/platform.h> // base class

//========================================
// Forward References
//========================================
struct IRadCementLibrary;
struct IRadMemoryHeap;
class tPlatform;
class tContext;

//
// Global function for disabling MFIFO, which we do to use radTextDisplay.
// I'm making it global because this also gets used in the heart of radSound,
// and I don't want to drag P3D dependencies in there.  It's icky, and I'm
// going to ask for a fix to radTextDisplay so we don't need to do this anymore.
//
void Simpsons2MFIFODisable();

//=============================================================================
//
// Synopsis:    Provides abstraction for setting up and closing down the PS2.
//
//=============================================================================
class PS2Platform : public Platform
{
    public:
    
        // Static Methods for accessing this singleton.
        static PS2Platform* CreateInstance();
        static PS2Platform* GetInstance();
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

        void SetProgressiveMode( bool progressiveScan );
        bool GetProgressiveMode() { return mProgressiveMode; }

        bool CheckForStartupButtons( void );

    protected:

        virtual void InitializeFoundationDrive();
        virtual void ShutdownFoundation();

        virtual void InitializePure3D();
        virtual void ShutdownPure3D();
    
    private:

        // Constructors, Destructors, and Operators
        PS2Platform();
        virtual ~PS2Platform();
        
        // Unused Constructors, Destructors, and Operators
        PS2Platform( const PS2Platform& aPlatform );
        PS2Platform& operator=( const PS2Platform& aPlatform );

        // PS2 specific methods
        void EnableSnProfiler();

        // Crash printout routines
#ifndef FINAL
        static void dumpExceptionData( const char* exceptionName,
                                       unsigned int stat,
                                       unsigned int cause,
                                       unsigned int epc,
                                       unsigned int bva,
                                       unsigned int bpa,
                                       u_long128* registers );

        static void handleTLBChange( unsigned int stat,
                                     unsigned int cause,
                                     unsigned int epc,
                                     unsigned int bva,
                                     unsigned int bpa,
                                     u_long128* registers );

        static void handleTLBLoadMismatch( unsigned int stat,
                                           unsigned int cause,
                                           unsigned int epc,
                                           unsigned int bva,
                                           unsigned int bpa,
                                           u_long128* registers );

        static void handleTLBStoreMismatch( unsigned int stat,
                                            unsigned int cause,
                                            unsigned int epc,
                                            unsigned int bva,
                                            unsigned int bpa,
                                            u_long128* registers );

        static void handleAddressLoadError( unsigned int stat,
                                            unsigned int cause,
                                            unsigned int epc,
                                            unsigned int bva,
                                            unsigned int bpa,
                                            u_long128* registers );

        static void handleAddressStoreError( unsigned int stat,
                                             unsigned int cause,
                                             unsigned int epc,
                                             unsigned int bva,
                                             unsigned int bpa,
                                             u_long128* registers );

        static void handleBusFetchError( unsigned int stat,
                                         unsigned int cause,
                                         unsigned int epc,
                                         unsigned int bva,
                                         unsigned int bpa,
                                         u_long128* registers );

        static void handleBusDataError( unsigned int stat,
                                        unsigned int cause,
                                        unsigned int epc,
                                        unsigned int bva,
                                        unsigned int bpa,
                                        u_long128* registers );

        static void handleReservedInstruction( unsigned int stat,
                                               unsigned int cause,
                                               unsigned int epc,
                                               unsigned int bva,
                                               unsigned int bpa,
                                               u_long128* registers );

        static void handleCoprocessor( unsigned int stat,
                                       unsigned int cause,
                                       unsigned int epc,
                                       unsigned int bva,
                                       unsigned int bpa,
                                       u_long128* registers );

        static void handleOverflow( unsigned int stat,
                                    unsigned int cause,
                                    unsigned int epc,
                                    unsigned int bva,
                                    unsigned int bpa,
                                    u_long128* registers );

        static void handleTrap( unsigned int stat,
                                unsigned int cause,
                                unsigned int epc,
                                unsigned int bva,
                                unsigned int bpa,
                                u_long128* registers );
#endif

        // Pointer to the one and only instance of this singleton.
        static PS2Platform* spInstance;

        // Pure 3D attributes
        tPlatform* mpPlatform; 
        tContext* mpContext;

        bool mProgressiveMode;
        
        static IRadCementLibrary* s_MainCement;
};

#endif // PS2PLATFORM_H
