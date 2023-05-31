#ifndef MAPPABLE_HPP_
#define MAPPABLE_HPP_

#include <input/controller.h>
#include <input/button.h>
#include <input/mapper.h>

#include <p3d/refcounted.hpp>

struct ControlMap;
class UserController;

class Mappable
:
public tRefCounted
{
public:
    Mappable( unsigned active = Input::ACTIVE_ALL & ~Input::ACTIVE_ANIM_CAM );

    virtual ~Mappable( void );

    //-------------------------------------------
    //Stuff that derived classes should override
    //-------------------------------------------
        
    // This method is called when ever a button is active (i.e. is non zero, or just went zero).
    virtual void OnButton( int controllerId, int id, const Button* pButton ) = 0;

    // This method is called when a button changes state from "Pressed" to "Released".
	virtual void OnButtonUp( int controllerId, int buttonId, const Button* pButton ) = 0;

    // This method is called when a button changes state from "Released" to "Pressed".
	virtual void OnButtonDown( int controllerId, int buttonId, const Button* pButton ) = 0;

    // This is how we create our controller device mappings to logical game mappings.
    // The mappings set up in this method are platform specific.
    //
    // The basic format of the calls is to "Map" a input, to a enumerated output id.
    // The output of the specified input will be contained in the Button[] array.
    // This id will also be sent as a the second parameter in the OnButton... messages.
    virtual void LoadControllerMappings( unsigned int controllerId ) = 0;

    // should be overriden by dewrived calasses to do something if the controller 
    // attached to this mappable unplugs.
    // there is a default implementation which does nothing.
    virtual void OnControllerDisconnect( int id );

    // should be overriden by dewrived calasses to do something if the controller 
    // attached to this mappable plugs in.
    // there is a default implementation which does nothing.
    virtual void OnControllerConnect( int id );

    // ---------------------
    // Regular functions 
    // ---------------------

    // Route the Button changes through the Mappers.
    void DispatchOnButton( int controllerId, int id, const Button* pButton );

    // load a complete physical button state
    // This will set the internal state, but not generate any events (OnButton*)
    void InitButtons( int controllerId, const Button* pButtons );

    // Is the button down?
    bool IsButtonDown( unsigned int id );

	// Is the button up?
    bool IsButtonUp( unsigned int id );

	// Update the internal variables that track the button state.
	// Always tracks the button state, regardless of game state.
	// OnButton will only be called in an active state.
	// Values updated BEFORE OnButton is called.
	void UpdateButtonState( int controllerId, int id, const Button* pButton );

    // set the current game state
    void SetGameState(unsigned state);
    
    // Returns true if the mappable is active in the current game state.
    bool IsActive( void ) const;

    // Returns the value of the logical output referenced by 'index'.
    // Hint: make an enum with your own Logical Names.
    float GetValue( unsigned int index ) const;

    // Returns a pointer to the Button at index.
    Button* GetButton( unsigned int index );

    // To set up the different controller configurations.
    void SetControlMap( unsigned map );

    // Returns the current control map enumeration value.
    //
    unsigned GetControlMap( void ) const;
    
    // Reset internal state (all vutton values go to 0)
    void Reset( void );

    bool GetResync(void) { return mNeedResync;}
    void SetResync(bool b) { mNeedResync = b;}
    
    // Returns a Mapper by index.
    //
    Mapper* GetMapper( unsigned int index );

protected:
    // set up controller mappings. return false if cannot find name.
    //
    bool Map( const char* pszName, int destination, unsigned int mapperIndex, unsigned int controllerId );

    // Remove all associations.
    //
    void ClearMap( unsigned int mapperIndex );

private:

    // Returns the Active mapper.
    //
    Mapper* GetActiveMapper( void );
    // A 32 Bit button mask.
	//
	unsigned int mButtonMask;

	// State for all the (logical) buttons.
    Button mButton[ Input::MaxLogicalButtons ];

	// The button map.
    Mapper mMapper[ Input::MaxMappings];

    // The index of the currently active mapper.
    unsigned mActiveMapper;

    unsigned mActiveState;
    bool mActive;
    bool mNeedResync;
};

#endif