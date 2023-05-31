#ifndef AIVEHICLECONTROLLER_H_
#define AIVEHICLECONTROLLER_H_

#include <worldsim/redbrick/vehiclecontroller/vehiclecontroller.h>
#include <input/button.h>

class Vehicle;

class AiVehicleController
:
public VehicleController
{
public:
	AiVehicleController( Vehicle* pVehicle );
	virtual ~AiVehicleController( void );

    virtual void Update( float timeins );

	virtual float GetGas( void ) const;
    virtual float GetThrottle( void ) const;
	virtual float GetBrake( void ) const;
    virtual float GetSteering( bool& isWheel ) const;
    
    virtual float GetSteerLeft( void ) const;
    virtual float GetSteerRight( void ) const;

	virtual float GetHandBrake( void ) const;
	virtual float GetReverse( void ) const;
	virtual float GetHorn( void ) const;
protected:
	Button mGas;
	Button mBrake;
    Button mSteering;
	Button mHandBrake;
	Button mReverse;
	Button mHorn;
private:
};

#endif //AIVEHICLECONTROLLER_H_