#include "SurfaceMorpher.h"

vector<vec3> SurfaceMorpher::GetLinearInterpolation(vector<vec3> vertices1, vector<vec3> vertices2, float duration)
{
	static clock_t startTime = clock();

	float time = (clock() - startTime) / (float)CLOCKS_PER_SEC;
	time = min(duration, time);

	vector<vec3> intermediateVertices;
	for (int i = 0; i < vertices1.size(); i++)
	{
		vec3 v1 = vertices1[i];
		vec3 v2 = vertices2[i];
		vec3 interpolation = v1 + (v2 - v1) * time / duration;
		intermediateVertices.push_back(interpolation);
	}

	return intermediateVertices;
}

vector<vec3> SurfaceMorpher::GetTransformBasedInterpolation(vector<vec3> vertices1, vector<vec3> vertices2, float duration)
{
	return vertices1;
}
