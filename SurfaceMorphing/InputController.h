#pragma once

#include "global.h"
#include <glm/gtc/matrix_transform.hpp>
#include "SurfaceMorpher.h"

class InputController {
public:
	static void InitCallback(GLFWwindow* window);
	static void ComputeMatricesFromInputs(GLFWwindow* window);
	static mat4 GetMVP();
	static vec3 convertToWorldCoordinate(double mouseX, double mouseY);
private:
};
