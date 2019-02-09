#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

// The model/view/projection matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// These outputs will be available in the fragment shader as inputs
out vec4 fragPos;
out vec4 fragNormal;
out vec2 fragTexCoords;

void main()
{
	mat4 MVP = projection * view * model;

	fragPos = MVP * vec4(pos, 1.0f);
	fragNormal = MVP * vec4(normal, 1.0f);
	fragTexCoords = texCoords;

	gl_Position = MVP * vec4(pos, 1.0f);
}