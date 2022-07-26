#include "pch.h"

#define LYRIC_PLACEHOLDER_MESSAGE u8"初音未来 Project DIVA メガー・ミクス"
// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
// Data
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
static WNDPROC originalWndProc;
static HWND window;

#pragma region Sigatures
const uint32_t* ValueThatChangesWhilistPlaying = (uint32_t*)0x1412EE338;
const uint32_t* PVEventFrame = (uint32_t*)0x1412EE324;

SIG_SCAN
(
    sigChangeGameState,
    0x1402C9660,
    "\x4C\x8B\xC2\x41\xB9\x00\x00\x00\x00\x48\x8B\x15\x00\x00\x00\x00\x48\x8B\x52\x10\xE9\x00\x00\x00\x00",
    "xxxxx????xxx????xxxxx????"
)
SIG_SCAN
(
    sigRenderLyricMaybe,
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

#pragma endregion

struct FontManager {
    enum FontType {
        Chinese,
        Japanese
    };

private:
    std::vector<std::string> fontsAvailable;
    std::string * fontnameChinese;
    std::string * fontnameJapanese;
    std::string charset;
    bool reloadFonts = false;

    float fontSize = 40.0f;
    ImVec4 fillColor = ImVec4(1,1,1,0);
    ImVec4 strokeColor = ImVec4(0,0,0,0);

public:
    // Current combined font used for lyrics renderer
    ImFont* font;  
    bool showGUI = true;

    void Init() {
        this->RefreshFontList();
        this->fontnameChinese = &this->fontsAvailable[0];
        this->fontnameJapanese = &this->fontsAvailable[this->fontsAvailable.size()-1];
        // Read custom charset from file. We don't need to cover the entire CJK codepoints
        std::ifstream f(to_utf8(FullFilenameUnderDLLPath(L"charset.txt")));
        std::stringstream charbuf;
        charbuf << f.rdbuf();
        charset = charbuf.str();
        LOG(L"Font charset size is %zd.\n", this->charset.size());
        this->RebuildFonts();
    }

    std::vector<std::string> RefreshFontList() {
        this->fontsAvailable.clear();
        for (auto file : std::filesystem::directory_iterator(to_utf8(FullFilenameUnderDLLPath(L"fonts\\")))) {
            std::wstring path = file.path();
            std::wstring ext = path.substr(path.length() - 3);
            if (ext == L"ttf" || ext == L"otf") {
                this->fontsAvailable.push_back(to_utf8(path));
            }
        }
        return this->fontsAvailable;
    }

    void SetDefaultStyles() {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowBorderSize = 0.0f;
        style.WindowRounding = 12.0f;
        LOG(L"Reseted ImGui styles.");
    }

    void RebuildFonts() {        
        /* Fonts and glyph ranges*/
        ImGuiIO& io = ImGui::GetIO();     
        io.Fonts->Clear();
        // Default font will still be usable
        io.Fonts->AddFontDefault();
        ImFontConfig config;
        // Use our own stroking implmentation
        config.StrokeColor = IM_COL32(this->strokeColor.x * 255,this->strokeColor.y * 255, this->strokeColor.z * 255, 0);
        config.FillColor = IM_COL32(this->fillColor.x * 255, this->fillColor.y * 255, this->fillColor.z * 255, 0);
        LOG(L"Building font atlas,please wait...\n");
        if (this->fontnameJapanese) {
            io.Fonts->AddFontFromFileTTF(
                this->fontnameJapanese->c_str(),
                this->fontSize,
                &config, // Merge into this font.
                io.Fonts->GetGlyphRangesDefault()
            );
            config.MergeMode = true;
            io.Fonts->AddFontFromFileTTF(
                this->fontnameJapanese->c_str(),
                this->fontSize,
                &config,
                io.Fonts->GetGlyphRangesJapanese()
            );
        }
        ImVector<ImWchar> ranges;
        ImFontGlyphRangesBuilder builder;
       
        builder.AddText(this->charset.c_str());
        builder.BuildRanges(&ranges);

        if (this->fontnameChinese) {
            this->font = io.Fonts->AddFontFromFileTTF(
                this->fontnameChinese->c_str(),
                this->fontSize,
                &config,
                &ranges[0]
            );
        }        
        io.Fonts->Build();
        LOG(L"Built font atlas.\n");
        this->SetDefaultStyles();
    }

    void OnTick() {
        if (this->reloadFonts) { 
            // ImGUI Dx11 Implmentation doesn't reload font textures automatically
            // Force it to do so by recreating the context
            ImGui::DestroyContext(ImGui::GetCurrentContext());
            ImGui::CreateContext();
            ImGui::GetIO().ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

            this->RebuildFonts();
            ImGui_ImplWin32_Init(window);
            ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
            this->reloadFonts = false;
        }
    }

    void OnImGUI() {
        if (!this->showGUI) return;
        ImGui::Begin("Fonts [Q]");
        ImGui::Text("Current charset size (For Chinese): %d",this->charset.size());
        ImGui::Separator();
        ImGui::Text("Select fonts");

        if (ImGui::BeginCombo("Chinese", FilenameFromPath(*this->fontnameChinese).c_str())) {
            for (int i = 0; i < this->fontsAvailable.size();i++) {
                std::string *font = &fontsAvailable[i];
                const bool isSelected = (font == this->fontnameChinese);
                if (ImGui::Selectable(FilenameFromPath(*font).c_str(), isSelected)) {
                    this->fontnameChinese = font;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("Japanese", FilenameFromPath(*this->fontnameJapanese).c_str())) {
            for (int i = 0; i < this->fontsAvailable.size(); i++) {
                std::string* font = &fontsAvailable[i];
                const bool isSelected = (font == this->fontnameJapanese);
                if (ImGui::Selectable(FilenameFromPath(*font).c_str(), isSelected)) {
                    this->fontnameJapanese = font;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),"WARNING : Going for a large font size might crash texture creation procedure!");
        ImGui::SliderFloat("Font Size", &this->fontSize, 10, 80,"%.1f");
        if (ImGui::Button("Scale+")) ImGui::GetIO().FontGlobalScale += 0.1f;
        ImGui::SameLine();
        if (ImGui::Button("Scale-")) ImGui::GetIO().FontGlobalScale -= 0.1f;
        ImGui::ColorEdit4("FillColor", (float*)&this->fillColor, ImGuiColorEditFlags_NoAlpha);
        ImGui::ColorEdit4("StrokeColor", (float*)&this->strokeColor, ImGuiColorEditFlags_NoAlpha);
        if (ImGui::Button("Apply")) {
            // Only possible outside NewFrame()
            this->reloadFonts = true;
        }
        ImGui::End();
    }
} fontManager;

struct LyricManager {

    enum DisplayStatus {
        OnScreen,
        NoLyric
    };
    enum LyricDisplayType {
        RyhthmGame,
        PVMode,
        None
    };
    enum LyricDisplayStatus {
        Ended,
        Progressing,
        Paused,
        Ready
    };

private:
    std::wstring line;
    std::string songAudioName;

    uint32_t lastLyricsUpdateFrame;
    uint32_t lastUpdateFrame;
    uint32_t lastChangingValue;

    uint64_t lastRecordTime;
    uint64_t timeElapsedSincePVStart;

    DisplayStatus displayStatus;
    LyricDisplayType lyricDisplayType;
    LyricDisplayStatus lyricDisplayStatus;

    float lyricWindowOpacity = 0.0f;
    LyricDisplayType lyricSouldMoveType;

public:
    bool showGUI = true;

    void Init() {
        this->displayStatus = NoLyric;
        this->lyricDisplayType = None;
        this->lyricSouldMoveType = None;
        this->lyricDisplayStatus = Ended;
        this->lastLyricsUpdateFrame = 0;
    }
    uint64_t TimeElapsed() {
        return this->timeElapsedSincePVStart;
    }
    std::wstring CurrentLyricLine() {
        return this->line;
    }
    std::string GetSongAudio() {
        return this->songAudioName;
    }

    void ResetTimer() {
        this->timeElapsedSincePVStart = 0;
        this->lastRecordTime = millis();
    }
    void OnLyricsBegin() {
        LOG(
            L"Started with audio %S.\n",            
            this->songAudioName.c_str());        
    }
    void OnLyricsEnd() {
        LOG(L"Ended.\n");
        this->line.clear();        

        this->displayStatus = NoLyric;
        this->lyricDisplayStatus = Ended;
        this->lyricDisplayType = None;
    }
    void OnTick() {
        if (this->lyricDisplayStatus != Ended) {
            if (this->lastChangingValue != *ValueThatChangesWhilistPlaying) {
                this->lyricDisplayStatus = Progressing;
                this->lastChangingValue = *ValueThatChangesWhilistPlaying;
            }
            else if (this->lyricDisplayStatus != Ready)
                this->lyricDisplayStatus = Paused;
        }

        if (this->displayStatus == OnScreen && this->lyricDisplayStatus == Progressing) {
            if (this->timeElapsedSincePVStart == 0) // Occured right after ResetTimer();
                this->OnLyricsBegin();
            this->timeElapsedSincePVStart += millis() - this->lastRecordTime;
            this->lastRecordTime = millis();
        }

        if (this->lastUpdateFrame > *PVEventFrame) {
            this->ResetTimer();
        }

        if (*PVEventFrame - this->lastLyricsUpdateFrame > 30) // Linger for a while
            this->line.clear(); // No lyrics being displayed as in the original implmentation
        this->lastUpdateFrame = *PVEventFrame;

    }    
    void SetLyricLine(bool isLyric , char* src) {
        if (isLyric) {
            this->lastLyricsUpdateFrame = *PVEventFrame;
            this->line = u8string_to_wide_string(src);
        }
        else {
            this->lyricDisplayType = RyhthmGame;
        }
    }
    void SetSongAudio(const char* src) {
        this->songAudioName.assign(src);
    }
    void UpdateGameState(const char* state) {       
        if (this->displayStatus == OnScreen && (strcmp(state,"PVsel") == 0)) {
            this->ResetTimer();
            this->OnLyricsEnd();
        }else if (this->displayStatus != OnScreen && strcmp(state,"PV POST PROCESS TASK") == 0) {
            this->displayStatus = OnScreen;
            this->ResetTimer(); 
            this->lyricDisplayType = PVMode;
            this->lyricDisplayStatus = Ready;
        }
    }    
    bool ShowLyric() {
        return this->lyricDisplayStatus != Ended;
    }

    const char* GetCurrentLyricDisplayStatusString() {
        switch (this->lyricDisplayStatus) {
            case Ended: return "Ended";
            case Progressing: return "Progressing";
            case Paused: return "Paused";
            case Ready: return "Ready";
        }
        return "";
    }
    const char* GetCurrentLyricDisplayTypeString() {
        switch (this->lyricDisplayType) {
            case PVMode: return "PV";
            case RyhthmGame: return "Ryhthm Game";
            case None: return "None";               
        }
        return "";
    }

    void OnImGUI() {
        // Debug messages
        if (this->showGUI) {
            ImGui::Begin("Lyric [W]");
            ImGui::Text("Debug Status");
            ImGui::Separator();
            if (this->ShowLyric()) {
                ImGui::Text(
                    "Playing %s",
                    this->GetSongAudio().c_str()
                );
                ImGui::Text(
                    "Status:%s PvEvent:%d Type:%s Time:%.2f s",
                    this->GetCurrentLyricDisplayStatusString(),
                    *PVEventFrame,
                    this->GetCurrentLyricDisplayTypeString(),
                    this->TimeElapsed() / 1000.0f
                );
            }
            else {
                ImGui::Text("No Lyric Type:%s", this->GetCurrentLyricDisplayTypeString());
            }            
            ImGui::Separator();
            ImGui::Text("Window Styling");
            ImGui::SliderFloat("Opacity", &this->lyricWindowOpacity, 0.0f, 1.0f, "%.1f");
            ImGuiStyle& style = ImGui::GetStyle();
            ImGui::Text("Borders");
            ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::Text("Rounding");
            ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
            ImGui::Separator();
            ImGui::Text("Move Lyric Window");
            if (ImGui::Button("Move PV")) {
                this->lyricSouldMoveType = PVMode;
            }
            ImGui::SameLine();
            if (ImGui::Button("Ryhthm Game")) {
                this->lyricSouldMoveType = RyhthmGame;
            }
            ImGui::SameLine();
            if (ImGui::Button("Current")) {
                this->lyricSouldMoveType = this->lyricDisplayType;
            }
            ImGui::End();
        }
        ImGui::SetNextWindowBgAlpha(this->lyricWindowOpacity); // Transparent background
        if (this->lyricSouldMoveType != None) {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();            
            ImVec2 window_pos, window_pos_pivot;
            if (this->lyricSouldMoveType == PVMode) {
                window_pos = viewport->GetWorkCenter();
                window_pos.y = viewport->WorkSize.y * 0.9f;
                window_pos_pivot = ImVec2(0.5f, 0.5f);
                ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always,window_pos_pivot);
                if (this->lyricDisplayType == PVMode) this->lyricSouldMoveType = PVMode;
                // Keep in center in PVMode
            }
            else if (this->lyricSouldMoveType == RyhthmGame) {
                window_pos = viewport->GetWorkCenter();
                window_pos.x = viewport->WorkSize.x * 0.07f;
                window_pos.y = viewport->WorkSize.y * 0.93f;
                window_pos_pivot = ImVec2(0.0f, 0.5f);
                ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
                this->lyricSouldMoveType = None;
            }
        }
        if (this->ShowLyric() && (this->CurrentLyricLine().size() == 0 || this->CurrentLyricLine()[0] == ' '))
            return; // Hide the window when we want to display lyrics but there's nothing to show
        ImGui::Begin(
            "Lyric Overlay", NULL,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing
        );

        ImGui::PushFont(fontManager.font);
        if (!this->ShowLyric())
            ImGui::Text(LYRIC_PLACEHOLDER_MESSAGE);
        else
            ImGui::Text(wide_string_to_u8string(&this->CurrentLyricLine()[0]).c_str());
        ImGui::PopFont();        
        ImGui::End();
    }
} lyricManager;

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
    LOG(L"Audio loading %S.\n", a3->c_str());
    lyricManager.SetSongAudio(a3->c_str());
    char* result = original_LoadSongAudio(a1,a2,a3,a4,a5,a6,a7,a8);
    return result;
}

HOOK(INT64, __fastcall, _ChangeGameState, sigChangeGameState(), INT64* a, const char* state) {
	INT64 result = original_ChangeGameState(a, state);
	LOG(L"Gamestate changed to : %S\n", state);
    lyricManager.UpdateGameState(state);
	return result;
}

HOOK(void**, __fastcall, _RenderLyric, sigRenderLyricMaybe(),
    float type,
    float a2,
    unsigned int a3,
    int a4,
    char* text,
    char a6,
    unsigned int a7,
    void** a8)
{
    bool isLyric = type == 172.0f;    
    if (isLyric){        
        lyricManager.SetLyricLine(isLyric, text);
        return NULL;
    } else {
        lyricManager.SetLyricLine(isLyric, NULL);
        void** result = original_RenderLyric(type, a2, a3, a4, text, a6, a7, a8);
        return result;
    }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

#define VK_Q 0x51
#define VK_W 0x57
LRESULT WINAPI WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    /* Window procedure hook*/
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;    
    switch (uMsg) {  
        case WM_KEYDOWN:
            if (LOWORD(wParam) == VK_Q) fontManager.showGUI = !fontManager.showGUI;
            if (LOWORD(wParam) == VK_W) lyricManager.showGUI = !lyricManager.showGUI;
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
        ImGui::CreateContext();
        ImGui::GetIO().ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
        
        lyricManager.Init();
        fontManager.Init();

        ImGui_ImplWin32_Init(window);
        ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    }

    void __declspec(dllexport) Init() {
		INSTALL_HOOK(_ChangeGameState);
        INSTALL_HOOK(_RenderLyric);
        INSTALL_HOOK(_LoadSongAudio);
   
        LOG(L"Hooks installed.\n");           
    }

    void __declspec(dllexport) OnFrame(IDXGISwapChain* m_pSwapChain) {
        lyricManager.OnTick();
        fontManager.OnTick();

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame(); 
        
        lyricManager.OnImGUI();
        fontManager.OnImGUI();

        ImGui::Render();        
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
}