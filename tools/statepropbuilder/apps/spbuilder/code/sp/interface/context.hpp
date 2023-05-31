/*
 * sp/interface/context.hpp
 */


#ifndef SP_INTERFACE_CONTEXT_HPP
#define SP_INTERFACE_CONTEXT_HPP

#include <sp/interface/base.hpp>


extern "C"
{

    SP_IMPORT int SP_CALLCONV SPContextOpen(HWND hwnd);
    SP_IMPORT int SP_CALLCONV SPContextClose();

    SP_IMPORT int SP_CALLCONV SPContextViewResize(int w, int h);

    SP_IMPORT int SP_CALLCONV SPContextIsPDDIStatsEnabled();
    SP_IMPORT int SP_CALLCONV SPContextSetIsPDDIStatsEnabled(int statsEnabled);

    SP_IMPORT int SP_CALLCONV SPContextAdvance();
    SP_IMPORT int SP_CALLCONV SPContextDisplay();

    SP_IMPORT int SP_CALLCONV SPContextMouseDown(int button, int shift, int x, int y);
    SP_IMPORT int SP_CALLCONV SPContextMouseMove(int button, int shift, int x, int y);
    SP_IMPORT int SP_CALLCONV SPContextMouseUp(int button, int shift, int x, int y);
    SP_IMPORT int SP_CALLCONV SPContextMouseWheel(int scroll);

    SP_IMPORT float SP_CALLCONV SPSimulationGetFPS();
    SP_IMPORT float SP_CALLCONV SPSimulationGetRate();
    SP_IMPORT int SP_CALLCONV SPSimulationSetRate(float rate);

    SP_IMPORT unsigned int SP_CALLCONV SPGetMemoryUsage();

    SP_IMPORT int SP_CALLCONV SPContextGetCameraLock();
    SP_IMPORT int SP_CALLCONV SPContextSetCameraLock(int cameraLock);

    SP_IMPORT int SP_CALLCONV SPInventoryClear();
    SP_IMPORT int SP_CALLCONV SPInventoryGetEntityCount();
    SP_IMPORT int SP_CALLCONV SPInventoryGetEntityName(int index, char* name, int nameLen, char* type, int typeLen);

    SP_IMPORT int SP_CALLCONV SPSetBackgroundColour( int r , int g , int b );


} // extern "C"


#endif // SP_INTERFACE_CONTEXT_HPP
