#ifndef WB_CONSTANTS
#define WB_CONSTANTS

namespace WBConstants
{
    const unsigned int TypeIDPrefix = 0x00040201;
    const float Scale = 100.0f;

    namespace NodeIDs
    {
        const unsigned int EventLocator             = 0xd0;
        const unsigned int ScriptLocator            = 0xd1;
        const unsigned int GenericLocator           = 0xd2;
        const unsigned int CarStartLocator          = 0xd3;
        const unsigned int SplineLocator            = 0xd4;
        const unsigned int ZoneEventLocator         = 0xd5;
        const unsigned int OcclusionLocator         = 0xd6;
        const unsigned int RailCamLocator           = 0xd7;
        const unsigned int InteriorEntranceLocator  = 0xd8;
        const unsigned int DirectionalLocator       = 0xd9;
        const unsigned int ActionEventLocator       = 0xda;
        const unsigned int FOVLocator               = 0xdb;
        const unsigned int BreakableCameraLocator   = 0xdc;
        const unsigned int StaticCameraLocator      = 0xdd;
        const unsigned int PedGroupLocator          = 0xde;

        const unsigned int TriggerVolume            = 0xe0;
    }
}

#endif