//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        gooutsideobjective.h
//
// Description: Just like the GoToObjective except we dont do the interior check.
//
// History:     15/04/2002 + Created -- NAME
//
//=============================================================================

#ifndef GOOUTSIDEOBJECTIVE_H
#define GOOUTSIDEOBJECTIVE_H

//========================================
// Nested Includes
//========================================

#include <mission/objectives/gotoobjective.h>


//========================================
// Forward References
//========================================


//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class GoOutsideObjective : public GoToObjective
{
public:

    GoOutsideObjective();
    virtual ~GoOutsideObjective(); 
    

protected:
    virtual void OnInitialize();    
   
private:
    
};

#endif //GOOUTSIDEOBJECTIVE_H
