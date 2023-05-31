//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        buyskinobjective.h
//
// Description: Blahblahblah
//
// History:     6/3/2003 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef BUYSKINOBJECTIVE_H
#define BUYSKINOBJECTIVE_H

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

class BuySkinObjective : public MissionObjective
{
public:
        BuySkinObjective();
        virtual ~BuySkinObjective();

        void SetSkinName( const char* name );

protected:

    virtual void OnUpdate( unsigned int elapsedTime );

private:
    char* mSkinName;

        //Prevent wasteful constructor creation.
    BuySkinObjective( const BuySkinObjective& buyskinobjective );
    BuySkinObjective& operator=( const BuySkinObjective& buyskinobjective );
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

#endif //BUYSKINOBJECTIVE_H
