//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        outofboundscondition.h
//
// Description: Blahblahblah
//
// History:     24/07/2002 + Created -- NAME
//
//=============================================================================

#ifndef OUTOFBOUNDSCONDITION_H
#define OUTOFBOUNDSCONDITION_H

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

class OutOfBoundsCondition : public MissionCondition
{
public:
    OutOfBoundsCondition();
    virtual ~OutOfBoundsCondition();
   
    virtual void HandleEvent( EventEnum id, void* pEventData );

    bool IsClose();

protected:
    virtual void OnInitialize();
    virtual void OnFinalize();

private:

    //Prevent wasteful constructor creation.
    OutOfBoundsCondition( const OutOfBoundsCondition& outofboundscondition );
    OutOfBoundsCondition& operator=( const OutOfBoundsCondition& outofboundscondition );
};


#endif //OUTOFBOUNDSCONDITION_H
