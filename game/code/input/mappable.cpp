#include <input/mappable.h>

#ifdef RAD_WIN32
#include <input/usercontrollerWin32.h>
#else
#include <input/usercontroller.h>
#endif

#include <input/inputmanager.h>

#include <raddebug.hpp>

Mappable::Mappable( unsigned active)
    :
mButtonMask( 0 ),
mActiveMapper( 0 ),
mActiveState(active),
mActive(true),
mNeedResync(false)
{
}

Mappable::~Mappable( void )
{
}

// Route the Button changes through the Active Mapper.
//
void Mappable::DispatchOnButton( int controllerId, int id, const Button* pButton )
{
    if(!mActive)
    {
        return;
    }

	// perform a lookup on this physical id and set state of the appropriate logical button
    int destButtonID = GetActiveMapper( )->GetLogicalIndex(id);

    if ( destButtonID != Input::INVALID_CONTROLLERID && IsActive())
	{
        // Grab the state of the button before we update the state.
        bool bWasButtonDown = IsButtonDown( destButtonID );
        bool duplicate = mButton[ destButtonID ].TimeSinceChange() == 0;

        if(!duplicate || (duplicate && ( pButton->GetValue( ) >= mButton[ destButtonID].GetValue( ) )))
        {
		    UpdateButtonState( controllerId, destButtonID, pButton );

		    OnButton( controllerId, destButtonID, pButton );
		    
            if ( 0.0f == pButton->GetValue( ) )
		    {
                if ( bWasButtonDown )
                {
				    OnButtonUp( controllerId, destButtonID, pButton );
                }
		    }
		    else
		    {
                if ( !bWasButtonDown )
                {
				    OnButtonDown( controllerId, destButtonID, pButton );
			    }
            }
        }
	}
}

// load in a complete button state
// we only update the state, no edge-trigerred stuff (OnButton*)
void Mappable::InitButtons( int controllerId, const Button* pButtons )
{
    for(unsigned int i = 0; i < Input::MaxPhysicalButtons; i++)
    {
        int destButtonID = GetActiveMapper( )->GetLogicalIndex(i);

        if ( destButtonID != Input::INVALID_CONTROLLERID )
	    {
		    UpdateButtonState( controllerId, destButtonID, &pButtons[i] );
        }
    }

}

void Mappable::OnControllerDisconnect( int id )
{
    this->Reset();
}

void Mappable::OnControllerConnect( int id )
{
}

// Update the internal variables that track the button state.
// Always tracks the button state, regardless of game state.
// OnButton will only be called in an active state.
// Values updated BEFORE OnButton is called.
//
void Mappable::UpdateButtonState( int controllerId, int buttonId, const Button* pButton )
{
    if(!mActive)
    {
        return;
    }

    unsigned int newButtonMask = 0;
    unsigned int bit = 1 << buttonId;

    mButton[ buttonId ] = *pButton;
    mButton[ buttonId ].ForceChange();

	if ( 0.0f != mButton[ buttonId ].GetValue( ) )
	{
		newButtonMask = bit | mButtonMask;
	}
	else
	{
        bit = ~bit;
        newButtonMask = bit & mButtonMask;
	}
    mButtonMask = newButtonMask;
}

// Returns the value of the logical output referenced by 'index'.
//
float Mappable::GetValue( unsigned int index ) const
{
    rAssert( index < Input::MaxLogicalButtons );
    if ( index < Input::MaxLogicalButtons )
    {
        return mButton[ index ].GetValue( );
    }

    return 0.0f;
}


// Returns a const pointer to the Button at index.
//
Button* Mappable::GetButton( unsigned int index )
{
    return &mButton [ index ];
}


// To set up the different controller configurations.
//
void Mappable::SetControlMap( unsigned map )
{
    mActiveMapper = map;
}

// Returns the current control map enumeration value.
//
unsigned Mappable::GetControlMap( void ) const
{
    return mActiveMapper;
}

// Returns a Mapper by index.
//
Mapper* Mappable::GetMapper( unsigned int index )
{
    return &mMapper[ index ];
}

// Returns the Active mapper.
//
Mapper* Mappable::GetActiveMapper( void )
{
    return &mMapper[ mActiveMapper ];
}

void Mappable::SetGameState(unsigned state)
{
    if(state & mActiveState)
    {
        mNeedResync = !mActive;
        mActive = true;
    }
    else
    {
        mNeedResync = false;
        mActive = false;
        Reset();
    }
}

// Is this mappable object active in this game state.
//
bool Mappable::IsActive( void ) const
{
    return mActive;
}

// Is the button down?
//
bool Mappable::IsButtonDown( unsigned int id )
{
    unsigned int bit = 1 << id;
    return ( mButtonMask & bit ) != 0;
}

// Is the button up?
//
bool Mappable::IsButtonUp( unsigned int id )
{
    return !IsButtonDown( id );
}

// set up controller mappings. return false if cannot find name.
//
bool Mappable::Map( const char* pszName, int destination, unsigned int mapperIndex, unsigned int controllerId )
{
    Mapper* pMapper = this->GetMapper( mapperIndex );
    //rValid( pMapper );

    InputManager* pInputManager = InputManager::GetInstance( );
    rAssert( pInputManager );

    const UserController* pUserController = pInputManager->GetController( controllerId );
    //rValid( pUserController );

    int source = pUserController->GetIdByName( pszName );
    if ( source >=0 )
    {
        pMapper->SetAssociation( source, destination );
        return true;
    }
    return false;
}

// Clear all associations.
//
void Mappable::ClearMap( unsigned int mapperIndex )
{
    Mapper* pMapper = GetMapper( mapperIndex );
    //rValid( pMapper );

    pMapper->ClearAssociations( );
}

void Mappable::Reset( void )
{
    for( unsigned int i = 0; i < Input::MaxLogicalButtons; i++)
    {
        mButton[i].SetValue(0.0f);
    }

    mButtonMask = 0;
}