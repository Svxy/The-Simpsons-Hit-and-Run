//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        steeringspring.h
//
// Description: Blahblahblah
//
// History:     10/19/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef STEERINGSPRING_H
#define STEERINGSPRING_H

//========================================
// Nested Includes
//========================================
#include <radcontroller.hpp>
#include <input/forceeffect.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class SteeringSpring : public ForceEffect
{
public:
    SteeringSpring();
	virtual ~SteeringSpring();

    void OnInit();
    void SetCenterPoint( s8 degrees, u8 deadband ); //Where 0 is straight up.
#ifdef RAD_WIN32
    void SetSpringStrength( u16 strength );
#else
    void SetSpringStrength( u8 strength );
#endif
    void SetSpringCoefficient( s16 coeff );

private:

#ifdef RAD_WIN32
    DICONDITION m_conditon;
#endif
    //Prevent wasteful constructor creation.
	SteeringSpring( const SteeringSpring& steeringspring );
	SteeringSpring& operator=( const SteeringSpring& steeringspring );
};


#endif //STEERINGSPRING_H


