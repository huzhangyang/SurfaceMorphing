#include "SurfaceMorpher.h"

static clock_t startTime = 0;
static float duration = 2.0f;

vector<vec3> SurfaceMorpher::GetLinearInterpolation(Mesh* mesh1, Mesh* mesh2)
{
	if (startTime == 0)
		startTime = clock();

	float time = (clock() - startTime) / (float)CLOCKS_PER_SEC;
	time = std::min(duration, time);
	float t = time / duration;

	vector<vec3> vertices1 = mesh1->GetSequencedVertices()[0];
	vector<vec3> vertices2 = mesh2->GetSequencedVertices()[0];
	vector<vec3> intermediateVertices;
	for (int i = 0; i < vertices1.size(); i++)
	{
		vec3 v1 = vertices1[i];
		vec3 v2 = vertices2[i];
		vec3 interpolation = v1 + (v2 - v1) * t;
		intermediateVertices.push_back(interpolation);
	}

	return intermediateVertices;
}

vector<vec3> SurfaceMorpher::GetTransformBasedInterpolation(Mesh* mesh1, Mesh* mesh2)
{
	if (startTime == 0)
		startTime = clock();

	float time = (clock() - startTime) / (float)CLOCKS_PER_SEC;
	time = std::min(duration, time);
	float t = time / duration;// progress

	vector<vec3> vertices1 = mesh1->GetSequencedVertices()[0];
	vector<vec3> vertices2 = mesh2->GetSequencedVertices()[0];
	vector<vec3> intermediateVertices;

	return intermediateVertices;
}

void SurfaceMorpher::Reset()
{
	startTime = 0;
}

void SurfaceMorpher::SpeedUp()
{
	duration -= 0.5f;
	duration = std::max(0.5f, duration);
	Reset();
}

void SurfaceMorpher::SpeedDown()
{
	duration += 0.5f;
	Reset();
}

void SurfaceMorpher::SpeedReset()
{
	duration = 2.0f;
	Reset();
}
