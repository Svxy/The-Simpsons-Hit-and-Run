#include <worldsim/redbrick/vehiclecontroller/vehiclecontroller.h>

VehicleController::VehicleController( void )
:
mpVehicle( 0 )
{
}

VehicleController::~VehicleController( void )
{
}

Vehicle* VehicleController::GetVehicle( void ) const
{
    return mpVehicle;
}
