// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.
#pragma once

#define _ITERATOR_DEBUG_LEVEL 0
// Disable checked iterators for std::string pointers to work with the game
#define WIN32_LEAN_AND_MEAN

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here

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

// Detours
#include <Windows.h>
#include <detours.h>

// Dependencies
#include <logging.h>
#include <helpers.h>
#include <sigscan.h>

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui/misc/freetype/imgui_freetype.h>

#include <toml.hpp>

// DirectX
#include <d3d11.h>

// Utils
extern std::wstring dllFilePath;
#include <utils.h>

// String literals
#define DEFAULT_CHARSET_NAME L"charset.txt"
#define MESSAGEBOX_TITLE L"MegaMix+ Freetype Lyrics"
#define CHARSET_NOTFOUND_WARNING_CONTENT L"Charset file " DEFAULT_CHARSET_NAME " not found. \n\n\
If you're using non-English lyrics translation mod, This will probably cause some characters appears to be missing \n\n\
In which case, you should copy their pv_db.txt file here alongside the DLLs, and rename it into " DEFAULT_CHARSET_NAME" .\n\n\
Otherwise, it's safe to ignore this error."
#define LYRIC_PLACEHOLDER_MESSAGE u8"初音ミク Project DIVA メガー・ミクス"

// Data
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
static WNDPROC originalWndProc;
static HWND window;

#endif //PCH_H