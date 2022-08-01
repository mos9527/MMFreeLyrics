#include <globals.h>
#include <managers/font_manager.h>
#include <managers/lyric_manager.h>
#pragma warning( disable : 26495 26812 )
// Signatures
SIG_SCAN
(
    sigChangeGameState,
    0x1402C9660,
    "\x4C\x8B\xC2\x41\xB9\x00\x00\x00\x00\x48\x8B\x15\x00\x00\x00\x00\x48\x8B\x52\x10\xE9\x00\x00\x00\x00",
    "xxxxx????xxx????xxxxx????"
)
SIG_SCAN
(
    sigRenderLyricAndTitle,
    0x14027B150,
    "\x48\x8B\xC4\x48\x89\x58\x08\x48\x89\x70\x10\x48\x89\x78\x18\x55\x48\x8D\x68\xD8\x48\x81\xEC\x00\x00\x00\x00\x0F\x29\x70\xE8\x0F\x29\x78\xD8\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x45\xF0\x48\x8B\x5D\x50\x41\x8B\xF9\x41\x8B\xF0\x0F\x28\xF1\x0F\x28\xF8",
    "xxxxxxxxxxxxxxxxxxxxxxx????xxxxxxxxxxx????xxxxxxxxxxxxxxxxxxxxxxx"
)
SIG_SCAN
(
    sigLoadSongAudio,
    0x14023E2B0,
    "\x48\x89\x5C\x24\x00\x55\x56\x57\x48\x81\xEC\x00\x00\x00\x00\x0F\x29\xB4\x24\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x84\x24\x00\x00\x00\x00",
    "xxxx?xxxxxx????xxxx????xxx????xxxxxxx????"
)
SIG_SCAN
(
    sigUpdateLyrics,
    0x140244CD0,
    "\x48\x89\x5C\x24\x00\x57\x48\x83\xEC\x50\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x44\x24\x00\x48\x8B\xF9\x48\x8D\x99\x00\x00\x00\x00",
    "xxxx?xxxxxxxx????xxxxxxx?xxxxxx????"
)
SIG_SCAN
(
    sigGetInputState,
    0x1402AC960,
    "\x4C\x8B\x05\x00\x00\x00\x00\x4D\x85\xC0\x74\x35\x4C\x63\xC9\x48\xB8\x00\x00\x00\x00\x00\x00\x00\x00\x49\x8B\x48\x08\x49\x2B\x08\x48\xF7\xE9",
    "xxx????xxxxxxxxxx????????xxxxxxxxxx"
)
HOOK(char*, __fastcall, _LoadSongAudio, sigLoadSongAudio(), 
    __int64 a1,
    __int64 a2,
    std::string* a3,
    unsigned __int8 a4,
    int a5,
    float a6,
    float a7,
    int a8)
{
    LyricManager_Inst.SetSongAudio(a3->c_str());
    char* result = original_LoadSongAudio(a1,a2,a3,a4,a5,a6,a7,a8);
    return result;
}

HOOK(INT64, __fastcall, _ChangeGameState, sigChangeGameState(), INT64* a, const char* state) {
	INT64 result = original_ChangeGameState(a, state);
	LOG(L"Gamestate changed to : %S", state);
    LyricManager_Inst.UpdateGameState(state);
	return result;
}

HOOK(void**, __fastcall, _RenderLyricAndTitle, sigRenderLyricAndTitle(),
    float x,
    float y,
    unsigned int scale,
    int a4,
    char* text,
    char a6,
    unsigned int a7,
    void** a8)
{
    bool isLyric = x == 172.0f;    
    if (isLyric){        
        LyricManager_Inst.SetLyricLine(true, text);
        return NULL;
    } else {        
        LyricManager_Inst.SetLyricLine(false, NULL);
        void** result = original_RenderLyricAndTitle(x, y, scale, a4, text, a6, a7, a8);
        return result;
    }
}

HOOK(INT64, __fastcall, _UpdateLyrics, sigUpdateLyrics(), void* this_, int index, void* a3) {
    INT64 result = original_UpdateLyrics(this_,index,a3);
    LyricManager_Inst.UpdateLyricIndex(index);
    return result;
}

HOOK(DivaInputState*, __fastcall, _GetInputState, sigGetInputState(), int player) {
    // TODO: Use this for ImGui's input handler
    // MM+ uses DirectInput for its main game.
    // But since there's no way to capture all keystrokes with this hook, I'll leave this as is.
    // Turns out dw_gui uses Win32 WndProc to handle K&M input as well...
    DivaInputState* result = original_GetInputState(player);
    if (result && (ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantCaptureMouse))
        memset(result, 0, sizeof(DivaInputState));
    return result;
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (pBackBuffer) {
        g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
        pBackBuffer->Release();
    }
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

#define VK_F 0x46
#define VK_G 0x47
#define VK_L 0x4C
#define VK_S 0x53
#define KEY_PRESSED(K) LOWORD(wParam) == K
#define KEY_PRESSED_WITH_CONTROL(K) KEY_PRESSED(K) && GetKeyState(VK_CONTROL)
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;    
    switch (uMsg) {  
        case WM_KEYDOWN:
            if (KEY_PRESSED_WITH_CONTROL(VK_F))
                FontManager_Inst.showGUI = !FontManager_Inst.showGUI;
            if (KEY_PRESSED_WITH_CONTROL(VK_G))
                LyricManager_Inst.showGUI = !LyricManager_Inst.showGUI;
            if (KEY_PRESSED_WITH_CONTROL(VK_S)) {
                Config::SaveGlobalConfig();
                MessageBeep(MB_ICONINFORMATION);
            }
            if (KEY_PRESSED_WITH_CONTROL(VK_L)) {
                Config::LoadGlobalConfig();
                MessageBeep(MB_ICONINFORMATION);
            }
            break;
        case WM_SIZE:
            if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
            {
                CleanupRenderTarget();
                g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                CreateRenderTarget();
            }
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }    
    return CallWindowProc(originalWndProc, hWnd, uMsg, wParam, lParam);
}

void Config::SaveGlobalConfig() {
    FontManager_Inst.ToConfig(GlobalConfig_Inst);
    LyricManager_Inst.ToConfig(GlobalConfig_Inst);
    GlobalConfig_Inst.ImGuiSettings = ImGui::SaveIniSettingsToMemory();
    GlobalConfig_Inst.Save();
}

void Config::LoadGlobalConfig() {
    GlobalConfig_Inst.Load();
    FontManager_Inst.FromConfig(GlobalConfig_Inst);
    LyricManager_Inst.FromConfig(GlobalConfig_Inst);
    if (GlobalConfig_Inst.ImGuiSettings.size() > 0)
        ImGui::LoadIniSettingsFromMemory(GlobalConfig_Inst.ImGuiSettings.c_str());
}

extern "C"
{
    void __declspec(dllexport) D3DInit(IDXGISwapChain* swapChain,
        ID3D11Device* device,
        ID3D11DeviceContext* deviceContext) {
        g_pSwapChain = swapChain;
        g_pd3dDevice = device;
        g_pd3dDeviceContext = deviceContext;

        DXGI_SWAP_CHAIN_DESC m_ChainDesc;
        swapChain->GetDesc(&m_ChainDesc);
        CreateRenderTarget();
        window = m_ChainDesc.OutputWindow;
        originalWndProc = (WNDPROC)SetWindowLongPtrA(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
        
        Config::LoadGlobalConfig();
        LyricManager_Inst.Init(GlobalConfig_Inst);
        FontManager_Inst.Init(GlobalConfig_Inst);

        ImGui_ImplWin32_Init(window);
        ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    }
    void __declspec(dllexport) Init() {
		INSTALL_HOOK(_ChangeGameState);
        INSTALL_HOOK(_RenderLyricAndTitle);
        INSTALL_HOOK(_LoadSongAudio);
        INSTALL_HOOK(_UpdateLyrics);
        INSTALL_HOOK(_GetInputState);
        ImGui::CreateContext();
        SET_IMGUI_DEFAULT_FLAGS;
        LOG(L"Hooks installed.");           
    }
    void __declspec(dllexport) OnFrame(IDXGISwapChain* m_pSwapChain) {
        LyricManager_Inst.OnFrame();
        FontManager_Inst.OnFrame();

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame(); 
        
        LyricManager_Inst.OnImGUI();
        FontManager_Inst.OnImGUI();

        ImGui::Render();        
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
}