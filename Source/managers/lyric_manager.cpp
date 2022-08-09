#include <globals.h>
#include <managers/lyric_manager.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui_internal.h>

bool ImGui_CursorButton(const char * ID,ImGuiMouseCursor cursor,bool flipX = false,bool flipY = false) {
    ImVec2 offset, size;
    ImVec2 out_uv_border[2]; // TODO : Render borders as well
    ImVec2 out_uv_fill[2];    
    ImTextureID texId = ImGui::GetIO().Fonts->TexID;
    ImGui::GetIO().Fonts->GetMouseCursorTexData(cursor, &offset, &size, out_uv_border, out_uv_fill);    
    if (flipX) std::swap(out_uv_border[0].x, out_uv_border[1].x);
    if (flipY) std::swap(out_uv_border[0].y, out_uv_border[1].y);
    return ImGui::ImageButton(ID,texId, size, out_uv_border[0], out_uv_border[1]);
}

void LyricManager::Init(Config& cfg) {
    if (!isInit) {
        FromConfig(cfg);
        isInit = true;
    }
}

Config& LyricManager::FromConfig(Config& cfg) {
    lyricWindowOpacity = cfg.lyricWindowOpacity;
    showGUI = cfg.showLyricsManagerGUI;
    useExternalLyrics = cfg.useExternalLyrics;
    showInternalLyrics = cfg.showInternalLyrics;
    showLyrics = cfg.showLyrics;
    lyricDockingStyle = (LyricDockingStyle)cfg.lyricDockingStyle;
    lyricPivotStyle = (LyricPivotStyle)cfg.lyricPivotStyle;
    lyricPivotOffset = cfg.lyricPivotOffset;
    ImGui::GetStyle().WindowBorderSize = cfg.WindowBorderSize;
    ImGui::GetStyle().WindowRounding = cfg.WindowRounding;
    return cfg;
}

Config& LyricManager::ToConfig(Config& cfg) {
    cfg.lyricWindowOpacity = lyricWindowOpacity;
    cfg.showLyricsManagerGUI = showGUI;
    cfg.useExternalLyrics = useExternalLyrics;
    cfg.showInternalLyrics = showInternalLyrics;
    cfg.showLyrics = showLyrics;
    cfg.lyricDockingStyle = lyricDockingStyle;
    cfg.lyricPivotStyle = lyricPivotStyle;
    cfg.lyricPivotOffset = lyricPivotOffset;
    cfg.WindowBorderSize = ImGui::GetStyle().WindowBorderSize;
    cfg.WindowRounding = ImGui::GetStyle().WindowRounding;
    return cfg;
}

/* Should the lyric be on screen? */
bool LyricManager::shouldShowLyrics() {
    return lyricDisplayStatus != Ended;
}

/* The timestamp of the Ryhthm Game / PV Session in seconds. */
float LyricManager::TimeElapsed() {
    return *PVTimestamp;
}

/* Current line of lyrics being displayed. */
std::string LyricManager::GetCurrentLyricLine() {
    if (useExternalLyrics && externalLyrics.size() > 0) {
        auto subtitle = SubtitleItem::getByTimestamp(externalLyrics, (long)(*PVTimestamp * 1000));
        if (subtitle)
            return subtitle->getText();
        else
            return "";
    }
    return internalLyricLine;
}

/* Copies audio file name into our own buffer */
void LyricManager::SetSongAudio(const char* src) {
    if (src) songAudioName.assign(src);
}

/* Set current index of lyric and flag for line updates. */
void LyricManager::UpdateLyricIndex(int index) {
    lyricIndex = index;
    if (index == 0)
    {
        lock.lock();
        internalLyricLine.clear();
        lock.unlock();
    }
}

/* Set current lyric line with a lock. */
void LyricManager::SetLyricLine(bool isLyric, const char* src) {
    if (isLyric && src) {
        lock.lock();        
        internalLyricLine = src;
        lock.unlock();
    }
    else {
        lyricDisplayType = RyhthmGame;
    }
}

/* Updates the 'GameState' and decide the state of the lyrics. */
void LyricManager::UpdateGameState(const char* state) {
    if (state) {
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
}

/* Called when Ryhthm Game / PV Session starts.*/
void LyricManager::OnLyricsBegin() {
    LOG(L"Started PV%d", *PVID);    
    if (useExternalLyrics) {
        // Attempt to load when there's nothing yet
        wchar_t buffer[64] = { 0 };
        swprintf_s(buffer, L"PV%03d-%S", *PVID, LanguageTypeStrings[(LanguageType)*Language]);
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
        if (shouldShowLyrics()) {
            ImGui::Text("Now Playing PVID: PV%03d-%s", *PVID, LanguageTypeStrings[(LanguageType)*Language]);
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
                songAudioName.c_str()
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
        ImGui::Checkbox("Show game lyrics",&showInternalLyrics);
        ImGui::SameLine();
        ImGui::Checkbox("Show lyrics",&showLyrics);
        ImGui::Separator();

        ImGui::Text("Docking (Automatic Alignment)");
        if (ImGui::Button("Free")) {
            lyricDockingStyle = Free;
            lyricPivotStyle = NotSet;
        }
        ImGui::SameLine();
        if (ImGui::Button("H.Center")) 
            lyricDockingStyle |= HorizontallyCentered;
        ImGui::SameLine();
        if (ImGui::Button("V.Center")) 
            lyricDockingStyle |= VerticallyCentered; 
        ImGui::Separator();
        if (lyricDockingStyle == Free) {
            ImGui::Text("Pivioting");
            int prev = lyricPivotStyle;
            ImGui::Separator();
            if (ImGui_CursorButton("Top Left",ImGuiMouseCursor_ResizeNESW, true, false))
                lyricPivotStyle = TopLeft;
            ImGui::SameLine();
            if (ImGui_CursorButton("Top Right",ImGuiMouseCursor_ResizeNESW, false, false))
                lyricPivotStyle = TopRight;
            if (ImGui_CursorButton("Bottom Left",ImGuiMouseCursor_ResizeNESW, true, true))
                lyricPivotStyle = BottomLeft;
            ImGui::SameLine();
            if (ImGui_CursorButton("Bottom Right",ImGuiMouseCursor_ResizeNESW, false, true))
                lyricPivotStyle = BottomRight;
            if (prev != lyricPivotStyle) lyricPivotOffset = ImVec2();
            ImGui::Separator();
        }
        SHOW_WINDOW_SAVE_SELECTOR;
        ImGui::End();
    }
    if (showLyrics) {
        bool isLyricsAvailable = shouldShowLyrics() && lyricIndex <= 0 && !useExternalLyrics;
        if (!isLyricsAvailable) {
            ImGui::SetNextWindowBgAlpha(lyricWindowOpacity);
            ImGui::Begin(
                "Lyric Overlay", NULL,
                ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing
            );
        }
        if (isLyricsAvailable)
            return ImGui::End(); // Hide the window when we want to display lyrics but there's nothing to show
        ImGui::PushFont(FontManager_Inst.font);
        if (!shouldShowLyrics() && showGUI) {
            ImGui::Text(LYRIC_PLACEHOLDER_MESSAGE);
        }
        else {
            lock.lock();            
            ImGui::Text(GetCurrentLyricLine().c_str());
            lock.unlock();
        }
        ImGui::PopFont();
        auto viewport = ImGui::GetMainViewport();
        auto window = ImGui::GetCurrentWindow();
        // Docking Mode
        // The movement axies are restrained to some degree by the docking settings
        if (lyricDockingStyle != Free) {
            window->SetWindowPosPivot = ImVec2(
                0.5f * (lyricDockingStyle & HorizontallyCentered),
                0.5f * (lyricDockingStyle & VerticallyCentered)
            );

            if (lyricDockingStyle & HorizontallyCentered)
                window->SetWindowPosVal.x = viewport->Size.x / 2;
            else
                window->SetWindowPosVal.x = window->Pos.x;
            if (lyricDockingStyle & VerticallyCentered)
                window->SetWindowPosVal.y = viewport->Size.y / 2;
            else
                window->SetWindowPosVal.y = window->Pos.y;
        } else {
            // Pivoting Mode
            // The lyrics window can be moved freely while being relative to a pivot (of itself)
            ImVec2 pivot;
            switch (lyricPivotStyle) {
                case TopLeft:
                    pivot.x = 0; break;
                case TopRight:
                    pivot.x = 1; break;
                case BottomLeft:
                    pivot.y = 1; break;
                case BottomRight:
                    pivot.x = 1; 
                    pivot.y = 1; break;
            }
            if (lyricPivotStyle != NotSet) {
                ImGuiContext& g = *GImGui;
                if (g.MovingWindow == window) {
                    lyricWasMoved = true;
                } else {
                    if (lyricWasMoved) {
                        lyricPivotOffset += window->Pos - lyricPosBeforeMove;
                        lyricWasMoved = false;
                    }
                    else lyricPosBeforeMove = window->Pos;
                    window->SetWindowPosPivot = pivot;
                    window->SetWindowPosVal = viewport->Size * pivot + lyricPivotOffset;
                }
            }
        }
        ImGui::End();
    }
}
LyricManager LyricManager_Inst;
