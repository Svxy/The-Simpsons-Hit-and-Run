//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   ConfigString
//
// Description: Implementation of the ConfigString class.
//
// Authors:     Ziemek Trzesicki
//
// Revisions		Date			Author	    Revision
//                  2003/05/05      ziemek      Created for SRR2
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================

#include <data/config/configstring.h>

//==============================================================================
// ConfigString constructor
//==============================================================================

ConfigString::ConfigString( ConfigStringMode mode, int size )
{
    rAssert( size > 0 );

    mMode = mode;
    mBuffer = new char[size + 1];
    mCursor = mBuffer;
    mSize = size;

    // init the buffer
    mBuffer[0] = mBuffer[size] = '\0';
}

//==============================================================================
// ConfigString destructor
//==============================================================================

ConfigString::~ConfigString()
{
    delete [] mBuffer;
}

//==============================================================================
// ConfigString::ReadSection
//==============================================================================

bool ConfigString::ReadSection( char* section )
{
    rAssert( mMode == Read );

    char line[MaxLength];

    if( GetLine(line) )
    {
        if( line[0] == SectionTag )
        {
            strcpy( section, line + 1 );
            return true;
        }
        else
        {
            // Try reading a section from the next line.
            ReadSection( section );
            return false;
        }
    }
    else
    {
        return false;
    }
}

//==============================================================================
// ConfigString::ReadProperty
//==============================================================================

bool ConfigString::ReadProperty( char* property, char* value )
{
    rAssert( mMode == Read );

    char line[MaxLength];
    char* oldCursor = mCursor;

    if( GetLine(line) )
    {
        if( line[0] == SectionTag )
        {
            // rewind - it's a new section.
            mCursor = oldCursor;
            return false;
        }
        else
        {
            // search for a property define.
            char* prop_def = strchr( line, PropertyTag );
            if( prop_def != NULL )
            {
                *prop_def = '\0';
                strcpy( property, line );
                strcpy( value, prop_def+1 );

                return true;
            }
            else
            {
                // Skip the invalid line.
                // Call read property recursively to read the next line.
                return ReadProperty( property, value );
            }
        }
    }
    else
    {
        return false;
    }
}

//==============================================================================
// ConfigString::WriteSection
//==============================================================================

void ConfigString::WriteSection( const char* section )
{
    rAssert( mMode == Write );

    // Make sure it's not too long.
    size_t len = strlen( section );
    rAssert( len + 1 < MaxLength );

    // Make sure we have room in the string buffer (extra chars for # and \n and \0).
    bool HaveRoom = ( mCursor - mBuffer ) + len + 6 < (size_t) mSize;
    rAssert( HaveRoom );

    // Rough guide of whether this is the first section
    bool FirstSection = mCursor == mBuffer;

    if( HaveRoom )
    {
        if( !FirstSection )
        {
            *mCursor = '\r';
            mCursor++;

            *mCursor = '\n';
            mCursor++;
        }

        *mCursor = SectionTag;
        mCursor++;

        strcpy( mCursor, section );
        mCursor += len;

        *mCursor = '\r';
        mCursor++;

        *mCursor = '\n';
        mCursor++;

        *mCursor = '\0';
    }
}

//==============================================================================
// ConfigString::WriteProperty
//==============================================================================

void ConfigString::WriteProperty( const char* property, const char* value )
{
    rAssert( mMode == Write );

    size_t lenp = strlen( property );
    size_t lenv = strlen( value );

    // Make sure it's not too long
    rAssert( lenp + lenv + 1 < MaxLength );

    // Make sure we have room in the string buffer (extra 3 for = and \n and \0).
    bool HaveRoom = ( mCursor - mBuffer ) + lenp + lenv + 4 < (size_t) mSize;
    rAssert( HaveRoom );

    if( HaveRoom )
    {
        strcpy( mCursor, property );
        mCursor += lenp;

        *mCursor = PropertyTag;
        mCursor++;

        strcpy( mCursor, value );
        mCursor += lenv;

        *mCursor = '\r';
        mCursor++;

        *mCursor = '\n';
        mCursor++;

        *mCursor = '\0';
    }
}

//==============================================================================
// ConfigString::GetLine
//==============================================================================

bool ConfigString::GetLine( char* buffer )
{
    // skip white space
    while( *mCursor == ' ' || *mCursor == '\t' || *mCursor == '\n' || *mCursor == '\r' )
    {
        mCursor++;
    }

    // Copy the line
    int i = 0;
    while( *mCursor != '\0' && *mCursor != '\n' && *mCursor != '\r' )
    {
        if( i < MaxLength )
        {
            buffer[i++] = *mCursor;
        }

        mCursor++;
    }

    // Trim trailing white space
    while( i > 0 && ( buffer[i-1] == ' ' || buffer[i-1] == '\t' ) )
    {
        i--;
    }

    buffer[i] = '\0';

    return i > 0;
}