#ifndef VEHICLECONTROLER_H_
#define VEHICLECONTROLER_H_

#include <p3d/refcounted.hpp>
#include <input/button.h>

class Vehicle;

/*
// Skeleton class for example.
//
struct IButton
{
    virtual float GetValue( void ) const = 0;
    virtual float GetHeldTime( void ) const = 0;
};
*/
class VehicleController
:
public tRefCounted
{
public:
	VehicleController( void );
	virtual ~VehicleController( void );
    
    virtual void Update( float timeins ) {};
    virtual void Init() {};
    virtual void Shutdown() {};

    void SetVehicle( Vehicle* pVehicle ) { mpVehicle = pVehicle; }

	virtual float GetGas( void ) const = 0;
    virtual float GetThrottle( void ) const = 0;
    
	virtual float GetBrake( void ) const = 0;
    virtual float GetSteering( bool& isWheel ) const = 0;
    virtual float GetSteerLeft( void ) const = 0;
    virtual float GetSteerRight( void ) const = 0;

	virtual float GetHandBrake( void ) const = 0;
	virtual float GetReverse( void ) const = 0;
	virtual float GetHorn( void ) const = 0;

    Vehicle* GetVehicle( void ) const;
private:
	Vehicle* mpVehicle;
};

#endif // VEHICLECONTROLER_H_