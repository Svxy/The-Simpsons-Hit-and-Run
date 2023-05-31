#include "precompiled/PCH.h"

#ifndef EXPORT_COMMAND_H
#define EXPORT_COMMAND_H

class ExportCommand : MPxCommand
{
public:
    enum ExportArg
    {
        SELECTED,
        ALL
    };

    static void*    creator();
    virtual MStatus doIt( const MArgList& args );  

    static const char*  stringId;

private:
    ExportCommand();
    ~ExportCommand();

    static bool sRegisteredChunks;
};
#endif