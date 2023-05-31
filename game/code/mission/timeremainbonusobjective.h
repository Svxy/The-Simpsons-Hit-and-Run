//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        timeremainbonusobjective.h
//
// Description: Blahblahblah
//
// History:     12/6/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef TIMEREMAINBONUSOBJECTIVE_H
#define TIMEREMAINBONUSOBJECTIVE_H

//========================================
// Nested Includes
//========================================
#include <mission/bonusobjective.h>
#include <events/eventlistener.h>

//========================================
// Forward References
//========================================
class Mission;

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class TimeRemainBonusObjective : public BonusObjective
{
public:
    TimeRemainBonusObjective();
    virtual ~TimeRemainBonusObjective();

    virtual void Initialize();
    virtual void Finalize();
    virtual unsigned int GetNumericData();

    void SetTime( unsigned int time );
    void SetMission( Mission* mission );

protected:
    virtual void OnReset();
    virtual void OnStart();
    virtual void OnUpdate( unsigned int milliseconds );

private:
    unsigned int mMinTime;
    Mission* mMission;

    //Prevent wasteful constructor creation.
    TimeRemainBonusObjective( const TimeRemainBonusObjective& timeremainbonusobjective );
    TimeRemainBonusObjective& operator=( const TimeRemainBonusObjective& timeremainbonusobjective );
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// TimeRemainBonusObjective::SetTime
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int time )
//
// Return:      void 
//
//=============================================================================
inline void TimeRemainBonusObjective::SetTime( unsigned int time )
{
    mMinTime = time;
}

//=============================================================================
// TimeRemainBonusObjective::SetMission
//=============================================================================
// Description: Comment
//
// Parameters:  ( Mission* mission )
//
// Return:      void 
//
//=============================================================================
inline void TimeRemainBonusObjective::SetMission( Mission* mission )
{
    mMission = mission;
}

#endif //TIMEREMAINBONUSOBJECTIVE_H
