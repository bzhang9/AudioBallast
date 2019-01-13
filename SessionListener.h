#pragma once
#include <Audiopolicy.h>

class SessionListener : public IAudioSessionNotification {
	LONG m_cRefAll;
	HWND m_hwndMain;

public:
	SessionListener(HWND hwnd);

	~SessionListener();

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID, void **);

	ULONG STDMETHODCALLTYPE AddRef(void);

	ULONG STDMETHODCALLTYPE Release(void);

	HRESULT STDMETHODCALLTYPE OnSessionCreated(IAudioSessionControl *);

};