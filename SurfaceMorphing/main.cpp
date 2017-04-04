#include "global.h"
#include "MeshLoader.h"
#include "ShaderLoader.h"
#include "TextureLoader.h"
#include "InputController.h"
#include "SurfaceMorpher.h"

int main()
{
	// Input Filename
	cout << "Welcome to Surface Morphing. Please input the name of two obj files seperated with space('.obj' not needed):" << endl;
	string filename1, filename2;
	cin >> filename1 >> filename2;
	filename1.append(".obj");
	filename2.append(".obj");

	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mesh Deformation", NULL, NULL);
	glfwMakeContextCurrent(window);
	InputController::InitCallback(window);

	// Init GLEW
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPointSize(8.0f);

	// Load Mesh
	MeshLoader::LoadObj2D(filename1);
	auto vertices1 = MeshLoader::GetSequencedVertices()[0];
	MeshLoader::LoadObj2D(filename2);
	auto vertices2 = MeshLoader::GetSequencedVertices()[0];
	//auto uvs = meshLoader->GetUVs();

	// Load Shader
	GLuint programID = ShaderLoader::LoadShader("vertex.shader", "fragment.shader");
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Load Texture
	//GLuint TextureID = TextureLoader::LoadDDS("Rabbit.dds");

	// Load into a VBO
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

	/*GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), &uvs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);*/

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while(!glfwWindowShouldClose(window)) 
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(programID);

		// Set Camera
		InputController::ComputeMatricesFromInputs(window);
		mat4 MVP = InputController::GetMVP();
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// Draw the mesh
		auto vertices = SurfaceMorpher::GetLinearInterpolation(vertices1, vertices2);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup
	glDeleteBuffers(1, &vertexbuffer);
	//glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	//glDeleteTextures(1, &TextureID);
	glfwTerminate();
	return 0;
}