#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 vertexUV;
layout (location = 3) in vec3 aNormal;

out vec3 color;
out vec2 uv;
out vec3 normal;
out vec3 curFragPos;

uniform mat4 matrix;
uniform mat4 mvp;
uniform mat3 normalMatrix;

void main()
{
    color = aColor;
    uv = vertexUV;
    // transpose inverse is expensive but less code on the CPU side so we keep it for now
    //normal = mat3(transpose(inverse(matrix))) * aNormal; 
    normal = normalMatrix * aNormal;
    curFragPos = vec3(matrix * vec4(aPos, 1.0f));
    gl_Position = mvp * matrix * vec4(aPos, 1.0f);
}