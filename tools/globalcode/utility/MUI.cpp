//-----------------------------------------------------------------------------
// Copyright (C) 2001 Radical Entertainment Ltd.  All rights reserved.
//
// MUI.cpp
//
// Description: Container class for custom UI windows for use in Maya.
//
// Modification History:
//  + Created Sep 30, 2001 -- bkusy 
//-----------------------------------------------------------------------------

//----------------------------------------
// System Includes
//----------------------------------------
#include <assert.h>

//----------------------------------------
// Project Includes
//----------------------------------------
#include "stdafx.h"
#include "MUI.h"
#include "util.h"
#include "mayahandles.h"

//----------------------------------------
// Forward References
//----------------------------------------

//----------------------------------------
// Constants, Typedefs and Statics
//----------------------------------------


//-----------------------------------------------------------------------------
// c o n f i r m 
//
// Synopsis:    Present a modal confirmation box to the user.
//
// Parameters:  message - the message to display in the box.
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
int MUI::ConfirmDialog( const char* message )
{
    UINT style = MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON1 | MB_TASKMODAL ;
    int button = MessageBox( 0, message, "MUI Confirmation", style );

    int result = MUI::YES;
    if ( IDNO == button ) result = MUI::NO;
    if ( IDCANCEL == button ) result = MUI::CANCEL;

    return result;
}

//-----------------------------------------------------------------------------
// E r r o r D i a l o g
//
// Synopsis:    Present a modal error dialog box to the user.
//
// Parameters:  message - the message to display in the box.
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void MUI::ErrorDialog( const char* message )
{
    UINT style = MB_OK | MB_ICONERROR | MB_TASKMODAL;
    MessageBox( 0, message, "MUI Error", style );
}

//-----------------------------------------------------------------------------
// E r r o r D i a l o g
//
// Synopsis:    Present a modal error dialog box to the user.
//
// Parameters:  message - the message to display in the box.
//
// Returns:     NOTHING
//
// Constraints: NONE
//
//-----------------------------------------------------------------------------
void MUI::InfoDialog( const char* message )
{
    UINT style = MB_OK | MB_ICONINFORMATION | MB_TASKMODAL;
    MessageBox( 0, message, "MUI Info", style );
}

//-----------------------------------------------------------------------------
// F i l e D i a l o g 
//
// Synopsis:    Present the user with a file browser window to select a file for
//              open or save.
//
// Parameters:  filePath         - a reference parameter to receive the full
//                                 file path from the dialog.  If not NULL on
//                                 input, the initial value is used as the
//                                 starting location for the dialog.
//              filePathSize     - the maximum length of the filePath buffer.
//              dialogTitle      - the caption to appear in the dialog title
//                                 bar.
//              extensionFilter  - A filter specifiying the types of files that
//                                 are eligilble for selection.  It must end
//                                 in "||" with all entries separated by "|".
//                                 Generally entries will be in pairs; the 
//                                 first is a type description and the second
//                                 is the actual filter.  e.g./
//                                 "Raddle Data(*.rdl)|*.rdl|All Files(*.*)|*.*||".
//              defaultExtension - if not NULL, then the given extension will 
//                                 automatically appended to an extensionless
//                                 entry in the Filename box.
//              broserType       - OPEN | SAVE.  Default is OPEN.
//
// Returns:     true, if a filePath is set; false if the dialog is cancelled.
//
// Notes:  The filePath will have "/" slashes and not "\" slashes on return.
//
//-----------------------------------------------------------------------------
bool MUI::FileDialog( char* filePath, int filePathSize,
                      const char* windowTitle,
                      const char* extensionFilter,
                      const char* defaultExtension,
                      int browserType
                    )  
{   
    //
    // We do need the file path to have windows backslashes.
    //
    util_replaceCharacters('/', '\\', filePath );

    bool isOk = false;
    
    BOOL doOpen;
    DWORD flags = 0;

	switch ( browserType )
	{
		case MUI::SET:
		{
			doOpen = TRUE;
			flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
			break;
		}

		case MUI::SAVE:
		{
			doOpen = FALSE;
			flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
			break;
		}

		default: // MUI::OPEN
		{
			doOpen = TRUE;
			flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			break;

		}
	}

    //
    // Set the AFX handle to maya's application instance handle.
    // 
    HINSTANCE handle = static_cast<HINSTANCE>(MayaHandles::GetHInstance());
    AfxSetResourceHandle( handle ); 

    //
    // Create the dialog
    //
	CFileDialog fileDialog( doOpen,
                            defaultExtension, 
                            filePath, 
                            flags,
                            extensionFilter,
                            NULL 
					       );

    if ( windowTitle ) fileDialog.m_ofn.lpstrTitle = windowTitle;

    //
    // Show the dialog and wait for a response.
    //
    int result = fileDialog.DoModal();
    if ( IDOK == result )
    {
        const char* filename = LPCSTR( fileDialog.GetPathName() );
        strncpy( filePath, LPCSTR( fileDialog.GetPathName() ), filePathSize );
        util_replaceCharacters('\\', '/', filePath );
        isOk = true;
    }

    return isOk;
}

//=============================================================================
// MUI::PopupDialogue
//=============================================================================
// Description: Comment
//
// Parameters:  ( int id, DLGPROC callBack )
//
// Return:      void 
//
//=============================================================================
void MUI::PopupDialogue( int id, DLGPROC callBack )
{
    HINSTANCE handle = static_cast<HINSTANCE>(MayaHandles::GetHInstance());
    AfxSetResourceHandle( handle );
    
    HWND hWnd = static_cast<HWND>(MayaHandles::GetHWND());

    DialogBox( handle, MAKEINTRESOURCE(id), hWnd, callBack );
}


