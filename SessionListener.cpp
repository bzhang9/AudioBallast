#include <windows.h>
#include "SessionListener.h"

SessionListener::SessionListener(HWND hwnd) : m_cRefAll{ 1 }, m_hwndMain{ hwnd } {}

SessionListener::~SessionListener() {}

HRESULT STDMETHODCALLTYPE SessionListener::QueryInterface(REFIID riid, void **ppvInterface) {
	if (IID_IUnknown == riid)
	{
		AddRef();
		*ppvInterface = (IUnknown*)this;
	}
	else if (__uuidof(IAudioSessionNotification) == riid)
	{
		AddRef();
		*ppvInterface = (IAudioSessionNotification*)this;
	}
	else
	{
		*ppvInterface = NULL;
		return E_NOINTERFACE;
	}
	return S_OK;
}

ULONG STDMETHODCALLTYPE SessionListener::AddRef() {
	return InterlockedIncrement(&m_cRefAll);
}

ULONG STDMETHODCALLTYPE SessionListener::Release() {
	ULONG ulRef = InterlockedDecrement(&m_cRefAll);
	if (0 == ulRef)
	{
		delete this;
	}
	return ulRef;
}

//TODO: invalid ref
HRESULT STDMETHODCALLTYPE SessionListener::OnSessionCreated(IAudioSessionControl *NewSession) {
	if (NewSession) {
		//PostMessage(m_hwndMain, WM_, 0, 0);
	}
	return TRUE;
}

