//**********************************************************
// C++ Class Name : Keyboard 
// ---------------------------------------------------------
// Filetype: (HEADER)
// Filepath: D:/simpsonsrr2/game/code/input/Keyboard.h 
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
#ifndef  KEYBOARD_H
#define  KEYBOARD_H

#include <input/RealController.h>

const NUM_KEYBOARD_BUTTONS = 256;

class Keyboard : public RealController
{
public: 
	Keyboard();
	virtual ~Keyboard();

    // Returns true if this is a valid input for the controller.
    virtual bool IsValidInput( int dxKey ) const;

    // Returns true if the key is banned for mapping.
    virtual bool IsBannedInput( int dxKey ) const;

    // Sets up a mapping from a dxkey/direction to a virtual button
    virtual bool SetMap( int dxKey, eDirectionType dir, int virtualButton );
    // Retrieves the virtual button of the given type mapped to a dxKey, direction
    virtual int  GetMap( int dxKey, eDirectionType dir, eMapType map ) const;
    // Clears the specified mapping so it no longer exists.
    virtual void ClearMap( int dxKey, eDirectionType dir, int virtualButton );
    // Clears all the cached mappings.
    virtual void ClearMappedButtons();

private:
    // Sets up a input point index -> direct input keycode map.
    virtual void MapInputToDICode();

private:
    int m_ButtonMap[ NUM_MAPTYPES ][ NUM_KEYBOARD_BUTTONS ];
};

#endif
