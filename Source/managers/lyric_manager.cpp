#include <globals.h>
#include <managers/lyric_manager.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui_internal.h>

bool ImGui_CursorButton(const char* ID, ImGuiMouseCursor cursor, bool flipX = false, bool flipY = false) {
	ImVec2 offset, size;
	ImVec2 out_uv_border[2]; // TODO : Render borders as well
	ImVec2 out_uv_fill[2];
	ImTextureID texId = ImGui::GetIO().Fonts->TexID;
	ImGui::GetIO().Fonts->GetMouseCursorTexData(cursor, &offset, &size, out_uv_border, out_uv_fill);
	if (flipX) std::swap(out_uv_border[0].x, out_uv_border[1].x);
	if (flipY) std::swap(out_uv_border[0].y, out_uv_border[1].y);
	return ImGui::ImageButton(ID, texId, size, out_uv_border[0], out_uv_border[1]);
}

void LyricManager::Init(Config& cfg) {
	if (!isInit) {
		FromConfig(cfg);
		ResetDefaultCharset();
		FontManager_Inst.reloadFonts = true;
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
	lyricFormat = cfg.lyricFormat;
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
	cfg.lyricFormat = lyricFormat;
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
	return playbackStatus->currentPlaybackTime;
}

/* Current line of lyrics being displayed. */
std::string LyricManager::GetCurrentLyricLine() {
	if (useExternalLyrics && externalLyrics.size() > 0) {
		auto subtitle = SubtitleItem::getByTimestamp(externalLyrics, (long)(TimeElapsed() * 1000) + exSubOffset);
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
		internalLyricLine.clear();
	}
}

/* Set current lyric line */
void LyricManager::SetLyricLine(bool isLyric, const char* src) {
	if (isLyric && src) {
		internalLyricLine = src;
	}
}

/* Updates the 'GameState' and decide the state of the lyrics. */
void LyricManager::UpdateGameState(const char* state) {
	if (state) {
		if (displayStatus == OnScreen && (strcmp(state, "PVsel") == 0)) {
			OnLyricsEnd();
		}
		else if (displayStatus != OnScreen && strcmp(state, "PV POST PROCESS TASK") == 0) {
			OnLyricsBegin();
		}
	}
}

void LyricManager::ResetDefaultCharset() {
	FontManager_Inst.charset.clear();
	FontManager_Inst.UpdateCharset(to_wstr(LYRIC_PLACEHOLDER_MESSAGE));
	FontManager_Inst.UpdateCharset(to_wstr(lyricFormat));
}

/* Called when Ryhthm Game / PV Session starts.*/
void LyricManager::OnLyricsBegin() {
	LOG(L"Started PV%d", pvsel->PVID);
	internalLyricLine.clear();
	displayStatus = OnScreen;
	lyricDisplayStatus = Ready;
	if (useExternalLyrics) {
		// Attempt to load when there's nothing yet
		wchar_t buffer[64] = { 0 };
		swprintf_s(buffer, L"PV%03d", pvsel->PVID);
		LOG(L"Attempting to load SubRip lyrics lyrics\\%s", buffer);
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
			ResetDefaultCharset();
			if (FontManager_Inst.UpdateCharset(buffer))
				FontManager_Inst.reloadFonts = true;
			return;
		}
		else {
			LOG(L"Failed to load SubRip (*.srt) file.");
		}
	}
	LOG(L"Using internal charset for PVID %d", pvsel->PVID);
	std::wstring buffer;
	if (FontManager_Inst.PVDB_Buffer.count(pvsel->PVID)) {
		if (FontManager_Inst.PVDB_Charset_Buffer.count(pvsel->PVID) <= 0) {
			std::wstring buffer = to_wstr(FontManager_Inst.PVDB_Buffer[pvsel->PVID]);
			ResetDefaultCharset();
			if (FontManager_Inst.UpdateCharset(buffer))
				FontManager_Inst.reloadFonts = true;
			FontManager_Inst.PVDB_Charset_Buffer[pvsel->PVID] = FontManager_Inst.charset;
		}
		FontManager_Inst.charset = FontManager_Inst.PVDB_Charset_Buffer[pvsel->PVID];
		FontManager_Inst.reloadFonts = true;

	}
}

/* Called when Ryhthm Game / PV Session ends.*/
void LyricManager::OnLyricsEnd() {
	internalLyricLine.clear();
	externalLyrics.clear();
	displayStatus = NoLyric;
	lyricDisplayStatus = Ended;
	lyricIndex = 0;
	LOG(L"Lyrics Ended.");
}

/* Called by DX Hook's OnFrame. Do not override.*/
void LyricManager::OnFrame() {
	if (lyricDisplayStatus != Ended) {
		if (playbackStatus2->isPlaying) {
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
		ImGui::Text("Performance Metrics : %.1f FPS (%.3f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);

		ImGui::Separator();
		if (shouldShowLyrics()) {
			ImGui::Text("Now Playing PVID: PV%03d (on %s)", pvsel->PVID, pvsel->getGameDifficultyString());
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
				"LyricId:%d Evt:%d Type:%s Time:%.2f s",
				lyricIndex,
				playbackStatus->evtNumber,
				pvsel->getGameModeString(),
				TimeElapsed()
			);
			if (externalLyrics.size() > 0) {
				ImGui::Text(
					"Using External Lyrics (%d lines)",
					externalLyrics.size()
				);
				ImGui::Separator();
				ImGui::SliderInt("Time Offset (ms)", (int*)&exSubOffset, -5000, 5000);
			}
		}
		else {
			ImGui::Text("No Lyric");
		}
		ImGui::Separator();
		ImGui::Text("Window Styling");
		ImGui::SliderFloat("Opacity", &lyricWindowOpacity, 0.0f, 1.0f, "%.1f");
		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::Text("Borders");
		ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 10.0f, "%.0f");
		ImGui::Text("Rounding");
		ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
		ImGui::Separator();
		ImGui::Checkbox("Attempt to load (and use) external SubRip Lyrics (applies on song start)", &useExternalLyrics);
		ImGui::Checkbox("Show game lyrics", &showInternalLyrics);
		ImGui::SameLine();
		ImGui::Checkbox("Show lyrics", &showLyrics);
		ImGui::Separator();
		ImGui::Text("Lyric Format");
		ImGui::PushFont(FontManager_Inst.font);
		ImGui::InputText("##", &lyricFormat);
		ImGui::PopFont();
		if (ImGui::Button("Force Build Font For Format"))
			if (FontManager_Inst.UpdateCharset(to_wstr(lyricFormat)))
				FontManager_Inst.reloadFonts = true;
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
			if (ImGui_CursorButton("Top Left", ImGuiMouseCursor_ResizeNESW, true, false))
				lyricPivotStyle = TopLeft;
			ImGui::SameLine();
			if (ImGui_CursorButton("Top Right", ImGuiMouseCursor_ResizeNESW, false, false))
				lyricPivotStyle = TopRight;
			if (ImGui_CursorButton("Bottom Left", ImGuiMouseCursor_ResizeNESW, true, true))
				lyricPivotStyle = BottomLeft;
			ImGui::SameLine();
			if (ImGui_CursorButton("Bottom Right", ImGuiMouseCursor_ResizeNESW, false, true))
				lyricPivotStyle = BottomRight;
			if (prev != lyricPivotStyle) lyricPivotOffset = ImVec2();
			ImGui::Separator();
		}
		SHOW_WINDOW_SAVE_SELECTOR;
		ImGui::End();
	}
	if (showLyrics) {
		auto lyrics = GetCurrentLyricLine();
		bool isLyricsUnavailable = (lyricIndex <= 0 && !useExternalLyrics) || lyrics.length() <= 0;
		const char* display;
		if (!shouldShowLyrics() && showGUI) {
			display = LYRIC_PLACEHOLDER_MESSAGE;
		}
		else {
			display = lyrics.c_str();
		}
		char text[1024] = { 0 };
		if (sprintf_s(text, lyricFormat.c_str(), display) < 0)
			sprintf_s(text, "%s", display);
		// With AutoResize, the window rectangle doesn't get updated on the first frame somehow
		// Manually calculate text size before Begin() to correct it.
		if ((shouldShowLyrics() && !isLyricsUnavailable) || (!shouldShowLyrics() && showGUI)) {
			ImGui::SetNextWindowBgAlpha(lyricWindowOpacity);
			ImGui::PushFont(FontManager_Inst.font);
			ImGui::SetNextWindowSize(ImGui::CalcTextSize(text) + ImGui::GetStyle().FramePadding * 2 + ImGui::GetStyle().WindowPadding);
			ImGui::Begin(
				"Lyric Overlay", NULL,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoResize
			);
			ImGui::Text(text);
			ImGui::PopFont();
		}
		else return; // Hide the window when we want to display lyrics but there's nothing to show          
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
		}
		else {
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
				}
				else {
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
