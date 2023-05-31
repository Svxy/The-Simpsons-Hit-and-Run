#include <worldsim/redbrick/vehiclecontroller/humanvehiclecontroller.h>
#include <worldsim/redbrick/vehiclecontroller/vehiclemappable.h>
#include <worldsim/redbrick/vehiclecontroller/../vehicle.h>
#include <presentation/gui/guisystem.h>
#include <raddebugwatch.hpp>

#include <input/inputmanager.h>
#include <events/eventmanager.h>
#include <events/eventdata.h>

#include <main/commandlineoptions.h>

#if defined(RAD_GAMECUBE) || defined(RAD_PS2) || defined(RAD_WIN32)
#include <input/basedamper.h>
#include <input/steeringspring.h>
#include <input/constanteffect.h>
#include <input/wheelrumble.h>
#endif

#ifdef RAD_WIN32
const short         DEFAULT_SPRING_COEFF        = 3000;
#else
const short         DEFAULT_SPRING_COEFF        = 50;
#endif
const char          DEFAULT_SPRING_OFFSET       = 0;
const unsigned char DEFAULT_SPRING_DEADBAND     = 0;
const unsigned char DEFAULT_SPRING_SAT          = 0;

const char          DEFAULT_DAMPER_OFFSET       = 0;
const unsigned char DEFAULT_DAMPER_DEADBAND     = 5;
const unsigned char DEFAULT_DAMPER_SAT          = 0;
const short         DEFAULT_DAMPER_COEFF        = 0;

const short          DEFAULT_CONSTANT_MAGNITUDE  = 0;
const unsigned short DEFAULT_CONSTANT_DIRECTION  = 0;

#ifdef DEBUGWATCH
short gDamperSlip = -100;
short gDamperMax = 100;
float gDamperSpeed = 200.0f;

unsigned char gSpringSlip = 20;
unsigned char gSpringMax = 200;
float gSpringSpeed = 80.0f;
#else
const float gDamperSpeed = 200.0f;

#ifdef RAD_WIN32
const u16 gSpringMax = 1000;
const unsigned char gSpringSlip = 20;
const short gDamperMax = 100;
const short gDamperSlip = -600;
#else
const unsigned char gSpringMax = 200;
const unsigned char gSpringSlip = 20;
const short gDamperMax = 100;
const short gDamperSlip = -100;
#endif

const float gSpringSpeed = 80.0f;

const short gConstantMag = 0;
const unsigned short gConstantDirection = 0;

#endif

#ifdef RAD_GAMECUBE
const float GC_STEERING_FUDGE = 1.25f;
#endif

HumanVehicleController::HumanVehicleController( void )
:
mpMappable( 0 ),
mControllerId( -1 ),
#if defined(RAD_GAMECUBE) || defined(RAD_PS2) || defined(RAD_WIN32)
mSpring( NULL ),
mDamper( NULL ),
mConstantEffect( NULL ),
mWheelRumble( NULL ),
mHeavyWheelRumble( NULL ),
#endif
mDisableReset( false )
{
#ifdef RAD_PS2
    unsigned int i;
    for ( i = 0; i < Input::MaxUSB; ++i )
    {
        mpWheel[ i ] = NULL;
    }
#endif
}

void HumanVehicleController::Create( Vehicle* pVehicle, VehicleMappable* pMappable, int controllerId )
{
    SetVehicle( pVehicle );
    tRefCounted::Assign( mpMappable, pMappable );
    mControllerId = controllerId;
}

#ifdef RAD_PS2
void HumanVehicleController::SetWheel( VehicleMappable* pMappable, unsigned int wheelNum )
{
    rAssert( wheelNum < Input::MaxUSB );

    tRefCounted::Assign( mpWheel[ wheelNum ], pMappable );
}
#endif

HumanVehicleController::~HumanVehicleController( void )
{
    ReleaseVehicleMappable();

#ifdef RAD_PS2
    unsigned int i;
    for ( i = 0; i < Input::MaxUSB; ++i )
    {
        if ( mpWheel[ i ] )
        {
            mpWheel[ i ]->Release();
            mpWheel[ i ] = NULL;
        }
    }
#endif
}

void HumanVehicleController::ReleaseVehicleMappable( void )
{
    if ( mpMappable )
    {
        mpMappable->Release();
        mpMappable = 0;
    }
}


float HumanVehicleController::GetGas( void ) const
{
#ifdef RAD_PS2
    float value = mpMappable->GetButton( VehicleMappable::Gas )->GetValue();

    if ( mControllerId >= Input::USB0 )
    {
        return value;
    }

    unsigned int i;
    for ( i = 0; i < Input::MaxUSB; ++i )
    {
        if ( mpWheel[ i ] )
        {
            float tempVal = mpWheel[ i ]->GetButton( VehicleMappable::Gas )->GetValue();
            if ( rmt::Fabs( tempVal ) > rmt::Fabs( value ) )
            {
                value = tempVal;
                break;
            }
        }
    }

    return value;
#else
	return mpMappable->GetButton( VehicleMappable::Gas )->GetValue();
#endif
}

float HumanVehicleController::GetThrottle( void ) const
{
    //No throttle on the wheels.
	return mpMappable->GetButton( VehicleMappable::Throttle )->GetValue();
}


float HumanVehicleController::GetBrake( void ) const
{
#ifdef RAD_PS2
    float value = mpMappable->GetButton( VehicleMappable::Brake )->GetValue();

    if ( mControllerId >= Input::USB0 )
    {
        return value;
    }

    unsigned int i;
    for ( i = 0; i < Input::MaxUSB; ++i )
    {
        if ( mpWheel[ i ] )
        {
            float tempVal = mpWheel[ i ]->GetButton( VehicleMappable::Brake )->GetValue();
            if ( rmt::Fabs( tempVal ) > rmt::Fabs( value ) )
            {
                value = tempVal;
                break;
            }
        }
    }

    return value;
#else
	return mpMappable->GetButton( VehicleMappable::Brake )->GetValue();
#endif
}

float HumanVehicleController::GetSteering( bool& isWheel ) const
{
#ifdef RAD_PS2
    isWheel = false;

    float value = mpMappable->GetButton( VehicleMappable::Steer )->GetValue();

    if ( mControllerId >= Input::USB0 )
    {
        isWheel = true;
        return value;
    }

    unsigned int i;
    for ( i = 0; i < Input::MaxUSB; ++i )
    {
        if ( mpWheel[ i ] )
        {
            float tempVal = mpWheel[ i ]->GetButton( VehicleMappable::Steer )->GetValue();
            if ( rmt::Fabs( tempVal ) > rmt::Fabs( value ) )
            {
                value = tempVal;
                isWheel = true;
                break;
            }
        }
    }

    return value;
#else
    //How do I tell if this is from a wheel or a stick?
#ifdef RAD_GAMECUBE
    //This is cheating.
    isWheel = mpMappable->IsWheel();
    return rmt::Clamp( mpMappable->GetButton( VehicleMappable::Steer )->GetValue() * GC_STEERING_FUDGE, -1.0f, 1.0f );
#else
#ifdef RAD_WIN32
    isWheel = GetInputManager()->GetController(mControllerId)->IsWheel();
#endif
	return mpMappable->GetButton( VehicleMappable::Steer )->GetValue();
#endif

#endif
}

float HumanVehicleController::GetSteerLeft( void ) const
{
    //No steerleft on the wheels
	return mpMappable->GetButton( VehicleMappable::SteerLeft )->GetValue();
}

float HumanVehicleController::GetSteerRight( void ) const
{
    //No steer right on the wheels
	return mpMappable->GetButton( VehicleMappable::SteerRight )->GetValue();
}

float HumanVehicleController::GetReverse( void ) const
{
    //No reverse mapped.
	return 0.0f;
}


float HumanVehicleController::GetHandBrake( void ) const
{
#ifdef RAD_PS2
    float value = mpMappable->GetButton( VehicleMappable::HandBrake )->GetValue();

    if ( mControllerId >= Input::USB0 )
    {
        return value;
    }

    unsigned int i;
    for ( i = 0; i < Input::MaxUSB; ++i )
    {
        if ( mpWheel[ i ] )
        {
            float tempVal = mpWheel[ i ]->GetButton( VehicleMappable::HandBrake )->GetValue();
            if ( rmt::Fabs( tempVal ) > rmt::Fabs( value ) )
            {
                value = tempVal;
                break;
            }
        }
    }

    return value;
#else
	return mpMappable->GetButton( VehicleMappable::HandBrake )->GetValue();
#endif
}

float HumanVehicleController::GetHorn( void ) const
{
#ifdef RAD_PS2
    float value = mpMappable->GetButton( VehicleMappable::Horn )->GetValue();

    if ( mControllerId >= Input::USB0 )
    {
        return value;
    }

    unsigned int i;
    for ( i = 0; i < Input::MaxUSB; ++i )
    {
        if ( mpWheel[ i ] )
        {
            float tempVal = mpWheel[ i ]->GetButton( VehicleMappable::Horn )->GetValue();
            if ( rmt::Fabs( tempVal ) > rmt::Fabs( value ) )
            {
                value = tempVal;
                break;
            }
        }
    }

    return value;
#else
	return mpMappable->GetButton( VehicleMappable::Horn )->GetValue();
#endif
}

void HumanVehicleController::Reset( void )
{
    if( !mDisableReset )
    {
    Vehicle* vehicle = GetVehicle();
	GetGuiSystem()->HandleMessage( GUI_MSG_MANUAL_RESET, reinterpret_cast< unsigned int >( vehicle ) );
    }
}

/*
==============================================================================
HumanVehicleController::GetMappable
==============================================================================
Description:    Comment

Parameters:     ( void )

Return:         VehicleMappable

=============================================================================
*/
VehicleMappable *HumanVehicleController::GetMappable( void ) const
{
    return mpMappable;
}

//=============================================================================
// HumanVehicleController::Update
//=============================================================================
// Description: Comment
//
// Parameters:  ( float timeins )
//
// Return:      void 
//
//=============================================================================
void HumanVehicleController::Update( float timeins )
{
    //This is where the output points will be set.

    Vehicle* vehicle = GetVehicle();
    float speed = vehicle->GetSpeedKmh();

#if defined(RAD_GAMECUBE) || defined(RAD_PS2) || defined(RAD_WIN32)
    UserController* uc = NULL;

    //Set up the output points to default settings.
#ifdef RAD_PS2
    //TODO: Make this only set up the active wheel.
    if ( mpWheel[ 0 ] != NULL )
    {
        uc = GetInputManager()->GetController( Input::USB0 );
    }
    else if ( mpWheel[ 1 ] != NULL )
    {
        uc = GetInputManager()->GetController( Input::USB1 );
    }
    else
    {
        uc = GetInputManager()->GetController( mControllerId );
    }
#elif defined RAD_GAMECUBE || defined(RAD_WIN32)
    uc = GetInputManager()->GetController( mControllerId );
#endif

    SetupRumbleFeatures( uc );

    if ( mSpring || mDamper || mConstantEffect )
    {
        rAssert( mSpring && mDamper );
        //Update these effects.

        if ( vehicle->IsAirborn() ||
             vehicle->mVehicleState == VS_SLIP ||
             vehicle->mVehicleState == VS_EBRAKE_SLIP )
        {
#ifdef RAD_WIN32
            if( vehicle->IsAirborn() )
            {
                mSpring->SetSpringStrength( 0 );
                mSpring->SetSpringCoefficient( 0 );

                mDamper->SetDamperCoefficient( 0 );            
                mDamper->SetDamperStrength( 0 );
            }
            else
            {
                mDamper->SetDamperCoefficient( gDamperSlip );
                mDamper->SetDamperStrength( 255 );

                mSpring->SetSpringStrength( static_cast<u16>(50*speed) );
                mSpring->SetSpringCoefficient( gDamperSlip*20 );

                mConstantEffect->SetDirection( static_cast<u16>(90*speed) );
                mConstantEffect->SetMagnitude( static_cast<s16>(-200*speed) );
            }
#else
            //Kill the damper
            mDamper->SetDamperCoefficient( gDamperSlip );
            mDamper->SetDamperStrength( 0 );


            mSpring->SetSpringStrength( gSpringSlip );
            mSpring->SetSpringCoefficient( gDamperSlip );
#endif
        }
        else
        {

            if ( speed > gSpringSpeed )
            {
                speed = gSpringSpeed;
            }

#ifdef RAD_WIN32
            u16 springSat = static_cast<u16>(rmt::FtoL(gSpringMax * ( speed / gSpringSpeed )));
            if( speed < 5.0f )
            {
                if( rmt::Epsilon( 0.0f, speed, 0.05f ) ) springSat = gSpringMax;
                else springSat = static_cast<u16>((gSpringMax)/(speed));
            }
            springSat*=50;

#else
            unsigned char springSat = static_cast<unsigned char>(rmt::FtoL(gSpringMax * ( speed / gSpringSpeed )));
#endif
            mSpring->SetSpringStrength( springSat );
            mSpring->SetSpringCoefficient( DEFAULT_SPRING_COEFF );

            if ( speed > gDamperSpeed )
            {
                speed = gDamperSpeed;
            }

            short dampercoeff = gDamperMax - static_cast<short>(rmt::FtoL(rmt::LtoF(gDamperMax) * ( speed / gDamperSpeed )));
            mDamper->SetDamperCoefficient( dampercoeff );            
            mDamper->SetDamperStrength( DEFAULT_DAMPER_SAT );
        } 
    }
#endif
    //Update the rumble effect.
    switch ( vehicle->mTerrainType )
    {
    case TT_Grass:	// Grass type terrain most everything else which isn't road or sidewalk.
    case TT_Dirt:   // Dirt type terrain.
        {
            if ( speed > 40.0f ) //Hmmmm...  TODO: allow this to be modified
            {
                GetInputManager()->GetController( mControllerId )->ApplyEffect( RumbleEffect::GROUND2, 250 );

#if defined(RAD_GAMECUBE) || defined(RAD_PS2) || defined(RAD_WIN32)
                if ( mWheelRumble )
                {
//                    mWheelRumble->SetMagDir( 200, 90 );
//                    mWheelRumble->SetPPO( 120 - (80 * (speed / 80.0f)), 0, 0 );
                }
#endif
            }
        }
    case TT_Water:	// Water on surface type terrain.
    case TT_Gravel:	// Loose gravel type terrain.
        {
            if ( speed > 40.0f ) //Hmmmm...  TODO: allow this to be modified
            {
                GetInputManager()->GetController( mControllerId )->ApplyEffect( RumbleEffect::GROUND4, 250 );

#if defined(RAD_GAMECUBE) || defined(RAD_PS2) || defined(RAD_WIN32)
                if ( mWheelRumble )
                {
//                    mWheelRumble->SetMagDir( 200, 90 );
//                    mWheelRumble->SetPPO( 120 - (60 * (speed / 80.0f)), 0, 0 );
                }
#endif
            }
        }
    case TT_Sand:	// Sand type terrain.
        {
            if ( speed > 40.0f ) //Hmmmm...  TODO: allow this to be modified
            {
                GetInputManager()->GetController( mControllerId )->ApplyEffect( RumbleEffect::GROUND3, 250 );
            }
        }
    case TT_Metal:  // Powerplant and other structurs.
        {
            if ( speed > 40.0f ) //Hmmmm...  TODO: allow this to be modified
            {
                GetInputManager()->GetController( mControllerId )->ApplyEffect( RumbleEffect::GROUND2, 250 );
#if defined(RAD_GAMECUBE) || defined(RAD_PS2) || defined(RAD_WIN32)
                if ( mWheelRumble )
                {
//                    mWheelRumble->SetMagDir( 255, 90 );
//                    mWheelRumble->SetPPO( 120 - (60 * (speed / 80.0f)), 0, 0 );
                }
#endif
            }
        }
    case TT_Road:
    case TT_Wood:
    default:
        {
#if defined(RAD_GAMECUBE) || defined(RAD_PS2) //|| defined(RAD_WIN32)
            if ( mWheelRumble )
            {
                mWheelRumble->SetMagDir( 0, 0 );
                mWheelRumble->SetPPO( 0, 0, 0 );
            }
#endif
            break;
        }
    }

//    if ( vehicle->mSkidLevel > 0.0f )
//    {
//        GetInputManager()->GetController( mControllerId )->ApplyEffect( RumbleEffect::PEELOUT, 250 );
//    }
}

//=============================================================================
// HumanVehicleController::Init
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void HumanVehicleController::Init()
{
    if ( mControllerId == -1 )
    {
        return;
    }

    UserController* uc = NULL;

#if defined(RAD_GAMECUBE) || defined(RAD_PS2) || defined(RAD_WIN32)
    //Set up the output points to default settings.
#ifdef RAD_PS2
    //TODO: Make this only set up the active wheel.
    if ( mpWheel[ 0 ] != NULL )
    {
        uc = GetInputManager()->GetController( Input::USB0 );
    }
    else if ( mpWheel[ 1 ] != NULL )
    {
        uc = GetInputManager()->GetController( Input::USB1 );
    }
    else
    {
        uc = GetInputManager()->GetController( mControllerId );
    }
#elif defined RAD_GAMECUBE || defined(RAD_WIN32)
    uc = GetInputManager()->GetController( mControllerId );
#endif

    if ( uc )
    {
        SetupRumbleFeatures( uc );
    }
#endif

    GetEventManager()->AddListener( this, EVENT_BIG_CRASH );
    GetEventManager()->AddListener( this, EVENT_BIG_VEHICLE_CRASH );
    GetEventManager()->AddListener( this, EVENT_MINOR_CRASH );
    GetEventManager()->AddListener( this, EVENT_MINOR_VEHICLE_CRASH );
    GetEventManager()->AddListener( this, EVENT_RUMBLE_COLLISION );
}

//=============================================================================
// HumanVehicleController::Shutdown
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      void 
//
//=============================================================================
void HumanVehicleController::Shutdown()
{
#if defined(RAD_GAMECUBE) || defined(RAD_PS2) || defined(RAD_WIN32)
    //Stop the vehicle output point settings

    if ( mSpring )
    {
        mSpring = NULL;
    }

    if ( mDamper )
    {
        mDamper = NULL;
    }

    if ( mConstantEffect )
    {
        mConstantEffect = NULL;
    }

#endif

    GetEventManager()->RemoveAll( this );
}

//=============================================================================
// HumanVehicleController::HandleEvent
//=============================================================================
// Description: Comment
//
// Parameters:  ( EventEnum id, void* pEventData )
//
// Return:      void 
//
//=============================================================================
void HumanVehicleController::HandleEvent( EventEnum id, void* pEventData )
{
    Vehicle* vehicle = GetVehicle();
    float speed = vehicle->GetSpeedKmh();
    switch ( id )
    {
    case EVENT_BIG_CRASH:
    case EVENT_BIG_VEHICLE_CRASH:
        {
#if defined(RAD_PS2) || defined(RAD_GAMECUBE) || defined(RAD_WIN32)
            if ( mHeavyWheelRumble )
            {
#ifdef RAD_WIN32
                mHeavyWheelRumble->SetMagDir( static_cast<u16>(speed*20), 90 );
                mHeavyWheelRumble->SetPPO( 20, 0, 0 );
                if( mWheelRumble )
                {
                    mWheelRumble->SetMagDir( static_cast<u16>(speed*20), 90 );
                    mWheelRumble->SetPPO( 20, 0, 0 );
                }
#else
                mHeavyWheelRumble->SetMagDir( 180, 90 );
                mHeavyWheelRumble->SetPPO( 20, 0, 0 );
#endif 
            }
#endif
//            GetInputManager()->GetController( mControllerId )->ApplyEffect( RumbleEffect::HARD1, 250 );
//            GetInputManager()->GetController( mControllerId )->ApplyEffect( RumbleEffect::HARD2, 250 );
            break;
        }
    case EVENT_MINOR_CRASH:
    case EVENT_MINOR_VEHICLE_CRASH:
        {
#ifdef RAD_WIN32
            if ( mWheelRumble )
            {
                mWheelRumble->SetMagDir( 100, 90 );
                mWheelRumble->SetPPO( 20, 0, 0 );
            }
#endif
//            GetInputManager()->GetController( mControllerId )->ApplyEffect( RumbleEffect::LIGHT, 250 );
//            GetInputManager()->GetController( mControllerId )->ApplyEffect( RumbleEffect::HARD2, 250 );
            break;
        }
    case EVENT_RUMBLE_COLLISION:
        {
            RumbleCollision* rc = static_cast<RumbleCollision*>(pEventData);
            if ( rc->vehicle == GetVehicle() && mControllerId != -1 && rc->vehicle->mSpeedKmh > 1.0f )
            {
                GetInputManager()->GetController( mControllerId )->ApplyDynaEffect( RumbleEffect::COLLISION1, 333, rc->normalizedForce );
                GetInputManager()->GetController( mControllerId )->ApplyDynaEffect( RumbleEffect::COLLISION2, 333, rc->normalizedForce );

#if defined(RAD_PS2) || defined(RAD_GAMECUBE) || defined(RAD_WIN32)
                if ( mConstantEffect )
                {
                    if ( mWheelRumble && rc->normalizedForce > 0.02f )
                    {
                        rmt::Vector thisPosition;
                        GetVehicle()->GetPosition( &thisPosition );

                        rmt::Vector X( 1.0f, 0.0f, 0.0f );                
                        rmt::Matrix mat = GetVehicle()->GetTransform();
                        mat.IdentityTranslation();

                        X.Transform( mat );

                        rmt::Vector carToPoint;
                        carToPoint.Sub( rc->point, thisPosition );
                        carToPoint.NormalizeSafe();

                        float dot = carToPoint.DotProduct( X );
                        bool left = ( dot < 0.0f );
                        u16 direction = rmt::FtoL( rmt::Fabs(dot) * 90.0f );
                        if ( left )
                        {
                            direction = (90 + 180) + ( 90 - (direction) );
                        }

                        mConstantEffect->SetDirection( direction );
                        mConstantEffect->SetMagnitude( 200 );
                    }
                }

                if ( mWheelRumble && rc->normalizedForce > 0.008f )
                {
                    mWheelRumble->SetMagDir( 180, 90 );
                    mWheelRumble->SetPPO( static_cast<u16>( rmt::FtoL(rmt::Clamp( rc->normalizedForce, 0.5f, 1.0f ) * 200.0f ) ),
                                          0, 0 );
                }
#endif
            }
        }
    default:
        break;
    }
}


#if defined(RAD_GAMECUBE) || defined(RAD_PS2) || defined(RAD_WIN32)
//=============================================================================
// HumanVehicleController::SetupRumbleFeatures
//=============================================================================
// Description: Comment
//
// Parameters:  ( UserController* uc )
//
// Return:      void 
//
//=============================================================================
void HumanVehicleController::SetupRumbleFeatures( UserController* uc )
{
    if ( !mSpring )
    {
        mSpring = uc->GetSpring();
        if ( mSpring )
        {
            //Set default settings for the spring
            mSpring->SetCenterPoint( DEFAULT_SPRING_OFFSET, DEFAULT_SPRING_DEADBAND );
            mSpring->SetSpringCoefficient( DEFAULT_SPRING_COEFF );
            mSpring->SetSpringStrength( DEFAULT_SPRING_SAT );
        }
    }

    if ( !mDamper )
    {
        mDamper = uc->GetDamper();
        if ( mDamper )
        {
            //Set default settings for the damper
            mDamper->SetCenterPoint( DEFAULT_SPRING_OFFSET, DEFAULT_SPRING_DEADBAND );
            mDamper->SetDamperCoefficient( DEFAULT_SPRING_COEFF );
            mDamper->SetDamperStrength( DEFAULT_SPRING_SAT );
        }
    }

    if ( !mConstantEffect )
    {
        mConstantEffect = uc->GetConstantEffect();
        if ( mConstantEffect )
        {
            //Default
            mConstantEffect->SetMagnitude( DEFAULT_CONSTANT_MAGNITUDE );
            mConstantEffect->SetDirection( DEFAULT_CONSTANT_DIRECTION );
        }
    }

    if ( !mWheelRumble )
    {
        mWheelRumble = uc->GetWheelRumble();
        if ( mWheelRumble )
        {
            mWheelRumble->SetMagDir( 0, 0 );
            mWheelRumble->SetPPO( 0, 0, 0 );
        }
    }

    if ( !mHeavyWheelRumble )
    {
        mHeavyWheelRumble = uc->GetHeavyWheelRumble();
        if ( mHeavyWheelRumble )
        {
            mHeavyWheelRumble->SetMagDir( 0, 0 );
            mHeavyWheelRumble->SetPPO( 0, 0, 0 );
        }
    }
}
#endif
