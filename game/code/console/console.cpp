//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        console.cpp
//
// Description: 
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
// Radcore
#include <radDebug.hpp>
#include <raddebugwatch.hpp>
#include <radFile.hpp>
// Pure3D
#include <p3d/utility.hpp>


//========================================
// Project Includes
//========================================
#include <console/console.h>

#include <console/fbstricmp.h>
#include <main/commandlineoptions.h>
#include <memory/srrmemory.h>
#include <memory/memorypool.h>
#include <loading/loadingmanager.h>

#ifdef RAD_DEBUG
#include <raddebugconsole.hpp>
#include <console/debugconsolecallback.h>
#endif

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

#define PROFILE_PARSER 0

const char* gErrFileName = 0;
unsigned int gErrLineNum = ~0;

//---------------------------------------------------------------------------//

// Static pointer to instance of singleton.
Console* Console::spInstance = 0;

static char gConsoleEntry[Console::MAX_STRING_LENGTH];

//log message buffers needed so the async writes to the log file don't bog
static char* gConsoleMsgBuffer[2] = {0};
static char* gConsoleMsgBufferPtr = 0;
static int gConsoleBufferIndex = 0;

#ifdef RAD_GAMECUBE
FBMemoryPool Console::FunctionTableEntry::sMemoryPool( sizeof(Console::FunctionTableEntry), Console::MAX_FUNCTIONS, GMA_GC_VMM );
FBMemoryPool Console::AliasTableEntry::sMemoryPool( sizeof(Console::AliasTableEntry), Console::MAX_ALIASES, GMA_GC_VMM );
#else
FBMemoryPool Console::FunctionTableEntry::sMemoryPool( sizeof(Console::FunctionTableEntry), Console::MAX_FUNCTIONS, GMA_PERSISTENT );
FBMemoryPool Console::AliasTableEntry::sMemoryPool( sizeof(Console::AliasTableEntry), Console::MAX_ALIASES, GMA_PERSISTENT );
#endif

//=============================================================================
// static bool cConsoleTrue
//=============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//=============================================================================
static bool cConsoleTrue( int argc, char** argv )
{
    argc;
    argv;
    return( true );
}

static bool cConsoleFalse( int argc, char** argv )
{
    argc;
    argv;
    return( false );
}

static void cConsoleExit(int argc, char **argv)
{
    argc;
    argv;
    //does nothing, but the parser will terminate executing a script
}

static void cConsoleExec(int argc, char **argv)
{
    GetConsole()->ExecuteScriptSync(argv[1], true);
}

static void cConsoleSetLogMode(int argc, char **argv)
{
    argc;
    if (! smStricmp(argv[1], "on"))
        GetConsole()->SetConsoleLogMode(Console::LOGMODE_ON);
    else if (! smStricmp(argv[1], "append"))
        GetConsole()->SetConsoleLogMode(Console::LOGMODE_APPEND);
    else
        GetConsole()->SetConsoleLogMode(Console::LOGMODE_OFF);
}

static void cConsoleFlushLog(int argc, char **argv)
{
    GetConsole()->FlushConsoleLogFile();
}

static void cConsoleEcho(int argc, char **argv)
{
    if (argc == 3)
        GetConsole()->Printf(atoi(argv[1]), argv[2]);
    else
        GetConsole()->Printf(argv[1]);
}

static void cConsoleError(int argc, char **argv)
{
    GetConsole()->Errorf(argv[1]);
}

static void cConsoleListFunctions(int argc, char **argv)
{
    argc;
    argv;
    GetConsole()->ListConsoleFunctions();
}

static void cConsoleListenChannel(int argc, char **argv)
{
    argc;
    GetConsole()->ConsoleListenChannel(atoi(argv[1]));
}

static void cConsoleBlockChannel(int argc, char **argv)
{
    argc;
    GetConsole()->ConsoleBlockChannel(atoi(argv[1]));
}

static void cConsoleAlias(int argc, char **argv)
{
    GetConsole()->AddAlias(argv[1], argv[2], argc - 3, &argv[3]);
}

static void cWatcherExecConsoleCommand(void *)
{
    GetConsole()->Printf("==> %s", gConsoleEntry);
    GetConsole()->Evaluate(gConsoleEntry, "CMDLine");
}


//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// Console::CreateInstance
//==============================================================================
//
// Description: Creates the Console.
//
// Parameters:  None.
//
// Return:      Pointer to the Console.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
Console* Console::CreateInstance()
{
MEMTRACK_PUSH_GROUP( "Console" );
    rTuneAssert( spInstance == 0 );

    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_PERSISTENT );
    #endif

    spInstance = new Console;
    rTuneAssert( spInstance );

    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_PERSISTENT );
    #endif
   
    Console::Initialize();
MEMTRACK_POP_GROUP( "Console" );

    return spInstance;
}


//==============================================================================
// Console::GetInstance
//==============================================================================
//
// Description: - Access point for the Console singleton.  
//
// Parameters:  None.
//
// Return:      Pointer to the Console.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
Console* Console::GetInstance()
{
    rTuneAssert( spInstance != 0 );
    
    return spInstance;
}


//==============================================================================
// Console::DestroyInstance
//==============================================================================
//
// Description: Destroy the Console.
//
// Parameters:  None.
//
// Return:      None.
//
//==============================================================================
void Console::DestroyInstance()
{
    rTuneAssert( spInstance != 0 );

    delete spInstance;
    spInstance = 0;
}


//==============================================================================
// Console::Initialize
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
bool Console::Initialize()
{
//the console doesn't use remote drive in the release build
#ifdef RAD_DEBUG
MEMTRACK_PUSH_GROUP( "Console Initialize" );

    if( !CommandLineOptions::Get( CLO_FIREWIRE ) )
    {
        //initialize the console log buffers
        gConsoleMsgBuffer[0] = new(GMA_DEBUG) char[MAX_STRING_LENGTH * MAX_BUFFERS];
        gConsoleMsgBuffer[1] = new(GMA_DEBUG) char[MAX_STRING_LENGTH * MAX_BUFFERS];
        gConsoleMsgBufferPtr = gConsoleMsgBuffer[0];
        gConsoleBufferIndex = 0;

        //add a function to allow the program to generate a console.log file
        GetConsole()->AddFunction("ifTrue", cConsoleTrue, "ifTrue() { } //returns true", 0, 0);
        GetConsole()->AddFunction("ifFalse", cConsoleFalse, "ifFalse() { } //returns false", 0, 0);
        GetConsole()->AddFunction("Exit", cConsoleExit, "Exit(msg);", 1, 1);
        GetConsole()->AddFunction("ConsoleExec", cConsoleExec, "ConsoleExec(file);", 1, 1);
        GetConsole()->AddFunction("ConsoleSetLogMode", cConsoleSetLogMode, "ConsoleSetLogMode(off/on/append);", 1, 1);
        GetConsole()->AddFunction("ConsoleFlushLog", cConsoleFlushLog, "ConsoleFlushLog();", 0, 0);
        GetConsole()->AddFunction("Echo", cConsoleEcho, "Echo([channel, ] msg);", 1, 2);
        GetConsole()->AddFunction("Error", cConsoleError, "Error(msg);", 1, 1);
        GetConsole()->AddFunction("ConsoleListFunctions", cConsoleListFunctions, "ConsoleListFunctions();", 0, 0);
        GetConsole()->AddFunction("ConsoleListenChannel", cConsoleListenChannel, "ConsoleListenChannel(-1 or [0 .. 31]);", 1, 1);
        GetConsole()->AddFunction("ConsoleBlockChannel", cConsoleBlockChannel, "ConsoleBlockChannel(-1 or [0 .. 31]);", 1, 1);
        GetConsole()->AddFunction("Alias", cConsoleAlias, "Alias(aliasName, functionName, arg1, arg2, \"%1\", arg3, \"%2\", etc...);", 2, MAX_ARGS);

        //add the console entry string and it's update function
        radDbgWatchAddString(gConsoleEntry, sizeof(gConsoleEntry), "Console command", "\\", cWatcherExecConsoleCommand);

        //add the debugconsole window as well...
        radDebugConsoleCreate(&(spInstance->mDebugConsole), GMA_DEBUG);

        if (spInstance->mDebugConsole)
        {
            //set the title and background color
            spInstance->mDebugConsole->SetTitle( "Fish Bulb Console" );
            spInstance->mDebugConsole->SetBackgroundColor(RGB( 0, 0, 0x188 ));  
            spInstance->mDebugConsole->Clear();

            spInstance->mDebugConsole->SetCursorPosition(0, 0);
            spInstance->mDebugConsole->SetTextColor(RGB(0xFF, 0xFF,0xFF));

            spInstance->mDebugConsoleCallback = new(GMA_DEBUG) DebugConsoleCallback();
            spInstance->mDebugConsole->SetKeyboardInputCallback(spInstance->mDebugConsoleCallback);
            spInstance->mDebugConsole->SetPointerInputCallback(spInstance->mDebugConsoleCallback);

            //initialize the debug console line number
            spInstance->mDebugConsoleLine = 0;
        }
    }
MEMTRACK_POP_GROUP( "Console Initialize" );
#endif

    return (true);
}


//==============================================================================
// Console::Console
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
Console::Console()
:
mpCallback( 0 )
{
    // Initialize the tables
    int i = 0;
    for( i = 0; i < MAX_FUNCTIONS; ++i )
    {
        mFunctionTable[i] = 0;
    }

    for( i = 0; i < MAX_ALIASES; ++i )
    {
        mAliasTable[i] = 0;
    }

    mFunctionCount = 0;
    mAliasCount = 0;


    //create the argv array
    #ifdef RAD_GAMECUBE
        HeapMgr()->PushHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PushHeap( GMA_PERSISTENT );
    #endif

    mArgv = new char*[MAX_ARGS];
    mPrevArgv = new char*[MAX_ARGS];
    
    for( i = 0; i < MAX_ARGS; ++i )
    {
        mArgv[i] = new char[MAX_ARG_LENGTH];
        mPrevArgv[i] = new char[MAX_ARG_LENGTH];
    }

    #ifdef RAD_GAMECUBE
        HeapMgr()->PopHeap( GMA_GC_VMM );
    #else
        HeapMgr()->PopHeap( GMA_PERSISTENT );
    #endif
    //set the log file mode
    mLogFile = 0;
    mLogMode = LOGMODE_OFF;

    //initialize the console listen channels to everything
    mChannelMask = ~0;

#ifdef RAD_DEBUG
    //initialize the radcore debugconsole members
    mDebugConsole = 0;
#endif
}



//==============================================================================
// Console::~Console
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
Console::~Console()
{
    //clean up the console
    if( spInstance )
    {
#ifdef RAD_DEBUG
        if( spInstance->mDebugConsole )
        {
            spInstance->mDebugConsole->Release();
            delete spInstance->mDebugConsoleCallback;
        }
#endif

        if( spInstance->mLogFile )
            spInstance->mLogFile->WaitForCompletion();
    }

    //clean up after the watcher
#ifdef RAD_DEBUG
    delete [] gConsoleMsgBuffer[0];
    delete [] gConsoleMsgBuffer[1];
    radDbgWatchDelete(gConsoleEntry);
#endif
    
    int i = 0;

    //delete the tables
    for( i = 0; i < mFunctionCount; ++i )
    {
        if (mFunctionTable[i])
        {
            delete mFunctionTable[i];
        }
        
    }

    for( i = 0; i < MAX_ALIASES; ++i )
    {
        if( mAliasTable[i] )
        {
            delete mAliasTable[i];
        }
        
    }


    //delete the argv array
    for( i = 0; i < MAX_ARGS; ++i )
    {
        delete [] mArgv[i];
        delete [] mPrevArgv[i];
    }
        
    delete [] mArgv;
    delete [] mPrevArgv;


    //close the logfile
    if( mLogFile )
    {
        FlushLogFile();
        mLogFile->Release();
    }
}


//==============================================================================
// Console::SetConsoleLogMode
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
bool Console::SetConsoleLogMode(int logMode)
{
#ifndef RAD_DEBUG
    return (false);
#else
    //make sure we have a console
    if (!spInstance)
        return (false);

    return (spInstance->SetLogMode(logMode));
#endif
}

bool Console::SetLogMode(int logMode)
{
    //no work to do if we're not actually changing the mode
    if (logMode == mLogMode)
        return (true);

    switch (logMode)
    {
        case LOGMODE_OFF:
            if (mLogFile)
            {
                mLogFile->WaitForCompletion();
                mLogFile->Release();
                mLogFile = 0;
            }
            mLogMode = logMode;
            return (true);

        case LOGMODE_ON:
            if (!mLogFile)
            {
                char fullPath[256];
// RM                sprintf(fullPath, "%sconsole.log", ResourceManager::GetInstance()->GetRemoteDriveName());
                ::radFileOpen(&mLogFile, fullPath, true, CreateAlways, NormalPriority, 0, RADMEMORY_ALLOC_TEMP);
                if (mLogFile)
                {
                    const char *msg = "\r\n******** CONSOLE LOG OPENED ********\r\n";
                    mLogFile->WaitForCompletion();
                    mLogFile->WriteAsync(msg, strlen(msg)); 
                    mLogFile->WaitForCompletion();
                    mLogMode = logMode;
                    return (true);
                }
                else
                {
                    mLogMode = LOGMODE_OFF;
                    return (false);
                }
            }
            return (true);

        case LOGMODE_APPEND:
            if (!mLogFile)
            {
                char fullPath[256];
// RM               sprintf(fullPath, "%sconsole.log", ResourceManager::GetInstance()->GetRemoteDriveName());
                ::radFileOpen(&mLogFile, fullPath, true, OpenAlways, NormalPriority, 0, RADMEMORY_ALLOC_TEMP);
                if (mLogFile)
                {
                    const char *msg = "\r\n******** CONSOLE LOG OPENED ********\r\n";
                    mLogFile->WaitForCompletion();
                    int fileSize = mLogFile->GetSize();
                    mLogFile->SetPositionAsync(fileSize);
                    mLogFile->WaitForCompletion();
                    mLogFile->WriteAsync(msg, strlen(msg)); 
                    mLogFile->WaitForCompletion();
                    mLogMode = logMode;
                    return (true);
                }
                else
                {
                    mLogMode = LOGMODE_OFF;
                    return (false);
                }
            }
            return (true);
    }

    // Set Error Condition, to remove compiler warning.
    return (false);
}


//==============================================================================
// Console::Printf
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Console::Printf(char *fmt, ...)
{
#ifdef RAD_DEBUG
    //make sure we have a console
    if (!spInstance)
        return;

    char message[MAX_STRING_LENGTH];
    va_list args;
    int i = 0;
    va_start(args, fmt);
    i = vsprintf(message, fmt, args);
    va_end(args);

    // do some tune printf for designers
    rTunePrintf( message );

    //record the message to the console log file
    spInstance->LogMessage(0, message);
#endif
}

//for now, it does the same as Printf()...  should also echo
//to an error page in the DebugInfo stuff Chakib is working on!
void Console::Errorf(char *fmt, ...)
{
#ifdef RAD_DEBUG
    //make sure we have a console
    if (!spInstance)
        return;

    char message[MAX_STRING_LENGTH];
    va_list args;
    int i = 0;
    va_start(args, fmt);
    i = vsprintf(message, fmt, args);
    va_end(args);

    //record the message to the console log file
    spInstance->LogMessage(0, message, true);
#endif
}

void Console::Printf(int channel, char *fmt, ...)
{
#ifdef RAD_DEBUG
    //make sure we have a console
    if (!spInstance)
        return;

    char message[MAX_STRING_LENGTH];

    //prefix the message with the channel number
    sprintf(message, "ch%d ", channel);
    char *msgPtr;
    if (channel >= 10)
        msgPtr = &message[5];
    else
        msgPtr = &message[4];

    va_list args;
    int i = 0;
    va_start(args, fmt);
    i = vsprintf(msgPtr, fmt, args);
    va_end(args);

    // do some tune printf for designers
    rTunePrintf( message );

    //record the message to the console log file
    spInstance->LogMessage(channel, message);
#endif
}


//==============================================================================
// Console::AssertFatal
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Console::AssertFatal(char *fmt, ...)
{
    char msgFmt[512];
    sprintf(msgFmt, "File: %s, line: %d  %s", gErrFileName, gErrLineNum, fmt);
    char message[MAX_STRING_LENGTH];
    va_list args;
    int i = 0;
    va_start(args, fmt);
    i = vsprintf(message, msgFmt, args);
    va_end(args);

    //record the message to the console log file
    this->LogMessage(0, message, true);

    //flush the log file in case we're about to crash
    this->FlushLogFile();

    //pop the assert
    rTuneAssertMsg( 0, message );
}


//==============================================================================
// Console::AssertWarn
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Console::AssertWarn(char *fmt, ...)
{
    char msgFmt[512];
    sprintf(msgFmt, "File: %s, line: %d  %s", gErrFileName, gErrLineNum, fmt);
    char message[MAX_STRING_LENGTH];
    va_list args;
    int i = 0;
    va_start(args, fmt);
    i = vsprintf(message, msgFmt, args);
    va_end(args);

    //record the message to the console log file
    spInstance->LogMessage(0, message, true);

    //flush the log file in case we're about to crash
    spInstance->FlushLogFile();

    //assert warn using the p3d versions
    rDebugWarningFail_Implementation(message, gErrFileName, gErrLineNum);
}


//==============================================================================
// Console::LogMessage
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Console::LogMessage(int channel, char* msg, bool warning)
{
    //see if we're listening to this channel
    int listenChannel = channel;
    if (channel < 0 || channel > 31)
        listenChannel = 0;
    if (! (mChannelMask & (1 << listenChannel)))
        return;

    //append an '\r\n' to the end of every message
    char logMessage[512];
    if (strchr(msg, '\n'))
        strcpy(logMessage, msg);
    else
        sprintf(logMessage, "%s\r\n", msg);

    //print the message to the console
    p3d::printf(logMessage);

#ifdef RAD_DEBUG
    //print the message out the the radcore debug console as well...
    AddDebugConsoleLine(logMessage, warning);
#endif

    //write the msg out to the console.log (buffered)
    if (mLogMode == LOGMODE_ON || mLogMode == LOGMODE_APPEND)
    {
        //see if there's enough room in the buffer to store the string
        int bytesUsed = int(gConsoleMsgBufferPtr) - int(gConsoleMsgBuffer[gConsoleBufferIndex]);
        int bytesLeft = (MAX_STRING_LENGTH * MAX_BUFFERS) - bytesUsed;
        int strLength = strlen(logMessage);

        //if we don't have space, wait for file completion, then start using the next buffer
        if (bytesUsed > 0 && bytesLeft < strLength)
        {
            mLogFile->WaitForCompletion();
            mLogFile->WriteAsync(gConsoleMsgBuffer[gConsoleBufferIndex], bytesUsed);
            gConsoleBufferIndex = 1 - gConsoleBufferIndex;
            gConsoleMsgBufferPtr = gConsoleMsgBuffer[gConsoleBufferIndex];
        }

        //copy the string into the static console msg buffer
        strcpy(gConsoleMsgBufferPtr, logMessage);

        //increment the console msg buffer
        gConsoleMsgBufferPtr += strLength;
    }
}


//==============================================================================
// Console::FlushConsoleLogFile
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Console::FlushConsoleLogFile()
{
    //flush the log file in case we're about to crash
    spInstance->FlushLogFile();
}


//==============================================================================
// Console::FlushLogFile
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Console::FlushLogFile()
{
    if (mLogMode == LOGMODE_ON || mLogMode == LOGMODE_APPEND)
    {
        //see if there's enough room in the buffer to store the string
        int bytesUsed = int(gConsoleMsgBufferPtr) - int(gConsoleMsgBuffer[gConsoleBufferIndex]);

        //if there is info waiting in the buffer to be written out...
        if (bytesUsed > 0)
            mLogFile->WriteAsync(gConsoleMsgBuffer[gConsoleBufferIndex], bytesUsed);

        //wait for the File server to complete
        mLogFile->WaitForCompletion();

        //reset the buffer vars
        gConsoleBufferIndex = 0;
        gConsoleMsgBufferPtr = gConsoleMsgBuffer[0];
    }
}

#ifdef RAD_DEBUG
//debug console methods


//==============================================================================
// char* FormatDebugConsoleString
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================

const unsigned char MAX_STRING_LEN = 128;

static const char* FormatDebugConsoleString(const char *text, bool consoleEntry)
{
    static char formatString[MAX_STRING_LEN + 1];

    //copy the first 80 characters of the text
    if (consoleEntry)
    {
        formatString[0] = '=';
        formatString[1] = '>';
        strncpy(&formatString[2], text, MAX_STRING_LEN - 2);
    }
    else
        strncpy(formatString, text, MAX_STRING_LEN);

    //make sure there are no \r\n's in the string
    char *eolChar = strchr(formatString, '\r');
    if (eolChar)
        *eolChar = '\0';
    eolChar = strchr(formatString, '\n');
    if (eolChar)
        *eolChar = '\0';

    //pad the string with spaces
    formatString[MAX_STRING_LEN] = '\0';
    int strlength = strlen(formatString);
    if (strlength < MAX_STRING_LEN)
        memset(&formatString[strlength], ' ', MAX_STRING_LEN - strlength);
    formatString[MAX_STRING_LEN] = '\0';

    //return the formatted string
    return(formatString);
}


//==============================================================================
// Console::AddDebugConsoleLine
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Console::AddDebugConsoleLine(const char *text, bool warning)
{
    //make sure we have a debugconsole
    if (! mDebugConsole)
        return;

    //create the string to send to the debug console
    const char *formatString = FormatDebugConsoleString(text, false);
    if (warning)
        mDebugConsole->SetTextColor(RGB(0xFF, 0, 0));
    else
        mDebugConsole->SetTextColor(RGB(0xFF, 0xFF, 0xFF));
    mDebugConsole->TextOutAt(formatString, 0, mDebugConsoleLine++);

    //create the console entry string
    int cursorPosition;
    const char *consoleEntry = mDebugConsoleCallback->GetConsoleEntry(&cursorPosition);
    SetDebugConsoleEntry(consoleEntry, cursorPosition);
}


//==============================================================================
// Console::SetDebugConsoleEntry
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Console::SetDebugConsoleEntry(const char *text, int cursorPosition)
{
    const char *formatString = FormatDebugConsoleString(text, true);

    //update the consoleEntry
    mDebugConsole->SetTextColor(RGB(0xFF, 0xFF, 0xFF));
    mDebugConsole->TextOutAt(formatString, 0, mDebugConsoleLine);

    //update the cursor position
    spInstance->mDebugConsole->SetCursorPosition(cursorPosition + 2, mDebugConsoleLine);
}

#endif


//==============================================================================
// Console::ConsoleListenChannel
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Console::ConsoleListenChannel(int channel)
{
    //make sure we have a console
    if (!spInstance)
        return;

    if (channel <= -2 || channel > 31)
    {
        Printf("Console error - invalid channel: %d.  Specify -1 for all channels, or 0 to 31", channel);
        return;
    }

    spInstance->ListenChannel(channel);
}


//==============================================================================
// Console::ConsoleBlockChannel
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Console::ConsoleBlockChannel(int channel)
{
    //make sure we have a console
    if (!spInstance)
        return;

    if (channel <= -2 || channel > 31)
    {
        Printf("Console error - invalid channel: %d.  Specify -1 for all channels, or 0 to 31", channel);
        return;
    }

    spInstance->BlockChannel(channel);
}


//==============================================================================
// Console::ListenChannel
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Console::ListenChannel(int channel)
{
    //validate params
    if (channel <= -2 || channel > 31)
        return;

    if (channel == -1)
        mChannelMask = 0xffffffff;
    else
        mChannelMask |= (1 << channel);
}


//==============================================================================
// Console::BlockChannel
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Console::BlockChannel(int channel)
{
    //validate params
    if (channel <= -2 || channel > 31)
        return;

    //note:  can't block channel 0
    if (channel == -1)
        mChannelMask = (1 << 0);
    else
        mChannelMask &= ~(1 << channel);
}


//==============================================================================
// Console::ListConsoleFunctions
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Console::ListConsoleFunctions()
{
    if (!spInstance)
        return;

    spInstance->ListFunctions();
}


//==============================================================================
// Console::ListFunctions
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Console::ListFunctions()
{
    //for (int i = 0; i < mFunctionCount; i++)
        //Printf(mFunctionTable[i]->help.GetText());
}


//==============================================================================
// Console::AddFunction
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
bool Console::AddFunction
(
    const char* name,
    CONSOLE_FUNCTION funcPtr,
    const char* helpString,
    int minArgs,
    int maxArgs
)
{
    //make sure we've initialized the global console
    if (!spInstance)
    {
        rDebugString("Error - Console::AddFunction():  Console::Initialize() hasn't been called yet.");
        return (false);
    }

    return (spInstance->AddFunc(name, funcPtr, 0, helpString, minArgs, maxArgs));
}


//==============================================================================
// Console::AddFunction
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
bool Console::AddFunction
(
    const char* name,
    CONSOLE_BOOL_FUNCTION funcPtr,
    const char* helpString,
    int minArgs,
    int maxArgs
)
{
    //make sure we've initialized the global console
    if (!spInstance)
    {
        rDebugString("Error - Console::AddFunction():  Console::Initialize() hasn't been called yet.");
        return (false);
    }

    return (spInstance->AddFunc(name, 0, funcPtr, helpString, minArgs, maxArgs));
}



//==============================================================================
// Console::AddFunc
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
bool Console::AddFunc
(
    const char* name,           
    CONSOLE_FUNCTION funcPtr,
    CONSOLE_BOOL_FUNCTION boolFuncPtr,
    const char* helpString,
    int minArgs,
    int maxArgs
)
{
    //validate the input
    if (!name || !name[0] || strlen(name) >= MAX_STRING_LENGTH)
    {
        rDebugString("Error - Console::AddFunction():  invalid function name specified.\n");
        return (false);
    }
    if (!helpString || strlen(helpString) >= MAX_STRING_LENGTH)
    {
        rDebugString("Error - Console::AddFunction():  invalid help string specified.\n");
        return (false);
    }
    if (!funcPtr && !boolFuncPtr)
    {
        rDebugString("Error - Console::AddFunction():  no CONSOLE_FUNCTION specified.\n");
        return (false);
    }
    if (minArgs < 0)
    {
        rDebugString("Error - Console::AddFunction():  invalid minArgs specified.\n");
        return (false);
    }
    if (maxArgs < minArgs || maxArgs > MAX_ARGS)
    {
        rDebugString("Error - Console::AddFunction():  invalid maxArgs specified.\n");
        return (false);
    }

    //make sure we have room for one more
    if( mFunctionCount >= MAX_FUNCTIONS )
    {
        rDebugString("Error - Console::AddFunction():  Maximum number of functions has been reached.\n");
        return (false);
    }
    
#ifdef RAD_GAMECUBE
    HeapMgr()->PushHeap( GMA_GC_VMM );
#else
    HeapMgr()->PushHeap( GMA_PERSISTENT );
#endif
   
    //create the new function
    FunctionTableEntry* newFunction;
    if( funcPtr )
    {
        newFunction = new FunctionTableEntry(name, funcPtr, helpString, minArgs, maxArgs);
    }
    else
    {
        newFunction = new FunctionTableEntry(name, boolFuncPtr, helpString, minArgs, maxArgs);
    }

    //add the function to the table
    mFunctionTable[mFunctionCount] = newFunction;
    mFunctionLookup[radMakeCaseInsensitiveKey32(name)] = mFunctionCount;

    //increment the function count
    mFunctionCount++;

#ifdef RAD_GAMECUBE
    HeapMgr()->PopHeap( GMA_GC_VMM );
#else
    HeapMgr()->PopHeap( GMA_PERSISTENT );
#endif

    // return success condition
    return (true);
}


//==============================================================================
// Console::AddAlias
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
bool Console::AddAlias
(
    const char* name, 
    const char* functionName, 
    int argc, 
    char** argv
)
{
    //make sure we've initialized the global console
    if (!spInstance)
    {
        rDebugString("Error - Console::AddFunction():  Console::Initialize() hasn't been called yet.");
        return (false);
    }

    return (spInstance->AddFunctionAlias(name, functionName, argc, argv));
}


//==============================================================================
// Console::AddFunctionAlias
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
bool Console::AddFunctionAlias
(
    const char* name, 
    const char* functionName, 
    int argc, 
    char** argv 
)
{
    //validate the input
    if (!name || !name[0] || strlen(name) >= MAX_STRING_LENGTH)
    {
        rDebugString("Error - Console::AddFunctionAlias():  invalid function name specified.\n");
        return (false);
    }

    //make sure we have room for one more
    if (mAliasCount >= MAX_FUNCTIONS)
    {
        rDebugString("Error - Console::AddFunctionAlias():  Maximum number of alias' has been reached.\n");
        return (false);
    }
    
#ifdef RAD_GAMECUBE
    HeapMgr()->PushHeap( GMA_GC_VMM );
#else
    HeapMgr()->PushHeap( GMA_PERSISTENT );
#endif

    //create the new alias
    AliasTableEntry* newAlias = new AliasTableEntry(name, functionName, argc, argv);

    //insert the new alias into the table alphabetically
    int index = 0;
    while (index < mAliasCount)
    {
        if (smStricmp(mAliasTable[index]->name, name) > 0)
            break;

        index++;
    }

    //the new alias should be inserted at position "index", shift everything else down by one
    for (int i = mAliasCount; i > index; i--)
        mAliasTable[i] = mAliasTable[i - 1];

    //add the alias to the table
    mAliasTable[index] = newAlias;

    //increment the alias count
    mAliasCount++;

#ifdef RAD_GAMECUBE
    HeapMgr()->PopHeap( GMA_GC_VMM );
#else
    HeapMgr()->PopHeap( GMA_PERSISTENT );
#endif

    // return success condition
    return (true);
}


//==============================================================================
// Console::TabCompleteFunction
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
const tNameInsensitive& Console::TabCompleteFunction
(
    const char* tabString, 
    const char* curFunction
)
{
    static tNameInsensitive error( "ERROR" );
    //make sure we've initialized the global console
    if (!spInstance)
    {
        rDebugString("Error - Console::TabCompleteFunction():  Console::Initialize() hasn't been called yet.");
        return error;
    }

    return (spInstance->TabComplete(tabString, curFunction));
}


//==============================================================================
// Console::TabComplete
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
const tNameInsensitive& Console::TabComplete
(
    const char* tabString, 
    const char* curFunction
)
{
    int i;
    static tNameInsensitive error( "ERROR" );
    //sanity check
    if (! tabString || ! tabString[0])
    {
        rAssert( false );
        return error;
    }

    //find the first function beginning with the tabString
    //note - functions are stored alphabetically...
    int firstFunctionIndex = -1;
    for (i = 0; i < mFunctionCount; i++)
    {
        if (! smStrincmp(tabString, mFunctionTable[i]->name.GetText(), strlen(tabString)))
        {
            firstFunctionIndex = i;
            break;
        }
    }

    //if no function was found, return 0
    if (firstFunctionIndex < 0)
    {
        rAssert( false );
        return error;
    }

    //if there's no current function, return the first function we found
    if (! curFunction || ! curFunction[0])
        return (mFunctionTable[firstFunctionIndex]->name);

    //if the current function doesn't contain the tabString, ???
    if (smStrincmp(tabString, curFunction, strlen(tabString)))
        return (mFunctionTable[firstFunctionIndex]->name);
        
    //search for the next function that contains the tabstring, but follows the curFunction
    int curFunctionIndex = -1;
    for (i = firstFunctionIndex; i < mFunctionCount; i++)
    {
        if ( mFunctionTable[i]->name == curFunction )
        {
            curFunctionIndex = i;
            break;
        }
    } 

    //if no current function was found, return the first function
    if (curFunctionIndex < 0 || curFunctionIndex == mFunctionCount - 1)
        return (mFunctionTable[firstFunctionIndex]->name);

    //see if the next function also contains the tabString
    if (! smStrincmp(tabString, mFunctionTable[curFunctionIndex + 1]->name.GetText(), strlen(tabString)))
        return (mFunctionTable[curFunctionIndex + 1]->name);

    //otherwise, return the first function found again
    else
        return (mFunctionTable[firstFunctionIndex]->name);
}

//---------------------------------------------------------------------------//

static const char* gWhiteSpace = " \t\r\n";
static const char* gEndOfToken = " \t\r\n,();[]{}+/*!@#$%^&:\"<>?'`~";

char *Console::SkipWhiteSpace(const char *string)
{
    //this function should never be called with an invalid string
    char *temp = const_cast<char*>(string);
    while (true)
    {
        //break on an end of string character
        if (*temp == '\0')
            break;

        //break when a non-white space character is found
        char *found = strchr(gWhiteSpace, *temp);
        if (! found)
            break;

        //see if we found an eol char
        if (*found == '\n')
            mLineNumber++;

        //check the next character
        temp++;
    }
    
    return (temp);
}


//==============================================================================
// Console::FoundComment
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
bool Console::FoundComment(char** stringPtr)
{
    char *tokenPtr = *stringPtr;
    if (*tokenPtr == '/')
    {
        tokenPtr++;

        //see if we've found a line comment
        if (*tokenPtr == '/')
        {
            while (*tokenPtr != '\r' && *tokenPtr != '\n' && *tokenPtr != '\0')
                tokenPtr++;

            //increment the line count
            if (*tokenPtr == '\n')
                mLineNumber++;

            //now we've hit the end of the line or file
            *stringPtr = tokenPtr;
            return (true);
        }

        //else see if we've found a block comment
        else if (*tokenPtr == '*')
        {
            //here we search for the end of the block:  */
            while (true)
            {
                tokenPtr++;
                while (*tokenPtr != '*' && *tokenPtr != '\0')
                {
                    //increment the line count
                    if (*tokenPtr == '\n')
                        mLineNumber++;

                    tokenPtr++;
                }

                //see if we've found the end of the file
                if (*tokenPtr == '\0')
                {
                    *stringPtr = tokenPtr;
                    return (true);
                }
                
                //else see if we've found the end of the comment block
                else
                {
                    tokenPtr++;
                    if (*tokenPtr == '/')
                    {
                        tokenPtr++;
                        *stringPtr = tokenPtr;
                        return (true);
                    }
                }
            }
        }

        //else no comment found
        else
            return (false);
    }

    //else no commment found
    else
        return (false);
}


//==============================================================================
// Console::FindTokenEnd
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
char* Console::FindTokenEnd( const char* string )
{
    //this function should never be called with an invalid string
    char *temp = const_cast<char*>(string);
    bool finished = false;
    while (true)
    {
        //break on an end of string character
        if (*temp == '\0')
            break;

        //break on an end of token character
        char *found = strchr(gEndOfToken, *temp);
        if (found)
        {
            //see if we found an eol char
            if (*found == '\n')
                mLineNumber++;

            break;
        }

        //neither was found, check the next character
        temp++;
    }
    
    return (temp);
}


//==============================================================================
// Console::ReadToken
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
bool Console::ReadToken(char **stringPtr, const char *requiredToken)
{
    //validate parameters
    if (! stringPtr || ! *stringPtr || !requiredToken)
        return (false);

    //read the next token
    char tempBuffer[MAX_STRING_LENGTH];
    char *token = GetNextToken(stringPtr, tempBuffer);

    //if we don't have a token, or it doesn't match what we're looking for, return false
    if (! token || smStricmp(token, requiredToken))
        return (false);

    //everything is ok, return true
    return (true);
}


//==============================================================================
// Console::GetNextToken
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
char* Console::GetNextToken(char **stringPtr, char *tokenBuffer)
{
    //validate parameters
    if (! stringPtr || ! *stringPtr || !tokenBuffer)
        return (0);

    char *tokenPtr = *stringPtr;
    char *tokenEnd = 0;

    //skip comments and white space until we reach the beginning of a valid token
    while (true)
    {
        tokenPtr = SkipWhiteSpace(tokenPtr);
        if (! FoundComment(&tokenPtr))
            break;
    }

    //if the next character is a '\0', no more tokens
    if (*tokenPtr == '\0')
        return (0);

    //if the token is enclosed in quotes, search for the end quote
    if (*tokenPtr == '\"')
    {
        tokenEnd = tokenPtr + 1;
        while (true)
        {
            tokenEnd = strchr(tokenEnd, '\"');
            if (! tokenEnd)
                return (0);

            //make sure the previous character isn't a '\\'
            if (*(tokenEnd - 1) == '\\')
                tokenEnd++;

            //otherwise, we've found our matching quotes
            else
                break;
        }

        //now we've got our string enclosed with quotes, copy it the contents to the tokenBuffer
        int strLength = (tokenEnd - tokenPtr) - 1;
        strncpy(tokenBuffer, tokenPtr + 1, strLength);
        tokenBuffer[strLength] = '\0';

        //update the next position in the string
        *stringPtr = tokenEnd + 1;
        return (tokenBuffer);
    }

    //else find the end of the token
    else
    {
        tokenEnd = FindTokenEnd(tokenPtr);

        //if 0 was returned, the token ends on a '\0'
        if (! tokenEnd)
        {
            //copy the token to the tokenBuffer
            strcpy(tokenBuffer, tokenPtr);

            //update the next position in the string
            *stringPtr = &tokenPtr[strlen(tokenPtr)];
            return (tokenBuffer);
        }

        //if the tokenEnd == the tokenPtr, the token is either a '\0', or one of the token end chars
        else if (tokenEnd == tokenPtr)
        {
            //copy the character into the tokenBuffer
            tokenBuffer[0] = *tokenPtr;
            tokenBuffer[1] = '\0';

            //update the next position in the string
            *stringPtr = tokenEnd + 1;
            return (tokenBuffer);
        } 

        else
        {
            //copy the token into the tokenBuffer
            int strLength = tokenEnd - tokenPtr;
            strncpy(tokenBuffer, tokenPtr, strLength);
            tokenBuffer[strLength] = '\0';

            //update the next position in the string
            *stringPtr = tokenEnd;
            return (tokenBuffer);
        }
    }
}


//==============================================================================
// Console::Evaluate
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
bool Console::Evaluate( const char* string, const char* fileName )
{
    rTuneAssert( string != 0 );

#if PROFILE_PARSER
    float curTime = 0.0f, elapsedTime = 0.0f, totalTime = 0.0f;
#endif

    // make sure we have a real string
    if( !string[0] )
    {
        return( false );
    }

    // intialize the file name line number
    strcpy( mFileName, fileName );
    mLineNumber = 1;

    // setup the string ptr
    char* stringPtr = const_cast<char*>(string);

    // process the string until it's finished
    int readEndBracketLevel = 0;
    bool negateCondition = false;
    
    do 
    {

#if !PROFILE_PARSER
//      Sound::daSoundManagerService( );
#endif

        // first, get the function name
        char* token = this->GetNextToken( &stringPtr, mArgv[0] );

        // if no function name found, we're finished
        if( !token )
        {
            if( readEndBracketLevel > 0 )
            {
                this->Printf( "ConERR file %s line %d - expecting '}'.", mFileName, mLineNumber );
                rTuneAssert( 0 );
                return( false );
            }
            break;
        }

        // it's possible we might read an end bracket - skip past it...
        while( !smStricmp(token, "}") )
        {
            if( readEndBracketLevel <= 0 )
            {
                this->Printf( "ConERR file %s line %d - unexpected '}' found.", mFileName, mLineNumber );
                rTuneAssert( 0 );
                return( false );
            }

            // otherwise, set the bool, and read the next token
            readEndBracketLevel--;
            token = GetNextToken( &stringPtr, mArgv[0] );

            // if no function name found, we're finished
            if( !token )
            {
                if( readEndBracketLevel > 0 )
                {
                    this->Printf("ConERR file %s line %d - expecting '}'.", mFileName, mLineNumber);
                    rTuneAssert( 0 );
                    return( false );
                }
                else
                {
                    return( true );
                }
                
            }
        }
        mArgc = 1;

        // see if we read a "!" symbol before a conditional function call...
        if( !strcmp( token, "!") )
        {
            // set the bool
            negateCondition = true;

            //read the next token (should be function name)
            token = GetNextToken(&stringPtr, mArgv[0]);

            if (! token)
            {
                Printf("ConERR file %s line %d - unexpected '!'.", mFileName, mLineNumber);
                rTuneAssert( 0 );
                return (false);
            }
        }

        //mArgv[0] currently contains the function name
        //next token should be an '('
        if (! ReadToken(&stringPtr, "("))
        {
            Printf("ConERR file %s line %d - unable to process function %s.  Expecting '('", mFileName, mLineNumber, mArgv[0]);
            rTuneAssert( 0 );
            return (false);
        }

        //some bools to ensure proper syntax
        bool canReadBrace = true;
        bool mustReadComma = false;
        do
        {
            //make sure we haven't run out of arguments
            if (mArgc >= MAX_ARGS)
            {
                Printf("ConERR file %s line %d - unable to process function %s.  Too many args.", mFileName, mLineNumber, mArgv[0]);
                rTuneAssert( 0 );
                return (false);
            }

            //get the next argument
            token = GetNextToken(&stringPtr, mArgv[mArgc]);
            if (! token)
            {
                Printf("ConERR file %s line %d - unable to process function %s.  Expecting ')'", mFileName, mLineNumber, mArgv[0]);
                rTuneAssert( 0 );
                return (false);
            }

            //if we've read a ')', the next token should be a ';' and we've found our function
            if (! smStricmp(token, ")"))
            {
                //make sure we can read a ')'
                if (! canReadBrace)
                {
                    Printf("ConERR file %s line %d - unable to process function %s.  Extra ',' found.", mFileName, mLineNumber, mArgv[0]);
                    rTuneAssert( 0 );
                    return (false);
                }

                //our function and it's arguments is complete
                break;
            }
            
            //else the next token is an argument or a comma
            else
            {
                if (mustReadComma)
                {
                    if (smStricmp(token, ","))
                    {
                        Printf("ConERR file %s line %d - unable to process function %s.  Args must be separated by a ','", mFileName, mLineNumber, mArgv[0]);
                        rTuneAssert( 0 );
                        return (false);
                    }

                    //else we read our comma in correctly
                    else
                    {
                        mustReadComma = false;
                        canReadBrace = false;
                    }
                }

                //otherwise the next token is a valid argument
                else
                {
                    mArgc++;
                    mustReadComma = true;
                    canReadBrace = true;
                }
            }

        } while (true);

        //at this point, the array mArgv should contain the function name and all the arg strings
        //search the table looking for the matching function
        //first, search the alias table...
        int i;
        for (i = 0; i < mAliasCount; i++)
        {
            if (! smStricmp(mAliasTable[i]->name, mArgv[0]))
            {
                //copy the original set of args

                int mPrevArgc = mArgc;
                int j;
                for( j = 0; j < mPrevArgc; ++j )
                {
                    strcpy( mPrevArgv[j], mArgv[j] );
                }


                //replace the function name
                AliasTableEntry *alias = mAliasTable[i];
                strcpy(mArgv[0], alias->functionName);

                //do the argument substitution
                bool argcSet = false;
                for (j = 0; j < alias->argc; j++)
                {
                    //see if the variable is a substitution (%1, %2, etc...)
                    if (alias->argv[j][0] == '%')
                    {
                        //copy the given argment in
                        int index = alias->argv[j][1] - '0';

                        if( index < mPrevArgc )
                        {
                            strcpy( mArgv[j + 1], mPrevArgv[index] );
                        }
                        //once we get to an argument which has not been provided
                        //the substitutions are complete...
                        else
                        {
                            //set the argc
                            mArgc = j + 1;
                            argcSet = true;
                            break;
                        }
                    }
                    
                    //else use the alias's argument
                    else
                    {
                        strcpy(mArgv[j + 1], alias->argv[j]);
                    }
                }

                //if we used the entire set of args from the alias, set the argc count
                if (! argcSet)
                    mArgc = alias->argc + 1;

                break;
            }
        }

        //now the mArgc and mArgv vars are set, and any alias substitutions have been done
        //search for the function
        bool isConditional = false;
        bool conditionalResult = false;
        bool found = false;

        std::map<radKey32, int>::const_iterator lookup;
        lookup = mFunctionLookup.find(radMakeCaseInsensitiveKey32(mArgv[0]));
        
        {
            if( lookup != mFunctionLookup.end() )
            {
                i = lookup->second;
                rTuneAssert( mFunctionTable[i]->name == mArgv[0] );  // make sure there wasn't a hash collision

                //call the function
                found = true;

                //now make sure the argument count is correct
                if ((mArgc - 1) >= mFunctionTable[i]->minArgs && (mArgc - 1) <= mFunctionTable[i]->maxArgs)
                {
                    //conditional function call
                    if (mFunctionTable[i]->isConditional)
                    {
                        //make sure we read the opening brace '{'
                        if (! ReadToken(&stringPtr, "{"))
                        {
                            Printf("ConERR file %s line %d - unable to process function %s.  Expecting '{'", mFileName, mLineNumber, mArgv[0]);
                            rTuneAssert( 0 );
                            return (false);
                        }

                        isConditional = true;
                        readEndBracketLevel++;
                        conditionalResult = (mFunctionTable[i]->functionPointer.boolReturn)(mArgc, (char**)mArgv);
                        if (negateCondition)
                            conditionalResult = (! conditionalResult);
                        negateCondition = false;
                    }
                    //non conditional function call
                    else
                    {
                        //make sure we're not trying to negate a non-conditional function
                        if (negateCondition)
                        {
                            Printf("ConERR file %s line %d - trying to negate non-conditional function %s.", mFileName, mLineNumber, mArgv[0]);
                            rTuneAssert( 0 );
                            return (false);
                        }
                        //make sure the function call ends with a ';'
                        if (! ReadToken(&stringPtr, ";"))
                        {
                            Printf("ConERR file %s line %d - unable to process function %s.  Expecting ';'", mFileName, mLineNumber, mArgv[0]);
                            rTuneAssert( 0 );
                            return (false);
                        }

                        isConditional = false;
                        negateCondition = false;

                        (mFunctionTable[i]->functionPointer.voidReturn)(mArgc, (char**)mArgv);

                        //see if the function was "exit"
                        if (! smStricmp(mArgv[0], "exit"))
                        {
                            //print out any exit messages
                            if (mArgc >= 2)
                                GetConsole()->Printf(mArgv[1]);

                            //terminate evaluating...
                            return (true);
                        }
                    }
                }

                //else print out the help string, but continue processing the string
                else
                {
                    Printf("ConERR file %s line %d - invalid arg list for function: %s()", mFileName, mLineNumber, mArgv[0]);
                    //Printf(mFunctionTable[i]->help);

                    rTuneAssert( 0 );

                    //even though the function couldn't be called, still need to read in
                    //either the ';' or the '{ ... }'
                    if (mFunctionTable[i]->isConditional)
                    {
                        //make sure we read the opening brace '{'
                        if (! ReadToken(&stringPtr, "{"))
                        {
                            Printf("ConERR file %s line %d - unable to process function %s.  Expecting '{'", mFileName, mLineNumber, mArgv[0]);
                            rTuneAssert( 0 );
                            return (false);
                        }

                        int balanceBrackets = readEndBracketLevel;
                        readEndBracketLevel++;

                        //skip past everything until we find the closing bracket
                        while (readEndBracketLevel > balanceBrackets)
                        {
                            char *token = GetNextToken(&stringPtr, mArgv[0]);
                            if (! token)
                            {
                                Printf("ConERR file %s line %d - end bracket for function %s not found.", mFileName, mLineNumber, mArgv[0]);
                                rTuneAssert( 0 );
                                return (false);
                            }

                            //see if we read another '{'
                            else if (! smStricmp(token, "{"))
                                readEndBracketLevel++;

                            else if (! smStricmp(token, "}"))
                                readEndBracketLevel--;
                        }
                    }
                    else
                    {
                        //make sure the function call ends with a ';'
                        if (! ReadToken(&stringPtr, ";"))
                        {
                            Printf("ConERR file %s line %d - unable to process function %s.  Expecting ';'", mFileName, mLineNumber, mArgv[0]);
                            rTuneAssert( 0 );
                            return (false);
                        }
                    }
                }
            }
        }

        //if we didn't find the function, print an error msg, but go on to the next...
        if (! found)
        {
            Printf("ConERR file %s line %d - console function not found: %s", mFileName, mLineNumber, mArgv[0]);
            rTuneAssert( 0 );

            //since the function couldn't be found, don't forget to skip past the ';'
            if (! ReadToken(&stringPtr, ";"))
            {
                Printf("ConERR file %s line %d - unable to process function %s.  Expecting ';'", mFileName, mLineNumber, mArgv[0]);
                rTuneAssert( 0 );
                return (false);
            }
        }

        //if the function is conditional, and the result is false
        //skip past until we find the closing brace...
        if (isConditional && ! conditionalResult)
        {
            //skip past everything until we find the closing bracket
            while (true)
            {
                char *token = GetNextToken(&stringPtr, mArgv[0]);
                if (! token)
                {
                    Printf("ConERR file %s line %d - end bracket for function %s not found.", mFileName, mLineNumber, mArgv[0]);
                    rTuneAssert( 0 );
                    return (false);
                }

                else if (! smStricmp(token, "}"))
                {
                    readEndBracketLevel--;
                    break;
                }
            }
        }

    } while (true);

#if PROFILE_PARSER
GetConsole()->Printf("DEBUG total time spent calling functions: %.3f", totalTime);
#endif

    return (true);
}



//==============================================================================
// Console::ExecuteScriptSync
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Console::ExecuteScriptSync( const char* filename, bool reload )
{
#if PROFILE_PARSER
float curTime = GameManager::GetRealTime();
GetConsole()->Printf("Console loading script file %s\n", filename);
#endif

    GetLoadingManager()->LoadSync( FILEHANDLER_CONSOLE, filename );
}


//==============================================================================
// 
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
bool Console::atob( const char* value )
{
    if( atoi(value) != 0 || !smStricmp(value, "true") )
    {
        return( true );
    }
    else
    {
        return( false );
    }
}


//==============================================================================
// Console::ExecuteScript
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Console::ExecuteScript
( 
    const char* filename, 
    Console::ExecuteScriptCallback* pCallback, 
    void* pUserData, 
    bool reload
)
{
    rTuneAssert( filename != 0 );
    rTuneAssert( pCallback != 0 );
    
    mpCallback = pCallback;
    GetLoadingManager()->AddRequest( FILEHANDLER_CONSOLE, filename, HeapMgr()->GetCurrentHeap(), this, pUserData );
}

//==============================================================================
// Console::OnProcessRequestsComplete
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
void Console::OnProcessRequestsComplete( void* pUserData )
{
    mpCallback->OnExecuteScriptComplete( pUserData );    
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//==============================================================================
// FunctionTableEntry::FunctionTableEntry
//==============================================================================
//
// Description: 
//
// Parameters:  
//
// Return:      
//
//==============================================================================
Console::FunctionTableEntry::FunctionTableEntry
( 
    const char* in_name, 
    CONSOLE_FUNCTION in_func, 
    const char* in_help, 
    int in_minArgs, 
    int in_maxArgs 
)
    :
    minArgs( in_minArgs ),
    maxArgs( in_maxArgs ),
    isConditional( false )
{
    rTuneAssert( in_func != 0 );
    rTuneAssert( strlen( in_name ) < MAX_FUNCTION_NAME );
    rTuneWarningMsg( strlen( in_help ) < MAX_HELP_LENGTH, "Help text too long.  Text has been truncated.\n" );

    functionPointer.voidReturn = in_func;
    name = in_name;
    help = in_help;
};


//==============================================================================
// FunctionTableEntry::FunctionTableEntry
//==============================================================================
//
// Description: Constructor.
//
// Parameters:  
//
// Return:      N/A.
//
//==============================================================================
Console::FunctionTableEntry::FunctionTableEntry
(
    const char* in_name,
    CONSOLE_BOOL_FUNCTION in_func,
    const char* in_help, 
    int in_minArgs, 
    int in_maxArgs 
)
    :
    minArgs( in_minArgs ),
    maxArgs( in_maxArgs ),
    isConditional( true )
{
    rTuneAssert( in_func != 0 );
    rTuneAssert( strlen( in_name ) < MAX_FUNCTION_NAME );
    rTuneAssert( strlen( in_help ) < MAX_HELP_LENGTH );

    functionPointer.boolReturn = in_func;
    name = in_name;
    help = in_help;
}


//==============================================================================
// AliasTableEntry::AliasTableEntry
//==============================================================================
//
// Description: Constructor.
//
// Parameters:  
//
// Return:      N/A.
//
//==============================================================================
Console::AliasTableEntry::AliasTableEntry
( 
    const char* srcName, 
    const char* funcName, 
    int srcArgc, 
    char** srcArgv 
)
{
    rTuneAssert( strlen( srcName ) < MAX_ALIAS_NAME );
    rTuneAssert( strlen( funcName ) < MAX_FUNCTION_NAME );
    rTuneAssert( srcArgc < MAX_ARGS );

    strcpy( name, srcName );
    strcpy( functionName, funcName );

    argc = srcArgc;

    int i;
    for( i = 0; i < argc; ++i )
    {
        rTuneAssert( strlen( srcArgv[i]) < MAX_ARG_LENGTH );
        strcpy(argv[i], srcArgv[i]);
    }
}


