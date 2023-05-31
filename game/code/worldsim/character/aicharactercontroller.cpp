#include <worldsim/character/aicharactercontroller.h>
#include <worldsim/character/character.h>
#include <worldsim/character/charactermanager.h>
#include <main/game.h>

class Behaviour
{
public:
	virtual rmt::Vector& Tick( Character& me ) = 0;
};

class Wander
:
public Behaviour
{
public:

	Wander( void );
	virtual ~Wander( void );

	rmt::Vector& GetTargetPoint( void );
	void SetTargetPoint( rmt::Vector& targetPoint );

	rmt::Vector& GetPosition( void );
	void SetPosition( rmt::Vector& position );

	float GetTargetRadius( void ) const;
	void SetTargetRadius( float radius );

	float GetSteeringRadius( void ) const;
	void SetSteeringRadius( float radius );

	rmt::Vector& Tick( Character& me );
private:
	rmt::Vector mTargetPoint;
	rmt::Vector mPosition;
	float mfTargetCircleRadius;
	float mfSteeringCircleRadius;
};

Wander::Wander( void )
:
mTargetPoint( 0.0f, 0.0f, -1.0f ),
mPosition( 0.0f, 0.0f, -1.0f ),
mfTargetCircleRadius( 1.0f ),
mfSteeringCircleRadius( 0.3f )
{
	mTargetPoint.z = mfTargetCircleRadius;
}

Wander::~Wander( void )
{
}

rmt::Vector& Wander::Tick( Character& me )
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

	return mTargetPoint;
}

AICharacterController::AICharacterController( Character* pCharacter, int index, tCamera* pCamera )
:
mpBehaviour( 0 )
{
}

void AICharacterController::Update( float timeins )
{
    static Character* pCharacter = GetCharacterManager( )->GetCharacter( 0 );
	if ( !mpBehaviour )
	{
		mpBehaviour = new Wander;
	}

	mDirection = mpBehaviour->Tick( *pCharacter );
}

void AICharacterController::GetDirection( rmt::Vector& outDirection ) const
{
    outDirection = mDirection;
}

float AICharacterController::GetValue( int buttonId ) const
{
    return 0.0f;
}

bool AICharacterController::IsButtonDown( int buttonId ) const
{
    return false;
}