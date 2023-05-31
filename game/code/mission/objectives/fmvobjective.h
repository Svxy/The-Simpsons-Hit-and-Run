//=============================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// File:        fmvobjective.h
//
// Description: The FMV objective is used to play an FMV during a mission. Usually at mission end.
//
// History:     04 Feb 2003 + Created -- James Harrison
//
//=============================================================================

#ifndef FMVOBJECTIVE_H
#define FMVOBJECTIVE_H

//========================================
// Nested Includes
//========================================
#include <mission/objectives/missionobjective.h>
#include <presentation/presevents/presentationevent.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class FMVObjective :    public MissionObjective,
                        public PresentationEvent::PresentationEventCallBack
{
public:
    enum { FMV_FILE_NAME_LEN = 13 }; // Enforce 8.3 filenaming.

    FMVObjective();
    virtual ~FMVObjective();

    void SetFileName( const char* name );
    void SetMusicStop() { mMusicStop = true; }

protected:
    virtual void OnInitialize();
    virtual void OnFinalize() {}; // nop.
    virtual void OnPresentationEventBegin( PresentationEvent* pEvent ) {};  // nop.
    virtual void OnPresentationEventLoadComplete( PresentationEvent* pEvent ) {};   // nop.
    virtual void OnPresentationEventEnd( PresentationEvent* pEvent );

private:
    char mFileName[ FMV_FILE_NAME_LEN ];
    bool mMusicStop;

//    FMVEventData mFMVEventData;

    //Prevent wasteful constructor creation.
    FMVObjective( const FMVObjective& That );
    FMVObjective& operator=( const FMVObjective& That );
};


#endif //FMVOBJECTIVE_H
