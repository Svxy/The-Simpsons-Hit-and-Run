#include "precompiled/PCH.h"

#ifndef INTERSECTION_COMMANDS
#define INTERSECTION_COMMANDS

class CreateRoadCmd : public MPxCommand
{
public:
    CreateRoadCmd() {};
    ~CreateRoadCmd() {};

    static void*    creator();
    virtual MStatus doIt( const MArgList& args );  

    static const char*  stringId;
};

class AddIntersectionToRoadCmd : public MPxCommand
{
public:
    AddIntersectionToRoadCmd() {};
    ~AddIntersectionToRoadCmd() {};

    static void*    creator();
    virtual MStatus doIt( const MArgList& args );  

    static const char*  stringId;
};

class ShowRoadCmd : public MPxCommand
{
public:
    ShowRoadCmd() {};
    ~ShowRoadCmd() {};

    static void*    creator();
    virtual MStatus doIt( const MArgList& args );  

    static const char*  stringId;
};

class DestroyRoadCmd : public MPxCommand
{
public:
    DestroyRoadCmd() {};
    ~DestroyRoadCmd() {};

    static void*    creator();
    virtual MStatus doIt( const MArgList& args );  

    static const char*  stringId;
};



//Global tool like thing.
bool GetRoadFromSelectionList( MObject& road );
bool GetRoadsFromSelectionList( MObjectArray& road );

#endif