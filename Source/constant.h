#pragma once
#include <globals.h>
// String literals
#define DEFAULT_CHARSET_NAME L"charset.txt"
#define MESSAGEBOX_TITLE L"MegaMix+ Freetype Lyrics"
#define CHARSET_NOTFOUND_WARNING_CONTENT L"Charset file " DEFAULT_CHARSET_NAME " not found. \n\n\
If you're using non-English lyrics translation mod, This will probably cause some characters appears to be missing \n\n\
In which case, you should copy their pv_db.txt file here alongside the DLLs, and rename it into " DEFAULT_CHARSET_NAME" .\n\n\
Otherwise, it's safe to ignore this error."
#define LYRIC_PLACEHOLDER_MESSAGE u8"初音ミク♡Project DIVA FreeType Lyrics"
static const char* LanguageTypeStrings[] = { u8"Japanese",u8"English",u8"SChinese",u8"TChinese",u8"Korean",u8"French",u8"Italian",u8"German",u8"Spanish" };
static const char* DisplayStatusStrings[] = { "On Screen","Not available" };
static const char* LyricDisplayTypeStrings[] = { "Ryhthm Game","PV Viewer","Not available" };
static const char* LyricDisplayStatusStrings[] = { "Ended","Playing","Paused","Ready" };

// Structs & Enums
struct DivaInputState
{
    // https://github.com/nastys/MEGAHAKUS/blob/master/source/structs.h
    struct ButtonState
    {
        uint32_t A;
        uint32_t B;
        uint32_t X;
        uint32_t Y;
    };
    ButtonState Tapped;
    ButtonState Released;
    ButtonState Down;
    uint32_t Padding_20[4];
    ButtonState DoubleTapped;
    uint32_t Padding_30[4];
    ButtonState IntervalTapped;
    uint32_t Padding_38[12];
    uint32_t Padding_MM[8];
    int32_t MouseX;
    int32_t MouseY;
    int32_t MouseDeltaX;
    int32_t MouseDeltaY;
    uint32_t Padding_AC[4];
    float LeftJoystickX;
    float LeftJoystickY;
    float RightJoystickX;
    float RightJoystickY;
    char KeyPadding[3];
    char Key;
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
inline const uint8_t* Language = (uint8_t*)0x14CC105F4;
inline const uint32_t* PVEvent = (uint32_t*)0x1412EE324;
inline const float* PVTimestamp = (float*)0x1412EE340;
inline const uint32_t* PVPlaying = (uint32_t*)0x1412F0258;
inline const uint32_t* PVID = (uint32_t*)0x1416E2BB0;
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
inline PVWaitScreenInfoStruct* PVWaitScreenInfo = (PVWaitScreenInfoStruct*)0x14CC0B5F8;
