#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 vertexUV;
layout (location = 3) in vec3 aNormal;

out vec2 uv;
out vec4 color;

uniform mat4 matrix;
uniform mat4 mvp;

void main()
{
    uv = vertexUV;
    color = vec4(aColor, 1.0f);
    gl_Position = mvp * matrix * vec4(pos, 1.0f);
}