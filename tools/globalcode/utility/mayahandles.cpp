//-----------------------------------------------------------------------------
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// mayahandles.cpp
//
// Description: Class to store and retrieve maya handles as generic pointers.
//
// Modification History:
//  + Created Oct 15, 2001 -- bkusy 
//  + More MFC goodness -- Cary Brisebois
//-----------------------------------------------------------------------------

//----------------------------------------
// System Includes
//----------------------------------------

//----------------------------------------
// Project Includes
//----------------------------------------
#include "mayahandles.h"
#include "stdafx.h"
#include "maya/mglobal.h"

//----------------------------------------
// Forward References
//----------------------------------------

//----------------------------------------
// Constants, Typedefs and Statics
//----------------------------------------
void* MayaHandles::m_hInstance = 0;
void* MayaHandles::m_HWND = 0;

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
	char str[1024];
	::GetWindowText(hwnd, str, 1024);

    MString mayaVersion = MGlobal::mayaVersion();

    char version[256];
    sprintf( version, "Maya %s", mayaVersion.asChar() );
    
	if(strncmp(str, version, strlen(version)) == 0)  //This is silly.
	{
		*(HWND*)lParam = hwnd;
		return FALSE;
	}
	return TRUE;
}


//-----------------------------------------------------------------------------
// S e t H I n s t a n c e 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void MayaHandles::SetHInstance( void* hInstance )
{
    m_hInstance = hInstance;
}

//-----------------------------------------------------------------------------
// G e t H I n s t a n c e 
//
// Synopsis:
//
// Parameters:  NONE
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void* MayaHandles::GetHInstance()
{
    return m_hInstance;
}

void* MayaHandles::GetHWND()
{
    if ( m_HWND == 0 )
    {
        HWND hWnd = NULL;
        EnumChildWindows(::GetDesktopWindow(), EnumChildProc, (long)&hWnd);
        if(hWnd == NULL)
        {
	        // houston we have a problem
	        MGlobal::displayError("can't find Maya window");
        }
        else
        {
            m_HWND = hWnd;
        }
    }

    return m_HWND;
}