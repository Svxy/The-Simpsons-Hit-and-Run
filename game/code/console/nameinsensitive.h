//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   tNames with case insensitivity
//
// Description: hashed names that are case insensitive
//
// Authors:     Ian Gipson
//
// Revisions		Date		Author	Revision
//					2003/06/02	IJG		Created
//
//===========================================================================

// Recompilation protection flag.
#ifndef __NAMEINSENSITIVE_H_
#define __NAMEINSENSITIVE_H_


//===========================================================================
// Nested Includes
//===========================================================================
#include <p3d/entity.hpp>

//===========================================================================
// Forward References
//===========================================================================

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================

class tNameInsensitive : public tName
{
private:
    typedef tName Parent;
public:
    // various constructors
    tNameInsensitive();                   // default constructor, both uid and text will be invalid
    tNameInsensitive( const char* name ); // sets text name and uid
    tNameInsensitive( const tNameInsensitive& );     // copy from another 

    const tNameInsensitive& operator=( const tNameInsensitive& other );
    const tNameInsensitive& operator=( const char*             other );
    void  SetText(const char* name);

private:
    const tNameInsensitive& operator=(const tName& other);
    tNameInsensitive( const tName& );     // copy from another name - DO NOT PERMIT
    void SetTextOnly(const char*); //set the text without changing the UID
    // the real data
};
#endif     //__NAMEINSENSITIVE_H_
