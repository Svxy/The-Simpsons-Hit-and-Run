//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        nocopbonusobjective.h
//
// Description: Blahblahblah
//
// History:     12/6/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef NOCOPBONUSOBJECTIVE_H
#define NOCOPBONUSOBJECTIVE_H

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

class NoCopBonusObjective  : public BonusObjective, public EventListener
{
public:
    NoCopBonusObjective();
    virtual ~NoCopBonusObjective();

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
    NoCopBonusObjective( const NoCopBonusObjective& nocopbonusobjective );
    NoCopBonusObjective& operator=( const NoCopBonusObjective& nocopbonusobjective );
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// NoCopBonusObjective::GetNumericData
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      unsigned 
//
//=============================================================================
inline unsigned int NoCopBonusObjective::GetNumericData()
{
    return mElapsedTime;
}

#endif //NoCopBonusObjective