#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;

// Ouput data
out vec3 color;

void main(){

	// Output color = blue
	color = vec3(0.0f, 0.0f, 1.0f);
}