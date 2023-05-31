//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        GCManager.cpp
//
// Description: Implement GCManager
//
// History:     14/06/2002 + Created -- Cary Brisebois
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <dolphin.h>
#include <dolphin/os.h>
#include <dolphin/lg.h>
#include <dolphin/dvd.h>

// Foundation Tech
#include <raddebug.hpp>
#include <raddebugwatch.hpp>
#include <radFile.hpp>
#include <radcontroller.hpp>
#include <radthread.hpp>


//========================================
// Project Includes
//========================================
#include <main/gamecube_extras/GCManager.h>
#include <main/gcplatform.h>
#include <main/game.h>
#include <gameflow/gameflow.h>
#include <main/platform.h>
#include <memory/srrmemory.h>
#include <input/inputmanager.h>

#include <debug/profiler.h>

#include <sound/soundmanager.h>

#include <data/gamedatamanager.h>

#include <presentation/fmvplayer/fmvplayer.h>
#include <presentation/presentation.h>

unsigned int MIN_TIME = 500; //0.5 seconds in milliseconds, as per doc

#ifdef DEBUGWATCH
int sX = 640;
int sY = 480;
#endif

//*****************************************************************************
//
// Call backs
//
//*****************************************************************************

void ResetButtonCallBack( void )
{
    if ( OSGetResetButtonState() )
    {
        //Inform the GCManager
        GCManager::GetInstance()->Reset();        
    }
    else
    {
        //Reset the callback the last one was bogus.
    	OSSetResetCallback( ResetButtonCallBack );   
    }
}

void CheckForGCNReset()
{
    GCManager::GetInstance()->TestForReset();
}


#ifdef DEBUGWATCH
void ChangeResolutionCallback( void* userData )
{
	sY = sX * 12;
	sX *= 16;
	
    GCManager::GetInstance()->ChangeResolution( sX, sY, 32 );
}
#endif

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

GCManager* GCManager::mInstance = NULL;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

GCManager* GCManager::GetInstance()
{
MEMTRACK_PUSH_GROUP( "GCManager" );
    if ( !mInstance )
    {
        mInstance = new(GMA_PERSISTENT) GCManager();
    }
MEMTRACK_POP_GROUP( "GCManager" );

    return mInstance;
}

//=============================================================================
// GCManager::Init
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GCManager::Init()
{
    //We want a callback to test for reset
    GetGame()->GetTimerList()->CreateTimer( &mTimer, 500, this );
}

//=============================================================================
// GCManager::Reset
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GCManager::Reset()
{
    mReset = true;
}

//=============================================================================
// GCManager::OnTimerDone
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int elapsedTime, void * pUserData )
//
// Return:      void 
//
//============================================================================= 
void GCManager::OnTimerDone( unsigned int elapsedTime, void * pUserData )
{
    BEGIN_PROFILE( "GCManager" );
    if ( mReset )
    {
        PerformReset();
    }

    TestForReset();

    END_PROFILE( "GCManager" );
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//==============================================================================
// GCManager::GCManager
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
GCManager::GCManager() :
    mDoingReset( false ),
    mTimer( NULL )
{
    unsigned int i;
    for ( i = 0; i < Input::MaxControllers; ++i )
    {
        mControllerReset[i] = false;
        mResetTime[i] = 0;
    }

    //Set up the reset callback.
    OSSetResetCallback( ResetButtonCallBack );   
    
#ifdef DEBUGWATCH
    radDbgWatchAddFunction( "Set Resolution", &ChangeResolutionCallback, this, "GCManager" );

    radDbgWatchAddInt( &sX, "X Resolution (multiple of 16)", "GCManager", NULL, NULL, 1, 40 );
//    radDbgWatchAddInt( &sY, "Y Resolution", "GCManager", NULL, NULL, 16, 480 );
#endif
}

//==============================================================================
// GCManager::~GCManager
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
GCManager::~GCManager()
{
    if ( mTimer )
    {
        mTimer->Release();
    }
}

//=============================================================================
// GCManager::TestForReset
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GCManager::TestForReset()
{

    unsigned int inputCount = 0;
    PADStatus controllers[PAD_MAX_CONTROLLERS];

    for ( inputCount = 0; inputCount < PAD_MAX_CONTROLLERS; ++inputCount )
    {
        controllers[ inputCount ].button = 0;
    }

    PADRead(controllers);
    //while( PAD_ERR_TRANSFER == PADRead(controllers) ){  ::radThreadSleep( 32 ); }

    LGPosition lgStatus[ SI_MAX_CHAN ];
    memset( lgStatus, 0, sizeof( LGPosition )*SI_MAX_CHAN  );

    extern bool g_isLGInitialized;
    if( g_isLGInitialized )
    {
        LGRead( lgStatus );
    }

    rAssert( PAD_MAX_CONTROLLERS == SI_MAX_CHAN );
    for ( inputCount = 0; inputCount < PAD_MAX_CONTROLLERS; ++inputCount )
    {
        if( lgStatus[inputCount].err == LG_ERR_NONE )
        {
            controllers[inputCount].err = PAD_ERR_NONE;
            controllers[inputCount].button = 0;

            if( lgStatus[inputCount].button & LG_BUTTON_B )
            {
                controllers[inputCount].button |= PAD_BUTTON_B;
            }
            if( lgStatus[inputCount].button & LG_BUTTON_X )
            {
                controllers[inputCount].button |= PAD_BUTTON_X;
            }
            if( lgStatus[inputCount].button & LG_BUTTON_START )
            {
                controllers[inputCount].button |= PAD_BUTTON_MENU;
            }
        }

        if ( controllers[ inputCount ].err == PAD_ERR_NONE &&
            controllers[ inputCount ].button & PAD_BUTTON_B &&
            controllers[ inputCount ].button & PAD_BUTTON_X &&
            controllers[ inputCount ].button & PAD_BUTTON_MENU )
        {
            if ( mControllerReset[inputCount] )
            {
                if ( OSTicksToMilliseconds( OSGetTime() ) - mResetTime[inputCount] > MIN_TIME )
                {
                    mControllerReset[inputCount] = false;
                    mResetTime[inputCount] = 0;

                    //Call reset
                    ControllerReset();
                    break;
                }
            }
            else
            {
                if ( mResetTime[inputCount] == 0 )
                {
                    if(GetGameFlow()==NULL)return;
                    ContextEnum context = GetGameFlow()->GetCurrentContext();

                    if ( context == CONTEXT_LOADING_SUPERSPRINT ||
                         context == CONTEXT_SUPERSPRINT ||
                         context == CONTEXT_LOADING_GAMEPLAY ||
                         context == CONTEXT_GAMEPLAY ||
                         context == CONTEXT_PAUSE )
                    {
                        if ( GetInputManager()->GetControllerPlayerIDforController( inputCount ) >= 0 )
                        {
                            mResetTime[inputCount] = OSTicksToMilliseconds( OSGetTime() );
                            mControllerReset[inputCount] = true;
                        }
                    }
                    else
                    {
                        mResetTime[inputCount] = OSTicksToMilliseconds( OSGetTime() );
                        mControllerReset[inputCount] = true;
                    }

                }
            }            
        }
        else
        {
            mResetTime[inputCount] = 0;
            mControllerReset[inputCount] = false;
        }
    }
    
    if ( OSGetResetButtonState() )
    {
    	mReset = true;
    } 
   
    if ( mReset )
    {
        //Bye bye!
        PerformReset();
    }
}

//=============================================================================
// GCManager::ControllerReset
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GCManager::ControllerReset()
{
    //TODO: Test if we should reset here or not!
    //For now...
    Reset();

    //Potentially, we could reset to the main menu...  That might be better.
}

//=============================================================================
// GCManager::PerformReset
//=============================================================================
// Description: Comment
//
// Parameters:  ( bool displaySplash )
//
// Return:      void 
//
//=============================================================================
void GCManager::PerformReset( bool displaySplash, bool launchIPL )
{
    // Wait until the switch is released
	if ( !mDoingReset && ( mReset ) && ( !OSGetResetButtonState( ) ) )
	{
        mDoingReset = true;

        if ( !GetGame()->GetPlatform()->PausedForErrors() )
        {
            // If we have troubles with sound, we should shut it down here. Don't foget
            // to service the sound system enough time for the changes to be reflected.

            if ( GetPresentationManager()->GetFMVPlayer()->IsPlaying() )
            {
                GetPresentationManager()->GetFMVPlayer()->Pause( );
            }
            else
            {
                GetSoundManager()->StopForMovie();

                while( !( GetSoundManager()->IsStoppedForMovie() ) )
                {
                    ::radMovieService2( );
                    ::radFileService( );
                    SoundManager::GetInstance()->Update();
                    SoundManager::GetInstance()->UpdateOncePerFrame( 0, NUM_CONTEXTS, false, true );
                }
            }
        }

        if ( displaySplash && !launchIPL && DVDCheckDisk() )
        {
            //Fade to black
         //   GetGame()->GetPlatform()->DisplaySplashScreen( Platform::FadeToBlack, NULL, 1.0f, 1.0f, 0.0f, tColour(0,0,0), 100 );
        }

        StopEverything();

        GXDrawDone();

		//
		// One last TRC when we reset
		//
	 	VISetBlack(true);
	 	VIFlush();
        VIWaitForRetrace();

        //TODO: If we have progressive scan mode enabled, we need to keep it enabled on reset.
        //This could be tricky.

        if ( launchIPL || !DVDCheckDisk() )
        {
            // Reset the system
            OSResetSystem( 
                OS_RESET_HOTRESET, 
                0,
                launchIPL );
        }
        else
        {
            // Reset the system
            OSResetSystem( 
                OS_RESET_RESTART,
                0,
                false );
        }
	}
}

//=============================================================================
// GCManager::StopEverything
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GCManager::StopEverything()
{
    //Stop the controllers.
    StopRumble();

    //Wait for the memory cards to complete.
    while ( GetGameDataManager()->IsUsingDrive() )
    {
        ::radFileService();
        ::radControllerSystemService();
        GetGameDataManager()->Update( 16 );
    }

    ::radControllerTerminate();

    //Shutdown the platform.
#ifdef RAD_RELEASE
    //GetGame()->GetPlatform()->ShutdownPlatform();  //This causes a crash in FTech...
#endif
}

//=============================================================================
// GCManager::ChangeResolution
//=============================================================================
// Description: Comment
//
// Parameters:  ( int x, int y, int bpp )
//
// Return:      void 
//
//=============================================================================
void GCManager::ChangeResolution( int x, int y, int bpp )
{
    rAssert( x % 16 == 0 );  //This must be a multiple of 16pixels!

    GetGame()->GetPlatform()->DisplaySplashScreen( Platform::Error, NULL, NULL, 0.0f, 0.0f, tColour(0,0,0), 0 );
    p3d::context->GetDisplay()->InitDisplay( x, y, bpp );
}

//=============================================================================
// GCManager::DoProgressiveScanTest
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void GCManager::DoProgressiveScanTest()
{
    //Test for button b being held down and ask if they want progressive mode
    //Or not.
    bool doProgressiveQuestion = false;

    if ( VIGetDTVStatus() != 1 )
    {
        return;
    }

    //There's a cable present..

    //If the progressive mode flagis set, then we make the message show also.
    if ( OSGetProgressiveMode() )
    {
        //The flag is set from last time.
        doProgressiveQuestion = true;
    }
    else
    {
        unsigned int inputCount = 0;
        PADStatus controllers[PAD_MAX_CONTROLLERS];

        for ( inputCount = 0; inputCount < PAD_MAX_CONTROLLERS; ++inputCount )
        {
            controllers[ inputCount ].button = 0;
        }
        
        ::radThreadSleep( 32 );

        PADRead(controllers);

        for ( inputCount = 0; inputCount < PAD_MAX_CONTROLLERS && !doProgressiveQuestion; ++inputCount )
        {
            if ( controllers[ inputCount ].err == PAD_ERR_NONE &&
                 controllers[ inputCount ].button & PAD_BUTTON_B )
            {
                //Someone is holding B.
                doProgressiveQuestion = true;
                break;            
            }
        }

        ::radControllerSystemService();
        ::radThreadSleep( 16 );
        ::radControllerSystemService();
        ::radThreadSleep( 16 );

        LGPosition lgStatus[ SI_MAX_CHAN ];
        memset( lgStatus, 0, sizeof( LGPosition )*SI_MAX_CHAN  );
        LGRead( lgStatus );

        for ( inputCount = 0; inputCount < SI_MAX_CHAN && !doProgressiveQuestion; ++inputCount )
        {
            if ( lgStatus[ inputCount ].err == LG_ERR_NONE &&
                 lgStatus[ inputCount ].button & LG_BUTTON_B )
            {
                //Someone is holding B.
                doProgressiveQuestion = true;
                break;            
            }
        }
    }

    if ( doProgressiveQuestion )
    {
        //Display the question.
        //TODO: Localization
        GetGame()->GetPlatform()->DisplaySplashScreen( Platform::Error, "Do you want to display in progressive mode?", 0.7f );

        bool setProgressiveOn = ((GCPlatform*)GetGame()->GetPlatform())->DisplayYesNo( 0.7f, -0.2f, -0.1f, 0.1f, -0.1f );
        
        if ( setProgressiveOn != ((GCPlatform*)GetGame()->GetPlatform())->GetInitData()->progressive )
        {

            //Do the TRC delay stuff.

            //Set the screen black
            VISetBlack( TRUE );
            VIFlush();
            VIWaitForRetrace();

            unsigned int i;
            for( i=0; i<10; i++ )
            {	//Set screen black and wait for several frames
                VIWaitForRetrace();
            }

            //Change modes
            ((GCPlatform*)GetGame()->GetPlatform())->GetInitData()->progressive = setProgressiveOn;

            //Need to re-init the display.
            p3d::context->GetDisplay()->InitDisplay( ((GCPlatform*)GetGame()->GetPlatform())->GetInitData() );  

            for( i=0; i<100; i++ )
            {	//Set screen black and wait for several dozen frames
                VIWaitForRetrace();
            }

            VISetBlack(FALSE);
            VIFlush();
            VIWaitForRetrace();
        }

        //Set progressive scan mode
        OSSetProgressiveMode( setProgressiveOn );

        //Display message that progressive is enabled
        //TODO: localization
        if ( setProgressiveOn && OSGetProgressiveMode() )
        {
            GetGame()->GetPlatform()->DisplaySplashScreen( Platform::Error, "Screen has been set to progressive mode.\n Press the A Button to continue.", 0.7f );
        }
        else if ( !setProgressiveOn && !OSGetProgressiveMode() )
        {
            GetGame()->GetPlatform()->DisplaySplashScreen( Platform::Error, "Screen has been set to interlaced mode.\n Press the A Button to continue.", 0.7f );
        }

		if ( setProgressiveOn == OSGetProgressiveMode() )
		{
	        //Wait for input or 10 second timeout.
	        unsigned int time = OSTicksToMilliseconds( OSGetTime() );

	        bool waiting = true;

	        PADStatus controllers[PAD_MAX_CONTROLLERS];
	        PADStatus lastControllers[PAD_MAX_CONTROLLERS];

	        unsigned int inputCount = 0;
	        
	        for ( inputCount = 0; inputCount < PAD_MAX_CONTROLLERS; ++inputCount )
	        {
	            controllers[ inputCount ].button = 0;
	            lastControllers[ inputCount ].button = 0;
	        }

            LGPosition lgStatus[ SI_MAX_CHAN ];
            memset( lgStatus, 0, sizeof( LGPosition )*SI_MAX_CHAN  );

	        while ( waiting )
	        {
                ::radControllerSystemService();
                ::radThreadSleep( 32 );
	            PADRead(controllers);
                LGRead( lgStatus );

	            for ( inputCount = 0; inputCount < PAD_MAX_CONTROLLERS; ++inputCount )
	            {
                    if( lgStatus[inputCount].err == LG_ERR_NONE )
                    {
                        controllers[inputCount].err = PAD_ERR_NONE;
                        controllers[inputCount].button = 0;

                        if( lgStatus[inputCount].button & LG_BUTTON_A )
                        {
                            controllers[inputCount].button = PAD_BUTTON_A;
                        }
                        if( lgStatus[inputCount].button & LG_BUTTON_START )
                        {
                            controllers[inputCount].button = PAD_BUTTON_MENU;
                        }
                    }

	                if ( controllers[ inputCount ].err == PAD_ERR_NONE &&
	                     ( !( controllers[ inputCount ].button & PAD_BUTTON_A ) &&
	                       lastControllers[ inputCount ].button & PAD_BUTTON_A ) ||
	                     ( !( controllers[ inputCount ].button & PAD_BUTTON_MENU ) &&
	                       lastControllers[ inputCount ].button & PAD_BUTTON_MENU ) )
	                { 
	                    waiting = false;
	                    break;
	                }

	                if( controllers[ inputCount ].err == PAD_ERR_NONE ) 
	                {
				        lastControllers[ inputCount ] = controllers[ inputCount ];
			        }
	            }

                unsigned int newTime = OSTicksToMilliseconds( OSGetTime() );
	            if (  newTime - time > 10000 )
	            {
	                waiting = false;
	            }
	        }
	    }
	}
}

//=============================================================================
// GCManager::StopRumble
//=============================================================================
// Description: Lets get ready to stop rumbling!
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================

void GCManager::StopRumble()
{
    PADControlMotor( PAD_CHAN0, PAD_MOTOR_STOP_HARD ); 
    PADControlMotor( PAD_CHAN1, PAD_MOTOR_STOP_HARD ); 
    PADControlMotor( PAD_CHAN2, PAD_MOTOR_STOP_HARD ); 
    PADControlMotor( PAD_CHAN3, PAD_MOTOR_STOP_HARD );
}
