#include <d2d1.h>
#include "AudioControl.h"

AudioControl::AudioControl(IAudioSessionControl *control, D2D1_ROUNDED_RECT *element) : 
	control{ control }, 
	element{ element } 
{}

AudioControl::AudioControl(IAudioSessionControl *) :
	control{ control },
	element{ nullptr }
{

}

AudioControl::~AudioControl() {
	delete element;
	control->Release();
}

int AudioControl::getVolume() {
	return volume;
}

bool AudioControl::setVolume(int vol) {
	if (vol > 100 || vol < 0) {
		return false;
	}
	volume = vol;
}

void AudioControl::mute() {
	isMuted = true;
	// Process
}

void AudioControl::unmute() {
	isMuted = false;
}

bool AudioControl::audible() {
	return (!isMuted && volume > 0);
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

