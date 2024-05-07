#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 vertexUV;
layout (location = 3) in vec3 aNormal;
// layout (location = 4) in vec3 tangent;
// layout (location = 5) in vec3 bitangent;
layout (location = 6) in ivec4 boneIds;
layout (location = 7) in vec4 weights;

out VS_OUT {
    vec3 color;
    vec2 uv;
    vec3 normal;
    vec3 updatedPos;
    vec4 fragPosLight;
    vec4 totalPos;
    mat4 model;
} vert_out;


uniform mat4 matrix;
uniform mat3 normalMatrix;
uniform mat4 mvp;
uniform mat4 lightMVP;
uniform mat4 normalMap;
uniform bool hasAnimation = true;


const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];


void main()
{
	vert_out.color = aColor;
	vert_out.uv = vertexUV;
	vert_out.normal = normalize(normalMatrix * aNormal);

	vec4 totalPosition = vec4(0.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1) 
            continue;
        if(boneIds[i] >= MAX_BONES) 
        {
            totalPosition = vec4(pos,1.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(pos,1.0f);
        totalPosition += localPosition * weights[i];
        vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * vert_out.normal;
    }

    
    int condition = int(hasAnimation);
    totalPosition = condition * totalPosition + (1 - condition) * vec4(pos, 1.0f);

    vert_out.totalPos = totalPosition;
	vert_out.updatedPos = (matrix * totalPosition).xyz;
	vert_out.fragPosLight = lightMVP * vec4(vert_out.updatedPos, 1.0f);
    vert_out.model = matrix;
    gl_Position = mvp * vec4(vert_out.updatedPos, 1.0f);
}




