#include "global.h"
#include "MeshLoader.h"
#include "ShaderLoader.h"
#include "TextureLoader.h"
#include "InputController.h"
#include "SurfaceMorpher.h"

int main()
{
	// Input Filename
	cout << "Welcome to Mesh Interpolation. Please input the name of obj files('.obj' not needed):" << endl;
	cout << "Pattern 1: '1,2,3,4' for '1.obj 2.obj 3.obj 4.obj'." << endl;
	cout << "Pattern 2: '1:4' for '1.obj 2.obj 3.obj 4.obj' (number only)." << endl;
	cout << "Usage: 1 : Linear Interpolation , 2 : Transform-based Interpolation. 3 : Surface-based Interpolation. " << endl;
	cout << "A: Speed Down, D: Speed Up, S: Speed Reset. Q: Line Mode. E: Fill Mode. R : Reset. W : Pause." << endl;
	string filenames;
	cin >> filenames;
	auto parsedFilenames = MeshLoader::ParseFilename(filenames);

	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mesh Interpolation", NULL, NULL);
	glfwMakeContextCurrent(window);
	InputController::InitCallback(window);

	// Init GLEW
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPointSize(8.0f);

	// Load Mesh
	vector<Mesh*> meshList;
	for each (auto filename in parsedFilenames)
	{
		Mesh* mesh = MeshLoader::LoadObj2D(filename);
		if (mesh->GetVertices()->size() > 0)// a valid mesh
		{
			meshList.push_back(mesh);
		}
	}

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
		int index = SurfaceMorpher::GetCurrentIndex();
		if (SurfaceMorpher::GetInterpolationProgress() == 1 && index < meshList.size() - 2)
		{//interpolate next mesh
			index++;
			SurfaceMorpher::PrepareNextInterpolation();
		}
		auto vertices = SurfaceMorpher::GetInterpolation(meshList[index], meshList[index + 1]);
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