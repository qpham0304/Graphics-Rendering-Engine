#version 460 core
layout (location = 0) out vec4 gAlbedo;
layout (location = 1) out vec4 gNormalMask;
layout (location = 2) out vec4 gMetalRoughness;
layout (location = 3) out vec4 gEmissive;
layout (location = 4) out vec4 gDUV;

in vec2 uv;
in vec3 fragPos;
in vec3 normal;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;
uniform sampler2D emissiveMap;
uniform sampler2D duvMap;

uniform float reflectiveMap = 0.0f; // 0.0 for non reflective and 1.0 for reflective

void main() {
    gAlbedo.rgb = texture(albedoMap, uv).rgb;
    gAlbedo.a = texture(aoMap, uv).r;
    
    gNormalMask.rgb = normalize(normal);
    gNormalMask.a = reflectiveMap;
    
    // gMetalRoughness.r = texture(roughnessMap, uv).r;
    gMetalRoughness.g = texture(roughnessMap, uv).g;
    gMetalRoughness.b = texture(metallicMap, uv).b;
    // gMetalRoughness.a = texture(roughnessMap, uv).a;
    
    gEmissive = texture(emissiveMap, uv);
    gDUV = texture(duvMap, uv);
}  