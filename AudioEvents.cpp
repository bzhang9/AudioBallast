#include "AudioEvents.h"

class AudioControl;

HWND AudioEvents::m_hwndMain = nullptr;

void AudioEvents::setHwnd(HWND hwnd) {
	m_hwndMain = hwnd;
}

AudioEvents::AudioEvents(AudioControl *controller) : 
	controller{ controller }, refCount { 1 }
{

}

AudioEvents::~AudioEvents() {

}

HRESULT AudioEvents::OnChannelVolumeChanged(DWORD ChannelCount, float NewChannelVolumeArray[], DWORD ChangedChannel, LPCGUID EventContext)
{
	return E_NOTIMPL;
}

HRESULT AudioEvents::OnDisplayNameChanged(LPCWSTR NewDisplayName, LPCGUID EventContext)
{
	return E_NOTIMPL;
}

HRESULT AudioEvents::OnGroupingParamChanged(LPCGUID NewGroupingParam, LPCGUID EventContext)
{
	return E_NOTIMPL;
}

HRESULT AudioEvents::OnIconPathChanged(LPCWSTR NewIconPath, LPCGUID EventContext)
{
	return E_NOTIMPL;
}

HRESULT AudioEvents::OnSessionDisconnected(AudioSessionDisconnectReason DisconnectReason)
{
	PostMessage(m_hwndMain, AE_DISC, (WPARAM)controller, DisconnectReason);
	return TRUE;
}

HRESULT AudioEvents::OnSimpleVolumeChanged(float NewVolume, BOOL NewMute, LPCGUID EventContext)
{
	// TODO: class containing newvol and mute
	PostMessage(m_hwndMain, AE_SVOL, (WPARAM)controller, (LPARAM)0);
	return TRUE;
}

HRESULT AudioEvents::OnStateChanged(AudioSessionState NewState)
{
	return E_NOTIMPL;
}

HRESULT __stdcall AudioEvents::QueryInterface(REFIID riid, void **ppvInterface)
{
	if (IID_IUnknown == riid)
	{
		AddRef();
		*ppvInterface = (IUnknown*)this;
	}
	else if (__uuidof(IAudioSessionEvents) == riid)
	{
		AddRef();
		*ppvInterface = (IAudioSessionEvents*)this;
	}
	else
	{
		*ppvInterface = NULL;
		return E_NOINTERFACE;
	}
	return S_OK;
}

ULONG __stdcall AudioEvents::AddRef()
{
	return InterlockedIncrement(&refCount);
}

ULONG __stdcall AudioEvents::Release()
{
	ULONG ulRef = InterlockedDecrement(&refCount);
	if (0 == ulRef)
	{
		delete this;
	}
	return ulRef;
}
