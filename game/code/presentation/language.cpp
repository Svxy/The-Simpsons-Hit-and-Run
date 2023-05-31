//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        language.cpp
//
// Description: contains functions for dealing with language
//
// History:     21/1/2002 + Created -- Ian Gipson
//
//=============================================================================

//========================================
// System Includes
//========================================
#ifdef RAD_PS2
    #include <libscf.h>
#endif

#ifdef RAD_GAMECUBE
    #include <dolphin/os.h>
#endif

#ifdef RAD_XBOX
    #include <xtl.h>
#endif
//========================================
// Project Includes
//========================================

#include <presentation/language.h>

//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************
namespace Language{

//=============================================================================
// Language::GetHardwareLanguage()
//=============================================================================
// Description: returns the currently set language for the console.
//
// Parameters: None.
//
// Return:      Language enum specifying the current language
//
//=============================================================================
Language GetHardwareLanguage()
{
    #ifdef RAD_PS2
    ////////////////////////////////////////////////////////////
    // PS2
    ////////////////////////////////////////////////////////////
    switch ( sceScfGetLanguage() )
    {
        case SCE_DUTCH_LANGUAGE : 
        {
            return DUTCH;
        }
        case SCE_ENGLISH_LANGUAGE : 
        {
            return ENGLISH;
        }
        case SCE_FRENCH_LANGUAGE : 
        {
            return FRENCH;
        }
        case SCE_GERMAN_LANGUAGE : 
        {
            return GERMAN;
        }
        case SCE_ITALIAN_LANGUAGE : 
        {
            return ITALIAN;
        }
        case SCE_JAPANESE_LANGUAGE : 
        {
            return JAPANESE;
        }
        case SCE_PORTUGUESE_LANGUAGE : 
        {
            return PORTUGUESE;
        }
        case SCE_SPANISH_LANGUAGE : 
        {
            return SPANISH;
        }
        default : 
        {
            return UNKNOWN;
        }
    }
    #endif

    #ifdef RAD_XBOX
    ////////////////////////////////////////////////////////////
    // XBOX
    ////////////////////////////////////////////////////////////
    switch ( XGetLanguage() )
    {
        case XC_LANGUAGE_ENGLISH : 
        {
            return ENGLISH;
        }
        case XC_LANGUAGE_FRENCH : 
        {
            return FRENCH;
        }
        case XC_LANGUAGE_GERMAN : 
        {
            return GERMAN;
        }
        case XC_LANGUAGE_ITALIAN : 
        {
            return ITALIAN;
        }
        case XC_LANGUAGE_JAPANESE : 
        {
            return JAPANESE;
        }
        case XC_LANGUAGE_SPANISH : 
        {
            return SPANISH;
        }
        default : 
        {
            return UNKNOWN;
        }
    }
    #endif

    #ifdef RAD_GAMECUBE
    switch ( OSGetLanguage() )
    {
        case OS_LANG_ENGLISH:
        {
            return ENGLISH;
        }
        case OS_LANG_GERMAN:
        {
            return GERMAN;
        }
        case OS_LANG_FRENCH:
        {
            return FRENCH;
        }
        case OS_LANG_SPANISH:
        {
            return SPANISH;
        }
        case OS_LANG_ITALIAN:
        {
            return ITALIAN;
        }
        case OS_LANG_DUTCH:
        {
            return DUTCH;
        }
        default:
        {
            return UNKNOWN;
        }
    }
    #endif

    #ifdef RAD_WIN32
    ////////////////////////////////////////////////////////////
    // WIN32
    ////////////////////////////////////////////////////////////
    return ENGLISH;     // to be implemented.
    #endif
}

} //namespace Language