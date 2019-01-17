#include <d2d1.h>
#include "AudioControl.h"

/*AudioControl::AudioControl(IAudioSessionControl *control, D2D1_ROUNDED_RECT *element) : 
	control{ control }, 
	element{ element } 
{}*/

AudioControl::AudioControl(IAudioSessionControl *control) :
	control{ control },
	volControl{ nullptr },
	element{ nullptr }
{
	HRESULT result = control->QueryInterface(__uuidof(ISimpleAudioVolume), (void **)&volControl);
	if (FAILED(result)) {
		return;
	}
}

AudioControl::~AudioControl() {
	delete element;
	volControl->Release();
	control->Release();
}

float AudioControl::getVolume() {
	float volume;
	HRESULT result = volControl->GetMasterVolume(&volume);
	if (FAILED(result)) {
		return 0;
	}
	return volume;
}

bool AudioControl::setVolume(float vol) {
	if (vol > 1.0f || vol < 0.0f) {
		return false;
	}
	// TODO: event context
	HRESULT result = volControl->SetMasterVolume(vol, nullptr);
	if (FAILED(result)) {
		return false;
	}
	return true;
}

bool AudioControl::setMute(BOOL mute) {
	HRESULT result = volControl->SetMute(mute, nullptr);
	return (SUCCEEDED(result));
}

bool AudioControl::mute() {
	return setMute(TRUE);
}

bool AudioControl::unmute() {
	return setMute(FALSE);
}

bool AudioControl::audible() {
	BOOL isMuted;
	HRESULT result = volControl->GetMute(&isMuted);
	if (FAILED(result)) {
		return false;
	}
	return (isMuted != TRUE && getVolume() > 0.0f);
}

D2D1_ROUNDED_RECT *AudioControl::getElement() {
	return element;
}

void AudioControl::setElement(D2D1_ROUNDED_RECT *rect) {
	element = rect;
}

void AudioControl::deleteElement() {
	delete element;
}

const D2D1::ColorF &AudioControl::getForeClr() {
	float vol = getVolume();
	float r = vol > 0.5f ? 2.0f - 2 * vol : 1.0f;
	float g = vol > 0.5f ? 1.0f : 2 * vol;
	return D2D1::ColorF(r, g, 0);
}

const D2D1::ColorF &AudioControl::getBackClr() {
	float vol = getVolume();
	float r = vol > 0.5f ? 2.0f - 2 * vol : 1.0f;
	float g = vol > 0.5f ? 1.0f : 2 * vol;
	return D2D1::ColorF(r, g, 0, 0.5f);
}