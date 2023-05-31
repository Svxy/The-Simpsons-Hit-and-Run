//-----------------------------------------------------------------------------
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// winutil.h
//
// Description: A set of utilities which are specific to the windows development
//              environment.
//
// Modification History:
//  + Created Aug 03, 2001 -- bkusy 
//-----------------------------------------------------------------------------

//----------------------------------------
// System Includes
//----------------------------------------
#include <windows.h>
#include <stdio.h>
#include <assert.h>

//----------------------------------------
// Project Includes
//----------------------------------------
#include "winutil.h"

//----------------------------------------
// Constants
//----------------------------------------
#define BUFFER_SIZE 256


//-----------------------------------------------------------------------------
// w i n u t i l _ V e r s i o n I n f o 
//
// Synopsis:    Retrieve a version info string for the specified module.
//
// Parameters:  name      - the module to retrieve the information for.
//              info      - a reference parameter to receive the info string.
//              info_size - the size of the buffer pointed to by info.
//
// Returns:     the length of the string returned in info.
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
int winutil_VersionInfo( const char* name, char* info, int info_size )
{
    char buffer[ BUFFER_SIZE + 1 ];
    char* data = 0;
    DWORD data_length, unused;
    VS_FIXEDFILEINFO* fixed_file_info;
    UINT fixed_file_info_length;
    int major, minor;
    int result;

    GetModuleFileName( GetModuleHandle( name ), buffer, BUFFER_SIZE );
    data_length = GetFileVersionInfoSize( buffer, &unused );
    data = calloc( data_length, sizeof( char ) );
    assert( data );
    GetFileVersionInfo( buffer, unused, data_length, data );
    VerQueryValue( data, "\\", (void**)(&fixed_file_info), &fixed_file_info_length );

    major = fixed_file_info->dwFileVersionMS >> 16;
    minor = fixed_file_info->dwFileVersionMS & 0x0ffff;
    result = _snprintf( info, info_size, "%d.%d", major, minor );
    return result;
}
    
    

