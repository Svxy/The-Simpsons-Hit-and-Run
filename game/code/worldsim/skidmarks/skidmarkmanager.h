//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   SkidmarkManager
//
// Description: 
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef SKIDMARKMANAGER_H
#define SKIDMARKMANAGER_H


//===========================================================================
// Nested Includes
//===========================================================================

//===========================================================================
// Forward References
//===========================================================================

class Skidmark;

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================

const int DEFAULT_NUM_SKID_MARKS = 30;

//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//      Generally, describe what behaviour this class possesses that
//      clients can rely on, and the actions that this service
//      guarantees to clients.
//
// Constraints:
//
//===========================================================================
class SkidmarkManager
{
    public:

        // Static Methods (for creating, destroying and acquiring an instance 
        // of the TriStripDSGManager)
        static SkidmarkManager* CreateInstance();
        static SkidmarkManager* GetInstance();
        static void DestroyInstance();
        static SkidmarkManager* spInstance;
        
        void Update( unsigned int timeInMS );
        Skidmark* GetUnusedSkidmark();
        void ReturnUsedSkidmark( Skidmark* );

        void SetTimedFadeouts( bool useFadeouts ) { m_UseTimedFadeouts = useFadeouts; }
        void Init( int numSkids = DEFAULT_NUM_SKID_MARKS );
        void Destroy();

    private:
        SkidmarkManager();
        ~SkidmarkManager();
        SkidmarkManager( const SkidmarkManager& );
        SkidmarkManager& operator=( const SkidmarkManager& );

        struct ManagedSkidmark
        {
            ManagedSkidmark();
            ~ManagedSkidmark();

            Skidmark* skidmark;
            unsigned int age;
            bool inUse;
        };

        ManagedSkidmark** m_SkidMarks;
        // Returns a preallocated skidmark thats not currently in use at the moment
        bool m_UseTimedFadeouts;
        int m_NumSkidMarks;

};
// A little syntactic sugar for getting at this singleton.
inline SkidmarkManager* GetSkidmarkManager() 
{ 
   return( SkidmarkManager::GetInstance() ); 
}

#endif