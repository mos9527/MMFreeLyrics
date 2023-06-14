#pragma once
#define _ITERATOR_DEBUG_LEVEL 0
// Disable checked iterators for std::string pointers (and etc) to work with the game
#define WIN32_LEAN_AND_MEAN
// Standard Libaries
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <string>
#include <xstring>
#include <vector>
#include <mutex>
#include <map>
#include <algorithm>
// Dependencies
#include <Windows.h>
#include <detours.h>
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui/misc/freetype/imgui_freetype.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <toml.hpp>
// DirectX
#include <d3d11.h>
// Data
inline ID3D11Device* g_pd3dDevice;
inline ID3D11DeviceContext* g_pd3dDeviceContext;
inline IDXGISwapChain* g_pSwapChain;
inline ID3D11RenderTargetView* g_mainRenderTargetView;
inline WNDPROC originalWndProc;
inline HWND window;
// Utils
#include <constant.h>
#include <utils/stringutils.h>
#include <utils/logging.h>
#include <utils/helpers.h>
#include <utils/sigscan.h>
#include <utils/config.h>
// Misc
#define SHOW_WINDOW_SAVE_SELECTOR ImGui::Text("Configuration"); \
ImGui::Text("Config for both windows (and their posistions) can be saved/loaded anytime via Ctrl+S/Ctrl+L."); \
if (ImGui::Button("Save")) \
	Config::SaveGlobalConfig(); \
ImGui::SameLine(); \
if (ImGui::Button("Load")) \
	Config::LoadGlobalConfig();
#define SET_IMGUI_DEFAULT_FLAGS \
	ImGui::GetIO().ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard; \
	ImGui::GetIO().IniFilename = NULL;

typedef uint8_t _BYTE;
typedef uint64_t _QWORD;
typedef uint32_t _DWORD;

struct DivaPlaybackStatus
{
	wchar_t wchar0;
	__declspec(align(8)) _BYTE byte8;
	_BYTE gap9[180003];
	int evtNumber;
	_QWORD qword2BF30;
	_QWORD qword2BF38;
	_QWORD currentPlaybackTimeQword;
	float currentPlaybackTime;
	_DWORD dword2BF4C;
	_BYTE gap2BF50[40];
	_QWORD qword2BF78;
	_BYTE gap2BF80[40];
	_QWORD qword2BFA8;
	_BYTE gap2BFB0[20];
	int int2BFC4;
	_BYTE gap2BFC8[14];
	_BYTE byte2BFD6;
	_BYTE gap2BFD7[1290];
	unsigned __int8 unsigned___int82C4E1;
	_DWORD dword2C4E4;
};
inline DivaPlaybackStatus* playbackStatus;

struct DivaPlaybackStatus2
{
	_BYTE gap1[36];
	int isPlaying;
};
inline DivaPlaybackStatus2* playbackStatus2;

struct PvSelector
{
	_DWORD diff;
	_DWORD diffEx;
	_QWORD pad1;
	_QWORD pad2;
	_QWORD gameModeIndex;
	_DWORD PVID;
	enum GameDifficulty {
		Easy,
		Normal,
		Hard,
		Extreme,
		ExExtreme
	};
	const char* getGameDifficultyString() {
		if (diffEx) return "ExExtreme";
		switch (diff)
		{
		case 0:return "Easy";
		case 1:return "Normal";
		case 2:return "Hard";
		case 3:return "Extreme";
		default:
			return "Easy";
		}
	}
	const char* getGameModeString() {
		switch (gameModeIndex) {
		case 0x0:
			return "RhythmGame";
		case 0x1:
			return "Practice";
		case 0x0101:
			return "PV Viewer";
		case 0x010101:
			return "PV Viewer / AltPv";
		default:
			return "Rhythm Game";
		}
	}
};
inline PvSelector* pvsel;
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
inline PVWaitScreenInfoStruct* PVWaitScreenInfo;