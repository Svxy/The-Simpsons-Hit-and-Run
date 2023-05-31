#ifndef AICHARACTERCONTROLLER_H_
#define AICHARACTERCONTROLLER_H_

#include <worldsim/character/charactercontroller.h>
#include <radmath/radmath.hpp>

class Behaviour;
class Character;
class tCamera;

class AICharacterController
:
public CharacterController
{
public:
    AICharacterController( Character* pCharacter, int index, tCamera* pCamera );
	void Update( float timeins );
	void GetDirection( rmt::Vector& outDirection ) const;
    float GetValue( int buttonId ) const;
    bool IsButtonDown( int buttonId ) const;
protected:
private:
	rmt::Vector mDirection;
	Behaviour* mpBehaviour;
};

#endif // AICHARACTERCONTROLLER_H_