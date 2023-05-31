#include <worldsim/redbrick/vehiclecontroller/aivehiclecontroller.h>
#include <main/game.h>
#include <memory/classsizetracker.h>
#include <radmath/radmath.hpp>

class Behaviour
{
public:
	virtual rmt::Vector Tick( Vehicle* pVehicle ) = 0;
};

class VehicleWander
:
public Behaviour
{
public:

	VehicleWander( void );
	virtual ~VehicleWander( void );

	rmt::Vector& GetTargetPoint( void );
	void SetTargetPoint( rmt::Vector& targetPoint );

	rmt::Vector& GetPosition( void );
	void SetPosition( rmt::Vector& position );

	float GetTargetRadius( void ) const;
	void SetTargetRadius( float radius );

	float GetSteeringRadius( void ) const;
	void SetSteeringRadius( float radius );

	rmt::Vector Tick( Vehicle* pVehicle );
private:
	rmt::Vector mTargetPoint;
	rmt::Vector mPosition;
	float mfTargetCircleRadius;
	float mfSteeringCircleRadius;
};

VehicleWander::VehicleWander( void )
:
mTargetPoint( 0.0f, 0.0f, 1.0f ),
mPosition( 0.0f, 0.0f, 0.1f ),
mfTargetCircleRadius( 1.0f ),
mfSteeringCircleRadius( 0.003f )
{
	mTargetPoint.z = mfTargetCircleRadius;
}

VehicleWander::~VehicleWander( void )
{
}

rmt::Vector VehicleWander::Tick( Vehicle* pVehicle )
{
	static rmt::Randomizer r( Game::GetRandomSeed () );
	static rmt::Vector point( 0.0f, 0.0f, 0.0f );

	point.x = r.FloatSigned( );
	point.y = 0.0f;
	point.z = r.FloatSigned( );

	// Make a point on the new circle.
	//
	point.Normalize( );
	point.Scale( mfSteeringCircleRadius );

	// Add to point on circle.
	//
	mTargetPoint.Add( point );
	// Project back to unit circle.
	//
	mTargetPoint.Normalize( );
	// Scale to actual Target Circle.
	//
	mTargetPoint.Scale( mfTargetCircleRadius );

    rmt::Vector retVector;
    retVector.Add( mPosition, mTargetPoint );
    retVector.Normalize( );

	return retVector;
}

AiVehicleController::AiVehicleController( Vehicle* pVehicle )
{
    //CLASSTRACKER_CREATE( AiVehicleController );
    SetVehicle( pVehicle );
}

AiVehicleController::~AiVehicleController( void )
{
    //CLASSTRACKER_DESTROY( AiVehicleController );
}

void AiVehicleController::Update( float timeins )
{
    /*
    static Behaviour* spBehaviour = 0;
    if ( !spBehaviour )
	{
		spBehaviour = new VehicleWander;
	}
    rmt::Vector direction = spBehaviour->Tick( GetVehicle( ) );
    mSteering.SetValue( direction.x );
    mGas.SetValue( direction.z );
    */
    mHandBrake.SetValue( 1.0f );

    mGas.SetValue( 0.0f );
    mSteering.SetValue( 0.0f );
    mBrake.SetValue( 0.0f );
    mReverse.SetValue( 0.0f );
    mHorn.SetValue( 0.0f );
    
}

float AiVehicleController::GetGas( void ) const
{
    // ai shoudln't really use this?
    return mGas.GetValue();
}

float AiVehicleController::GetThrottle( void ) const
{
    return mGas.GetValue();
}

float AiVehicleController::GetBrake( void ) const
{
    return mBrake.GetValue();
}

float AiVehicleController::GetSteering( bool& isWheel ) const
{
    isWheel = false;
    return mSteering.GetValue();
}


// AI shouldn't really call these
float AiVehicleController::GetSteerLeft( void ) const
{
    return 0.0f;
}


float AiVehicleController::GetSteerRight( void ) const
{
    return 0.0f;
}

float AiVehicleController::GetHandBrake( void ) const
{
    return mHandBrake.GetValue();
}

float AiVehicleController::GetReverse( void ) const
{
    return mReverse.GetValue();
}

float AiVehicleController::GetHorn( void ) const
{
    return mHorn.GetValue();
}