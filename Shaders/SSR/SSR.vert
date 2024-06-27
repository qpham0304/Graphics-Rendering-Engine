#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 mvp;
uniform mat4 matrix;

out vec2 uv;
out vec4 clipSpace;

void main() {
    uv = aTexCoords;
    vec4 worldPos = matrix * vec4(aPos, 1.0);
    clipSpace = mvp * worldPos;
    gl_Position = vec4(aPos, 1.0);
}