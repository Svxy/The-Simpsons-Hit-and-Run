//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   FMVUserInputHandler
//
// Description: This class feeds the inputs received by the controller
//              system into the GUI system.
//
// Authors:     Darwin Chau
//              Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2000/10/20		DChau		Created
//                  2002/05/29      TChu        Modified for SRR2
//
//===========================================================================

#ifndef FMVUSERINPUTHANDLER_H
#define FMVUSERINPUTHANDLER_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <input/mappable.h>

//===========================================================================
// Forward References
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================
namespace FMVInput
{
    // Definition of control points for an abstracted player controller
    //
    enum FMVInputEnum
    {
        UNKNOWN = -1,

        Skip,
        Start,

        NUM_FMV_INPUTS
    };
};

class FMVUserInputHandler : public Mappable
{
public:

    FMVUserInputHandler( void );
    virtual ~FMVUserInputHandler( void );

    // Mappable interface declarations
    //
    virtual void OnButton( int controllerId, int buttonId, const IButton* pButton );
	virtual void OnButtonUp( int controllerId, int buttonId, const IButton* pButton );
	virtual void OnButtonDown( int controllerId, int buttonId, const IButton* pButton );

    // Mappable interface declarations.
    // Dispatch a message when controller is disconnected.
    //
	virtual void OnControllerDisconnect( int id );

    // Mappable interface declarations.
    // Dispatch a message when controller is connected.
    //
	virtual void OnControllerConnect( int id );

    // Mappable interface declarations
    //
    virtual void LoadControllerMappings( unsigned int controllerId );

    inline bool IsEnabled() const { return m_isEnabled; }
    inline void SetEnabled( bool isEnabled ) { m_isEnabled = isEnabled; }


private:
    // Disallow object copying or assigning until we know we need it
    //
    FMVUserInputHandler( const FMVUserInputHandler& original );
    FMVUserInputHandler& operator=( const FMVUserInputHandler& rhs );

    bool m_isEnabled : 1;
    bool m_controllerPromptShown : 1;
    bool m_controllerReconnect;

};

#endif // FMVUSERINPUTHANDLER_H
