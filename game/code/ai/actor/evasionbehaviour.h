//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   EvasionBehaviour
//
// Description: Evades the player, dodging away from it
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef EVASIONBEHAVIOUR_H
#define EVASIONBEHAVIOUR_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <ai/actor/behaviour.h>
#include <radmath/spline.hpp>
#include <radmath/random.hpp>

//===========================================================================
// Forward References
//===========================================================================



//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================


//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//
// Constraints:
//
//===========================================================================
class EvasionBehaviour : public Behaviour
{
    public:
        EvasionBehaviour( float minEvadeDistHoriz, float maxEvadeDistHoriz, float minEvadeDistVert, float maxEvadeDistVert, float speedInKPH );
        virtual ~EvasionBehaviour();
        virtual void Apply( Actor*, unsigned int timeInMS );

        virtual void Activate();
        virtual void Deactivate();

        void SetSpeed( float kph );

    protected:
        
        // Evasion speed in meters per milliseconds
        float m_EvadeSpeed;
        float m_MinEvadeDistHoriz;
        float m_MaxEvadeDistHoriz;
        float m_MinEvadeDistVert;
        float m_MaxEvadeDistVert;

        rmt::Spline m_MotionPathSpline;
        // We update distance by deltaT along the spline. 
        float m_DeltaTPerMilliSecond; 
        // Current position in the range 0 to 1 along the spline
        float m_T;

        static rmt::Randomizer s_Randomizer;
        static bool s_RandomizerSeeded;

    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow EvasionBehaviour from being copied and assigned.
        EvasionBehaviour( const EvasionBehaviour& );
        EvasionBehaviour& operator=( const EvasionBehaviour& );

    private:
    
        // Finds a new evasion point from the current actor position
        // on returning success, spline, deltaT, and evasion destination are all updated
        // if false, a point could not be found
        bool FindEvasionDestination( const rmt::Vector& actorPosition, rmt::Vector* dest );
        bool FindGroundHeight( float x, float z, float* out_height )const;
};



#endif