#pragma once

#include "global.h"

class Mesh {
public:
	vector<vec3>* GetSequencedVertices()
	{
		return &outVertices;
	}

	vector<vec3>* GetVertices()
	{
		return &rawVertices;
	}

	vector<vec2>* GetUVs()
	{
		return &outUVs;
	}

	vector<vec3>* GetNormals()
	{
		return &outNormals;
	}

	vector<int>* GetVertexIndices()
	{
		return &vertexIndices;
	}

	vector<int> vertexIndices, uvIndices, normalIndices;
	vector<vec3> rawVertices;
	vector<vec2> rawUVs;
	vector<vec3> rawNormals;
	vector<vec3> outVertices;
	vector<vec2> outUVs;
	vector<vec3> outNormals;
private:
};

class MeshLoader {
public:
	static Mesh* LoadObj2D(string filename);
private:
	static vector<string> split(string& s, const char* delim);
};