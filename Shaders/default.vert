#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 vertexUV;
layout (location = 3) in vec3 aNormal;
// layout (location = 4) in vec3 tangent;
// layout (location = 5) in vec3 bitangent;
layout (location = 6) in ivec4 boneIds;
layout (location = 7) in vec4 weights;

out vec3 color;
out vec2 uv;
out vec3 normal;
out vec3 updatedPos;
out vec4 fragPosLight;

uniform mat4 mvp;
uniform mat4 matrix;
uniform mat4 lightProjection;
uniform mat4 normalMap;

void main()
{
	color = aColor;
	uv = vertexUV;
	normal = mat3(transpose(inverse(matrix))) * aNormal;
	updatedPos = (matrix * vec4(pos,1)).xyz;
	fragPosLight = lightProjection * vec4(updatedPos, 1.0f);
	gl_Position = mvp * vec4(updatedPos, 1);
}




