//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        contextenum.h
//
// Description: Game contexts.
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

#ifndef CONTEXTENUM_H
#define CONTEXTENUM_H

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
enum ContextEnum
{
    // The following shows the possible transitions between
    // contexts:
    //                            [ PREVIOUS ]        [ CURRENT ]       [ NEXT ]

    CONTEXT_ENTRY,               // (Start)      -----> ENTRY      -----> BOOTUP

    CONTEXT_BOOTUP,              // ENTRY        -----> BOOTUP     -----> FRONTEND

    CONTEXT_FRONTEND,            // BOOTUP       -----> FRONTEND   -----> LOADING_G
                                 //                |                 |
                                 // PAUSE        --|                 |--> LOADING_D

    CONTEXT_LOADING_DEMO,        // FRONTEND     -----> LOADING_D  -----> DEMO

    CONTEXT_DEMO,                // LOADING_D    -----> DEMO       -----> FRONTEND

    CONTEXT_SUPERSPRINT_FE,      // FRONTEND     -----> SS_FE      -----> LOADING_S
                                 //                                  |
                                 //                                  |--> FRONTEND

    CONTEXT_LOADING_SUPERSPRINT, // SS_FE        -----> LOADING_S  -----> SUPERSPRINT

    CONTEXT_SUPERSPRINT,         // LOADING_S    -----> SUPERSPRINT -----> SS_FE

    CONTEXT_LOADING_GAMEPLAY,    // FRONTEND     -----> LOADING_G  -----> GAMEPLAY

    CONTEXT_GAMEPLAY,            // LOADING_G    -----> GAMEPLAY   -----> PAUSE
                                 //                |
                                 // PAUSE        --|

    CONTEXT_PAUSE,               // GAMEPLAY     -----> PAUSE      -----> GAMEPLAY
                                 //                                  | 
                                 //                                  |--> FRONTEND

    CONTEXT_EXIT,                // FRONTEND     -----> EXIT       -----> (End)

    NUM_CONTEXTS
};

#endif // CONTEXTENUM_H
