//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        worldbuilder.h
//
// Description: Blahblahblah
//
// History:     16/05/2002 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef WORLDBUILDER_H
#define WORLDBUILDER_H

//========================================
// Nested Includes
//========================================
#include "precompiled/PCH.h"

#include <assert.h>

#include <..\..\..\game\code\meta\locatortypes.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

const char* const WORLD_BUILDER_NAME        = "WBWorldBuilder";
const char* const LOCATORS_NAME             = "WBLocators";
const char* const TRIGGER_VOLUMES_NAME      = "WBTriggerVolumes";
const char* const SPLINES_NAME              = "WBSplines";

class WorldBuilder
{
public:
    
    enum { MAX_PREFIX_LENGTH = 24, MAX_NAME_LENGTH = 256 };

    enum DisplayLevel
    {
        EVENT_LOCATORS              = ( 1 << 0 ),
        SCRIPT_LOCATORS             = ( 1 << 1 ),
        GENERIC_LOCATORS            = ( 1 << 2 ),
        CARSTART_LOCATORS           = ( 1 << 3 ),
        ZONE_EVENT_LOCATORS         = ( 1 << 4 ),
        OCCLUSION_LOCATORS          = ( 1 << 5 ),
        RAILCAM_LOCATORS            = ( 1 << 6 ),
        INTERIOR_LOCATORS           = ( 1 << 7 ),
        DIRECTIONAL_LOCATORS        = ( 1 << 8 ),
        ACTION_EVENT_LOCATORS       = ( 1 << 9 ),
        FOV_LOCATORS                = ( 1 << 10 ),
        BREAKABLE_CAMERA_LOCATORS   = ( 1 << 11 ),
        STATIC_CAMERA_LOCATORS      = ( 1 << 12 ),
        PED_GROUP_LOCATORS          = ( 1 << 13 ),


        LOCATORS = EVENT_LOCATORS | SCRIPT_LOCATORS | 
                   GENERIC_LOCATORS | ZONE_EVENT_LOCATORS | 
                   OCCLUSION_LOCATORS | RAILCAM_LOCATORS | 
                   INTERIOR_LOCATORS | DIRECTIONAL_LOCATORS |
                   ACTION_EVENT_LOCATORS | FOV_LOCATORS |
                   BREAKABLE_CAMERA_LOCATORS | STATIC_CAMERA_LOCATORS |
                   PED_GROUP_LOCATORS,

        TRIGGER_VOLUMES = ( 1 << 20 ),

        ALL = ~0,

        TOTAL_LEVELS = 6
    };
    
    WorldBuilder();
    virtual ~WorldBuilder();

    static unsigned int GetDisplayLevel();
    static LocatorType::Type GetLocatorType();

    static void SetPrefix( const char* prefix );
    static const char* const GetPrefix();

    static bool Exists();
    static MStatus AddChild( MObject& obj );

    static void SetSelectedLocator( const char* name );
    static const char* GetSelectedLocator();

    static const char* sName;

protected:

    friend class WBChangeDisplayCommand;
    static unsigned int sDisplayLevel;

    friend class WBSetLocatorTypeCmd;
    static LocatorType::Type sLocatorType;

    static char sPrefix[MAX_PREFIX_LENGTH + 1];  //+ 1 for the \n
    static char sSelectedLocator[MAX_NAME_LENGTH + 1];

    static void SetDisplayLevel( unsigned int level, bool on );
    static void SetLocatorType( LocatorType::Type type );

private:

    static void CreateWorldBuilderNode();

    //Prevent wasteful constructor creation.
    WorldBuilder( const WorldBuilder& worldbuilder );
    WorldBuilder& operator=( const WorldBuilder& worldbuilder );
};

//*****************************************************************************
//
// Inline Public Methods
//
//*****************************************************************************

//=============================================================================
// WorldBuilder::GetDisplayLevel
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      DisplayLevel 
//
//=============================================================================
inline unsigned int WorldBuilder::GetDisplayLevel()
{
    return sDisplayLevel;
}

//=============================================================================
// WorldBuilder::GetLocatorType
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      LocatorType 
//
//=============================================================================
inline LocatorType::Type WorldBuilder::GetLocatorType()
{
    return sLocatorType;
}

//=============================================================================
// WorldBuilder::SetLocatorType
//=============================================================================
// Description: Comment
//
// Parameters:  ( LocatorType::Type type )
//
// Return:      void 
//
//=============================================================================
inline void WorldBuilder::SetLocatorType( LocatorType::Type type )
{
    sLocatorType = type;
}

//=============================================================================
// WorldBuilder::GetPrefix
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const char* const 
//
//=============================================================================
inline const char* const WorldBuilder::GetPrefix()
{
    return sPrefix;
}

//=============================================================================
// WorldBuilder::SetPrefix
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* prefix )
//
// Return:      void 
//
//=============================================================================
inline void WorldBuilder::SetPrefix( const char* prefix )
{
    strcpy( sPrefix, prefix );
    sPrefix[MAX_PREFIX_LENGTH] = '\0';
}

//=============================================================================
// WorldBuilder::SetSelectedLocator
//=============================================================================
// Description: Comment
//
// Parameters:  ( const char* name )
//
// Return:      void 
//
//=============================================================================
inline void WorldBuilder::SetSelectedLocator( const char* name )
{
    strcpy( sSelectedLocator, name );
    sSelectedLocator[MAX_NAME_LENGTH] = '\0';
}

//=============================================================================
// WorldBuilder::GetSelectedLocator
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      const 
//
//=============================================================================
inline const char* WorldBuilder::GetSelectedLocator()
{
    return sSelectedLocator;
}

//*****************************************************************************
//
// Inline Protected Methods
//
//*****************************************************************************

//=============================================================================
// WorldBuilder::SetDisplayLevel
//=============================================================================
// Description: Comment
//
// Parameters:  ( unsigned int level, bool on )
//
// Return:      inline 
//
//=============================================================================
inline void WorldBuilder::SetDisplayLevel( unsigned int level, bool on )
{
    assert( level <= TOTAL_LEVELS );

    on ? sDisplayLevel |= ( 1 << level ) : sDisplayLevel &= ~( 1 << level );
}

//*****************************************************************************
//
// Inline Private Methods
//
//*****************************************************************************

#endif //WORLDBUILDER_H
