/******************************************************************************
    File:        RealController.h
    Desc:        Interface for the RealController class.
                - RealController essentially is a physical controller class.
                - a wrapper around IRadController.

    Date:        May 16, 2003
    History:
 *****************************************************************************/

#ifndef REALCONTROLLER_H
#define REALCONTROLLER_H

//========================================
// System Includes
//========================================
#include <radcontroller.hpp>
#include <list>
using namespace std;

//========================================
// Nested Includes
//========================================

#include <input/controller.h>
#include <input/mapper.h>
#include <input/virtualinputs.hpp>

class UserController;
/*****************************************
    Some typedefs for convienince
 ****************************************/
typedef ref< IRadController > RADCONTROLLER;
typedef list< IRadControllerInputPoint* > RADINPUTPOINTLIST;
typedef list< IRadControllerInputPoint* >::iterator INPUTPOINTITER;

//=============================================================================
// Enumerations
//=============================================================================

// Types of real controllers.
enum eControllerType
{
    GAMEPAD = 0, // gamepad/joystick
    KEYBOARD,
    MOUSE,
    STEERINGWHEEL,
    NUM_CONTROLLERTYPES
};

// Directions which inputs can process.
enum eDirectionType
{
    DIR_UP = 0,
    DIR_DOWN,
    DIR_RIGHT,
    DIR_LEFT,
    NUM_DIRTYPES
};

//=============================================================================
// Class: RealController
//=============================================================================
//
// Description: The base class for every device plugged in to receive input
//              from the user.  There is one instance for each keyboard, mouse,
//              gamepad...
//
//=============================================================================

class RealController
{
public:
    RealController( eControllerType type );
    virtual ~RealController();
        
    RADCONTROLLER getController()    const { return m_radController;   }
    bool IsConnected()               const { return m_bConnected;      }
    void Connect()                         { m_bConnected = true;      }
    void Disconnect()                      { m_bConnected = false;     }
    eControllerType ControllerType() const { return m_eControllerType; }

    void Init( IRadController* pController );
    void Release();

    // Return true if the dxKey is one of the following types of inputs.
    virtual bool IsInputAxis( int dxKey ) const;
    virtual bool IsMouseAxis( int dxKey ) const;
    virtual bool IsPovHat( int dxKey ) const;

    // Returns true if this is a valid input for the controller.
    virtual bool IsValidInput( int dxKey ) const = 0;
    
    // Returns true if the key is banned for mapping.
    virtual bool IsBannedInput( int dxKey ) const;

    // Sets up a mapping from a dxkey/direction to a virtual button
    virtual bool SetMap( int dxKey, eDirectionType dir, int virtualButton ) = 0;
    // Retrieves the virtual button of the given type mapped to a dxKey, direction
    virtual int  GetMap( int dxKey, eDirectionType dir, eMapType map ) const = 0;
    // Clears the specified mapping so it no longer exists.
    virtual void ClearMap( int dxKey, eDirectionType dir, int virtualButton ) = 0;
    // Clears all the cached mappings.
    virtual void ClearMappedButtons() = 0;

    // Returns the name of the input.
    const char* GetInputName( int dxKey ) const;

    // Store & release registered input points.
    void AddInputPoints( IRadControllerInputPoint* pInputPoint );
    void ReleaseInputPoints( UserController* parent );

    // Returns the direct input code representing a given input point for
    // the controller, or Input::INVALID_CONTROLLERID if the i.p. doesn't exist.
    int GetDICode( int inputpoint ) const;

protected:
    // Sets up an input point index -> direct input keycode map for the
    // controller in m_InputToDICode.  Called automatically by Init().
    virtual void MapInputToDICode() = 0;

protected:
    RADCONTROLLER       m_radController;
    bool                m_bConnected;
    eControllerType     m_eControllerType;
    RADINPUTPOINTLIST   m_inputPointList;
    int*                m_InputToDICode;
    int                 m_numInputPoints;
};

#endif