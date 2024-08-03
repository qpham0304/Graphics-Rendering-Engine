#version 460 core

out float FragColor;

in vec2 uv;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;
uniform vec3 samples[64];
uniform vec2 noiseScale;

int kernelSize = 64;
float radius = 0.5;
float bias = 0.15;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    // get input for SSAO algorithm
    vec3 fragPos = (view * vec4((texture(gPosition, uv)).xyz, 1.0)).xyz; // convert to view space;
    vec3 normal = normalize(texture(gNormal, uv).rgb);
    vec3 randomVec = normalize(texture(texNoise, uv * noiseScale).xyz);
    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 samplePos = TBN * samples[i]; // from tangent to view-space
        samplePos = fragPos + samplePos * radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = projection * vec4(samplePos, 1.0); // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // gPosition is calculated in world space so convert it to view space
        vec4 gPosView = view * vec4(texture(gPosition, offset.xy).xyz, 1.0f); 
        float sampleDepth = gPosView.z;
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
    FragColor = occlusion;
}
