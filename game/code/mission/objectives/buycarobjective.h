//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        buycarobjective.h
//
// Description: Blahblahblah
//
// History:     6/3/2003 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef BUYCAROBJECTIVE_H
#define BUYCAROBJECTIVE_H

//========================================
// Nested Includes
//========================================
#include <mission/objectives/missionobjective.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class BuyCarObjective : public MissionObjective
{
public:
    BuyCarObjective();
    virtual ~BuyCarObjective();

    void SetVehicleName( const char* name );

protected:

    virtual void OnUpdate( unsigned int elapsedTime );

private:
    char* mVehicleName;

    //Prevent wasteful constructor creation.
    BuyCarObjective( const BuyCarObjective& buycarobjective );
    BuyCarObjective& operator=( const BuyCarObjective& buycarobjective );
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

#endif //BUYCAROBJECTIVE_H
