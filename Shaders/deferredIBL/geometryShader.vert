#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec2 aNormal2;
layout (location = 4) in vec2 tangent;
layout (location = 5) in vec2 bitangent;
layout (location = 6) in vec2 boneIDs;
layout (location = 7) in vec2 weights;

out vec2 uv;
out vec3 normal;

uniform bool invertedNormals = false;
uniform bool invertedTexCoords = false;
uniform mat4 modelViewNormal;
uniform mat4 mvp;

void main() {
    uv = aTexCoords;
    invertedTexCoords ? uv.y *= -1 : uv.y; // TODO: only use this to flip texture horizontally
    normal = mat3(modelViewNormal) * (invertedNormals ? -aNormal : aNormal);
    gl_Position = mvp * vec4(aPos, 1.0);
}