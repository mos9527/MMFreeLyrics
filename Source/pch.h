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

// DirectX
#include <d3d11.h>

// Utils
extern std::wstring dllFilePath;
#include <utils.h>

#endif //PCH_H