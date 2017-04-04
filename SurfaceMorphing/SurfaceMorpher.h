#pragma once

#include "global.h"
#include <ctime>

class SurfaceMorpher {
public:
	static vector<vec3> GetLinearInterpolation(vector<vec3> vertices1, vector<vec3> vertices2, float duration = 3.0f);
	static vector<vec3> GetTransformBasedInterpolation(vector<vec3> vertices1, vector<vec3> vertices2, float duration = 3.0f);
private:
};
