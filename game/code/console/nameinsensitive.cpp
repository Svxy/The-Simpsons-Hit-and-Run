//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   string subsystem
//
// Description: This file contains the implementation of the class UnicodeString
//
// Authors:     Ian Gipson
//
// Revisions		Date		Author	Revision
//					2000/10/23	IJG		Created
//
//===========================================================================

//---------------------------------------------------------------------------
// Includes
//===========================================================================
#include "console/nameinsensitive.h"
#include "console/upcase.h"

//===========================================================================
// Local Constants, Typedefs, and Macros
//===========================================================================

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Member Functions
//===========================================================================

//===========================================================================
// tNameInsensitive::tNameInsensitive
//===========================================================================
// Description: default constructor
//
// Constraints:	none
//
// Parameters:	none
//
// Return:      none
//
//===========================================================================
tNameInsensitive::tNameInsensitive():
    Parent()
{
    //nothing
}

//===========================================================================
// tNameInsensitive::tNameInsensitive
//===========================================================================
// Description: constructor from a const char*
//
// Constraints:	none
//
// Parameters:	name - const char* to the string to convert
//
// Return:      none
//
//===========================================================================
tNameInsensitive::tNameInsensitive( const char* name ):
    Parent()
{
    SetText( name );
}

//===========================================================================
// tNameInsensitive::tNameInsensitive
//===========================================================================
// Description: Copy constructor
//
// Constraints:	none
//
// Parameters:	name - tNameInsensitive to copy in
//
// Return:      none
//
//===========================================================================
tNameInsensitive::tNameInsensitive( const tNameInsensitive& name ):
    Parent( name )
{
    //nothing
}

//===========================================================================
// tNameInsensitive::tNameInsensitive
//===========================================================================
// Description: assignment from another name
//
// Constraints:	none
//
// Parameters:	other - char* to assign
//
// Return:      none
//
//===========================================================================
const tNameInsensitive& tNameInsensitive::operator=( const char* other )
{
    SetText( other );
    return *this;
}

//===========================================================================
// tNameInsensitive::tNameInsensitive
//===========================================================================
// Description: assignment from another name
//
// Constraints:	none
//
// Parameters:	other - tNameInsensitive to assign
//
// Return:      none
//
//===========================================================================
const tNameInsensitive& tNameInsensitive::operator=(const tNameInsensitive& other)
{
    Parent::operator=( other );
    return *this;
}

//===========================================================================
// tNameInsensitive::SetText
//===========================================================================
// Description: sets the text and the UID from a string
//
// Constraints:	none
//
// Parameters:	name - const char* to set this name to
//
// Return:      none
//
//===========================================================================
void tNameInsensitive::SetText( const char* name )
{
    char temp[ 1024 ];
    #ifdef RAD_DEBUG
        size_t length = strlen( name );
        rAssert( length < 1024 );
    #endif
    strcpy( temp, name );
    Upcase( temp );
    Parent::SetText( name );
}
