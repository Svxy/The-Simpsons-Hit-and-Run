//=============================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   UserController
//
// Description: An input/output controller for a given player in the game.  It
//              bundles a number of actual controllers together to allow the
//              player to use different devices (keyboard, mouse, gamepad...).
//              This is the input interface between the game and the player,
//              sending inputs to registered input processing classes, letting
//              the player drive, move around, control the front end...
//
//              This specific class is an implementation of the UserController
//              class strictly for win32.
//
// History:     Branched from the console UserController class.
//
//=============================================================================

#ifndef USERCONTROLLER_HPP
#define USERCONTROLLER_HPP

//========================================
// Platform check
//========================================

#ifndef RAD_WIN32
#error 'This implementation of the user controller is specific to Win32.'
#endif

//========================================
// System Includes
//========================================

#include <radcontroller.hpp>
#include <radkey.hpp>

//========================================
// Nested Includes
//========================================

#include <data/config/gameconfig.h>
#include <input/controller.h>
#include <input/button.h>
#include <input/mapper.h>
#include <input/rumbleeffect.h>
#include <input/RealController.h>

//=============================================================================
// Forward Class Declarations
//=============================================================================

class Mappable;
class Mapper;
class SteeringSpring;
class BaseDamper;  
class ConstantEffect;
class WheelRumble;  

//=============================================================================
// Enumerations
//=============================================================================

enum eVirtualMapSlot
{
    SLOT_PRIMARY = 0,
    SLOT_SECONDARY
};

#define MAX_AXIS_THRESH 1.0f
#define MIN_AXIS_THRESH -1.0f

#define MAPPING_DEADZONE 0.5f

//=============================================================================
// Interface: ButtonMappedCallback
//=============================================================================
//
// Description: Callback interface for the UserController::RemapButton method.
//              Implement the method to receive notification for when a button
//              has been mapped.
//
//=============================================================================

struct ButtonMappedCallback
{
    virtual void OnButtonMapped( const char* InputName, eControllerType cont, int num_dirs, eDirectionType direction ) = 0;
};

//=============================================================================
// Struct: ButtonMapData
//=============================================================================
//
// Description: Internal struct for user controller.
//
//=============================================================================

struct ButtonMapData
{
    ButtonMapData() : MapNext(false) {};

    bool MapNext;
    int map;
    int virtualButton;
    ButtonMappedCallback* callback;
};

//=============================================================================
// Class: UserController
//=============================================================================
//
// Description: The controller interface between the user and the game.  It
//              encapsulates a number of radControllers for a given player.
//
//=============================================================================

class UserController :
    public IRadControllerInputPointCallback,
    public GameConfigHandler
{
public:
	UserController( void );
	virtual ~UserController( void );

    // initial set up of controller (called once at input ystem init)
    virtual void Create( int id );

    // set up actual controller mapping (called once at startup and prior to any time input 
    // connection state changes)
    virtual void Initialize( RADCONTROLLER* pIController );

    // release ftech controllers (called prior to input connection state change)
    virtual void ReleaseRadController( void );

    RealController* GetRealController( eControllerType type ) { return m_pController[type]; }
    // get the index of this controller
    unsigned int GetControllerId( void ) const { return m_controllerId; }

    // per-frame update
    void Update( unsigned timeins );

    void StartForceEffects();
    void StopForceEffects();

    // set the current game state (to activate / deactivate appropriate logical controllerts)
    void SetGameState(unsigned);

    // Rumble (not yet implimented)
    void SetRumble( bool bRumbleOn, bool pulse = false );
    bool IsRumbleOn( void ) const;
    void PulseRumble();
    void ApplyEffect( RumbleEffect::Effect effect, unsigned int durationms );
    void ApplyDynaEffect( RumbleEffect::DynaEffect effect, unsigned int durationms, float gain );

    // connection status
    bool IsConnected( void ) const; 
    void NotifyConnect( void );
    void NotifyDisconnect( void );

    // Returns the value stored by input point at index.
    float GetInputValue( unsigned int index ) const;
    Button* GetInputButton( unsigned int index );

    // Returns the current 'real-time' value of the input point
    float GetInputValueRT( unsigned int index ) const;

    // Attaches an logical controller this physical controller 
    int RegisterMappable( Mappable* pMappable );

    // Detach a logical controller
    void UnregisterMappable( int handle );
    void UnregisterMappable( Mappable* pMappable );

    // Set up the physical-logical button mappings.
    void LoadControllerMappings( void );

    // return the button id from the name.
    int GetIdByName( const char* pszName ) const;

    SteeringSpring* GetSpring();
    BaseDamper* GetDamper();
    ConstantEffect* GetConstantEffect();
    WheelRumble* GetWheelRumble();
    WheelRumble* GetHeavyWheelRumble();

    Mappable* GetMappable( unsigned int which ) { return mMappable[ which ];  };

    bool IsWheel() const { return m_bIsWheel; }

    // Implementation of the GameConfigHandler interface
    virtual const char* GetConfigName() const;
    virtual int GetNumProperties() const;
    virtual void LoadDefaults();
    virtual void LoadConfig( ConfigString& config );
    virtual void SaveConfig( ConfigString& config );

    // Public mapping functions for the user controller.
    const char* GetMap( int map, int virtualKey, int& numDirs, eControllerType& cont, eDirectionType& dir ) const;
    void RemapButton( int map, int VirtualButton, ButtonMappedCallback* callback );

    void SetMouseLook( bool bMouseLook )         { m_bMouseLook = bMouseLook;         }
    void SetInvertMouseX( bool bInvertMouseX )   { m_bInvertMouseX = bInvertMouseX;   }
    void SetInvertMouseY( bool bInvertMouseY )   { m_bInvertMouseY = bInvertMouseY;   }
    void SetForceFeedback( bool bForceFeedback ) { m_bForceFeedback = bForceFeedback; }
    void SetTutorialDisabled( bool bDisabled )   { m_bTutorialDisabled = bDisabled; }
    void SetMouseSensitivityX( float fValue )    { m_fMouseSensitivityX = fValue;     }
    void SetMouseSensitivityY( float fValue )    { m_fMouseSensitivityY = fValue;     }
    void SetWheelSensitivityX( float fValue )    { m_fWheelSensitivityX = fValue;     }
    void SetWheelSensitivityY( float fValue )    { m_fWheelSensitivityY = fValue;     }

    bool  IsMouseLookOn()        const { return m_bMouseLook;         }
    bool  IsMouseXInverted()     const { return m_bInvertMouseX;      }
    bool  IsMouseYInverted()     const { return m_bInvertMouseY;      }
    bool  IsForceFeedbackOn()    const { return m_bForceFeedback;     }
    bool  IsTutorialDisabled()   const { return m_bTutorialDisabled;  }

    float GetMouseSensitivityX() const { return m_fMouseSensitivityX; }
    float GetMouseSensitivityY() const { return m_fMouseSensitivityY; }
    float GetWheelSensitivityX() const { return m_fWheelSensitivityX; }
    float GetWheelSensitivityY() const { return m_fWheelSensitivityY; }

protected:

    // These register our callbacks with the RadControllers.
    void RegisterInputPoints();
    void RegisterInputPoint( eControllerType type, int inputpoint );

    // Callback for when keys/inputs are pressed by the user.
    void OnControllerInputPointChange( unsigned int buttonId, float value );

    // These process input point values as received by the input point callback.
    int DeriveDirectionValues( eControllerType type, int dxKey, float value, float* dir_values );
    void SetButtonValue( unsigned int virtualButton, float value, bool sticky );

    // Needed to get mouse axes to function properly
    void ResetMouseAxes();

    // Private mapping functions for the user controller.
    void SetMap( int map, int virtualKey, eControllerType cont, int dxKey, eDirectionType dir = DIR_UP );
    bool GetMap( int map, int virtualKey, eControllerType& cont, int& dxKey, eDirectionType& dir ) const;
    void Remap( eControllerType cont, int dxKey, float* dvalues, int directions );
    void ClearMappings();
    void LoadFEMappings();

private:

    int m_controllerId;
    bool mIsConnected;

    unsigned mGameState;
    bool mbInputPointsRegistered;

    RealController* m_pController[ NUM_CONTROLLERTYPES ];

    SteeringSpring*  m_pSteeringSpring;
    BaseDamper*      m_pSteeringDamper;
    ConstantEffect*  m_pConstantEffect;
    WheelRumble*     m_pWheelRumble;
    WheelRumble*     m_pHeavyWheelRumble;

    Mappable* mMappable[ Input::MaxMappables ];

    // Virtual mapping data.
    Mapper mVirtualMap[ Input::MaxVirtualMappings ];
    ButtonMapData mMapData;

    // Button data.
    int mNumButtons;
    Button mButtonArray[ Input::MaxPhysicalButtons ];
    radKey mButtonNames[ Input::MaxPhysicalButtons ];
    float  mButtonDeadZones[ Input::MaxPhysicalButtons ];
    bool   mButtonSticky[ Input::MaxPhysicalButtons ];

    bool mKeyboardBack;

    bool mbIsRumbleOn;
    bool m_bIsWheel;
    RumbleEffect mRumbleEffect;   

    bool m_bMouseLook,
         m_bInvertMouseX,
         m_bInvertMouseY,
         m_bForceFeedback;
    bool m_bTutorialDisabled;

    float m_fMouseSensitivityX,
          m_fMouseSensitivityY,
          m_fWheelSensitivityX, // The wheel
          m_fWheelSensitivityY; // The pedals

    int mResetMouseCounter[3]; // Hack needed for proper mouse inputs.
};

#endif
