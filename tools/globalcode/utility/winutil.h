#ifndef _WINUTIL_H
#define _WINUTIL_H
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

//----------------------------------------
// Project Includes
//----------------------------------------

//----------------------------------------
// Forward References
//----------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

int winutil_VersionInfo( const char* name, char* info, int info_size );

#ifdef __cplusplus
}
#endif

#endif
