//**********************************************************
// C++ Class Name : Gamepad 
// ---------------------------------------------------------
// Filetype: (HEADER)
// Filepath: D:/simpsonsrr2/game/code/input/Gamepad.h 
// 
// 
// GDPro Properties 
// ---------------------------------------------------
//  - GD Symbol Type    : CLD_Class 
//  - GD Method         : UML ( 5.0 ) 
//  - GD System Name    : Simpsons Controller System 
//  - GD Diagram Type   : Class Diagram 
//  - GD Diagram Name   : Input Device 
// ---------------------------------------------------  
//  Author         : nharan 
//  Creation Date  : Tues - May 20, 2003 
// 
//  Change Log     : 
// 
//**********************************************************
#ifndef  GAMEPAD_H
#define  GAMEPAD_H

#include <input/RealController.h>

// We need the eGamepadButtons enum to provide a sequential equivalent to DIJOFS_X... 
// so that we can use them to access our arrays.
enum eGamepadButton
{
    GAMEPAD_X,
    GAMEPAD_Y,
    GAMEPAD_Z,
    GAMEPAD_RX,
    GAMEPAD_RY,
    GAMEPAD_RZ,
    GAMEPAD_SLIDER0,
    GAMEPAD_SLIDER1,
    GAMEPAD_POV0,
    GAMEPAD_POV1,
    GAMEPAD_BUTTON0,
    GAMEPAD_BUTTON1,
    GAMEPAD_BUTTON2,
    GAMEPAD_BUTTON3,
    GAMEPAD_BUTTON4,
    GAMEPAD_BUTTON5,
    GAMEPAD_BUTTON6,
    GAMEPAD_BUTTON7,
    GAMEPAD_BUTTON8,
    GAMEPAD_BUTTON9,
    GAMEPAD_BUTTON10,
    GAMEPAD_BUTTON11,
    GAMEPAD_BUTTON12,
    GAMEPAD_BUTTON13,
    GAMEPAD_BUTTON14,
    GAMEPAD_BUTTON15,
    GAMEPAD_BUTTON16,
    GAMEPAD_BUTTON17,
    GAMEPAD_BUTTON18,
    GAMEPAD_BUTTON19,
    GAMEPAD_BUTTON20,
    GAMEPAD_BUTTON21,
    GAMEPAD_BUTTON22,
    GAMEPAD_BUTTON23,
    GAMEPAD_BUTTON24,
    GAMEPAD_BUTTON25,
    GAMEPAD_BUTTON26,
    GAMEPAD_BUTTON27,
    GAMEPAD_BUTTON28,
    GAMEPAD_BUTTON29,
    GAMEPAD_BUTTON30,
    GAMEPAD_BUTTON31,
    NUM_GAMEPAD_BUTTONS
};

class Gamepad : public RealController
{
public:
    Gamepad () ;
    virtual ~Gamepad();

    virtual bool IsInputAxis( int dxKey ) const;
    virtual bool IsPovHat( int dxKey ) const;

    // Returns true if this is a valid input for the controller.
    virtual bool IsValidInput( int dxKey ) const;

    void CalculatePOV( float povVal, float* up, float* down, float* right, float* left ) const;

    // Sets up a mapping from a dxkey/direction to a virtual button
    virtual bool SetMap( int dxKey, eDirectionType dir, int virtualButton );
    // Retrieves the virtual button of the given type mapped to a dxKey, direction
    virtual int  GetMap( int dxKey, eDirectionType dir, eMapType map ) const;
    // Clears the specified mapping so it no longer exists.
    virtual void ClearMap( int dxKey, eDirectionType dir, int virtualButton );
    // Clears all the cached mappings.
    virtual void ClearMappedButtons();

private:
    virtual void MapInputToDICode();

    eGamepadButton GetButtonEnum( int dxKey ) const;

private:
    int m_ButtonMap[ NUM_MAPTYPES ][ NUM_GAMEPAD_BUTTONS ][ NUM_DIRTYPES ];
};
#endif
