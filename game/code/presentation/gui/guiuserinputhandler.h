//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiUserInputHandler
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

#ifndef GUIUSERINPUTHANDLER_H
#define GUIUSERINPUTHANDLER_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <input/mappable.h>

//===========================================================================
// Forward References
//===========================================================================
class CGuiSystem;

//===========================================================================
// Interface Definitions
//===========================================================================
namespace GuiInput
{
    // Definition of control points for an abstracted player controller
    //
    enum eGuiInput
    {
        UNKNOWN = -1,

        Left,
        Right,
        Up,
        Down,

        NUM_DPAD_INPUTS,

        L1,
        R1,
        XAxis,
        YAxis,
        XAxisRight,
        YAxisRight,
        Start,
        Select,
        Back,
        AuxX,
        AuxY,

        //These are just for the PS2 GT Wheel.
        AuxStart,
        AuxSelect,
        AuxBack,
        AuxUp,
        AuxDown,
        AuxXAxis,

#ifdef RAD_WIN32
        P1_KBD_Start,
        P1_KBD_Select,
        P1_KBD_Back,
        P1_KBD_Left,
        P1_KBD_Right,
#endif

        NUM_GUI_INPUTS
    };
};

class CGuiUserInputHandler : public Mappable
{
public:

    CGuiUserInputHandler( void );
    virtual ~CGuiUserInputHandler( void );

    void Left( int controllerId = 0 );
    void Right( int controllerId = 0 );
    void Up( int controllerId = 0 );
    void Down( int controllerId = 0 );
    void Start( int controllerId = 0 );
    void Select( int controllerId = 0 );
    void Back( int controllerId = 0 );
    void AuxX( int controllerId = 0 );
    void AuxY( int controllerId = 0 );
    void L1( int controllerId = 0 );
    void R1( int controllerId = 0 );

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

    // Update repeated input states
    void Update( unsigned int elapsedTime, unsigned int controllerId );
    
    bool IsXAxisOnLeft() const;
    bool IsXAxisOnRight() const;
    bool IsYAxisOnUp() const;
    bool IsYAxisOnDown() const;

    void EnableStartToSelectMapping( bool isEnabled ) { m_isStartToSelectMappingEnabled = isEnabled; }

private:
    // Disallow object copying or assigning until we know we need it
    //
    CGuiUserInputHandler( const CGuiUserInputHandler& original );
    CGuiUserInputHandler& operator=( const CGuiUserInputHandler& rhs );

    void ResetRepeatableButtons();

    int m_buttonDownDuration[ GuiInput::NUM_DPAD_INPUTS ];

    float m_XAxisValue;
    float m_YAxisValue;

    int m_XAxisDuration;
    int m_YAxisDuration;

#ifdef RAD_WIN32
    float m_RightValue;
    float m_LeftValue;
    float m_UpValue;
    float m_DownValue;

    int m_RightDuration;
    int m_LeftDuration;
    int m_UpDuration;
    int m_DownDuration;
#endif

    bool m_isStartToSelectMappingEnabled : 1;

};

#endif // GUIUSERINPUTHANDLER_H
