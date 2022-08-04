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
    enum LyricDockingStyle {
        Free = 0b0000,
        HorizontallyCentered = 0b0001,
        VerticallyCentered = 0b0010,
    };
    enum LyricPivotStyle {        
        NotSet,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
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

    bool lyricWasMoved = false;
    ImVec2 lyricPosBeforeMove;
public:
    bool isInit = false;
    // Configurable 
    int lyricDockingStyle = Free;
    int lyricPivotStyle = NotSet;
    ImVec2 lyricPivotOffset;

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

    void UpdateLyricIndex(int index);
    void UpdateGameState(const char* state);
    void SetLyricLine(bool isLyric, const char* src);
    void SetSongAudio(const char* src);

    void OnLyricsBegin();
    void OnLyricsEnd();

    void OnFrame();
    void OnImGUI();
};

extern LyricManager LyricManager_Inst;
