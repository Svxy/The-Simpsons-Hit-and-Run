//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        inputmanager.h
//
// Description: InputManager class declaration.
//
// History:     + Created -- TBJ
//
//=============================================================================

#ifndef InputManager_HPP
#define InputManager_HPP

//========================================
// System Includes
//========================================
#include <radcontroller.hpp>

//========================================
// Project Includes
//========================================
#include <data/gamedata.h>
#include <input/controller.h>
#ifdef RAD_WIN32
#include <input/usercontrollerWin32.h>
#include <input/FEMouse.h>
#else
#include <input/usercontroller.h>
#endif
#include <constants/maxplayers.h>

//========================================
// Forward References
//========================================
class UserController;
class Mappable;

//==============================================================================
//
// Synopsis: Used to trigger events and route them to the listeners.
//
//==============================================================================
class InputManager : public IRadControllerConnectionChangeCallback,
                     public GameDataHandler
{
public:
#ifdef RAD_XBOX
    enum eButtonMap
    {
        DPadUp,
        DPadDown,
        DPadLeft,
        DPadRight,
        Start,
        Back,
        Select = Back,
        LeftThumb,
        L3 = LeftThumb,
        RightThumb,
        R3 = RightThumb,
        A,
        X = A,
        B,
        Circle = B,
//        X,  //This one is conflicting with PS2
        Square,  //This is X
        Y,
        Triangle = Y,
        AnalogA,
        AnalogB,
        AnalogX,
        AnalogY,
        Black,
        White,
        AnalogBlack,
        AnalogWhite,
        LeftTrigger,
        AnalogL1 = LeftTrigger,
        RightTrigger,
        AnalogR1 = RightTrigger,
        LeftStickX,
        LeftStickY,
        RightStickX,
        RightStickY
    };
#elif defined(RAD_WIN32)  // Clumsy because of win32<->console differences
    enum eButtonMap
    {
        MoveUp,                 // These are the real buttons names
        MoveDown,
        MoveLeft,
        MoveRight,
        Attack,
        Jump,
        Sprint,
        DoAction,
        Accelerate,
        Reverse,
        SteerLeft,
        SteerRight,
        GetOutCar,
        HandBrake,
        Horn,
        ResetCar,
        CameraLeft,
        CameraRight,
        CameraMoveIn,
        CameraMoveOut,
        CameraZoom,
        CameraLookUp,
        CameraCarLeft,
        CameraCarRight,
        CameraCarLookUp,
        CameraCarLookBack,
        CameraToggle,
        feBack,
        feMoveUp,
        feMoveDown,
        feMoveLeft,
        feMoveRight,
        feSelect,
        feFunction1,
        feFunction2,
        feMouseLeft,
        feMouseRight,
        feMouseUp,
        feMouseDown,

        P1_KBD_Start,
        P1_KBD_Gas,
        P1_KBD_Brake,
        P1_KBD_EBrake,
        P1_KBD_Nitro,
        P1_KBD_Left,
        P1_KBD_Right,
        Select = feBack,        // These are mappings to PS2 buttons, needed sometimes.
        Start = feSelect,
        DPadUp = MoveUp,
        DPadRight = MoveRight,
        DPadDown = MoveDown,
        DPadLeft = MoveLeft,
        R1 = HandBrake,
        Triangle = DoAction,
        Circle = Sprint,
        X = Jump,
        Square = Attack,
        L3 = Horn,
        AnalogL1 = CameraZoom,
        AnalogR1 = CameraLookUp,
        LeftStickX = 200,        // Must hack to get camera stuff to compile
        LeftStickY = 201,        // Must hack to get camera stuff to compile
        KeyboardEsc = 202        // Cute hack to get escape only back buttons
    };
#elif defined(RAD_PS2)
    enum eButtonMap
    {
        Select,
        Start,
        DPadUp,
        DPadRight,
        DPadDown,
        DPadLeft,
        L2,
        R2,
        L1,
        R1,
        Triangle,
        Circle,
        X,
        Square,
        L3,
        R3,
        RightStickX,
        RightStickY,
        LeftStickX,
        LeftStickY,
        AnalogDPadRight,
        AnalogDPadLeft,
        AnalogDPadUp,
        AnalogDPadDown,
        AnalogTriangle,
        AnalogCircle,
        AnalogX,
        AnalogSquare,
        AnalogL1,
        AnalogR1,
        AnalogL2,
        AnalogR2,
    };
#else //RAD_GAMECUBE
    enum eButtonMap
    {
        DPadLeft,
        DPadRight,
        DPadDown,
        DPadUp,
        TriggerZ,
        L3 = TriggerZ,
        TriggerR,
        AnalogR1 = TriggerR,
        TriggerL,
        AnalogL1 = TriggerL,
        A,
        X = A,
        B,
        Square = B,
//        X,  //This one is conflicting with PS2
        Circle, //This is X
        Y,
        Triangle = Y,
        Menu,
        Start = Menu,
        LeftStickX,
        LeftStickY,
        RightStickX,
        RightStickY,
        AnalogTriggerL,
        LeftTrigger = AnalogTriggerL,
        AnalogTriggerR,
        RightTrigger = AnalogTriggerR,
        AnalogA,
        AnalogueX = AnalogA,
        AnalogB,
        AnalogSquare = AnalogB
    };
#endif

    // Static Methods for accessing this singleton.
    static InputManager* CreateInstance( void );
    static InputManager* GetInstance( void );
    static void DestroyInstance( void );

    // set everything up
    void Init();
    
    // per frame update
    void Update ( unsigned int timeinms );

    // various info 
    bool IsControllerInPort( int portnum ) const;
    static unsigned int GetMaxControllers( void ) { return Input::MaxControllers; }
    const char* GetName() const { return "Game Controller System"; }

    void ToggleRumble( bool on );
    void SetRumbleForDevice( int controllerId, bool bRumbleOn );
    bool IsRumbleOnForDevice( int controllerId ) const;
    void TriggerRumblePulse( int controllerId );

    void SetRumbleEnabled( bool isEnabled );
    bool IsRumbleEnabled() const;

#ifdef RAD_WIN32
    void StartRumbleEffects();
    void StopRumbleEffects();
#endif

    // Returns the value of the input point 'inputIndex' owned by the controller at
    // controllerIndex.
    float GetValue( unsigned int controllerIndex, unsigned int inputIndex ) const;

    // Get a physical controller 
    UserController* GetController( unsigned int controllerIndex );

    // Associate this logical controller with the physical controller in slot "index"
    int RegisterMappable( unsigned int index, Mappable *pMappable );

    // Remove associations between a physical and logical controller
    void UnregisterMappable( unsigned int index, int handle );
    void UnregisterMappable( unsigned int index, Mappable *pMappable  );
    void UnregisterMappable( Mappable *pMappable  );

    // set the current game state for the input system (one of the enums in Input::Active state)
    void SetGameState( Input::ActiveState state );
    Input::ActiveState GetGameState() const;

    // registration of controller ID to player ID
    //
    void RegisterControllerID( int playerID, int controllerID );
    void UnregisterControllerID( int playerID );
    void UnregisterAllControllerID();
    int  GetControllerIDforPlayer( int playerID ) const;
    int  GetControllerPlayerIDforController( int controllerIndex ) const;

    // Implements GameDataHandler
    //
    virtual void LoadData( const GameDataByte* dataBuffer, unsigned int numBytes );
    virtual void SaveData( GameDataByte* dataBuffer, unsigned int numBytes );
    virtual void ResetData();

    void EnableReset( bool reset ) { mResetEnabled = reset; };

    bool IsProScanButtonsPressed() const { return m_isProScanButtonsPressed; }

#ifdef RAD_WIN32
    FEMouse* GetFEMouse() const { return m_pFEMouse; }
#endif

#ifdef RAD_PS2
    int GetLastMultitapStatus(int port) const {return mLastMultitapStatus[port];}
    int GetCurMultitapStatus(int port) const {return mCurMultitapStatus[port];}
#endif

private:
    InputManager();
    ~InputManager();

    // IRadControllerConnectionChangeCallback interface
    // called when someone plugs in or pulls out a controller
    void OnControllerConnectionStatusChange( IRadController* pIController );

    void ReleaseAllControllers( void );
    void EnumerateControllers( void );

    static InputManager* spInstance;

    ref< IRadControllerSystem > mxIControllerSystem2;

    UserController mControllerArray[ Input::MaxControllers ];

    unsigned mGameState;
    bool mChangeGameState : 1;
    bool mConnectStateChanged : 1;
    bool mIsRumbleEnabled : 1;
    bool mIsResetting : 1;
    bool mResetEnabled : 1;

    int m_registeredControllerID[ MAX_PLAYERS ];

    unsigned int mResetTimeout;

    bool m_isProScanButtonsPressed : 1;

#ifdef RAD_WIN32
    FEMouse* m_pFEMouse;
#endif
#ifdef RAD_PS2
    int mLastMultitapStatus[2];
    int mCurMultitapStatus[2];
#endif
};


// A little syntactic sugar for getting at this singleton.
inline InputManager* GetInputManager() { return( InputManager::GetInstance() ); }

inline int
InputManager::GetControllerIDforPlayer( int playerID ) const
{
    int controllerID;
    rAssert( playerID >= 0 && playerID < MAX_PLAYERS );
    
    if ( playerID >= 0 && playerID < MAX_PLAYERS )
    {
        controllerID = m_registeredControllerID[ playerID ];
    }
    else
    {
        // Return error code
        controllerID =  -1;
    }
    #ifndef RAD_RELEASE
        if( m_registeredControllerID[ playerID ] == -1 )
        {
           // too much spew, we are polling this constantly for controller unplugged
		   // rTunePrintf( "*** WARNING: No controller ID registered for player %d!]\n",
           //             playerID + 1 );
        }
    #endif

    return controllerID;
}

//=============================================================================
// InputManager::GetControllerPlayerIDforController
//=============================================================================
// Description: Comment
//
// Parameters:  ( int controllerIndex )
//
// Return:      int  
//
//=============================================================================
inline int InputManager::GetControllerPlayerIDforController( int controllerIndex ) const
{
    int i;
    for ( i = 0; i < MAX_PLAYERS; ++i )
    {
        if ( m_registeredControllerID[ i ] == controllerIndex )
        {
            return i;
        }
    }

    return -1;
}

//=============================================================================
// InputManager::IsRumbleEnabled
//=============================================================================
// Description: Comment
//
// Parameters:  none
//
// Return:      bool  
//
//=============================================================================
inline bool InputManager::IsRumbleEnabled() const
{
    return mIsRumbleEnabled;
}


#endif
