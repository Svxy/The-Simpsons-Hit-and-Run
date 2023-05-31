//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        memoryutilities.h
//
// Description: functions for telling you things about memory
//
// History:     2002/12/03 + Created -- Ian Gipson
//
//=============================================================================

#ifndef CLASSSIZETRACKER_H_
#define CLASSSIZETRACKER_H_

//========================================
// Nested Includes
//========================================
class tName;
#ifndef WIN32
#define USECLASSSIZETRACKER
#endif

//==============================================================================
//
// Synopsis: This class allows you to tag allocations for reporting in the 
//           FTech Memory Monitor.
//
//==============================================================================
#if !(defined RAD_RELEASE) && (defined USECLASSSIZETRACKER)
class ClassSizeTracker
{
public:
    ClassSizeTracker();
    static void RegisterClassSize  ( const tName& className, const unsigned int size );
    static void RegisterClassSize  ( const char*  className, const unsigned int size );
    static void RegisterCreation   ( const tName& className );
    static void RegisterCreation   ( const char*  className );
    static void RegisterDestruction( const tName& className );
    static void RegisterDestruction( const char*  className );
    static void UpdateTotalMemorySize( const tName& className );
    static void UpdateTotalMemorySize( const char*  className );
protected:
private:
};

    #define CLASSTRACKER_CREATE( myname )  ClassSizeTracker::RegisterCreation( #myname );                   \
                                           ClassSizeTracker::RegisterClassSize( #myname, sizeof( myname ) );\
                                           ClassSizeTracker::UpdateTotalMemorySize( #myname )               \

    #define CLASSTRACKER_DESTROY( myname ) ClassSizeTracker::RegisterDestruction( #myname );  \
                                           ClassSizeTracker::UpdateTotalMemorySize( #myname ) \

#else
    #define CLASSTRACKER_CREATE( myname )  ((void)0)
    #define CLASSTRACKER_DESTROY( myname ) ((void)0)
#endif

#endif //MEMORYTAGGER_H
