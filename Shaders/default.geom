#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 color;
out vec2 uv;
out vec3 normal;
out vec3 updatedPos;
out vec4 fragPosLight;

uniform mat4 mvp;
uniform float explodeRadius;

in VS_OUT {
    vec3 color;
    vec2 uv;
    vec3 normal;
    vec3 updatedPos;
    vec4 fragPosLight;
    vec4 totalPos;
    mat4 model;
} frag_in[];

in GEOM_OUT {
    vec3 color;
    vec2 uv;
    vec3 normal;
    vec3 updatedPos;
    vec4 fragPosLight;
} geom_out[];

vec4 explodeFunction() {
    vec3 vector0 = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position);
    vec3 vector1 = vec3(gl_in[2].gl_Position - gl_in[1].gl_Position);
    vec4 surfaceNormal = vec4(normalize(cross(vector0, vector1)), 0.0f);
    return surfaceNormal * explodeRadius;
}

void main()
{


	gl_Position = mvp * (vec4(frag_in[0].updatedPos, 1.0f) + explodeFunction());
    color = frag_in[0].color;
    uv = frag_in[0].uv;
    normal = frag_in[0].normal;
    updatedPos = frag_in[0].updatedPos;
    fragPosLight = frag_in[0].fragPosLight;
    EmitVertex();

    gl_Position = mvp * (vec4(frag_in[1].updatedPos, 1.0f) + explodeFunction());
    color = frag_in[1].color;
    uv = frag_in[1].uv;
    normal = frag_in[1].normal;
    updatedPos = frag_in[1].updatedPos;
    fragPosLight = frag_in[1].fragPosLight;
    EmitVertex();

    gl_Position = mvp * (vec4(frag_in[2].updatedPos, 1.0f) + explodeFunction());
    color = frag_in[2].color;
    uv = frag_in[2].uv;
    normal = frag_in[2].normal;
    updatedPos = frag_in[2].updatedPos;
    fragPosLight = frag_in[2].fragPosLight;
    EmitVertex();

    EndPrimitive();
}

//in DATA
//{
//    vec3 normal;
//    vec3 color;
//    vec2 uv;
//    mat4 projection;
//} data_in[];