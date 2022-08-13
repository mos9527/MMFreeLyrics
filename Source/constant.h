#pragma once
#include <globals.h>
// String literals
#define DEFAULT_CHARSET_NAME L"charset.txt"
#define MESSAGEBOX_TITLE L"MegaMix+ Freetype Lyrics"

#define CHARSET_NOTFOUND_WARNING L"Default charset file " DEFAULT_CHARSET_NAME " not found. \n\n\
If you're using non-English lyrics translation mod, This will probably cause some characters appears to be missing \n\n\
In which case, you should copy their pv_db.txt file here alongside the DLLs, and rename it into " DEFAULT_CHARSET_NAME" .\n\n\
Otherwise, it's safe to ignore this error."

#define FONTS_FOLDER_NOT_FOUND L"Fonts folder 'fonts\' not found. \n\n\
This will cause the renderer to use the built-in ImGui font (aka ProggyClean), \n\n\
which has NO SUPPORT for CJK glyphs. (in which case they get replaced by ?).\n\n\
You should 'install' the fonts by placing them (with extension .ttf/.otf) into this folder. \n\n\
Check the GitHub repo for more info."

#define LYRIC_PLACEHOLDER_MESSAGE u8"初音ミク Project DIVA FreeType Lyrics"
static const char* LanguageTypeStrings[] = { u8"Japanese",u8"English",u8"SChinese",u8"TChinese",u8"Korean",u8"French",u8"Italian",u8"German",u8"Spanish" };
static const char* DisplayStatusStrings[] = { "On Screen","Not available" };
static const char* LyricDisplayStatusStrings[] = { "Ended","Playing","Paused","Ready" };
static const char* Difficulties[] = { "Easy", "Normal", "Hard", "Extreme", "Extra Extreme" };
static const char* GameModes[] = { "Ryhthm Game" ,  "Practice Mode" ,"PV Viewer"};
// Structs & Enums
static const uint32_t* PVEvent = (uint32_t*)0x1412EE324;
static const float* PVTimestamp = (float*)0x1412EE340;
static const uint32_t* PVPlaying = (uint32_t*)0x1412F0258;
static const uint32_t* Difficulty = (uint32_t*)0x1416E2B90;
static const uint32_t* DifficultyExtra = (uint32_t*)0x1416E2B94;
static const uint32_t* GameMode = (uint32_t*)0x1416E2BA8;
static const uint32_t* PVID = (uint32_t*)0x1416E2BB0;
static const uint8_t* Language = (uint8_t*)0x14CC105F4;
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
static PVWaitScreenInfoStruct* PVWaitScreenInfo = (PVWaitScreenInfoStruct*)0x14CC0B5F8;
inline const char* GameDifficultyString() {
    return Difficulties[*Difficulty + *DifficultyExtra];
}
inline const char* GameLanguageString() {
    return LanguageTypeStrings[*Language];
}
inline const char* GameModeString() {
    return GameModes[((*GameMode) & 0x00000001) + (((*GameMode) & 0x00000100) > 0)];
}