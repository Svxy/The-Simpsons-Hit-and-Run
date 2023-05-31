//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        nodamagebonusobjective.h
//
// Description: This listens for EVENT_COLLISION and if that happens the objective
//              fails.
//
// History:     12/5/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef NODAMAGEBONUSOBJECTIVE_H
#define NODAMAGEBONUSOBJECTIVE_H

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

class NoDamageBonusObjective : public BonusObjective, public EventListener
{
public:
    NoDamageBonusObjective();
    virtual ~NoDamageBonusObjective();

    virtual void Initialize();
    virtual void Finalize();
    virtual unsigned int GetNumericData();

    virtual void HandleEvent( EventEnum id, void* pEventData );

protected:
    virtual void OnReset();
    virtual void OnStart();
    virtual void OnUpdate( unsigned int milliseconds );

private:
    unsigned int mElapsedTime;

    //Prevent wasteful constructor creation.
    NoDamageBonusObjective( const NoDamageBonusObjective& nodamagebonusobjective );
    NoDamageBonusObjective& operator=( const NoDamageBonusObjective& nodamagebonusobjective );
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// NoDamageBonusObjective::GetNumericData
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
inline unsigned int NoDamageBonusObjective::GetNumericData()
{
    return mElapsedTime;
}

#endif //NODAMAGEBONUSOBJECTIVE_H
