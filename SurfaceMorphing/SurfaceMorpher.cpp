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
	
	//algorithm
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
	float t = time / duration;

	vector<vec3> vertices1 = mesh1->GetSequencedVertices()[0];
	vector<vec3> vertices2 = mesh2->GetSequencedVertices()[0];
	vector<vec3> intermediateVertices;

	//algorithm
	size_t faceCount = vertices1.size() / 3;
	size_t verticesCount = mesh1->GetVertices()[0].size();

	MatrixXf A(4 * faceCount + 2, 2 * verticesCount), B(4 * faceCount + 2, 1);
	MatrixXf P(6, 6), Q(6, 1), Affine(2, 2), Rt(2, 2), I(2, 2);
	I << 1, 0, 0, 1;

	for (int i = 0; i < faceCount; i++)
	{
		vec3 v11 = vertices1[3 * i];
		vec3 v12 = vertices1[3 * i + 1];
		vec3 v13 = vertices1[3 * i + 2];
		vec3 v21 = vertices2[3 * i];
		vec3 v22 = vertices2[3 * i + 1];
		vec3 v23 = vertices2[3 * i + 2];

		P << v11.x, v11.y, 1, 0, 0, 0,
			0, 0, 0, v11.x, v11.y, 1,
			v12.x, v12.y, 1, 0, 0, 0,
			0, 0, 0, v12.x, v12.y, 1,
			v13.x, v13.y, 1, 0, 0, 0,
			0, 0, 0, v13.x, v13.y, 1;

		Q << v21.x, v21.y, v22.x, v22.y, v23.x, v23.y;

		MatrixXf PI = P.inverse();
		MatrixXf PIQ = PI * Q;
		Affine << PIQ(0, 0), PIQ(1, 0), PIQ(3, 0), PIQ(4, 0);

		JacobiSVD<MatrixXf> svd(Affine, Eigen::ComputeThinU | Eigen::ComputeThinV);
		auto U = svd.matrixU();
		auto S = svd.singularValues();
		auto V = svd.matrixV();
		Matrix2f D;
		D << S[0], 0, 0, S[1];

		MatrixXf R = U * V.transpose();
		MatrixXf Symmetric = V * D * V.transpose();
		Rt << (R(0, 0) - 1) * t + 1, R(0, 1) * t, R(1, 0) * t, (R(1, 1) - 1) * t + 1;
		MatrixXf At = Rt * (I * (1 - t) + Symmetric * t);

		B(4 * i) = At(0, 0);
		B(4 * i + 1) = At(0, 1);
		B(4 * i + 2) = At(1, 0);
		B(4 * i + 3) = At(1, 1);

		for (int k = 0; k < 3; k++)
		{
			A(4 * i, 2 * mesh1->vertexIndices[3 * i + k]) = PI(0, 2 * k);
			A(4 * i, 2 * mesh1->vertexIndices[3 * i + k] + 1) = PI(0, 2 * k + 1);
			A(4 * i + 1, 2 * mesh1->vertexIndices[3 * i + k]) = PI(1, 2 * k);
			A(4 * i + 1, 2 * mesh1->vertexIndices[3 * i + k] + 1) = PI(1, 2 * k + 1);
			A(4 * i + 2, 2 * mesh1->vertexIndices[3 * i + k]) = PI(3, 2 * k);
			A(4 * i + 2, 2 * mesh1->vertexIndices[3 * i + k] + 1) = PI(3, 2 * k + 1);
			A(4 * i + 3, 2 * mesh1->vertexIndices[3 * i + k]) = PI(4, 2 * k);
			A(4 * i + 3, 2 * mesh1->vertexIndices[3 * i + k] + 1) = PI(4, 2 * k + 1);
		}

	}

	B(4 * faceCount) = vertices1[0].x * (1 - t) + vertices2[0].x * t;
	B(4 * faceCount + 1) = vertices1[0].y * (1 - t) + vertices2[0].y * t;

	A(4 * faceCount, 0) = 1;
	A(4 * faceCount + 1, 1) = 1;

	auto newV = (A.transpose() * A).llt().solve(A.transpose() * B);

	vector<vec3> newVertices;
	for (int i = 0; i< verticesCount; i++) 
	{
		vec3 vertex(newV(2 * i, 0), newV(2 * i + 1, 0), 0);
		newVertices.push_back(vertex);
	}

	for (unsigned int i = 0; i< mesh1->vertexIndices.size(); i++)
	{
		unsigned int vertexIndex = mesh1->vertexIndices[i];
		vec3 vertex = newVertices[vertexIndex];
		intermediateVertices.push_back(vertex);
	}

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
