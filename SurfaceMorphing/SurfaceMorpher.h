#pragma once

#include <ctime>
#include "Eigen/Dense"
#include "global.h"
#include "MeshLoader.h"

using namespace Eigen;

class SurfaceMorpher {
public:
	static vector<vec3> GetLinearInterpolation(Mesh* mesh1, Mesh* mesh2);
	static vector<vec3> GetTransformBasedInterpolation(Mesh* mesh1, Mesh* mesh2);
	static int GetCurrentIndex();
	static void PrepareNextInterpolation();
	static float GetInterpolationProgress();
	static void Reset();
	static void SpeedUp();
	static void SpeedDown();
	static void SpeedReset();
private:
};
