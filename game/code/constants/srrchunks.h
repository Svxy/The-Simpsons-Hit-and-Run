#ifndef SRR_CHUNKS_H
#define SRR_CHUNKS_H

//CHUNK IDs
//0x03000000 - 0x03ffffff  Simpsons

namespace SRR2
{
    namespace ChunkID
    {
        //Track Editor / World Builder would like to reseve up to
        //0x030000ff
        const unsigned WALL             = 0x03000000;
        const unsigned FENCELINE        = 0x03000001;
        const unsigned ROAD_SEGMENT     = 0x03000002;
        const unsigned ROAD             = 0x03000003;
        const unsigned INTERSECTION     = 0x03000004;

        const unsigned LOCATOR          = 0x03000005;
        const unsigned TRIGGER_VOLUME   = 0x03000006;
        const unsigned SPLINE           = 0x03000007;
        const unsigned INSTANCES        = 0x03000008;

        const unsigned ROAD_SEGMENT_DATA = 0x03000009;
        
        const unsigned RAIL             = 0x0300000A;

        const unsigned PED_PATH         = 0x0300000B;
        const unsigned EXTRA_MATRIX     = 0x0300000C;
        const unsigned PED_PATH_SEGMENT = 0x0300000D;
		const unsigned TERRAIN_TYPE     = 0x0300000E;
        
	//Camera data ids 
        const unsigned FOLLOWCAM = 0x03000100;
        const unsigned WALKERCAM = 0x03000101;        

    // SFX chunk id's.
        const unsigned CHUNK_SET                = 0x03000110;

        //Next usable ID 0x03000120


    //Object Attribute chunks for greg.
		const unsigned OBJECT_ATTRIBUTES = 0x03000600;
		const unsigned PHYS_WRAPPER = 0x03000601;
		const unsigned ATTRIBUTE_TABLE = 0x03000602;

    // Effects chunk id's
        const unsigned BREAKABLE_OBJECT         = 0x03001000;
        const unsigned INST_PARTICLE_SYSTEM     = 0x03001001;
        
	//DSG chunk id's
        const unsigned ENTITY_DSG               = 0x03f00000;
        const unsigned STATIC_PHYS_DSG          = 0x03f00001;
        const unsigned DYNA_PHYS_DSG            = 0x03f00002;
        const unsigned INTERSECT_DSG            = 0x03f00003;
        const unsigned TREE_DSG                 = 0x03f00004;
        const unsigned CONTIGUOUS_BIN_NODE      = 0x03f00005;
        const unsigned SPATIAL_NODE             = 0x03f00006;
        const unsigned FENCE_DSG                = 0x03f00007;
        const unsigned ANIM_COLL_DSG            = 0x03f00008;
        const unsigned INSTA_ENTITY_DSG         = 0x03f00009;
        const unsigned INSTA_STATIC_PHYS_DSG    = 0x03f0000A;
        const unsigned WORLD_SPHERE_DSG         = 0x03f0000B;
        const unsigned ANIM_DSG                 = 0x03f0000C;
		const unsigned LENS_FLARE_DSG			= 0x03f0000D;
		const unsigned INSTA_ANIM_DYNA_PHYS_DSG	= 0x03f0000E;
		const unsigned ANIM_DSG_WRAPPER			= 0x03f0000F;
		const unsigned ANIM_OBJ_DSG_WRAPPER     = 0x03f00010;
    }
}

#endif
