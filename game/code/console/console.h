//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        console.h
//
// Description: 
//
// History:     05/06/2002 + Created. Thanks to Mirth. -- Darwin Chau 
//
//=============================================================================

#ifndef CONSOLE_H
#define CONSOLE_H

//========================================
// Nested Includes
//========================================
#include <console/nameinsensitive.h>
#include <loading/loadingmanager.h>
#include <memory/srrmemory.h>
#include <memory/memorypool.h>
// Radcore
#include <raddebug.hpp>

#ifdef RAD_DEBUG
#include <raddebugconsole.hpp>
#endif

#include <p3d/entity.hpp>

#include <map>

//========================================
// Forward References
//========================================
struct IRadFile;

#ifdef RAD_DEBUG
struct IRadDebugConsole;
class DebugConsoleCallback;
#endif

//=============================================================================
//
// Synopsis:    
//
//=============================================================================

class Console : public LoadingManager::ProcessRequestsCallback
{
    public:

        //
        // Static Methods for accessing this singleton.
        //
        static Console* CreateInstance();
        static Console* GetInstance();
        static void DestroyInstance();

        //----------------------------------------------------------------------
        // Register functions with the console.
        //
        // Functions must be of format:
        //
        //   void Foo( int argc, char** argv );
        //  
        // or  
        // 
        //   bool Foo( int argc, char** argv );
        //
        typedef void (*CONSOLE_FUNCTION)(int argc, char** argv);
        bool AddFunction( const char* name,
                          CONSOLE_FUNCTION funcPtr,
                          const char* helpString,
                          int minArgs,
                          int maxArgs );
        
        typedef bool (*CONSOLE_BOOL_FUNCTION)(int argc, char** argv);
        bool AddFunction( const char* name,
                          CONSOLE_BOOL_FUNCTION funcPtr,
                          const char* helpString,
                          int minArgs,
                          int maxArgs );
        
        //
        // Register an alias to a function.
        //
        bool AddAlias( const char* name, const char* functionName, int argc, char** argv );
        
        
        //
        // Async load and execute script file.
        //
        struct ExecuteScriptCallback
        {
            virtual void OnExecuteScriptComplete( void* pUserData ) = 0;   
        };
        
        void ExecuteScript( const char* filename, 
                            Console::ExecuteScriptCallback* pCallback, 
                            void* pUserData = 0, 
                            bool reload = false );
        
        //
        // Sync load and execute script file.
        //
        void ExecuteScriptSync( const char* filename, bool reload = false );

        
        //
        // Evaluate a string buffer.
        //
        bool Evaluate( const char* string, const char* inputSourceName );


        void Printf( char* fmt, ... );
        void Printf( int channel, char* fmt, ... );
        void Errorf( char* fmt, ... );
        
        void AssertFatal( char* fmt, ... );
        void AssertWarn( char* fmt, ... );
        
        const tNameInsensitive& TabCompleteFunction( const char* tabString, const char* curFunction );

        void ListConsoleFunctions();

        void ConsoleListenChannel( int channel );
        void ConsoleBlockChannel( int channel );

        enum
        {
            LOGMODE_OFF,
            LOGMODE_ON,
            LOGMODE_APPEND
        };
        bool SetConsoleLogMode( int mode );
        void FlushConsoleLogFile();

        //utility functions
        bool atob(const char* value);

        //
        // LoadingManager::ProcessRequestsCallback
        //
        void OnProcessRequestsComplete( void* pUserData );

        enum
        {
            MAX_STRING_LENGTH = 256,
            MAX_FUNCTIONS = 256,
            MAX_FUNCTION_NAME = 32,
            MAX_HELP_LENGTH = 256,
            MAX_ALIAS_NAME = 32,
            MAX_ARGS = 16,
            MAX_ARG_LENGTH = 64,
            MAX_ALIASES = 32,
            MAX_BUFFERS = 16
        };

#ifdef RAD_DEBUG
    public:
        void AddDebugConsoleLine(const char* text, bool warning = false);
        void SetDebugConsoleEntry(const char* text, int cursorPosition);
    private:
        int mDebugConsoleLine;
        IRadDebugConsole* mDebugConsole;
        DebugConsoleCallback* mDebugConsoleCallback;
#endif

    private:
        Console();
        ~Console();

        // Prevent wasteful constructor creation.
        Console( const Console& );
        Console& operator=( const Console& );

        static bool Initialize();

        struct FunctionTableEntry
        {
            union
            {
                CONSOLE_FUNCTION voidReturn;
                CONSOLE_BOOL_FUNCTION boolReturn;
            } functionPointer;

            //char name[MAX_FUNCTION_NAME];
            tNameInsensitive name;
            tName help;
            //char help[MAX_HELP_LENGTH];
            int minArgs;
            int maxArgs;
            bool isConditional;

            static FBMemoryPool sMemoryPool;

            FunctionTableEntry( const char* in_name, CONSOLE_FUNCTION in_func, 
                                const char* in_help, int in_minArgs, int in_maxArgs );

            FunctionTableEntry( const char* in_name, CONSOLE_BOOL_FUNCTION in_func,
                                const char* in_help, int in_minArgs, int in_maxArgs );

            static void* operator new( size_t size ) { return( sMemoryPool.Allocate( size ) ); }
            static void  operator delete( void* deadObject, size_t size ) { sMemoryPool.Free( deadObject, size ); }

            // Declared but not defined to prevent access.
            static void* operator new[]( size_t size );
            static void  operator delete[]( void* pMemory );
        };

        struct AliasTableEntry
        {
            char name[MAX_ALIAS_NAME];
            char functionName[MAX_FUNCTION_NAME];
            int argc;
            char argv[MAX_ARGS][MAX_ARG_LENGTH];

            static FBMemoryPool sMemoryPool;

            AliasTableEntry( const char* srcName, const char* funcName, 
                             int srcArgc, char** srcArgv );

            static void* operator new( size_t size ) { return( sMemoryPool.Allocate( size ) ); }
            static void  operator delete( void* deadObject, size_t size ) { sMemoryPool.Free( deadObject, size ); }

            // Declared but not defined to prevent access.
            static void* operator new[]( size_t size );
            static void  operator delete[]( void* pMemory );
        };

        // table of functions added by other modules through AddFunction()
        FunctionTableEntry* mFunctionTable[MAX_FUNCTIONS];
        int mFunctionCount;

        // nv:  to speed up the function name lookup
        std::map<radKey32, int> mFunctionLookup;

        AliasTableEntry* mAliasTable[MAX_ALIASES];
        int mAliasCount;

        // argv table for processing the function arguments
        int mArgc;
        char** mArgv;

        int mPrevArgc;
        char** mPrevArgv;

        //line number for reporting console errors
        int mLineNumber;
        char mFileName[256];

        //vars for echoing to a log file
        int mLogMode;
        IRadFile* mLogFile;

        //internal functions for parsing the string
        char* SkipWhiteSpace( const char* string );
        bool FoundComment( char** stringPtr );
        char* FindTokenEnd( const char* string );
        char* GetNextToken( char** stringPtr, char* tokenBuffer );
        bool ReadToken( char** stringPtr, const char* requiredToken );

        bool AddFunc( const char* name,
                      CONSOLE_FUNCTION funcPtr,
                      CONSOLE_BOOL_FUNCTION boolFuncPtr,
                      const char* helpString,
                      int minArgs,
                      int maxArgs );
        bool AddFunctionAlias(const char* name, const char* functionName, int argc, char** argv);
        const tNameInsensitive& TabComplete(const char* tabString, const char* curFunction);
        void ListFunctions();

        bool SetLogMode(int mode);
        void LogMessage(int channel, char* msg, bool warning = false);
        void FlushLogFile();

        //setting up a channel system so you can eliminate console spam on your local system
        int mChannelMask;
        void ListenChannel(int channel);
        void BlockChannel(int channel);

        static Console* spInstance;
        ExecuteScriptCallback* mpCallback;
};

// A little syntactic sugar for getting at this singleton.
inline Console* GetConsole() { return( Console::GetInstance() ); }


#endif // CONSOLE_H