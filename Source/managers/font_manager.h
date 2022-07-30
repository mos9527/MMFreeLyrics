#pragma once
#include <globals.h>

#define VEC4_TO_COL32(name) IM_COL32(name.x*255,name.y*255,name.z*255,name.w*255)

class FontManager {

private:
    std::vector<std::string> fontsAvailable;
    std::string* fontnameDefault;
    std::string* fontnameWithCharset;
    std::string charset;
    bool reloadFonts = false;

    float fontSize;
    float fontSizeImGui;
    bool isBold;
    bool isItalic;
    ImVec4 fillColor;
    ImVec4 strokeColor;
    ImVec4 shadowColor;
    ImVec2 shadowOffset;

    int strokeSize;

public:
    // Current combined font used for lyrics renderer
    ImFont* font;
    bool showGUI = true;

    void Init(Config& cfg);

    Config& FromConfig(Config& cfg);
    Config& ToConfig(Config& cfg);

    std::vector<std::string>& RefreshFontList();

    void BuildCharset();
    void RebuildFonts();
    void OnFrame();
    void OnImGUI();

};

extern FontManager FontManager_Inst;
