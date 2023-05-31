//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        memoryutilities.h
//
// Description: some funcitons for checking the amount of free memory, etc
//
// History:     2002/12/03 + Created -- Ian Gipson
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <memory/classSizeTracker.h>
#include <memory/srrmemory.h>
#include <map>
#include <p3d/entity.hpp>
#include <raddebugwatch.hpp>

#define SHUTOFFCLASSSIZETRACKER

#ifdef  USECLASSSIZETRACKER
#ifndef RAD_RELEASE
//=============================================================================
//
// Global Data, Local Data, Local Classes
//
//=============================================================================
typedef std::map< tName, int > NAMEINTMAP;
typedef std::map< tName, bool   > NAMEBOOLMAP;
NAMEINTMAP  g_NameToClassSize;
NAMEINTMAP  g_NameToInt;
NAMEINTMAP  g_TotalMemorySize;
NAMEBOOLMAP g_AddedToWatcher;
NAMEBOOLMAP g_SizeAddedToWatcher;
bool        g_DisableClassTracker;

//=============================================================================
//
// Public Member Functions
//
//=============================================================================

bool operator<( const tName& left, const tName& right )
{
    return( left.GetUID() < right.GetUID() );
}
//==============================================================================
// ClassSizeTracker::ClassSizeTracker()
//==============================================================================
//
// Description: constructor
//
// Parameters:	None.
//
// Return:      NONE
//
// Constraints: None
//
//==============================================================================
ClassSizeTracker::ClassSizeTracker()
{
    size_t size = sizeof( ClassSizeTracker );
}


//==============================================================================
// ClassSizeTracker::RegisterClassSize
//==============================================================================
//
// Description: register the size of the class
//
// Parameters:	None.
//
// Return:      NONE
//
// Constraints: None
//
//==============================================================================
void ClassSizeTracker::RegisterClassSize( const tName& className, const unsigned int size )
{
    #ifdef SHUTOFFCLASSSIZETRACKER
        return;
    #endif //SHUTOFFCLASSSIZETRACKER
    if( !HeapManager::IsCreated() )
    {
        return;
    }
    HeapMgr()->PushHeap( GMA_DEFAULT );
    int& sizeInMap = g_NameToClassSize[ className ];
    sizeInMap = size;

    bool& addedYet = g_SizeAddedToWatcher[ className ];
    if( !addedYet )
    {
        bool watcherEnabled = radDebugWatchEnabled();
        if( watcherEnabled )
        {
            //
            // Add the variable to the watcher
            //
            radDbgWatchAddInt( &sizeInMap, className.GetText(), "ClassSizeTracker", NULL, NULL, 0, 1000000, true );
            addedYet = true;
        }
    }
    HeapMgr()->PopHeap( GMA_DEFAULT );
}

//==============================================================================
// ClassSizeTracker::RegisterClassSize
//==============================================================================
//
// Description: register the size of the class
//
// Parameters:	None.
//
// Return:      NONE
//
// Constraints: None
//
//==============================================================================
void ClassSizeTracker::RegisterClassSize( const char* className, const unsigned int size )
{
    #ifdef SHUTOFFCLASSSIZETRACKER
        return;
    #endif //SHUTOFFCLASSSIZETRACKER
    if( !HeapManager::IsCreated() )    {        return;    }
    if( g_DisableClassTracker )        {        return;    }
    g_DisableClassTracker = true;
    RegisterClassSize( static_cast< tName >( className ), size );
    g_DisableClassTracker = false;
}

//==============================================================================
// ClassSizeTracker::RegisterCreation()
//==============================================================================
//
// Description: registers creation of a class by a char*
//
// Parameters:	None.
//
// Return:      NONE
//
// Constraints: None
//
//==============================================================================
void ClassSizeTracker::RegisterCreation( const tName& className )
{
    #ifdef SHUTOFFCLASSSIZETRACKER
        return;
    #endif //SHUTOFFCLASSSIZETRACKER

    if( !HeapManager::IsCreated() )
    {
        return;
    }
    HeapMgr()->PushHeap( GMA_DEFAULT );
    int& size = g_NameToInt[ className ];
    int& totalMemorySize = g_TotalMemorySize[ className ];
    ++size;

    bool& addedYet = g_AddedToWatcher[ className ];
    if( !addedYet )
    {
        bool watcherEnabled = radDebugWatchEnabled();
        if( watcherEnabled )
        {
            //
            // Add the variable to the watcher
            //
            radDbgWatchAddInt( &size, className.GetText(), "ClassCountTracker", NULL, NULL, 0, 1000000, true );
            radDbgWatchAddInt( &totalMemorySize, className.GetText(), "ClassTotalSize", NULL, NULL, 0, 1000000, true );
            addedYet = true;
        }
    }
    HeapMgr()->PopHeap( GMA_DEFAULT );
}

//==============================================================================
// ClassSizeTracker::RegisterCreation()
//==============================================================================
//
// Description: registers creation of a class via a char*
//
// Parameters:	None.
//
// Return:      NONE
//
// Constraints: None
//
//==============================================================================
void ClassSizeTracker::RegisterCreation( const char* className )
{
    #ifdef SHUTOFFCLASSSIZETRACKER
        return;
    #endif //SHUTOFFCLASSSIZETRACKER

    if( !HeapManager::IsCreated() )    {        return;    }
    if( g_DisableClassTracker )        {        return;    }
    g_DisableClassTracker = true;
    tName name = className;
    RegisterCreation( name );
    g_DisableClassTracker = false;
}

//==============================================================================
// ClassSizeTracker::RegisterDestruction()
//==============================================================================
//
// Description: registers the destruction of a class
//
// Parameters:	None.
//
// Return:      NONE
//
// Constraints: None
//
//==============================================================================
void ClassSizeTracker::RegisterDestruction( const tName& className )
{
    #ifdef SHUTOFFCLASSSIZETRACKER
        return;
    #endif //SHUTOFFCLASSSIZETRACKER

    if( !HeapManager::IsCreated() )
    {
        return;
    }

    int& size = g_NameToInt[ className ];
    --size;   
}

//==============================================================================
// ClassSizeTracker::RegisterDestruction()
//==============================================================================
//
// Description: registers the destruction of a class
//
// Parameters:	classname - the name of the class we're destroying
//
// Return:      NONE
//
// Constraints: None
//
//==============================================================================
void ClassSizeTracker::RegisterDestruction( const char* className )
{
    #ifdef SHUTOFFCLASSSIZETRACKER
        return;
    #endif //SHUTOFFCLASSSIZETRACKER

    if( !HeapManager::IsCreated() )    {        return;    }
    if( g_DisableClassTracker )        {        return;    }
    g_DisableClassTracker = true;
    RegisterDestruction( static_cast< tName >( className ) );
    g_DisableClassTracker = false;
}

//==============================================================================
// ClassSizeTracker::UpdateTotalMemorySize()
//==============================================================================
//
// Description: updates the total memory size variable in the watcher
//
// Parameters:	None.
//
// Return:      NONE
//
// Constraints: None
//
//==============================================================================
void ClassSizeTracker::UpdateTotalMemorySize( const tName& className )
{
    #ifdef SHUTOFFCLASSSIZETRACKER
        return;
    #endif //SHUTOFFCLASSSIZETRACKER

    if( !HeapManager::IsCreated() )
    {
        return;
    }
    int totalMemorySize = g_NameToClassSize[ className ] * g_NameToInt[ className ];
    g_TotalMemorySize[ className ] = totalMemorySize;
}

//==============================================================================
// ClassSizeTracker::UpdateTotalMemorySize()
//==============================================================================
//
// Description: updates the total memory size variable in the watcher
//
// Parameters:	className - the class that we're updating the memory size for.
//
// Return:      NONE
//
// Constraints: None
//
//==============================================================================
void ClassSizeTracker::UpdateTotalMemorySize( const char* className )
{
    #ifdef SHUTOFFCLASSSIZETRACKER
        return;
    #endif //SHUTOFFCLASSSIZETRACKER

    if( !HeapManager::IsCreated() )    {        return;    }
    if( g_DisableClassTracker )        {        return;    }
    g_DisableClassTracker = true;
    UpdateTotalMemorySize( static_cast< tName >( className ) );
    g_DisableClassTracker = false;
}
#endif RAD_RELEASE
#endif USECLASSSIZETRACKER