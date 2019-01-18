#pragma once
#include <Audiopolicy.h>

class SessionListener : public IAudioSessionNotification {
	LONG m_cRefAll;
	HWND m_hwndMain;
	
public:
	static const UINT SL_SESSION_CREATED = 0x0400;

	SessionListener(HWND hwnd);

	~SessionListener();

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID, void **);

	ULONG STDMETHODCALLTYPE AddRef(void);

	ULONG STDMETHODCALLTYPE Release(void);

	HRESULT STDMETHODCALLTYPE OnSessionCreated(IAudioSessionControl *);

	void setWnd(HWND &wnd);
};