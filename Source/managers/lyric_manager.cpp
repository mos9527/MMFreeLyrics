#include <globals.h>
#include <managers/lyric_manager.h>

void LyricManager::Init(Config& cfg) {
    styleTestString.assign(LYRIC_PLACEHOLDER_MESSAGE);
    FromConfig(cfg);
}

Config& LyricManager::FromConfig(Config& cfg) {
    lyricWindowOpacity = cfg.lyricWindowOpacity;
    showGUI = cfg.showLyricsManagerGUI;
    useExternalLyrics = cfg.useExternalLyrics;
    ImGui::GetStyle().WindowBorderSize = cfg.WindowBorderSize;
    ImGui::GetStyle().WindowRounding = cfg.WindowRounding;
    return cfg;
}

Config& LyricManager::ToConfig(Config& cfg) {
    cfg.lyricWindowOpacity = lyricWindowOpacity;
    cfg.showLyricsManagerGUI = showGUI;
    cfg.useExternalLyrics = useExternalLyrics;
    cfg.WindowBorderSize = ImGui::GetStyle().WindowBorderSize;
    cfg.WindowRounding = ImGui::GetStyle().WindowRounding;
    return cfg;
}

/* Should the lyric be on screen? */
bool LyricManager::ShowLyric() {
    return lyricDisplayStatus != Ended;
}

/* The timestamp of the Ryhthm Game / PV Session in seconds. */
float LyricManager::TimeElapsed() {
    return *PVTimestamp;
}

/* Current line of lyrics being displayed. This only gets updated when the source is updated. */
std::wstring& LyricManager::GetCurrentLyricLine() {
    return internalLyricLine;
}

/* Copies audio file name into our own buffer */
void LyricManager::SetSongAudio(const char* src) {
    songAudioName.assign(src);
}

/* The audio being played by the Ryhthm Game / PV Session. */
std::string& LyricManager::GetSongAudio() {
    return songAudioName;
}

/* Set current index of lyric and flag for line updates. */
void LyricManager::UpdateLyricIndex(int index) {
    lyricIndex = index;
    lyricUpdated = true;
    if (index == 0)
    {
        lock.lock();
        internalLyricLine.clear();
        lock.unlock();
    }
}

/* Set current lyric line with a lock. Only works when UpdateLyricIndex flagged for updates. */
void LyricManager::SetLyricLine(bool isLyric, char* src) {
    if (lyricUpdated && isLyric) {
        lock.lock();
        internalLyricLine = u8string_to_wide_string(src);
        lock.unlock();
    }
    else if (!isLyric) {
        lyricDisplayType = RyhthmGame;
    }
}

/* Updates the 'GameState' and decide the state of the lyrics. */
void LyricManager::UpdateGameState(const char* state) {
    if (displayStatus == OnScreen && (strcmp(state, "PVsel") == 0)) {
        internalLyricLine.clear();
        displayStatus = NoLyric;
        lyricDisplayStatus = Ended;
        lyricDisplayType = None;
        lyricIndex = 0;
        OnLyricsEnd();
    }
    else if (displayStatus != OnScreen && strcmp(state, "PV POST PROCESS TASK") == 0) {
        internalLyricLine.clear();
        displayStatus = OnScreen;
        lyricDisplayType = PVMode;
        lyricDisplayStatus = Ready;
        OnLyricsBegin();
    }
}

/* Called when Ryhthm Game / PV Session starts.*/
void LyricManager::OnLyricsBegin() {
    LOG(L"Started PV%d", *PVID);    
    if (useExternalLyrics) {
        // Attempt to load when there's nothing yet
        wchar_t buffer[32] = { 0 };
        swprintf_s(buffer, L"PV%3d", *PVID);
        LOG(L"Attempting to load SubRip lyrics lyrics\\%s",buffer);
        std::wstring filename = FullPathInDllFolder(L"lyrics\\" + std::wstring(buffer) + L".srt");
        SubtitleParserFactory* subParserFactory = new SubtitleParserFactory(to_utf8(filename));
        SubtitleParser* parser = subParserFactory->getParser();
        LOG(L"Loading %s", filename.c_str());
        if (parser->isLoaded) {
            externalLyrics = parser->getSubtitles();
            LOG(L"SubRip file loaded. Lines: %d", externalLyrics.size());
            std::wstring buffer;
            for (auto line : externalLyrics)
                buffer += to_wstr(line->getText());
            if (FontManager_Inst.UpdateCharset(buffer)) {
                // Rebuild fonts if anything new is added
                FontManager_Inst.reloadFonts = true;
            }
        }
        else {
            LOG(L"Failed to load SubRip (*.srt) file.");
        }
    }
}

/* Called when Ryhthm Game / PV Session ends.*/
void LyricManager::OnLyricsEnd() {
    LOG(L"Lyrics Ended.");
    externalLyrics.clear();
}

/* Called by DX Hook's OnFrame. Do not override.*/
void LyricManager::OnFrame() {
    if (lyricDisplayStatus != Ended) {
        if (*PVPlaying) {
            lyricDisplayStatus = Progressing;
        }
        else if (lyricDisplayStatus != Ready)
            lyricDisplayStatus = Paused;
    }
}

/* Called by DX Hook's OnFrame. Do not override.*/
void LyricManager::OnImGUI() {
    if (showGUI) {
        ImGui::SetNextWindowBgAlpha(0.5);
        ImGui::Begin("Lyric [Ctrl+G]");
        ImGui::Text("Performance Metrics : %.1f FPS (%.3f ms)", ImGui::GetIO().Framerate , 1000.0f / ImGui::GetIO().Framerate);        
        ImGui::Separator();
        if (ShowLyric()) {
            ImGui::Text("Now Playing [PV %d] [%s]", *PVID, LanguageTypeStrings[(LanguageType)*Language]);
            ImGui::PushFont(FontManager_Inst.font);
            ImGui::Text("%s", PVWaitScreenInfo->Name.c_str());
            ImGui::PopFont();
            ImGui::Separator();
            ImGui::Text("Music/%s MV/%s Lyrics/%s",                
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
            ImGui::Text(
                "Loaded external lyrics lines:%d",
                externalLyrics.size()
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
        ImGui::Checkbox("Attempt to load (and use) external SubRip Lyrics (applies on song start)", &useExternalLyrics);
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
        SHOW_WINDOW_SAVE_SELECTOR;
        ImGui::End();
    }
    ImGui::SetNextWindowBgAlpha(lyricWindowOpacity);
    if (lyricSouldMoveType != None) {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 window_pos, window_pos_pivot;
        if (lyricSouldMoveType == PVMode) {
            window_pos = viewport->GetWorkCenter();
            window_pos.y = viewport->WorkSize.y * 0.85f;
            window_pos_pivot = ImVec2(0.5f, 0.5f);
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
            if (lyricDisplayType == PVMode) lyricSouldMoveType = PVMode;
            // Keep in horizontal center in PVMode
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
    ImGui::Begin(
        "Lyric Overlay", NULL,
        (!ShowLyric() && showGUI) ? ImGuiWindowFlags_NoDecoration : (ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing)
    );
    if (ShowLyric() && lyricIndex <= 0 &&!useExternalLyrics)
        return ImGui::End(); // Hide the window when we want to display lyrics but there's nothing to show
    ImGui::PushFont(FontManager_Inst.font);
    if (!ShowLyric() && showGUI) {
        // Allow the user to edit the text for style testing
        ImVec2 size = ImGui::CalcTextSize(styleTestString.c_str());
        size.x *= 2;
        size.y *= 2; // TODO : Correct windows size
        ImGui::SetWindowSize(size, ImGuiCond_Always);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
        ImGui::InputText("###", &styleTestString, 1024);
        ImGui::PopStyleColor();
    }
    else {
        lock.lock();
        if (useExternalLyrics && externalLyrics.size() > 0) {
            auto subtitle = SubtitleItem::getByTimestamp(externalLyrics, (long)(*PVTimestamp * 1000));
            if (subtitle) ImGui::Text(subtitle->getText().c_str());
        }
        else {
            ImGui::Text(to_utf8(GetCurrentLyricLine()).c_str());
        }
        lock.unlock();
    }
    ImGui::PopFont();
    ImGui::End();
}
LyricManager LyricManager_Inst;
