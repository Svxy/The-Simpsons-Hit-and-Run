//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        followcondition.h
//
// Description: Blahblahblah
//
// History:     04/07/2002 + Created -- NAME
//
//=============================================================================

#ifndef FOLLOWCONDITION_H
#define FOLLOWCONDITION_H

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

class FollowCondition : public VehicleCondition
{
public:
    FollowCondition();
    virtual ~FollowCondition();

    void SetMaxDistance( float maxDist );
    float GetMaxDistance();

    void SetMinDistance( float minDist );
    float GetMinDistance();
    
    virtual void Update( unsigned int elapsedTime );

    bool IsChaseCondition();
    bool IsClose();

protected:
    void OnInitialize();
    void OnFinalize();

private:

    //Prevent wasteful constructor creation.
    FollowCondition( const FollowCondition& followcondition );
    FollowCondition& operator=( const FollowCondition& followcondition );

    float CalculateDistanceToTarget();

    static const unsigned int MIN_UPDATE_PERIOD = 100;

    float mMinDistance;
    float mMaxDistance;

    unsigned int muTime;
};

//=============================================================================
// FollowCondition::SetMaxDistance
//=============================================================================
// Description: Comment
//
// Parameters:  ( float maxDist )
//
// Return:      inline 
//
//=============================================================================
inline void FollowCondition::SetMaxDistance( float maxDist )
{
    mMaxDistance = maxDist;
}

//=============================================================================
// FollowCondition::GetMaxDistance
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
inline float FollowCondition::GetMaxDistance()
{
    return mMaxDistance;
}

//=============================================================================
// FollowCondition::SetMinDistance
//=============================================================================
// Description: Comment
//
// Parameters:  ( float minDist )
//
// Return:      inline 
//
//=============================================================================
inline void FollowCondition::SetMinDistance( float minDist )
{
    mMinDistance = minDist;
}

//=============================================================================
// FollowCondition::GetMinDistance
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      inline 
//
//=============================================================================
inline float FollowCondition::GetMinDistance()
{
    return mMinDistance;
}

#endif //FOLLOWCONDITION_H
