#include "pch.h"
#include "config.h"
#pragma warning( disable : 26495 26812 )
// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


#pragma region Constant
#define WINDOW_SAVE_SELECTOR ImGui::Text("Configuration"); \
ImGui::Text("Config for both windows (and their posistions) can be saved/loaded anytime via Ctrl+S/Ctrl+L."); \
if (ImGui::Button("Save")) SaveGlobalConfig(); \
ImGui::SameLine(); \
if (ImGui::Button("Load")) LoadGlobalConfig();

enum LanguageType {
    Japanese = 0,
    English = 1,
    SChinese = 2,
    TChinese = 3,
    Korean = 4,
    French = 5,
    Italian = 6,
    German = 7,
    Spanish = 8
};
const uint8_t* Language = (uint8_t*)0x14CC105F4;
const uint32_t* PVEvent = (uint32_t*)0x1412EE324;
const uint32_t* PVPlaying = (uint32_t*)0x1412F0258;
const float* PVTimestamp = (float*)0x1412EE340;
struct PVWaitScreenInfoStruct {
    std::string Name;
    std::string Music;      // .music
    std::string MusicVideo; // .illustrator
    std::string Lyrics;     // .lyrics
    std::string Arranger;   // .arranger
    std::string Manipulator;// .manipulator
    std::string PV;         // .pv_editor
    std::string GuitarPlayer;//.guitar_player
};
PVWaitScreenInfoStruct* PVWaitScreenInfo = (PVWaitScreenInfoStruct*)0x14CC0B5F8;
const char* LanguageTypeStrings[] = { u8"Japanese",u8"English",u8"SChinese",u8"TChinese",u8"Korean",u8"French",u8"Italian",u8"German",u8"Spanish" };
const char* DisplayStatusStrings[] = { "On Screen","Not available" };
const char* LyricDisplayTypeStrings[] = { "Ryhthm Game","PV Viewer","Not available" };
const char* LyricDisplayStatusStrings[] = { "Ended","Playing","Paused","Ready" };
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
#pragma endregion
void SetImGuiDefaultFlags() {
    ImGui::GetIO().ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::GetIO().IniFilename = NULL;
}
void SaveGlobalConfig();
void LoadGlobalConfig();
class FontManager {
private:
    std::vector<std::string> fontsAvailable;
    std::string * fontnameDefault;
    std::string * fontnameWithCharset;
    std::string charset;
    bool reloadFonts = false;

    float fontSize;
    float fontSizeImGui;
    ImVec4 fillColor;
    ImVec4 strokeColor;

public:
    // Current combined font used for lyrics renderer
    ImFont* font;  
    bool showGUI = true;
    void Init(Config& cfg) {
        RefreshFontList();        
        if (fontsAvailable.size() > 0) {
            fontnameWithCharset = &fontsAvailable[0];
            fontnameDefault = &fontsAvailable[fontsAvailable.size() - 1];
            // Assign default fonts by alphabetical order
        }
        FromConfig(cfg);
        BuildCharset();
        RebuildFonts();
    }
    
    Config& FromConfig(Config& cfg) {
        // Fonts should be available. If not, fallback values are used.
        for (int i = 0; i < fontsAvailable.size();i++) {
            if (fontsAvailable[i] == cfg.fontnameDefault) fontnameDefault = &fontsAvailable[i];
            if (fontsAvailable[i] == cfg.fontnameWithCharset) fontnameWithCharset = &fontsAvailable[i];
        }
        // Font Styles
        fontSize = cfg.fontSize;
        fontSizeImGui = cfg.fontSizeImGui;
        ImGui::GetIO().FontGlobalScale = cfg.FontGlobalScale;
        fillColor = cfg.fillColor;
        strokeColor = cfg.strokeColor;
        showGUI = cfg.showFontManagerGUI;
        return cfg;
    }
    Config& ToConfig(Config& cfg) {        
        // Fonts are saved with filenames
        cfg.fontnameDefault = *fontnameDefault;
        cfg.fontnameWithCharset = *fontnameWithCharset;
        // Font Styles
        cfg.fontSize = fontSize;
        cfg.fontSizeImGui = fontSizeImGui;
        cfg.FontGlobalScale = ImGui::GetIO().FontGlobalScale;
        cfg.fillColor = fillColor;
        cfg.strokeColor = strokeColor;
        cfg.showFontManagerGUI = showGUI;
        return cfg;
    }
    
    void BuildCharset() {
        // Build charset via a hashtable
        std::ifstream f(to_utf8(FullpathInDllFolder(DEFAULT_CHARSET_NAME)));
        if (!f.is_open()) {
            MessageBoxW(
                window,
                CHARSET_NOTFOUND_WARNING_CONTENT,
                MESSAGEBOX_TITLE,
                MB_ICONEXCLAMATION
            );
            return;
        }
        std::stringstream charbuf; charbuf << f.rdbuf();
        std::wstring chars = to_wstr(charbuf.str());
        std::wstring charset_w;
        std::map<wchar_t, int> table;
        for (auto& c : chars) table[c] = 1;
        for (auto& c : table) charset_w.push_back(c.first);
        LOG(L"%zd unique characters (out of %zd) will be built into atlas.\n", charset_w.size(), chars.size());
        charset = to_utf8(charset_w);        
    }
    /* Retrives all available fonts under the fonts\ directroy. Results are alphabetically sorted. */
    std::vector<std::string>& RefreshFontList() {
        fontsAvailable.clear();
        for (auto file : std::filesystem::directory_iterator(to_utf8(FullpathInDllFolder(L"fonts\\")))) {
            std::wstring path = file.path();
            std::wstring ext = path.substr(path.length() - 3);
            if (ext == L"ttf" || ext == L"otf") {
                fontsAvailable.push_back(to_utf8(path));
            }
        }
        std::sort(fontsAvailable.begin(), fontsAvailable.end());
        return fontsAvailable;
    }
    void RebuildFonts() {                
        ImGuiIO& io = ImGui::GetIO();     
        io.Fonts->Clear();
        // Default font will still be usable
        ImFontConfig config;
        config.SizePixels = fontSizeImGui;
        io.Fonts->AddFontDefault(&config);
        // Use our own stroking implmentation
        config.StrokeColor = IM_COL32(strokeColor.x * 255,strokeColor.y * 255, strokeColor.z * 255, 0);
        config.FillColor = IM_COL32(fillColor.x * 255, fillColor.y * 255, fillColor.z * 255, 0);
        LOG(L"Now rebuilding fonts atlas,please wait...\n");
        if (fontnameDefault) {
            io.Fonts->AddFontFromFileTTF(
                fontnameDefault->c_str(),
                fontSize,
                &config, // Merge into this font.
                io.Fonts->GetGlyphRangesDefault()
            );
            config.MergeMode = true;
            io.Fonts->AddFontFromFileTTF(
                fontnameDefault->c_str(),
                fontSize,
                &config,
                io.Fonts->GetGlyphRangesJapanese()
            );
        }
        ImVector<ImWchar> ranges;
        ImFontGlyphRangesBuilder builder;
        if (charset.size() > 0)
            builder.AddText(charset.c_str());
        else {
            LOG(L"Using fallback charset (Japanese / Romal) since custom charset isn't loaded.\n");
            builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
            builder.AddRanges(io.Fonts->GetGlyphRangesJapanese());
        }
        builder.BuildRanges(&ranges);
        if (fontnameWithCharset) {
            font = io.Fonts->AddFontFromFileTTF(
                fontnameWithCharset->c_str(),
                fontSize,
                &config,
                &ranges[0]
            );
        }        
        io.Fonts->Build();
        LOG(L"Built font atlas.\n");
    }
    void OnTick() {
        if (reloadFonts) { 
            // ImGUI Dx11 Implmentation doesn't reload font textures automatically
            // Force it to do so by recreating the context
            ImGui::DestroyContext(ImGui::GetCurrentContext());
            ImGui::CreateContext();
            SetImGuiDefaultFlags();
            RebuildFonts();
            ImGui_ImplWin32_Init(window);
            ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
            LoadGlobalConfig();
            reloadFonts = false;
        }
    }
    void OnImGUI() {
        if (!showGUI) return;
        ImGui::Begin("Fonts [Ctrl+F]");
        ImGui::Text("Current charset size (Custom): %d",charset.size());
        ImGui::Separator();
        ImGui::Text("Select fonts");

        if (ImGui::BeginCombo("Default (w/ Romal & Japanese charset)", FilenameFromPath(*fontnameDefault).c_str())) {
            for (int i = 0; i < fontsAvailable.size(); i++) {
                std::string* font = &fontsAvailable[i];
                const bool isSelected = (font == fontnameDefault);
                if (ImGui::Selectable(FilenameFromPath(*font).c_str(), isSelected)) {
                    fontnameDefault = font;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        
        if (ImGui::BeginCombo("Custom (w/ Custom Charset)", FilenameFromPath(*fontnameWithCharset).c_str())) {
            for (int i = 0; i < fontsAvailable.size();i++) {
                std::string *font = &fontsAvailable[i];
                const bool isSelected = (font == fontnameWithCharset);
                if (ImGui::Selectable(FilenameFromPath(*font).c_str(), isSelected)) {
                    fontnameWithCharset = font;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),"WARNING : Going for a large font size might crash texture creation procedure!");
        ImGui::SliderFloat("Font Size", &fontSize, 10, 80,"%.1f");
        ImGui::SliderFloat("ImGui Font Size", &fontSizeImGui, 10, 80, "%.1f");
        ImGui::Text("Global Scaling: %.1f", ImGui::GetIO().FontGlobalScale);
        if (ImGui::Button("Scale+")) ImGui::GetIO().FontGlobalScale += 0.1f;
        ImGui::SameLine();
        if (ImGui::Button("Scale-")) ImGui::GetIO().FontGlobalScale -= 0.1f;
        ImGui::ColorEdit4("FillColor", (float*)&fillColor, ImGuiColorEditFlags_NoAlpha);
        ImGui::ColorEdit4("StrokeColor", (float*)&strokeColor, ImGuiColorEditFlags_NoAlpha);
        WINDOW_SAVE_SELECTOR;
        ImGui::SameLine();
        if (ImGui::Button("Save & Apply")) {
            // Only possible outside NewFrame()
            SaveGlobalConfig();
            reloadFonts = true;
        }
        ImGui::End();
    }
} fontManager;
class LyricManager {

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
    std::mutex lock;
    std::string songAudioName;

    int pvID=0;
    int lyricIndex=0;
    bool lyricUpdated = false;

    DisplayStatus displayStatus = NoLyric;
    LyricDisplayType lyricDisplayType = None;
    LyricDisplayStatus lyricDisplayStatus = Ended;

    float lyricWindowOpacity = 0.0f;
    LyricDisplayType lyricSouldMoveType = None;

public:
    bool showGUI = true;
    void Init(Config& cfg) {
        FromConfig(cfg);
    }
    Config& FromConfig(Config& cfg) {
        lyricWindowOpacity = cfg.lyricWindowOpacity;
        showGUI = cfg.showLyricsManagerGUI;
        ImGui::GetStyle().WindowBorderSize = cfg.WindowBorderSize;
        ImGui::GetStyle().WindowRounding = cfg.WindowRounding;
        return cfg;
    }
    Config& ToConfig(Config& cfg) {
        cfg.lyricWindowOpacity = lyricWindowOpacity;
        cfg.showLyricsManagerGUI = showGUI;
        cfg.WindowBorderSize = ImGui::GetStyle().WindowBorderSize;
        cfg.WindowRounding = ImGui::GetStyle().WindowRounding;
        return cfg;
    }
    float TimeElapsed() {
        return *PVTimestamp;
    }
    std::wstring& CurrentLyricLine() {
        return line;
    }
    void SetSongAudio(const char* src) {
        songAudioName.assign(src);
        // TODO: find procedures where PV id is assigned instead of this hack
        std::string filename = FilenameFromPath(src);
        sscanf_s(filename.c_str() + 1,"pv_%d.ogg",&pvID);
    }
    std::string& GetSongAudio() {
        return songAudioName;
    }
    void OnLyricsBegin() {
        LOG(L"Started with audio %S.\n",songAudioName.c_str());        
    }
    void OnLyricsEnd() {
        LOG(L"Ended.\n");
        line.clear();        

        displayStatus = NoLyric;
        lyricDisplayStatus = Ended;
        lyricDisplayType = None;
    }
    void OnTick() {
        if (lyricDisplayStatus != Ended) {
            if (*PVPlaying){
                lyricDisplayStatus = Progressing;                
            } else if (lyricDisplayStatus != Ready)
                lyricDisplayStatus = Paused;
        }        
    }    
    void UpdateLyricIndex(int index) {
        lyricIndex = index;
        lyricUpdated = true;
        if (index == 0)
        {
            lock.lock();
            line.clear();
            lock.unlock();
        }
    }
    void SetLyricLine(bool isLyric , char* src) {
        if (lyricUpdated && isLyric) {
            lock.lock();           
            line = u8string_to_wide_string(src);
            lock.unlock();
        }
        else {
            lyricDisplayType = RyhthmGame;
        }
    }
    void UpdateGameState(const char* state) {       
        if (displayStatus == OnScreen && (strcmp(state,"PVsel") == 0)) {
            OnLyricsEnd();
        }else if (displayStatus != OnScreen && strcmp(state,"PV POST PROCESS TASK") == 0) {
            displayStatus = OnScreen;
            lyricDisplayType = PVMode;
            lyricDisplayStatus = Ready;
        }
    }    
    bool ShowLyric() {
        return lyricDisplayStatus != Ended;
    }
    void OnImGUI() {
        if (showGUI) {
            ImGui::Begin("Lyric [Ctrl+G]");
            if (!ShowLyric()) ImGui::Text("Idle");
            ImGui::Separator();
            if (ShowLyric()) {  
                ImGui::Text("Now Playing [PV %d]", pvID);
                ImGui::PushFont(fontManager.font);
                ImGui::Text("%s", PVWaitScreenInfo->Name.c_str());
                ImGui::PopFont();
                ImGui::Separator();
                ImGui::Text("[%s] Music/%s MV/%s Lyrics/%s",
                    LanguageTypeStrings[(LanguageType)*Language],
                    PVWaitScreenInfo->Music.c_str(),
                    PVWaitScreenInfo->MusicVideo.c_str(),
                    PVWaitScreenInfo->Lyrics.c_str()
                );
                ImGui::Text("Arranger/%s Manipulator/%s PV/%s Guitar/%s",
                    PVWaitScreenInfo->Arranger.c_str(),
                    PVWaitScreenInfo->Manipulator.c_str(),
                    PVWaitScreenInfo->PV.c_str(),
                    PVWaitScreenInfo->GuitarPlayer.c_str()
                );
                ImGui::Separator();
                ImGui::Text(
                    "Audio %s",
                    GetSongAudio().c_str()
                );
                ImGui::Text(
                    "Status:%s LyricId:%d Event:%d Type:%s Time:%.2f s",
                    LyricDisplayStatusStrings[lyricDisplayStatus],
                    lyricIndex,
                    *PVEvent,
                    LyricDisplayTypeStrings[lyricDisplayType],
                    TimeElapsed()
                );
            }
            else {
                ImGui::Text("No Lyric Type:%s", LyricDisplayTypeStrings[lyricDisplayType]);
            }            
            ImGui::Separator();
            ImGui::Text("Window Styling");
            ImGui::SliderFloat("Opacity", &lyricWindowOpacity, 0.0f, 1.0f, "%.1f");
            ImGuiStyle& style = ImGui::GetStyle();
            ImGui::Text("Borders");
            ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::Text("Rounding");
            ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
            ImGui::Separator();
            ImGui::Text("Move Lyric Window");
            if (ImGui::Button("PV (Horizontally Centered)")) {
                lyricSouldMoveType = PVMode;
            }
            ImGui::SameLine();
            if (ImGui::Button("Ryhthm Game")) {
                lyricSouldMoveType = RyhthmGame;
            }
            ImGui::SameLine();
            if (ImGui::Button("Current")) {
                lyricSouldMoveType = lyricDisplayType;
            }
            ImGui::Separator();
            WINDOW_SAVE_SELECTOR;
            ImGui::End();
        }
        ImGui::SetNextWindowBgAlpha(lyricWindowOpacity); // Transparent background
        if (lyricSouldMoveType != None) {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();            
            ImVec2 window_pos, window_pos_pivot;
            if (lyricSouldMoveType == PVMode) {
                window_pos = viewport->GetWorkCenter();
                window_pos.y = viewport->WorkSize.y * 0.85f;
                window_pos_pivot = ImVec2(0.5f, 0.5f);
                ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always,window_pos_pivot);
                if (lyricDisplayType == PVMode) lyricSouldMoveType = PVMode;
                // Keep in center in PVMode
            }
            else if (lyricSouldMoveType == RyhthmGame) {
                window_pos = viewport->GetWorkCenter();
                window_pos.x = viewport->WorkSize.x * 0.07f;
                window_pos.y = viewport->WorkSize.y * 0.91f;
                window_pos_pivot = ImVec2(0.0f, 0.5f);
                ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
                lyricSouldMoveType = None;
            }
        }
        if (ShowLyric() && lyricIndex <= 0)
            return; // Hide the window when we want to display lyrics but there's nothing to show
        ImGui::Begin(
            "Lyric Overlay", NULL,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing
        );
        ImGui::PushFont(fontManager.font);
        if (!ShowLyric()) {
            if (showGUI) ImGui::Text(LYRIC_PLACEHOLDER_MESSAGE);
        } else {
            lock.lock();
            ImGui::Text(to_utf8(CurrentLyricLine()).c_str());
            lock.unlock();
        }
        ImGui::PopFont();        
        ImGui::End();
    }
} lyricManager;
void SaveGlobalConfig() {
    fontManager.ToConfig(globalConfig);
    lyricManager.ToConfig(globalConfig);
    globalConfig.ImGuiSettings = ImGui::SaveIniSettingsToMemory();
    globalConfig.Save();
}
void LoadGlobalConfig() {
    globalConfig.Load();
    fontManager.FromConfig(globalConfig);
    lyricManager.FromConfig(globalConfig);
    if (globalConfig.ImGuiSettings.size() > 0)
        ImGui::LoadIniSettingsFromMemory(globalConfig.ImGuiSettings.c_str());
}
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
        lyricManager.SetLyricLine(isLyric, text);
        return NULL;
    } else {        
        void** result = original_RenderLyricAndTitle(x, y, scale, a4, text, a6, a7, a8);
        return result;
    }
}
HOOK(INT64, __fastcall, _UpdateLyrics, sigUpdateLyrics(), void* this_, int index, void* a3) {
    INT64 result = original_UpdateLyrics(this_,index,a3);
    lyricManager.UpdateLyricIndex(index);
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
#define VK_S 0x53
#define VK_L 0x4C
#define KEY_PRESSED(K) LOWORD(wParam) == K
#define KEY_PRESSED_WITH_CONTROL(K) KEY_PRESSED(K) && GetKeyState(VK_CONTROL)
LRESULT WINAPI WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    /* Window procedure hook*/
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;    
    switch (uMsg) {  
        case WM_KEYDOWN:
            if (KEY_PRESSED_WITH_CONTROL(VK_F))
                fontManager.showGUI = !fontManager.showGUI;
            if (KEY_PRESSED_WITH_CONTROL(VK_G))
                lyricManager.showGUI = !lyricManager.showGUI;
            if (KEY_PRESSED_WITH_CONTROL(VK_S)) {
                SaveGlobalConfig();
                MessageBeep(MB_ICONINFORMATION);
            }
            if (KEY_PRESSED_WITH_CONTROL(VK_L)) {
                LoadGlobalConfig();
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
        
        LoadGlobalConfig();
        lyricManager.Init(globalConfig);
        fontManager.Init(globalConfig);

        ImGui_ImplWin32_Init(window);
        ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    }
    void __declspec(dllexport) Init() {
		INSTALL_HOOK(_ChangeGameState);
        INSTALL_HOOK(_RenderLyricAndTitle);
        INSTALL_HOOK(_LoadSongAudio);
        INSTALL_HOOK(_UpdateLyrics);
        ImGui::CreateContext();
        SetImGuiDefaultFlags();
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