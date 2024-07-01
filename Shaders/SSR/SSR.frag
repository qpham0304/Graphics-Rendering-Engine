#version 460 core
struct RayMarchOutput {
    bool hit;
    vec2 uv;
};

uniform sampler2D gPosition;
uniform sampler2D gNormal;      //scene normal
uniform sampler2D gAlbedoSpec;  //scene color
uniform sampler2D gExtraComponents; // shaded lighting color
uniform sampler2D ColorBuffer; 

uniform mat4 projection;
uniform mat4 view;
uniform mat4 invView;
uniform mat4 invProjection;
uniform vec3 camPos;
uniform int width;
uniform int height;

in vec2 uv;
in vec4 clipSpace;

out vec4 FragColor;

const float step = 0.1;
const float minRayStep = 0.1;   // stepsize
const float maxSteps = 30;      // num steps
const int numBinarySearchSteps = 5;
const float fallOffExponent = 3.0;

#define Scale vec3(.8, .8, .8)
#define K 19.19

vec3 BinarySearch(inout vec3 dir, inout vec3 position, inout float dDepth); 
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 hash(vec3 a);
vec4 RayMarch(vec3 dir, inout vec3 hitCoord, out float dDepth);

vec3 reconstructViewPosition(vec2 uv, float depth)
{
    vec4 clipSpacePosition = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 viewSpacePosition = invProjection * clipSpacePosition;
    return viewSpacePosition.xyz / viewSpacePosition.w;
}

vec3 reflectVector(vec3 I, vec3 N)
{
    return I - 2.0 * dot(I, N) * N;
}

float computeDepth(vec3 viewPos)
{
    vec4 clipSpacePos = projection * vec4(viewPos, 1.0);
    return (clipSpacePos.z / clipSpacePos.w) * 0.5 + 0.5; // Depth in the range [0, 1]
}

struct RayTraceOutput {
    bool Hit;
    vec2 UV;
};

float march() {
    
    return 0.0;
}

void main() {
    vec4 worldSpaceNormal = texture(gNormal, uv);
    vec3 viewSpaceNormal = normalize(mat3(view) * worldSpaceNormal.xyz);
    float Metallic = normalize(worldSpaceNormal.a); //reflection mask in alpha channel
    if(Metallic == 1.0){
        vec4 worldSpacePosition = texture(gPosition, uv);
        vec3 viewSpacePosition = vec3(view * vec4(texture(gPosition, uv).xyz, 1.0));
        vec3 albedo = texture(gAlbedoSpec, uv).rgb;
        float spec = texture(ColorBuffer, uv).w;
        vec3 jitt = mix(vec3(0.0), vec3(hash(viewSpacePosition)), spec);

        vec3 F0 = vec3(0.04); 
        F0 = mix(F0, albedo, Metallic);
        vec3 Fresnel = fresnelSchlick(max(dot(normalize(viewSpaceNormal.xyz), normalize(viewSpacePosition)), 0.0), F0);
        vec3 reflected = normalize(reflect(normalize(viewSpacePosition), normalize(viewSpaceNormal.xyz))); // Reflection vector
        if(reflected.z > 0){
            FragColor = vec4(0,0,0,1);
            return;
        }
        float dDepth;
    
        vec3 viewPosition = viewSpacePosition;
        vec4 coords = RayMarch(reflected * max(minRayStep, -viewSpacePosition.z), viewPosition, dDepth);
        vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5) - coords.xy));
        float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
        float ReflectionMultiplier = pow(Metallic, fallOffExponent) * screenEdgefactor * -reflected.z;
    
        vec3 SSR = texture(gExtraComponents, coords.xy).rgb * clamp(ReflectionMultiplier, 0.0, 0.9) * Fresnel;  
        FragColor = vec4(SSR, 1.0);
    }
    else {
        FragColor = texture(gExtraComponents, uv);
        return;
    }

}

vec3 BinarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth) {
    float depth;

    vec4 projectedCoord;
    for(int i = 0; i < numBinarySearchSteps; i++) {
        projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
        depth = textureLod(gPosition, projectedCoord.xy, 2).z;
        dDepth = hitCoord.z - depth;

        dir *= 0.5;
        if(dDepth > 0.0)
            hitCoord += dir;
        else
            hitCoord -= dir;    
    }

    projectedCoord = projection * vec4(hitCoord, 1.0);
    projectedCoord.xy /= projectedCoord.w;
    projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
    return vec3(projectedCoord.xy, depth);
}

vec4 RayMarch(vec3 dir, inout vec3 position, out float dDepth) {
    dir *= step;
 
    float depth;
    vec4 projectedCoord;
 
    for(int i = 0; i < maxSteps; i++) {
        position += dir;
        projectedCoord = projection * vec4(position, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
        depth = texture(gPosition, projectedCoord.xy).z;
        if(depth > 1000.0)
            continue;
 
        dDepth = position.z - depth;

        // if((dir.z - dDepth) < 1.2) {
        //     if(dDepth <= 0.0) {   
        //         vec4 Result = vec4(BinarySearch(dir, position, dDepth), 1.0);
        //         return Result;
        //     }
        // }
    }
    return vec4(projectedCoord.xy, depth, 0.0);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 hash(vec3 a) {
    a = fract(a * Scale);
    a += dot(a, a.yxz + K);
    return fract((a.xxy + a.yxx)*a.zyx);
}

/*
#version 330 core
// uniform sampler2D gColor;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gExtraComponents;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 invView;
uniform mat4 invProjection;
uniform vec3 camPos;
uniform int width;
uniform int height;

in vec2 uv;
out vec4 FragColor;
// Consts should help improve performance
const float rayStep = 0.25;
const float minRayStep = 0.1;
const float maxSteps = 20;
const float searchDist = 5;
const float searchDistInv = 0.2;
const int numBinarySearchSteps = 5;
const float maxDDepth = 1.0;
const float maxDDepthInv = 1.0;


const float reflectionSpecularFalloffExponent = 3.0;

vec3 BinarySearch(vec3 dir, inout vec3 hitCoord, out float dDepth) {
    float depth;
    for(int i = 0; i < numBinarySearchSteps; i++) {
        vec4 projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

        depth = texture2D(gPosition, projectedCoord.xy).z;
        dDepth = hitCoord.z - depth;

        if(dDepth > 0.0)
            hitCoord += dir;

        dir *= 0.5;
        hitCoord -= dir;    
    }
    vec4 projectedCoord = projection * vec4(hitCoord, 1.0); 
    projectedCoord.xy /= projectedCoord.w;
    projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
    return vec3(projectedCoord.xy, depth);
}


vec4 RayCast(vec3 dir, inout vec3 hitCoord, out float dDepth) {
    dir *= rayStep;
    float depth;

    vec4 hitCoordsWorld = invView * vec4(hitCoord, 1.0);

    for(int i = 0; i < maxSteps; i++) {
        hitCoord += dir;
        vec4 projectedCoord = projection * view * hitCoordsWorld;
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
        
        depth = texture2D(gPosition, projectedCoord.xy).z;
        dDepth = hitCoord.z - depth;

        if(dDepth < 0.0)
            return vec4(BinarySearch(dir, hitCoord, dDepth), 1.0);
    }
    return vec4(0.0, 0.0, 0.0, 0.0);
}


void main() {
    float specular = texture2D(gNormal, uv).a;
    if(specular == 0.0) {
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
        return;
    }

    // vec4 worldSpaceNormal = texture(gNormal, uv);
    // vec3 viewSpaceNormal = normalize(mat3(view) * worldSpaceNormal.xyz);
    // vec3 viewSpacePosition = vec3(view * vec4(texture(gPosition, uv).xyz, 1.0));
    vec3 viewNormal = texture(gNormal, uv).xyz;
    vec3 viewPos = texture(gPosition, uv).xyz;
    vec3 reflected = normalize(reflect(normalize(viewPos), normalize(viewNormal)));// Reflection vector
    // Ray cast
    vec3 hitPos = viewPos;
    float dDepth;

    vec4 coords = RayCast(reflected * max(minRayStep, -viewPos.z), hitPos, dDepth);
    vec2 dCoords = abs(vec2(0.5, 0.5) - coords.xy);

    float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
    // Get color
    FragColor = vec4(texture2D(gExtraComponents, coords.xy).rgb,
        pow(specular, reflectionSpecularFalloffExponent) *
        screenEdgefactor * clamp(-reflected.z, 0.0, 1.0) *
        clamp((searchDist - length(viewPos - hitPos)) * searchDistInv, 0.0, 1.0) * coords.w);
}
*/