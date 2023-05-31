//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   SkidMarkGenerator
//
// Description: A per-vehicle class that generates skidmarks
//
// Authors:     Michael Riegger
//
//===========================================================================

// Recompilation protection flag.
#ifndef SKIDMARKGENERATOR_H
#define SKIDMARKGENERATOR_H

//===========================================================================
// Nested Includes
//===========================================================================

#include <render/IntersectManager/IntersectManager.h>



//===========================================================================
// Forward References
//===========================================================================

class Skidmark;
class tShader;

//===========================================================================
// Constants, Typedefs, and Macro Definitions (needed by external clients)
//===========================================================================

//===========================================================================
// Interface Definitions
//===========================================================================

//===========================================================================
//
// Description: 
//
//			
//
//
//
// Constraints:
//
//===========================================================================



class SkidMarkGenerator
{
    public:
        // Tries and finds the shaders the skidmarks in the inventory, retrieves
        // them so that skidmarkgenerator doesn't need to call p3d::find<>
        // and also sets the zwrite/ztest properties
        static void InitShaders();
        // Releases the shaders that were addrefed in initshaders
        static void ReleaseShaders();

    public:
        SkidMarkGenerator();
        ~SkidMarkGenerator();

		enum Constants
		{
			MAX_NUM_WHEELS = 4
		};

		// Data for a single tire
		struct TireData
		{
			// Tire dimensions as they would look when pressed evenly flat against the road
			// A tank tread, or a snowboard, for instance would be much longer than it is wide
			float width;
			float length;
			// offset of the tire, relative to the vehicle center
			rmt::Vector offset;			
		};

		struct SkidData
		{
			SkidData() : groundPlaneNormal( 0 , 1 ,0 ) { }

			// Direction that the vehicle is moving
			rmt::Vector velocityDirection;
			// Orientation and position of the vehicle
			rmt::Matrix transform;			
			float intensity; // 1 most intense, 0 nonexistent skid
			// Ground plane normal, used to raise the skid mark over the ground
			// to avoid Z figthing. 
			// Constraint : this variable must be normalized before use
			rmt::Vector groundPlaneNormal;

            // Terrain type underneath the tire. Used to determine which texture to use.
            eTerrainType terrainType;
		};

		// Generate a skidmark, starts a new strip if necessary


		/*
                      ^
                      |
                      |
                      |
                      +Z 
                      |
                      |
                    front
                _____________  
       wheel 2  |           |  wheel 3 
                |           |   
                |           |   
                |           |   
                |           |   
                |           |   
                |           | ---+X----->   
                |           |   
                |           |   
                |           |   
                |           |   
                |           |   
       wheel 1  -------------  wheel 0
                     rear

					 */

		// Sets the translation offset for the specified wheel relative to the
		// vehicle's origin
		void SetWheelOffset( int wheel, const rmt::Vector& offset )
        {
            m_CurrentSkidData[ wheel ].offset = offset;
        }   

		// Sets the length and width of the wheel as it presses against the ground
		// Determines the dimensions of the skid
		void SetWheelDimensions( int wheel, float width, float length )
        {
            m_CurrentSkidData[ wheel ].halfWidth = width / 2.0f;
        }
		void GenerateSkid( int wheel, const SkidData& );

        void Update(  );

    protected:

    private:

        tShader* GetShader( eTerrainType terrainType );
        void LayDownVertices( int wheel, const SkidMarkGenerator::SkidData& skidData );

		struct TireGeneratorData
		{	
            TireGeneratorData():
                halfWidth( 0.25f ),
                currentSkid( NULL ),
                skidExtendedThisFrame( false ),
                offset( 0,0,0 ),
                wasLastVertexLaidTemporary( false ),
                terrainType( TT_NumTerrainTypes )
            {

            }
            float halfWidth;
            Skidmark* currentSkid;
            bool skidExtendedThisFrame;
            rmt::Vector offset;
            bool wasLastVertexLaidTemporary;
            eTerrainType terrainType;
        };

        TireGeneratorData m_CurrentSkidData[4];



        // These methods defined as private and not implemented ensure that
        // clients will not be able to use them.  For example, we will
        // disallow SkidMarkGenerator from being copied and assigned.
        SkidMarkGenerator( const SkidMarkGenerator& );
        SkidMarkGenerator& operator=( const SkidMarkGenerator& );

};

#endif