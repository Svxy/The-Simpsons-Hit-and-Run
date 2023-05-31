//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   UserController
//
// Description: Implementation of the usercontroller class for win32.
//
// History:     Branched from the console UserController class.
//
//===========================================================================

//========================================
// System Includes
//========================================

#include <radcontroller.hpp>
#include <raddebug.hpp>
#include <radmath/radmath.hpp>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <string.h>

//========================================
// Project Includes
//========================================

#include <input/usercontrollerWin32.h>
#include <input/mappable.h>
#include <input/mapper.h>
#include <input/button.h>
#include <input/Mouse.h>
#include <input/Keyboard.h>
#include <input/Gamepad.h>
#include <input/SteeringWheel.h>
#include <input/inputmanager.h>

#include <input/steeringspring.h>
#include <input/baseDamper.h>
#include <input/constanteffect.h>
#include <input/wheelrumble.h>
#include <presentation/tutorialmanager.h>

#include <console/fbstricmp.h>
#include <data/config/gameconfigmanager.h>

#ifndef WORLD_BUILDER
#include <main/commandlineoptions.h>
#endif

#include <gameflow/gameflow.h>
#include <contexts/context.h>

// needed for the mouse reset hack.
static int maxes[] = { DIMOFS_X, DIMOFS_Y, DIMOFS_Z };

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

//=============================================================================
// Class: InputCode
//=============================================================================
//
// Description: Need this for compressing ( Controller / directX key code /
//              direction ) triplets into one integer.  Then we can re-use the
//              Mapper class for virtual maps.
//
//=============================================================================

class InputCode
{
public:
    InputCode( unsigned int inputcode ) { mInputCode = inputcode; }
    InputCode( eControllerType controller, int dxKey, eDirectionType direction )
    {
        mInputCode = dxKey | (direction << 24) | (controller << 28);
        rAssert( controller == GetController() );
        rAssert( dxKey == GetDxKeyCode() );
        rAssert( direction == GetDirection() );
    }

    eControllerType GetController() const 
    { 
        rAssert( (mInputCode >> 28) < NUM_CONTROLLERTYPES );
        return eControllerType( mInputCode >> 28 ); 
    }
    int GetDxKeyCode() const 
    { 
        return mInputCode & 0xFFFFFF; 
    }
    eDirectionType GetDirection() const 
    {
        rAssert( ((mInputCode >> 24) & 0xF) <= NUM_DIRTYPES );
        return eDirectionType( (mInputCode >> 24) & 0xF ); 
    }

    int GetInputCode() const { return mInputCode; }

private:
    unsigned int mInputCode;
};

//******************************************************************************
//
// Constructors, Destructors and Operators
//
//******************************************************************************
UserController::UserController( )
:
m_controllerId( -1 ),
mIsConnected( false ),
mGameState( Input::ACTIVE_ALL ),
mbInputPointsRegistered( false ),
mKeyboardBack( false ),
mbIsRumbleOn( false ),
m_bIsWheel( false ),
m_bTutorialDisabled(true)
{
    int i = 0;
    for ( i = 0; i < Input::MaxMappables; i++ )
    {
        mMappable[ i ] = 0;
    }

    for( i = 0; i < 3; i++ )
    {
        mResetMouseCounter[ i ] = 0;
    }

    // Set up the virtual buttons.
    mNumButtons = VirtualInputs::GetNumber();
    rAssert(mNumButtons < Input::MaxPhysicalButtons);

    for( i = 0; i < mNumButtons; i++ )
    {
        // Define the button names for the virtual keys
        mButtonNames[ i ] = radMakeKey( VirtualInputs::GetName( i ) );

        mButtonDeadZones[ i ] = 0.10f;
        mButtonSticky[ i ] = false;
    }
	
    // Set up the controllers for each type.
	m_pController[GAMEPAD] = new Gamepad();
	m_pController[KEYBOARD] = new Keyboard();
	m_pController[MOUSE] = new Mouse();
    m_pController[STEERINGWHEEL] = new Gamepad();

    m_pSteeringSpring   = new SteeringSpring;
    m_pSteeringDamper   = new BaseDamper;
    m_pConstantEffect   = new ConstantEffect;
    m_pWheelRumble      = new WheelRumble;
    m_pHeavyWheelRumble = new WheelRumble;

    //
    // Register with the game config manager
    //
    GetGameConfigManager()->RegisterConfig( this );
}

void UserController::NotifyConnect( void )
{
    if ( !IsConnected( ) )
    {
		for( int i = 0; i < NUM_CONTROLLERTYPES; i++ )
		{
			m_pController[i]->Connect();
		}

        for ( unsigned int i = 0; i < Input::MaxMappables, this->mMappable[ i ]; i++ )
        {
            mMappable[ i ]->OnControllerConnect( GetControllerId( ) );
        }

        mIsConnected = true;
    }
}
void UserController::NotifyDisconnect( void )
{
    if ( IsConnected( ) )
    {
		for( int i = 0; i < NUM_CONTROLLERTYPES; i++ )
		{
			m_pController[i]->Disconnect();
		}

        for ( unsigned int i = 0; i < Input::MaxMappables, this->mMappable[ i ]; i++ )
        {
            mMappable[ i ]->OnControllerDisconnect( GetControllerId( ) );
        }

        mIsConnected = false;
    }
}

bool UserController::IsConnected() const
{
	return mIsConnected;
}

void UserController::Create( int id )
{
    m_controllerId = id;

    // Now that we know our controller id,
    // load the default controller mappings.
    //
    LoadDefaults();
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

void UserController::SetButtonValue( unsigned int buttonId, float value, bool sticky )
{
    float fLastValue = mButtonArray[ buttonId ].GetValue( );
    float fDeadZone = mButtonDeadZones[ buttonId ];

    if( mButtonSticky[ buttonId ] && !sticky )
    {
        return; // don't overwrite a sticky button.
    }
    
    if ( rmt::Epsilon( value, 0.0f, fDeadZone ) )
    {
        value = 0.0f;
    }
    else
    {
        //Clamp our values.
        value = rmt::Clamp( value, MIN_AXIS_THRESH, MAX_AXIS_THRESH );

        // Recalibrate them from [deadzone..1] to [0..1].
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

    if( value > 0.0f || fLastValue > 0.0f )
    {
        mButtonArray[ buttonId ].SetValue( value );
    }

    // Reset the sticky flag.
    mButtonSticky[ buttonId ] = sticky;
    if( sticky && value == 0.0f )
    {
        mButtonSticky[ buttonId ] = false;
    }
}

void UserController::OnControllerInputPointChange( unsigned int code, float value )
{
    // Decode the virtual button code.
    InputCode icode( code );

    eControllerType cont = icode.GetController();
    int dxKey = icode.GetDxKeyCode();

    // Set the IsWheel boolean if this is a steering wheel axis input
    m_bIsWheel = STEERINGWHEEL == cont && m_pController[ cont ]->IsInputAxis( dxKey );

    // Set the keyboard back value
    if( KEYBOARD == cont && dxKey == DIK_ESCAPE && value > 0 )
    {
        mKeyboardBack = true;
    }

    // Derive the directional values
    float dvalues[ NUM_DIRTYPES ];
    int ndirections = DeriveDirectionValues( cont, dxKey, value, dvalues );

    // Check if we're remapping a key.
    if( mMapData.MapNext )
    {
        Remap( cont, dxKey, dvalues, ndirections );
        return;
    }

    // Look up what virtual buttons have been mapped to this input, then update them.
    for( int map = 0; map < NUM_MAPTYPES; map++ )
    {
        for( int dir = 0; dir < ndirections; dir++ )
        {
            int vButton = m_pController[ cont ]->GetMap( dxKey, (eDirectionType) dir, (eMapType) map );
            if( vButton != Input::INVALID_CONTROLLERID )
            {
                SetButtonValue( vButton, dvalues[ dir ], cont == KEYBOARD );
            }
            if( vButton == InputManager::feBack && cont != KEYBOARD )
            {
                mKeyboardBack = false;
            }
        }
    }
}

int UserController::DeriveDirectionValues( eControllerType type, int dxKey, float value, float* values )
{
    // Reset the directional values
    values[ DIR_UP ] = values[ DIR_DOWN ] = values[ DIR_RIGHT ] = values[ DIR_LEFT ] = 0.0f;

    // Calculate the values based on the type of input.
    if( m_pController[ type ]->IsInputAxis( dxKey ) )
    {
        // Normalize the axis.
        value = ( MAX_AXIS_THRESH - MIN_AXIS_THRESH ) * value + MIN_AXIS_THRESH;

        // Assign the values.
        values[ DIR_UP ] = ( value >= 0 ) ? value : 0.0f;
        values[ DIR_DOWN ] = ( value >= 0 ) ? 0.0f : -value;
        
        return 2;
    }
    else if(  m_pController[ type ]->IsMouseAxis( dxKey ) )
    {
        // Translate mouse val to -1..1 axis value.
        value = ((Mouse*)m_pController[MOUSE])->CalculateMouseAxis( dxKey, value );

        // Invert if necessary
        if( dxKey == DIMOFS_X && m_bInvertMouseX || 
            dxKey == DIMOFS_Y && m_bInvertMouseY )
        {
            value *= -1.0f;
        }

        if( dxKey == DIMOFS_X )
        {
            value *= m_fMouseSensitivityX;
        }
        else if( dxKey == DIMOFS_Y )
        {
            value *= m_fMouseSensitivityY;
        }

        // Assign the values.
        values[ DIR_UP ] = ( value >= 0 ) ? value : 0.0f;
        values[ DIR_DOWN ] = ( value >= 0 ) ? 0.0f : -value;

        // Initiate the mouse reset hack
        for( int i = 0; i < 3; i++ )
        {
            if( maxes[ i ] == dxKey && value != 0.0f )
            {
                mResetMouseCounter[ i ] = 1;
            }
        }

        return 2;
    }
    else if( m_pController[ type ]->IsPovHat( dxKey ) )
    {
        ((Gamepad*)m_pController[GAMEPAD])->CalculatePOV( value, &values[DIR_UP], &values[DIR_DOWN], 
                                                                 &values[DIR_RIGHT], &values[DIR_LEFT] );

        return 4;
    }
    else // It's a button.
    {
        values[ DIR_UP ] = value;

        return 1;
    }
}

void UserController::Initialize( RADCONTROLLER* pIController )
{
    // if we are being called while initialized, something is wrong
    rAssert(!mbInputPointsRegistered);

    // Initialize the controllers.
    //
    for( int i = 0; i < NUM_CONTROLLERTYPES; i++ )
    {
        m_pController[i]->Init( pIController[i] );
    }

    // Register the input points now.
    //
    RegisterInputPoints();

    RADCONTROLLER pSteeringWheel = m_pController[STEERINGWHEEL]->getController();

    static bool bSteeringSet = false;
    if( pSteeringWheel /*&& !bSteeringSet*/ )
    {
        //Get the steering spring.
        IRadControllerOutputPoint* p_OutputPoint = pSteeringWheel->GetOutputPointByName( "Spring" );
        if ( p_OutputPoint )
        {
            m_pSteeringSpring->Init( p_OutputPoint );
        }

        p_OutputPoint = pSteeringWheel->GetOutputPointByName( "Damper" );
        if ( p_OutputPoint )
        {
            m_pSteeringDamper->Init( p_OutputPoint );
        }

        p_OutputPoint = pSteeringWheel->GetOutputPointByName( "Constant" );
        if ( p_OutputPoint )
        {
            m_pConstantEffect->Init( p_OutputPoint );
        }

        p_OutputPoint = pSteeringWheel->GetOutputPointByName( "Sine Wave" );
        if ( p_OutputPoint )
        {
            m_pWheelRumble->Init( p_OutputPoint );
            m_pWheelRumble->SetResetTime( 1000 );
        }

        p_OutputPoint = pSteeringWheel->GetOutputPointByName( "Square Wave" );
        if ( p_OutputPoint )
        {
            m_pHeavyWheelRumble->Init( p_OutputPoint );
            m_pHeavyWheelRumble->SetResetTime( 1000 );
        }
        bSteeringSet = true;
    }

    // Set up rumbling for the gamepad.
    //
    RADCONTROLLER pGamepad = m_pController[GAMEPAD]->getController();
    if( pGamepad != NULL )
    {
        IRadControllerOutputPoint* p_OutputPoint = pGamepad->GetOutputPointByName( "LeftMotor" );
        if ( p_OutputPoint )
        {
            mRumbleEffect.SetMotor( 0,  p_OutputPoint );
        }

        p_OutputPoint = pGamepad->GetOutputPointByName( "RightMotor" );
        if ( p_OutputPoint )
        {
            mRumbleEffect.SetMotor( 1,  p_OutputPoint );
        }
    }
}

void UserController::ReleaseRadController( void )
{
    // deregister input point callbacks
    if( mbInputPointsRegistered )
    {
		//unregister the inputpoints associated with the controllers.
        int i;
        for( i = 0; i < NUM_CONTROLLERTYPES; i++ )
        {
            if( m_pController[ i ] != NULL )
            {
                m_pController[ i ]->ReleaseInputPoints( this );
            }
        }

        for( i = 0; i < mNumButtons; i++ )
        {
            mButtonArray[ i ].SetValue(0.0f);
        }

        mbInputPointsRegistered = false;
    }

    if( m_pSteeringSpring ) m_pSteeringSpring->ShutDownEffects();
    if( m_pSteeringDamper ) m_pSteeringDamper->ShutDownEffects();
    if( m_pConstantEffect ) m_pConstantEffect->ShutDownEffects();
    if( m_pWheelRumble ) m_pWheelRumble->ShutDownEffects();
    if( m_pHeavyWheelRumble ) m_pHeavyWheelRumble->ShutDownEffects();

    mRumbleEffect.ShutDownEffects();
}

void UserController::SetRumble( bool bRumbleOn, bool pulse )
{
    if ( bRumbleOn && !mbIsRumbleOn && !CommandLineOptions::Get( CLO_NO_HAPTIC ) && m_bForceFeedback )
    {
        StartForceEffects();
    }
    else if ( !bRumbleOn && mbIsRumbleOn && m_bForceFeedback )
    {
        StopForceEffects();
    }

    if ( pulse && m_bForceFeedback )
    {
        PulseRumble();
    }

    if ( !bRumbleOn && mbIsRumbleOn && m_bForceFeedback )
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

    // Reset any mouse inputs that have gone stale.
    ResetMouseAxes();

    // update the logical controller button values
    for(unsigned int i = 0; i < Input::MaxPhysicalButtons; i++)
    {
        // always rebrodcast non-zero button values (otherwise multiple physical -> single logical
        // button mapping will screw up in some cases)
        unsigned int timesincechange = mButtonArray[ i ].TimeSinceChange();
        bool bIsDown = mButtonArray[ i ].IsDown();
        if((timesincechange == 0) || bIsDown)
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

    m_pSteeringSpring->Update();
    m_pSteeringDamper->Update();
    m_pConstantEffect->Update();
    m_pWheelRumble->Update(timeins);
    m_pHeavyWheelRumble->Update(timeins);

    //I leave this out so the FE can rumble me anyway.
    mRumbleEffect.Update( timeins );
}

void UserController::StartForceEffects()
{
    m_pSteeringSpring->Start();
    m_pSteeringDamper->Start();
    m_pConstantEffect->Start();
    m_pWheelRumble->Start();
    m_pHeavyWheelRumble->Start();
}

void UserController::StopForceEffects()
{
    m_pSteeringSpring->Stop();
    m_pSteeringDamper->Stop();
    m_pConstantEffect->Stop();
    m_pWheelRumble->Stop();
    m_pHeavyWheelRumble->Stop();
}

// Returns the value stored by input point at index.
float UserController::GetInputValue( unsigned int index ) const
{
    switch( index )
    {
        case InputManager::LeftStickX:
        {
            float up = mButtonArray[ InputManager::CameraRight ].GetValue();
            float down = mButtonArray[ InputManager::CameraLeft ].GetValue();
            return ( up > down ) ? up : -down;
        }
        case InputManager::LeftStickY:
        {
            float up = mButtonArray[ InputManager::CameraMoveIn ].GetValue();
            float down = mButtonArray[ InputManager::CameraMoveOut ].GetValue();
            return ( up > down ) ? up : -down;
        }
        case InputManager::KeyboardEsc:
        {
            return mKeyboardBack ? 1.0f : 0.0f;
        }
        default:
            return mButtonArray[ index ].GetValue( );
    }
}

float UserController::GetInputValueRT( unsigned int index ) const
{
    // not supported.
    return 0.0f;
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
	for( int i = 0; i < NUM_CONTROLLERTYPES; i++ )
	{
		delete m_pController[i];
		m_pController[i] = NULL;
	}

    delete m_pSteeringSpring;
    m_pSteeringSpring = NULL;
    delete m_pSteeringDamper;
    m_pSteeringDamper = NULL;
    delete m_pConstantEffect;
    m_pConstantEffect = NULL;
    delete m_pWheelRumble;
    m_pWheelRumble = NULL;
    delete m_pHeavyWheelRumble;
    m_pHeavyWheelRumble = NULL;
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

SteeringSpring* UserController::GetSpring() 
{ 
    return m_pSteeringSpring->IsInit() ? m_pSteeringSpring : NULL; 
}

BaseDamper* UserController::GetDamper() 
{ 
    return m_pSteeringDamper->IsInit() ? m_pSteeringDamper : NULL; 
}

ConstantEffect* UserController::GetConstantEffect() 
{ 
    return m_pConstantEffect->IsInit() ? m_pConstantEffect : NULL; 
}

WheelRumble* UserController::GetWheelRumble() 
{ 
    return m_pWheelRumble->IsInit() ? m_pWheelRumble : NULL; 
}

WheelRumble* UserController::GetHeavyWheelRumble() 
{ 
    return m_pHeavyWheelRumble->IsInit() ? m_pHeavyWheelRumble : NULL; 
}

void UserController::RegisterInputPoints()
{
    // If we've already registered, then return.
    if( mbInputPointsRegistered )
    {
        return;
    }

    // Register input points for each controller.
    for( int type = 0; type < NUM_CONTROLLERTYPES; type++ )
    {
        // Retrieve the controller.. If not connected, skip.
        RADCONTROLLER pController = m_pController[type]->getController();
        if( pController == NULL )
        {
            continue;
        }

        // Go through each input point of the controller.
        unsigned num = pController->GetNumberOfInputPoints();
        for( unsigned ip = 0; ip < num; ip++ )
        {
            RegisterInputPoint( (eControllerType) type, ip );
        }
    }

    mbInputPointsRegistered = true;
}

void UserController::RegisterInputPoint( eControllerType type, int inputpoint )
{
    // Can only map when the controller connected.. otherwise no work.
	RADCONTROLLER pController = m_pController[type]->getController();
    rAssert( pController != NULL );

    IRadControllerInputPoint* pInputPoint = pController->GetInputPointByIndex( inputpoint );
    rAssert( pInputPoint != NULL );

    int dxKey = m_pController[type]->GetDICode( inputpoint );

    // For banned input points, return right away
    if( dxKey != Input::INVALID_CONTROLLERID && !m_pController[type]->IsBannedInput( dxKey ) )
    {
        // Register the callback.
        // the direction value below is unimportant & ignored.
        InputCode icode( type, dxKey, NUM_DIRTYPES );
        pInputPoint->RegisterControllerInputPointCallback( this, icode.GetInputCode() );
        m_pController[type]->AddInputPoints( pInputPoint );
    }
}

const char* UserController::GetConfigName() const
{
    // do this unfortunate switch cause of multiplayer possibilities :/
    // maybe clean it up later.
    switch( m_controllerId )
    {
        case -1:
            return "Control-default";
        case 0:
            return "Controller";
        case 1:
            return "Controller1";
        case 2:
            return "Controller2";
        case 3:
            return "Controller3";
        default:
            rAssert( false );
            return "Controller3";
    }
}

int UserController::GetNumProperties() const
{
    return Input::MaxVirtualMappings * Input::MaxPhysicalButtons + 8;
}

void UserController::LoadDefaults()
{
    ClearMappings();

    if ( m_controllerId == 0 )
    {
        // The primary mapping - to the keyboard & mouse.
        SetMap( SLOT_PRIMARY, InputManager::MoveUp, KEYBOARD, DIK_W );
        SetMap( SLOT_PRIMARY, InputManager::MoveDown, KEYBOARD, DIK_S );
        SetMap( SLOT_PRIMARY, InputManager::MoveLeft, KEYBOARD, DIK_A );
        SetMap( SLOT_PRIMARY, InputManager::MoveRight, KEYBOARD, DIK_D );
        SetMap( SLOT_PRIMARY, InputManager::Jump, KEYBOARD, DIK_SPACE );
        SetMap( SLOT_PRIMARY, InputManager::Sprint, KEYBOARD, DIK_LSHIFT );

        SetMap( SLOT_PRIMARY, InputManager::Accelerate, KEYBOARD, DIK_W );
        SetMap( SLOT_PRIMARY, InputManager::Reverse, KEYBOARD, DIK_S );
        SetMap( SLOT_PRIMARY, InputManager::SteerLeft, KEYBOARD, DIK_A );
        SetMap( SLOT_PRIMARY, InputManager::SteerRight, KEYBOARD, DIK_D );
        SetMap( SLOT_PRIMARY, InputManager::Horn, KEYBOARD, DIK_LSHIFT );
        SetMap( SLOT_PRIMARY, InputManager::ResetCar, KEYBOARD, DIK_SPACE );

        SetMap( SLOT_PRIMARY, InputManager::CameraLeft, KEYBOARD, DIK_NUMPAD4 );
        SetMap( SLOT_PRIMARY, InputManager::CameraRight, KEYBOARD, DIK_NUMPAD6 );
        SetMap( SLOT_PRIMARY, InputManager::CameraMoveIn, KEYBOARD, DIK_NUMPAD8 );
        SetMap( SLOT_PRIMARY, InputManager::CameraMoveOut, KEYBOARD, DIK_NUMPAD2 );
        SetMap( SLOT_PRIMARY, InputManager::CameraZoom, KEYBOARD, DIK_NUMPAD5 );
        SetMap( SLOT_PRIMARY, InputManager::CameraLookUp, KEYBOARD, DIK_NUMPAD0 );
        SetMap( SLOT_PRIMARY, InputManager::CameraToggle, KEYBOARD, DIK_NUMPAD0 );
        SetMap( SLOT_PRIMARY, InputManager::CameraCarLeft, KEYBOARD, DIK_NUMPAD4 );
        SetMap( SLOT_PRIMARY, InputManager::CameraCarRight, KEYBOARD, DIK_NUMPAD6 );
        SetMap( SLOT_PRIMARY, InputManager::CameraCarLookUp, KEYBOARD, DIK_NUMPAD8 );
        SetMap( SLOT_PRIMARY, InputManager::CameraCarLookBack, KEYBOARD, DIK_NUMPAD2 );
    }

    SetMap( SLOT_PRIMARY, InputManager::Attack, MOUSE, DIMOFS_BUTTON1 );
    SetMap( SLOT_PRIMARY, InputManager::DoAction, MOUSE, DIMOFS_BUTTON0 );

    SetMap( SLOT_PRIMARY, InputManager::GetOutCar, MOUSE, DIMOFS_BUTTON0 );
    SetMap( SLOT_PRIMARY, InputManager::HandBrake, MOUSE, DIMOFS_BUTTON1 );

    // The secondary mapping - to the gamepad.
    SetMap( SLOT_SECONDARY, InputManager::MoveUp, GAMEPAD, DIJOFS_Y, DIR_UP );
    SetMap( SLOT_SECONDARY, InputManager::MoveDown, GAMEPAD, DIJOFS_Y, DIR_DOWN );
    SetMap( SLOT_SECONDARY, InputManager::MoveLeft, GAMEPAD, DIJOFS_X, DIR_DOWN );
    SetMap( SLOT_SECONDARY, InputManager::MoveRight, GAMEPAD, DIJOFS_X, DIR_UP );
    SetMap( SLOT_SECONDARY, InputManager::Attack, GAMEPAD, DIJOFS_BUTTON0 );
    SetMap( SLOT_SECONDARY, InputManager::Jump, GAMEPAD, DIJOFS_BUTTON3 );
    SetMap( SLOT_SECONDARY, InputManager::Sprint, GAMEPAD, DIJOFS_BUTTON7 );
    SetMap( SLOT_SECONDARY, InputManager::DoAction, GAMEPAD, DIJOFS_BUTTON4 );

    SetMap( SLOT_SECONDARY, InputManager::Accelerate, GAMEPAD, DIJOFS_BUTTON7 );
    SetMap( SLOT_SECONDARY, InputManager::Reverse, GAMEPAD, DIJOFS_BUTTON6 );
    SetMap( SLOT_SECONDARY, InputManager::SteerLeft, GAMEPAD, DIJOFS_X, DIR_DOWN );
    SetMap( SLOT_SECONDARY, InputManager::SteerRight, GAMEPAD, DIJOFS_X, DIR_UP );
    SetMap( SLOT_SECONDARY, InputManager::GetOutCar, GAMEPAD, DIJOFS_BUTTON4 );
    SetMap( SLOT_SECONDARY, InputManager::HandBrake, GAMEPAD, DIJOFS_BUTTON1 );
    SetMap( SLOT_SECONDARY, InputManager::Horn, GAMEPAD, DIJOFS_BUTTON2 );
    SetMap( SLOT_SECONDARY, InputManager::ResetCar, GAMEPAD, DIJOFS_BUTTON5 );

    SetMap( SLOT_SECONDARY, InputManager::CameraLeft, GAMEPAD, DIJOFS_Z, DIR_DOWN );
    SetMap( SLOT_SECONDARY, InputManager::CameraRight, GAMEPAD, DIJOFS_Z, DIR_UP );
    SetMap( SLOT_SECONDARY, InputManager::CameraMoveIn, GAMEPAD, DIJOFS_RZ, DIR_UP );
    SetMap( SLOT_SECONDARY, InputManager::CameraMoveOut, GAMEPAD, DIJOFS_RZ, DIR_DOWN );
    SetMap( SLOT_SECONDARY, InputManager::CameraZoom, GAMEPAD, DIJOFS_BUTTON6 );
    SetMap( SLOT_SECONDARY, InputManager::CameraLookUp, GAMEPAD, DIJOFS_BUTTON7 );
    SetMap( SLOT_SECONDARY, InputManager::CameraToggle, GAMEPAD, DIJOFS_BUTTON5 );
    SetMap( SLOT_SECONDARY, InputManager::CameraCarLeft, GAMEPAD, DIJOFS_Z, DIR_DOWN );
    SetMap( SLOT_SECONDARY, InputManager::CameraCarRight, GAMEPAD, DIJOFS_Z, DIR_UP );
    SetMap( SLOT_SECONDARY, InputManager::CameraCarLookUp, GAMEPAD, DIJOFS_RZ, DIR_UP );
    SetMap( SLOT_SECONDARY, InputManager::CameraCarLookBack, GAMEPAD, DIJOFS_RZ, DIR_DOWN );
    

    // Initialize the game settings
    m_bMouseLook = true;
    m_bInvertMouseX = false;
    m_bInvertMouseY = false;
    m_bForceFeedback = false;
    m_bTutorialDisabled = false;
    m_fMouseSensitivityX = 0.35f; 
    m_fMouseSensitivityY = 0.5f;
    m_fWheelSensitivityX = 0.5f;
    m_fWheelSensitivityY = 0.5f;

    //Enable the tutorials if controls are reverted.
    TutorialManager* pTutorialManager = GetTutorialManager();
    if( pTutorialManager )
        pTutorialManager->EnableTutorialMode( m_bTutorialDisabled );
}

void UserController::LoadConfig( ConfigString& config )
{
    ClearMappings();

    char property[ ConfigString::MaxLength ];
    char value[ ConfigString::MaxLength ];

    int map;
    int virtualKey;
    int cont;
    int dxKey;
    int dir;

    // Load the mappings.
    while ( config.ReadProperty( property, value ) )
    {
        if( _stricmp( property, "buttonmap" ) == 0 )
        {
            int ret = sscanf( value, "%d, %d: ( %d %d %d )", &map, &virtualKey, &cont, &dxKey, &dir );

            if( ret == 5 &&
                map >= 0 && map < Input::MaxVirtualMappings &&
                virtualKey >= 0 && virtualKey < (int) mNumButtons && 
                VirtualInputs::GetType( virtualKey ) != MAP_FRONTEND &&
                cont >= 0 && cont < NUM_CONTROLLERTYPES &&
                m_pController[ cont ]->IsValidInput( dxKey ) &&
                dir >= 0 && dir < NUM_DIRTYPES
              )
            {
                SetMap( map, virtualKey, (eControllerType) cont, dxKey, (eDirectionType) dir );
            }
        }
        else if( _stricmp( property, "mouselook" ) == 0 )
        {
            if( strcmp( value, "yes" ) == 0 )
            {
                m_bMouseLook = true;
            }
            else if( strcmp( value, "no" ) == 0 )
            {
                m_bMouseLook = false;
            }
        }
        else if( _stricmp( property, "invertmousex" ) == 0 )
        {
            if( strcmp( value, "yes" ) == 0 )
            {
                m_bInvertMouseX = true;
            }
            else if( strcmp( value, "no" ) == 0 )
            {
                m_bInvertMouseX = false;
            }
        }
        else if( _stricmp( property, "invertmousey" ) == 0 )
        {
            if( strcmp( value, "yes" ) == 0 )
            {
                m_bInvertMouseY = true;
            }
            else if( strcmp( value, "no" ) == 0 )
            {
                m_bInvertMouseY = false;
            }
        }
        else if( _stricmp( property, "useforcefeedback" ) == 0 )
        {
            if( strcmp( value, "yes" ) == 0 )
            {
                m_bForceFeedback = true;
            }
            else if( strcmp( value, "no" ) == 0 )
            {
                m_bForceFeedback= false;
            }
        }
        else if( _stricmp( property, "disabletutorials" ) == 0 )
        {
            if( strcmp( value, "yes" ) == 0 )
            {
                m_bTutorialDisabled = true;
            }
            else if( strcmp( value, "no" ) == 0 )
            {
                m_bTutorialDisabled = false;
            }
        }
        else if( _stricmp( property, "mousesensitivityx" ) == 0 )
        {
            float val = (float) atof( value );
            if( val > 0 )
            {
                m_fMouseSensitivityX = val;
            }
        }
        else if( _stricmp( property, "mousesensitivityy" ) == 0 )
        {
            float val = (float) atof( value );
            if( val > 0 )
            {
                m_fMouseSensitivityY = val;
            }
        }
        else if( _stricmp( property, "wheelsensitivityx" ) == 0 )
        {
            float val = (float) atof( value );
            if( val > 0 )
            {
                m_fWheelSensitivityX = val;
            }
        }
        else if( _stricmp( property, "wheelsensitivityy" ) == 0 )
        {
            float val = (float) atof( value );
            if( val > 0 )
            {
                m_fWheelSensitivityY = val;
            }
        }
    }
}

void UserController::SaveConfig( ConfigString& config )
{
    char value[ ConfigString::MaxLength ];
    eControllerType cont;
    int dxKey;
    eDirectionType dir;

    // For each of our mappings...
    for( int map = 0; map < Input::MaxVirtualMappings; map++ )
    {
        // For each virtual button...
        for( int vk = 0; vk < (int) mNumButtons; vk++ )
        {
            // Save any mapped key.
            if( VirtualInputs::GetType( vk ) != MAP_FRONTEND &&
                GetMap( map, vk, cont, dxKey, dir ) )
            {
                sprintf( value, "%d, %d: ( %d %d %d )", map, vk, cont, dxKey, dir );
                config.WriteProperty( "buttonmap", value );
            }
        }
    }

    // Save the other controller settings
    config.WriteProperty( "mouselook", m_bMouseLook ? "yes" : "no" );
    config.WriteProperty( "invertmousex", m_bInvertMouseX ? "yes" : "no" );
    config.WriteProperty( "invertmousey", m_bInvertMouseY ? "yes" : "no" );
    config.WriteProperty( "useforcefeedback", m_bForceFeedback ? "yes" : "no" );
    config.WriteProperty( "disabletutorials", m_bTutorialDisabled ? "yes" : "no" );

    sprintf( value, "%f", m_fMouseSensitivityX );
    config.WriteProperty( "mousesensitivityx", value );

    sprintf( value, "%f", m_fMouseSensitivityY );
    config.WriteProperty( "mousesensitivityy", value );

    sprintf( value, "%f", m_fWheelSensitivityX );
    config.WriteProperty( "wheelsensitivityx", value );

    sprintf( value, "%f", m_fWheelSensitivityY );
    config.WriteProperty( "wheelsensitivityy", value );

}

void UserController::RemapButton( int map, int VirtualButton, ButtonMappedCallback* callback )
{
    rAssert( map >= 0 && map < Input::MaxVirtualMappings );
    rAssert( VirtualButton >= 0 && VirtualButton < mNumButtons );
    rAssert( VirtualInputs::GetType( VirtualButton ) != MAP_FRONTEND );

    // Save the data.  Button gets mapped asynchronously in OnControllerInputPointChange()
    mMapData.MapNext = true;
    mMapData.map = map;
    mMapData.virtualButton = VirtualButton;
    mMapData.callback = callback;
}

void UserController::SetMap( int map, int virtualKey, eControllerType cont, int dxKey, eDirectionType dir )
{
    // Check over the inputs.
    rAssert( map >= 0 && map < Input::MaxVirtualMappings );
    rAssert( virtualKey >= 0 && virtualKey < mNumButtons );
    rAssert( m_pController[ cont ]->IsValidInput( dxKey ) );
    rAssert( VirtualInputs::GetType( virtualKey ) != MAP_FRONTEND );

    // Create the compressed key code
    InputCode icode( cont, dxKey, dir );

    // Clear the previous key that was used for this virtual button.
    eControllerType oldcont;
    int olddxKey;
    eDirectionType olddir;
    if( GetMap( map, virtualKey, oldcont, olddxKey, olddir ) )
    {
        m_pController[ oldcont ]->ClearMap( olddxKey, olddir, virtualKey );
    }

    // Clear any previous virtual button mapped to this controller/key/direction
    eMapType maptype = VirtualInputs::GetType( virtualKey );
    int old_vb = m_pController[ cont ]->GetMap( dxKey, dir, maptype );
    if( old_vb != Input::INVALID_CONTROLLERID )
    {
        for( int i = 0; i < Input::MaxVirtualMappings; i++ )
        {
            if( mVirtualMap[ i ].GetLogicalIndex( old_vb ) == icode.GetInputCode() )
            {
                mVirtualMap[ i ].SetAssociation( old_vb, Input::INVALID_CONTROLLERID );
            }
        }
    }


    // Save the new mapping
    mVirtualMap[ map ].SetAssociation( virtualKey, icode.GetInputCode() );

    // Propagate the mapping to the controller
    m_pController[ cont ]->SetMap( dxKey, dir, virtualKey );
}

bool UserController::GetMap( int map, int virtualKey, eControllerType& type, int& dxKey, eDirectionType& dir ) const
{
    // Check over the inputs.
    rAssert( map >= 0 && map < Input::MaxVirtualMappings );
    rAssert( virtualKey >= 0 && virtualKey < mNumButtons );

    // Load the mapping
    int code = mVirtualMap[ map ].GetLogicalIndex( virtualKey );

    if( code != Input::INVALID_CONTROLLERID )
    {
        InputCode icode = code;
        type = icode.GetController();
        dxKey = icode.GetDxKeyCode();
        dir = icode.GetDirection();
    }

    return code != Input::INVALID_CONTROLLERID;
}

const char* UserController::GetMap( int map, int virtualKey, int& numDirs, eControllerType& cont, eDirectionType& dir ) const
{
    int dxKey;

    // Retrieve the mapping for the key.
    // If the controller is not plugged in, return null.
    if( !GetMap( map, virtualKey, cont, dxKey, dir ) ||
        m_pController[ cont ]->getController() == NULL )
    {
        return NULL;
    }

    // figure out how many directions there are for this input
    if( m_pController[ cont ]->IsInputAxis( dxKey ) || m_pController[ cont ]->IsMouseAxis( dxKey ) )
    {
        numDirs = 2;
    }
    else if( m_pController[ cont ]->IsPovHat( dxKey ) )
    {
        numDirs = 4;
    }
    else
    {
        numDirs = 1;
    }

    // return the name of the input point
    return m_pController[ cont ]->GetInputName( dxKey );
}

void UserController::Remap( eControllerType cont, int dxKey, float* dvalues, int ndirections )
{
    if( cont == KEYBOARD &&
        m_pController[ cont ]->GetMap( dxKey, DIR_UP, MAP_FRONTEND ) == InputManager::feBack )
    {
        // If the user pressed back on the keyboard, cancel the mapping.        
        mMapData.MapNext = false;
        mMapData.callback->OnButtonMapped( NULL, cont, 1, NUM_DIRTYPES );
    }
    else if( cont == MOUSE && ( dxKey == DIMOFS_X || dxKey == DIMOFS_Y ) )
    {
        // Not allowed to map these.
    }
    else
    {
        // Find out which direction the user is pressing in. 
        for( int dir = 0; dir < ndirections; dir++ )
        {
            if( dvalues[ dir ] >= MAPPING_DEADZONE )
            {
                SetMap( mMapData.map, mMapData.virtualButton, cont, dxKey, (eDirectionType) dir );

                // Call back the mapper.
                mMapData.MapNext = false;
                mMapData.callback->OnButtonMapped( m_pController[ cont ]->GetInputName( dxKey ), cont, ndirections, (eDirectionType) dir );

                break;
            }
        }
    }
}

void UserController::ClearMappings()
{
    for( int i = 0; i < Input::MaxVirtualMappings; i++ )
    {
        mVirtualMap[ i ].ClearAssociations();
    }

    for( int j = 0; j < NUM_CONTROLLERTYPES; j++ )
    {
        m_pController[ j ]->ClearMappedButtons();
    }

    // Make sure these are always loaded.
    LoadFEMappings();
}

void UserController::LoadFEMappings()
{
    switch ( m_controllerId )
    {
    case 0:
        {
            m_pController[KEYBOARD]->SetMap( DIK_ESCAPE, DIR_UP, InputManager::feBack );
            m_pController[KEYBOARD]->SetMap( DIK_UP, DIR_UP, InputManager::feMoveUp );
            m_pController[KEYBOARD]->SetMap( DIK_DOWN, DIR_UP, InputManager::feMoveDown );
            m_pController[KEYBOARD]->SetMap( DIK_LEFT, DIR_UP, InputManager::feMoveLeft );
            m_pController[KEYBOARD]->SetMap( DIK_RIGHT, DIR_UP, InputManager::feMoveRight );
            m_pController[KEYBOARD]->SetMap( DIK_RETURN, DIR_UP, InputManager::feSelect );
            m_pController[KEYBOARD]->SetMap( DIK_NUMPAD8, DIR_UP, InputManager::feMoveUp );
            m_pController[KEYBOARD]->SetMap( DIK_NUMPAD2, DIR_UP, InputManager::feMoveDown );
            m_pController[KEYBOARD]->SetMap( DIK_NUMPAD4, DIR_UP, InputManager::feMoveLeft );
            m_pController[KEYBOARD]->SetMap( DIK_NUMPAD6, DIR_UP, InputManager::feMoveRight );
            m_pController[KEYBOARD]->SetMap( DIK_NUMPADENTER, DIR_UP, InputManager::feSelect );
            m_pController[KEYBOARD]->SetMap( DIK_F1, DIR_UP, InputManager::feFunction1 );
            m_pController[KEYBOARD]->SetMap( DIK_F2, DIR_UP, InputManager::feFunction2 );
            break;
        }
    case 3:
        {
            //P4
            m_pController[KEYBOARD]->SetMap( DIK_F4, DIR_UP, InputManager::P1_KBD_Start );
            m_pController[KEYBOARD]->SetMap( DIK_O, DIR_UP, InputManager::P1_KBD_Gas );
            m_pController[KEYBOARD]->SetMap( DIK_L, DIR_UP, InputManager::P1_KBD_Brake );
            m_pController[KEYBOARD]->SetMap( DIK_RETURN, DIR_UP, InputManager::P1_KBD_EBrake );
            m_pController[KEYBOARD]->SetMap( DIK_RSHIFT, DIR_UP, InputManager::P1_KBD_Nitro );
            m_pController[KEYBOARD]->SetMap( DIK_K, DIR_UP, InputManager::P1_KBD_Left );
            m_pController[KEYBOARD]->SetMap( DIK_SEMICOLON, DIR_UP, InputManager::P1_KBD_Right );
            break;
        }
    }

    m_pController[MOUSE]->SetMap( DIMOFS_BUTTON1, DIR_UP, InputManager::feBack );
    m_pController[MOUSE]->SetMap( DIMOFS_X, DIR_UP, InputManager::feMouseRight );
    m_pController[MOUSE]->SetMap( DIMOFS_X, DIR_DOWN, InputManager::feMouseLeft );
    m_pController[MOUSE]->SetMap( DIMOFS_Y, DIR_UP, InputManager::feMouseUp );
    m_pController[MOUSE]->SetMap( DIMOFS_Y, DIR_DOWN, InputManager::feMouseDown );
    m_pController[MOUSE]->SetMap( DIMOFS_Z, DIR_UP, InputManager::feMoveRight );
    m_pController[MOUSE]->SetMap( DIMOFS_Z, DIR_DOWN, InputManager::feMoveLeft );

    m_pController[GAMEPAD]->SetMap( DIJOFS_Y, DIR_UP, InputManager::feMoveUp );
    m_pController[GAMEPAD]->SetMap( DIJOFS_Y, DIR_DOWN, InputManager::feMoveDown );
    m_pController[GAMEPAD]->SetMap( DIJOFS_X, DIR_UP, InputManager::feMoveRight );
    m_pController[GAMEPAD]->SetMap( DIJOFS_X, DIR_DOWN, InputManager::feMoveLeft );
    m_pController[GAMEPAD]->SetMap( DIJOFS_BUTTON0, DIR_UP, InputManager::feSelect );
    m_pController[GAMEPAD]->SetMap( DIJOFS_BUTTON1, DIR_UP, InputManager::feBack );
    m_pController[GAMEPAD]->SetMap( DIJOFS_POV(0), DIR_UP, InputManager::feMoveUp );
    m_pController[GAMEPAD]->SetMap( DIJOFS_POV(0), DIR_DOWN, InputManager::feMoveDown );
    m_pController[GAMEPAD]->SetMap( DIJOFS_POV(0), DIR_RIGHT, InputManager::feMoveRight );
    m_pController[GAMEPAD]->SetMap( DIJOFS_POV(0), DIR_LEFT, InputManager::feMoveLeft );
}

void UserController::ResetMouseAxes()
{
    for( int i = 0; i < 3; i++ )
    {
        if( mResetMouseCounter[ i ] > 1 )
        {
            // Reset this mouse axis.
            InputCode icode( MOUSE, maxes[ i ], NUM_DIRTYPES );
            OnControllerInputPointChange( icode.GetInputCode(), 0.0f );

            mResetMouseCounter[ i ] = 0;
        }
        else if( mResetMouseCounter[ i ] > 0 )
        {
            mResetMouseCounter[ i ]++;
        }
    }
}
