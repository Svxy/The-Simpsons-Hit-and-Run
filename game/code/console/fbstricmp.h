//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   String Subsystem
//
// Description: Class definitions for a Unicode String UnicodeString
//
// Authors:     Ian Gipson
//
// Revisions		Date		Author	Revision
//					2000/10/23	IJG		Created
//
//===========================================================================

// Recompilation protection flag.
#ifndef __SMSTRICMP_H
#define __SMSTRICMP_H


//===========================================================================
// Nested Includes
//===========================================================================

//===========================================================================
// Forward References
//===========================================================================

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: performs a case independant string comparison on any platform
//
// Constraints: 
//
//===========================================================================
int smStricmp( const char *string1, const char *string2 );

//===========================================================================
//
// Description: performs a case independant string comparison on any platform
//				up to tCount characters
//
// Constraints: 
//
//===========================================================================
int smStrincmp( const char *string1, const char *string2, int tCount);


#endif                                  // End conditional inclusion
