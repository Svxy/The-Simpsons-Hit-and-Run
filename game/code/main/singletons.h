//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        singletons.h
//
// Description: Define all our global objects here
//
// History:     + Created -- Darwin Chau
//
//=============================================================================
#ifndef SINGLETONS_H_
#define SINGLETONS_H_
//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================

//========================================
// Forward References
//========================================
#ifdef DEBUGWATCH
    void AddVariablesToWatcher();
#else
    inline void AddVariablesToWatcher(){};
#endif //DEBUGWATCH
void CreateSingletons();
void DestroySingletons();

//========================================
// Constants, Typedefs and Statics
//========================================
#endif