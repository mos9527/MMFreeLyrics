#include <globals.h>

void FontManager::Init(Config& cfg) {
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

Config& FontManager::FromConfig(Config& cfg) {
    // Fonts should be available. If not, fallback values are used.
    for (int i = 0; i < fontsAvailable.size(); i++) {
        if (fontsAvailable[i] == cfg.fontnameDefault) fontnameDefault = &fontsAvailable[i];
        if (fontsAvailable[i] == cfg.fontnameWithCharset) fontnameWithCharset = &fontsAvailable[i];
    }
    // Font Styles
    fontSize = cfg.fontSize;
    fontSizeImGui = cfg.fontSizeImGui;
    isBold = cfg.isBold;
    isItalic = cfg.isItalic;
    ImGui::GetIO().FontGlobalScale = cfg.FontGlobalScale;
    fillColor = cfg.fillColor;
    strokeColor = cfg.strokeColor;
    shadowColor = cfg.shadowColor;
    strokeSize = cfg.strokeSize;
    shadowOffset = cfg.shadowOffset;
    showGUI = cfg.showFontManagerGUI;
    return cfg;
}
Config& FontManager::ToConfig(Config& cfg) {
    // Fonts are saved with filenames
    cfg.fontnameDefault = *fontnameDefault;
    cfg.fontnameWithCharset = *fontnameWithCharset;
    // Font Styles
    cfg.fontSize = fontSize;
    cfg.fontSizeImGui = fontSizeImGui;
    cfg.isBold = isBold;
    cfg.isItalic = isItalic;
    cfg.FontGlobalScale = ImGui::GetIO().FontGlobalScale;
    cfg.fillColor = fillColor;
    cfg.strokeColor = strokeColor;
    cfg.shadowColor = shadowColor;
    cfg.strokeSize = strokeSize;
    cfg.shadowOffset = shadowOffset;
    cfg.showFontManagerGUI = showGUI;
    return cfg;
}

/* Build charset containing every character the renderer could ever display.
- This is based on the charset.txt file provided. 
- Normally it should contain the pv_db.txt from any lyrics mod you're using
- Otherwise, it would use a fallback charset which would cover most Japanese characters & Romal characters
*/
void FontManager::BuildCharset() {
    std::ifstream f(to_utf8(FullPathInDllFolder(DEFAULT_CHARSET_NAME)));
    if (!f.is_open()) {
        MessageBoxW(
            window,
            CHARSET_NOTFOUND_WARNING_CONTENT,
            MESSAGEBOX_TITLE,
            MB_ICONEXCLAMATION
        );
        return;
    }
    LOG(L"Enumerating unique characters...");
    std::stringstream charbuf; charbuf << f.rdbuf();
    std::wstring chars = to_wstr(charbuf.str());
    std::wstring charset_w;
    std::map<wchar_t, int> table;
    for (auto& c : chars) table[c] = 1;
    for (auto& c : table) charset_w.push_back(c.first);
    LOG(L"%zd unique characters (out of %zd) will be built into atlas.", charset_w.size(), chars.size());
    charset = to_utf8(charset_w);
}

/* Retrives all available fonts under the fonts\ directroy. Results are alphabetically sorted. */
std::vector<std::string>& FontManager::RefreshFontList() {
    fontsAvailable.clear();
    for (auto file : std::filesystem::directory_iterator(to_utf8(FullPathInDllFolder(L"fonts\\")))) {
        std::wstring path = file.path();
        std::wstring ext = path.substr(path.length() - 3);
        if (ext == L"ttf" || ext == L"otf") {
            fontsAvailable.push_back(to_utf8(path));
        }
    }
    std::sort(fontsAvailable.begin(), fontsAvailable.end());
    return fontsAvailable;
}

/* Build atlas based on current charset and fonts.
- Fonts DO NOT get re-rendered on every frame. Including styles, etc.
- They get put on a texture here ONCE and would not change unless this function is called again.
- which will update the fonts based on current settings.
- TL;DR: If you made any change to the settings and want to see in on screen, this function MUST be called afterwards.
- And do it outside ImGui's NewFrame() scope.
-
- NOTE : Do not use offical DearImGui repo to build since it does not contain some new features I introduced to
-        this version of ImGui.
*/
void FontManager::RebuildFonts() {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    // Default font will still be usable
    ImFontConfig config;
    config.SizePixels = fontSizeImGui;
    io.Fonts->AddFontDefault(&config);
    // Setting up styles
    config.ShadowOffset = shadowOffset;
    config.StrokeSize = strokeSize;
    if (isBold) config.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_Bold;
    if (isItalic) config.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_Oblique;
    config.StrokeColor = VEC4_TO_COL32(strokeColor);
    config.FillColor = VEC4_TO_COL32(fillColor);
    config.ShadowColor = VEC4_TO_COL32(shadowColor);

    LOG(L"Now rebuilding fonts atlas,please wait...");
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
        LOG(L"Using fallback charset (Japanese / Romal) since custom charset isn't loaded.");
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
    LOG(L"Built font atlas.");
}

/* Called by DX Hook's OnFrame. Do not override.*/
void FontManager::OnFrame() {
    if (reloadFonts) {
        // ImGUI Dx11 Implmentation doesn't reload font textures automatically
        // Force it to do so by recreating the context
        ImGui::DestroyContext(ImGui::GetCurrentContext());
        ImGui::CreateContext();
        SET_IMGUI_DEFAULT_FLAGS;
        RebuildFonts();
        ImGui_ImplWin32_Init(window);
        ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
        Config::LoadGlobalConfig();
        reloadFonts = false;
    }
}

/* Called by DX Hook's OnFrame. Do not override.*/
void FontManager::OnImGUI() {
    if (!showGUI) return;
    ImGui::Begin("Fonts [Ctrl+F]");
    ImGui::Text("Note : The settings here don't reflect automatically. You need to apply it first.");
    ImGui::Text("Current charset size (Custom): %d", charset.size());
    ImGui::Text("Fonts");
    if (ImGui::BeginCombo("Default (w/ Romal & Japanese charset)", FileNameFromPath(*fontnameDefault).c_str() + 1)) {
        for (int i = 0; i < fontsAvailable.size(); i++) {
            std::string* font = &fontsAvailable[i];
            const bool isSelected = (font == fontnameDefault);
            if (ImGui::Selectable(FileNameFromPath(*font).c_str(), isSelected)) {
                fontnameDefault = font;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    if (ImGui::BeginCombo("Fallback (w/ Custom Charset)", FileNameFromPath(*fontnameWithCharset).c_str() + 1)) {
        for (int i = 0; i < fontsAvailable.size(); i++) {
            std::string* font = &fontsAvailable[i];
            const bool isSelected = (font == fontnameWithCharset);
            if (ImGui::Selectable(FileNameFromPath(*font).c_str(), isSelected)) {
                fontnameWithCharset = font;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::Separator();
    ImGui::Text("Font Size");
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "WARNING : Going for a large font size might crash the texture creation procedure!");
    ImGui::SliderFloat("Font Size", &fontSize, 10, 80, "%.1f");
    ImGui::SliderFloat("ImGui Font Size", &fontSizeImGui, 10, 80, "%.1f");
    ImGui::SliderInt("Stroke Size", &strokeSize, 0, 10);

    ImGui::Separator();
    ImGui::Text("Shadows");
    ImGui::Text("Shadow X=%.0f", shadowOffset.x);
    ImGui::SameLine();
    if (ImGui::Button("  Left  ")) shadowOffset.x = -2;
    ImGui::SameLine();
    if (ImGui::Button("H.Center")) shadowOffset.x = 0;
    ImGui::SameLine();
    if (ImGui::Button("  Right ")) shadowOffset.x = 2;

    ImGui::Text("Shadow Y=%.0f", shadowOffset.y);
    ImGui::SameLine();
    if (ImGui::Button("  Top   ")) shadowOffset.y = -2;
    ImGui::SameLine();
    if (ImGui::Button("V.Center")) shadowOffset.y = 0;
    ImGui::SameLine();
    if (ImGui::Button(" Bottom ")) shadowOffset.y = 2;

    ImGui::Separator();
    ImGui::Text("Font Colors & Misc");
    ImGui::ColorEdit4("Fill", (float*)&fillColor);
    ImGui::ColorEdit4("Stroke", (float*)&strokeColor);
    ImGui::ColorEdit4("Shadow", (float*)&shadowColor);
    ImGui::Checkbox("Bold", &isBold);
    ImGui::SameLine();
    ImGui::Checkbox("Italic", &isItalic);
    ImGui::Text("Scaling: %.1f", ImGui::GetIO().FontGlobalScale);
    if (ImGui::Button("Scale+")) ImGui::GetIO().FontGlobalScale += 0.1f;
    ImGui::SameLine();
    if (ImGui::Button("Scale-")) ImGui::GetIO().FontGlobalScale -= 0.1f;
    ImGui::Separator();
    SHOW_WINDOW_SAVE_SELECTOR;
    ImGui::SameLine();
    if (ImGui::Button("Save & Apply")) {
        // Only possible outside NewFrame()
        Config::SaveGlobalConfig();
        reloadFonts = true;
    }
    ImGui::End();
}
FontManager FontManager_Inst;