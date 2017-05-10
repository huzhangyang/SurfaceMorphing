#include "SurfaceMorpher.h"

static clock_t startTime = 0;
static float duration = 2.0f;
static float t = 0.0f;
static int currentMeshIndex = 0;
InterpolationMethod method = InterpolationMethod::SurfaceBased;

vector<vec3> SurfaceMorpher::GetLinearInterpolation(Mesh* mesh1, Mesh* mesh2)
{
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
	vector<vec3> vertices1 = mesh1->GetSequencedVertices()[0];
	vector<vec3> vertices2 = mesh2->GetSequencedVertices()[0];
	vector<vec3> intermediateVertices;

	//algorithm
	size_t faceCount = vertices1.size() / 3;
	size_t verticesCount = mesh1->GetVertices()[0].size();

	MatrixXf A(4 * faceCount, 2 * verticesCount - 2), B(4 * faceCount, 1), P(6, 6), Q(6, 1);
	Matrix2f Affine, Rt, D, I;
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

		for (int j = 0; j < 3; j++)
		{
			if (mesh1->vertexIndices[3 * i + j] > 0)
			{
				A(4 * i + 0, 2 * mesh1->vertexIndices[3 * i + j] - 2) = PI(0, 2 * j + 0);
				A(4 * i + 0, 2 * mesh1->vertexIndices[3 * i + j] - 1) = PI(0, 2 * j + 1);
				A(4 * i + 1, 2 * mesh1->vertexIndices[3 * i + j] - 2) = PI(1, 2 * j + 0);
				A(4 * i + 1, 2 * mesh1->vertexIndices[3 * i + j] - 1) = PI(1, 2 * j + 1);
				A(4 * i + 2, 2 * mesh1->vertexIndices[3 * i + j] - 2) = PI(3, 2 * j + 0);
				A(4 * i + 2, 2 * mesh1->vertexIndices[3 * i + j] - 1) = PI(3, 2 * j + 1);
				A(4 * i + 3, 2 * mesh1->vertexIndices[3 * i + j] - 2) = PI(4, 2 * j + 0);
				A(4 * i + 3, 2 * mesh1->vertexIndices[3 * i + j] - 1) = PI(4, 2 * j + 1);
			}
		}

		JacobiSVD<MatrixXf> svd(Affine, ComputeThinU | ComputeThinV);
		auto U = svd.matrixU();
		auto S = svd.singularValues();
		auto V = svd.matrixV();

		D << S[0], 0, 0, S[1];

		MatrixXf R = U * V.transpose();
		MatrixXf Symmetric = V * D * V.transpose();

		Matrix3f R33;
		R33 << R(0, 0), R(0, 1), 0, R(1, 0), R(1, 1), 0, 0, 0, 1;
		Quaternionf q(R33);
		Quaternionf q0(1.0f, 0.0f, 0.0f, 0.0f);
		Rt = q0.slerp(t, q).toRotationMatrix().block(0, 0, 2, 2); // quaternion interpolation
		MatrixXf At = Rt * ((1 - t) * I + t * Symmetric);

		B(4 * i) = At(0, 0);
		B(4 * i + 1) = At(0, 1);
		B(4 * i + 2) = At(1, 0);
		B(4 * i + 3) = At(1, 1);
	}

	MatrixXf newV = (A.transpose() * A).llt().solve(A.transpose() * B);

	vector<vec3> newVertices;

	float v0x = mesh1->GetVertices()[0][0].x + (mesh2->GetVertices()[0][0].x - mesh1->GetVertices()[0][0].x) * t;
	float v0y = mesh1->GetVertices()[0][0].y + (mesh2->GetVertices()[0][0].y - mesh1->GetVertices()[0][0].y) * t;
	newVertices.push_back(vec3(v0x, v0y, 0));
	for (int i = 1; i < verticesCount; i++)
	{
		vec3 vertex(newV(2 * i - 2), newV(2 * i - 1), 0);
		newVertices.push_back(vertex);
		//cout << "ori:" << vertices1[i].x << "," << vertices1[i].y << endl;
		//cout << "new:" << vertex.x << "," << vertex.y << endl;
	}

	for (unsigned int i = 0; i < mesh1->vertexIndices.size(); i++)
	{
		unsigned int vertexIndex = mesh1->vertexIndices[i];
		vec3 vertex = newVertices[vertexIndex];
		intermediateVertices.push_back(vertex);
	}

	return intermediateVertices;
}

vector<vec3> SurfaceMorpher::GetSurfaceBasedInterpolation(Mesh * mesh1, Mesh * mesh2)
{
	vector<vec3> vertices1 = mesh1->GetSequencedVertices()[0];
	vector<vec3> vertices2 = mesh2->GetSequencedVertices()[0];
	vector<vec3> intermediateVertices;

	//algorithm
	size_t faceCount = vertices1.size() / 3;
	size_t verticesCount = mesh1->GetVertices()[0].size();
	float alpha = 1 / (float)sqrt(SCREEN_WIDTH * SCREEN_WIDTH + SCREEN_HEIGHT * SCREEN_HEIGHT);

	MatrixXf K = MatrixXf::Zero(faceCount * 4, faceCount + verticesCount);
	MatrixXf Bx = MatrixXf::Zero(faceCount * 4, 1), By = MatrixXf::Zero(faceCount * 4, 1);
	Matrix2f U, V, Rt, D, I;

	for (int i = 0; i < faceCount; i++)
	{
		vec3 v1 = vertices1[3 * i];
		vec3 v2 = vertices1[3 * i + 1];
		vec3 v3 = vertices1[3 * i + 2];
		vec3 u1 = vertices2[3 * i];
		vec3 u2 = vertices2[3 * i + 1];
		vec3 u3 = vertices2[3 * i + 2];
		//vec3 v4 = (v1 + v2 + v3) / 3.0f + (v2 - v1) * (v3 - v2) / sqrt(abs(v2 - v1) * abs(v3 - v2)); // no use of v4 in 2D
		V << v1.x - v3.x, v2.x - v3.x, v1.y - v3.y, v2.y - v3.y;
		U << u1.x - u3.x, u2.x - u3.x, u1.y - u3.y, u2.y - u3.y;

		MatrixXf M = V * U.inverse();
		Vector2f T = Vector2f(v1.x, v1.y) - M * Vector2f(u1.x, u1.y);

		JacobiSVD<MatrixXf> svd(M, ComputeThinU | ComputeThinV);
		auto U = svd.matrixU();
		auto S = svd.singularValues();
		auto V = svd.matrixV();

		D << S[0], 0, 0, S[1];
		MatrixXf R = U * V.transpose();
		MatrixXf Symmetric = V * D * V.transpose();

		Matrix3f R33;
		R33 << R(0, 0), R(0, 1), 0, R(1, 0), R(1, 1), 0, 0, 0, 1;
		Quaternionf q(R33);
		Quaternionf q0(1.0f, 0.0f, 0.0f, 0.0f);
		Rt = q0.slerp(t, q).toRotationMatrix().block(0, 0, 2, 2); // quaternion interpolation
		MatrixXf At = Rt * ((1 - t) * I + t * Symmetric);

		vec3 cp = (v2 - v1) * (v3 - v1);
		float area = 0.5f * sqrt(cp.x * cp.x + cp.y * cp.y);

		MatrixXf KX(3, 3);
		KX << v1.x, v1.y, 1,
			v2.x, v2.y, 1,
			v3.x, v3.y, 1;
		MatrixXf KXI = KX.inverse();
		for (int j = 0; j < 3; j++) 
		{
			K(i + j, mesh1->vertexIndices[3 * i + 0]) = area * KXI(j, 0);
			K(i + j, mesh1->vertexIndices[3 * i + 1]) = area * KXI(j, 1);
			K(i + j, mesh1->vertexIndices[3 * i + 2]) = 0;
			K(i + j, verticesCount + i) = area * KXI(j, 2);
		}

		Bx(4 * i + 0) = area * At(0, 0);
		By(4 * i + 0) = area * At(1, 0);
		Bx(4 * i + 1) = area * At(0, 1);
		By(4 * i + 1) = area * At(1, 1);
		Bx(4 * i + 2) = 0;
		By(4 * i + 2) = 0;
		Bx(4 * i + 3) = area * T(0) * t * alpha;
		By(4 * i + 3) = area * T(1) * t * alpha;
	}

	MatrixXf newVx = (K.transpose() * K).llt().solve(K.transpose() * Bx);
	MatrixXf newVy = (K.transpose() * K).llt().solve(K.transpose() * By);

	vector<vec3> newVertices;
	for (int i = 0; i < verticesCount; i++)
	{
		vec3 vertex(newVx(i), newVy(i), 0);
		newVertices.push_back(vertex);
		cout << "ori:" << vertices1[i].x << "," << vertices1[i].y << endl;
		cout << "new:" << vertex.x << "," << vertex.y << endl;
	}

	for (unsigned int i = 0; i < mesh1->vertexIndices.size(); i++)
	{
		unsigned int vertexIndex = mesh1->vertexIndices[i];
		vec3 vertex = newVertices[vertexIndex];
		intermediateVertices.push_back(vertex);
	}
	return intermediateVertices;
}

vector<vec3> SurfaceMorpher::GetInterpolation(Mesh * mesh1, Mesh * mesh2)
{
	if (method == InterpolationMethod::Linear)
	{
		return GetLinearInterpolation(mesh1, mesh2);
	}
	else if (method == InterpolationMethod::TransformBased)
	{
		return GetTransformBasedInterpolation(mesh1, mesh2);
	}
	else if (method == InterpolationMethod::SurfaceBased)
	{
		return GetSurfaceBasedInterpolation(mesh1, mesh2);
	}
	return vector<vec3>();
}

int SurfaceMorpher::GetCurrentIndex()
{
	return currentMeshIndex;
}

void SurfaceMorpher::PrepareNextInterpolation()
{
	startTime = 0;
	t = 0.0f;
	currentMeshIndex++;
}

void SurfaceMorpher::SetInterpolationMethod(InterpolationMethod _method)
{
	method = _method;
}

float SurfaceMorpher::GetInterpolationProgress()
{
	if (startTime == 0)
		startTime = clock();

	float time = (clock() - startTime) / (float)CLOCKS_PER_SEC;
	time = std::min(duration, time);
	t = time / duration;
	return t;
}

void SurfaceMorpher::Reset()
{
	startTime = 0;
	t = 0.0f;
	currentMeshIndex = 0;
}

void SurfaceMorpher::SpeedUp()
{
	duration -= 0.5f;
	duration = std::max(0.5f, duration);
}

void SurfaceMorpher::SpeedDown()
{
	duration += 0.5f;
}

void SurfaceMorpher::SpeedReset()
{
	duration = 2.0f;
}
