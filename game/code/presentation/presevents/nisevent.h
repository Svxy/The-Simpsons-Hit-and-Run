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

#ifndef NISEVENT_H
#define NISEVENT_H

//========================================
// Nested Includes
//========================================

#include <presentation/presevents/presentationevent.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class NISEvent : public PresentationEvent
{
    public:
        NISEvent();
        ~NISEvent();

        enum NISType
        {
            NIS_CAMERA,
            NIS_SCENEGRAPH,
            NUM_NIS_TYPES
        };

        NISType type;

        char controller[32];
        char camera[32];
        char animation[32];

        void LoadNow();
        void LoadFromInventory();

        AnimationPlayer* GetPlayer();

        virtual void Init();

        virtual void Start();
    protected:
    private:
        void SetNames();

        bool mbHasSetNames;
};

#endif //NISEVENT_H

