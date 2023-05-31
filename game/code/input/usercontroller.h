#ifndef USERCONTROLLER_HPP
#define USERCONTROLLER_HPP

#include <input/controller.h>
#include <input/button.h>
#include <input/rumbleeffect.h>
#include <radcontroller.hpp>
#include <radkey.hpp>

#if defined(RAD_GAMECUBE) || defined(RAD_PS2)
#include <input/steeringspring.h>
#include <input/baseDamper.h>
#include <input/constanteffect.h>
#include <input/wheelrumble.h>
#endif

class Mappable;
class Mapper;


class UserController :
    public IRadControllerInputPointCallback
{
public:
	UserController( void );
	virtual ~UserController( void );

    // initial set up of controller (called once at input ystem init)
    virtual void Create( int id );

    // set up actual controller mapping (called once at startup and prior to any time input 
    // connection state changes)
    virtual void Initialize( IRadController* pIController2 );

    // release ftech controllers (called prior to input connection state change)
    virtual void ReleaseRadController( void );

    // get the index of this controller
    unsigned int GetControllerId( void ) const { return m_controllerId; }

    // per-frame update
    void Update( unsigned timeins );

    // set the current game state (to activate / deactivate appropriate logical controllerts)
    void SetGameState(unsigned);

    // Rumble (not yet implimented)
    void SetRumble( bool bRumbleOn, bool pulse = false );
    bool IsRumbleOn( void ) const;
    void PulseRumble();
    void ApplyEffect( RumbleEffect::Effect effect, unsigned int durationms );
    void ApplyDynaEffect( RumbleEffect::DynaEffect effect, unsigned int durationms, float gain );


    // set the player index associated with this controller
    void SetPlayerIndex( int i ) { m_iPlayerIndex = i; }
    int GetPlayerIndex( void ) const { return m_iPlayerIndex; }

    // connection status
    bool IsConnected( void ) const { return m_bConnected; } 
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

#if defined(RAD_GAMECUBE) || defined(RAD_PS2)
    SteeringSpring* GetSpring() { return mSteeringSpring.IsInit() ? &mSteeringSpring : NULL; };
    BaseDamper* GetDamper() { return mSteeringDamper.IsInit() ? &mSteeringDamper : NULL; };
    ConstantEffect* GetConstantEffect() { return mConstantEffect.IsInit() ? &mConstantEffect : NULL; };
    WheelRumble* GetWheelRumble() { return mWheelRumble.IsInit() ? &mWheelRumble : NULL; };
    WheelRumble* GetHeavyWheelRumble() { return mHeavyWheelRumble.IsInit() ? &mHeavyWheelRumble : NULL; };
#endif

    Mappable* GetMappable( unsigned int which ) { return mMappable[ which ];  };

    bool IsWheel() 
    {
#if defined(RAD_GAMECUBE) || defined(RAD_PS2)
        return mSteeringSpring.IsInit();
#else
        return false;
#endif
    }
protected:
    // Implements IRadControllerInputPointCallback
    void OnControllerInputPointChange( unsigned int buttonId, float value );

    // the player index we are controlling.
    //
    int m_iPlayerIndex;

	ref< IRadController > m_xIController2;

	int m_controllerId;

    bool m_bConnected;

    Mappable* mMappable[ Input::MaxMappables ];
    bool mbInputPointsRegistered;

    unsigned mNumButtons;
    Button mButtonArray[ Input::MaxPhysicalButtons ];
    radKey mButtonNames[ Input::MaxPhysicalButtons ];
    float  mButtonDeadZones[ Input::MaxPhysicalButtons ];

    unsigned mGameState;
    
#if defined(RAD_GAMECUBE) || defined(RAD_PS2)
    SteeringSpring mSteeringSpring;
    BaseDamper mSteeringDamper;
    ConstantEffect mConstantEffect;
    WheelRumble mWheelRumble;
    WheelRumble mHeavyWheelRumble;
#endif
    bool mbIsRumbleOn;

    RumbleEffect mRumbleEffect;    
};

#endif

