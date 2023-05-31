/*
 * fv/interface/platform.hpp
 */


#ifndef SP_INTERFACE_PLATFORM_HPP
#define SP_INTERFACE_PLATFORM_HPP


#include <sp/interface/base.hpp>


extern "C"
{


SP_IMPORT int SP_CALLCONV SPPlatformOpen(HWND hwnd);
SP_IMPORT int SP_CALLCONV SPPlatformClose();


} // extern "C"


#endif // SP_INTERFACE_PLATFORM_HPP
