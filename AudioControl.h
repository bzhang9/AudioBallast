#pragma once
#include <windows.h>
#include <audiopolicy.h>

struct D2D1_ROUNDED_RECT;

class AudioControl {
	IAudioSessionControl *control;
	D2D1_ROUNDED_RECT *element;

	int volume;
	bool isMuted;
	// Track process

public:
	AudioControl(IAudioSessionControl *, D2D1_ROUNDED_RECT *);

	AudioControl(IAudioSessionControl *);

	~AudioControl();

	int getVolume();

	bool setVolume(int);

	void mute();

	void unmute();

	bool audible();

	D2D1_ROUNDED_RECT *getElement();

	void setElement(D2D1_ROUNDED_RECT *);

	void deleteElement();

	
};