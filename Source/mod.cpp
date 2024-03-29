﻿#include <globals.h>
#include <managers/font_manager.h>
#include <managers/lyric_manager.h>
#pragma warning( disable : 26495 26812 )
// Signatures
SIG_SCAN
(
	sigChangeGameState,
	0x1402C9660,
	"\x4C\x8B\xC2\x41\xB9\x00\x00\x00\x00\x48\x8B\x15\x00\x00\x00\x00\x48\x8B\x52\x10\xE9\x00\x00\x00\x00",
	"xxxxx????xxx????xxxxx????"
)
SIG_SCAN
(
	sigRenderLyricAndTitle,
	0x14027B150,
	"\x48\x8B\xC4\x48\x89\x58\x08\x48\x89\x70\x10\x48\x89\x78\x18\x55\x48\x8D\x68\xD8\x48\x81\xEC\x00\x00\x00\x00\x0F\x29\x70\xE8\x0F\x29\x78\xD8\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x45\xF0\x48\x8B\x5D\x50\x41\x8B\xF9\x41\x8B\xF0\x0F\x28\xF1\x0F\x28\xF8",
	"xxxxxxxxxxxxxxxxxxxxxxx????xxxxxxxxxxx????xxxxxxxxxxxxxxxxxxxxxxx"
)
SIG_SCAN
(
	sigLoadSongAudio,
	0x14023E2B0,
	"\x48\x89\x5C\x24\x00\x55\x56\x57\x48\x81\xEC\x00\x00\x00\x00\x0F\x29\xB4\x24\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x84\x24\x00\x00\x00\x00",
	"xxxx?xxxxxx????xxxx????xxx????xxxxxxx????"
)
SIG_SCAN
(
	sigUpdateLyrics,
	0x140244CD0,
	"\x48\x89\x5C\x24\x00\x57\x48\x83\xEC\x50\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x44\x24\x00\x48\x8B\xF9\x48\x8D\x99\x00\x00\x00\x00",
	"xxxx?xxxxxxxx????xxxxxxx?xxxxxx????"
)
SIG_SCAN
(
	sigGetInputState,
	0x1402AC960,
	"\x4C\x8B\x05\x00\x00\x00\x00\x4D\x85\xC0\x74\x35\x4C\x63\xC9\x48\xB8\x00\x00\x00\x00\x00\x00\x00\x00\x49\x8B\x48\x08\x49\x2B\x08\x48\xF7\xE9",
	"xxx????xxxxxxxxxx????????xxxxxxxxxx"
)
SIG_SCAN
(
	sigCopyCharsByCount,
	0x140192673,
	"\x48\x8B\xC2\x4D\x85\xC0\x74\x26\x4D\x8D\x48\xFF\x4C\x03\xC9\x49\x3B\xC9\x74\x17\x44\x0F\xB6\x00",
	"xxxxxxxxxxxxxxxxxxxxxxxx"
)
SIG_SCAN
(
	sigPVDBLookup,
	0x14018B100,
	"\x40\x53\x55\x41\x54\x41\x56\x41\x57\x48\x83\xEC\x40\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x44\x24\x00",
	"xxxxxxxxxxxxxxxx????xxxxxxx?"
)
SIG_SCAN(
	sigDivaPlayback,
	0x14024EB40,
	"\x40\x53\x55\x41\x54\x41\x57\x48\x81\xEC\x00\x00\x00\x00\x0F\x29\x74\x24\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x44\x24\x00\x80\x39\x00\x45\x0F\xB6\xF9",
	"xxxxxxxxxx????xxxx?xxx????xxxxxxx?xxxxxxx"
)
SIG_SCAN(
	sigDivaPlayback2,
	0x14CC60620,
	"\x40\x53\x48\x83\xEC\x20\x65\x48\x8B\x04\x25\x00\x00\x00\x00\xBA\x00\x00\x00\x00\x48\x8B\x08\x8B\x04\x0A\x39\x05\x00\x00\x00\x00\x7F\x0D",
	"xxxxxxxxxxx????x????xxxxxxxx????xx"
)
SIG_SCAN(
	sigpv_selector_switch__PvSelector__applySel,
	0x1406EFA30,
	"\x40\x57\x48\x81\xEC\x00\x00\x00\x00\x80\xB9\x00\x00\x00\x00\x00\x48\x8B\xF9\x74\x0B\x33\xC0\x48\x81\xC4\x00\x00\x00\x00\x5F\xC3",
	"xxxxx????xx?????xxxxxxxxxx????xx"
)
SIG_SCAN(
	sigMegaMix__CreateTaskWaitScreen,
	0x140653EF0,
	"\x48\x89\x5C\x24\x00\x48\x89\x4C\x24\x00\x57\x48\x83\xEC\x20\x48\x8D\x3D\x00\x00\x00\x00\x48\x89\x7C\x24\x00\x48\x8B\xCF\xE8\x00\x00\x00\x00\x90\x48\x8D\x05\x00\x00\x00\x00\x48\x89\x05\x00\x00\x00\x00\x33\xDB\x48\x89\x1D\x00\x00\x00\x00\x48\x89\x1D\x00\x00\x00\x00\x48\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x90\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x89\x1D\x00\x00\x00\x00",
	"xxxx?xxxx?xxxxxxxx????xxxx?xxxx????xxxx????xxx????xxxxx????xxx????xxx????????xxx????x????xxxx????x????xxx????"
)
FUNCTION_PTR(DivaPlaybackStatus2*, __fastcall, getPlaybackStatus2, sigDivaPlayback2Addr);
FUNCTION_PTR(PvSelector*, __fastcall, getPvSelAddr, readRelCall16Address((uint64_t)sigpv_selector_switch__PvSelector__applySelAddr + 0x1D0));

HOOK(int64_t, __fastcall, _DivaPlayback, sigDivaPlayback(),
	DivaPlaybackStatus* a1, __int64 a2, __int64 a3, char a4)
{
	playbackStatus = a1;
	return original_DivaPlayback(a1, a2, a3, a4);
}

HOOK(char*, __fastcall, _LoadSongAudio, sigLoadSongAudio(),
	__int64 a1,
	__int64 a2,
	std::string* a3,
	unsigned __int8 a4,
	int a5,
	float a6,
	float a7,
	int a8)
{
	LyricManager_Inst.SetSongAudio(a3->c_str());
	char* result = original_LoadSongAudio(a1, a2, a3, a4, a5, a6, a7, a8);
	return result;
}

HOOK(INT64, __fastcall, _ChangeGameState, sigChangeGameState(), INT64* a, const char* state) {
	INT64 result = original_ChangeGameState(a, state);
#ifdef _DEBUG
	LOG(L"Gamestate changed to : %S", state);
#endif // DEBUG
	LyricManager_Inst.UpdateGameState(state);
	return result;
}
std::string lyricsBuffer;
HOOK(void**, __fastcall, _RenderLyricAndTitle, sigRenderLyricAndTitle(),
	float y,
	float x,
	unsigned int scale,
	int a4,
	const char* text,
	char a6,
	unsigned int a7,
	void** a8)
{
	bool isLyric = y == 172.0f;
	if (isLyric) {
		lyricsBuffer = LyricManager_Inst.showInternalLyrics ? LyricManager_Inst.GetCurrentLyricLine() : "";
		// Maintain a local copy in case the return value gets destoryed
		return original_RenderLyricAndTitle(y, x, scale, a4, lyricsBuffer.c_str(), a6, a7, a8);
	}
	else {
		LyricManager_Inst.SetLyricLine(false, NULL);
		return original_RenderLyricAndTitle(y, x, scale, a4, text, a6, a7, a8);;
	}
}

HOOK(INT64, __fastcall, _UpdateLyrics, sigUpdateLyrics(), void* this_, int index, void* a3) {
	INT64 result = original_UpdateLyrics(this_, index, a3);
	LyricManager_Inst.UpdateLyricIndex(index);
	return result;
}

HOOK(void*, __fastcall, _GetInputState, sigGetInputState(), int player) {
	// TODO: Use this for ImGui's input handler
	// MM+ uses DirectInput for its main game.
	// But since there's no way to capture all keystrokes with this hook, I'll leave this as is.
	// Turns out dw_gui uses Win32 WndProc to handle K&M input as well...
	void* result = original_GetInputState(player);
	if (result && (ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantCaptureMouse))
	{
		return original_GetInputState(1);
		// Since there's no player 2...
	}
	return result;
}

HOOK(char, __fastcall, _CopyCharsByCount, sigCopyCharsByCount(), const char* out, const char* in, int length) {
	if (length == 0x4C && LyricManager_Inst.shouldShowLyrics()) {
		// Magic number! 76 chars is only used during lyrics rendering
		LyricManager_Inst.SetLyricLine(true, in);
	}
	return original_CopyCharsByCount(out, in, length);
}

HOOK(__int64*, __fastcall, _PVDBLookup, sigPVDBLookup(), __int64* tbl_address_end, __int64* tbl_address, char** tbl_name) {
	auto result = original_PVDBLookup(tbl_address_end, tbl_address, tbl_name);
	int n = (tbl_address_end - tbl_address) / 8;
	if (n >= 7 && strlen(*tbl_name) == 7) {
		int pvid; sscanf_s(*tbl_name, "pv_%d.", &pvid);
		if (pvid == 0) // New pv_db (or mod_ variant) is loaded
		{
			// The pv_db content, once read by MM+ , will be cleared at once
			// Copy it at init time (id=0) for our own uses.
			char* pvdb_buffer = (char*)(*(tbl_address_end)+3);
			std::istringstream pvdb(pvdb_buffer);
			std::string pvdb_line;
			while (std::getline(pvdb, pvdb_line)) {
				try
				{
					auto pvid = std::stoi(pvdb_line.substr(3, 3));
					FontManager_Inst.PVDB_Buffer[pvid].append(pvdb_line);
				}
				catch (...) {}
			}
		}
	}
	return result;
}
void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	if (pBackBuffer) {
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
		pBackBuffer->Release();
	}
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

#define VK_F 0x46
#define VK_G 0x47
#define VK_L 0x4C
#define VK_S 0x53
#define KEY_PRESSED(K) LOWORD(wParam) == K
#define KEY_PRESSED_WITH_CONTROL(K) KEY_PRESSED(K) && GetKeyState(VK_CONTROL)
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;
	switch (uMsg) {
	case WM_KEYDOWN:
		if (KEY_PRESSED_WITH_CONTROL(VK_F))
			FontManager_Inst.showGUI = !FontManager_Inst.showGUI;
		if (KEY_PRESSED_WITH_CONTROL(VK_G))
			LyricManager_Inst.showGUI = !LyricManager_Inst.showGUI;
		if (KEY_PRESSED_WITH_CONTROL(VK_S)) {
			Config::SaveGlobalConfig();
			MessageBeep(MB_ICONINFORMATION);
		}
		if (KEY_PRESSED_WITH_CONTROL(VK_L)) {
			Config::LoadGlobalConfig();
			MessageBeep(MB_ICONINFORMATION);
		}
		break;
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			CreateRenderTarget();
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return CallWindowProc(originalWndProc, hWnd, uMsg, wParam, lParam);
}

void Config::SaveGlobalConfig() {
	FontManager_Inst.ToConfig(GlobalConfig_Inst);
	LyricManager_Inst.ToConfig(GlobalConfig_Inst);
	GlobalConfig_Inst.ImGuiSettings = ImGui::SaveIniSettingsToMemory();
	GlobalConfig_Inst.Save();
}

void Config::LoadGlobalConfig() {
	GlobalConfig_Inst.Load();
	FontManager_Inst.FromConfig(GlobalConfig_Inst);
	LyricManager_Inst.FromConfig(GlobalConfig_Inst);
	if (GlobalConfig_Inst.ImGuiSettings.size() > 0)
		ImGui::LoadIniSettingsFromMemory(GlobalConfig_Inst.ImGuiSettings.c_str());
}

extern "C"
{
	void __declspec(dllexport) D3DInit(IDXGISwapChain* swapChain,
		ID3D11Device* device,
		ID3D11DeviceContext* deviceContext) {
		g_pSwapChain = swapChain;
		g_pd3dDevice = device;
		g_pd3dDeviceContext = deviceContext;

		DXGI_SWAP_CHAIN_DESC m_ChainDesc;
		swapChain->GetDesc(&m_ChainDesc);
		CreateRenderTarget();
		window = m_ChainDesc.OutputWindow;
		originalWndProc = (WNDPROC)SetWindowLongPtrA(window, GWLP_WNDPROC, (LONG_PTR)WndProc);

		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
	}
	void __declspec(dllexport) Init() {
		INSTALL_HOOK(_ChangeGameState);
		INSTALL_HOOK(_RenderLyricAndTitle);
		INSTALL_HOOK(_LoadSongAudio);
		INSTALL_HOOK(_UpdateLyrics);
		INSTALL_HOOK(_GetInputState);
		INSTALL_HOOK(_CopyCharsByCount);
		INSTALL_HOOK(_PVDBLookup);
		INSTALL_HOOK(_DivaPlayback);
		ImGui::CreateContext();
		SET_IMGUI_DEFAULT_FLAGS;
		Config::LoadGlobalConfig();
		LyricManager_Inst.Init(GlobalConfig_Inst);
		FontManager_Inst.Init(GlobalConfig_Inst);
		// Prepare addresses from sigs
		playbackStatus = new DivaPlaybackStatus; // acquried by the hook
		playbackStatus2 = getPlaybackStatus2();
		pvsel = getPvSelAddr();
		PVWaitScreenInfo = (PVWaitScreenInfoStruct*)readLongMOVAddress(
			(uint64_t)sigMegaMix__CreateTaskWaitScreenAddr + 0x34
		);
	}
	void __declspec(dllexport) OnFrame(IDXGISwapChain* m_pSwapChain) {
		LyricManager_Inst.OnFrame();
		FontManager_Inst.OnFrame();

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		LyricManager_Inst.OnImGUI();
		FontManager_Inst.OnImGUI();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
}