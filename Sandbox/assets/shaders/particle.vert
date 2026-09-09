#version 460

#extension GL_GOOGLE_include_directive : require
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#include "common/buffers.glsl"

struct Container {
    uint64_t lifetimeRef;
    uint64_t positionsRef;
    uint64_t scalesRef;
    uint64_t velocitiesRef;
    uint64_t colorsRef;
};

layout(buffer_reference, scalar) buffer ContainerBuffers { Container containers[]; };
layout(buffer_reference, scalar) buffer Lifetime { float lifetime[]; };
layout(buffer_reference, scalar) buffer Positions { vec3 positions[]; };
layout(buffer_reference, scalar) buffer Scales { vec3 scales[]; };
layout(buffer_reference, scalar) buffer Velocities { vec3 velocities[]; };
layout(buffer_reference, scalar) buffer Colors { vec4 colors[]; };

layout(push_constant) uniform ParticleContainerRefs {
    ContainerBuffers containersRef;
    uint containerIdx; // The index for THIS draw call
} pc;


layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 invNormal;
    mat4 view;
    mat4 prevViewProj;
    mat4 proj;
    vec4 cameraPos;
    mat4 invView;
    mat4 invProj;
    float width;
    float height;
} ubo;

layout(set = 0, binding = 1) uniform EmitterUBO {
    vec3 position;   // placeholder only remove
} emitter;

void main() {
    const vec2 localOffsets[6] = vec2[](
        vec2(-0.5, -0.5), vec2(0.5, -0.5), vec2(0.5, 0.5),
        vec2(-0.5, -0.5), vec2(0.5, 0.5),  vec2(-0.5, 0.5)
    );
    
    //gl_VertexIndex defined from vkCmdDraw
    uint particleID = gl_VertexIndex / 6;
    uint vertexInQuad = gl_VertexIndex % 6;

    Container c = pc.containersRef.containers[pc.containerIdx];
    Positions posBuffer = Positions(c.positionsRef);
    vec3 particlePos = posBuffer.positions[particleID];

    vec2 offset = localOffsets[vertexInQuad] * 0.1;
    vec4 worldPos = vec4(particlePos + vec3(offset, 0.0), 1.0);

    gl_Position = ubo.proj * ubo.view * worldPos;
    
    // vec2 uv = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    // gl_Position = vec4(uv * 2.0f - 1.0f, 0.0f, 1.0f);
}