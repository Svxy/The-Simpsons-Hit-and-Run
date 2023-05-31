/*
 * sp/engine/platform.cpp
 */


#include <sp/interface/platform.hpp>

#include <radmemory.hpp>
#include <radplatform.hpp>
#include <radtime.hpp>
#include <radfile.hpp>
#include <radthread.hpp>

#include <p3d/utility.hpp>


extern HINSTANCE SPHInstance;

//---------------------------------------------------------------------------
// SPPlatform - DLL interface
//---------------------------------------------------------------------------

int SP_CALLCONV SPPlatformOpen(HWND hwnd)
{
	// ftt thread init
	radThreadInitialize();

    // ftt memory init
    radMemoryInitialize();

    // ftt platform init
    radPlatformInitialize(hwnd, SPHInstance);

    // ftt time init
    radTimeInitialize();

    // ftt file init
    radFileInitialize();

    // p3d platform init
    tPlatform* platform = tPlatform::Create(SPHInstance);
    if (platform == 0)
        return -1;

    // game project create
	/*
    if (g_SPProject == 0)
    {
        g_SPProject = SP_PROC_NEWPROJECT();
        P3DASSERT(g_SPProject != 0);
        if (g_SPProject == 0)
            return -1;
        g_SPProject->AddRef();
        g_SPProject->ProjectInit();
    }
	*/

    return 0;
}

int SP_CALLCONV SPPlatformClose()
{
    // destroy game project
    //tRefCounted::Release(g_SPProject);

    // destroy workspace
    //tRefCounted::Release(g_SPWorkspace);

    // p3d platform destroy
    if (p3d::platform != 0)
    {
        tPlatform::Destroy(p3d::platform);
    }

    // ftt file destroy
    radFileTerminate();

    // ftt time destroy
    radTimeTerminate();

    // ftt platform destroy
    radPlatformTerminate();

    // ftt memory destroy
    radMemoryTerminate();

	// ftt thread destroy
	radThreadTerminate();

    return 0;
}


// End of file.
