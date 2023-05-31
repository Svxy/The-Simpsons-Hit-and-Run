//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   cutcambehaviour
//
// Description: While inside a trigger volume, the actor will switch to its POV
//              
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef CUTCAM_BEHAVIOUR_H
#define CUTCAM_BEHAVIOUR_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <ai/actor/behaviour.h>
#include <radmath/vector.hpp>
#include <events/eventlistener.h>

//===========================================================================
// Forward References
//===========================================================================

class SurveillanceCam;
class Actor;
//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================


//===========================================================================
// Interface Definitions
//===========================================================================


class CutCamBehaviour : public Behaviour, public EventListener
{
    public:
        CutCamBehaviour( float radius );
        virtual ~CutCamBehaviour();
        virtual void Apply( Actor*, unsigned int timeInMS );
        virtual void HandleEvent( EventEnum id, void* pEventData );

        virtual void Activate();
        virtual void Deactivate();

    protected:
        
        float m_TriggerRadiusSqr;
        bool m_SwitchingEnabled;

        // Camera for use when the character gets really close to the camera
        // and wants to switch to using it
        SurveillanceCam* mpCamera;
        bool m_CutCamViewActivated;
        unsigned int m_SuperCamID;
        bool m_Registered; // Is the camera registered with the supercam central
        Actor* m_ParentActor;

        void SwitchToCutCam();
        void RevertCamera();


    private:
        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow CutCamBehaviour from being copied and assigned.
        CutCamBehaviour( const CutCamBehaviour& );
        CutCamBehaviour& operator=( const CutCamBehaviour& );
    private:

};

#endif