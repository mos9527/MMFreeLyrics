#include <globals.h>

void LyricManager::Init(Config& cfg) {
    FromConfig(cfg);
}

Config& LyricManager::FromConfig(Config& cfg) {
    lyricWindowOpacity = cfg.lyricWindowOpacity;
    showGUI = cfg.showLyricsManagerGUI;
    ImGui::GetStyle().WindowBorderSize = cfg.WindowBorderSize;
    ImGui::GetStyle().WindowRounding = cfg.WindowRounding;
    return cfg;
}

Config& LyricManager::ToConfig(Config& cfg) {
    cfg.lyricWindowOpacity = lyricWindowOpacity;
    cfg.showLyricsManagerGUI = showGUI;
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
    return line;
}

/* Copies audio file name into our own buffer, and extract PVID from it. */
void LyricManager::SetSongAudio(const char* src) {
    songAudioName.assign(src);
    // TODO: find procedures where PV id is assigned instead of this hack
    std::string filename = FileNameFromPath(src);
    sscanf_s(filename.c_str() + 1, "pv_%d.ogg", &pvID);
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
        line.clear();
        lock.unlock();
    }
}

/* Set current lyric line with a lock. Only works when UpdateLyricIndex flagged for updates. */
void LyricManager::SetLyricLine(bool isLyric, char* src) {
    if (lyricUpdated && isLyric) {
        lock.lock();
        line = u8string_to_wide_string(src);
        lock.unlock();
    }
    else if (!isLyric) {
        lyricDisplayType = RyhthmGame;
    }
}

/* Updates the 'GameState' and decide the state of the lyrics. */
void LyricManager::UpdateGameState(const char* state) {
    if (displayStatus == OnScreen && (strcmp(state, "PVsel") == 0)) {
        line.clear();
        displayStatus = NoLyric;
        lyricDisplayStatus = Ended;
        lyricDisplayType = None;
        lyricIndex = 0;
        OnLyricsEnd();
    }
    else if (displayStatus != OnScreen && strcmp(state, "PV POST PROCESS TASK") == 0) {
        displayStatus = OnScreen;
        lyricDisplayType = PVMode;
        lyricDisplayStatus = Ready;
        OnLyricsBegin();
    }
}

/* Called when Ryhthm Game / PV Session starts. May be overridden.*/
void LyricManager::OnLyricsBegin() {
    LOG(L"Started with audio %S.", songAudioName.c_str());
}

/* Called when Ryhthm Game / PV Session ends. May be overridden.*/
void LyricManager::OnLyricsEnd() {
    LOG(L"Lyrics Ended.");
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
            ImGui::Text("Now Playing [PV %d] [%s]", pvID, LanguageTypeStrings[(LanguageType)*Language]);
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
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing
    );
    if (ShowLyric() && lyricIndex <= 0)
        return ImGui::End(); // Hide the window when we want to display lyrics but there's nothing to show
    ImGui::PushFont(FontManager_Inst.font);
    if (!ShowLyric()) {
        if (showGUI) ImGui::Text(LYRIC_PLACEHOLDER_MESSAGE);
    }
    else {
        lock.lock();
        ImGui::Text(to_utf8(GetCurrentLyricLine()).c_str());
        lock.unlock();
    }
    ImGui::PopFont();
    ImGui::End();
}
LyricManager LyricManager_Inst;
