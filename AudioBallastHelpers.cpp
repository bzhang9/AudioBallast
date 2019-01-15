#include "AudioBallastHelpers.h"

template <class T> void AudioBallastHelpers::SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = nullptr;
	}
}