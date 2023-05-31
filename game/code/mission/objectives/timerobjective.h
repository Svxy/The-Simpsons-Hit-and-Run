//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        TimerObjective.h
//
// Description: Used as a dummy stage that needs to loiter for a few seconds 
//
// History:     May. 21. 2003 + Created -- Chuck C.
//
//=============================================================================

#ifndef TIMEROBJECTIVE_H
#define TIMEROBJECTIVE_H

//========================================
// Nested Includes
//========================================
#include <mission/objectives/missionobjective.h>
#include <events/eventdata.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Timer Objective
//
//=============================================================================

class TimerObjective : public MissionObjective
{
public:
   

    TimerObjective();
    virtual ~TimerObjective();
    
    void SetTimer(unsigned int milliseconds);
       
protected:
    virtual void OnInitialize();
    virtual void OnFinalize();
    virtual void OnUpdate(unsigned int elapsedTime);
    virtual void Update(unsigned int elaspedTime);
private:
    
    unsigned int mDurationTime;
    unsigned int mElapsedTime;
    //Prevent wasteful constructor creation.
    TimerObjective( const TimerObjective& TimerObjective );
    TimerObjective& operator=( const TimerObjective& TimerObjective );
};


#endif //COINOBJECTIVE_H
