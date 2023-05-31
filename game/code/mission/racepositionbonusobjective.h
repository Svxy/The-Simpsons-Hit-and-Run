//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        racepositionbonusobjective.h
//
// Description: Blahblahblah
//
// History:     12/6/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef RACEPOSITIONBONUSOBJECTIVE_H
#define RACEPOSITIONBONUSOBJECTIVE_H

//========================================
// Nested Includes
//========================================
#include <mission/bonusobjective.h>
#include <events/eventlistener.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class RacePositionBonusObjective : public BonusObjective, public EventListener
{
public:
    RacePositionBonusObjective();
    virtual ~RacePositionBonusObjective();

    virtual void Initialize();
    virtual void Finalize();
    virtual unsigned int GetNumericData();

    virtual void HandleEvent( EventEnum id, void* pEventData );

    void SetDesiredPosition( unsigned int position );

protected:
    virtual void OnReset();
    virtual void OnStart();
    virtual void OnUpdate( unsigned int milliseconds );

private:
    unsigned int mPosition;
    unsigned int mDesiredPosition;

    //Prevent wasteful constructor creation.
    RacePositionBonusObjective( const RacePositionBonusObjective& racepositionbonusobjective );
    RacePositionBonusObjective& operator=( const RacePositionBonusObjective& racepositionbonusobjective );
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// RacePositionBonusObjective::GetNumericData
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
inline unsigned int RacePositionBonusObjective::GetNumericData()
{
    return mPosition;
}

//=============================================================================
// RacePositionBonusObjective::SetDesiredPosition
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int position )
//
// Return:      void 
//
//=============================================================================
inline void RacePositionBonusObjective::SetDesiredPosition( unsigned int position )
{
    mDesiredPosition = position;
}

#endif //RACEPOSITIONBONUSOBJECTIVE_H
