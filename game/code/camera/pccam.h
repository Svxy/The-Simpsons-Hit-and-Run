//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        pccam.h
//
// Description: Blahblahblah
//
// History:     7/25/2003 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef PCCAM_H
#define PCCAM_H

//========================================
// Nested Includes
//========================================
#include <camera/supercam.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class PCCam : public SuperCam
{
public:
        PCCam();
        virtual ~PCCam();

        //Update: Called when you want the super cam to update its state.
        virtual void Update( unsigned int milliseconds );
        virtual void UpdateForPhysics( unsigned int milliseconds );

        //Returns the name of the super cam.  
        //This can be used in the FE or debug info
        virtual const char* const GetName() const { return "PC_CAM"; };

        virtual Type GetType() { return SuperCam::PC_CAM; };

        //These are for favourable support of this command
        virtual void SetTarget( ISuperCamTarget* target ) { mTarget = target; }; 

        virtual unsigned int GetNumTargets() const { return mTarget == NULL ? 0 : 1; };

        virtual void SetCollisionOffset( const rmt::Vector* offset, unsigned int numCollisions, const rmt::Vector& groundOffset );
        virtual float GetIntersectionRadius() const;

        SuperCamController* GetController() const { return mController; }

        void SetPitchVelocity( float speed ) { m_fPitchVelocity += speed; }
        void SetYawVelocity( float speed )   { m_fYawVelocity += speed;   }

        float GetAngularSpeed()  const { return m_fAngularSpeed;  }
        float GetYawVelocity()   const { return m_fYawVelocity;   }
        float GetPitchVelocity() const { return m_fPitchVelocity; }


protected:
    virtual void OnInit() { InitMyController(); };

private:
    ISuperCamTarget* mTarget;
    rmt::Vector mGroundOffset;
    const rmt::Vector* mCollisionOffset;
    unsigned int mNumCollisions;
    float mFOVDelta;
    float m_fAngularSpeed,
          m_fYawVelocity,
          m_fPitchVelocity;

    float m_fInterpolationSpeed;
    int mLastPCCamFacing;

    //Prevent wasteful constructor creation.
    PCCam( const PCCam& pccam );
    PCCam& operator=( const PCCam& pccam );
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

#endif //PCCAM_H
