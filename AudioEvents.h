#pragma once

#include <audiopolicy.h>

class AudioControl;

class AudioEvents : public IAudioSessionEvents {
	AudioControl *controller;
	LONG m_cRefAll;
	static HWND m_hwndMain;
public:
	static const UINT AE_CHVOL = 0x0401;
	static const UINT AE_DISPNAME = 0x0402;
	static const UINT AE_GRP = 0x0403;
	static const UINT AE_ICON = 0x0404;
	static const UINT AE_DISC = 0x0405;
	static const UINT AE_SVOL = 0x0406;
	static const UINT AE_STATE = 0x0407;

	static void setHwnd(HWND hwnd);

	AudioEvents(AudioControl *);

	~AudioEvents();

	HRESULT STDMETHODCALLTYPE OnChannelVolumeChanged(
		DWORD    ChannelCount,
		float	 NewChannelVolumeArray[],
		DWORD    ChangedChannel,
		LPCGUID  EventContext
	);

	HRESULT STDMETHODCALLTYPE OnDisplayNameChanged(
		LPCWSTR NewDisplayName,
		LPCGUID EventContext
	);

	HRESULT STDMETHODCALLTYPE OnGroupingParamChanged(
		LPCGUID NewGroupingParam,
		LPCGUID EventContext
	);

	HRESULT STDMETHODCALLTYPE OnIconPathChanged(
		LPCWSTR NewIconPath,
		LPCGUID EventContext
	);

	HRESULT STDMETHODCALLTYPE OnSessionDisconnected(
		AudioSessionDisconnectReason DisconnectReason
	);

	HRESULT STDMETHODCALLTYPE OnSimpleVolumeChanged(
		float   NewVolume,
		BOOL    NewMute,
		LPCGUID EventContext
	);

	HRESULT STDMETHODCALLTYPE OnStateChanged(
		AudioSessionState NewState
	);

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID, void **);

	ULONG STDMETHODCALLTYPE AddRef(void);

	ULONG STDMETHODCALLTYPE Release(void);
};