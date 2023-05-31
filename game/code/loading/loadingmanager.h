//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        loadingmanager.h
//
// Description: Declaration of LoadingManager singleton class.
//
// History:     3/25/2002 + Created -- Darwin Chau
//
//=============================================================================

#ifndef LOADINGMANAGER_H
#define LOADINGMANAGER_H

//========================================
// Nested Includes
//========================================
#include <loading/filehandlerenum.h>
#include <loading/filehandler.h>

#ifndef TOOLS
#include <memory/srrmemory.h>
#endif

//========================================
// Forward References
//========================================
class FileHandler;
struct ILoadingListCallback;
struct IRadCementLibrary;


//
// Types
//
typedef int CementFileHandle;


//=============================================================================
//
// Synopsis:    All non-streamed disc loads must be routed through this class.
//
//=============================================================================
class LoadingManager : public FileHandler::LoadFileCallback
{
    public:

        // Static Methods for accessing this singleton.
        static LoadingManager* GetInstance();
        static LoadingManager* CreateInstance();
        static void DestroyInstance();
        
        //
        // Cement file registration functions.  Should probably be called from 
        // context switch functions and the like
        //
        CementFileHandle RegisterCementLibrary( const char* filename );
        void UnregisterCementLibrary( CementFileHandle handle );

        //----------------------------------------------------------------------
        // ASYNCHRONOUS LOADING
        //----------------------------------------------------------------------
        
        struct ProcessRequestsCallback
        {
            virtual void OnProcessRequestsComplete( void* pUserData ) = 0;   
        };

        void AddCallback( LoadingManager::ProcessRequestsCallback* pCallback = NULL, 
                          void* pUserData = 0 );

        void AddRequest( FileHandlerEnum handlerType, 
                         const char* filename, 
                         GameMemoryAllocator heap,
                         const char* sectionName = 0, 
                         const char* groupTag = NULL ,
                         LoadingManager::ProcessRequestsCallback* pCallback = NULL, 
                         void* pUserData = 0 );

        inline void AddRequest( FileHandlerEnum handlerType, 
                                const char* filename,
                                GameMemoryAllocator heap,
                                LoadingManager::ProcessRequestsCallback* pCallback, 
                                void* pUserData = 0 )
        {
            AddRequest( handlerType, filename, heap, 0, NULL, pCallback, pUserData);
        }

        //----------------------------------------------------------------------
        // SYNCHRONOUS LOADING
        //----------------------------------------------------------------------

        void LoadSync( FileHandlerEnum handlerType, const char* filename, GameMemoryAllocator heap = GMA_DEFAULT, 
                       const char* sectionName = NULL );
        

        //----------------------------------------------------------------------
        // Loading Accessor
        //----------------------------------------------------------------------
        bool IsLoading();

        inline int GetRequestHead() const { return mRequestHead; }
        inline int GetRequestTail() const { return mRequestTail; }

        int GetNumCurrentRequests() const;

        void CancelPendingRequests();  //This will not cancel the current request.

#ifdef RAD_WIN32
        int GetNumRequestsProcessed() const { return mRequestsProcessed; }
        inline void ResetRequestsProcessed();
#endif

        //
        // Used to hold a pointer to a cement library object.  isLoading bool
        // needed since registration is asynchronous.
        //
        struct CementLibraryStruct
        {
            IRadCementLibrary* library;
            bool isLoading;
        };

    private:

        // No public access to these, use singleton interface.
        LoadingManager();
        ~LoadingManager();

        // Declared but not defined to prevent copying and assignment.
        LoadingManager( const LoadingManager& loadingmanager );
        LoadingManager& operator=( const LoadingManager& loadingmanager );

        // Implement FileHandler::LoadFileCallback interface.
        virtual void OnLoadFileComplete( void* pUserData );        

        void ProcessNextRequest();

        // Pointer to the one and only instance of this singleton.
        static LoadingManager* spInstance;
        
        // Store loading requests here.
        enum { MAX_REQUESTS = 200 };

        struct LoadingRequest
        {
            enum { LOADING_FILENAME_LENGTH = 128 };
            // Darwin: is 64 too big/big enough?
            char filename[LOADING_FILENAME_LENGTH];
            FileHandler* pFileHandler;
            unsigned int startTime;
            GameMemoryAllocator heap;
            ProcessRequestsCallback* pCallback;
            void* pUserData;
#ifdef MEMORYTRACKER_ENABLED
            // Since filenames go in here by default, use filename length for the size
            char groupTag[LOADING_FILENAME_LENGTH];
#endif
        };
        LoadingRequest mRequests[MAX_REQUESTS];
        int mRequestHead;
        int mRequestTail;

        bool mLoading;

        bool mCancellingLoads;

#ifdef RAD_WIN32
        int mRequestsProcessed;
#endif

        //
        // Store cement libraries
        //
#ifdef RAD_XBOX
        static const int MAX_CEMENT_LIBRARIES = 15;
#else
        static const int MAX_CEMENT_LIBRARIES = 10;
#endif
        CementLibraryStruct mCementLibraries[MAX_CEMENT_LIBRARIES];
};

// A little syntactic sugar for getting at this singleton.
inline LoadingManager* GetLoadingManager() { return( LoadingManager::GetInstance() ); }

//=============================================================================
// LoadingManager::IsLoading
//=============================================================================
// Description: Comment
//
// Parameters:  ()
//
// Return:      bool 
//
//=============================================================================
inline bool LoadingManager::IsLoading()
{
    return ( mLoading || mRequestTail != mRequestHead );
}

inline int LoadingManager::GetNumCurrentRequests() const
{
    return( (mRequestTail - mRequestHead + MAX_REQUESTS) % MAX_REQUESTS );
}

#ifdef RAD_WIN32
    inline void LoadingManager::ResetRequestsProcessed()
    {
        mRequestsProcessed = 0;
    }
#endif // RAD_WIN32

#endif //LOADINGMANAGER_H
