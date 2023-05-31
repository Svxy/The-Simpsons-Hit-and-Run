//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   ConfigString
//
// Description: Game configuration string which builds the concepts of
//              properties and sections around strings.  Really simple to
//              get the job done.
//
// Authors:     Ziemek Trzesicki
//
// Revisions		Date			Author	    Revision
//                  2003/05/02      ziemek      Created for SRR2
//
//===========================================================================

#ifndef CONFIGSTRING_H
#define CONFIGSTRING_H

//===========================================================================
// Interface Definitions
//===========================================================================

class ConfigString
{
public:
    // The character used to tag sections.
    // Cannot be used in names.
    static const char SectionTag = '#';

    // The character used to define properties.
    // Cannot be used in property keys.
    static const char PropertyTag = '=';

    // The maximum length of a section/property name or property value.
    static const int MaxLength = 80;

    // Defines the two modes that config strings can be in.
    enum ConfigStringMode
    {
        Read,
        Write
    };

public:
    // Creates a config string.
    // For reading, size is the size of the string buffer.
    // For writing, size is the max size of the buffer.
    ConfigString( ConfigStringMode mode, int size );
    ~ConfigString();

    // Returns the mode - read / write
    ConfigStringMode GetMode() const { return mMode; }
    // Returns the string buffer for reading/writing to a file.
    char* GetBuffer() { return mBuffer; }
    // Returns the size of hte string buffer.
    int GetSize() const { return mSize; }
        
    // Reads the next section title, returning true if one is found.
    bool ReadSection( char* section );
    // Reads the next property.  these are bracketed by sections.
    bool ReadProperty( char* property, char* value );

    // Writes a section.  These bracket properties.
    void WriteSection( const char* section );
    // Writes a property.
    void WriteProperty( const char* property, const char* value );

private:
    // Reads the next non-empty line starting from the cursor.
    // Saves a max number of <MaxLength> characters to buffer, to avoid overflows.
    // The cursor is advanced to the end of the line regardless.
    // Returns true if a line is read, false if there are no more.
    bool GetLine( char* buffer );

private:
    ConfigStringMode mMode;
    char*            mBuffer;
    char*            mCursor;

    int              mSize;
};

#endif // CONFIGSTRING_H
