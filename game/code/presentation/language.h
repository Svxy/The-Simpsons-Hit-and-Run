//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        language.h
//
// Description: this file contains the prototypes for functions for getting 
//              access to languages that the console is set to
//
// History:     21/11/2002 + Created -- Ian Gipson
//
//=============================================================================

#ifndef LANGUAGE_H
#define LANGUAGE_H

//========================================
// Nested Includes
//========================================


//========================================
// Forward References
//========================================
namespace Language
{
    enum Language
    {
        ENGLISH,
        FRENCH,
        GERMAN,
        ITALIAN,
        SPANISH,
        DUTCH,
        JAPANESE,
        PORTUGUESE,

        UNKNOWN
    };

    Language GetHardwareLanguage();
}

#endif // LANGUAGE_H
