#pragma once
#include <windows.h>
#include <audiopolicy.h>

struct D2D1_ROUNDED_RECT;

class AudioControl {
	IAudioSessionControl *control;
	ISimpleAudioVolume *volControl;
	D2D1_ROUNDED_RECT *element;

	// Track process
	bool setMute(BOOL);

public:
	//AudioControl(IAudioSessionControl *, D2D1_ROUNDED_RECT *);

	AudioControl(IAudioSessionControl *);

	~AudioControl();

	float getVolume();

	bool setVolume(float);

	bool mute();

	bool unmute();

	bool audible();

	D2D1_ROUNDED_RECT *getElement();

	void setElement(D2D1_ROUNDED_RECT *);

	void deleteElement();

	
};