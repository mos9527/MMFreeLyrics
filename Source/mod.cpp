#include "pch.h"
#pragma warning( disable : 26495 26812 )
// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
// Data
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
static WNDPROC originalWndProc;
static HWND window;

#pragma region Constant
#define DEFAULT_CHARSET_NAME L"charset.txt"
#define LYRIC_PLACEHOLDER_MESSAGE u8"初音未来 Project DIVA メガー・ミクス"
#define CHARSET_NOTFOUND_WARNING_TITLE L"MegaMix+ Freetype Lyrics"
#define CHARSET_NOTFOUND_WARNING_CONTENT L"Charset file " DEFAULT_CHARSET_NAME " not found. \n\n\
If you're using non-English lyrics translation mod, This will probably cause some characters appears to be missing \n\n\
In which case, you should copy their pv_db.txt file here alongside the DLLs, and rename it into " DEFAULT_CHARSET_NAME" .\n\n\
Otherwise, it's safe to ignore this error."

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
const char* LanguageTypeStrings[] = { u8"日本語",u8"English",u8"简体中文",u8"繁體中文",u8"한국어",u8"Français",u8"Italiano",u8"Deutsch",u8"Español" };
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

struct FontManager {
private:
    std::vector<std::string> fontsAvailable;
    std::string * fontnameDefault;
    std::string * fontnameWithCharset;
    std::string charset;
    bool reloadFonts = false;

    float fontSize = 35.0f;
    float fontSizeImGui = 13.0f;
    ImVec4 fillColor = ImVec4(1,1,1,0);
    ImVec4 strokeColor = ImVec4(0,0,0,0);

public:
    // Current combined font used for lyrics renderer
    ImFont* font;  
    bool showGUI = true;
    void Init() {
        this->RefreshFontList();        
        if (this->fontsAvailable.size() > 0) {
            this->fontnameWithCharset = &this->fontsAvailable[0];
            this->fontnameDefault = &this->fontsAvailable[this->fontsAvailable.size() - 1];
        }
        this->BuildCharset();
        this->RebuildFonts();
    }
    void BuildCharset() {
        // Build charset via a hashtable
        std::ifstream f(to_utf8(FullFilenameUnderDLLPath(DEFAULT_CHARSET_NAME)));
        if (!f.is_open()) {
            MessageBoxW(
                window,
                CHARSET_NOTFOUND_WARNING_CONTENT,
                CHARSET_NOTFOUND_WARNING_TITLE,
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
        this->charset = to_utf8(charset_w);        
    }
    /* Retrives all available fonts under the fonts\ directroy. Results are alphabetically sorted. */
    std::vector<std::string>& RefreshFontList() {
        this->fontsAvailable.clear();
        for (auto file : std::filesystem::directory_iterator(to_utf8(FullFilenameUnderDLLPath(L"fonts\\")))) {
            std::wstring path = file.path();
            std::wstring ext = path.substr(path.length() - 3);
            if (ext == L"ttf" || ext == L"otf") {
                this->fontsAvailable.push_back(to_utf8(path));
            }
        }
        std::sort(this->fontsAvailable.begin(), this->fontsAvailable.end());
        return this->fontsAvailable;
    }
    void SetDefaultStyles() {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowBorderSize = 0.0f;
        style.WindowRounding = 12.0f;
        LOG(L"Reseted ImGui styles.\n");
    }
    void RebuildFonts() {        
        /* Fonts and glyph ranges*/
        ImGuiIO& io = ImGui::GetIO();     
        io.Fonts->Clear();
        // Default font will still be usable
        ImFontConfig config;
        config.SizePixels = this->fontSizeImGui;
        io.Fonts->AddFontDefault(&config);
        // Use our own stroking implmentation
        config.StrokeColor = IM_COL32(this->strokeColor.x * 255,this->strokeColor.y * 255, this->strokeColor.z * 255, 0);
        config.FillColor = IM_COL32(this->fillColor.x * 255, this->fillColor.y * 255, this->fillColor.z * 255, 0);
        LOG(L"Now rebuilding fonts atlas,please wait...\n");
        if (this->fontnameDefault) {
            io.Fonts->AddFontFromFileTTF(
                this->fontnameDefault->c_str(),
                this->fontSize,
                &config, // Merge into this font.
                io.Fonts->GetGlyphRangesDefault()
            );
            config.MergeMode = true;
            io.Fonts->AddFontFromFileTTF(
                this->fontnameDefault->c_str(),
                this->fontSize,
                &config,
                io.Fonts->GetGlyphRangesJapanese()
            );
        }
        ImVector<ImWchar> ranges;
        ImFontGlyphRangesBuilder builder;
        if (this->charset.size() > 0)
            builder.AddText(this->charset.c_str());
        else {
            LOG(L"Using fallback charset (Japanese / Romal) since custom charset isn't loaded.\n");
            builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
            builder.AddRanges(io.Fonts->GetGlyphRangesJapanese());
        }
        builder.BuildRanges(&ranges);
        if (this->fontnameWithCharset) {
            this->font = io.Fonts->AddFontFromFileTTF(
                this->fontnameWithCharset->c_str(),
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
        ImGui::Text("Current charset size (Custom): %d",this->charset.size());
        ImGui::Separator();
        ImGui::Text("Select fonts");

        if (ImGui::BeginCombo("Default (w/ Romal & Japanese charset)", FilenameFromPath(*this->fontnameDefault).c_str())) {
            for (int i = 0; i < this->fontsAvailable.size(); i++) {
                std::string* font = &fontsAvailable[i];
                const bool isSelected = (font == this->fontnameDefault);
                if (ImGui::Selectable(FilenameFromPath(*font).c_str(), isSelected)) {
                    this->fontnameDefault = font;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        
        if (ImGui::BeginCombo("Custom (w/ Custom Charset)", FilenameFromPath(*this->fontnameWithCharset).c_str())) {
            for (int i = 0; i < this->fontsAvailable.size();i++) {
                std::string *font = &fontsAvailable[i];
                const bool isSelected = (font == this->fontnameWithCharset);
                if (ImGui::Selectable(FilenameFromPath(*font).c_str(), isSelected)) {
                    this->fontnameWithCharset = font;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),"WARNING : Going for a large font size might crash texture creation procedure!");
        ImGui::SliderFloat("Font Size", &this->fontSize, 10, 80,"%.1f");
        ImGui::SliderFloat("ImGui Font Size", &this->fontSizeImGui, 10, 80, "%.1f");
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
    void Init() { }
    float TimeElapsed() {
        return *PVTimestamp;
    }
    std::wstring& CurrentLyricLine() {
        return this->line;
    }
    void SetSongAudio(const char* src) {
        this->songAudioName.assign(src);
        // TODO: find procedures where PV id is assigned instead of this hack
        std::string filename = this->songAudioName.substr(songAudioName.find_last_of('/') + 1);        
        sscanf_s(filename.c_str(),"pv_%d.ogg",&this->pvID);
    }
    std::string& GetSongAudio() {
        return this->songAudioName;
    }
    void OnLyricsBegin() {
        LOG(L"Started with audio %S.\n",this->songAudioName.c_str());        
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
            if (*PVPlaying){
                this->lyricDisplayStatus = Progressing;                
            } else if (this->lyricDisplayStatus != Ready)
                this->lyricDisplayStatus = Paused;
        }        
    }    
    void UpdateLyricIndex(int index) {
        this->lyricIndex = index;
        this->lyricUpdated = true;
        if (index == 0)
        {
            this->lock.lock();
            this->line.clear();
            this->lock.unlock();
        }
    }
    void SetLyricLine(bool isLyric , char* src) {
        if (this->lyricUpdated && isLyric) {
            this->lock.lock();           
            this->line = u8string_to_wide_string(src);
            this->lock.unlock();
        }
        else {
            this->lyricDisplayType = RyhthmGame;
        }
    }
    void UpdateGameState(const char* state) {       
        if (this->displayStatus == OnScreen && (strcmp(state,"PVsel") == 0)) {
            this->OnLyricsEnd();
        }else if (this->displayStatus != OnScreen && strcmp(state,"PV POST PROCESS TASK") == 0) {
            this->displayStatus = OnScreen;
            this->lyricDisplayType = PVMode;
            this->lyricDisplayStatus = Ready;
        }
    }    
    bool ShowLyric() {
        return this->lyricDisplayStatus != Ended;
    }
    void OnImGUI() {
        if (this->showGUI) {
            ImGui::Begin("Lyric [W]");
            ImGui::Text("Debug Stats for PV%d",this->pvID);
            ImGui::Separator();
            if (this->ShowLyric()) {               
                ImGui::Text("[%s] %s Lyric : %s Arranger : %s",
                    LanguageTypeStrings[(LanguageType)*Language], PVWaitScreenInfo->Name.c_str(), PVWaitScreenInfo->Lyrics.c_str(), PVWaitScreenInfo->Arranger.c_str()
                );
                ImGui::Separator();
                ImGui::Text(
                    "Audio %s",
                    this->GetSongAudio().c_str()
                );
                ImGui::Text(
                    "Status:%s LyricId:%d Event:%d Type:%s Time:%.2f s",
                    LyricDisplayStatusStrings[this->lyricDisplayStatus],
                    this->lyricIndex,
                    *PVEvent,
                    LyricDisplayTypeStrings[this->lyricDisplayType],
                    this->TimeElapsed()
                );
            }
            else {
                ImGui::Text("No Lyric Type:%s", LyricDisplayTypeStrings[this->lyricDisplayType]);
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
                window_pos.y = viewport->WorkSize.y * 0.85f;
                window_pos_pivot = ImVec2(0.5f, 0.5f);
                ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always,window_pos_pivot);
                if (this->lyricDisplayType == PVMode) this->lyricSouldMoveType = PVMode;
                // Keep in center in PVMode
            }
            else if (this->lyricSouldMoveType == RyhthmGame) {
                window_pos = viewport->GetWorkCenter();
                window_pos.x = viewport->WorkSize.x * 0.07f;
                window_pos.y = viewport->WorkSize.y * 0.91f;
                window_pos_pivot = ImVec2(0.0f, 0.5f);
                ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
                this->lyricSouldMoveType = None;
            }
        }
        if (this->ShowLyric() && this->lyricIndex != 0)
            return; // Hide the window when we want to display lyrics but there's nothing to show
        ImGui::Begin(
            "Lyric Overlay", NULL,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing
        );
        ImGui::PushFont(fontManager.font);
        if (!this->ShowLyric())
            ImGui::Text(LYRIC_PLACEHOLDER_MESSAGE);
        else {
            this->lock.lock();
            ImGui::Text(wide_string_to_u8string(&this->CurrentLyricLine()[0]).c_str());
            this->lock.unlock();
        }
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
HOOK(void**, __fastcall, _RenderLyricAndTitle, sigRenderLyricAndTitle(),
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
        void** result = original_RenderLyricAndTitle(type, a2, a3, a4, text, a6, a7, a8);
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
        ImGui::GetIO().ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard;
        
        lyricManager.Init();
        fontManager.Init();

        ImGui_ImplWin32_Init(window);
        ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    }
    void __declspec(dllexport) Init() {
		INSTALL_HOOK(_ChangeGameState);
        INSTALL_HOOK(_RenderLyricAndTitle);
        INSTALL_HOOK(_LoadSongAudio);
        INSTALL_HOOK(_UpdateLyrics);
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