#pragma once
#include <globals.h>
#define millis GetTickCount64
extern std::wstring DllFilePath;
extern std::wstring u8string_to_wide_string(char* src);
extern std::string wide_string_to_u8string(wchar_t* src);
extern std::string to_utf8(std::wstring wstr);
extern std::wstring to_wstr(std::string utf8);
extern std::wstring FullPathInDllFolder(std::wstring fileName);
extern std::string FileNameFromPath(std::string path);