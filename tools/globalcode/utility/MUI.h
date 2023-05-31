#ifndef _MUI_HPP
#define _MUI_HPP
//-----------------------------------------------------------------------------
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// MUI.hpp
//
// Description: Container class for custom UI windows for use in Maya.
//
// Modification History:
//  + Created Sep 30, 2001 -- bkusy 
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

//This is an example callback.
inline BOOL CALLBACK PopupCallBack( HWND hWnd, UINT uMsg, UINT wParam, long lParam )
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        {
            return true;
        }
        break;
    default:
        {
//            EndDialog( hWnd, 0);  //this is how you close the window.
            return false;
        }
        break;
    }
}


class MUI
{
    public:
		
		enum {
				YES,
				NO,
				CANCEL,
                SAVE,
				SET,
                NEW,
                OPEN
		};

        static  int ConfirmDialog( const char* message );

		static bool FileDialog( char* filePath, int filePathSize,
                                const char* windowTitle = 0,
                                const char* extensionFilter = 0,
                                const char* defaultExtension = 0,
                                int browserType = MUI::OPEN
                              );

		static void ErrorDialog( const char* message );

        static void InfoDialog( const char* message );

        static void PopupDialogue( int id, DLGPROC callBack );

    private:
};

#endif
