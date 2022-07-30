#pragma once
#define _ITERATOR_DEBUG_LEVEL 0
// Disable checked iterators for std::string pointers to work with the game
#define WIN32_LEAN_AND_MEAN
#ifndef PCH_H
#define PCH_H
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
#include <io.h>
#include <fcntl.h>
// Dependencies
#include <Windows.h>
#include <detours.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui/misc/freetype/imgui_freetype.h>
#include <toml.hpp>
// DirectX
#include <d3d11.h>
// String literals
#define DEFAULT_CHARSET_NAME L"charset.txt"
#define MESSAGEBOX_TITLE L"MegaMix+ Freetype Lyrics"
#define CHARSET_NOTFOUND_WARNING_CONTENT L"Charset file " DEFAULT_CHARSET_NAME " not found. \n\n\
If you're using non-English lyrics translation mod, This will probably cause some characters appears to be missing \n\n\
In which case, you should copy their pv_db.txt file here alongside the DLLs, and rename it into " DEFAULT_CHARSET_NAME" .\n\n\
Otherwise, it's safe to ignore this error."
#define LYRIC_PLACEHOLDER_MESSAGE u8"初音ミク Project DIVA FreeType Lyrics"
static const char* LanguageTypeStrings[] = { u8"Japanese",u8"English",u8"SChinese",u8"TChinese",u8"Korean",u8"French",u8"Italian",u8"German",u8"Spanish" };
static const char* DisplayStatusStrings[] = { "On Screen","Not available" };
static const char* LyricDisplayTypeStrings[] = { "Ryhthm Game","PV Viewer","Not available" };
static const char* LyricDisplayStatusStrings[] = { "Ended","Playing","Paused","Ready" };
// Data
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
static WNDPROC originalWndProc;
static HWND window;
// Utils
#include <utils/stringutils.h>
#include <utils/logging.h>
#include <utils/helpers.h>
#include <utils/sigscan.h>
#include <utils/config.h>
// Managers,Config & UI
#define SHOW_WINDOW_SAVE_SELECTOR ImGui::Text("Configuration"); \
ImGui::Text("Config for both windows (and their posistions) can be saved/loaded anytime via Ctrl+S/Ctrl+L."); \
if (ImGui::Button("Save")) Config::SaveGlobalConfig(); \
ImGui::SameLine(); \
if (ImGui::Button("Load")) Config::LoadGlobalConfig();

#define SET_IMGUI_DEFAULT_FLAGS \
ImGui::GetIO().ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard; \
ImGui::GetIO().IniFilename = NULL;

#include <managers/fonts.h>
#include <managers/lyrics.h>
#endif