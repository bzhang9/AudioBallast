#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include "State.h"
//req?
#include "MainWnd.h"

IAudioSessionManager2 *getAudioManager();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	HRESULT result = CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE);
	if (FAILED(result)) {
		return 1;
	}

	MainWnd win{getAudioManager()};

	//TODO: not working
	if (!win.Create(L"AudioBallast", WS_OVERLAPPEDWINDOW))
	{
		return 0;
	}

	win.init();
	ShowWindow(win.Window(), nCmdShow);

	// Run the message loop.

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	CoUninitialize();
	return 0;
}

IAudioSessionManager2 *getAudioManager() {
	const CLSID CLSID_MMDevEnum = __uuidof(MMDeviceEnumerator);
	const IID IID_IASM2 = _uuidof(IAudioSessionManager2);

	IMMDeviceEnumerator *mmDevEnum;

	HRESULT result = CoCreateInstance(
		CLSID_MMDevEnum, 
		nullptr,
		CLSCTX_ALL,
		IID_PPV_ARGS(&mmDevEnum)
	);

	if (FAILED(result)) {
		return nullptr;
	}

	IMMDevice *device;
	// TODO: check eMultimedia
	result = mmDevEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &device);

	if (FAILED(result)) {
		mmDevEnum->Release();
		return nullptr;
	}

	IAudioSessionManager2 *manager;

	result = device->Activate(
		IID_IASM2,
		CLSCTX_ALL,
		nullptr,
		(void**)&manager
	);

	device->Release();
	mmDevEnum->Release();

	if (FAILED(result)) {
		return nullptr;
	}

	return manager;
}

// unused?
bool setWndState(HWND hwnd, LPARAM lp) {
	CREATESTRUCT *wrapper = reinterpret_cast<CREATESTRUCT *>(lp);
	State *state = reinterpret_cast<State *>(wrapper->lpCreateParams);
	if (!state) {
		return false;
	}
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)state);
	return true;
}

State *getWndState(HWND hwnd) {
	LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (!ptr) {
		return nullptr;
	}
	return (reinterpret_cast<State *>(ptr));
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	State *state;
	if (uMsg == WM_CREATE) {
		setWndState(hwnd, lParam);
	}
	else {
		state = getWndState(hwnd);
	}

	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(hwnd, &ps);
	}
	return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


