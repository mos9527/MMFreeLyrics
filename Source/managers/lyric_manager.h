#pragma once
#include <globals.h>

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
    std::string styleTestString;

    int pvID = 0;
    int lyricIndex = 0;
    bool lyricUpdated = false;

    DisplayStatus displayStatus = NoLyric;
    LyricDisplayType lyricDisplayType = None;
    LyricDisplayStatus lyricDisplayStatus = Ended;

    float lyricWindowOpacity = 0.0f;
    LyricDisplayType lyricSouldMoveType = None;

public:
    bool showGUI = true;
    bool ShowLyric();

    void Init(Config& cfg);

    Config& FromConfig(Config& cfg);
    Config& ToConfig(Config& cfg);

    float TimeElapsed();
    std::wstring& GetCurrentLyricLine();
    std::string& GetSongAudio();

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
