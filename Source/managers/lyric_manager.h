#pragma once
#include <globals.h>
#include <managers/font_manager.h>
#include <srtparser/srtparser.h>
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
    std::string internalLyricLine;
    std::mutex lock;
    std::string songAudioName;
    std::string styleTestString;

    int lyricIndex = 0;
    std::vector<SubtitleItem*> externalLyrics;

    DisplayStatus displayStatus = NoLyric;
    LyricDisplayType lyricDisplayType = None;
    LyricDisplayStatus lyricDisplayStatus = Ended;
    LyricDisplayType lyricSouldMoveType = None;

public:
    // Configurable 
    bool useExternalLyrics;
    float lyricWindowOpacity = 0.0f;
    bool showInternalLyrics;
    bool showLyrics;

    bool showGUI = true;
    bool shouldShowLyrics();

    void Init(Config& cfg);

    Config& FromConfig(Config& cfg);
    Config& ToConfig(Config& cfg);

    float TimeElapsed();
    std::string GetCurrentLyricLine();
    std::string GetSongAudio();

    void UpdateLyricIndex(int index);
    void UpdateGameState(const char* state);
    void SetLyricLine(bool isLyric, char* src);
    void SetSongAudio(const char* src);

    void OnLyricsBegin();
    void OnLyricsEnd();

    void OnFrame();
    void OnImGUI();
};

extern LyricManager LyricManager_Inst;
