#pragma once
#include <windows.h>
#include <audiopolicy.h>
#include <d2d1.h>

//struct D2D1_ROUNDED_RECT;
//class D2D1::ColorF;

class AudioControl {
	IAudioSessionControl *control;
	ISimpleAudioVolume *volControl;
	IAudioSessionEvents *listener;
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

	const D2D1::ColorF &getForeClr();

	const D2D1::ColorF &getBackClr();
};