#include <globals.h>
void Config::LoadDefault() {
	fontnameDefault = "";
	fontnameWithCharset = "";
	fontSize = 35.0f;
	fontSizeImGui = 13.0f;
	isBold = false;
	isItalic = false;
	strokeSize = 2;
	shadowOffset = ImVec2(2, 2);
	fillColor = ImVec4(1, 1, 1, 1);
	strokeColor = ImVec4(0, 0, 0, 1);
	shadowColor = ImVec4(0, 0, 0, 0.5);

	lyricWindowOpacity = 0.0f;
	lyricFormat = "%s";

	WindowBorderSize = 0.0f;
	WindowRounding = 12.0f;
	FontGlobalScale = 1.0f;
	ImGuiSettings = "";

	showFontManagerGUI = true;
	showLyricsManagerGUI = true;
	useExternalLyrics = true;
	showLyrics = true;
	showInternalLyrics = false;
}

#pragma warning( disable : 4244 )
bool Config::Save() {
	config = toml::parse(FullPathInDllFolder(L"config.toml"));
	if (!config.contains("freelyrics")) {
		config["freelyrics"] = toml::value();
	}
	toml::value& table = toml::find<toml::value>(config, "freelyrics");

	ASSIGN_TO_TABLE(fontnameDefault);
	ASSIGN_TO_TABLE(fontnameWithCharset);
	ASSIGN_TO_TABLE(fontSize);
	ASSIGN_TO_TABLE(fontSizeImGui);
	ASSIGN_TO_TABLE(isBold);
	ASSIGN_TO_TABLE(isItalic);
	ASSIGN_TO_TABLE(strokeSize);
	ASSIGN_TO_TABLE(lyricWindowOpacity);
	ASSIGN_TO_TABLE(WindowBorderSize);
	ASSIGN_TO_TABLE(WindowRounding);
	ASSIGN_TO_TABLE(FontGlobalScale);
	ASSIGN_TO_TABLE(ImGuiSettings);
	ASSIGN_TO_TABLE(showFontManagerGUI);
	ASSIGN_TO_TABLE(showLyricsManagerGUI);
	ASSIGN_TO_TABLE(useExternalLyrics);
	ASSIGN_TO_TABLE(showInternalLyrics);
	ASSIGN_TO_TABLE(showLyrics);
	ASSIGN_TO_TABLE(lyricDockingStyle);
	ASSIGN_TO_TABLE(lyricPivotStyle);
	ASSIGN_TO_TABLE(lyricFormat);

	VEC2_TO_XY(lyricPivotOffset);
	ASSIGN_TO_TABLE(lyricPivotOffsetX);
	ASSIGN_TO_TABLE(lyricPivotOffsetY);

	VEC2_TO_XY(shadowOffset);
	ASSIGN_TO_TABLE(shadowOffsetX);
	ASSIGN_TO_TABLE(shadowOffsetY);

	VEC4_TO_RGBA(fillColor);
	ASSIGN_TO_TABLE(fillColorR);
	ASSIGN_TO_TABLE(fillColorG);
	ASSIGN_TO_TABLE(fillColorB);
	ASSIGN_TO_TABLE(fillColorA);
	VEC4_TO_RGBA(strokeColor);
	ASSIGN_TO_TABLE(strokeColorR);
	ASSIGN_TO_TABLE(strokeColorG);
	ASSIGN_TO_TABLE(strokeColorB);
	ASSIGN_TO_TABLE(strokeColorA);
	VEC4_TO_RGBA(shadowColor);
	ASSIGN_TO_TABLE(shadowColorR);
	ASSIGN_TO_TABLE(shadowColorG);
	ASSIGN_TO_TABLE(shadowColorB);
	ASSIGN_TO_TABLE(shadowColorA);

	// Writing back to the file
	std::ofstream f(FullPathInDllFolder(L"config.toml"), std::ios::binary);
	if (!f.is_open()) {
		MessageBoxW(
			window,
			L"Cannot open configuration file (config.toml) for writing!",
			MESSAGEBOX_TITLE,
			MB_ICONEXCLAMATION
		);
		return false;
	}
	f << config;
	f.close();
	return true;
}

bool Config::Load() {
	config = toml::parse(FullPathInDllFolder(L"config.toml"));
	if (config.contains("freelyrics")) {
		toml::table freelyrics = toml::find<toml::table>(config, "freelyrics");
		try {
			fontnameDefault = freelyrics["fontnameDefault"].as_string();
			fontnameWithCharset = freelyrics["fontnameWithCharset"].as_string();
			fontSize = freelyrics["fontSize"].as_floating();
			fontSizeImGui = freelyrics["fontSizeImGui"].as_floating();
			isBold = freelyrics["isBold"].as_boolean();
			isItalic = freelyrics["isItalic"].as_boolean();
			strokeSize = freelyrics["strokeSize"].as_integer();
			lyricWindowOpacity = freelyrics["lyricWindowOpacity"].as_floating();
			WindowBorderSize = freelyrics["WindowBorderSize"].as_floating();
			WindowRounding = freelyrics["WindowRounding"].as_floating();
			FontGlobalScale = freelyrics["FontGlobalScale"].as_floating();
			ImGuiSettings = freelyrics["ImGuiSettings"].as_string();
			showFontManagerGUI = freelyrics["showFontManagerGUI"].as_boolean();
			showLyricsManagerGUI = freelyrics["showLyricsManagerGUI"].as_boolean();
			useExternalLyrics = freelyrics["useExternalLyrics"].as_boolean();
			showInternalLyrics = freelyrics["showInternalLyrics"].as_boolean();
			showLyrics = freelyrics["showLyrics"].as_boolean();
			lyricDockingStyle = freelyrics["lyricDockingStyle"].as_integer();
			lyricPivotStyle = freelyrics["lyricPivotStyle"].as_integer();
			lyricFormat = freelyrics["lyricFormat"].as_string();
			lyricPivotOffset = ImVec2(
				freelyrics["lyricPivotOffsetX"].as_floating(),
				freelyrics["lyricPivotOffsetY"].as_floating()
			);

			shadowOffset = ImVec2(
				freelyrics["shadowOffsetX"].as_floating(),
				freelyrics["shadowOffsetY"].as_floating()
			);

			fillColor = ImVec4(
				freelyrics["fillColorR"].as_floating(),
				freelyrics["fillColorG"].as_floating(),
				freelyrics["fillColorB"].as_floating(),
				freelyrics["fillColorA"].as_floating()
			);
			strokeColor = ImVec4(
				freelyrics["strokeColorR"].as_floating(),
				freelyrics["strokeColorG"].as_floating(),
				freelyrics["strokeColorB"].as_floating(),
				freelyrics["strokeColorA"].as_floating()
			);
			shadowColor = ImVec4(
				freelyrics["shadowColorR"].as_floating(),
				freelyrics["shadowColorG"].as_floating(),
				freelyrics["shadowColorB"].as_floating(),
				freelyrics["shadowColorA"].as_floating()
			);
		}
		catch (std::exception e) {
			MessageBoxW(
				window,
				L"Failed to load configuration file (config.toml)!\nTry saving it again to fix this.",
				MESSAGEBOX_TITLE,
				MB_ICONEXCLAMATION
			);
			LoadDefault();
		}
		return true;
	}
	return false;
}
#pragma warning( default : 4244 )
Config GlobalConfig_Inst;
