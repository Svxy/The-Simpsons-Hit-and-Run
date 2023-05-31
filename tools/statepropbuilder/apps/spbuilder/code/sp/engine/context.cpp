/*
 * sp/engine/context.cpp
 */


#include <SP/engine/workspace.hpp>
#include <SP/engine/context.hpp>
#include <SP/interface/context.hpp>


//#include <SP/engine/project.hpp>
//#include <choreo/load.hpp>

#include <p3d/context.hpp>
#include <p3d/utility.hpp>
#include <p3d/view.hpp>
#include <p3d/directionallight.hpp>
#include <p3d/pointcamera.hpp>
#include <p3d/geometry.hpp>
#include <p3d/anim/drawablepose.hpp>
#include <radload/radload.hpp>

#include <radtime.hpp>
#include <radplatform.hpp>

#include <shlwapi.h>

#include <typeinfo>


#ifdef RAD_DEBUG
static const char* pddiDLL = "pddidx8d.dll";
#endif

#ifdef RAD_TUNE
static const char* pddiDLL = "pddidx8t.dll";
#endif

#ifdef RAD_RELEASE
static const char* pddiDLL = "pddidx8r.dll";
#endif


// global SPContext reference
SPContext* g_SPContext = 0;

extern Workspace* g_Workspace;


//---------------------------------------------------------------------------
// class SPContext - implementation
//---------------------------------------------------------------------------

SPContext::SPContext(HWND hwnd):
    m_HWnd(hwnd),

    m_Context(0),
    m_View(0),
    m_Camera(0),
    m_Light(0),
    m_Shader(0),

    m_CurrentTime(-1.0f),
    m_FPSIndex(0),
    m_SimRate(1.0f),

    m_MouseCapture(0),
    m_MouseX(0),
    m_MouseY(0),

    m_IsCameraLocked(false)
{
    memset(m_FPS, 0, sizeof(m_FPS));
}

SPContext::~SPContext()
{
    tRefCounted::Release(m_Shader);
    tRefCounted::Release(m_Light);
    tRefCounted::Release(m_Camera);
    tRefCounted::Release(m_View);
}

bool SPContext::Open()
{
    // p3d context init
    RECT wndRect;
    GetWindowRect(m_HWnd, &wndRect);

    tContextInitData init;
    init.hwnd = m_HWnd;
    init.displayMode = PDDI_DISPLAY_WINDOW;
    init.bufferMask = PDDI_BUFFER_COLOUR | PDDI_BUFFER_DEPTH;
    int wndWidth = wndRect.right - wndRect.left;
    int wndHeight = wndRect.bottom - wndRect.top;
    init.xsize = wndWidth;
    init.ysize = wndHeight;
    init.bpp = 32;
    strcpy(init.PDDIlib, pddiDLL);

    //p3d::platform->SetActiveContext(0);
    m_Context = p3d::platform->CreateContext(&init);
    if (m_Context == 0)
        return false;

    p3d::platform->SetActiveContext(m_Context);

    // Call into PDDI to enable Z-buffering.
    p3d::pddi->EnableZBuffer(true);


    radLoadInitialize();

    // This call installs chunk handlers for all the primary chunk types that
    // Pure3D supports.  This includes textures, materials, geometries, and the
    // like.  
    p3d::InstallDefaultLoaders();

    // install default choreo loaders
    //choreo::InstallDefaultLoaders();

    // p3d view init
    m_View = new tView;
    m_View->AddRef();

    m_View->SetBackgroundColour(tColour(64, 64, 128));
    m_View->SetAmbientLight(tColour(40, 40, 40));

    // p3d light init
    m_Light = new tDirectionalLight;
    m_Light->AddRef();

    m_Light->SetDirection(0.0f, 0.0f, 1.0f);
    m_Light->SetColour(tColour(255, 255, 255));
    m_View->AddLight(m_Light);

    // p3d camera init
    m_Camera = new tPointCamera;
    m_Camera->AddRef();
    m_Camera->SetNearPlane(0.01f);
    m_Camera->SetFarPlane(5000.0f);
    m_Camera->SetFOV(rmt::DegToRadian(90.0f), (float)wndWidth / (float)wndHeight);
    m_View->SetCamera(m_Camera);

    m_CameraTarget.Clear();
    m_CameraRotateX = rmt::DegToRadian(-45.0f);
    m_CameraRotateY = 0.0f;
    m_CameraDistance = 5.0f;

    // pddi shader
    m_Shader = p3d::device->NewShader("simple");
    m_Shader->AddRef();
    m_Shader->SetInt(PDDI_SP_ISLIT, 0);

    // workspace create
    if (g_Workspace == 0)
    {
        g_Workspace = new Workspace;
        g_Workspace->AddRef();
    }

    return true;
}

bool SPContext::Close()
{
    // p3d release stuff
    tRefCounted::Release(m_Shader);
    tRefCounted::Release(m_Light);
    tRefCounted::Release(m_Camera);
    tRefCounted::Release(m_View);

    // p3d context destroy
    if (m_Context != 0)
    {
        p3d::platform->DestroyContext(m_Context);
        m_Context = 0;
    }

    radLoadTerminate();

    return true;
}

bool SPContext::ViewResize(int w, int h)
{
    if (w <= 0)
        w = 1;
    if (h <= 0)
        h = 1;

    RECT wndRect;
    GetWindowRect(m_HWnd, &wndRect);
    int wndWidth = wndRect.right - wndRect.left;
    if (wndWidth <= 0)
        wndWidth = 1;
    int wndHeight = wndRect.bottom - wndRect.top;
    if (wndHeight <= 0)
        wndHeight = 1;

    if (w > wndWidth)
        w = wndWidth;
    if (h > wndHeight)
        h = wndHeight;

    m_View->SetWindow(0.0f, 0.0f, (float)w / (float)wndWidth, (float)h / (float)wndHeight);
    m_Camera->SetFOV(rmt::DegToRadian(90.0f), (float)w / (float)h);

    return true;
}

bool SPContext::Advance()
{
    // do time
    float deltaTime;
    {
        float newTime = (float)radTimeGetMilliseconds() / 1000.0f;

        if (m_CurrentTime < 0.0f)
        {
            deltaTime = 0.0f;
            m_CurrentTime = newTime;
        }
        else
        {
            deltaTime = newTime - m_CurrentTime;

            if (deltaTime < 0.0f)
            {
                deltaTime = 0.0f;
            }
            else
            {
                m_CurrentTime = newTime;
            }

            // for debugging
            if (deltaTime > 1.0f)
            {
                deltaTime = 1.0f / 60.0f;
            }
            else
            {
                // do fps
                m_FPS[m_FPSIndex] = deltaTime;
                m_FPSIndex = (m_FPSIndex + 1) % MAX_FPS_COUNT;
            }
        }
    }

    float simDeltaTime = m_SimRate * deltaTime;
	if ( g_Workspace )
	{
		g_Workspace->Advance(simDeltaTime * 1000.0f);
	}

    // update camera & light
    {
        /*if (m_IsCameraLocked)
        {
            rmt::Vector desiredTarget = m_CVPuppet->GetCameraTarget();

            rmt::Vector delta = desiredTarget;
            delta.Sub(m_CameraTarget);
            float mag = delta.NormalizeSafe();

            static const float cameraSpeed = 1.5f;  // m/s
            float maxMag = cameraSpeed * deltaTime * mag;
            if (mag > maxMag)
            {
                mag = maxMag;
            }

            delta.Scale(mag);
            m_CameraTarget.Add(delta);
            m_CameraTarget.x = desiredTarget.x;
            m_CameraTarget.z = desiredTarget.z;
        }*/

        m_Camera->SetTarget(m_CameraTarget);
        rmt::Matrix mY;
        mY.FillRotateY(m_CameraRotateY);
        rmt::Matrix mX;
        mX.FillRotateX(m_CameraRotateX);
        rmt::Vector cameraPos(0.0f, 0.0f, m_CameraDistance);
        cameraPos.Rotate(mX);
        cameraPos.Rotate(mY);
        cameraPos.Add(m_CameraTarget);
        m_Camera->SetPosition(cameraPos);

        rmt::Vector lightDir;
        lightDir = m_CameraTarget;
        lightDir.Sub(cameraPos);
        m_Light->SetDirection(lightDir);
    }

    //return rc;
	return true;
}

bool SPContext::Display()
{
    m_Context->BeginFrame();
    m_View->BeginRender();

    // render game context
	if ( g_Workspace )
	{
		g_Workspace->Display(m_Context);
	}

    // draw origin
    {
        rmt::Matrix m;
        m.Identity();
        DrawAxes(m, 0.5f);
    }

    m_View->EndRender();
    m_Context->EndFrame();

    //return rc;
	return true;
}

void SPContext::DrawAxes(const rmt::Matrix& m, float radius)
{
   rmt::Vector oldVtx[4] =
   {
      rmt::Vector(0,0,0),
      rmt::Vector(radius,0,0),
      rmt::Vector(0,radius,0),
      rmt::Vector(0,0,radius)
   };
   rmt::Vector newVtx[4];
   m.Transform(4, oldVtx, newVtx);

   pddiPrimStream* stream = p3d::pddi->BeginPrims(m_Shader, PDDI_PRIM_LINES, PDDI_V_C , 6);

   stream->Vertex((pddiVector*)newVtx, pddiColour(255,0,0));
   stream->Vertex((pddiVector*)(newVtx + 1), pddiColour(255,0,0));

   stream->Vertex((pddiVector*)newVtx, pddiColour(0,255,0));
   stream->Vertex((pddiVector*)(newVtx + 2), pddiColour(0,255,0));

   stream->Vertex((pddiVector*)newVtx, pddiColour(0,0,255));
   stream->Vertex((pddiVector*)(newVtx + 3), pddiColour(0,0,255));

   p3d::pddi->EndPrims(stream);
}

void SPContext::DrawLine(const rmt::Vector& a, const rmt::Vector& b, tColour c)
{
	pddiPrimStream* stream = p3d::pddi->BeginPrims(m_Shader, PDDI_PRIM_LINES, PDDI_V_C , 2 );
	stream->Vertex((pddiVector*)(&a), c);
	stream->Vertex((pddiVector*)(&b), c);
	p3d::pddi->EndPrims(stream);
}

void SPContext::RenderPose(tPose* pose)
{
    pose->Evaluate();

	int i;
	int startJoint = 0;
	for (i = startJoint; i < pose->GetNumJoint(); ++i)
	{
        tPose::Joint* joint = pose->GetJoint(i);
        tPose::Joint* parent = joint->parent;

        if (parent != 0)
        {
            DrawLine(parent->worldMatrix.Row(3), joint->worldMatrix.Row(3), tColour(255,255,255));
            DrawAxes(parent->worldMatrix, 0.02f);
        }
	}
}

void SPContext::MouseDown(int button, int shift, int x, int y)
{
    if (m_MouseCapture == 0)
    {
        SetCapture(m_HWnd);
        m_MouseCapture = 1;
    }

    m_MouseX = x;
    m_MouseY = y;
}

void SPContext::MouseMove(int button, int shift, int x, int y)
{
    if (m_MouseCapture == 0)
        return;
    if (GetCapture() != m_HWnd)
        return;

    int dx = x - m_MouseX;
    int dy = y - m_MouseY;
    m_MouseX = x;
    m_MouseY = y;

    if ((button & MK_LBUTTON) != 0)
    {
        if ((button & MK_MBUTTON) != 0)
        {
            CameraZoom(dx * 10);
        }
        else
        {
            CameraRotate(dx, dy);
        }
    }
    else if ((button & MK_MBUTTON) != 0)
    {
        CameraPan(dx, dy);
    }
}

void SPContext::MouseUp(int button, int shift, int x, int y)
{
    MouseMove(button, shift, x, y);

    if (m_MouseCapture == 0)
        return;

    ReleaseCapture();
    m_MouseCapture = 0;
}

void SPContext::MouseWheel(int scroll)
{
    CameraZoom(scroll);
}

void SPContext::CameraPan(int dx, int dy)
{
    RECT wndRect;
    GetWindowRect(m_HWnd, &wndRect);

    rmt::Vector v((float)-dx / (float)(wndRect.right - wndRect.left),
                  (float)dy / (float)(wndRect.bottom - wndRect.top), 0.0f);
    rmt::Vector a, b;
    m_Camera->ViewToWorld(v, &a, &b);
    b.Sub(a);
    b.NormalizeSafe();
    b.Scale(m_CameraDistance - m_Camera->GetNearPlane());
    b.Add(a);
    m_CameraTarget = b;
}

void SPContext::CameraZoom(int dz)
{
	m_CameraDistance += ((float)-dz * 0.001f * m_CameraDistance);

	if (m_CameraDistance < 0.1f)
	{
		m_CameraDistance = 0.1f;
	}
}

void SPContext::CameraRotate(int dx, int dy)
{
    m_CameraRotateX += ((float)-dy * 0.01f);

    if (m_CameraRotateX > rmt::DegToRadian(89.9f))
    {
        m_CameraRotateX = rmt::DegToRadian(89.9f);
    }

    if (m_CameraRotateX < rmt::DegToRadian(-89.9f))
    {
        m_CameraRotateX = rmt::DegToRadian(-89.9f);
    }

    m_CameraRotateY += ((float)dx * 0.01f);
}

float SPContext::SimulationGetFPS() const
{
    float fps = 0.0f;

    for (int i = 0; i < MAX_FPS_COUNT; ++i)
    {
        fps += m_FPS[i];
    }

    fps /= (float)MAX_FPS_COUNT;

    if (!rmt::Epsilon(fps, 0.0f))
    {
        fps = 1.0f / fps;
    }

    return fps;
}

void SPContext::SimulationSetRate(float simRate)
{
    m_SimRate = simRate;

    if (m_SimRate < 0.0f)
    {
        m_SimRate = 0.0f;
    }
}

bool SPContext::GetEntityName(tEntity* entity, char* name, int maxLen)
{
    if (maxLen > 0)
    {
        name[0] = 0;
    }

    if (entity == 0)
        return false;

    if (entity->GetName() != 0)
    {
        _snprintf(name, maxLen, "%s", entity->GetName());
    }
    else
    {
        _snprintf(name, maxLen, "0x%016I64x", entity->GetUID());
    }

    if (maxLen > 0)
    {
        name[maxLen - 1] = 0;
    }

    return true;
}

void SPContext::InventoryClear()
{
    if (m_Context != 0)
    {
        m_Context->GetInventory()->RemoveAllElements();
    }
}

int SPContext::InventoryGetEntityCount() const
{
    if (m_Context == 0)
        return 0;

    int entityCount = 0;
	/*
    for (int s = 0; s < m_Context->GetInventory()->GetSectionCount(); ++s)
    {
        entityCount += m_Context->GetInventory()->GetSectionByIndex(s)->GetElementCount();
    }
	*/

    return entityCount;
}

bool SPContext::InventoryGetEntityName(int index,
                                       char* name, int nameLen,
                                       char* type, int typeLen) const
{
    int entityCount = 0;

	/*
    for (int s = 0; s < m_Context->GetInventory()->GetSectionCount(); ++s)
    {
        tEntityTable::RawIterator itor(m_Context->GetInventory()->GetSectionByIndex(s));

        tEntity* obj = itor.First();
        while (obj)
        {
            if (entityCount == index)
            {
                GetEntityName(obj, name, nameLen);
                _snprintf(type, typeLen, "%s", typeid(*obj).name());
                type[typeLen - 1] = '\0';

                return true;
            }

            ++entityCount;
            obj = itor.Next();
        }
    }
	*/

    return false;
}

void SPContext::SetBackgroundColour(int r , int g , int b )
{
	m_View->SetBackgroundColour( tColour( r , g , b ) );
}


//---------------------------------------------------------------------------
// SPContext - DLL interface
//---------------------------------------------------------------------------

int SP_CALLCONV SPContextOpen(HWND hwnd)
{
    P3DASSERT(g_SPContext == 0);
    if (g_SPContext != 0)
        return -1;

    g_SPContext = new SPContext(hwnd);
    if (!g_SPContext->Open())
    {
        delete g_SPContext;
        g_SPContext = 0;
        return -1;
    }

    return 1;
}

int SP_CALLCONV SPContextClose()
{
    if (g_SPContext == 0)
        return 0;

    if (!g_SPContext->Close())
    {
        delete g_SPContext;
        g_SPContext = 0;
        return -1;
    }

    delete g_SPContext;
    g_SPContext = 0;
    return 1;
}

int SP_CALLCONV SPContextViewResize(int w, int h)
{
    if (g_SPContext == 0)
        return -1;

    g_SPContext->ViewResize(w, h);
    return 1;
}

int SP_CALLCONV SPContextIsPDDIStatsEnabled()
{
    if (p3d::pddi == 0)
        return -1;
    return p3d::pddi->IsStatsOverlayEnabled();
}

int SP_CALLCONV SPContextSetIsPDDIStatsEnabled(int statsEnabled)
{
    if (p3d::pddi == 0)
        return -1;
    p3d::pddi->EnableStatsOverlay(statsEnabled != 0);
    return 1;
}

int SP_CALLCONV SPContextAdvance()
{
    if (g_SPContext == 0)
        return -1;

    return g_SPContext->Advance();
}

int SP_CALLCONV SPContextDisplay()
{
    if (g_SPContext == 0)
        return -1;

    return g_SPContext->Display();
}

int SP_CALLCONV SPContextMouseDown(int button, int shift, int x, int y)
{
    if (g_SPContext == 0)
        return -1;

    g_SPContext->MouseDown(button, shift, x, y);
    return 0;
}

int SP_CALLCONV SPContextMouseMove(int button, int shift, int x, int y)
{
    if (g_SPContext == 0)
        return -1;

    g_SPContext->MouseMove(button, shift, x, y);
    return 0;
}

int SP_CALLCONV SPContextMouseUp(int button, int shift, int x, int y)
{
    if (g_SPContext == 0)
        return -1;

    g_SPContext->MouseUp(button, shift, x, y);
    return 0;
}

int SP_CALLCONV SPContextMouseWheel(int scroll)
{
    if (g_SPContext == 0)
        return -1;

    g_SPContext->MouseWheel(scroll);
    return 0;
}

float SP_CALLCONV SPSimulationGetFPS()
{
    if (g_SPContext == 0)
        return 0.0f;

    return g_SPContext->SimulationGetFPS();
}

float SP_CALLCONV SPSimulationGetRate()
{
    if (g_SPContext == 0)
        return 0.0f;

    return g_SPContext->SimulationGetRate();
}

int SP_CALLCONV SPSimulationSetRate(float rate)
{
    if (g_SPContext == 0)
        return -1;

    g_SPContext->SimulationSetRate(rate);
    return 0;
}

int SP_CALLCONV SPContextGetCameraLock()
{
    if (g_SPContext == 0)
        return -1;

    return g_SPContext->IsCameraLocked();
}

int SP_CALLCONV SPContextSetCameraLock(int cameraLock)
{
    if (g_SPContext == 0)
        return -1;

    g_SPContext->SetIsCameraLocked(cameraLock != 0);
    return 0;
}

int SP_CALLCONV SPInventoryClear()
{
    if (g_SPContext == 0)
        return -1;

    g_SPContext->InventoryClear();
    return 0;
}

int SP_CALLCONV SPInventoryGetEntityCount()
{
    if (g_SPContext == 0)
        return -1;

    return g_SPContext->InventoryGetEntityCount();
}

int SP_CALLCONV SPInventoryGetEntityName(int index, char* name, int nameLen, char* type, int typeLen)
{
    if (g_SPContext == 0)
        return -1;

    if (!g_SPContext->InventoryGetEntityName(index, name, nameLen, type, typeLen))
    {
        return -1;
    }

    return 0;
}

int SP_CALLCONV SPSetBackgroundColour( int r , int g , int b )
{
    if (g_SPContext == 0)
        return 0;

    g_SPContext->SetBackgroundColour( r , g , b );
	return 1;
}

// End of file.
