#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexcoord;

uniform mat4 matrix;
uniform mat4 mvp;

void main()
{
	gl_Position = mvp * matrix * vec4(aPos, 1.0f);
}