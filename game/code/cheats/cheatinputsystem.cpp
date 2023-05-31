//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CheatInputSystem
//
// Description: Implementation of the CheatInputSystem class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/09/19      TChu        Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================
#include <cheats/cheatinputsystem.h>
#include <cheats/cheatinputhandler.h>

#include <events/eventmanager.h>
#include <input/inputmanager.h>
#include <memory/srrmemory.h>
#include <mission/charactersheet/charactersheetmanager.h>

#include <presentation/gui/guisystem.h>
#include <presentation/gui/guiscreen.h>
#include <presentation/gui/guiwindow.h> 
#include <presentation/gui/guimanager.h> 

// Static pointer to instance of singleton.
CheatInputSystem* CheatInputSystem::spInstance = NULL;

//===========================================================================
// Local Constants
//===========================================================================

CHEATBITMASK CheatInputSystem::s_cheatsEnabled = 0;

//===========================================================================
// Public Member Functions
//===========================================================================

//==============================================================================
// CheatInputSystem::CreateInstance
//==============================================================================
//
// Description: - Creates the Game Data Manager.
//
// Parameters:	None.
//
// Return:      Pointer to the CheatInputSystem.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
CheatInputSystem* CheatInputSystem::CreateInstance()
{
MEMTRACK_PUSH_GROUP( "CheatInputSystem" );
    spInstance = new( GMA_PERSISTENT ) CheatInputSystem;
    rAssert( spInstance != NULL );
MEMTRACK_POP_GROUP( "CheatInputSystem" );

    return spInstance;
}

//==============================================================================
// CheatInputSystem::DestroyInstance
//==============================================================================
//
// Description: Destroy the Game Data Manager.
//
// Parameters:	None.
//
// Return:      None.
//
//==============================================================================
void CheatInputSystem::DestroyInstance()
{
    rAssert( spInstance != NULL );

    delete( GMA_PERSISTENT, spInstance );
    spInstance = NULL;
}

//==============================================================================
// CheatInputSystem::GetInstance
//==============================================================================
//
// Description: - Access point for the CheatInputSystem singleton.  
//              - Creates the CheatInputSystem if needed.
//
// Parameters:	None.
//
// Return:      Pointer to the CheatInputSystem.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
CheatInputSystem* CheatInputSystem::GetInstance()
{
    rAssert( spInstance != NULL );

    return spInstance;
}

//===========================================================================
// CheatInputSystem::CheatInputSystem
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
CheatInputSystem::CheatInputSystem()
:   m_enabled( false ),
    m_activatedBitMask( 0 ),
    m_cheatsDB( NULL ),
    m_cheatInputHandler( NULL ),
    m_numClientCallbacks( 0 )
{
    for( unsigned int i = 0; i < sizeof( m_clientCallbacks ) /
                                 sizeof( m_clientCallbacks[ 0 ] ); i++ )
    {
        m_clientCallbacks[ i ] = NULL;
    }
}

//===========================================================================
// CheatInputSystem::~CheatInputSystem
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
CheatInputSystem::~CheatInputSystem()
{
    // clean-up memory
    //
    if( m_cheatsDB != NULL )
    {
        delete( GMA_PERSISTENT, m_cheatsDB );
        m_cheatsDB = NULL;
    }

    if( m_cheatInputHandler != NULL )
    {
        m_cheatInputHandler->Release();
        m_cheatInputHandler = NULL;
    }
}

//===========================================================================
// CheatInputSystem::Init
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void
CheatInputSystem::Init()
{
MEMTRACK_PUSH_GROUP( "CheatInputSystem Init" );
    // create (one and only) instance of Cheats DB
    //
    m_cheatsDB = new( GMA_PERSISTENT ) CheatsDB();
    rAssert( m_cheatsDB );

    // create (one and only) cheat input handler
    //
    m_cheatInputHandler = new( GMA_PERSISTENT ) CheatInputHandler;
    rAssert( m_cheatInputHandler );
    m_cheatInputHandler->AddRef();
MEMTRACK_POP_GROUP( "CheatInputSystem Init" );
}

//===========================================================================
// CheatInputSystem::SetEnabled
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void
CheatInputSystem::SetEnabled( bool enable )
{
    m_enabled = enable;

    int maxControllers = GetInputManager()->GetMaxControllers();
    for( int i = 0; i < maxControllers; i++ )
    {
        if( enable )
        {
            GetInputManager()->RegisterMappable( i, m_cheatInputHandler );
        }
        else
        {
            GetInputManager()->UnregisterMappable( i, m_cheatInputHandler );
        }
    }

    rAssert( m_cheatInputHandler );
    m_cheatInputHandler->ResetInputSequence();
    m_cheatInputHandler->ResetTriggerStates();
}

//===========================================================================
// CheatInputSystem::SetActivated
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void
CheatInputSystem::SetActivated( int controllerId, bool activated )
{
    if( activated )
    {
        m_activatedBitMask |= (1 << controllerId);
    }
    else
    {
        m_activatedBitMask &= ~(1 << controllerId);
    }

#ifndef RAD_GAMECUBE
    // TC: *** temporary work-around to GC-specific problem w/ L and R
    //         triggers constantly sending alternating UP/DOWN inputs
    //
    rAssert( m_cheatInputHandler );
    m_cheatInputHandler->ResetInputSequence();
#endif
}

//===========================================================================
// CheatInputSystem::IsActivated
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
bool
CheatInputSystem::IsActivated( int controllerId ) const
{
    return ((m_activatedBitMask & (1 << controllerId)) > 0);
}

//===========================================================================
// CheatInputSystem::SetCheatEnabled
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void
CheatInputSystem::SetCheatEnabled( eCheatID cheatID, bool enable )
{
#ifdef FINAL
    if( cheatID == CHEAT_ID_UNLOCK_CARDS ||
        cheatID == CHEAT_ID_UNLOCK_SKINS ||
        cheatID == CHEAT_ID_UNLOCK_VEHICLES )
    {
        // for these cheats, unless all missions are completed, don't do
        // anything
        //
        if( !GetCharacterSheetManager()->IsAllStoryMissionsCompleted() )
        {
            return;
        }
    }
#endif

    // turn on/off corresponding bit in cheats bitmask
    //
    if( enable )
    {
        s_cheatsEnabled |= (1 << cheatID);
    }
    else
    {
        s_cheatsEnabled &= ~(1 << cheatID);
    }

    // notify registered clients that valid cheat code has been entered
    //
    for( int i = 0; i < m_numClientCallbacks; i++ )
    {
        rAssert( m_clientCallbacks[ i ] );
        m_clientCallbacks[ i ]->OnCheatEntered( cheatID, enable );
    }
}

//===========================================================================
// CheatInputSystem::IsCheatEnabled
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
bool
CheatInputSystem::IsCheatEnabled( eCheatID cheatID ) const
{
    return ((s_cheatsEnabled & (1 << cheatID)) > 0);
}

//===========================================================================
// CheatInputSystem::ReceiveInputs
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void
CheatInputSystem::ReceiveInputs( eCheatInput* cheatInputs,
                                 int numInputs )
{
    int cheatIndex = CheatsDB::ConvertSequenceToIndex( cheatInputs,
                                                       numInputs );

    // get cheatID associated with received input sequence
    //
    rAssert( m_cheatsDB );
    eCheatID cheatID = m_cheatsDB->GetCheatID( cheatIndex );

    // validate cheatID
    //
    if( cheatID != CHEAT_ID_UNREGISTERED )
    {
        // Yay! Successful cheat code entered!!
        //
#ifdef FINAL
        // TC: toggling cheats on/off isn't really supported for all cheats, so
        //     letz just not allow this in the final build
        // 
        bool isCheatEnabled = true;
#else
        bool isCheatEnabled = !this->IsCheatEnabled( cheatID ); // toggle
#endif

        this->SetCheatEnabled( cheatID, isCheatEnabled );

        if( this->IsCheatEnabled( cheatID ) )
        {
            GetEventManager()->TriggerEvent( EVENT_FE_CHEAT_SUCCESS );
        }
        else
        {
            GetEventManager()->TriggerEvent( EVENT_FE_CHEAT_FAILURE );

            isCheatEnabled = false;

            rReleasePrintf( "*** This cheat cannot be enabled until all story missions have been completed!\n" );
        }

        rReleasePrintf( "*** Cheat code successfully entered: %s (%s)\n",
                        m_cheatsDB->GetCheat( cheatID )->m_cheatName,
                        isCheatEnabled ? "enabled" : "disabled" );

        // if this is the "unlock everything" cheat, then unlock everything (duh...)
        //
        if( cheatID == CHEAT_ID_UNLOCK_EVERYTHING )
        {
            for( int unlockCheatID = CHEAT_ID_UNLOCK_BEGIN;
                 unlockCheatID < CHEAT_ID_UNLOCK_EVERYTHING;
                 unlockCheatID++ )
            {
                this->SetCheatEnabled( static_cast<eCheatID>( unlockCheatID ), isCheatEnabled );
            }
        }
        else if ( cheatID == CHEAT_ID_DEMO_TEST )
        {
            GetGuiSystem()->GotoScreen( CGuiWindow::GUI_SCREEN_ID_SPLASH, 0, 0, CLEAR_WINDOW_HISTORY );
        }
    }
    else
    {
        // Booo.... try again!!
        //
        GetEventManager()->TriggerEvent( EVENT_FE_CHEAT_FAILURE );

        rReleasePrintf( "*** Invalid cheat code entered!\n" );
    }
}

//===========================================================================
// CheatInputSystem::RegisterCallback
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void
CheatInputSystem::RegisterCallback( ICheatEnteredCallback* callback )
{
    rAssert( callback != NULL );

    // first, check if callback is already registered
    //
    for( int i = 0; i < m_numClientCallbacks; i++ )
    {
        if( m_clientCallbacks[ i ] == callback )
        {
            // yup, ignore redundant request
            return;
        }
    }

    rAssert( static_cast<unsigned int>( m_numClientCallbacks ) < 
             sizeof( m_clientCallbacks ) / sizeof( m_clientCallbacks[ 0 ] ) );

    // add new registered callback
    //
    m_clientCallbacks[ m_numClientCallbacks ] = callback;
    m_numClientCallbacks++;
}

//===========================================================================
// CheatInputSystem::UnregisterCallback
//===========================================================================
// Description: 
//
// Constraints:	None.
//
// Parameters:	None.
//
// Return:      
//
//===========================================================================
void
CheatInputSystem::UnregisterCallback( ICheatEnteredCallback* callback )
{
    // search for callback
    //
    for( int i = 0; i < m_numClientCallbacks; i++ )
    {
        if( m_clientCallbacks[ i ] == callback )
        {
            // found it! now remove it
            //
            m_clientCallbacks[ i ] = NULL;
            m_numClientCallbacks--;

            // if removed from the middle, replace w/ one at the end
            //
            if( i < m_numClientCallbacks )
            {
                m_clientCallbacks[ i ] = m_clientCallbacks[ m_numClientCallbacks ];
                m_clientCallbacks[ m_numClientCallbacks ] = NULL;
            }

            // all done, return
            return;
        }
    }

    // callback not found!
    //
    rAssertMsg( 0, "WARNING: *** Callback not found!" );
}

//===========================================================================
// Private Member Functions
//===========================================================================

