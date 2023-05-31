#ifndef VEHICLEMAPPABLE_H_
#define VEHICLEMAPPABLE_H_

#include <input/mappable.h>

class HumanVehicleController;

class VehicleMappable
:
public Mappable
{
public:
	VehicleMappable( void );
	~VehicleMappable( void );

    void ReleaseController( void );

    void OnButton( int controllerId, int id, const IButton* pButton );
	void OnButtonUp( int controllerId, int buttonId, const IButton* pButton );
	void OnButtonDown( int controllerId, int buttonId, const IButton* pButton );
    void LoadControllerMappings( unsigned int controllerId );
    
    void SetController( HumanVehicleController* pHumanVehicleController );

    bool IsWheel() const { return mIsWheel; };
    bool IsWheelA() const { return mIsWheelA; };

	    enum
	    {
	        Steer = 0,
	        SteerLeft,
	        SteerRight,
	        Throttle,
	        Gas,
	        Brake,
	        Horn,
	        Reset,
	        Pause,
            Reverse,
	        HandBrake,
	        Burnout,
            GetOutofCar,
	        LookBack,
            Spring,
            Damper,
            Rumble,
            RightStickX,
            RightStickY
        };
protected:
	HumanVehicleController* GetController( void ) const
		{ return mpHumanVehicleController; }
private:
	HumanVehicleController* mpHumanVehicleController;

    bool mIsWheel;
    bool mIsWheelA;
};

#endif //VEHICLEMAPPABLE_H_