//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   SurveillanceCam
//
// Description: A surveillance cam, derived from SuperCam. The camera
//              is at a fixed position, such as mounted on a call
//              and can rotate to lock onto a target
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef SURVEILLANCECAM_H
#define SURVEILLANCECAM_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <camera/supercam.h>


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
//      A Surveillance camera
//
// Constraints:
//
//
//===========================================================================
class SurveillanceCam : public SuperCam
{
    public:
        SurveillanceCam();
        virtual ~SurveillanceCam();
 
        //Update: Called when you want the super cam to update its state.
        virtual void Update( unsigned int milliseconds );

        //Returns the name of the super cam.  
        //This can be used in the FE or debug info
        virtual const char* const GetName() const;

        // Forces the camera to lookat the suspect
        virtual void SetTarget( ISuperCamTarget* target ); 

        virtual Type GetType();

        // Sets the position of the camera, it will remain fixed but can rotate
        void SetPosition( const rmt::Vector& position );

        void OnRegisterDebugControls();
        void OnUnregisterDebugControls();

    protected:

        // Camera target 
        ISuperCamTarget* mTarget;

        // Current camera position
        rmt::Vector mPosition; 

private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow XxxClassName from being copied and assigned.
        SurveillanceCam( const SurveillanceCam& );
        SurveillanceCam& operator=( const SurveillanceCam& );

};

#endif