//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   footprintmanager
//
// Description: Holds a list of all footprints, and fades them out over time
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef FOOTPRINTMANAGER_H
#define FOOTPRINTMANAGER_H


//===========================================================================
// Nested Includes
//===========================================================================

#include <worldsim\character\footprint\footprintmanager.h>
#include <render\culling\swaparray.h> 
#include <p3d\utility.hpp>

//===========================================================================
// Forward References
//===========================================================================

class tShader;
class tTexture;

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================



//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//      Footprint manager - creates and renders footprints
//
// Constraints:
//      
//
//===========================================================================
class FootprintManager
{
    public:
        FootprintManager();
        ~FootprintManager();

	    static FootprintManager* GetInstance( void );
	    static FootprintManager* CreateInstance( void );
        static void DestroyInstance( void );

        enum { MAX_NUM_FOOTPRINTS = 1 };
        enum TYPE { eSquishies, eNumFootprintTypes };

        bool CreateFootprint( const rmt::Matrix& transform, TYPE type );
        void Update( unsigned int timeInMS );
        void FreeTextures();

        void SetTexture( TYPE, const char* texture );
        void ClearAllFootPrints();
        void Render();

    protected:

    private:

        static FootprintManager* spFootprintManager;

        struct Footprint
        {
            Footprint();

            rmt::Vector points[4];
            float alpha;
        };
        
        typedef SwapArray< Footprint > ListOfFootprints;
        
        ListOfFootprints m_ActiveFootprints[eNumFootprintTypes];
        
        tTexture* mpTextures[ eNumFootprintTypes ];

        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow FootprintManager from being copied and assigned.
        FootprintManager( const FootprintManager& );
        FootprintManager& operator=( const FootprintManager& );

        // A little syntactic sugar for getting at this singleton.
};

inline FootprintManager* GetFootprintManager() { return( FootprintManager::GetInstance() ); }



#endif