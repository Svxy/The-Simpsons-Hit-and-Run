//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        keepbarrelcondition.h
//
// Description: Blahblahblah
//
// History:     5/30/2003 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef KEEPBARRELCONDITION_H
#define KEEPBARRELCONDITION_H

//========================================
// Nested Includes
//========================================
#include <mission/conditions/missioncondition.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class KeepBarrelCondition : public MissionCondition
{
public:
        KeepBarrelCondition();
        virtual ~KeepBarrelCondition();

        virtual void HandleEvent( EventEnum id, void* pEventData );

        void JumpBackBy( unsigned int num ) { mJumpBackBy = num; };

protected:
    virtual void OnInitialize();
    virtual void OnFinalize();

private:
    unsigned int mJumpBackBy;

    //Prevent wasteful constructor creation.
    KeepBarrelCondition( const KeepBarrelCondition& keepbarrelcondition );
    KeepBarrelCondition& operator=( const KeepBarrelCondition& keepbarrelcondition );
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

#endif //KEEPBARRELCONDITION_H
