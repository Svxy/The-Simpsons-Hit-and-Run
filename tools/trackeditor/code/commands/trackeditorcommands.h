#include "precompiled/PCH.h"

#ifndef TE_COMMANDS_H
#define TE_COMMANDS_H

class TEStateChangeCommand : public MPxCommand
{
public:
    TEStateChangeCommand() {};
    ~TEStateChangeCommand() {};

    static void*    creator();
    virtual MStatus doIt( const MArgList& args );  

    static const char*  stringId;
};

class TEGetSelectedVertexPosition : public MPxCommand
{
public:
    TEGetSelectedVertexPosition() {};
    ~TEGetSelectedVertexPosition() {};

    static void*    creator();
    virtual MStatus doIt( const MArgList& args );  

    static const char*  stringId;
};

class TEGetSelectedVertexIndex : public MPxCommand
{
public:
    TEGetSelectedVertexIndex() {};
    ~TEGetSelectedVertexIndex() {};

    static void*    creator();
    virtual MStatus doIt( const MArgList& args );  

    static const char*  stringId;
};

#endif