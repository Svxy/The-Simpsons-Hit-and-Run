//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        .h
//
// Description: Blahblahblah
//
// History:     23/05/2002 + Created -- NAME
//
//=============================================================================

#ifndef TRANSEVENT_H
#define TRANSEVENT_H

//========================================
// Nested Includes
//========================================

#include <presentation/presevents/presentationevent.h>
#include <presentation/transitionplayer.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class TransitionEvent : public PresentationEvent
{
    public:
        TransitionPlayer::TransitionInfo transInfo;

        AnimationPlayer* GetPlayer();

        virtual void Start();
    protected:
};

#endif //TRANSEVENT_H

