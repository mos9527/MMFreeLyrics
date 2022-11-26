#include <globals.h>
#include <managers/font_manager.h>

void FontManager::Init(Config& cfg) {
    if (!isInit) {
        RefreshFontList();
        if (fontsAvailable.size() > 0) {
            fontnameWithCharset = &fontsAvailable[0];
            fontnameDefault = &fontsAvailable[fontsAvailable.size() - 1];
            // Assign default fonts by alphabetical order
        }
        FromConfig(cfg);
        isInit = true;
    }
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
- This will APPEND all unique characters of the string to the charset.
- Normally it should contain the pv_db.txt from any lyrics mod you're using
- Otherwise, it would use a fallback charset which would cover most Japanese characters & Romal characters
*/
int FontManager::UpdateCharset(std::wstring chars) {    
    LOG(L"Enumerating unique characters...");
    size_t size_before = charset.size();    
    for (auto& c : chars) if (c) charset[c] = 1;
    LOG(L"%zd new unique characters (out of %zd) will be built into atlas.", charset.size() - size_before, chars.size());
    return (int)(charset.size() - size_before);
}

/* Retrives all available fonts under the fonts\ directroy. Results are alphabetically sorted. */
std::vector<std::string>& FontManager::RefreshFontList() {
    fontsAvailable.clear();
    try {
        for (auto file : std::filesystem::directory_iterator(to_utf8(FullPathInDllFolder(L"fonts\\")))) {
            std::wstring path = file.path();
            std::wstring ext = path.substr(path.length() - 3);
            if (ext == L"ttf" || ext == L"otf") {
                fontsAvailable.push_back(to_utf8(path));
            }
    }   
        std::sort(fontsAvailable.begin(), fontsAvailable.end());
    }
    catch (std::filesystem::filesystem_error e){
        MessageBoxW(
            window,
            FONTS_FOLDER_NOT_FOUND,
            MESSAGEBOX_TITLE,
            MB_ICONEXCLAMATION
        );
    }
    return fontsAvailable;
}

/* Build atlas based on current charset and fonts.
- Fonts DO NOT get re-rendered on every frame. Including styles, etc.
- They get put on a texture here ONCE and would not change unless this function is called again.
- which will update the fonts based on current settings.
- TL;DR: If you made any change to the settings and want to see in on screen, this function MUST be called afterwards.
- And do it outside ImGui's NewFrame() scope. In our instance just set `reloadFonts` to true
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
    if (charset.size() > 0) {
        std::wstring charset_buffer;
        for (auto& c : charset) charset_buffer.push_back(c.first);
        builder.AddText(to_utf8(charset_buffer).c_str());
    }
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
        ImGui_ImplDX11_Shutdown();
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
    ImGui::SetNextWindowBgAlpha(0.5);
    ImGui::Begin("Fonts [Ctrl+F]");
    ImGui::Text("Note : The settings here don't reflect automatically. You need to apply it first.");
    ImGui::Text("Current charset size : %d", charset.size());
    ImGui::Text("Fonts");
    if (fontsAvailable.size() > 0) {
        if (ImGui::BeginCombo("Default (w/ Romal & Japanese charset)", FileNameFromPath(*fontnameDefault).c_str())) {
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

        if (ImGui::BeginCombo("Fallback (w/ Custom Charset)", FileNameFromPath(*fontnameWithCharset).c_str())) {
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
    }
    else {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "WARNING : No fonts available!");
    }
    if (ImGui::Button("Refresh List")) RefreshFontList();
    ImGui::Text("Font Size");
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "WARNING : Going for a large font size might crash the texture creation procedure!");
    ImGui::SliderFloat("Font Size", &fontSize, 10, 100, "%.1f");
    ImGui::SliderFloat("ImGui Font Size", &fontSizeImGui, 10, 80, "%.1f");
    ImGui::SliderInt("Stroke Size", &strokeSize, 0, 10);

    ImGui::Separator();
    ImGui::Text("Shadows");    
    ImGui::SliderFloat("X", &shadowOffset.x, -10, 10);        
    ImGui::SliderFloat("Y", &shadowOffset.y, -10, 10);
    shadowOffset.x = ceilf(shadowOffset.x);
    shadowOffset.y = ceilf(shadowOffset.y);
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
