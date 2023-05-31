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

//---------------------------------------------------------------------------
// Includes
//===========================================================================

#include <camera/surveillancecam.h>
#include <camera/isupercamtarget.h>

#include <presentation/gui/guisystem.h>
#include <presentation/gui/ingame/guiscreenhud.h>

#ifdef RAD_GAMECUBE
#include <main/gamecube_extras/gcmanager.h>
#include <main/gcplatform.h>

#endif

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Member Functions
//===========================================================================

//===========================================================================
// SurveillanceCam::SurveillanceCam
//===========================================================================
// Description:
//      SurveillanceCam ctor
//
// Constraints:
//
//
// Parameters:
//
// Return:
//
//===========================================================================

SurveillanceCam::SurveillanceCam():
mTarget( NULL )
{
}
//===========================================================================
// SurveillanceCam::~SurveillanceCam
//===========================================================================
// Description:
//      SurveillanceCam dtor
//
// Constraints:
//
//
// Parameters:
//
// Return:
//
//===========================================================================

SurveillanceCam::~SurveillanceCam()
{

}
//===========================================================================
// SurveillanceCam::Update
//===========================================================================
// Description:
//      Updates camera for a new frame. Reorients to target (if target is locked)
//      or swivels it around
//
// Constraints:
//
// Parameters:
//      time elapsed in milliseconds
//
// Return:
//      None
//
//===========================================================================
void 
SurveillanceCam::Update( unsigned int milliseconds )
{
    if ( mTarget != NULL )
    {
        //SetFOV( FOV );
        
        rmt::Vector targetPos;
        
        mTarget->GetPosition( &targetPos );
        
        SetCameraValues( milliseconds, mPosition, targetPos );

    }
    else
    {

    }
}


//===========================================================================
// SurveillanceCam::GetName
//===========================================================================
// Description:
//      Returns the name of the camera for debugging purposes
//
// Constraints:
//
// Parameters:
//
//
// Return:
//      
//
//===========================================================================
const char* const 
SurveillanceCam::GetName() const
{
    return "Surveillance Camera";
}
//===========================================================================
// SurveillanceCam::GetType
//===========================================================================
// Description:
//      Returns the type of camera as listed in SuperCam::Type
//
// Constraints:
//
// Parameters:
//
//
// Return:
//      
//
//===========================================================================
SuperCam::Type 
SurveillanceCam::GetType()
{
    return SURVEILLANCE_CAM;
}
//===========================================================================
// SurveillanceCam::SetTarget
//===========================================================================
// Description:
//      Sets the target for the camera to lock on to
//
// Constraints:
//
// Parameters:
//      ISuperCamTarget* 
//
// Return:
//      None
//
//===========================================================================
void 
SurveillanceCam::SetTarget( ISuperCamTarget* target )
{
    mTarget = target;
}
//===========================================================================
// SurveillanceCam::SetPosition
//===========================================================================
// Description:
//      Sets the location of the camera
//
// Constraints:
//
// Parameters:
//      const reference to the position
//
// Return:
//      None
//
//===========================================================================
void 
SurveillanceCam::SetPosition( const rmt::Vector& position )
{
    mPosition = position;
}

//=============================================================================
// SurveillanceCam::OnRegisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SurveillanceCam::OnRegisterDebugControls()
{
#ifdef RAD_GAMECUBE
    GCManager::GetInstance()->ChangeResolution( WindowSizeX / 8, WindowSizeY / 8, WindowBPP );
#endif

    // Hide the HUD Map.
    //
    GetGuiSystem()->HandleMessage( GUI_MSG_HIDE_HUD_OVERLAY, HUD_MAP );
}

//=============================================================================
// SurveillanceCam::OnUnregisterDebugControls
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void SurveillanceCam::OnUnregisterDebugControls()
{
#ifdef RAD_GAMECUBE
    GCManager::GetInstance()->ChangeResolution( WindowSizeX, WindowSizeY, WindowBPP );
#endif

    // Show the HUD Map.
    //
    GetGuiSystem()->HandleMessage( GUI_MSG_SHOW_HUD_OVERLAY, HUD_MAP );
}

