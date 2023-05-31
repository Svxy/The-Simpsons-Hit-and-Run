//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        leaveinteriorcondition.h
//
// Description: Blahblahblah
//
// History:     30/08/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef LEAVEINTERIORCONDITION_H
#define LEAVEINTERIORCONDITION_H

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

class LeaveInteriorCondition : public MissionCondition
{
public:
    LeaveInteriorCondition();
    virtual ~LeaveInteriorCondition();

    virtual void HandleEvent( EventEnum id, void* pEventData );

    bool IsClose();

protected:
    virtual void OnInitialize();
    virtual void OnFinalize();

private:

    //Prevent wasteful constructor creation.
    LeaveInteriorCondition( const LeaveInteriorCondition& leaveinteriorcondition );
    LeaveInteriorCondition& operator=( const LeaveInteriorCondition& leaveinteriorcondition );
};


#endif //LEAVEINTERIORCONDITION_H
