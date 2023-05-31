//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        debugconsolecallback.cpp
//
// Description: 
//
// History:     + Created. Thanks to Mirth for this. -- Darwin Chau
//
//=============================================================================

//========================================
// System Includes
//========================================
// Radcore
#include <raddebugconsole.hpp>
// Pure3D
#include <p3d/utility.hpp>


//========================================
// Project Includes
//========================================
#include <console/debugconsolecallback.h>
#include <console/console.h>
#include <console/fbstricmp.h>

#include <string.h>


//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************


//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************


//==============================================================================
// DebugConsoleCallback::DebugConsoleCallback
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
DebugConsoleCallback::DebugConsoleCallback()
    :
    mFirstHistoryIndex( 0 ),
    mLastHistoryIndex( 0 ),
    mGetHistoryIndex( 0 ),
    mCursorPosition( 0 )
{
    mConsoleHistory[0][0] = '\0';
    mTabCompleteString  = "";
    mTabCurrentFunction = "";
}


//==============================================================================
// DebugConsoleCallback::~DebugConsoleCallback
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
DebugConsoleCallback::~DebugConsoleCallback()
{
}


//==============================================================================
// DebugConsoleCallback::OnVKey
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void DebugConsoleCallback::OnVKey
(
    int virtualKey, 
    bool ctrl, 
    bool shift, 
    bool alt
)
{
#ifdef RAD_DEBUG
    //set the bool so that OnChar() will not process the event as well...
    mInputHandled = true;

    //see if the ctrl key was pressed
    if (ctrl)
    {
        switch (virtualKey)
        {
            //cut
            case 'X':
	            p3d::printf("DEBUG cut not yet implemented.");
                break;

            //copy:
            case 'C':
	            p3d::printf("DEBUG copy not yet implemented.");
                break;

            //paste:
            case 'V':
	            p3d::printf("DEBUG paste not yet implemented.");
                break;
        }
    }
    else
    {
        switch (virtualKey)
        {
            case VK_RETURN:
                GetConsole()->Printf("=>%s", mConsoleHistory[0]);
	            GetConsole()->Evaluate(mConsoleHistory[0], "CMDLine");

                //only copy the current console history if we have a non-empty line
                if (mConsoleHistory[0][0] != '\0')
                {
                    //don't duplicate exact history entries...
                    if (mLastHistoryIndex == 0 || smStricmp(mConsoleHistory[0], mConsoleHistory[mLastHistoryIndex]))
                    {
                        //copy the current console history[0] into the next history buffer
                        mLastHistoryIndex++;
                        if (mLastHistoryIndex >= CONSOLE_HISTORY_SIZE)
                            mLastHistoryIndex = 1;
                        strcpy(mConsoleHistory[mLastHistoryIndex], mConsoleHistory[0]);

                        //update the "firstHistory" buffer index
                        if (mFirstHistoryIndex == 0)
                            mFirstHistoryIndex = 1;
                        else if (mLastHistoryIndex == mFirstHistoryIndex)
                            mFirstHistoryIndex++;
                    }
                }

                //reset the "getHistory" index
                mGetHistoryIndex = 0;

                //reset the current buffer
                mConsoleHistory[0][0] = '\0';

                //reset the cursor position
                mCursorPosition = 0;

                //reset the tab completion string
                mTabCompleteString = "";

                break;

            case VK_BACK:
                //delete from the current cursor position
                if (mCursorPosition > 0)
                {
                    //copy from the cursor position into a temp buffer
                    char temp[Console::MAX_STRING_LENGTH];
                    strcpy(temp, &mConsoleHistory[0][mCursorPosition]);

                    //decriment the cursor position, and copy the string back
                    mCursorPosition--;
                    strcpy(&mConsoleHistory[0][mCursorPosition], temp);

                    //reset the tab completion string
                    mTabCompleteString = "";
                }

                break;

            case VK_DELETE:
                //delete the character at the cursor position
                if( mCursorPosition < static_cast<int>(strlen(mConsoleHistory[0])) )
                {
                    //copy from the cursor position + 1 into a temp buffer
                    char temp[Console::MAX_STRING_LENGTH];
                    strcpy(temp, &mConsoleHistory[0][mCursorPosition + 1]);

                    //and copy the string back to the cursor position
                    strcpy(&mConsoleHistory[0][mCursorPosition], temp);

                    //reset the tab completion string
                    mTabCompleteString = "";
                }
                break;

            case VK_UP:
                //retrieve the previous available history
                if (mGetHistoryIndex == 0)
                    mGetHistoryIndex = mLastHistoryIndex;
                else
                {
                    //see if we've run out of history
                    if (mGetHistoryIndex == mFirstHistoryIndex)
                        break;

                    mGetHistoryIndex--;
                    if (mGetHistoryIndex <= 0)
                        mGetHistoryIndex = CONSOLE_HISTORY_SIZE - 1;
                }

                //now copy the history into the "current" entry buffer
                strcpy(mConsoleHistory[0], mConsoleHistory[mGetHistoryIndex]);

                //set the new cursor position
                mCursorPosition = strlen(mConsoleHistory[0]);

                //reset the tab completion string
                mTabCompleteString = "";

                break;

            case VK_DOWN:
                //retrieve the next available history
                if (mGetHistoryIndex == 0 || mGetHistoryIndex == mLastHistoryIndex)
                    break;

                mGetHistoryIndex++;
                if (mGetHistoryIndex >= CONSOLE_HISTORY_SIZE)
                    mGetHistoryIndex = 1;

                //now copy the history into the "current" entry buffer
                strcpy(mConsoleHistory[0], mConsoleHistory[mGetHistoryIndex]);

                //set the new cursor position
                mCursorPosition = strlen(mConsoleHistory[0]);

                //reset the tab completion string
                mTabCompleteString = "";

                break;

            case VK_LEFT:
                if (mCursorPosition > 0)
                    mCursorPosition--;
                break;

            case VK_RIGHT:
                if (mCursorPosition < static_cast<int>(strlen(mConsoleHistory[0])) - 1)
                    mCursorPosition++;
                break;

            case VK_HOME:
                mCursorPosition = 0;
                break;

            case VK_END:
                mCursorPosition = static_cast<int>(strlen(mConsoleHistory[0]));
                break;

            case VK_ESCAPE:
                //reset the "getHistory" index
                mGetHistoryIndex = 0;

                //reset the current buffer
                mConsoleHistory[0][0] = '\0';

                //reset the cursor position
                mCursorPosition = 0;

                //reset the tab completion string
                mTabCompleteString = "";

                break;

            case VK_TAB:
            {
                //make sure we have *something* to tabcomplete
                if (mConsoleHistory[0][0])
                {
                    //initialize the tab search
                    if ( mTabCompleteString.GetUID() != static_cast< tUID >( 0 ) )
                    {
                        mTabCompleteString = mConsoleHistory[ 0 ];
                        mTabCurrentFunction = "";
                    }

                    const tNameInsensitive& tabCompleteFunction = GetConsole()->TabCompleteFunction(mTabCompleteString.GetText(), mTabCurrentFunction.GetText() );
                    if( tabCompleteFunction.GetUID() != static_cast< tUID >( 0 ) )
                    {
                        //copy the tabCompleteFunction
                        mTabCurrentFunction  = tabCompleteFunction;

                        //create the new console entry string
		                char tabConsoleEntry[Console::MAX_STRING_LENGTH];
                        sprintf(tabConsoleEntry, "%s();", tabCompleteFunction.GetText() );
                        strcpy(mConsoleHistory[0], tabConsoleEntry);
                        mCursorPosition = strlen(mConsoleHistory[0]) - 2;
                    }
                }

                break;
            }

            default:
                //set the bool to allow OnChar() to handling this event
                mInputHandled = false;
                break;
        }
    }

    //set the new debug console line
    if (mInputHandled)
    {
        GetConsole()->SetDebugConsoleEntry(mConsoleHistory[0], mCursorPosition);
    }
#endif //RAD_DEBUG
}


//==============================================================================
// DebugConsoleCallback::OnChar
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void DebugConsoleCallback::OnChar(int asciiKey)
{
#ifdef RAD_DEBUG
    //if the bool is set, OnVKey already handled the event
    if (mInputHandled)
        return;

    //anything not handled by OnVKey will fall through and be handled here...
    int length = strlen(mConsoleHistory[0]);
    if (asciiKey >= 0x20 && length < Console::MAX_STRING_LENGTH - 1)
    {
        //add the character to the console entry at the cursor position
        char temp[Console::MAX_STRING_LENGTH];
        strcpy(temp, &mConsoleHistory[0][mCursorPosition]);

        //insert the character and increment the cursor position
        mConsoleHistory[0][mCursorPosition++] = asciiKey;
        strcpy(&mConsoleHistory[0][mCursorPosition], temp);

        //reset the tab completion string
        mTabCompleteString = "";

        //set the new debug console line
        GetConsole()->SetDebugConsoleEntry(mConsoleHistory[0], mCursorPosition);
    }
#endif //RAD_DEBUG
}


void DebugConsoleCallback::OnButtonClick( int xTextPixels, int yTextPixels, int xScreenPixels, int yScreenPixels, bool ctrl, bool shift, bool alt )
{
}
void DebugConsoleCallback::OnButtonDown( int xTextPixels, int yTextPixels, int xScreenPixels, int yScreenPixels, bool ctrl, bool shift, bool alt )
{
}
void DebugConsoleCallback::OnButtonUp( int xTextPixels, int yTextPixels, int xScreenPixels, int yScreenPixels, bool ctrl, bool shift, bool alt )
{
}
void DebugConsoleCallback::OnButtonMove( int xTextPixels, int yTextPixels, int xScreenPixels, int yScreenPixels, bool ctrl, bool shift, bool alt, bool bottondown )
{
}
