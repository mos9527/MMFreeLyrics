#pragma once
#include <globals.h>
#include <cstdarg>

inline void wprintf_(const wchar_t* fmt, ...) {
	// Release builds somehow omitted printf functions
	// This is to replace it with a Windows API,which doesn't get removed.
	HANDLE hdl = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hdl) {
		wchar_t buf[512] = { 0 };
		va_list va;
		va_start(va, fmt);
		vswprintf(buf, 512, fmt, va);
		WriteConsoleW(hdl, buf, (int)wcslen(buf), NULL, NULL);
		va_end(va);
	}
}

#define LOG(fmt,...) \
	{ \
		wprintf_(L"[FreeLyrics] "); \
		wprintf_(fmt,__VA_ARGS__); \
		wprintf_(L"\n"); \
	}
