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
