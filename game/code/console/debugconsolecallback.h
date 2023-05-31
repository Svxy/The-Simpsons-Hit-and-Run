//==============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        debugconsolecallback.h
//
// Description: 
//
// History:     05/06/2002 + Created. Thanks to Mirth for this. -- Darwin Chau
//
//==============================================================================

#ifndef DEBUG_CONSOLE_CALLBACK_H
#define DEBUG_CONSOLE_CALLBACK_H

//========================================
// Nested Includes
//========================================
// Radcore
#include <raddebugconsole.hpp>
#include <console/console.h>

//========================================
// Forward References
//========================================

//==============================================================================
//
// Synopsis: Implement the callbacks so our console integrates with the
//           remote input support from radcore.
//
//==============================================================================
class DebugConsoleCallback : public IRadDebugConsoleKeyboardInputCallback,
                             public IRadDebugConsolePointerInputCallback
{
	public:

		DebugConsoleCallback();
		~DebugConsoleCallback();

        // Implement IRadDebugConsoleKeyboardInputCallback
        void OnChar( int asciiKey );
        void OnVKey( int virtualKey, bool ctrl, bool shift, bool alt );
		
		// Implement IRadDebugConsolePointerInputCallback
        void OnButtonClick( int xTextPixels, int yTextPixels, int xScreenPixels, int yScreenPixels, bool ctrl, bool shift, bool alt );
        void OnButtonDown( int xTextPixels, int yTextPixels, int xScreenPixels, int yScreenPixels, bool ctrl, bool shift, bool alt );
        void OnButtonUp( int xTextPixels, int yTextPixels, int xScreenPixels, int yScreenPixels, bool ctrl, bool shift, bool alt );
        void OnButtonMove( int xTextPixels, int yTextPixels, int xScreenPixels, int yScreenPixels, bool ctrl, bool shift, bool alt, bool bottondown );

		const char* GetConsoleEntry( int* cursorPosition )
		{
			*cursorPosition = mCursorPosition;
			return( mConsoleHistory[0] );
		}

	private:
		
        enum{ CONSOLE_HISTORY_SIZE = 64 };
		
        char mConsoleHistory[CONSOLE_HISTORY_SIZE][Console::MAX_STRING_LENGTH];
		int mFirstHistoryIndex;
		int mLastHistoryIndex;
		int mGetHistoryIndex;
		int mCursorPosition;
		
		// Used to let events not handled by OnVKey() be passed to OnChar()
		bool mInputHandled;

		// Tab completion vars
		tNameInsensitive mTabCompleteString;
		tNameInsensitive mTabCurrentFunction ;
};

#endif // DEBUG_CONSOLE_CALLBACK_H