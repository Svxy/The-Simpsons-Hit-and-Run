/*
 * sp/engine/context.hpp
 */


#ifndef SP_ENGINE_CONTEXT_HPP
#define SP_ENGINE_CONTEXT_HPP


#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>

#include <radmath/radmath.hpp>
#include <p3d/p3dtypes.hpp>


class tEntity;
class tContext;
class tView;
class tPointCamera;
class tDirectionalLight;
class tPose;
class pddiShader;


//---------------------------------------------------------------------------
// class SPContext
//---------------------------------------------------------------------------

class SPContext
{
public:

    enum
    {
        MAX_FPS_COUNT = 10
    };

    SPContext(HWND hwnd);
    virtual ~SPContext();

    tContext* GetContext()
        { return m_Context; }

    bool Open();
    bool Close();

    bool ViewResize(int w, int h);

    bool Advance();
    bool Display();

    void MouseDown(int button, int shift, int x, int y);
    void MouseMove(int button, int shift, int x, int y);
    void MouseUp(int button, int shift, int x, int y);
    void MouseWheel(int scroll);

    float SimulationGetFPS() const;
    float SimulationGetRate() const
        { return m_SimRate; }
    void SimulationSetRate(float simRate);

    bool IsCameraLocked() const
        { return m_IsCameraLocked; }
    void SetIsCameraLocked(bool cameraLock)
        { m_IsCameraLocked = cameraLock; }
    tPointCamera* GetCamera() const
        { return m_Camera; }

    static bool GetEntityName(tEntity* entity, char* name, int maxLen);

    void InventoryClear();
    int InventoryGetEntityCount() const;
    bool InventoryGetEntityName(int index,
                                char* name, int nameLen,
                                char* type, int typeLen) const;

    void RenderPose(tPose* pose);
    void DrawAxes(const rmt::Matrix& m, float radius);
    void DrawLine(const rmt::Vector& a, const rmt::Vector& b, tColour c);

	void SetBackgroundColour( int r , int g , int b );

private:

    void CameraPan(int dx, int dy);
    void CameraZoom(int dz);
    void CameraRotate(int dx, int dy);

    HWND m_HWnd;

    tContext* m_Context;
    tView* m_View;
    tPointCamera* m_Camera;
    tDirectionalLight* m_Light;
    pddiShader* m_Shader;

    float m_CurrentTime;
    float m_FPS[MAX_FPS_COUNT];
    int m_FPSIndex;
    float m_SimRate;

    rmt::Vector m_CameraTarget;
    float m_CameraRotateX;
    float m_CameraRotateY;
    float m_CameraDistance;
    bool m_IsCameraLocked;

    int m_MouseCapture;
    int m_MouseX;
    int m_MouseY;
};


extern SPContext* g_SPContext;


#endif // SP_ENGINE_CONTEXT_HPP
