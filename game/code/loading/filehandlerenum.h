//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        filehandlerenum.h
//
// Description: File handler types.
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

#ifndef FILEHANDLERENUM_H
#define FILEHANDLERENUM_H

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================

//========================================
// Forward References
//========================================

//========================================
// Constants, Typedefs and Statics
//========================================
enum FileHandlerEnum
{
    FILEHANDLER_PURE3D,
    FILEHANDLER_LEVEL,
    FILEHANDLER_MISSION,
    FILEHANDLER_ANIMATION,
    FILEHANDLER_CHOREO,
    FILEHANDLER_CONSOLE,
    FILEHANDLER_SCROOBY,
    FILEHANDLER_SOUND,
    FILEHANDLER_TEMP,
    FILEHANDLER_ICON,

    NUM_FILEHANDLERS
};

#endif // FILEHANDLERENUM_H
