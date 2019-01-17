#include "PxlConverter.h"
#include <d2d1.h>

float PxlConverter::xMult = 1.0f;
float PxlConverter::yMult = 1.0f;

void PxlConverter::init(ID2D1Factory *fact) {
	FLOAT xDpi;
	FLOAT yDpi;
	fact->GetDesktopDpi(&xDpi, &yDpi);
	xMult = xDpi / 96.0f;
	yMult = yDpi / 96.0f;
}

