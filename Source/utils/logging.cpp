#include <globals.h>
#include <cstdarg>

void wprintf_(const wchar_t* fmt, ...) {
	// Release builds somehow omitted printf functions
	// This is to replace it with a Windows API,which doesn't get removed.
	wchar_t buf[512] = { 0 };
	va_list va;
	va_start(va, fmt);
	vswprintf(buf, _CRT_INT_MAX, fmt, va);
	WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), buf, (int)wcslen(buf), NULL, NULL);
	va_end(va);
}