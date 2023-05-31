//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        constanteffect.h
//
// Description: Blahblahblah
//
// History:     6/25/2003 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef CONSTANTEFFECT_H
#define CONSTANTEFFECT_H

//========================================
// Nested Includes
//========================================
#include <input/forceeffect.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class ConstantEffect : public ForceEffect
{
public:
        ConstantEffect();
        virtual ~ConstantEffect();

        void OnInit();

        void SetMagnitude( s16 magnitude );
        void SetDirection( u16 direction );

private:

#ifdef RAD_WIN32
    DICONSTANTFORCE     m_diConstant;
    DIENVELOPE          m_diEnvelope;
#endif

        //Prevent wasteful constructor creation.
    ConstantEffect( const ConstantEffect& constanteffect );
    ConstantEffect& operator=( const ConstantEffect& constanteffect );
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

#endif //CONSTANTEFFECT_H
