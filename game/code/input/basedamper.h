//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        basedamper.h
//
// Description: Blahblahblah
//
// History:     10/19/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef BASEDAMPER_H
#define BASEDAMPER_H

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

class BaseDamper : public ForceEffect
{
public:
    BaseDamper();
	virtual ~BaseDamper();

    void OnInit();

    void SetCenterPoint( s8 degrees, u8 deadband ); //Where 0 is straight up.
#ifdef RAD_WIN32
    void SetDamperStrength( u16 strength );
#else
    void SetDamperStrength( u8 strength );
#endif
    void SetDamperCoefficient( s16 coeff );
private:

#ifdef RAD_WIN32
    DICONDITION m_conditon;
#endif

    //Prevent wasteful constructor creation.
	BaseDamper( const BaseDamper& basedamper );
	BaseDamper& operator=( const BaseDamper& basedamper );
};


#endif //BASEDAMPER_H
