//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        gcmanager.h
//
// Description: Blahblahblah
//
// History:     14/06/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef GCMANAGER_H
#define GCMANAGER_H

//========================================
// Nested Includes
//========================================
#include <radtime.hpp>  // IRadTimerCallback

//========================================
// Forward References
//========================================
#include <dolphin/pad.h>

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class GCManager : public IRadTimerCallback
{
public:
    static GCManager* GetInstance();

    void Init();

    void Reset();
    void PerformReset( bool displaySplash = true, bool launchIPL = false );

    //For IRadTimerCallback
    void OnTimerDone( unsigned int elapsedTime, void * pUserData ); 
    
    void ChangeResolution( int x, int y, int bpp );

    void DoProgressiveScanTest();

    // MKR - adding StopRumble, turn off rumble immediately (for use when the GC cover is opened)
    void StopRumble();

    void TestForReset();

private:
    static GCManager* mInstance;

    bool mReset;
    bool mDoingReset;

    bool mControllerReset[PAD_MAX_CONTROLLERS];
    unsigned int mResetTime[PAD_MAX_CONTROLLERS];

    // Timer for updates.
    IRadTimer* mTimer;

    GCManager();
    virtual ~GCManager();

    void ControllerReset();
    void StopEverything();

    //Prevent wasteful constructor creation.
    GCManager( const GCManager& gcmanager );
    GCManager& operator=( const GCManager& gcmanager );
};


//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

#endif //GCMANAGER_H
