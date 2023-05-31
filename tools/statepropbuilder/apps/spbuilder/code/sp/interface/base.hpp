/*
 * fv/interface/base.hpp
 */


#ifndef SP_INTERFACE_BASE_HPP
#define SP_INTERFACE_BASE_HPP


#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>


#ifdef SMARTPROP_ENGINE
#   define SP_IMPORT __declspec(dllexport)
#else
#   define SP_IMPORT __declspec(dllimport)
#endif


#define SP_CALLCONV __cdecl


#endif // SP_INTERFACE_BASE_HPP
