#include <input/mapper.h>
#include <input/mappable.h>

Mapper::Mapper( void )
{
    ClearAssociations( );
}

int Mapper::SetAssociation( int sourceButtonID, int destButtonID )
{
    buttonMap[sourceButtonID] = destButtonID;
	return 0;
}

int Mapper::GetLogicalIndex( int sourceButtonID ) const
{
    return buttonMap[sourceButtonID]; 
}

int Mapper::GetPhysicalIndex( int destButtonID ) const
{
    for ( unsigned i = 0; i < Input::MaxLogicalButtons; i++ )
	{
		if ( buttonMap[i] == destButtonID)
		{
    	    return i;
        }
    }
    return Input::INVALID_CONTROLLERID;
}


void Mapper::ClearAssociations( void )
{
    for( unsigned int i = 0; i < Input::MaxLogicalButtons; i++)
    {
        buttonMap[i] = Input::INVALID_CONTROLLERID; 
    }
}
