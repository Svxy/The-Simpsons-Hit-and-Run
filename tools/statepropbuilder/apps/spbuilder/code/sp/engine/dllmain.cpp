/*
 * fightviewer/code/engine/dllmain.cpp
 */


#include <windows.h>


HINSTANCE SPHInstance = 0;


BOOL WINAPI DllMain(HINSTANCE hinstDLL,
                    DWORD reason,
                    LPVOID reserved)
{
    SPHInstance = hinstDLL;

    // Perform actions based on the reason for calling.
    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            // Initialize once for each new process.
            // Return FALSE to fail DLL load.
            break;

        case DLL_THREAD_ATTACH:
            // Do thread-specific initialization.
            break;

        case DLL_THREAD_DETACH:
            // Do thread-specific cleanup.
            break;

        case DLL_PROCESS_DETACH:
            // Perform any necessary cleanup.
            break;
    }

    return TRUE;
}


// End of file.
