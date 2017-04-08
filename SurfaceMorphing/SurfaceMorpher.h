#pragma once

#include "global.h"
#include <ctime>

class SurfaceMorpher {
public:
	static vector<vec3> GetLinearInterpolation(vector<vec3> vertices1, vector<vec3> vertices2);
	static vector<vec3> GetTransformBasedInterpolation(vector<vec3> vertices1, vector<vec3> vertices2);
	static void Reset();
	static void SpeedUp();
	static void SpeedDown();
	static void SpeedReset();
private:
};
