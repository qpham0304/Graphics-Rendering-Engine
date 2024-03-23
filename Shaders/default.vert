#version 330 core

layout (location = 0) in vec3 pos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in vec3 aNormal;

out vec3 color;
out vec2 uv;
out vec3 normal;
out vec3 updatedPos;

uniform mat4 mvp;
uniform mat4 matrix;

void main()
{
	color = aColor;
	uv = vertexUV;
	normal = aNormal;
	updatedPos = (matrix * vec4(pos,1)).xyz;
	gl_Position = mvp * vec4(updatedPos, 1);
}




