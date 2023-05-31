//==============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        filehandlerfactory.h
//
// Description: This factory shields its clients from the details of derived
//              FileHandler classes.
//
// History:     3/27/2002 + Created -- Darwin Chau
//
//==============================================================================

#ifndef FILEHANDLERFACTORY_H
#define FILEHANDLERFACTORY_H

//========================================
// Nested Includes
//========================================
#include <loading/filehandlerenum.h>

//========================================
// Forward References
//========================================
class FileHandler;

//==============================================================================
//
// Synopsis: Constructs and returns a FileHandler of the desired derived type.
//           Clients are responsible for deleting the FileHandler.
//
//==============================================================================

class FileHandlerFactory
{
    public:

        static FileHandler* CreateFileHandler( FileHandlerEnum handlerType, const char* sectionName = 0 );
};


#endif // FILEHANDLERFACTORY_H
