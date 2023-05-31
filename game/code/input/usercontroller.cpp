#include <input/usercontroller.h>
#include <input/mappable.h>
#include <input/mapper.h>
#include <input/button.h>

#include <radcontroller.hpp>
#include <raddebug.hpp>
#include <radmath/radmath.hpp>

#if defined(RAD_PS2) || defined(RAD_GAMECUBE)
#include <input/wheeldefines.h>
#endif



#ifndef WORLD_BUILDER
#include <main/commandlineoptions.h>
#endif

#ifdef WORLD_BUILDER
#ifdef CONTROLLER_DEBUG
#undef CONTROLLER_DEBUG
#endif
enum { CLO_NO_HAPTIC, CLO_RANDOM_BUTTONS };
namespace CommandLineOptions
{
    static bool Get( int i ) { return false; };
};
#endif

// We use ::strcmp()
//
#include <string.h>

#include <raddebugwatch.hpp>
#ifdef DEBUGWATCH
#include <input/inputmanager.h>


struct ButtonData
{
    ButtonData() : controllerID( -1 ), button( -1 ), range( 1.0f ) {};
    int controllerID;
    int button;
    float range;
};

static ButtonData gData[Input::MaxPhysicalButtons];

void ButtonCallback( void* userData )
{
    ButtonData* data = static_cast<ButtonData*>( userData );

    if ( data->button != Input::INVALID_CONTROLLERID )
    {
        UserController* controller = GetInputManager()->GetController( data->controllerID );
          
        Button* button = controller->GetInputButton( data->button );
        button->SetValue( 1.0f );
        button->ForceChange();

        for ( unsigned j = 0; j < Input::MaxMappables; j++ )
        {
            Mappable* mappable = controller->GetMappable( j );
            if ( mappable )
            {
                mappable->DispatchOnButton( data->controllerID, data->button, button );
            }
        }

        button->SetValue( 0.0f );
        button->ForceChange();

        for ( unsigned j = 0; j < Input::MaxMappables; j++ )
        {
            Mappable* mappable = controller->GetMappable( j );
            if ( mappable )
            {
                mappable->DispatchOnButton( data->controllerID, data->button, button );
            }
        }
    }
}

#endif

//This enables the random button pressing.
#define CONTROLLER_DEBUG
#ifdef CONTROLLER_DEBUG

#define MAX_BUTTON_TIME 3000
#define MAX_BUTTON_PRESSES 8
struct RandomButton
{
    RandomButton() : 
        mRandomButtonEnable( false ), 
        mRandomButtonTiming( 0 ), 
        mRadomizeButtonTiming( false ), 
        mButtonTimeout( 0 ), 
        mMaxButtons( MAX_BUTTON_PRESSES ), 
        mCurrentButton( 0 )
    {
        int i;
        for ( i = 0; i < MAX_BUTTON_PRESSES; ++i )
        {
            mLastButton[ i ] = -1;
        }
    }

    bool mRandomButtonEnable;
    unsigned int mRandomButtonTiming;
    bool mRadomizeButtonTiming;
    int mButtonTimeout;
    int mLastButton[ MAX_BUTTON_PRESSES ];
    unsigned int mMaxButtons;
    unsigned int mCurrentButton;
};

RandomButton gRandomButtoners[ Input::MaxControllers ];

#endif

//******************************************************************************
//
// Constructors, Destructors and Operators
//
//******************************************************************************
UserController::UserController( )
:
m_iPlayerIndex( -1 ),
m_xIController2( NULL ),
m_controllerId( -1 ),
m_bConnected( false ),
mbInputPointsRegistered( false ),
mGameState( Input::ACTIVE_ALL ),
mbIsRumbleOn( false )
{
    unsigned int i = 0;
    for ( i = 0; i < Input::MaxMappables; i++ )
    {
        this->mMappable[ i ] = 0;
    }

#if defined(RAD_PS2) || defined(RAD_GAMECUBE)
    mHeavyWheelRumble.SetRumbleType( LG_TYPE_SQUARE );
#endif
}

void UserController::NotifyConnect( void )
{
    if ( !IsConnected( ) )
    {
        m_bConnected = true;

        for ( unsigned int i = 0; i < Input::MaxMappables; i++ )
        {
            if (mMappable[ i ])
                mMappable[ i ]->OnControllerConnect( GetControllerId( ) );
        }
    }
}
void UserController::NotifyDisconnect( void )
{
    if ( IsConnected( ) )
    {
        m_bConnected = false;

        for ( unsigned int i = 0; i < Input::MaxMappables; i++ )
        {
            if (mMappable[ i ])
                mMappable[ i ]->OnControllerDisconnect( GetControllerId( ) );
        }
    }
}

void UserController::Create( int id )
{
    m_controllerId = id;
}

void UserController::SetGameState( unsigned state)
{
    mGameState = state;
    for ( unsigned i = 0; i < Input::MaxMappables; i++ )
    {
        if(mMappable[ i ])
        {
            mMappable[ i ]->SetGameState( state );

            // if the game state change involved and inactive mappable being activated
            // wwe need to reload the current button state
            if(mMappable[ i ]->GetResync())
            {
                mMappable[ i ]->InitButtons( m_controllerId, mButtonArray);
                mMappable[ i ]->SetResync(false);
            }
        }
    }
}

void UserController::OnControllerInputPointChange( unsigned int buttonId, float value )
{
    // We need to query the input point directly to get the value
    // remapped to the requested range.

	float fLastValue = mButtonArray[ buttonId ].GetValue( );
    float fDeadZone = mButtonDeadZones[buttonId];

    value = m_xIController2->GetInputPointByIndex( buttonId )->GetCurrentValue( );

	if ( rmt::Epsilon( value, 0.0f, fDeadZone ) )
	{
		value = 0.0f;
	}
	else
	{
		float sign = rmt::Sign( value );
		float calibratedButtonData = rmt::Fabs( value ) - fDeadZone;
		calibratedButtonData *= 1.0f / ( 1.0f - fDeadZone );
		calibratedButtonData *= sign;
		bool bChanged = !( rmt::Epsilon( fLastValue, calibratedButtonData, 0.05f ) );

		if ( bChanged )
		{
			// go with the deadzone corrected value.
			value = calibratedButtonData;
		}
		else
		{
			// restore the old deadzone corrected value.
			value = fLastValue;
		}
	}

    mButtonArray[ buttonId ].SetValue( value );
}

void UserController::Initialize( IRadController* pIController2 )
{
    // if we are being called while initialized, something is wrong
    rAssert(!mbInputPointsRegistered && !m_xIController2);

    m_xIController2 = pIController2;

    if( !mbInputPointsRegistered && m_xIController2 != NULL )
    {
        // register input point callbacks
        unsigned int i;

        for( i = 0; i < Input::MaxPhysicalButtons; i++ )
        {
            mButtonNames[i] = 0;
        }

        // get number of input points on controller
        mNumButtons = m_xIController2->GetNumberOfInputPoints();
        rAssert(mNumButtons < Input::MaxPhysicalButtons);

        // register all input points
        for( i = 0; i < mNumButtons; i++ )
        {
            IRadControllerInputPoint* pInputPoint = m_xIController2->GetInputPointByIndex( i );
            if( pInputPoint != NULL )
            {
                const char* inputPointType = pInputPoint->GetType();

                if( ::strcmp( inputPointType, "Button" ) == 0 )
                {
                    pInputPoint->SetTolerance( 1.0f );
                    mButtonDeadZones[ i ] = 0.0f;
                }
                else if( strcmp( inputPointType, "XAxis" ) == 0
                    || strcmp( inputPointType, "YAxis" ) == 0)
                {
                    pInputPoint->SetTolerance( 0.01f );
                    pInputPoint->SetRange( -1.0f, 1.0f );

                    if ( strcmp( pInputPoint->GetName(), "Wheel" ) == 0 )
                    {
                        mButtonDeadZones[ i ] = 0.025f;
                    }
                    else
                    {
                        mButtonDeadZones[ i ] = 0.25f;
                    }
                }
                else // Analog?
                {
                    pInputPoint->SetTolerance( 0.01f );
                    pInputPoint->SetRange( 0.0f, 1.0f );
                    mButtonDeadZones[ i ] = 0.0f;
                }
                mButtonNames[ i ] = radMakeKey(pInputPoint->GetName( ) );
                pInputPoint->RegisterControllerInputPointCallback( static_cast< IRadControllerInputPointCallback* >( this ), i );

                // [ps] Initialize the input points
                OnControllerInputPointChange( i , pInputPoint->GetCurrentValue( ) );

#ifdef DEBUGWATCH
                char name[64];
                sprintf( name, "%s Button", pInputPoint->GetName() );
                char nameSpace[64];
                sprintf( nameSpace, "Controller\\Input%d", m_controllerId );
                gData[m_controllerId].controllerID = m_controllerId;
                gData[m_controllerId].button = static_cast<InputManager::eButtonMap>(i);
                float min, max;
                pInputPoint->GetRange( &min, &max );
                gData[m_controllerId].range = max;
                radDbgWatchAddFunction( name, &ButtonCallback, &gData[m_controllerId], nameSpace );
#endif
            }
        }
    
        mbInputPointsRegistered = true;

#if defined(RAD_GAMECUBE) || defined(RAD_PS2)
        //Get the steering spring.
        IRadControllerOutputPoint* p_OutputPoint = m_xIController2->GetOutputPointByName( "CenterSpring" );
        if ( p_OutputPoint )
        {
            mSteeringSpring.Init( p_OutputPoint );
        }

        p_OutputPoint = m_xIController2->GetOutputPointByName( "BaseDamper" );
        if ( p_OutputPoint )
        {
            mSteeringDamper.Init( p_OutputPoint );
        }

        p_OutputPoint = m_xIController2->GetOutputPointByName( "Constant" );
        if ( p_OutputPoint )
        {
            mConstantEffect.Init( p_OutputPoint );
        }

        p_OutputPoint = m_xIController2->GetOutputPointByName( "Rumble" );
        if ( p_OutputPoint )
        {
            mWheelRumble.Init( p_OutputPoint );
        }

        p_OutputPoint = m_xIController2->GetOutputPointByName( "HeavyRumble" );
        if ( p_OutputPoint )
        {
            mHeavyWheelRumble.Init( p_OutputPoint );
        }
#endif

#ifdef RAD_GAMECUBE
        p_OutputPoint = m_xIController2->GetOutputPointByName( "Motor" );
        if ( p_OutputPoint )
        {
            mRumbleEffect.SetMotor( 0,  p_OutputPoint );
        }
#elif defined( RAD_PS2 )
        p_OutputPoint = m_xIController2->GetOutputPointByName( "SmallMotor" );
        if ( p_OutputPoint )
        {
            mRumbleEffect.SetMotor( 0,  p_OutputPoint );
        }
        
        p_OutputPoint = m_xIController2->GetOutputPointByName( "LargeMotor" );
        if ( p_OutputPoint )
        {
            mRumbleEffect.SetMotor( 1,  p_OutputPoint );
        }
#else
        IRadControllerOutputPoint* p_OutputPoint = m_xIController2->GetOutputPointByName( "LeftMotor" );
        if ( p_OutputPoint )
        {
            mRumbleEffect.SetMotor( 0,  p_OutputPoint );
        }

        p_OutputPoint = m_xIController2->GetOutputPointByName( "RightMotor" );
        if ( p_OutputPoint )
        {
            mRumbleEffect.SetMotor( 1,  p_OutputPoint );
        }
#endif
    }

#ifdef CONTROLLER_DEBUG
#ifdef DEBUGWATCH
    char nameSpace[64];
    sprintf( nameSpace, "Controller\\Input%d", m_controllerId );
    RandomButton& rb = gRandomButtoners[ m_controllerId ];
    radDbgWatchAddBoolean( &rb.mRandomButtonEnable, "Enable Random Buttons", nameSpace );
    radDbgWatchAddUnsignedInt( &rb.mRandomButtonTiming, "Button Timing", nameSpace, NULL, NULL, 0, MAX_BUTTON_TIME );
    radDbgWatchAddBoolean( &rb.mRadomizeButtonTiming, "Randomize Timing", nameSpace );
    radDbgWatchAddUnsignedInt( &rb.mMaxButtons, "Max Simul. Buttons", nameSpace, NULL, NULL, 1, MAX_BUTTON_PRESSES );
#endif
#endif
}

void UserController::ReleaseRadController( void )
{
    // deregister input point callbacks
    if( mbInputPointsRegistered && m_xIController2 != NULL )
    {
        // get number of input points on controller.
        //
        const unsigned int NUM_INPUT_POINTS = m_xIController2->GetNumberOfInputPoints();
        unsigned int i;

        // unregister all input points.
        //
        for( i = 0; i < NUM_INPUT_POINTS; i++ )
        {
            IRadControllerInputPoint* pInputPoint = m_xIController2->GetInputPointByIndex( i );
            if( pInputPoint != NULL )
            {
                pInputPoint->UnRegisterControllerInputPointCallback( static_cast< IRadControllerInputPointCallback* >( this ) );
#ifdef DEBUGWATCH
                char name[64];
                sprintf( name, "%s Button", pInputPoint->GetName() );
                radDbgWatchDelete( &name );
#endif
            }
            mButtonArray[ i ].SetValue(0.0f);
        }

        mbInputPointsRegistered = false;
    }

    mRumbleEffect.ShutDownEffects();

    this->m_xIController2 = NULL;

#ifdef CONTROLLER_DEBUG
#ifdef DEBUGWATCH
    RandomButton& rb = gRandomButtoners[ m_controllerId ];
    radDbgWatchDelete( &rb.mRandomButtonEnable );
    radDbgWatchDelete( &rb.mRandomButtonTiming );
    radDbgWatchDelete( &rb.mRadomizeButtonTiming );
    radDbgWatchDelete( &rb.mMaxButtons );
#endif
#endif
}

void UserController::SetRumble( bool bRumbleOn, bool pulse )
{
#if defined(RAD_GAMECUBE) || defined(RAD_PS2)
    if ( bRumbleOn && !mbIsRumbleOn && !CommandLineOptions::Get( CLO_NO_HAPTIC ) )
    {
        mSteeringSpring.Start();
        mSteeringDamper.Start();
        mConstantEffect.Start();
        mWheelRumble.Start();
        mHeavyWheelRumble.Start();
    }
    else if ( !bRumbleOn && mbIsRumbleOn )
    {
        mSteeringSpring.Stop();
        mSteeringDamper.Stop();
        mConstantEffect.Stop();
        mWheelRumble.Stop();
        mHeavyWheelRumble.Stop();
    }
#endif

    if ( pulse )
    {
        PulseRumble();
    }

    if ( !bRumbleOn && mbIsRumbleOn )
    {
        mRumbleEffect.ShutDownEffects();
    }

    mbIsRumbleOn = bRumbleOn;
}

bool UserController::IsRumbleOn( void ) const
{
    return mbIsRumbleOn;
}

void UserController::PulseRumble()
{
    mRumbleEffect.SetEffect( RumbleEffect::PULSE, 500 );
}

void UserController::ApplyEffect( RumbleEffect::Effect effect, unsigned int durationms )
{
    if ( mbIsRumbleOn && !CommandLineOptions::Get( CLO_NO_HAPTIC ) )
    {
        mRumbleEffect.SetEffect( effect, durationms );
    }
}

void UserController::ApplyDynaEffect( RumbleEffect::DynaEffect effect, unsigned int durationms, float gain )
{
    if ( mbIsRumbleOn && !CommandLineOptions::Get( CLO_NO_HAPTIC ) )
    {
#ifdef RAD_XBOX
        gain *= 2.0f;
#endif
        mRumbleEffect.SetDynaEffect( effect, durationms, gain );
    }
}

void UserController::Update( unsigned timeins )
{
    if(!IsConnected())
        return;

#ifdef CONTROLLER_DEBUG
    if ( gRandomButtoners[ m_controllerId ].mRandomButtonEnable || CommandLineOptions::Get( CLO_RANDOM_BUTTONS ) )
    {
        RandomButton& rb = gRandomButtoners[ m_controllerId ];
        rb.mButtonTimeout -= static_cast<int>( timeins );
        if ( rb.mButtonTimeout < 0 )
        {
            rb.mButtonTimeout = 0;
        }

        if ( rb.mButtonTimeout == 0 )
        {
            //Reset the last button.
            if ( rb.mLastButton[ rb.mCurrentButton ] != -1 )
            {
                mButtonArray[ rb.mLastButton[ rb.mCurrentButton ] ].SetValue( 0.0f );
                mButtonArray[ rb.mLastButton[ rb.mCurrentButton ] ].ForceChange();

                for ( unsigned j = 0; j < Input::MaxMappables; j++ )
                {
                    if ( mMappable[ j ] )
                    {
                        mMappable[ j ]->DispatchOnButton( m_controllerId, rb.mLastButton[ rb.mCurrentButton ], &mButtonArray[ rb.mLastButton[ rb.mCurrentButton ] ] );
                    }
                }
            }

            rb.mLastButton[ rb.mCurrentButton ] = rand() % Input::MaxPhysicalButtons;
            mButtonArray[ rb.mLastButton[ rb.mCurrentButton ] ].SetValue( 1.0f );
            mButtonArray[ rb.mLastButton[ rb.mCurrentButton ] ].ForceChange();

            for ( unsigned j = 0; j < Input::MaxMappables; j++ )
            {
                if ( mMappable[ j ] )
                {
                    mMappable[ j ]->DispatchOnButton( m_controllerId, rb.mLastButton[ rb.mCurrentButton ], &mButtonArray[ rb.mLastButton[ rb.mCurrentButton ] ] );
                }
            }

            //Increment the current button
            rb.mCurrentButton = (rb.mCurrentButton + 1) % rb.mMaxButtons;

            if ( rb.mRadomizeButtonTiming )
            {
                rb.mRandomButtonTiming = rand() % MAX_BUTTON_TIME;
            }

            rb.mButtonTimeout = rb.mRandomButtonTiming;
        }
    }

#endif

    // update the logical controller button values
    for(unsigned int i = 0; i < Input::MaxPhysicalButtons; i++)
    {
        // always rebrodcast non-zero button values (otherwise multiple physical -> single logical
        // button mappiung will screw up in some cases)
        if((mButtonArray[ i ].TimeSinceChange() == 0) || (mButtonArray[ i ].IsDown()))
        {
            for ( unsigned j = 0; j < Input::MaxMappables; j++ )
            {
                if ( mMappable[ j ] )
                {
                    mMappable[ j ]->DispatchOnButton( m_controllerId, i, &mButtonArray[ i ] );
                }
            }
        }
    }

#ifdef RAD_GAMECUBE
    if ( mbIsRumbleOn )
    {
#endif

#if defined(RAD_GAMECUBE) || defined(RAD_PS2)
        mSteeringSpring.Update();
        mSteeringDamper.Update();
        mConstantEffect.Update();
        mWheelRumble.Update();
        mHeavyWheelRumble.Update();
#endif

        //I leave this out so the FE can rumble me anyway.
        mRumbleEffect.Update( timeins );

#ifdef RAD_GAMECUBE
    }
#endif
}

// Returns the value stored by input point at index.
float UserController::GetInputValue( unsigned int index ) const
{
    return mButtonArray[ index ].GetValue( );
}

// Returns the real-time value of the input point at index.
float UserController::GetInputValueRT( unsigned int index ) const
{
    rAssert( m_xIController2 != NULL );

    IRadControllerInputPoint* pInputPoint = m_xIController2->GetInputPointByIndex( index );
    if( pInputPoint != NULL )
    {
        return pInputPoint->GetCurrentValue();
    }
    else
    {
        return 0.0f;
    }
}

Button* UserController::GetInputButton( unsigned int index )
{
    return &mButtonArray[ index ];
}


UserController::~UserController( void )
{
    unsigned int i = 0;
    for ( i = 0; i < Input::MaxMappables; i++ )
    {
        if( this->mMappable[ i ] != NULL )
        {
            mMappable[ i ]->Release( );
            mMappable[ i ] = 0;
        }
    }
}

int UserController::RegisterMappable( Mappable *pMappable )
{
    // Find a slot.
    unsigned int i = 0;
    for ( i = 0; i < Input::MaxMappables; i++ )
    {
        if( this->mMappable[ i ] == NULL )
        {
            break;
        }
    }

    // Assign the mappable.
    if ( i < Input::MaxMappables )
    {
        this->mMappable[ i ] = pMappable;
        mMappable[ i ]->AddRef( );
        mMappable[ i ]->LoadControllerMappings( m_controllerId );
        mMappable[ i ]->InitButtons( m_controllerId, mButtonArray);
    }
    else
    {
        rAssertMsg( false, "Not enough mappables allocated.\n" );
        return -1;
    }

    // make sure the input state get correctly set up for newly bound mappables
    pMappable->SetResync(true);
    pMappable->SetGameState(mGameState);

    return (int)i;
}

void UserController::UnregisterMappable( int handle )
{
    rAssert( handle < static_cast< int >( Input::MaxMappables ) );
    if ( mMappable[ handle ] )
    {
        mMappable[ handle ]->Reset( );
        mMappable[ handle ]->Release( );
        mMappable[ handle ] = 0;
        LoadControllerMappings();
    }
}

void UserController::UnregisterMappable( Mappable* mappable)
{
    for ( unsigned i = 0; i < Input::MaxMappables; i++ )
    {
        if( this->mMappable[ i ] == mappable )
        {
            mMappable[ i ]->Reset( );
            mMappable[ i ]->Release( );
            mMappable[ i ] = 0;
            LoadControllerMappings();
            return;
        }
    }
}

void UserController::LoadControllerMappings( void )
{
    unsigned int i = 0;
    for ( i = 0; i < Input::MaxMappables; i++ )
    {
        if( this->mMappable[ i ] != NULL )
        {
            this->mMappable[ i ]->LoadControllerMappings( m_controllerId );
            mMappable[ i ]->InitButtons( m_controllerId, mButtonArray);
        }
    } 
}
 
// return the button id from the name.
int UserController::GetIdByName( const char* pszName ) const
{
    radKey key = radMakeKey(pszName);
    unsigned int i;
    for( i = 0; i < Input::MaxPhysicalButtons; i++ )
    {
        if (mButtonNames[i] == key)
        {
            return i;
        }
    }
    return -1;
}
