#include "AudioControl.h"

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