//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/02/26		TChu		Created
//
//===========================================================================

#ifndef GUITEXTBIBLE_H
#define GUITEXTBIBLE_H

//===========================================================================
// Nested Includes
//===========================================================================

#include <p3d/plat_types.hpp>

// Scrooby
//
#include <enums.h>

//===========================================================================
// External Constants
//===========================================================================

const Scrooby::XLLanguage SRR2_LANGUAGE[] =
{
    Scrooby::XL_ENGLISH,
    Scrooby::XL_FRENCH,
    Scrooby::XL_GERMAN,
//    Scrooby::XL_ITALIAN,
    Scrooby::XL_SPANISH,

    Scrooby::XL_LAST_LANGUAGE
};

const int NUM_SRR2_LANGUAGES =
    sizeof( SRR2_LANGUAGE ) / sizeof( SRR2_LANGUAGE[ 0 ] ) - 1;

//===========================================================================
// Forward References
//===========================================================================

namespace Scrooby
{
    class TextBible;
}

//===========================================================================
// Interface Definitions
//===========================================================================

class CGuiTextBible
{
public:
    CGuiTextBible();
    virtual ~CGuiTextBible();

    // update reference to Scrooby text bible
    //
    void SetTextBible( const char* textBible );

    // get localized text for string ID
    //
    static P3D_UNICODE* GetLocalizedText( const char* stringID );

    // get/set current locale language
    //
    static void SetCurrentLanguage( const Scrooby::XLLanguage language );
    static Scrooby::XLLanguage GetCurrentLanguage();
    static bool IsTextBibleLoaded();

private:
    static Scrooby::TextBible* s_textBible;
    static Scrooby::XLLanguage s_currentLanguage;

};

inline P3D_UNICODE* GetTextBibleString( const char* stringID )
{
    return CGuiTextBible::GetLocalizedText( stringID );
}

#endif // GUITEXTBIBLE_H
