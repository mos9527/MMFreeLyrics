#pragma once

extern void wprintf_(const wchar_t* fmt, ...);

#define LOG(fmt,...) \
	{ \
		wprintf_(L"[FreeLyrics] "); \
		wprintf_(fmt,__VA_ARGS__); \
	}