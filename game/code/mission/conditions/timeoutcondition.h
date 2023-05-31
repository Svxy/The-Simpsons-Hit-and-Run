//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        timeoutcondition.h
//
// Description: Blahblahblah
//
// History:     08/07/2002 + Created -- NAME
//
//=============================================================================

#ifndef TIMEOUTCONDITION_H
#define TIMEOUTCONDITION_H

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

class TimeOutCondition : public MissionCondition
{
public:
    TimeOutCondition();
    virtual ~TimeOutCondition();

    virtual void Update( unsigned int elapsedTime );
    void SetViolated(bool flag);

    //void SetHitNRun() { mHitNRun = true; };

    bool IsClose();

private:

    //bool mHitNRun;
    bool mDone;

    //Prevent wasteful constructor creation.
    TimeOutCondition( const TimeOutCondition& timeoutcondition );
    TimeOutCondition& operator=( const TimeOutCondition& timeoutcondition );
};


#endif //TIMEOUTCONDITION_H
