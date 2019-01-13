#pragma once

class AudioControl {
	int volume;
	bool isMuted;
	// Track process

public:
	int getVolume();

	bool setVolume(int);

	void mute();

	void unmute();

	bool audible();
};