//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        positioncondition.h
//
// Description: Blahblahblah
//
// History:     11/20/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef POSITIONCONDITION_H
#define POSITIONCONDITION_H

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

class PositionCondition : public VehicleCondition
{
public:
    PositionCondition();
	virtual ~PositionCondition();

    virtual void HandleEvent( EventEnum id, void* pEventData );

    void SetRequiredPosition( int position );

private:
    virtual void OnInitialize();
    virtual void OnFinalize();

    int mRequiredPosition;
    int mNumOthersFinished;

    //Prevent wasteful constructor creation.
	PositionCondition( const PositionCondition& positioncondition );
	PositionCondition& operator=( const PositionCondition& positioncondition );
};

//******************************************************************************
//
// Inline Public Member Functions
//
//******************************************************************************

//=============================================================================
// PositionCondition::SetRequiredPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( int position )
//
// Return:      void 
//
//=============================================================================
inline void PositionCondition::SetRequiredPosition( int position )
{
    mRequiredPosition = position;
}

#endif //POSITIONCONDITION_H
