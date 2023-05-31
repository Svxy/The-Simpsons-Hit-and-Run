//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Authors:     Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2003/07/27      TChu        Created for SRR2
//
//===========================================================================

#ifndef GUIHUDTEXTBOX_H
#define GUIHUDTEXTBOX_H

//===========================================================================
// Nested Includes
//===========================================================================

//===========================================================================
// External Constants
//===========================================================================

const float MESSAGE_TEXT_SCALE = 0.8f;
const float MESSGAE_TEXT_HORIZONTAL_STRETCH = 1.1f;

#ifdef RAD_WIN32
const float MESSAGE_BOX_CORRECTION_SCALE = 1.0f;
const float MESSAGE_BOX_HORIZONTAL_STRETCH = 1.1f;
#else
const float MESSAGE_BOX_CORRECTION_SCALE = 2.0f;
const float MESSAGE_BOX_HORIZONTAL_STRETCH = 1.1f;
#endif

#ifdef PAL
    const int MESSAGE_TEXT_VERTICAL_TRANSLATION = +10;
    const float MESSAGE_BOX_VERTICAL_STRETCH = 1.2f;
#else
    const int MESSAGE_TEXT_VERTICAL_TRANSLATION = 0;
    const float MESSAGE_BOX_VERTICAL_STRETCH = 1.0f;
#endif // PAL

#endif // GUIHUDTEXTBOX_H
