#ifndef BIPEDCHARACTERMAPPABLE_H_
#define BIPEDCHARACTERMAPPABLE_H_

#include <input/mappable.h>
#include <radmath/radmath.hpp>


class Character;
class CharacterController;
class tCamera;

class CharacterMappable
:
public Mappable
{
public:

	CharacterMappable( void );
	virtual ~CharacterMappable( void );

    // This method is called when ever a button state changes.
    //
    virtual void OnButton( int controllerId, int id, const IButton* pButton );

    // This method is called when a button changes state from "Pressed" to "Released".
    //
	virtual void OnButtonUp( int controllerId, int buttonId, const IButton* pButton );

    // This method is called when a button changes state from "Released" to "Pressed".
    //
	virtual void OnButtonDown( int controllerId, int buttonId, const IButton* pButton );

    // This is how we create our controller device mappings to logical game mappings.
    // The mappings set up in this method are platform specific.
    //
    // The basic format of the calls is to "Map" a input, to a enumerated output id.
    // The output of the specified input will be contained in the Button[] array.
    // This id will also be sent as a the second parameter in the OnButton... messages.
    //
    virtual void LoadControllerMappings( unsigned int controllerId );

    // Return the controller input direction transformed from camera to world space.
    //
    virtual void GetDirection( rmt::Vector& outDirection ) const;
    
    CharacterController* GetCharacterController( void ) const;
    void SetCharacterController( CharacterController* pCharacterController );
private:
    CharacterController* mpCharacterController;
};

class BipedCharacterMappable
:
public CharacterMappable
{
public:

	BipedCharacterMappable( void );
	virtual ~BipedCharacterMappable( void );

    // This method is called when ever a button state changes.
    //
    //virtual void OnButton( int controllerId, int id, const IButton* pButton );

    // This method is called when a button changes state from "Pressed" to "Released".
    //
	//virtual void OnButtonUp( int controllerId, int buttonId, const IButton* pButton );

    // This method is called when a button changes state from "Released" to "Pressed".
    //
	virtual void OnButtonDown( int controllerId, int buttonId, const IButton* pButton );

    // This is how we create our controller device mappings to logical game mappings.
    // The mappings set up in this method are platform specific.
    //
    // The basic format of the calls is to "Map" a input, to a enumerated output id.
    // The output of the specified input will be contained in the Button[] array.
    // This id will also be sent as a the second parameter in the OnButton... messages.
    //
    //virtual void LoadControllerMappings( unsigned int controllerId );

    // Return the controller input direction transformed from camera to world space.
    //
    virtual void GetDirection( rmt::Vector& outDirection ) const;
    
private:
};


class InCarCharacterMappable
:
public CharacterMappable
{
public:

	InCarCharacterMappable( void );
	virtual ~InCarCharacterMappable( void );

    // This method is called when ever a button state changes.
    //
    //virtual void OnButton( int controllerId, int id, const IButton* pButton );

    // This method is called when a button changes state from "Pressed" to "Released".
    //
	//void OnButtonUp( int controllerId, int buttonId, const IButton* pButton );

    // This method is called when a button changes state from "Released" to "Pressed".
    //
	virtual void OnButtonDown( int controllerId, int buttonId, const IButton* pButton );

    // This is how we create our controller device mappings to logical game mappings.
    // The mappings set up in this method are platform specific.
    //
    // The basic format of the calls is to "Map" a input, to a enumerated output id.
    // The output of the specified input will be contained in the Button[] array.
    // This id will also be sent as a the second parameter in the OnButton... messages.
    //
    //virtual void LoadControllerMappings( unsigned int controllerId );

    // Return the controller input direction transformed from camera to world space.
    //
    virtual void GetDirection( rmt::Vector& outDirection ) const;
    
private:
};
#endif // BIPEDCHARACTERMAPPABLE_H_