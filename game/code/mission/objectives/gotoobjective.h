//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        gotoobjective.h
//
// Description: Blahblahblah
//
// History:     15/04/2002 + Created -- NAME
//
//=============================================================================

#ifndef GOTOOBJECTIVE_H
#define GOTOOBJECTIVE_H

//========================================
// Nested Includes
//========================================

#include <mission/objectives/missionobjective.h>

#include <meta/locatorevents.h>

//========================================
// Forward References
//========================================

class Locator;
class AnimatedIcon;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class GoToObjective : public MissionObjective
{
public:
    GoToObjective();
    virtual ~GoToObjective();
    
    Locator* GetDestinationLocator() { return( mDestinationLocator ); }
    void SetDestinationLocator( Locator* pLocator ) { mDestinationLocator = pLocator; }

    void SetDestinationNames( char* locatorname, char* p3dname, float scale );
    void SetCollectEffectName( char* name );
    void SetGotoDialogOff() { mGotoDialogOn = false; }

    virtual void HandleEvent( EventEnum id, void* pEventData );

    void SetMustActionTrigger() { mActionTrigger = true; };

protected:
    virtual void OnInitialize();
    virtual void OnFinalize();
    virtual void OnUpdate( unsigned int elapsedTime );

    PathStruct mArrowPath;
    char mDestinationLocatorName[ 32 ];
	char mDestinationDrawableName[ 32 ];
    char mEffectName[32];

    Locator* mDestinationLocator;
	AnimatedIcon* mAnimatedIcon;
    float mScaleFactor;

    int mHudMapIconID;

    AnimatedIcon* mCollectEffect;
    bool mWaitingForEffect;

    bool mGotoDialogOn;

    bool mActionTrigger;
    LocatorEvent::Event mOldEvent;
    int mOldData;
   
private:
    
};

#endif //GOTOOBJECTIVE_H
