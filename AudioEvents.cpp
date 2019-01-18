#include "AudioEvents.h"

class AudioControl;

HWND AudioEvents::m_hwndMain = nullptr;

void AudioEvents::setHwnd(HWND hwnd) {
	m_hwndMain = hwnd;
}

AudioEvents::AudioEvents(AudioControl *controller) : 
	controller{ controller }, m_cRefAll { 1 }
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
	return E_NOTIMPL;
}

HRESULT AudioEvents::OnStateChanged(AudioSessionState NewState)
{
	return E_NOTIMPL;
}

HRESULT __stdcall AudioEvents::QueryInterface(REFIID, void **)
{
	return S_OK;
}

ULONG __stdcall AudioEvents::AddRef(void)
{
	return 0;
}

ULONG __stdcall AudioEvents::Release(void)
{
	return 0;
}
