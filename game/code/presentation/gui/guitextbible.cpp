//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/02/26		TChu		Created
//
//===========================================================================

//===========================================================================
// Includes
//===========================================================================

#include <presentation/gui/guitextbible.h>

#include <app.h>
#include <textbible.h>
#include <raddebug.hpp>

#include <sound/soundmanager.h>

Scrooby::TextBible* CGuiTextBible::s_textBible = NULL;
Scrooby::XLLanguage CGuiTextBible::s_currentLanguage = Scrooby::XL_ENGLISH;

//===========================================================================
// Public Member Functions
//===========================================================================

CGuiTextBible::CGuiTextBible()
{
}

CGuiTextBible::~CGuiTextBible()
{
    s_textBible = NULL;
}

void
CGuiTextBible::SetTextBible( const char* textBible )
{
    rAssert( textBible != NULL );
    s_textBible = Scrooby::App::GetInstance()->GetTextBible( textBible );
}

P3D_UNICODE*
CGuiTextBible::GetLocalizedText( const char* stringID )
{
    rAssert( stringID != NULL );
    rAssert( s_textBible != NULL );

    P3D_UNICODE* localizedText = static_cast<P3D_UNICODE*>( s_textBible->GetWChar( stringID ) );

/*
#ifndef RAD_RELEASE
    if( localizedText == NULL )
    {
        char msg[ 256 ];
        sprintf( msg, "Can't find text bible string for entry: %s!", stringID );
        rTuneWarningMsg( false, msg );
    }
#endif
*/

    return localizedText;
}

void
CGuiTextBible::SetCurrentLanguage( const Scrooby::XLLanguage language )
{
    Scrooby::App::GetInstance()->SetLocalizationLanguage( language );

    s_currentLanguage = language;

    GetSoundManager()->SetDialogueLanguage( language );
}

Scrooby::XLLanguage
CGuiTextBible::GetCurrentLanguage()
{
    return s_currentLanguage;
}

bool CGuiTextBible::IsTextBibleLoaded()
{
    return s_textBible != NULL;
}


