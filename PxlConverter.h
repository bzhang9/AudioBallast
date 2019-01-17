#pragma once
#include <d2d1.h>

class PxlConverter {
	static float xMult;
	static float yMult;

public:
	static void init(ID2D1Factory *);
	template<typename T> 
	static D2D1_POINT_2F toDip(T x, T y) {
		return D2D1::Point2F(static_cast<float>(x) / xMult, static_cast<float>(y) / yMult);
	}
};