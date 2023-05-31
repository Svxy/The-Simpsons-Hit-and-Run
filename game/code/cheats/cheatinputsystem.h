//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CheatInputSystem
//
// Description: Interface for the CheatInputSystem class.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2002/09/19      TChu        Created for SRR2
//
//===========================================================================

#ifndef CHEATINPUTSYSTEM_H
#define CHEATINPUTSYSTEM_H

//===========================================================================
// Nested Includes
//===========================================================================

#include <cheats/cheatinputs.h>
#include <cheats/cheats.h>

//===========================================================================
// Forward References
//===========================================================================

class CheatsDB;
class CheatInputHandler;

struct ICheatEnteredCallback
{
    virtual void OnCheatEntered( eCheatID cheatID, bool isEnabled ) = 0;
};

const unsigned int MAX_NUM_CHEAT_CALLBACKS = 32;

//===========================================================================
// Interface Definitions
//===========================================================================

class CheatInputSystem
{
public:
    // Static Methods for accessing this singleton.
    static CheatInputSystem* CreateInstance();
    static void DestroyInstance();
    static CheatInputSystem* GetInstance();

	CheatInputSystem();
    virtual ~CheatInputSystem();

    void Init();
    void Reset() { s_cheatsEnabled = 0; }

    void SetEnabled( bool enable );
    bool IsEnabled() const { return m_enabled; }

    void SetActivated( int controllerId, bool activated );
    bool IsActivated( int controllerId ) const;

    void SetCheatEnabled( eCheatID cheatID, bool enable );
    bool IsCheatEnabled( eCheatID cheatID ) const;

    void ReceiveInputs( eCheatInput* cheatInputs,
                        int numInputs = NUM_CHEAT_SEQUENCE_INPUTS );

    CheatsDB* GetCheatsDB() const { return m_cheatsDB; }

    void RegisterCallback( ICheatEnteredCallback* callback );
    void UnregisterCallback( ICheatEnteredCallback* callback );

private:
    //---------------------------------------------------------------------
    // Private Functions
    //---------------------------------------------------------------------

    // No copying or assignment. Declare but don't define.
    //
    CheatInputSystem( const CheatInputSystem& );
    CheatInputSystem& operator= ( const CheatInputSystem& );

    //---------------------------------------------------------------------
    // Private Data
    //---------------------------------------------------------------------

    // Pointer to the one and only instance of this singleton.
    static CheatInputSystem* spInstance;

    bool m_enabled;
    unsigned int m_activatedBitMask;

    static CHEATBITMASK s_cheatsEnabled;
    CheatsDB* m_cheatsDB;

    CheatInputHandler* m_cheatInputHandler;

    ICheatEnteredCallback* m_clientCallbacks[ MAX_NUM_CHEAT_CALLBACKS ];
    int m_numClientCallbacks;

};

// A little syntactic sugar for getting at this singleton.
inline CheatInputSystem* GetCheatInputSystem() { return( CheatInputSystem::GetInstance() ); }

#endif // CHEATINPUTSYSTEM_H
