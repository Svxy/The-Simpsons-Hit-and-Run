#include <worldsim/redbrick/vehiclecontroller/vehiclemappable.h>
#include <worldsim/redbrick/vehiclecontroller/humanvehiclecontroller.h>

#include <worldsim/worldphysicsmanager.h>

// Temp.
//
#include <worldsim/avatarmanager.h>

#ifdef RAD_WIN32
#include <gameflow/gameflow.h>
#include <contexts/context.h>
#endif

// Constructor.
//
VehicleMappable::VehicleMappable( void )
:
Mappable(Input::ACTIVE_GAMEPLAY),
mpHumanVehicleController( 0 ),
mIsWheel( false ),
mIsWheelA( false )
{
}

// Destructor
//
VehicleMappable::~VehicleMappable( void )
{
    ReleaseController();
}

void VehicleMappable::ReleaseController( void )
{
    if ( mpHumanVehicleController )
    {
        mpHumanVehicleController->Release();
        mpHumanVehicleController = 0;
    }
}



// This method is called when ever a button state changes.
//void
void VehicleMappable::OnButton( int controllerId, int id, const IButton* pButton )
{
}

// This method is called when a button changes state from "Pressed" to "Released".
//
void VehicleMappable::OnButtonUp( int controllerId, int buttonId, const IButton* pButton )
{
}

// This method is called when a button changes state from "Released" to "Pressed".
//
void VehicleMappable::OnButtonDown( int controllerId, int buttonId, const IButton* pButton )
{
    switch ( buttonId )
    {
    case Reset:
        {
#ifdef RAD_GAMECUBE
            if ( pButton->GetValue() > 0.5f )
            {
                GetController( )->Reset();
            }
#else
            GetController( )->Reset();
#endif
            break;
        }
    case Horn:
        {
            GetWorldPhysicsManager()->ToggleTimerState();
        }
    default:
        {
            // Do nothing.  That's cool.
            //
            break;
        }
    }
}

// This is how we create our controller device mappings to logical game mappings.
// The mappings set up in this method are platform specific.
//
// The basic format of the calls is to "Map" a input, to a enumerated output id.
// The output of the specified input will be contained in the Button[] array.
// This id will also be sent as a the second parameter in the OnButton... messages.
//
void VehicleMappable::LoadControllerMappings( unsigned int controllerId )
{
    // The names used in parameter 1 of the Map method are
	// Platform Specific.
	//

	// Configuration 1.
	//

    // MS7
    // #ifdef this for different platforms
    #ifdef RAD_XBOX

    ClearMap(0);
    Map( "LeftStickX", Steer, 0, controllerId );
	Map( "DPadLeft", SteerLeft, 0, controllerId );
	Map( "DPadRight", SteerRight, 0, controllerId );
    
    // The misnamed "Throttle" controls gas (up) and brake (down).
    //
	//Map( "RightStickY", Throttle, 0, controllerId );  Not on XBOX, please

	Map( "RightTrigger", Gas, 0, controllerId );
  	Map( "A", Gas, 0, controllerId );

    Map( "B", Brake, 0, controllerId );
    Map( "LeftTrigger", Brake, 0, controllerId );

	Map( "X", HandBrake, 0, controllerId );

    Map( "White", Horn, 0, controllerId );
    Map( "LeftThumb", Horn, 0, controllerId );

	Map( "Back", Reset, 0, controllerId );
    #endif


    #ifdef RAD_PS2

    ClearMap(0);
   
    if ( controllerId >= Input::MaxControllers - Input::MaxUSB )
    {
        mIsWheel = true;

        //This is a wheel!
        //And for the wheels!
        Map( "Wheel", Steer, 0, controllerId );
        Map( "Gas", Gas, 0, controllerId );
        Map( "Brake", Brake, 0, controllerId );

        if ( !Map( "R2", Gas, 0, controllerId ) )
        {
            mIsWheelA = true;

            //This is the GT wheel.
            Map( "LGR1", Gas, 0, controllerId );
            Map( "LGA", Brake, 0, controllerId );
            Map( "LGL1", HandBrake, 0, controllerId );
            Map( "LGX", Horn, 0, controllerId );
            Map( "LGB", Reset, 0, controllerId );
        }
        else
        {
            //This is the DPad Wheel
            Map( "X", Gas, 0, controllerId );
            Map( "R1", Brake, 0, controllerId );
            Map( "Circle", Brake, 0, controllerId );
            Map( "Square", HandBrake, 0, controllerId );
            Map( "L1", HandBrake, 0, controllerId );
            Map( "DPadUp", Horn, 0, controllerId );
            Map( "DPadDown", Horn, 0, controllerId );
            Map( "DPadLeft", Horn, 0, controllerId );
            Map( "DPadRight", Horn, 0, controllerId );
            Map( "Select", Reset, 0, controllerId );
        }
    }
    else
    {
        //This is a DUALSHOCK!
        Map( "LeftStickX", Steer, 0, controllerId );
	    Map( "DPadLeft", SteerLeft, 0, controllerId );
	    Map( "DPadRight", SteerRight, 0, controllerId );

        // The misnamed "Throttle" controls gas (up) and brake (down).
        //
	    Map( "RightStickY", Throttle, 0, controllerId );

        Map( "X", Gas, 0, controllerId );

        Map( "Circle", Brake, 0, controllerId );

        Map( "R1", HandBrake, 0, controllerId );
	    Map( "Square", HandBrake, 0, controllerId );

        Map( "L3", Horn, 0, controllerId );

	    Map( "Select", Reset, 0, controllerId );
    }

    #endif

    #ifdef RAD_GAMECUBE

    ClearMap(0);
    Map( "LeftStickX", Steer, 0, controllerId );
//    Map( "DPadLeft", SteerLeft, 0, controllerId );
//    Map( "DPadRight", SteerRight, 0, controllerId );

    Map( "AnalogTriggerR", Gas, 0, controllerId );
    Map( "A", Gas, 0, controllerId );

    Map( "X", Brake, 0, controllerId );
    Map( "AnalogTriggerL", Brake, 0, controllerId );

    Map( "B", HandBrake, 0, controllerId );

    Map( "TriggerZ", Horn, 0, controllerId );
    Map( "DPadUp", Reset, 0, controllerId );
    Map( "DPadDown", Reset, 0, controllerId );
    Map( "DPadLeft", Reset, 0, controllerId );
    Map( "DPadRight", Reset, 0, controllerId );

    if ( Map( "Wheel", Steer, 0, controllerId ) )
    {
        mIsWheel = true;
        //This is a wheel!
        Map( "Gas", Gas, 0, controllerId );
        Map( "Brake", Brake, 0, controllerId );
    }
    else
    {
//
    }

    #endif

    #ifdef RAD_WIN32

    ClearMap(0);
    if ( GetGameFlow()->GetCurrentContext() == CONTEXT_LOADING_SUPERSPRINT &&
         controllerId == 3 )
    {
        Map( "P1_KBD_Left", SteerLeft, 0, controllerId );
        Map( "P1_KBD_Right", SteerRight, 0, controllerId );
        Map( "P1_KBD_Gas", Gas, 0, controllerId );
        Map( "P1_KBD_Brake", Brake, 0, controllerId );
        Map( "P1_KBD_EBrake", HandBrake, 0, controllerId );
        Map( "P1_KBD_Nitro", Horn, 0, controllerId );
    }

    Map( "SteerLeft", SteerLeft, 0, controllerId );
    Map( "SteerRight", SteerRight, 0, controllerId );

    Map( "Accelerate", Gas, 0, controllerId );
    Map( "Reverse", Brake, 0, controllerId );

    Map( "HandBrake", HandBrake, 0, controllerId );
    Map( "Horn", Horn, 0, controllerId );

    Map( "ResetCar", Reset, 0, controllerId );
    
    #endif

}

void VehicleMappable::SetController( HumanVehicleController* pHumanVehicleController )
{
    tRefCounted::Assign( mpHumanVehicleController, pHumanVehicleController );
}
