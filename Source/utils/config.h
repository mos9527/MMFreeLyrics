#pragma once
#include <globals.h>

#define ASSIGN_TO_TABLE(v) table[#v] = toml::value(v);
#define VEC4_TO_RGBA(name) float name ## R = name.x;float name ## G = name.y;float name ## B = name.z;float name ## A = name.w;
#define VEC2_TO_XY(name) float name ## X = name.x;float name ## Y = name.y;

class Config
{
public:
	// FontManager
	std::string fontnameDefault;
	std::string fontnameWithCharset;
	float fontSize;
	float fontSizeImGui;
	int strokeSize;
	bool isBold;
	bool isItalic;
	ImVec4 fillColor;
	ImVec4 strokeColor;
	ImVec4 shadowColor;
	ImVec2 shadowOffset;
	bool showFontManagerGUI;
	// LyricManager
	float lyricWindowOpacity;
	bool showLyricsManagerGUI;
	bool useExternalLyrics;
	bool showInternalLyrics;
	bool showLyrics;
	int lyricDockingStyle;
	int lyricPivotStyle;
	ImVec2 lyricPivotOffset;
	std::string lyricFormat;
	// ImGui Styles
	float WindowBorderSize;
	float WindowRounding;
	float FontGlobalScale;
	std::string ImGuiSettings;
	toml::value config;

	void LoadDefault();
	Config() {
		LoadDefault();
	}
	bool Save();
	bool Load();
	static void SaveGlobalConfig();
	static void LoadGlobalConfig();
};
extern Config GlobalConfig_Inst;
