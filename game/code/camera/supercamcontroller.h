//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        supercamcontroller.h
//
// Description: Blahblahblah
//
// History:     14/05/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef SUPERCAMCONTROLLER_H
#define SUPERCAMCONTROLLER_H

//========================================
// Nested Includes
//========================================
#include <input/mappable.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class SuperCamController : public Mappable
{
public:
    enum Button
    {
        stickX,
        stickY,
        zToggle,
        lookToggle,
        zoom,
        in, 
        out,
        lookBack,
        altLookBack,
        A,
        toggleFirstPerson,
        l2,
        r2,
        leftStickY,
        start,
        cameraToggle
#ifdef RAD_WIN32
        ,stickXup,
        stickXdown,
        stickYup,
        stickYdown,
        carLookLeft,
        carLookRight,
        carLookUp,
        mouseLookUp,
        mouseLookDown
#endif
    };

    SuperCamController();
    virtual ~SuperCamController();

    virtual void OnButton( int controllerId, int id, const IButton* pButton );

    // This method is called when a button changes state from "Pressed" to "Released".
    //
	virtual void OnButtonUp( int controllerId, int buttonId, const IButton* pButton );

    // This method is called when a button changes state from "Released" to "Pressed".
    //
	virtual void OnButtonDown( int controllerId, int buttonId, const IButton* pButton );

    // This is how we create our controller device mappings to logical game mappings.
    // The mappings set up in this method are platform specific.
    //
    // The basic format of the calls is to "Map" a input, to a enumerated output id.
    // The output of the specified input will be contained in the Button[] array.
    // This id will also be sent as a the second parameter in the OnButton... messages.
    //
    virtual void LoadControllerMappings( unsigned int controllerId );

    bool IsWheel() { return mIsWheel; };

    // A specific camera button value accessor.
    // On PC it's specialized to process axes.
    // On consoles it's optimized out and replaced with GetValue().
    float GetAxisValue( unsigned int buttonId ) const;

private:

    bool mIsWheel;

    //Prevent wasteful constructor creation.
    SuperCamController( const SuperCamController& supercamcontroller );
    SuperCamController& operator=( const SuperCamController& supercamcontroller );
};
//*****************************************************************************
//
// Inline Public Member Functions
//
//*****************************************************************************

//=============================================================================
// SuperCamController::OnButton
//=============================================================================
// Description: Comment
//
// Parameters:  ( int controllerId, int id, const IButton* pButton )
//
// Return:      void 
//
//=============================================================================
inline void SuperCamController::OnButton( int controllerId, int id, const IButton* pButton )
{
}

//=============================================================================
// SuperCamController::OnButtonUp
//=============================================================================
// Description: Comment
//
// Parameters:  ( int controllerId, int buttonId, const IButton* pButton )
//
// Return:      void 
//
//=============================================================================
inline void SuperCamController::OnButtonUp( int controllerId, int buttonId, const IButton* pButton )
{
}

//=============================================================================
// SuperCamController::GetAxisValue
//=============================================================================
// Description: A specific camera button value accessor.
//              On PC it's specialized to process axes.
//              On consoles it's optimized out and replaced with GetValue().
//              This is the cleanest way to implement camera controls for PC.
//
// Parameters:  n/a
//
// Return:      n/a
//
//=============================================================================
inline float SuperCamController::GetAxisValue( unsigned int buttonId ) const
{
#ifdef RAD_WIN32
    switch( buttonId )
    {
    case stickX:
        {
            float up = GetValue( stickXup );
            float down = GetValue( stickXdown );

            return ( up > down ) ? up : -down;
        }
    case stickY:
        {
            float up = GetValue( stickYup );
            float down = GetValue( stickYdown );

            return ( up > down ) ? up : -down;
        }
    default:
        {
            return GetValue( buttonId );
        }
    }
#else
    return GetValue( buttonId );
#endif
}

#endif //SUPERCAMCONTROLLER_H
