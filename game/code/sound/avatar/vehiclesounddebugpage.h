//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        vehiclesounddebugpage.h
//
// Description: Declaration for VehicleSoundDebugPage class, which displays
//              vehicle sound info through Watcher
//
// History:     11/22/2002 + Created -- Darren
//
//=============================================================================

#ifndef VEHICLESOUNDDEBUGPAGE_H
#define VEHICLESOUNDDEBUGPAGE_H

//========================================
// Nested Includes
//========================================
#include <sound/sounddebug/sounddebugpage.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    VehicleSoundDebugPage
//
//=============================================================================

class VehicleSoundDebugPage : public SoundDebugPage
{
    public:
        VehicleSoundDebugPage( unsigned int pageNum, SoundDebugDisplay* master );
        virtual ~VehicleSoundDebugPage();

        void SetShiftInProgress( bool inProgress ) { m_shiftInProgress = inProgress; }
        void SetCurrentGear( int gear ) { m_currentGear = gear; }
        void SetCurrentSpeed( float speed ) { m_currentSpeed = speed; }
        void SetShiftTime( unsigned int time ) { m_shiftTime = time; }
        void SetDownshiftSpeed( float speed ) { m_downshiftSpeed = speed; }
        void SetUpshiftSpeed( float speed ) { m_upshiftSpeed = speed; }
        void SetCurrentPitch( float pitch ) { m_currentPitch = pitch; }
        void SetDamageEnabled( bool isDamaged ) { m_isDamaged = isDamaged; }
        void SetVehicleLife( float life ) { m_vehicleLife = life; }
        void SetDamageThreshold( float threshold ) { m_damageThreshold = threshold; }

    protected:
        //
        // Pure virtual functions from SoundDebugPage
        //
        void fillLineBuffer( int lineNum, char* buffer );
        int getNumLines();

    private:
        //Prevent wasteful constructor creation.
        VehicleSoundDebugPage();
        VehicleSoundDebugPage( const VehicleSoundDebugPage& original );
        VehicleSoundDebugPage& operator=( const VehicleSoundDebugPage& rhs );

        bool m_shiftInProgress;
        int m_currentGear;
        float m_currentSpeed;
        unsigned int m_shiftTime;
        float m_downshiftSpeed;
        float m_upshiftSpeed;
        float m_currentPitch;
        bool m_isDamaged;
        float m_vehicleLife;
        float m_damageThreshold;
};


#endif // VEHICLESOUNDDEBUGPAGE_H

