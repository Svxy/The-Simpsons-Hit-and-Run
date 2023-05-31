//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        globaltypes.h
//
// Description: Declarations of utility types.
//
// History:     2/18/2003 + Created -- Esan
//
//=============================================================================

#ifndef GLOBALTYPES_H
#define GLOBALTYPES_H

//
// define 64 bit integer for all platforms
//
#ifdef RAD_PS2
typedef unsigned long simpsonsUInt64;
#endif

#ifdef RAD_GAMECUBE
typedef unsigned long long simpsonsUInt64;
#endif

#if defined RAD_XBOX || defined RAD_WIN32
typedef unsigned __int64 simpsonsUInt64;
#endif

#endif //GLOBALTYPES_H
