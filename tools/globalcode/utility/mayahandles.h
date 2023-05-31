//-----------------------------------------------------------------------------
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// mayahandles.h
//
// Description: Class to store and retrieve maya handles as generic pointers.
//
// Modification History:
//  + Created Oct 15, 2001 -- bkusy 
//-----------------------------------------------------------------------------

//----------------------------------------
// System Includes
//----------------------------------------

//----------------------------------------
// Project Includes
//----------------------------------------

//----------------------------------------
// Forward References
//----------------------------------------

//----------------------------------------
// Constants, Typedefs and Statics
//----------------------------------------

class MayaHandles
{
    public:

        static void SetHInstance( void* hInstance );
        static void* GetHInstance();

        static void* GetHWND();

    private:

        static void* m_hInstance;
        static void* m_HWND;
};
        
