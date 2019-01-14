#include <d2d1.h>
#include "AudioControl.h"

/*AudioControl::AudioControl(IAudioSessionControl *control, D2D1_ROUNDED_RECT *element) : 
	control{ control }, 
	element{ element } 
{}*/

AudioControl::AudioControl(IAudioSessionControl *control, ISimpleAudioVolume *volControl) :
	control{ control },
	volControl{ volControl },
	element{ nullptr }
{

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

