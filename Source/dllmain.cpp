// dllmain.cpp : Defines the entry point for the DLL application.
#include "Windows.h"
#include "pch.h"
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    wchar_t _buffer[512] = { 0 };
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:        
        GetModuleFileNameW(hModule, _buffer, 512);
        dllFilePath.assign(_buffer);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

