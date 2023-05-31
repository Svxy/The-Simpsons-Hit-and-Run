//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        racecondition.h
//
// Description: Blahblahblah
//
// History:     24/07/2002 + Created -- NAME
//
//=============================================================================

#ifndef RACECONDITION_H
#define RACECONDITION_H

//========================================
// Nested Includes
//========================================

#include <mission/conditions/vehiclecondition.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class RaceCondition : public VehicleCondition
{
public:
    RaceCondition();
    virtual ~RaceCondition();

    virtual void HandleEvent( EventEnum id, void* pEventData );

    bool IsChaseCondition();
    bool IsClose();

protected:
    virtual void OnInitialize();
    virtual void OnFinalize();

private:

    //Prevent wasteful constructor creation.
    RaceCondition( const RaceCondition& racecondition );
    RaceCondition& operator=( const RaceCondition& racecondition );

    int m_playerWaypointsRemaining;
    int m_aiWaypointsRemaining;
};


#endif //RACECONDITION_H
