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

    ExportCommand();
    ~ExportCommand();

    static void*    creator();
    virtual MStatus doIt( const MArgList& args );  

    static const char*  stringId;

private:
    static bool sRegisteredChunks;
};
#endif