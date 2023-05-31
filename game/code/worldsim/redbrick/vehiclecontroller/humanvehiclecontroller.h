#ifndef HUMANVEHICLECONTROLLER_H_
#define HUMANVEHICLECONTROLLER_H_

#include <worldsim/redbrick/vehiclecontroller/vehiclecontroller.h>

#include <input/controller.h>
#include <events/eventlistener.h>

class VehicleMappable;
class BaseDamper;
class SteeringSpring;
class ConstantEffect;
class WheelRumble;
class UserController;

class HumanVehicleController
:
public VehicleController,
public EventListener
{
public:
	HumanVehicleController( void );
	virtual ~HumanVehicleController( void );

    void ReleaseVehicleMappable( void );

    void Create( Vehicle* pVehicle, VehicleMappable* pMappable, int controllerId );
#ifdef RAD_PS2
    void SetWheel( VehicleMappable* pMappable, unsigned int wheelNum );
#endif
	virtual float GetGas( void ) const;
    virtual float GetThrottle( void ) const;
	virtual float GetBrake( void ) const;
    virtual float GetSteering( bool& isWheel ) const;

    // for dpad
    virtual float GetSteerLeft( void ) const;
    virtual float GetSteerRight( void ) const;

	virtual float GetHandBrake( void ) const;
	virtual float GetReverse( void ) const;
	virtual float GetHorn( void ) const;

	void Reset( void );
    void SetDisableReset( bool tf ) { mDisableReset = tf; }

    VehicleMappable* GetMappable( void ) const;

    virtual void Update( float timeins );
    virtual void Init();
    virtual void Shutdown();

    void HandleEvent( EventEnum id, void* pEventData );

protected:
private:

	VehicleMappable* mpMappable;

#ifdef RAD_PS2
    VehicleMappable* mpWheel[ Input::MaxUSB ];
#endif

    int mControllerId;

#if defined(RAD_GAMECUBE) || defined(RAD_PS2) || defined(RAD_WIN32)
    SteeringSpring* mSpring;
    BaseDamper* mDamper;
    ConstantEffect* mConstantEffect;
    WheelRumble* mWheelRumble;
    WheelRumble* mHeavyWheelRumble;

    void SetupRumbleFeatures( UserController* uc );
#endif
    bool mDisableReset;
};

#endif //HUMANVEHICLECONTROLLER_H_