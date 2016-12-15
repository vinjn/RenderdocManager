#pragma once

#include "renderdoc_app.h"
#include <Windows.h>

#ifdef _WIN64
static const char* DEFAULT_RENDERDOC_DLL = "c:/Program Files/RenderDoc/x86/renderdoc.dll";
#else
static const char* DEFAULT_RENDERDOC_DLL = "c:/Program Files/RenderDoc/renderdoc.dll";
#endif

class RenderDocManager
{
public:
    bool setup(const char* pCapturePath, const char* pRenderDocPath = DEFAULT_RENDERDOC_DLL);
    ~RenderDocManager();
    void TriggerCapture();

private:
    RENDERDOC_API_1_1_1* m_Api;
    HINSTANCE m_RenderDocDLL;
    RENDERDOC_DevicePointer m_Device;
    RENDERDOC_WindowHandle m_Handle;
};

bool RenderDocManager::setup(const char* pCapturePath, const char* pRenderDocPath)
{
    m_RenderDocDLL = ::LoadLibraryA(pRenderDocPath);
    if (NULL == m_RenderDocDLL)
    {
        OutputDebugString(L"Render doc dll not found!");
        return false;
    }

    pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)::GetProcAddress(m_RenderDocDLL, "RENDERDOC_GetAPI");
    if (RENDERDOC_GetAPI != NULL
        && RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_1, (void**)&m_Api) == 1)
    {
        m_Api->SetLogFilePathTemplate(pCapturePath);

        RENDERDOC_InputButton captureKey = eRENDERDOC_Key_F12;
        m_Api->SetCaptureKeys(&captureKey, 1);
        m_Api->SetFocusToggleKeys(NULL, 0);

        m_Api->SetCaptureOptionU32(eRENDERDOC_Option_CaptureCallstacks, 1);
        m_Api->SetCaptureOptionU32(eRENDERDOC_Option_CaptureAllCmdLists, 1);
        m_Api->SetCaptureOptionU32(eRENDERDOC_Option_SaveAllInitials, 1);

        m_Api->MaskOverlayBits(eRENDERDOC_Overlay_None, eRENDERDOC_Overlay_None);

        return true;
    }
    else
    {
        OutputDebugString(L"Render doc API is not compatible !");
        ::FreeLibrary(m_RenderDocDLL);
        m_RenderDocDLL = NULL;
        return false;
    }
}

void RenderDocManager::TriggerCapture()
{
    m_Api->TriggerCapture();
}

RenderDocManager::~RenderDocManager(void)
{
    m_Api->Shutdown();
    ::FreeLibrary(m_RenderDocDLL);
}
