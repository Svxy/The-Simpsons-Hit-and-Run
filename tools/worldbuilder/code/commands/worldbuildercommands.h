#ifndef WORLD_BUILDER_COMMANDS_H
#define WORLD_BUILDER_COMMANDS_H

#include "precompiled/PCH.h"

class WBChangeDisplayCommand : public MPxCommand
{
public:
    WBChangeDisplayCommand() {};
    ~WBChangeDisplayCommand() {};

    static void*    creator();
    virtual MStatus doIt( const MArgList& args );  

    static const char*  stringId;
};

class WBSetLocatorTypeCmd : public MPxCommand
{
public:
    WBSetLocatorTypeCmd() {};
    virtual ~WBSetLocatorTypeCmd() {};

    static void*    creator();
    virtual MStatus doIt( const MArgList& args );  

    static const char*  stringId;
};

class WBSetPrefixCmd : public MPxCommand
{
public:
    WBSetPrefixCmd() {};
    virtual ~WBSetPrefixCmd() {};

    static void*    creator();
    virtual MStatus doIt( const MArgList& args );

    static const char* stringId;
};

class WBSnapLocatorCmd : public MPxCommand
{
public:
    WBSnapLocatorCmd() {};
    virtual ~WBSnapLocatorCmd() {};

    static void*    creator();
    virtual MStatus doIt( const MArgList& args );

    static const char* stringId;
};

class WBSplineCompleteCmd : public MPxCommand
{
public:
    WBSplineCompleteCmd() {};
    virtual ~WBSplineCompleteCmd() {};

    static void*    creator();
    virtual MStatus doIt( const MArgList& args );

    static const char* stringId;
};

class WBCoinSplineCompleteCmd : public MPxCommand
{
public:
    WBCoinSplineCompleteCmd() {};
    virtual ~WBCoinSplineCompleteCmd() {};

    static void*    creator();
    virtual MStatus doIt( const MArgList& args );

    static const char* stringId;
};

class WBSelectObjectCmd : public MPxCommand
{
public:
    WBSelectObjectCmd() {};
    virtual ~WBSelectObjectCmd() {};

    static void*    creator();
    virtual MStatus doIt( const MArgList& args );

    static const char* stringId;
};

#endif //WORLD_BUILDER_COMMANDS_H