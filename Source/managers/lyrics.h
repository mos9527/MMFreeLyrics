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

private:
    std::wstring line;
    std::mutex lock;
    std::string songAudioName;

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