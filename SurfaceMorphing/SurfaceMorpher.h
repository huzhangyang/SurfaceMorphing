#pragma once

#include <ctime>
#include "Eigen/Dense"
#include "global.h"
#include "MeshLoader.h"

using namespace Eigen;

enum InterpolationMethod { Linear = 1, TransformBased = 2 , SurfaceBased = 3};

class SurfaceMorpher {
public:
	static vector<vec3> GetInterpolation(Mesh* mesh1, Mesh* mesh2);
	static int GetCurrentIndex();
	static float GetInterpolationProgress();
	static void PrepareNextInterpolation();
	static void SetInterpolationMethod(InterpolationMethod _method);
	static void Reset();
	static void SpeedUp();
	static void SpeedDown();
	static void SpeedReset();
private:
	static vector<vec3> GetLinearInterpolation(Mesh* mesh1, Mesh* mesh2);
	static vector<vec3> GetTransformBasedInterpolation(Mesh* mesh1, Mesh* mesh2);
	static vector<vec3> GetSurfaceBasedInterpolation(Mesh* mesh1, Mesh* mesh2);
};
